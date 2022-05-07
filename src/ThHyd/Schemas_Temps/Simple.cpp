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
// File:        Simple.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Simple.h>
#include <Navier_Stokes_std.h>
#include <EChaine.h>
#include <Debog.h>
#include <Matrice_Bloc.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Nulle.h>
#include <Matrix_tools.h>
#include <Assembleur_base.h>
#include <Statistiques.h>
#include <Schema_Temps_base.h>
#include <Schema_Euler_Implicite.h>
#include <Fluide_Dilatable_base.h>
#include <Probleme_base.h>
#include <MD_Vector_composite.h>
#include <MD_Vector_tools.h>
#include <TRUSTTab_parts.h>
#include <SETS.h>
#include <TRUSTTrav.h>
#include <Discretisation_base.h>

Implemente_instanciable_sans_constructeur(Simple,"Simple",Simpler_Base);

Simple::Simple()
{
  alpha_ = 1.;
  beta_ = 1.;
  with_d_rho_dt_ = 1;
  Ustar_old.resize(0);
}

Sortie& Simple::printOn(Sortie& os ) const
{
  return Simpler_Base::printOn(os);
}

Entree& Simple::readOn(Entree& is )
{
  return Simpler_Base::readOn(is);
}

Entree& Simple::lire(const Motcle& motlu,Entree& is)
{
  Motcles les_mots(1);
  {
    les_mots[0] = "relax_pression";
  }

  int rang=les_mots.search(motlu);
  switch(rang)
    {
    case 0:
      {
        is >> beta_;
        break;
      }

    default :
      {
        Cerr << "Keyword : " << motlu << " is not undertood in " << que_suis_je() << finl;
        exit();
      }
    }

  return is;
}

void diviser_par_rho_np1_face(Equation_base& eqn,DoubleTab& tab)
{
  Fluide_Dilatable_base& fluide_dil = ref_cast(Fluide_Dilatable_base,eqn.milieu());
  const DoubleTab& rho = fluide_dil.rho_face_np1();
  int nbdim = tab.nb_dim();
  int taille_0_tot = tab.dimension_tot(0);

  if (nbdim==1)
    for (int i=0; i<taille_0_tot; i++)
      tab(i) /= rho(i);
  else
    {
      int dim = tab.dimension(1);
      for (int i=0; i<taille_0_tot; i++)
        for (int j=0; j<dim; j++)
          tab(i,j) /= rho(i);
    }
}

void iterer_eqn_expl(Equation_base& eqn,int nb_iter,double dt,DoubleTab& current,DoubleTab& dudt,
                     int& converge,const int no_qdm_)
{
  if (nb_iter>1)
    {
      converge = 1;
      return;
    }
  if ((no_qdm_))
    {
      Cout<<eqn.que_suis_je()<<" equation is not solved."<<finl;
      eqn.valider_iteration();
      converge = 1;
      return;

    }
  double dt_stab = eqn.calculer_pas_de_temps();

  // Voir Schema_Temps_base::limpr pour information sur modf
  double n_sous_ite;
  modf((dt/dt_stab), &n_sous_ite);
  if (dt>dt_stab*n_sous_ite) n_sous_ite=n_sous_ite+1.;
  double dt_calc = dt/n_sous_ite;
  assert(dt_calc<=dt_stab);
  double test = 0;
  for (double i=0; i<n_sous_ite; i=i+1.)
    {
      eqn.derivee_en_temps_inco(dudt);
      dudt *= dt_calc;
      test += dt_calc;
      current += dudt;
      eqn.valider_iteration();
    }
  Cout <<"It is solved with " << n_sous_ite << " explicit sub-iterations."<<finl;
  assert(est_egal(test,dt));
  converge=1;
}

void iterer_eqn_expl_diffusion_implicite(Equation_base& eqn,int nb_iter,double dt,DoubleTab& current,DoubleTab& dudt,
                                         int& converge,const int no_qdm_,const double seuil)
{
  if (nb_iter>1)
    {
      converge = 1;
      return;
    }
  if ((no_qdm_))
    {
      Cout<<eqn.que_suis_je()<<" equation is not solved."<<finl;
      eqn.valider_iteration();
      converge = 1;
      return;
    }

  //dudt = current;
  Schema_Temps_base& sch = eqn.probleme().schema_temps();
  double seuil_sa = sch.seuil_diffusion_implicite();
  int flag_sa = sch.diffusion_implicite();
  sch.set_seuil_diffusion_implicite() = seuil;
  sch.set_diffusion_implicite() = 1;
  eqn.derivee_en_temps_inco(dudt);
  dudt *= dt;
  current += dudt;
  eqn.valider_iteration();

  //On remet les parametres de diffusion implicite a leur ancienne valeur
  sch.set_seuil_diffusion_implicite() = seuil_sa;
  sch.set_diffusion_implicite() = flag_sa;
  converge = 1;
}

