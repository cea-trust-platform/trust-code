/****************************************************************************
* Copyright (c) 2022, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        SETS.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Schemas_Temps
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Discretisation_base.h>
#include <Operateur_Diff_base.h>
#include <Schema_Temps_base.h>
#include <Masse_Multiphase.h>
#include <Neumann_val_ext.h>
#include <MD_Vector_tools.h>
#include <Assembleur_base.h>
#include <TRUSTTab_parts.h>
#include <QDM_Multiphase.h>
#include <Pb_Multiphase.h>
#include <Pb_Conduction.h>
#include <MD_Vector_std.h>
#include <Matrix_tools.h>
#include <Matrice_Bloc.h>
#include <Statistiques.h>
#include <TRUSTTrav.h>
#include <Dirichlet.h>
#include <Zone_VF.h>
#include <EChaine.h>
#include <Lapack.h>
#include <Debog.h>
#include <SETS.h>

#ifndef NDEBUG
template class std::map<std::string, matrices_t>;
template class std::map<std::string, Matrice_Morse>;
#endif

Implemente_instanciable_sans_constructeur(SETS,"SETS",Simpler);

Implemente_instanciable_sans_constructeur(ICE,"ICE",SETS);

SETS::SETS()
{
  sets_ = 1;
}

ICE::ICE()
{
  sets_ = 0;
}

Sortie& SETS::printOn(Sortie& os ) const
{
  return Simpler::printOn(os);
}

Entree& SETS::readOn(Entree& is )
{
  /* valeurs par defaut des criteres de convergence */
  crit_conv = { { "alpha", 1e-2 }, { "temperature", 1e-1 }, { "vitesse", 1e-2 }, { "pression", 100 }, {"k", 1e-2}, {"tau", 1e-2}, {"omega", 1e-2}, {"k_WIT", 1e-2} };
  first_call_ = 1;
  Simpler::readOn(is);
  return is;
}

Entree& SETS::lire(const Motcle& mot, Entree& is)
{
  if (mot == Motcle("criteres_convergence"))
    {
      Nom nom;
      is >> nom;
      if (nom != "{") Cerr << "SETS::lire() : { expected instead of " << nom << finl, Process::exit();
      for (is >> nom; nom != "}"; is >> nom)
        {
          double val;
          is >> val;
          crit_conv[nom.getString()] = val;
        }
    }
  else if (mot == "iter_min") is >> iter_min_;
  else if (mot == "iter_max") is >> iter_max_;
  else return Simpler::lire(mot, is); //la classe mere connait-elle ce mot cle?
  return is;
}


Sortie& ICE::printOn(Sortie& os ) const
{
  return SETS::printOn(os);
}

Entree& ICE::readOn(Entree& is )
{
  SETS::readOn(is);
  return is;
}

static inline double corriger_incr_alpha(const DoubleTab& alpha, DoubleTab& incr)
{
  int i, n, N = alpha.line_size();
  double a_sum, corr_max = 0;
  DoubleTrav n_a(N);
  for (i = 0; i < alpha.dimension_tot(0); i++)
    {
      for (n = 0; n < N; n++) n_a(n) = alpha(i, n) + incr(i, n);
      for (a_sum = 0, n = 0; n < N; n++) n_a(n) = std::max(n_a(n), 0.), a_sum += n_a(n);
      if (a_sum) for (n = 0; n < N; n++) n_a(n) /= a_sum;
      else for (n = 0; n < N; n++) n_a(n) = 1. / N;
      for (n = 0; n < N; n++) corr_max = std::max(corr_max, std::fabs(alpha(i, n) + incr(i, n) - n_a(n))), incr(i, n) = n_a(n) - alpha(i, n);
    }
  return Process::mp_max(corr_max);
}

double SETS::unknown_positivation(const DoubleTab& uk, DoubleTab& incr)
{
  double corr_max = 0;
  int N = uk.line_size();
  for (int i=0 ; i<uk.dimension_tot(0) ; i++) for (int n = 0 ; n<N ; n++) if (uk(i, n) + incr(i, n) < 0)
        {
          if (- uk(i, n) - incr(i, n) > corr_max) corr_max =  std::abs(uk(i, n) + incr(i, n));
          incr(i, n) = -uk(i, n);
        }
  return Process::mp_max(corr_max);
}

