/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Echange_contact_PolyMAC_P0P1NC.h>
#include <Op_Diff_PolyMAC_P0P1NC_Elem.h>
#include <Champ_Elem_PolyMAC_P0P1NC.h>
#include <Echange_externe_impose.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <Flux_parietal_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Dirichlet_homogene.h>
#include <Schema_Temps_base.h>
#include <Champ_front_calc.h>
#include <TRUSTTab_parts.h>
#include <MD_Vector_base.h>
#include <communications.h>
#include <Synonyme_info.h>
#include <Pb_Multiphase.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <functional>
#include <cmath>
#include <deque>

extern Stat_Counter_Id diffusion_counter_;

Implemente_instanciable_sans_constructeur(Op_Diff_PolyMAC_P0P1NC_Elem, "Op_Diff_PolyMAC_P0P1NC_Elem|Op_Diff_PolyMAC_P0P1NC_var_Elem", Op_Diff_PolyMAC_P0P1NC_base);

Op_Diff_PolyMAC_P0P1NC_Elem::Op_Diff_PolyMAC_P0P1NC_Elem()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_PolyMAC_P0P1NC_Elem::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0P1NC_base::printOn(os); }

Entree& Op_Diff_PolyMAC_P0P1NC_Elem::readOn(Entree& is) { return Op_Diff_PolyMAC_P0P1NC_base::readOn(is); }

void Op_Diff_PolyMAC_P0P1NC_Elem::completer()
{
  Op_Diff_PolyMAC_P0P1NC_base::completer();
  Equation_base& eq = equation();
  Champ_Elem_PolyMAC_P0P1NC& ch = ref_cast(Champ_Elem_PolyMAC_P0P1NC, le_champ_inco.non_nul() ? le_champ_inco.valeur() : eq.inconnue().valeur());
  ch.init_auxiliary_variables();
  const Domaine_PolyMAC_P0P1NC& domaine = le_dom_poly_.valeur();
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyMAC_P0P1NC_Elem : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  flux_bords_.resize(domaine.premiere_face_int(), ch.valeurs().line_size());
}

void Op_Diff_PolyMAC_P0P1NC_Elem::init_op_ext() const
{
  if (op_ext.size())
    return; //deja fait
  /* recherche d'operateurs connectes par des Echange_Contact_PolyMAC_P0P1NC (eventuellement indirectement) */
  std::set<const Op_Diff_PolyMAC_P0P1NC_Elem*> ops_tbd = { this }, ops; //operateurs a scanner pour remplir op_ext, operateurs trouves
  while (ops_tbd.size())
    {
      const Op_Diff_PolyMAC_P0P1NC_Elem *op = *ops_tbd.begin();
      ops_tbd.erase(ops_tbd.begin()), ops.insert(op);
      const Conds_lim& cls = op->equation().domaine_Cl_dis()->les_conditions_limites();
      for (const auto &itr : cls)
        if (sub_type(Echange_contact_PolyMAC_P0P1NC, itr.valeur()))
          {
            const Echange_contact_PolyMAC_P0P1NC& cl = ref_cast(Echange_contact_PolyMAC_P0P1NC, itr.valeur());
            cl.init_op();
            if (!ops.count(&cl.o_diff.valeur()))
              ops_tbd.insert(&cl.o_diff.valeur()); //on a un nouvel operateur a scanner
          }
    }
  op_ext = { this };
  for (auto &&op : ops)
    if (op != this)
      op_ext.push_back(op); /* remplissage de op_ext avec l'operateur local en 1er */

  /* remplissage des o_idx des Echange_Contact_PolyMAC_P0P1NC connectes */
  const Conds_lim& cls = equation().domaine_Cl_dis()->les_conditions_limites();
  for (int i = 0; i < cls.size(); i++)
    if (sub_type(Echange_contact_PolyMAC_P0P1NC, cls[i].valeur()))
      {
        const Echange_contact_PolyMAC_P0P1NC& cl = ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[i].valeur());
        cl.o_idx = (int) (std::find(op_ext.begin(), op_ext.end(), &cl.o_diff.valeur()) - op_ext.begin());
      }
}