bool Simple::iterer_eqn(Equation_base& eqn,const DoubleTab& inut,DoubleTab& current,
                        double dt,int nb_iter, int& ok)
{
  DoubleTab dudt(current);
  Parametre_implicite& param = get_and_set_parametre_implicite(eqn);


  int converge=0;
  int nb_pas_dt = eqn.schema_temps().nb_pas_dt();

  double temps = eqn.schema_temps().temps_courant();
  int freq = param.equation_frequence_resolue(temps);

  if (freq!=0 && (nb_pas_dt%freq)!=0)
    {
      Cout<<"Time step : "<<nb_pas_dt<<" - "<<eqn.que_suis_je()<<" equation is not solved."<<finl;
      eqn.valider_iteration();
      return true;
    }
  if (eqn.equation_non_resolue())
    {
      Cout<<eqn.que_suis_je()<<" equation is not solved."<<finl;
      // on calcule une fois la derivee pour avoir les flux bord
      if  (eqn.schema_temps().nb_pas_dt()==0)
        {
          DoubleTab toto(eqn.inconnue().valeurs());
          eqn.derivee_en_temps_inco(toto);
        }
      eqn.valider_iteration();
      converge = 1;
      return true;
    }
  if (param.calcul_explicite())
    {
      if (param.seuil_diffusion_implicite()<0)
        iterer_eqn_expl(eqn,nb_iter,dt,current,dudt,converge,no_qdm_);
      else
        iterer_eqn_expl_diffusion_implicite(eqn,nb_iter,dt,current,dudt,converge,no_qdm_,param.seuil_diffusion_implicite());

      return (converge==1);
    }

  double& seuil_convergence_implicite = param.seuil_convergence_implicite();
  double seuil_verification_solveur = param.seuil_verification_solveur();
  double seuil_test_preliminaire_solveur = param.seuil_test_preliminaire_solveur();
  assert(seuil_verification_solveur>0);
  SolveurSys& solveur = param.solveur();
  double seuil_convg = seuil_convergence_implicite;
  if (is_seuil_convg_variable)
    {
      double residu = ref_cast(Schema_Euler_Implicite,eqn.schema_temps()).residu_old();

      if ((residu*dt*10<seuil_convg)&&(nb_iter==1))
        {
          seuil_convergence_implicite /= 1.1;
          seuil_convg = seuil_convergence_implicite;
        }
      Cout<<"seuil_convg "<<seuil_convg<<finl;
    }

  converge = 0;
  if ((no_qdm_) && (sub_type(Navier_Stokes_std,eqn)))
    {
      Matrice& matrice_en_pression_2 = ref_cast(Navier_Stokes_std, eqn).matrice_pression();
      if (!matrice_en_pression_2.non_nul()) matrice_en_pression_2.detach();

      converge = 1;
      return true;
    }

  //////////////////////////////////////////////////////////////////////////////////////////
  // Resolution implicite -par iterer_NS pour Navier_Stokes
  //                          -...solveur.resoudre_systeme()... pour les autres equations
  /////////////////////////////////////////////////////////////////////////////////////////

  dudt = current; // pour pouvoir tester la convergence.
  Matrice_Morse matrice;
  if (!(sub_type(Navier_Stokes_std,eqn) && sub_type(SETS, *this))) //SETS et ICE gerent eux-memes leurs matrices
    {
      eqn.dimensionner_matrice(matrice);
      matrice.get_set_coeff() = 0;
    }

  DoubleTrav resu(current);

  if( sub_type(Navier_Stokes_std,eqn))
    {
      Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn);
      DoubleTab& pression = eqnNS.pression().valeurs();
      DoubleTrav secmem(pression);
      pression.echange_espace_virtuel();
      iterer_NS(eqnNS,current,pression,dt,matrice,seuil_verification_solveur,secmem,nb_iter,converge, ok);
    }
  else
    {
      solveur.valeur().reinit();
      DoubleTrav resu_temp(current); /* residu en increments */
      if (eqn.has_interface_blocs()) /* si assembler_blocs est disponible */
        {
          if (eqn.discretisation().que_suis_je().finit_par("MAC"))
            {
              eqn.assembler_blocs_avec_inertie({{ eqn.inconnue().le_nom().getString(), &matrice }}, resu_temp, { });
              resu = resu_temp;
              matrice.ajouter_multvect(current, resu);
            }
          else
            {
              eqn.assembler_blocs_avec_inertie({{ eqn.inconnue().le_nom().getString(), &matrice }}, resu, { });
              resu_temp = 0;
              matrice.ajouter_multvect(current,resu_temp);
              resu_temp -= resu;
            }

        }
      else
        {
          eqn.assembler_avec_inertie(matrice,current,resu);
          resu_temp = 0;
          matrice.ajouter_multvect(current,resu_temp);
          resu_temp -= resu;
        }

      if (seuil_test_preliminaire_solveur>0)
        {
          double norme_b=mp_norme_vect(resu_temp);
          if (norme_b<seuil_test_preliminaire_solveur)
            {
              //  GF le test suivant est peut etre intelligent ?
              //      if ( nb_iter>1)
              {
                converge = 1;
                Cout<<"Resolution of system is not necessary: "<< norme_b <<" < "<< seuil_test_preliminaire_solveur <<finl;
              }
            }
        }
      if (converge==0)
        {
          int con = 0;
          while (con==0)
            {
              con = 1;
              solveur.resoudre_systeme(matrice,resu,current);
              ok = eqn.milieu().check_unknown_range(); //verification que l'inconnue est dans les bornes du milieu

              if (ok)
                {
                  resu_temp = 0;
                  matrice.ajouter_multvect(current,resu_temp);
                  resu_temp -= resu;
                  double norme_resu = mp_norme_vect(resu_temp) ;

                  if (norme_resu>seuil_verification_solveur) con = 0;
                  if (con==0)
                    {
                      Cout<<"Residu of iterative solving : "<<norme_resu<<" instead of "<<seuil_verification_solveur<<finl;
                      Cout<<"Iterating on the linear system again:"<< finl;
                      seuil_verification_solveur *= 2;
                      con = 0;
                    }
                }
              else current = eqn.inconnue().passe(); //si ok == 0, on restaure la valeur passee de inco
            }
          converge = 0;
        }
    }

  ///////////////////////////////////////////////////////////////////////
  // Test de convergence de la solution entre deux iterations successives
  // Pas applique pour l inconnue de N_S avec alorithme PISO et Implicite
  ///////////////////////////////////////////////////////////////////////

  if(!converge && ok)
    {
      // permet de controler ce qui se passe
      // en particulier la positivite de K et de eps
      eqn.valider_iteration();
      dudt -= current;
      double dudt_norme = mp_norme_vect(dudt);
      converge = (dudt_norme < seuil_convg);
      if (!converge)
        {
          Cout<<eqn.que_suis_je()<<" is not converged at the implicit iteration "<<nb_iter<<" ( ||uk-uk-1|| = "<<dudt_norme<<" > implicit threshold "<<seuil_convg<<" )"<<finl;
          if (nb_iter>=10) Cout << "Consider lowering facsec_max value. Look at the reference manual for advice to set facsec_max value according to the problem type." << finl;
        }
      else
        Cout<<eqn.que_suis_je()<<" is converged at the implicit iteration "<<nb_iter<<" ( ||uk-uk-1|| = "<<dudt_norme<<" < implicit threshold "<<seuil_convg<<" )"<<finl;
    }

  if(ok && eqn.discretisation().que_suis_je().finit_par("MAC")) eqn.probleme().mettre_a_jour(eqn.schema_temps().temps_courant());
  solveur->reinit();
  return (ok && converge==1);
}