bool SETS::iterer_eqn(Equation_base& eqn, const DoubleTab& inut, DoubleTab& current, double dt, int numero_iteration, int& ok)
{
  int cv;
  /* on ne traite que Pb_Multiphase ou Pb_conduction */
  if (!sub_type(Pb_Multiphase, eqn.probleme()) && !sub_type(Pb_Conduction, eqn.probleme()))
    Process::exit(que_suis_je() + " cannot be applied to the problem " + eqn.probleme().le_nom() + " of type " + eqn.probleme().que_suis_je() + "!");

  /* equations non resolues directement : Masse_Multiphase (toujours), Energie_Multiphase (en ICE), Convection_Diffusion_std i.e. quantites turbulentes (en ICE) */
  if (sub_type(Masse_Multiphase, eqn) || (!sets_ && sub_type(Energie_Multiphase, eqn))|| (!sets_ && sub_type(Convection_Diffusion_std, eqn)))
    return true;

  /* QDM_Multiphase: resolue par iterer_NS */
  if (sub_type(QDM_Multiphase, eqn))
    {
      Matrice_Morse matrix_unused;
      DoubleTrav secmem_unused;
      iterer_NS(eqn, current, ref_cast(QDM_Multiphase, eqn).pression().valeurs(), dt, matrix_unused, 0, secmem_unused, numero_iteration, cv, ok);
      return cv;
    }

  /* cas restant : equation thermique d'un Pb_Multi ou d'un Pb_conduction -> on regle semi_impl si necessaire, puis on resout */
  const std::string& nom_inco = eqn.inconnue().le_nom().getString(), nom_pb_inco = eqn.probleme().le_nom().getString() + "_" + nom_inco;
  tabs_t semi_impl; /* en ICE, les temperatures de tous les problemes sont explicites */
  const Operateur_Diff_base& op_diff = ref_cast(Operateur_Diff_base, eqn.operateur(0).l_op_base());
  if (!sets_) for (auto &&op_ext : op_diff.op_ext)
      semi_impl[nom_inco + (op_ext != &op_diff ? "_" + op_ext->equation().probleme().le_nom().getString() : "")] = op_ext->equation().inconnue().passe();

  if (!mat_pred.count(nom_pb_inco)) /* matrice : dimensionnement au premier passage */
    eqn.dimensionner_blocs({{ nom_inco, &mat_pred[nom_pb_inco]}}, semi_impl);

  /* assemblage et resolution */
  DoubleTrav secmem(current);
  SolveurSys& solv = get_and_set_parametre_implicite(eqn).solveur();
  eqn.assembler_blocs_avec_inertie({{ nom_inco, &mat_pred[nom_pb_inco] }}, secmem, semi_impl);
  mat_pred[nom_pb_inco].ajouter_multvect(current, secmem); //passage increment -> variable
  solv.valeur().reinit();
  solv.resoudre_systeme(mat_pred[nom_pb_inco], secmem, current);

  if (eqn.positive_unkown()==1)
    {
      DoubleTrav incr ;
      incr = current;
      incr -= eqn.inconnue()->valeurs();
      unknown_positivation(eqn.inconnue()->valeurs(), incr);
      incr += eqn.inconnue()->valeurs();
      current = incr;
    }

  /* mise a jour */
  eqn.probleme().mettre_a_jour(eqn.schema_temps().temps_courant());
  eqn.inconnue().futur() = current;
  return true;
}

//Entree Un ; Pn
//Sortie Un+1 = U***_k ; Pn+1 = P**_k
//n designe une etape temporelle