double Op_Diff_PolyMAC_P0P1NC_Elem::calculer_dt_stab() const
{
  const Domaine_PolyMAC_P0P1NC& domaine = le_dom_poly_.valeur();
  const IntTab& e_f = domaine.elem_faces();
  const DoubleTab& nf = domaine.face_normales(), *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue()->passe() : nullptr, &diffu =
                                                          diffusivite_pour_pas_de_temps().valeurs(), &lambda = diffusivite().valeurs();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes();
  update_nu();

  int i, e, f, n, N = equation().inconnue()->valeurs().dimension(1), cD = diffu.dimension(0) == 1, cL = lambda.dimension(0) == 1;
  double dt = 1e10;
  DoubleTrav flux(N);
  for (e = 0; e < domaine.nb_elem(); e++)
    {
      for (flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          flux(n) += domaine.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
      for (n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 1e-3) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = std::min(dt, pe(e) * ve(e) * (alp ? (*alp)(e, n) : 1) * (lambda(!cL * e, n) / diffu(!cD * e, n)) / flux(n));
      if (dt < 0)
        abort();
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyMAC_P0P1NC_Elem::dimensionner_blocs_ext(int aux_only, matrices_t matrices, const tabs_t& semi_impl) const
{
  init_op_ext();
  const std::string& nom_inco = (le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue())->le_nom().getString();
  int i, j, k, l, e, o_e, f, o_f, fb, m, n, M, n_ext = (int) op_ext.size(), n_sten = 0, semi = (int) semi_impl.count(nom_inco);
  long p;
  std::vector<Matrice_Morse*> mat(n_ext); //matrices
  std::vector<int> N, ne_tot; //composantes, nombre d'elements total par pb
  std::vector<std::reference_wrapper<const Domaine_PolyMAC_P0P1NC>> domaine; //domaines
  std::vector<std::reference_wrapper<const Conds_lim>> cls; //conditions aux limites
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_e; //tableaux "fcl", "elem_faces", "faces_voisins"
  std::vector<std::reference_wrapper<const DoubleTab>> diffu, inco; //inconnues, normales aux faces, positions elems / faces / sommets
  std::deque<ConstDoubleTab_parts> v_part; //blocs de chaque inconnue
  std::vector<IntTrav> stencil(n_ext); //stencils par matrice
  for (i = 0, M = 0; i < n_ext; M = std::max(M, N[i]), i++)
    {
      std::string nom_mat = i ? nom_inco + "/" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : nullptr;
      domaine.push_back(std::ref(ref_cast(Domaine_PolyMAC_P0P1NC, op_ext[i]->equation().domaine_dis().valeur())));
      f_e.push_back(std::ref(domaine[i].get().face_voisins())), e_f.push_back(std::ref(domaine[i].get().elem_faces()));
      cls.push_back(std::ref(op_ext[i]->equation().domaine_Cl_dis()->les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyMAC_P0P1NC_Elem, *op_ext[i]).nu());
      const Champ_Elem_PolyMAC_P0P1NC& ch = ref_cast(Champ_Elem_PolyMAC_P0P1NC, op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue().valeur() : op_ext[i]->equation().inconnue().valeur());

      N.push_back(ch.valeurs().line_size()), fcl.push_back(std::ref(ch.fcl())), ne_tot.push_back(domaine[i].get().nb_elem_tot());
      inco.push_back(ch.valeurs()), v_part.emplace_back(ch.valeurs());
      stencil[i].resize(0, 2);

    }

  IntTrav tpfa(0, N[0]); //pour suivre quels flux sont a deux points
  domaine[0].get().creer_tableau_faces(tpfa), tpfa = 1;

  if (!aux_only)
    Cerr << "Op_Diff_PolyMAC_P0P1NC_Elem::dimensionner() : ";
  DoubleTrav w2;
  /* probleme local */
  for (e = 0; e < ne_tot[0]; e++)
    {
      domaine[0].get().W2(&diffu[0].get(), e, w2); //interpolation : [n_ef.nu grad T]_f = w2_{ff'} (T_f' - T_e)
      //element <-> toutes ses faces (non Dirichlet)
      if (!aux_only && !semi)
        for (i = 0; i < w2.dimension(0); i++)
          if (!semi && fcl[0](f = e_f[0](e, i), 0) < 6)
            {
              if (e < domaine[0].get().nb_elem())
                for (n = 0; n < N[0]; n++)
                  stencil[0].append_line(N[0] * e + n, N[0] * (ne_tot[0] + f) + n);
              if (f < domaine[0].get().nb_faces())
                for (n = 0; n < N[0]; n++)
                  stencil[0].append_line(N[0] * (ne_tot[0] + f) + n, N[0] * e + n);
            }
      //face <-> face (si les deux sont non Dirichlet)
      for (i = 0; i < w2.dimension(0); i++)
        if ((f = e_f[0](e, i)) >= domaine[0].get().nb_faces())
          continue; //face virtuelle -> rien
        else if (semi || fcl[0](f = e_f[0](e, i), 0) > 5)
          for (n = 0; n < N[0]; n++) //Dirichlet ou semi-implicite -> diagonale
            stencil[0].append_line(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[0] + f) + n);
        else
          for (j = 0; j < w2.dimension(1); j++)
            for (fb = e_f[0](e, j), n = 0; n < N[0]; n++) //cas reel
              if (fcl[0](fb, 0) < 6 && w2(i, j, n))
                stencil[0].append_line(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[0] + fb) + n), tpfa(f, n) &= (i == j);
    }
  /* problemes distants : pour les Echange_contact */
  const Echange_contact_PolyMAC_P0P1NC *pcl;
  if (!semi)
    for (i = 0; i < cls[0].get().size(); i++)
      if ((pcl = sub_type(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[i].valeur()) ? &ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[i].valeur()) : nullptr))
        for (pcl->init_f_dist(), j = 0, p = std::find(op_ext.begin(), op_ext.end(), &pcl->o_diff.valeur()) - op_ext.begin(); j < pcl->fvf->nb_faces(); j++)
          {
            f = pcl->fvf->num_face(j), o_f = pcl->f_dist(j), o_e = f_e[p](o_f, 0); //faces cote local/distant, elem cote distant
            domaine[p].get().W2(&diffu[p].get(), o_e, w2); //matrice w2 de l'autre cote
            k = (int) (std::find(&e_f[p](o_e, 0), &e_f[p](o_e, 0) + w2.dimension(0), o_f) - &e_f[p](o_e, 0)); //indice de o_f dans o_e
            if (!aux_only)
              for (n = 0; n < N[0]; n++)
                for (m = (N[0] == N[p]) * n; m < (N[0] == N[p] ? n + 1 : N[p]); m++)
                  stencil[p].append_line(N[0] * (ne_tot[0] + f) + n, N[p] * o_e + m); //face <-> elem : compo par compo si N[0] == N[p], tout sinon
            for (l = 0; l < w2.dimension(0); l++)
              if (fcl[p](fb = e_f[p](o_e, l), 0) < 6)
                for (n = 0; n < N[0]; n++)
                  for (m = (N[0] == N[p]) * n; m < (N[0] == N[p] ? n + 1 : N[p]); m++)
                    if (w2(k, l, m))
                      stencil[p].append_line(N[0] * (!aux_only * ne_tot[0] + f) + n, N[p] * (!aux_only * ne_tot[p] + fb) + m);
          }

  for (i = 0; i < n_ext; i++)
    if (mat[i])
      {
        tableau_trier_retirer_doublons(stencil[i]);
        Matrice_Morse mat2;
        Matrix_tools::allocate_morse_matrix(aux_only ? v_part[0][1].size_totale() : inco[0].get().size_totale(), aux_only ? v_part[i][1].size_totale() : inco[i].get().size_totale(), stencil[i], mat2);
        mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
      }

  for (auto &&st : stencil)
    n_sten += st.dimension(0); //n_sten : nombre total de points du stencil de l'operateur
  if (!aux_only)
    Cerr << "width " << Process::mp_sum(n_sten) * 1. / (N[0] * domaine[0].get().mdv_elems_faces->nb_items_seq_tot()) << " "
         << mp_somme_vect(tpfa) * 100. / (N[0] * domaine[0].get().md_vector_faces()->nb_items_seq_tot()) << "% TPFA " << finl;
}