bool Simple::iterer_eqs(LIST(REF(Equation_base)) eqs, int nb_iter, int& ok)
{
  // on recupere le solveur de systeme lineaire
  Parametre_implicite& param = get_and_set_parametre_implicite(eqs[0]);
  SolveurSys& solveur = param.solveur();
  double seuil_convg = param.seuil_convergence_implicite();
  int i, j;

  /* cle pour la memoization */
  std::vector<intptr_t> key(eqs.size());
  for (i = 0; i < eqs.size(); i++) key[i] = (intptr_t) &eqs[i].valeur();

  int init = !mbloc.count(key); //premier passage
  Matrice_Bloc& Mglob = mbloc[key];

  if (init) for (Mglob.dimensionner(eqs.size(), eqs.size()), i = 0; i < eqs.size(); i++)
      for (j = 0; j < eqs.size(); j++) Mglob.get_bloc(i, j).typer("Matrice_Morse");

  /* pour interface_blocs : si toutes les equations ont cette interface, on l'utilise */
  int interface_blocs_ok = 1;
  for (i = 0; i < eqs.size(); i++) interface_blocs_ok &= eqs[i]->has_interface_blocs();
  std::vector<matrices_t> mats(eqs.size()); //ligne de matrices de l'equation i
  for (i = 0; i < eqs.size(); i++) for (j = 0; j < eqs.size(); j++)
      {
        Nom nom_i = eqs[j]->inconnue().le_nom();
        // champ d'un autre probleme : on ajoute un suffixe
        if (eqs[i]->probleme().le_nom().getString() != eqs[j]->probleme().le_nom().getString()) nom_i += Nom("_") + eqs[j]->probleme().le_nom();
        mats[i][nom_i.getString()] = &ref_cast(Matrice_Morse, Mglob.get_bloc(i, j).valeur());
      }

  //MD_Vector global
  MD_Vector_composite mdc; //version composite
  for (i = 0; i < eqs.size(); i++)
    mdc.add_part(eqs[i]->inconnue().valeurs().get_md_vector(), eqs[i]->inconnue().valeurs().line_size());
  MD_Vector mdv;
  mdv.copy(mdc);

  if (init) //1er passage -> dimensionnement des MD_Vector et des matrices
    {
      /* dimensionnement de la matrice globale */
      if (interface_blocs_ok) for (i = 0; i < eqs.size(); i++) eqs[i]->dimensionner_blocs(mats[i], {});
      else for (i = 0; i < eqs.size(); i++) for (j = 0; j < eqs.size(); j++)
            {
              Matrice_Morse& mat = ref_cast(Matrice_Morse, Mglob.get_bloc(i, j).valeur()), mat2;
              int nl = eqs[i]->inconnue().valeurs().size_totale(), nc = eqs[j]->inconnue().valeurs().size_totale();
              if (i == j) eqs[i]->dimensionner_matrice(mat);
              eqs[i]->dimensionner_termes_croises(i == j ? mat2 : mat, eqs[j]->probleme(), nl, nc);
              if (i == j) mat += mat2;
            }
    }
  else for (i = 0; i < eqs.size(); i++) for (j = 0; j < eqs.size(); j++) //passages suivantes -> il suffit de reallouer les tableaux coeff()
        {
          Matrice_Morse& mat = ref_cast(Matrice_Morse, Mglob.get_bloc(i, j).valeur());
          mat.get_set_coeff().resize(mat.get_set_tab2().size());
        }

  //tableaux de travail
  DoubleTrav inconnues, residus, dudt;
  MD_Vector_tools::creer_tableau_distribue(mdv, inconnues);
  MD_Vector_tools::creer_tableau_distribue(mdv, residus);
  MD_Vector_tools::creer_tableau_distribue(mdv, dudt);
  DoubleTab_parts residu_parts(residus), inconnues_parts(inconnues), dudt_parts(dudt);

  //remplissage des inconnues
  for(i = 0; i < eqs.size(); i++) inconnues_parts[i] = eqs[i]->inconnue().valeurs();
  dudt = inconnues;

  //remplissage des matrices
  if (interface_blocs_ok)
    {
      for (i = 0; i < eqs.size(); i++)
        {
          eqs[i]->assembler_blocs_avec_inertie(mats[i], residu_parts[i], {});
          if (!eqs[i]->discretisation().que_suis_je().finit_par("MAC"))
            {
              for (j = 0; j < eqs.size(); j++)
                {
                  Nom nom_j = eqs[j]->inconnue().le_nom();
                  if (eqs[i]->probleme().le_nom().getString() != eqs[j]->probleme().le_nom().getString())
                    {
                      nom_j += Nom("_") + eqs[j]->probleme().le_nom();
                      mats[i][nom_j.getString()]->ajouter_multvect(inconnues_parts[j], residu_parts[i]);
                    }
                }
            }
        }
      if (eqs[0]->discretisation().que_suis_je().finit_par("MAC")) Mglob.ajouter_multvect(inconnues, residus); //pour ne pas resoudre en increments
    }
  else for(i = 0; i < eqs.size(); i++) for (j = 0; j < eqs.size(); j++)
        {
          Matrice_Morse& mat = ref_cast(Matrice_Morse, Mglob.get_bloc(i, j).valeur());
          eqs[i]->ajouter_termes_croises(inconnues_parts[i], eqs[j]->probleme(), inconnues_parts[j], residu_parts[i]);
          eqs[i]->contribuer_termes_croises(inconnues_parts[i], eqs[j]->probleme(), inconnues_parts[j], mat);
          /* si i == j, alors assembler_avec_inertie() se charge du produit matrice/vecteur : sinon, on doit le faire a la main */
          if (i == j) eqs[i]->assembler_avec_inertie(mat, inconnues_parts[i], residu_parts[i]);
          else mat.ajouter_multvect(inconnues_parts[j], residu_parts[i]);
        }

  // resolution
  solveur.valeur().reinit();
  solveur.resoudre_systeme(Mglob, residus, inconnues);
  inconnues.echange_espace_virtuel();

  // mise a jour
  bool converge = true;
  for(i = 0; i < eqs.size(); i++)
    {
      dudt_parts[i] -= inconnues_parts[i];
      double dudt_norme = mp_norme_vect(dudt_parts[i]);
      eqs[i]->inconnue().valeurs() = inconnues_parts[i];

      converge &= (dudt_norme < seuil_convg);
      if (!converge)
        {
          Cout<<eqs[i]->que_suis_je()<<" is not converged at the implicit iteration "<<nb_iter<<" ( ||uk-uk-1|| = "<<dudt_norme<<" > implicit threshold "<<seuil_convg<<" )"<<finl;
          if (nb_iter>=10) Cout << "Consider lowering facsec_max value. Look at the reference manual for advice to set facsec_max value according to the problem type." << finl;
        }
      else
        Cout<<eqs[i]->que_suis_je()<<" is converged at the implicit iteration "<<nb_iter<<" ( ||uk-uk-1|| = "<<dudt_norme<<" < implicit threshold "<<seuil_convg<<" )"<<finl;
      eqs[i]->inconnue().futur() = eqs[i]->inconnue().valeurs();
      const double t = eqs[i]->schema_temps().temps_courant() + eqs[i]->schema_temps().pas_de_temps();
      eqs[i]->zone_Cl_dis()->imposer_cond_lim(eqs[i]->inconnue(), t);
      eqs[i]->inconnue().valeurs() = eqs[i]->inconnue().futur();
      eqs[i]->inconnue().valeur().Champ_base::changer_temps(t);
    }
  for(i = 0; i < eqs.size(); i++) eqs[i]->probleme().mettre_a_jour(eqs[i]->schema_temps().temps_courant());

  //on desalloue les tableaux de coeffs
  for (i = 0; i < eqs.size(); i++) for (j = 0; j < eqs.size(); j++) ref_cast(Matrice_Morse, Mglob.get_bloc(i, j).valeur()).get_set_coeff().reset();

  return converge;
}