void SETS::iterer_NS(Equation_base& eqn,DoubleTab& current,DoubleTab& pression,
                     double dt,Matrice_Morse& matrice_unused,double seuil_resol,DoubleTrav& secmem_unused,int nb_ite,int& cv, int& ok)
{
  int i, j, &it = iteration;
  Pb_Multiphase& pb = *(Pb_Multiphase *) &ref_cast(Pb_Multiphase, eqn.probleme());
  QDM_Multiphase& eq_qdm = ref_cast(QDM_Multiphase, eqn);
  double t = eqn.schema_temps().temps_courant();

  std::vector<Equation_base *> eq_list; //ordres des equations
  for (i = 0; i < pb.nombre_d_equations(); i++) eq_list.push_back(&pb.equation(i));
  std::map<std::string, Equation_base *> eqs; //eqs[inconnue] = equation
  std::vector<std::string> noms; //ordre des inconnues : le meme que les equations, puis la pression
  for (i = 0; i < pb.nombre_d_equations(); i++) noms.push_back(eq_list[i]->inconnue().le_nom().getString()), eqs[noms[i]] = eq_list[i];
  noms.push_back("pression"); //pas d'equation associee a la pression!

  std::map<std::string, Champ_Inc_base *> inco; //tous les Champ_Inc
  for (auto &n_eq : eqs) inco[n_eq.first] = &n_eq.second->inconnue().valeur();
  inco["pression"] = &eq_qdm.pression().valeur();
  //initialisation du Newton avec les valeurs presentes (stockees dans passe() en implicite), dimensionnement de incr / sec
  pb.mettre_a_jour(t); //inconnues -> milieu -> champs conserves

  /* valeurs semi-implicites : inconnues (alpha, v, T, ..) et champs conserves (alpha_rho, alpha_rho_e, ..) */
  tabs_t semi_impl;
  for (auto &&n_eq : eqs) if (n_eq.second != &eq_qdm)
      {
        semi_impl[n_eq.first] = n_eq.second->inconnue().passe();
        semi_impl[n_eq.second->champ_conserve().le_nom().getString()] = n_eq.second->champ_conserve().passe();
        semi_impl[n_eq.second->champ_convecte().le_nom().getString()]  =n_eq.second->champ_convecte().passe();
      }
  //en SETS, on remplace la valeur passee de v par celle donnee par une etape de prediction
  if (sets_ && !first_call_)
    {
      DoubleTrav secmem(current);
      /* assemblage "implicite, vitesses seulement" */
      if (!mat_pred.count("vitesse")) eq_qdm.dimensionner_blocs({{"vitesse", &mat_pred["vitesse"] }}); //premier passage : dimensionnement
      eq_qdm.assembler_blocs_avec_inertie({{"vitesse", &mat_pred["vitesse"] }}, secmem);

      /* resolution et stockage de la vitesse pedite dans current */
      SolveurSys& solv_qdm = get_and_set_parametre_implicite(eqn).solveur();
      solv_qdm.valeur().reinit();
      mat_pred["vitesse"].ajouter_multvect(current, secmem); //passage increment -> variable pour faire plaisir aux solveurs iteratifs
      solv_qdm.resoudre_systeme(mat_pred["vitesse"], secmem, current);
      semi_impl["vitesse"] = current;
    }
  else semi_impl["vitesse"] = eq_qdm.inconnue().passe();
  eqn.solv_masse().corriger_solution(current, current, 0); //pour CoviMAC : vf -> ve

  first_call_ = 0;

  //premier passage : dimensionnement de mat_semi_impl, remplissage de p_degen_
  if (!mat_semi_impl.nb_lignes())
    {
      mat_semi_impl.dimensionner(eq_list.size(), inco.size());
      for (i = 0; i < (int) eq_list.size(); i++) for (j = 0; j < (int) inco.size(); j++)
          mat_semi_impl.get_bloc(i, j).typer("Matrice_Morse"), mats[noms[i]][noms[j]] = &ref_cast(Matrice_Morse, mat_semi_impl.get_bloc(i, j).valeur());
      for (auto &&n_eq : eqs) n_eq.second->dimensionner_blocs(mats[n_eq.first], semi_impl); //option semi-implicite

      /* si incompressible sans CLs de pression imposee, alors la pression est degeneree */
      p_degen = sub_type(Fluide_base, eq_qdm.milieu());
      for (i = 0; i < eq_qdm.zone_Cl_dis().nb_cond_lim(); i++)
        p_degen &= !sub_type(Neumann_val_ext, eq_qdm.zone_Cl_dis().les_conditions_limites(i).valeur());
    }

  /* Newton : assemblage de mat_semi_impl -> assemblage de la matrice en pression -> resolution -> substitution */
  tabs_t incr, sec; //increments / seconds membres associes
  for (auto &&n_i : inco) incr[n_i.first] = sec[n_i.first] = n_i.second->valeurs();

  if (!Process::me())
    {
      fprintf(stderr, "Increments for semi-implicit method :\n  it");
      for (auto &&n_i : inco) fprintf(stderr, " %11s", n_i.first.c_str());
      fprintf(stderr, "\n");
    }

  for (it = 0, cv = 0; it < iter_min_ || (!cv && it < iter_max_); it++)
    {
      /* remplissage par assembler_blocs */
      //equation d'energie en premier pour pouvoir utiliser q_pi dans d'autres equations
      eqs["temperature"]->assembler_blocs_avec_inertie(mats["temperature"], sec["temperature"], semi_impl);
      for (auto &n_e : eqs) if (n_e.first != "temperature") n_e.second->assembler_blocs_avec_inertie(mats[n_e.first], sec[n_e.first], semi_impl);

      /* expression des autres inconnues (x) en fonction de p : vitesse, puis temperature / pression */
      tabs_t b_p;
      std::vector<std::set<std::pair<std::string, int>>> ordre;
      if (eq_qdm.zone_dis().valeur().le_nom() == "CoviMAC") ordre.push_back({{ "vitesse", 1 }}); //si CoviMAC: on commence par ve
      ordre.push_back({{ "vitesse", 0 }}), ordre.push_back({}); //puis vf, puis toutes les autres inconnues simultanément
      for (auto &&nom : noms) if (nom != "vitesse" && nom != "pression") ordre.back().insert({{ nom, 0 }});
      eliminer(ordre, "pression", mats, sec, A_p, b_p);

      /* assemblage du systeme en pression */
      DoubleTrav secmem_pression;
      assembler("pression", {}, A_p, b_p, mats, sec, inco["alpha"]->valeurs(), matrice_pression, secmem_pression, p_degen);

      /* resolution : seulement si l'erreur en alpha (dans secmem_pression) depasse un seuil */
      if (mp_max_abs_vect(secmem_pression) > 1e-16)
        {
          matrice_pression.ajouter_multvect(inco["pression"]->valeurs(), secmem_pression); //passage increment -> variable pour faire plaisir aux solveurs iteratifs
          SolveurSys& solv_p = eq_qdm.solveur_pression();
          solv_p.valeur().reinit(), solv_p.valeur().set_return_on_error(1); /* pour eviter un exit() en cas d'echec */
          ok = (solv_p.resoudre_systeme(matrice_pression, secmem_pression, incr["pression"]) >= 0);
          if (!ok) break; //le solveur a echoue -> on sort
          incr["pression"] -= inco["pression"]->valeurs();
        }
      else incr["pression"] = 0;

      //increments des autres variables
      for (auto &&n_v : b_p)
        {
          incr[n_v.first] = n_v.second; //partie constante
          A_p[n_v.first].ajouter_multvect(incr["pression"], incr[n_v.first]); //dependance en les increments de pression
          incr[n_v.first].echange_espace_virtuel();
        }
      eqn.solv_masse().corriger_solution(incr["vitesse"], incr["vitesse"], 1); //pour CoviMAC : sert a corriger ve

      /* convergence? */
      cv = (corriger_incr_alpha(inco["alpha"]->valeurs(), incr["alpha"]) < crit_conv["alpha"]);
      for (i = 0 ; i < pb.nombre_d_equations(); i++) if (pb.equation(i).positive_unkown()==1)
          {
            std::string nom_inco = pb.equation(i).inconnue().le_nom().getString() ;
            cv &= (unknown_positivation(inco[nom_inco]->valeurs(), incr[nom_inco]) < crit_conv[nom_inco]);
          }
      for (auto && n_v : incr) if (crit_conv.count(n_v.first)) cv &= mp_max_abs_vect(n_v.second) < crit_conv.at(n_v.first);

      if (!Process::me())
#ifndef INT_is_64_
        fprintf(stderr, "%4d", it + 1);
#else
        fprintf(stderr, "%4ld", it + 1);
#endif
      for (auto &&n_v : incr)
        {
          double vm = mp_min_vect(n_v.second), vM = mp_max_vect(n_v.second), x = std::fabs(vM) > std::fabs(vm) ? vM : vm;
          if (!Process::me()) fprintf(stderr, " %11g", x);
        }
      if (!Process::me()) fprintf(stderr, "\n");

      /* mises a jour : inconnues -> milieu -> champs/conserves -> sources */
      for (auto && n_i : inco) n_i.second->valeurs() += incr[n_i.first];
      if (p_degen) inco["pression"]->valeurs() -= mp_min_vect(inco["pression"]->valeurs()); // On prend la pression minimale comme pression de reference afin d'avoir la meme pression de reference en sequentiel et parallele
      if (!(ok = eq_qdm.milieu().check_unknown_range())) break; //si on a depasse les bornes du milieu sur (p, T), on doit sortir
      eq_qdm.pression_pa().valeurs() = inco["pression"]->valeurs(); //en multiphase, la pression est deja en Pa
      pb.mettre_a_jour(t); //inconnues -> milieu -> champs conserves
    }

//ha ha ha
  if (ok && cv)
    {
      pb.mettre_a_jour(t); //inconnues -> milieu -> champs conserves
      for (auto && n_i : inco) n_i.second->futur() = n_i.second->valeurs();
      eq_qdm.pression().futur() = eq_qdm.pression().valeurs(), eq_qdm.pression_pa().futur() = eq_qdm.pression_pa().valeurs();
    }
  else
    {
      for (auto && n_i : inco)  n_i.second->futur() = n_i.second->valeurs() = n_i.second->passe();
      eq_qdm.pression_pa().futur() = eq_qdm.pression_pa().valeurs() = inco["pression"]->passe();
      ok = 0;
    }
  return;
}