void Op_Diff_PolyMAC_P0P1NC_Elem::ajouter_blocs_ext(int aux_only, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  init_op_ext();
  const std::string& nom_inco = (le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue())->le_nom().getString();
  int i, j, k1, k2, e, f, fb, n, M, n_ext = (int) op_ext.size(), semi = (int) semi_impl.count(nom_inco), d, D = dimension;
  std::vector<Matrice_Morse*> mat(n_ext); //matrices
  std::vector<int> N, ne_tot; //composantes
  std::vector<std::reference_wrapper<const Domaine_PolyMAC_P0P1NC>> domaine; //domaines
  std::vector<std::reference_wrapper<const Conds_lim>> cls; //conditions aux limites
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_e, f_s; //tableaux "fcl", "elem_faces", "faces_voisins"
  std::vector<std::reference_wrapper<const DoubleVect>> fs, pf, pe, ve; //surfaces
  std::vector<std::reference_wrapper<const DoubleTab>> inco, xp, xv, diffu, v_aux; //inconnues, normales aux faces, positions elems / faces / sommets
  std::deque<ConstDoubleTab_parts> v_part; //blocs de chaque inconnue
  std::vector<const Flux_parietal_base*> corr; //correlations de flux parietal (si elles existent)
  for (i = 0, M = 0; i < n_ext; M = std::max(M, N[i]), i++)
    {
      std::string nom_mat = i ? nom_inco + "/" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : nullptr;
      domaine.push_back(std::ref(ref_cast(Domaine_PolyMAC_P0P1NC, op_ext[i]->equation().domaine_dis().valeur())));
      f_e.push_back(std::ref(domaine[i].get().face_voisins())), e_f.push_back(std::ref(domaine[i].get().elem_faces())), f_s.push_back(std::ref(domaine[i].get().face_sommets()));
      fs.push_back(std::ref(domaine[i].get().face_surfaces()));
      xp.push_back(std::ref(domaine[i].get().xp())), xv.push_back(std::ref(domaine[i].get().xv()));
      pe.push_back(std::ref(equation().milieu().porosite_elem())), pf.push_back(std::ref(equation().milieu().porosite_face())), ve.push_back(std::ref(domaine[i].get().volumes()));
      cls.push_back(std::ref(op_ext[i]->equation().domaine_Cl_dis()->les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyMAC_P0P1NC_Elem, *op_ext[i]).nu());
      const Champ_Elem_PolyMAC_P0P1NC& ch = ref_cast(Champ_Elem_PolyMAC_P0P1NC, op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue().valeur() : op_ext[i]->equation().inconnue().valeur());
      inco.push_back(std::ref(semi_impl.count(nom_mat) ? semi_impl.at(nom_mat) : ch.valeurs())), v_part.emplace_back(inco.back());
      corr.push_back(
        sub_type(Energie_Multiphase, op_ext[i]->equation()) && op_ext[i]->equation().probleme().has_correlation("flux_parietal") ?
        &ref_cast(Flux_parietal_base, op_ext[i]->equation().probleme().get_correlation("flux_parietal").valeur()) : nullptr);
      N.push_back(inco[i].get().line_size()), ne_tot.push_back(domaine[i].get().nb_elem_tot()), fcl.push_back(std::ref(ch.fcl()));
    }

  /* que faire avec les variables auxiliaires ? */
  double t = equation().schema_temps().temps_courant(), dt = equation().schema_temps().pas_de_temps(), fac, prefac;
  if (aux_only)
    use_aux_ = 0; /* 1) on est en train d'assembler le systeme de resolution des variables auxiliaires lui-meme */
  else if (mat[0] && !semi)
    t_last_aux_ = t + dt, use_aux_ = 0; /* 2) on est en implicite complet : pas besoin de mat_aux / var_aux, on aura les variables a t + dt */
  else if (t_last_aux_ < t)
    update_aux(t); /* 3) premier pas a ce temps en semi-implicite : on calcule les variables auxiliaires a t et on les stocke dans var_aux */
  for (i = 0; i < n_ext; i++)
    v_aux.push_back(use_aux_ ? ref_cast(Op_Diff_PolyMAC_P0P1NC_Elem, *op_ext[i]).var_aux : v_part[i][1]); /* les variables auxiliaires peuvent etre soit dans inco/semi_impl (cas 1), soit dans var_aux (cas 2) */

  DoubleTrav w2, flux(N[0]), acc(N[0]);
  for (e = 0; e < ne_tot[0]; e++)
    {
      domaine[0].get().W2(&diffu[0].get(), e, w2); //interpolation : [n_ef.nu grad T]_f = w2_{ff'} (T_f' - T_e)

      for (i = 0; i < w2.dimension(0); i++) //seconds membres
        {
          for (f = e_f[0](e, i), flux = 0, j = 0; j < w2.dimension(1); j++)
            for (fb = e_f[0](e, j), n = 0; n < N[0]; n++)
              flux(n) += w2(i, j, n) * (v_aux[0](fb, n) - inco[0](e, n));
          if (!semi && fcl[0](f, 0) < 6)
            for (n = 0; n < N[0]; n++)
              secmem(!aux_only * ne_tot[0] + f, n) -= flux(n);
          if (!aux_only)
            for (n = 0; n < N[0]; n++)
              secmem(e, n) += flux(n);
          if (!aux_only && f < domaine[0].get().premiere_face_int())
            for (n = 0; n < N[0]; n++)
              flux_bords_(f, n) = flux(n); //flux aux bords
        }

      if (semi || !mat[0])
        continue;
      //matrice: elem-elem
      for (acc = 0, i = 0; i < w2.dimension(0); i++)
        for (j = 0; j < w2.dimension(1); j++)
          for (n = 0; n < N[0]; n++)
            acc(n) += w2(i, j, n);
      if (!aux_only && e < domaine[0].get().nb_elem())
        for (n = 0; n < N[0]; n++)
          (*mat[0])(N[0] * e + n, N[0] * e + n) += acc(n);
      //matrice: elem-face
      if (!aux_only)
        for (i = 0; i < w2.dimension(0); i++)
          if (fcl[0](f = e_f[0](e, i), 0) < 6)
            {
              for (acc = 0, j = 0; j < w2.dimension(1); j++)
                for (n = 0; n < N[0]; n++)
                  acc(n) += w2(i, j, n);
              if (f < domaine[0].get().nb_faces())
                for (n = 0; n < N[0]; n++)
                  (*mat[0])(N[0] * (ne_tot[0] + f) + n, N[0] * e + n) -= acc(n);
              if (e < domaine[0].get().nb_elem())
                for (n = 0; n < N[0]; n++)
                  (*mat[0])(N[0] * e + n, N[0] * (ne_tot[0] + f) + n) -= acc(n);
            }
      //matrice : face-face
      for (i = 0; i < w2.dimension(0); i++)
        if (fcl[0](f = e_f[0](e, i), 0) < 6 && f < domaine[0].get().nb_faces())
          for (j = 0; j < w2.dimension(1); j++)
            if (fcl[0](fb = e_f[0](e, j), 0) < 6)
              for (n = 0; n < N[0]; n++)
                if (w2(i, j, n))
                  (*mat[0])(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[0] + fb) + n) += w2(i, j, n);
    }

  //contributions restantes aux equations aux faces
  for (f = 0; f < domaine[0].get().nb_faces(); f++)
    if (!aux_only && semi && mat[0])
      for (n = 0; n < N[0]; n++) //semi-implicite : T_f^+ = var_aux
        secmem(ne_tot[0] + f, n) += v_aux[0](f, n) - (le_champ_inco.non_nul() ? le_champ_inco->valeur().valeurs() : equation().inconnue()->valeurs())(ne_tot[0] + f, n), (*mat[0])(N[0] * (ne_tot[0] + f) + n,
                                    N[0] * (ne_tot[0] + f) + n)++;
    else if (fcl[0](f, 0) == 0 || fcl[0](f, 0) == 5)
      continue; //face interne ou Neumann_val_ext -> rien
    else if (corr[0]) //Pb_Multiphase avec flux parietal -> on ne traite (pour le moment) que Dirichlet et Echange_contact
      {
        if (fcl[0](f, 0) == 2)
          abort(); //Echange_global_impose -> on ne sait pas faire (que vaut T_paroi dans l'element?)
        const Echange_contact_PolyMAC_P0P1NC *ech = fcl[0](f, 0) == 3 ? &ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[fcl[0](f, 1)].valeur()) : nullptr;
        int o_p = ech ? ech->o_idx : -1, o_f = ech ? ech->f_dist(fcl[0](f, 2)) : -1;
        const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
        const DoubleTab& alpha = pbm.equation_masse().inconnue()->passe(), &dh = pbm.milieu().diametre_hydraulique_elem(),
                         &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression()->passe(), &vit = pbm.equation_qdm().inconnue()->passe(),
                          &lambda = pbm.milieu().conductivite()->passe(), &mu = ref_cast(Fluide_base, pbm.milieu()).viscosite_dynamique()->passe(), &rho = pbm.milieu().masse_volumique()->passe(), &Cp =
                                                                                  pbm.milieu().capacite_calorifique()->passe();
        Flux_parietal_base::input_t in;
        Flux_parietal_base::output_t out;
        DoubleTrav qpk(N[0]), dTf_qpk(N[0], N[0]), dTp_qpk(N[0]), qpi(N[0], N[0]), dTf_qpi(N[0], N[0], N[0]), dTp_qpi(N[0], N[0]), v(N[0], D), nv(N[0]);
        in.N = N[0], in.f = f, in.D_h = dh(e), in.D_ch = dh(e), in.alpha = &alpha(e, 0), in.T = &v_aux[0](f, 0), in.p = press(e), in.v = nv.addr(), in.lambda = &lambda(e, 0), in.mu = &mu(e, 0), in.rho =
                                                                                                                          &rho(e, 0), in.Cp = &Cp(e, 0);
        out.qpk = &qpk, out.dTf_qpk = &dTf_qpk, out.dTp_qpk = &dTp_qpk, out.qpi = &qpi, out.dTf_qpi = &dTf_qpi, out.dTp_qpi = &dTp_qpi;
        for (e = f_e[0](f, 0), i = 0; i < e_f[0].get().dimension(1) && (fb = e_f[0](e, i)) >= 0; i++)
          for (prefac = fs[0](fb) * pf[0](fb) * (e == f_e[0](fb, 0) ? 1 : -1) / (pe[0](e) * ve[0](e)), n = 0; n < N[0]; n++)
            for (fac = prefac * vit(fb, n), d = 0; d < D; d++)
              v(n, d) += fac * (xv[0](fb, d) - xp[0](e, d));
        for (n = 0; n < N[0]; n++)
          nv(n) = sqrt(domaine[0].get().dot(&v(n, 0), &v(n, 0)));
        //Tparoi : estimation initiale + Newton si on ne la connait pas
        double h_imp = fcl[0](f, 0) == 1 ? ref_cast(Echange_impose_base, cls[0].get()[fcl[0](f, 1)].valeur()).h_imp(fcl[0](f, 2), 0) : 0, T_ext =
                         fcl[0](f, 0) == 1 ? ref_cast(Echange_impose_base, cls[0].get()[fcl[0](f, 1)].valeur()).T_ext(fcl[0](f, 2), 0) : 0, dTp, FT, dFTp;
        in.Tp = ech ? v_aux[o_p](o_f, 0) : fcl[0](f, 0) == 5 ? 0 : fcl[0](f, 0) == 6 ? ref_cast(Dirichlet, cls[0].get()[fcl[0](f, 1)].valeur()).val_imp(fcl[0](f, 2), 0) :
                fcl[0](f, 0) == 1 ? T_ext : v_aux[0](f, 0);
        //appel : on n'est implicite qu'en les temperatures
        dTp = 0;
        for (int it = 0; it < 10 && (!it || std::abs(dTp) > 1e-5); in.Tp += dTp, it++)
          {
            corr[0]->qp(in, out);
            for (FT = 0, dFTp = 0; n < N[0]; n++)
              FT += qpk(n), dFTp += dTp_qpk(n);
            dTp = fcl[0](f, 0) == 5 ? (ref_cast(Neumann, cls[0].get()[fcl[0](f, 1)].valeur()).flux_impose(fcl[0](f, 2), 0) - FT) / dFTp :
                  fcl[0](f, 0) == 1 ? (h_imp * (T_ext - in.Tp) - FT) / (dFTp + h_imp) : 0;
          }
        for (n = 0; n < N[0]; n++)
          secmem(!aux_only * ne_tot[0] + f, n) += fs[0](f) * qpk(n); //second membre
        if (mat[0])
          for (k1 = 0; k1 < N[0]; k1++)
            for (k2 = 0; k2 < N[0]; k2++) //derivees en Tfluide
              (*mat[0])(N[0] * (!aux_only * ne_tot[0] + f) + k1, N[0] * (!aux_only * ne_tot[0] + f) + k2) -= fs[0](f) * dTf_qpk(k1, k2);
        if (ech && mat[o_p])
          for (n = 0; n < N[0]; n++) /* derivees en Tparoi (si Echange_contact) */
            (*mat[o_p])(N[0] * (!aux_only * ne_tot[0] + f) + n, !aux_only * ne_tot[o_p] + o_f) -= fs[0](f) * dTp_qpk(n);
      }
    else if (fcl[0](f, 0) > 5)
      for (n = 0; n < N[0]; n++) //Dirichlet : T_f^+ = T_b
        {
          secmem(!aux_only * ne_tot[0] + f, n) += (fcl[0](f, 0) == 6 ? ref_cast(Dirichlet, cls[0].get()[fcl[0](f, 1)].valeur()).val_imp(fcl[0](f, 2), n) : 0) - v_aux[0](f, n);
          if (mat[0])
            (*mat[0])(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[0] + f) + n)++;
        }
    else if (fcl[0](f, 0) == 3) //Echange_contact
      {
        const Echange_contact_PolyMAC_P0P1NC& ech = ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[fcl[0](f, 1)].valeur());
        int o_p = ech.o_idx, o_f = ech.f_dist(fcl[0](f, 2)), o_e = f_e[o_p](o_f, 0); //autre pb/face/elem
        if (corr[o_p] || N[o_p] != N[0]) /* correlation de l'autre cote */
          {
            const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, op_ext[o_p]->equation().probleme());
            const DoubleTab& alpha = pbm.equation_masse().inconnue()->passe(), &dh = pbm.milieu().diametre_hydraulique_elem(),
                             &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression()->passe(), &vit = pbm.equation_qdm().inconnue()->passe(),
                              &lambda = pbm.milieu().conductivite()->passe(), &mu = ref_cast(Fluide_base, pbm.milieu()).viscosite_dynamique()->passe(), &rho = pbm.milieu().masse_volumique()->passe(), &Cp =
                                                                                      pbm.milieu().capacite_calorifique()->passe();
            DoubleTrav qpk(N[o_p]), dTf_qpk(N[o_p], N[o_p]), dTp_qpk(N[o_p]), qpi(N[o_p], N[o_p]), dTf_qpi(N[o_p], N[o_p], N[o_p]), dTp_qpi(N[o_p], N[o_p]), v(N[o_p], D), nv(N[o_p]);
            for (i = 0; i < e_f[o_p].get().dimension(1) && (fb = e_f[o_p](o_e, i)) >= 0; i++)
              for (prefac = fs[o_p](fb) * pf[o_p](fb) * (o_e == f_e[o_p](fb, 0) ? 1 : -1) / (pe[o_p](o_e) * ve[o_p](o_e)), n = 0; n < N[o_p]; n++)
                for (fac = prefac * vit(fb, n), d = 0; d < D; d++)
                  v(n, d) += fac * (xv[o_p](fb, d) - xp[o_p](o_e, d));
            for (n = 0; n < N[o_p]; n++)
              nv(n) = sqrt(domaine[0].get().dot(&v(n, 0), &v(n, 0)));
            //appel : on n'est implicite qu'en les temperatures
            Flux_parietal_base::input_t in;
            Flux_parietal_base::output_t out;

            in.N = N[o_p], in.f = o_f, in.D_h = dh(o_e), in.D_ch = dh(o_e), in.alpha = &alpha(o_e, 0), in.T = &v_aux[o_p](o_f, 0), in.p = press(e), in.v = nv.addr(), in.Tp = v_aux[0](f, 0), in.lambda =
                                                                                                                &lambda(o_e, 0), in.mu = &mu(o_e, 0), in.rho = &rho(o_e, 0), in.Cp = &Cp(o_e, 0);
            out.qpk = &qpk, out.dTf_qpk = &dTf_qpk, out.dTp_qpk = &dTp_qpk, out.qpi = &qpi, out.dTf_qpi = &dTf_qpi, out.dTp_qpi = &dTp_qpi, out.nonlinear = &j;
            corr[o_p]->qp(in, out);
            for (n = 0; n < N[o_p]; n++)
              secmem(!aux_only * ne_tot[0] + f, 0) -= fs[0](f) * qpk(n); //second membre
            if (mat[o_p])
              for (k1 = 0; k1 < N[o_p]; k1++)
                for (k2 = 0; k2 < N[o_p]; k2++) //derivees en Tfluide
                  (*mat[o_p])(!aux_only * ne_tot[0] + f, N[o_p] * (!aux_only * ne_tot[o_p] + o_f) + k2) += fs[0](f) * dTf_qpk(k1, k2);
            if (mat[0])
              for (n = 0; n < N[o_p]; n++) /* derivees en Tparoi */
                (*mat[0])(!aux_only * ne_tot[0] + f, !aux_only * ne_tot[0] + f) += fs[0](f) * dTp_qpk(n);
          }
        else if (ech.invh_paroi)
          for (n = 0; n < N[0]; n++)/* resistance de la paroi -> ajout du h(T'-T) */
            {
              secmem(!aux_only * ne_tot[0] + f, n) -= fs[0](f) / ech.invh_paroi * (v_aux[0](f, n) - v_aux[o_p](o_f, n)); //second membre
              if (mat[0])
                (*mat[0])(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[0] + f) + n) += fs[0](f) / ech.invh_paroi; //derivees
              if (mat[o_p])
                (*mat[o_p])(N[0] * (!aux_only * ne_tot[0] + f) + n, N[o_p] * (!aux_only * ne_tot[o_p] + o_f) + n) -= fs[0](f) / ech.invh_paroi;
            }
        else
          for (domaine[o_p].get().W2(&diffu[o_p].get(), o_e, w2), i = 0; i < w2.dimension(0); i++) /* pas de resistance -> ajout du flux de l'autre cote */
            if (e_f[o_p](o_e, i) == o_f)
              for (j = 0; j < w2.dimension(1); j++)
                for (n = 0; n < N[0]; n++)
                  if (w2(i, j, n))
                    {
                      secmem(!aux_only * ne_tot[0] + f, n) -= w2(i, j, n) * (v_aux[o_p * (j != i)](j != i ? e_f[o_p](o_e, j) : f, n) - inco[o_p](o_e, n)); //second membre
                      if (mat[o_p * (j != i)] && (j == i || fcl[o_p](o_f, 0) < 6)) //autre face: on substitue T_fb par T_f
                        (*mat[o_p * (j != i)])(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[o_p * (j != i)] + (j != i ? e_f[o_p](o_e, j) : f)) + n) += w2(i, j, n);
                      if (!aux_only && mat[o_p])
                        (*mat[o_p])(N[0] * (ne_tot[0] + f) + n, N[0] * o_e + n) -= w2(i, j, n); //autre element
                    }
      }
    else if (fcl[0](f, 0) == 4)
      for (n = 0; n < N[0]; n++) //Neumann
        secmem(!aux_only * ne_tot[0] + f, n) -= fs[0](f) * ref_cast(Neumann, cls[0].get()[fcl[0](f, 1)].valeur()).flux_impose(fcl[0](f, 2), n);
    else if (fcl[0](f, 0) && fcl[0](f, 0) < 3)
      for (e = f_e[0](f, 0), n = 0; n < N[0]; n++) //Echange_global_impose
        {
          const Echange_impose_base& ech = ref_cast(Echange_impose_base, cls[0].get()[fcl[0](f, 1)].valeur());
          double h = ech.h_imp(fcl[0](f, 2), n), T = ech.T_ext(fcl[0](f, 2), n);
          secmem(!aux_only * ne_tot[0] + f, n) -= fs[0](f) * h * ((fcl[0](f, 0) == 1 ? v_aux[0](f, n) : inco[0](e, n)) - T);
          if (mat[0] && fcl[0](f, 0) == 1)
            (*mat[0])(N[0] * (!aux_only * ne_tot[0] + f) + n, N[0] * (!aux_only * ne_tot[0] + f) + n) += h * fs[0](f);
          if (mat[0] && fcl[0](f, 0) == 2 && !aux_only)
            (*mat[0])(N[0] * (ne_tot[0] + f) + n, N[0] * e + n) += h * fs[0](f);
        }
  statistiques().end_count(diffusion_counter_);
}