void Simple::calculer_correction_en_vitesse(const DoubleTrav& correction_en_pression,DoubleTrav& gradP,DoubleTrav& correction_en_vitesse,const Matrice_Morse& matrice,const Operateur_Grad& gradient)
{
  int deux_entrees = 0;
  if (correction_en_vitesse.nb_dim()==2) deux_entrees = 1;
  gradient.valeur().multvect(correction_en_pression,gradP);
  int nb_comp = 1;
  if(deux_entrees)
    nb_comp = correction_en_vitesse.dimension(1);

  int nb_ligne_reel = correction_en_vitesse.dimension(0);
  if ((Process::nproc()==1)&& (nb_ligne_reel!=matrice.ordre()/nb_comp)) abort();
  if (deux_entrees==0)
    {
      // D(Uk-1)^-1 resu
      int i,j;
      for(i=0; i<nb_ligne_reel; i++)

        for (j=0; j<nb_comp; j++)
          {
            //k=tab1(i*nb_comp+j)-1;
            correction_en_vitesse(i) = -gradP(i)/matrice(i*nb_comp+j,i*nb_comp+j);
          }
    }
  else
    {
      int i,j;
      for(i=0; i<nb_ligne_reel; i++)
        for (j=0; j<nb_comp; j++)
          {
            //k = tab1(i*nb_comp+j)-1;
            correction_en_vitesse(i,j) = -gradP(i,j)/matrice(i*nb_comp+j,i*nb_comp+j);
          }
    }
  correction_en_vitesse.echange_espace_virtuel();
}