void SETS::eliminer(const std::vector<std::set<std::pair<std::string, int>>> ordre, const std::string inco_p, const std::map<std::string, matrices_t>& mats,
                    const tabs_t& sec, std::map<std::string, Matrice_Morse>& A_p, tabs_t& b_p)
{
  int i, j, jb, k, l, lb, m, oMl, oMg, M, n, oNl, oNg, N, prems = !A_p.size(), infoo = 0; //si A_p est vide, premier passage -> on doit dimensionner
  const Matrice_Morse * A;
  char trans = 'T';

  /* decoupage des inconnues de sec en parties par DoubleTab_parts */
  std::map<std::pair<std::string, int>, int> offs; //offs[{inco, bloc}] : offset du bloc k de l'inconnue inco
  std::map<std::pair<std::string, int>, std::array<int, 2>> dims; //dims[{inco, bloc}] : dimension 0/ line_size() du bloc k de inco
  for (auto &&n_v : sec)
    {
      ConstDoubleTab_parts part(n_v.second);
      for (i = 0; i < part.size(); i++)
        {
          offs[ {n_v.first, i}] = offs.count({ n_v.first, i - 1 }) ? offs[ { n_v.first, i - 1 }] + dims[ { n_v.first, i - 1 }][0] * dims[ { n_v.first, i - 1 }][1] : 0;
          dims[ {n_v.first, i}] = { part[i].dimension_tot(0), part[i].line_size() };
        }
    }

  /* boucle sur les blocs */
  std::set<std::pair<std::string, int>> e_ib; //liste des { variable, bloc } deja elimines
  std::set<std::string> e_i; //liste des variables deja eliminees
  for (auto &&bloc : ordre)
    {
      std::set<std::string> i_bloc, dep; //variables du bloc, variables deja eliminees dont le bloc depend
      for (auto &&i_b : bloc) i_bloc.insert(i_b.first);
      for (auto &&i_b : bloc) for (auto && v_m : mats.at(i_b.first)) if (v_m.second->nb_colonnes() && v_m.first != inco_p && e_i.count(v_m.first) && !i_bloc.count(v_m.first))
            dep.insert(v_m.first);

      /* lignes du bloc a traiter */
      std::pair<std::string, int> i_b0 = *bloc.begin();//premiere inconnue du bloc
      IntTrav calc(dims[i_b0][0]); //calc[i] = 1 si on doit traiter l'item i
      for (A = mats.at(i_b0.first).at(i_b0.first), oMg = offs[i_b0], M = dims[i_b0][1], i = 0; i < calc.size_array(); i++)
        if (A->get_tab1()(oMg + M * i + 1) > A->get_tab1()(oMg + M * i)) calc(i) = 1; //on traite toutes les lignes dont la matrice est remplie

      if (prems) //premier passage -> dimensionnement des A_p
        {
          /* verification de la compatibilite des inconnues du bloc -> avec les MD_Vector renseignes dans sec */
          for (auto &&i_b : bloc) if (dims[i_b0][0] != dims[i_b][0])
              Cerr << "SETS::eliminer() : discretisation des inconnues" << i_b0.first << "/" << i_b0.second << " et " << i_b.first
                   << "/" << i_b.second << " incompatibles!" << finl, Process::exit();

          std::vector<std::set<int>> stencil(calc.size_array()); //stencil[i] -> stencil de l'item i (a demultiplier par le line_size() de chaque variable)
          for (auto &&i_b : bloc) for (auto &&v_m : mats.at(i_b.first)) if (v_m.second->nb_colonnes())
                {
                  oMg = offs[i_b], M = dims[i_b][1];
                  if (v_m.first == inco_p) //dependance directe en inco_p
                    {
                      for (i = 0; i < calc.size_array(); i++) if (calc[i])
                          for (j = v_m.second->get_tab1()(oMg + M * i) - 1; j < v_m.second->get_tab1()(oMg + M * (i + 1)) - 1; j++) //dependances de toutes les lignes
                            stencil[i].insert(v_m.second->get_tab2()(j) - 1);
                    }
                  else if (e_i.count(v_m.first) || i_bloc.count(v_m.first)) //dependance en une variable partiellement / totalement eliminee
                    {
                      A = A_p.count(v_m.first) ? &A_p.at(v_m.first) : NULL;
                      for (i = 0; i < calc.size_array(); i++) if (calc[i])
                          for (j = v_m.second->get_tab1()(oMg + M * i) - 1; j < v_m.second->get_tab1()(oMg + M * (i + 1)) - 1; j++)
                            {
                              for (jb = v_m.second->get_tab2()(j) - 1, k = 0; offs.count({ v_m.first, k + 1}) && jb >= offs.at({ v_m.first, k + 1 }); ) k++; //l : bloc de l'inconnue dont on depend
                              oNg = offs[ { v_m.first, k }], N = dims[ { v_m.first, k }][1], n = jb - oNg - N * i;
                              if (bloc.count({ v_m.first, k }) && n >= 0 && n < N) continue; //(variable, bloc) en cours d'elimination et coeff bloc-diagonal -> ok
                              else if (e_ib.count({ v_m.first, k }))  //(variable, bloc) elimine -> dependance en inco_p dans A_p
                                for (l = A->get_tab1()(jb) - 1; l < A->get_tab1()(jb + 1) - 1; l++)
                                  stencil[i].insert(A->get_tab2()(l) - 1);
                              else Cerr << "SETS::eliminer() : dependance ( " << i_b.first << "/" << i_b.second << " , "
                                          << v_m.first << "/" << k << " ) interdite!" << finl, Process::exit();
                            }
                    }
                  else Cerr << "SETS::eliminer() : dependance ( " << i_b.first << "/" << i_b.second << " , "
                              << v_m.first << " ) interdite!" << finl, Process::exit();
                }

          for (auto &&i_bl : bloc) //stencil par inconnue -> en demultipliant
            {
              IntTrav sten(0, 2);
              sten.set_smart_resize(1);
              for (oMg = offs[i_bl], M = dims[i_bl][1], i = 0; i < calc.size_array(); i++) if (calc[i])
                  for (m = 0; m < M; m++) for (auto &&col : stencil[i]) sten.append_line(oMg + M * i + m, col);
              Matrice_Morse mat2;
              Matrix_tools::allocate_morse_matrix(sec.at(i_bl.first).size_totale(), sec.at(inco_p).size_totale(), sten, mat2);
              A_p[i_bl.first].nb_colonnes() ? A_p[i_bl.first] += mat2 : A_p[i_bl.first] = mat2; //A_p peut deja etre partiellement creee
            }
        }

      std::vector<std::string> vbloc(i_bloc.begin(), i_bloc.end()), vdep(dep.begin(), dep.end()); //sous forme de liste
      int nv = vbloc.size(), nd = vdep.size(), nb = 0; //nombre de variables du bloc, de dependances, taille totale
      std::vector<int> size, off_l, off_g; //par (variable, bloc) : taille dans le systeme local, offset dans le systeme local, offset dans les systemes globaux
      for (auto && i_b : bloc) off_l.push_back(nb), size.push_back(dims[i_b][1]), off_g.push_back(offs[i_b]), nb += size.back();

      std::vector<const Matrice_Morse *> pmat(nv), dAp(nd); //par variable : dependance directe en inco_p, des variables du bloc / deja resolues
      std::vector<std::vector<const Matrice_Morse *>> mat(nv), dmat(nv); //matrices des variables du bloc, des dependances
      std::vector<const DoubleTab*> vsec(nv), dbp(nd); //seconds membres des variables, vecteurs b_p des variables / des dependances

      std::vector<Matrice_Morse *> Ap(nv); //resultats : d{inco} = A_p[inco].d{inco_p} + b_p[inco] pour les variables du bloc
      std::vector<DoubleTab*> bp(nv);

      for (i = 0; i < nv; i++)
        {
          Ap[i] = &A_p[vbloc[i]], vsec[i] = &sec.at(vbloc[i]);
          if (!b_p.count(vbloc[i])) b_p[vbloc[i]] = *vsec[i]; //creation des b_p
          bp[i] = &b_p[vbloc[i]];
          const matrices_t& line = mats.at(vbloc[i]);
          pmat[i] = line.count(inco_p) && line.at(inco_p)->nb_colonnes() ? line.at(inco_p) : NULL;
          for ( mat[i].resize(nv), j = 0; j < nv; j++)  mat[i][j] = line.count(vbloc[j]) && line.at(vbloc[j])->nb_colonnes() ? line.at(vbloc[j]) : NULL;
          for (dmat[i].resize(nd), j = 0; j < nd; j++) dmat[i][j] = line.count(vdep[j]) && line.at(vdep[j])->nb_colonnes() ? line.at(vdep[j]) : NULL;
        }
      for (i = 0; i < nd; i++) dbp[i] = &b_p.at(vdep[i]), dAp[i] = &A_p.at(vdep[i]); //b_p / A_p des dependances

      DoubleTrav D(nb, nb), S; //bloc diagonal, seconds membres
      S.set_smart_resize(1);
      IntTrav piv(nb);
      for (i = 0; i < calc.size_array(); i++) if (calc[i])
          {
            const int *deb = Ap[0]->get_tab2().addr() + Ap[0]->get_tab1()(off_g[0] + size[0] * i) - 1,
                       *fin = Ap[0]->get_tab2().addr() + Ap[0]->get_tab1()(off_g[0] + size[0] * i + 1) - 1,
                        ic = fin - deb, nc = ic + 1;
            S.resize(nc, nb), S = 0; //second membre : 5(i, .) -> dependance en la i-eme colonne du stencil des Ap du bloc, S(ic, .) -> partie constante
            //partie "second membre des equations"
            for (j = 0; j < nv; j++) for (M = size[j], oMg = off_g[j], oMl = off_l[j], m = 0; m < M; m++) S(ic, oMl + m) = vsec[j]->addr()[oMg + M * i + m];

            /* remplissage par les matrices du bloc : diagonale, second membre (si partie d'une variable deja eliminee) */
            for (D = 0, j = 0; j < nv; j++) for (M = size[j], oMg = off_g[j], oMl = off_l[j], k = 0; k < nv; k++) if (mat[j][k])
                  {
                    for (N = size[k], oNg = off_g[k], oNl = off_l[k], m = 0; m < M; m++) for (l = mat[j][k]->get_tab1()(oMg + M * i + m) - 1; l < mat[j][k]->get_tab1()(oMg + M * i + m + 1) - 1; l++)
                        if ((n = (jb = mat[j][k]->get_tab2()(l) - 1) - oNg - N * i) >= 0 && n < N) //on est dans le bloc diagonal
                          D(oMl + m, oNl + n) = mat[j][k]->get_coeff()(l);
                        else //dependance en un bloc deja elimine
                          {
                            double coeff = mat[j][k]->get_coeff()(l);
                            S(ic, oMl + m) -= coeff * bp[k]->addr()[jb];
                            for (lb = Ap[k]->get_tab1()(jb) - 1; lb < Ap[k]->get_tab1()(jb + 1) - 1; lb++)
                              S(std::lower_bound(deb, fin, Ap[k]->get_tab2()(lb)) - deb, oMl + m) -= coeff * Ap[k]->get_coeff()(lb);
                          }
                  }

            //partie "dependance directe en inco_p" -> dans S([0, ic[, .)
            for (j = 0; j < nv; j++) if (pmat[j]) for (M = size[j], oMg = off_g[j], oMl = off_l[j], m = 0; m < M; m++)
                  for (k = pmat[j]->get_tab1()(oMg + M * i + m) - 1; k < pmat[j]->get_tab1()(oMg + M * i + m + 1) - 1; k++)
                    S(std::lower_bound(deb, fin, pmat[j]->get_tab2()(k)) - deb, oMl + m) -= pmat[j]->get_coeff()(k);
            //partie "dependance en une variable hors bloc eliminee" -> b_p contribue a S(0, .), A_p contribue a S(1..nc, .)
            for (j = 0; j < nv; j++) for (k = 0; k < nd; k++) if (dmat[j][k]) for (M = size[j], oMg = off_g[j], oMl = off_l[j], m = 0; m < M; m++)
                    for (l = dmat[j][k]->get_tab1()(oMg + M * i + m) - 1; l < dmat[j][k]->get_tab1()(oMg + M * i + m + 1) - 1; l++)
                      {
                        double coeff = dmat[j][k]->get_coeff()(l);
                        jb = dmat[j][k]->get_tab2()(l) - 1, S(ic, oMl + m) -= coeff * dbp[k]->addr()[jb]; //partie "constante"
                        for (lb = dAp[k]->get_tab1()(jb) - 1; lb < dAp[k]->get_tab1()(jb + 1) - 1; lb++) //partie "dependance en inco_p"
                          S(std::lower_bound(deb, fin, dAp[k]->get_tab2()(lb)) - deb, oMl + m) -= coeff * dAp[k]->get_coeff()(lb);
                      }

            /* factorisation et resolution */
            DoubleTrav D_back = D;
            F77NAME(dgetrf)(&nb, &nb, &D(0, 0), &nb, &piv(0), &infoo);
            F77NAME(dgetrs)(&trans, &nb, &nc, &D(0, 0), &nb, &piv(0), &S(0, 0), &nb, &infoo);

            /* stockage : S(0, .) dans b_p, S(1..nc, .) dans A_p */
            for (j = 0; j < nv; j++) for (M = size[j], oMg = off_g[j], oMl = off_l[j], m = 0; m < M; m++)
                for (bp[j]->addr()[oMg + M * i + m] = S(ic, oMl + m), k = 0, l = Ap[j]->get_tab1()(oMg + M * i + m) - 1; k < ic; k++, l++)
                  Ap[j]->get_set_coeff()(l) = S(k, oMl + m);
          }

      for (auto &&i_b : bloc) e_ib.insert(i_b), e_i.insert(i_b.first);
    }
}

void SETS::assembler(const std::string inco_p, const std::vector<std::string> extra_eq, const std::map<std::string, Matrice_Morse>& A_p, const tabs_t& b_p,
                     const std::map<std::string, matrices_t>& mats, const tabs_t& sec, const DoubleTab& inco_a, Matrice_Morse& P, DoubleTab& secmem, int p_degen)
{
  int i, ib, j, na = inco_a.dimension_tot(0), m, M = inco_a.line_size();
  const int *deb, *fin; //bornes pour chercher des indices avec lower_bound()
  if (extra_eq.size()) Cerr << "SETS::assembler() : extra_eq pas encore code!" << finl, Process::exit();
  const Matrice_Morse& A = A_p.at("alpha");

  /* calc(i) = 1 si on doit remplir les lignes [N * i, (N + 1) * i[ de la matrice */
  ArrOfBit calc(na);
  if (inco_a.get_md_vector().non_nul()) MD_Vector_tools::get_sequential_items_flags(inco_a.get_md_vector(), calc);
  else calc = 1;

  if (!P.nb_colonnes()) //dimensionnement au premier passage
    {
      IntTrav stencil(0, 2);
      stencil.set_smart_resize(1);
      std::set<int> idx;
      for (i = 0; i < na; i++) if (calc[i]) //chaque
          {
            if (p_degen && !Process::me() && i == 0) for (j = 0; j < sec.at(inco_p).size_totale(); j++) idx.insert(j);
            else for (idx.clear(), m = 0, ib = M * i; m < M; m++, ib++) for (j = A.get_tab1()(ib) - 1; j < A.get_tab1()(ib + 1) - 1; j++)
                  idx.insert(A.get_tab2()(j) - 1);
            for (auto && col : idx) stencil.append_line(i, col);
          }
      Matrix_tools::allocate_morse_matrix(na, sec.at(inco_p).size_totale(), stencil, P);
    }

  /* matrice */
  for (P.get_set_coeff() = 0, i = 0; i < na; i++) if (calc[i])
      for (m = 0, ib = M * i, deb = P.get_tab2().addr() + P.get_tab1()(i) - 1, fin = P.get_tab2().addr() + P.get_tab1()(i + 1) - 1; m < M; m++, ib++)
        for (j = A.get_tab1()(ib) - 1; j < A.get_tab1()(ib + 1) - 1; j++)
          P.get_set_coeff()(std::lower_bound(deb, fin, A.get_tab2()(j)) - &P.get_tab2()(0)) += A.get_coeff()(j);
  double diag = P.get_coeff()(0);
  if (p_degen && !Process::me()) for (i = 0; i < P.get_tab1()(1) - 1; i++) P.get_set_coeff()(i) += diag; //de-degeneration de la matrice

  /* second membre : meme structure que inco_a, mais une seule composante */
  if (inco_a.get_md_vector().non_nul()) MD_Vector_tools::creer_tableau_distribue(inco_a.get_md_vector(), secmem, Array_base::NOCOPY_NOINIT);
  else secmem.resize(inco_a.dimension_tot(0));

  const DoubleTab& b = b_p.at("alpha");
  for (i = 0; i < na; i++) if (calc[i]) for (secmem(i) = 1, m = 0; m < M; m++)
        secmem(i) -= b(i, m) + inco_a(i, m);
}