//Entree : Uk-1 ; Pk-1
//Sortie Uk ; Pk
//k designe une iteration

void Simple::iterer_NS(Equation_base& eqn,DoubleTab& current,DoubleTab& pression,
                       double dt,Matrice_Morse& matrice,double seuil_resol,DoubleTrav& secmem,int nb_ite,int& converge, int& ok)
{
  Parametre_implicite& param = get_and_set_parametre_implicite(eqn);
  SolveurSys& solveur = param.solveur();

  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn);
  DoubleTrav gradP(current);
  DoubleTrav correction_en_pression(pression);
  DoubleTrav correction_en_vitesse(current);
  DoubleTrav resu(current);
  int is_dilat = eqn.probleme().is_dilatable();

  //int deux_entrees = 0;
  //if (current.nb_dim()==2) deux_entrees = 1;
  Operateur_Grad& gradient = eqnNS.operateur_gradient();
  Operateur_Div& divergence = eqnNS.operateur_divergence();

  /* int nb_comp = 1;
     int nb_dim = current.nb_dim();
     if (nb_dim==2)
     nb_comp = current.dimension(1);
  */

  gradient.calculer(pression,gradP);
  //Construction de matrice et resu
  //matrice = A[Uk-1] = M/dt + CONV + DIFF
  //resu = A[Uk-1]Uk-1 -(A[Uk-1]Uk-1-Ss) + Sv + (M/dt)Uk-1 -BtPk-1
  if (eqnNS.has_interface_blocs()) //si l'interface blocs est disponible, on l'utilise
    eqnNS.assembler_blocs_avec_inertie({{ "vitesse", &matrice }}, resu);
  else //sinon, on passe par ajouter/contribuer
    {
      resu -= gradP;
      eqnNS.assembler_avec_inertie(matrice,current,resu);
    }

  solveur.valeur().reinit();

  //Resolution du systeme A[Uk-1]U* = -BtP* + Sv + Ss + (M/dt)Uk-1
  //current = U*
  solveur.resoudre_systeme(matrice,resu,current);

  //Relaxation du champ de vitesse U*
  //U* = alpha U*_new + (1-alpha)*U*_old
  if (nb_ite==1)
    Ustar_old = current;
  current *= alpha_ ;
  current.ajoute(1.-alpha_,Ustar_old);
  current.echange_espace_virtuel();
  Ustar_old = current;

  //Construction de matrice_en_pression_2 = BD-1Bt[Uk-1]
  Matrice& matrice_en_pression_2 = eqnNS.matrice_pression();
  assembler_matrice_pression_implicite(eqnNS,matrice,matrice_en_pression_2);
  SolveurSys& solveur_pression_ = eqnNS.solveur_pression();
  solveur_pression_.valeur().reinit();

  //Calcul de secmem = BU* (en incompressible) BU* -drho/dt (en quasi-compressible)
  if (is_dilat)
    {
      if (with_d_rho_dt_)
        {
          Fluide_Dilatable_base& fluide_dil = ref_cast(Fluide_Dilatable_base,eqn.milieu());
          fluide_dil.secmembre_divU_Z(secmem);
          secmem *= -1;
        }
      else secmem = 0;
      divergence.ajouter(current,secmem);
    }
  else
    divergence.calculer(current,secmem);
  secmem *= -1;
  secmem.echange_espace_virtuel();


  //Resolution du systeme (BD-1Bt)P' = BU* en incompressible
  //                          (BD-1Bt)P' = BU* -drho/dt en quasi-compressible
  //correction_en_pression = P'
  solveur_pression_.resoudre_systeme(matrice_en_pression_2.valeur(),
                                     secmem,correction_en_pression);

  //Resolution de DU' = BP'
  //correction_en_vitesse = U'
  calculer_correction_en_vitesse(correction_en_pression,gradP,correction_en_vitesse,matrice,gradient);

  //Correction de la pression P = P* + beta_*P'
  //Correction de la vitesse U = U* + beta_u*U' (beta_u=1)

  pression.ajoute(beta_,correction_en_pression);
  eqnNS.assembleur_pression().valeur().modifier_solution(pression);

  current += correction_en_vitesse;

  if (is_dilat)
    diviser_par_rho_np1_face(eqn,current);
}

