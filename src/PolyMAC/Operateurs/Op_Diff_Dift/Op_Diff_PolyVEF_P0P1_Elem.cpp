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

#include <Convection_Diffusion_Temperature.h>
#include <Modele_turbulence_scal_base.h>
#include <Echange_contact_PolyMAC_P0P1NC.h>
#include <Echange_externe_impose.h>
#include <Op_Diff_PolyVEF_P0P1_Elem.h>
#include <Dirichlet_homogene.h>
#include <Schema_Temps_base.h>
#include <Champ_Elem_PolyVEF_P0P1.h>
#include <Champ_front_calc.h>
#include <Domaine_Cl_PolyMAC.h>
#include <MD_Vector_base.h>
#include <communications.h>
#include <Synonyme_info.h>
#include <Pb_Multiphase.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Domaine_PolyVEF.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <cmath>
#include <deque>
#include <functional>
#include <Flux_parietal_base.h>
#include <TRUSTTab_parts.h>

Implemente_instanciable_sans_constructeur(Op_Diff_PolyVEF_P0P1_Elem, "Op_Diff_PolyVEF_P0P1_Elem|Op_Diff_PolyVEF_P0P1_var_Elem", Op_Diff_PolyMAC_P0P1NC_Elem);

Op_Diff_PolyVEF_P0P1_Elem::Op_Diff_PolyVEF_P0P1_Elem()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_PolyVEF_P0P1_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_PolyMAC_P0P1NC_base::printOn( os );
}

Entree& Op_Diff_PolyVEF_P0P1_Elem::readOn( Entree& is )
{
  return Op_Diff_PolyMAC_P0P1NC_base::readOn( is );
}

void Op_Diff_PolyVEF_P0P1_Elem::completer()
{
  Op_Diff_PolyMAC_P0P1NC_base::completer();
  Equation_base& eq = equation();
  Champ_Elem_PolyVEF_P0P1& ch = ref_cast(Champ_Elem_PolyVEF_P0P1, le_champ_inco.non_nul() ? le_champ_inco.valeur() : eq.inconnue());
  ch.init_auxiliary_variables();
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  if (dom.nb_joints() && dom.joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyVEF_P0P1_Elem : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  flux_bords_.resize(dom.premiere_face_int(), ch.valeurs().line_size());
}

double Op_Diff_PolyVEF_P0P1_Elem::calculer_dt_stab() const
{
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  const IntTab& e_f = dom.elem_faces();
  const DoubleTab& nf = dom.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr,
                    &diffu = diffusivite_pour_pas_de_temps().valeurs(), &lambda = diffusivite().valeurs(), &diffu_tot = nu();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &vf = dom.volumes_entrelaces(), &ve = dom.volumes();

  int i, e, f, n, m, N = equation().inconnue().valeurs().dimension(1), cD = diffu.dimension(0) == 1, cL = lambda.dimension(0) == 1, multi = multiscalar_;
  double dt = 1e10;
  DoubleTrav flux(N);
  for (e = 0; e < dom.nb_elem(); e++)
    {
      for (flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          for (m = (multi ? 0 : n); m < (multi ? N : n + 1); m++)
            flux(n) += dom.nu_dot(&diffu_tot, e, n, &nf(f, 0), &nf(f, 0), nullptr, nullptr, multi ? m : -1) / vf(f);
      for (n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 1e-3) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = std::min(dt, pe(e) * ve(e) * (alp ? (*alp)(e, n) : 1) * (lambda(!cL * e, n) / diffu(!cD * e, n)) / flux(n));
      if (dt < 0) abort();
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyVEF_P0P1_Elem::dimensionner_blocs_ext(int aux_only, matrices_t matrices, const tabs_t& semi_impl) const
{
  init_op_ext();
  const std::string& nom_inco = (le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue()).le_nom().getString();
  int i, j, k, e, eb, o_e, s = 0, sb = 0, o_s, f, o_f, m, n, M, p, n_ext = (int)op_ext.size(), semi = (int) semi_impl.count(nom_inco), multi = multiscalar_;
  std::vector<Matrice_Morse *> mat(n_ext); //matrices
  std::vector<int> N, ne_tot; //composantes, nombre d'elements total par pb
  std::vector<std::reference_wrapper<const Domaine_PolyVEF>> dom; //domaines
  std::vector<std::reference_wrapper<const Conds_lim>> cls; //conditions aux limites
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_e, f_s, scl_d, scl_c; //tableaux "fcl", "elem_faces", "faces_voisins"
  std::vector<std::reference_wrapper<const DoubleTab>> diffu, inco; //inconnues, normales aux faces, positions elems / faces / sommets
  std::deque<ConstDoubleTab_parts> v_part; //blocs de chaque inconnue
  std::vector<IntTrav> stencil(n_ext); //stencils par matrice
  for (i = 0, M = 0; i < n_ext; M = std::max(M, N[i]), i++)
    {
      std::string nom_mat = i ? nom_inco + "/" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : nullptr;
      dom.push_back(std::ref(ref_cast(Domaine_PolyVEF, op_ext[i]->equation().domaine_dis())));
      f_e.push_back(std::ref(dom[i].get().face_voisins())), e_f.push_back(std::ref(dom[i].get().elem_faces())), f_s.push_back(std::ref(dom[i].get().face_sommets()));
      cls.push_back(std::ref(op_ext[i]->equation().domaine_Cl_dis().les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyVEF_P0P1_Elem, *op_ext[i]).nu());
      const Champ_Elem_PolyVEF_P0P1& ch = ref_cast(Champ_Elem_PolyVEF_P0P1, op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue() : op_ext[i]->equation().inconnue());
      scl_d.push_back(std::ref(ch.scl_d(0))), scl_c.push_back(std::ref(ch.scl_c(0)));
      N.push_back(ch.valeurs().line_size()), fcl.push_back(std::ref(ch.fcl())), ne_tot.push_back(dom[i].get().nb_elem_tot());
      inco.push_back(ch.valeurs()), v_part.emplace_back(ch.valeurs());
      stencil[i].resize(0, 2);
    }
  /* probleme local */
  if (!semi)
    for (f = 0; f < dom[0].get().nb_faces_tot(); f++)
      for (i = 0; i < 2 + f_s[0].get().dimension(1); i++)
        if (i < 2 ? !aux_only && (e = f_e[0](f, i)) >= 0 && e < dom[0].get().nb_elem() : (s = f_s[0](f, i - 2)) >= 0 && s < dom[0].get().domaine().nb_som() && scl_d[0](s) == scl_d[0](s + 1))
          for (j = 0; j < 2 + f_s[0].get().dimension(1); j++)
            if (j < 2 ? !aux_only && (eb = f_e[0](f, j)) >= 0 : (sb = f_s[0](f, j - 2)) >= 0 && scl_d[0](sb) == scl_d[0](sb + 1))
              for (n = 0; n < N[0]; n++)
                for (m = (multi ? 0 : n); m < (multi ? N[0] : n + 1); m++)
                  stencil[0].append_line(N[0] * (i < 2 ? e : !aux_only * ne_tot[0] + s) + n, N[0] * (j < 2 ? eb : !aux_only * ne_tot[0] + sb) + m);
  /* lignes aux sommets */
  for (s = 0; s < dom[0].get().nb_som(); s++)
    for (n = 0; n < N[0]; n++)
      for (m = (multi ? 0 : n); m < (multi ? N[0] : n + 1); m++)
        stencil[0].append_line(N[0] * (!aux_only * ne_tot[0] + s) + n, N[0] * (!aux_only * ne_tot[0] + s) + m);

  /* problemes distants : pour les Echange_contact */
  const Echange_contact_PolyMAC_P0P1NC *pcl;
  if (!semi)
    for (i = 0; i < cls[0].get().size(); i++)
      if ((pcl = sub_type(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[i].valeur()) ? &ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[i].valeur()) : nullptr))
        for (pcl->init_fs_dist(), j = 0, p = (int) (std::find(op_ext.begin(), op_ext.end(), &pcl->o_diff.valeur()) - op_ext.begin()); j < pcl->fvf->nb_faces_tot(); j++)
          for (f = pcl->fvf->num_face(j), o_f = pcl->f_dist(j), k = 0; k < f_s[0].get().dimension(1) && (s = f_s[0](f, k)) >= 0; k++)
            if (s < dom[0].get().nb_som() && scl_d[0](s) == scl_d[0](s + 1))
              for (o_s = pcl->s_dist[s], o_e = f_e[p](o_f, 0), n = 0; n < N[0]; n++)
                for (m = 0; m < N[p]; m++)
                  {
                    stencil[p].append_line(N[0] * (!aux_only * ne_tot[0] + s) + n, N[p] * (!aux_only * ne_tot[p] + o_s) + m);
                    if (!aux_only) stencil[p].append_line(N[0] * (!aux_only * ne_tot[0] + s) + n, N[p] * o_e + m);
                  }

  for (i = 0; i < n_ext; i++)
    if (mat[i])
      {
        tableau_trier_retirer_doublons(stencil[i]);
        Matrice_Morse mat2;
        Matrix_tools::allocate_morse_matrix(aux_only ? v_part[0][1].size_totale() : inco[0].get().size_totale(), aux_only ? v_part[i][1].size_totale() : inco[i].get().size_totale(), stencil[i], mat2);
        mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
      }
}

void Op_Diff_PolyVEF_P0P1_Elem::ajouter_blocs_ext(int aux_only, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  init_op_ext();
  const std::string& nom_inco = (le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue()).le_nom().getString();
  int i, ib, j, k, l, m, e, eb, f, s = 0, sb = 0, n, nb, M, n_ext = (int)op_ext.size(), semi = (int)semi_impl.count(nom_inco), d, D = dimension, n_sf, nt, ok, multi = multiscalar_;
  std::vector<Matrice_Morse *> mat(n_ext); //matrices
  std::vector<int> N, ne_tot; //composantes
  std::vector<std::reference_wrapper<const Domaine_PolyVEF>> dom; //zones
  std::vector<std::reference_wrapper<const Conds_lim>> cls; //conditions aux limites
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_e, f_s, scl_d, scl_c; //tableaux "fcl", "elem_faces", "faces_voisins"
  std::vector<std::reference_wrapper<const DoubleVect>> fs, vf; //surfaces
  std::vector<std::reference_wrapper<const DoubleTab>> inco, xp, xv, xs, nf, diffu, v_aux; //inconnues, normales aux faces, positions elems / faces / sommets
  std::deque<ConstDoubleTab_parts> v_part; //blocs de chaque inconnue
  std::vector<const Flux_parietal_base *> corr; //correlations de flux parietal (si elles existent)
  for (i = 0, M = 0; i < n_ext; M = std::max(M, N[i]), i++)
    {
      std::string nom_mat = i ? nom_inco + "/" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : nullptr;
      dom.push_back(std::ref(ref_cast(Domaine_PolyVEF, op_ext[i]->equation().domaine_dis())));
      f_e.push_back(std::ref(dom[i].get().face_voisins())), e_f.push_back(std::ref(dom[i].get().elem_faces())), f_s.push_back(std::ref(dom[i].get().face_sommets()));
      fs.push_back(std::ref(dom[i].get().face_surfaces())), nf.push_back(std::ref(dom[i].get().face_normales()));
      xp.push_back(std::ref(dom[i].get().xp())), xv.push_back(std::ref(dom[i].get().xv())), xs.push_back(std::ref(dom[i].get().domaine().coord_sommets()));
      vf.push_back(std::ref(dom[i].get().volumes_entrelaces()));
      cls.push_back(std::ref(op_ext[i]->equation().domaine_Cl_dis().les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyVEF_P0P1_Elem, *op_ext[i]).nu());
      const Champ_Elem_PolyVEF_P0P1& ch = ref_cast(Champ_Elem_PolyVEF_P0P1, op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue() : op_ext[i]->equation().inconnue());
      scl_d.push_back(std::ref(ch.scl_d(0))), scl_c.push_back(std::ref(ch.scl_c(0)));
      inco.push_back(std::ref(semi_impl.count(nom_mat) ? semi_impl.at(nom_mat) : ch.valeurs())), v_part.emplace_back(inco.back());
      corr.push_back((sub_type(Energie_Multiphase, op_ext[i]->equation()) || sub_type(Convection_Diffusion_Temperature, op_ext[i]->equation())) && op_ext[i]->equation().probleme().has_correlation("flux_parietal")
                     ? &ref_cast(Flux_parietal_base, op_ext[i]->equation().probleme().get_correlation("flux_parietal")) : nullptr);
      N.push_back(inco[i].get().line_size()), ne_tot.push_back(dom[i].get().nb_elem_tot()), fcl.push_back(std::ref(ch.fcl()));
    }

  /* que faire avec les variables auxiliaires ? */
  double t = equation().schema_temps().temps_courant(), dt = equation().schema_temps().pas_de_temps(), fac, prefac;
  if (aux_only) use_aux_ = 0; /* 1) on est en train d'assembler le systeme de resolution des variables auxiliaires lui-meme */
  else if (mat[0] && !semi) t_last_aux_ = t + dt, use_aux_ = 0; /* 2) on est en implicite complet : pas besoin de mat_aux / var_aux, on aura les variables a t + dt */
  else if (t_last_aux_ < t) update_aux(t); /* 3) premier pas a ce temps en semi-implicite : on calcule les variables auxiliaires a t et on les stocke dans var_aux */
  for (i = 0; i < n_ext; i++) v_aux.push_back(use_aux_ ? ref_cast(Op_Diff_PolyVEF_P0P1_Elem, *op_ext[i]).var_aux : v_part[i][1]); /* les variables auxiliaires peuvent etre soit dans inco/semi_impl (cas 1), soit dans var_aux (cas 2) */

  DoubleTrav Sa, Gf, Gfa, A; //surface par arete du diamant, gradient non stabilise (par diamant) et stabilise (par arete du diamant), forme bilineaire
  flux_bords_ = 0;
  for (f = 0; f < dom[0].get().nb_faces_tot(); f++)
    {
      for (ok = 0, i = 0; i < 2 + f_s[0].get().dimension(1); i++) /* si aucun element ou sommet reel autour de f, rien a faire */
        ok |= i < 2 ? (e = f_e[0](f, i)) >= 0 && e < dom[0].get().nb_elem() : (s = f_s[0](f, i - 2)) >= 0 && s < dom[0].get().nb_som() && scl_d[0](s) == scl_d[0](s + 1);
      if (!ok) continue;
      dom[0].get().surf_elem_som(f, Sa), n_sf = Sa.dimension(0), nt = 2 + n_sf, Gf.resize(nt, D), Gfa.resize(n_sf, D, nt, D), A.resize(nt, nt, N[0] * (multi ? N[0] : 1));

      /* Gf : gradient non stabilise dans le diamant */
      for (Gf = 0, i = 0; i < n_sf; i++)
        for (j = 0; j < D; j++)
          if (j == 2) //partie s -> sb (3D)
            for (ib = i + 1 < n_sf ? i + 1 : 0, d = 0; d < D; d++) Gf(2 + ib, d) += Sa(i, j, d), Gf(2 + i, d) -= Sa(i, j, d);
          else if (f_e[0](f, j) >= 0)
            for (d = 0; d < D; d++) Gf(j, d) += Sa(i, j, d), Gf(2 + i, d) -= Sa(i, j, d); //parties s -> e
      Gf /= vf[0](f);

      /* Gfa : gradient stabilise pour chaque arete du diamant */
      double /*dxn = dom[0].get().dot(f_e[0](f, 1) >= 0 ? &xp[0](f_e[0](f, 1), 0) : &xv[0](f, 0), &nf[0](f, 0), &xp[0](f_e[0](f, 0), 0)) / fs[0](f),
             dx2 = dom[0].get().dot(f_e[0](f, 1) >= 0 ? &xp[0](f_e[0](f, 1), 0) : &xv[0](f, 0), f_e[0](f, 1) >= 0 ? &xp[0](f_e[0](f, 1), 0) : &xv[0](f, 0), &xp[0](f_e[0](f, 0), 0), &xp[0](f_e[0](f, 0), 0)),
             rap = dxn * dxn / dx2, *///vaut 1 si decouplage
      beta = 1. / D;//1 / sqrt(D) * std::min(std::max(1. + 10. * (rap - 1.), 0.), 1.);
      for (i = 0; i < n_sf; i++)
        for (s = f_s[0](f, i), ib = i + 1 < n_sf ? i + 1 : 0, sb = D < 3 ? -1 : f_s[0](f, ib), j = 0; j < D; j++)
          if (j == 2 || (e = f_e[0](f, j)) >= 0)
            for (prefac = D * beta / dom[0].get().dot(j < 2 ? &xp[0](e, 0) : &xs[0](sb, 0), &Sa(i, j, 0), &xs[0](s, 0)), k = 0; k < nt; k++)
              for (fac = prefac * ((k == (j < 2 ? j : 2 + ib)) - (k == 2 + i) - dom[0].get().dot(j < 2 ? &xp[0](e, 0) : &xs[0](sb, 0), &Gf(k, 0), &xs[0](s, 0))), d = 0; d < D; d++)
                Gfa(i, j, k, d) = Gf(k, d) + fac * Sa(i, j, d);

      /* forme bilineaire A : Gfa.lambda.Gfa sur chaque arete du diamant */
      for (A = 0, i = 0; i < n_sf; i++)
        for (s = f_s[0](f, i), sb = D < 3 ? -1 : f_s[0](f, i + 1 < n_sf ? i + 1 : 0), j = 0; j < 2 && (e = f_e[0](f, j)) >= 0; j++)
          for (k = 0; k < D - 1; k++) //k = 0 -> partie s->e, k = 1 -> partie s->sb du cote de e
            for (fac = dom[0].get().dot(k ? &xs[0](sb, 0) : &xp[0](e, 0), &Sa(i, k ? 3 + j : j, 0), &xs[0](s, 0)) / D, l = 0; l < nt; l++)
              for (m = 0; m <= l; m++)
                for (n = 0; n < N[0]; n++)
                  for (nb = (multi ? 0 : n); nb < (multi ? N[0] : n + 1); nb++)
                    A(l, m, multi ? N[0] * n + nb : n) += fac * dom[0].get().nu_dot(&diffu[0].get(), e, n, &Gfa(i, k ? 2 : j, l, 0), &Gfa(i, k ? 2 : j, m, 0), nullptr, nullptr, multi ? nb : -1);
      for (i = 0; i < nt; i++) //symetrisation
        for (j = i + 1; j < nt; j++)
          for (n = 0; n < N[0] * (multi ? N[0] : 1); n++)
            A(i, j, n) = A(j, i, n);

      /* contributions */
      for (i = 0; i < nt; i++)
        if (i < 2 ? !aux_only && (e = f_e[0](f, i)) >= 0 && e < dom[0].get().nb_elem() : (s = f_s[0](f, i - 2)) >= 0 && s < dom[0].get().domaine().nb_som() && scl_d[0](s) == scl_d[0](s + 1))
          for (j = 0; j < nt; j++)
            if (j < 2 ? (eb = f_e[0](f, j)) >= 0 : (sb = f_s[0](f, j - 2)) >= 0)
              for (n = 0; n < N[0]; n++)
                for (m = (multi ? 0 : n); m < (multi ? N[0] : n + 1); m++)
                  {
                    secmem(i < 2 ? e : !aux_only * ne_tot[0] + s, n) -= A(i, j, multi ? N[0] * n + m : n) * (j < 2 ? inco[0](eb, m) : v_aux[0](sb, m));
                    if (mat[0] && !semi && (j < 2 ? !aux_only : scl_d[0](sb) == scl_d[0](sb + 1)))
                      (*mat[0])(N[0] * (i < 2 ? e : !aux_only * ne_tot[0] + s) + n, N[0] * (j < 2 ? eb : !aux_only * ne_tot[0] + sb) + m) += A(i, j, multi ? N[0] * n + m : n);
                  }

      /* CLs non Dirichlet -> injection aux sommets */
      if (fcl[0](f, 0) && fcl[0](f, 0) < 5)
        for (e = f_e[0](f, 0), i = 0; i < n_sf; i++)
          if ((s = f_s[0](f, i)) < dom[0].get().domaine().nb_som() && scl_d[0](s) == scl_d[0](s + 1))
            {
              double surf = dom[0].get().dot(&nf[0](f, 0), &Sa(i, 1, 0)) / fs[0](f); //partie de la surface vers le sommet
              if (fcl[0](f, 0) == 1 || fcl[0](f, 0) == 2) //Echange_{externe,global}_impose
                for (n = 0; n < N[0]; n++)
                  for (m = (multi ? 0 : n); m < (multi ? N[0] : n + 1); m++)
                    {
                      const Echange_impose_base& ech = ref_cast(Echange_impose_base, cls[0].get()[fcl[0](f, 1)].valeur());
                      double sh = surf * ech.h_imp(fcl[0](f, 2), multi ? N[0] * n + m : n), T = ech.T_ext(fcl[0](f, 2), m);
                      secmem(!aux_only * ne_tot[0] + s, n) -= sh * ((fcl[0](f, 0) == 1 ? v_aux[0](s, m) : inco[0](e, m)) - T);
                      if (f < dom[0].get().premiere_face_int())
                        flux_bords_(f, n) += sh * ((fcl[0](f, 0) == 1 ? v_aux[0](s, m) : inco[0](e, m)) - T);
                      if (mat[0] && (fcl[0](f, 0) == 1 || !aux_only) && !semi)
                        (*mat[0])(N[0] * (!aux_only * ne_tot[0] + s) + n, N[0] * (fcl[0](f, 0) == 1 ? !aux_only * ne_tot[0] + s : e) + m) += sh;
                    }
              else if (corr[0]) //Pb_Multiphase avec flux parietal -> on ne traite (pour le moment) que Dirichlet et Echange_contact
                {
                  if ((fcl[0](f, 0) != 6 && fcl[0](f, 0) != 7 && fcl[0](f, 0) != 3) || multi)
                    abort(); //cas non geres
                  const Echange_contact_PolyMAC_P0P1NC *ech = fcl[0](f, 0) == 3 ? &ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[fcl[0](f, 1)].valeur()) : nullptr;
                  int o_p = ech ? ech->o_idx : -1, o_s = ech ? ech->s_dist[s] : -1, k1, k2, wall = corr[0]->T_at_wall();
                  const Probleme_base& pbm = equation().probleme();
                  const DoubleTab* alpha = sub_type(Pb_Multiphase, pbm) ? &ref_cast(Pb_Multiphase, pbm).equation_masse().inconnue().passe() : nullptr, &dh = pbm.milieu().diametre_hydraulique_elem(),
                                   &press = ref_cast(Navier_Stokes_std, pbm.equation(0)).pression().passe(), &vit = pbm.equation(0).inconnue().passe(),
                                    &lambda = pbm.milieu().conductivite().passe(), &mu = ref_cast(Fluide_base, pbm.milieu()).viscosite_dynamique().passe(),
                                     &rho = pbm.milieu().masse_volumique().passe(), &Cp = pbm.milieu().capacite_calorifique().passe();
                  const int Clambda = lambda.dimension(0) == 1, Cmu = mu.dimension(0) == 1, Crho = rho.dimension(0) == 1, Ccp = Cp.dimension(0) == 1;

                  Flux_parietal_base::input_t in;
                  Flux_parietal_base::output_t out;
                  DoubleTrav qpk(N[0]), dTf_qpk(N[0], N[0]), dTp_qpk(N[0]), qpi(N[0], N[0]), dTf_qpi(N[0], N[0], N[0]), dTp_qpi(N[0], N[0]), v(N[0], D), nv(N[0]);
                  in.N = N[0], in.f = f, in.D_h = dh(e), in.D_ch = dh(e), in.alpha = alpha ? &(*alpha)(e, 0) : nullptr, in.T = wall ? &v_aux[0](s, 0) : &inco[0](e, 0), in.p = press(e), in.v = nv.addr();
                  in.y = dom[0].get().dist_norm_bord(f) / 2; //ok?
                  in.lambda = &lambda(!Clambda * e, 0), in.mu = &mu(!Cmu * e, 0), in.rho = &rho(!Crho * e, 0), in.Cp = &Cp(!Ccp * e, 0);
                  in.Tp = ech ? v_aux[o_p](o_s, 0) : fcl[0](f, 0) == 6 ? ref_cast(Dirichlet, cls[0].get()[fcl[0](f, 1)].valeur()).val_imp(fcl[0](f, 2), 0) : 0;
                  out.qpk = &qpk, out.dTf_qpk = &dTf_qpk, out.dTp_qpk = &dTp_qpk, out.qpi = &qpi, out.dTf_qpi = &dTf_qpi, out.dTp_qpi = &dTp_qpi;
                  for (d = 0; d < D; d++)
                    for (n = 0; n < N[0]; n++)
                      nv(n) += vit(f, N[0] * d + n) * vit(f, N[0] * d + n);
                  for (n = 0; n < N[0]; n++) nv(n) = sqrt(nv[n]);
                  //appel : on n'est implicite qu'en les temperatures
                  corr[0]->qp(in, out);
                  //remplissage
                  for (n = 0; n < N[0]; n++)
                    secmem(!aux_only * ne_tot[0] + s, n) += surf * qpk(n); //second membre
                  if (f < dom[0].get().premiere_face_int())
                    for (n = 0; n < N[0]; n++)
                      flux_bords_(f, n) -= surf * qpk(n);
                  if (mat[0] && !semi && (wall || !aux_only))
                    for (k1 = 0; k1 < N[0]; k1++)
                      for (k2 = 0; k2 < N[0]; k2++) //derivees en Tfluide
                        (*mat[0])(N[0] * (!aux_only * ne_tot[0] + s) + k1, N[0] * (wall ? !aux_only * ne_tot[0] + s : e) + k2) -= surf * dTf_qpk(k1, k2);
                  if (ech && mat[o_p] && !semi)
                    for (n = 0; n < N[0]; n++) /* derivees en Tparoi (si Echange_contact) */
                      (*mat[o_p])(N[0] * (!aux_only * ne_tot[0] + s) + n, !aux_only * ne_tot[o_p] + o_s) -= surf * dTp_qpk(n);
                }
              else if (fcl[0](f, 0) == 3) //Echange_contact
                {
                  const Echange_contact_PolyMAC_P0P1NC& ech = ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[0].get()[fcl[0](f, 1)].valeur());
                  int o_p = ech.o_idx, o_f = ech.f_dist(fcl[0](f, 2)), o_e = f_e[o_p](o_f, 0), o_s = ech.s_dist[s], k1, k2; //autre pb/face/elem
                  if (corr[o_p] || N[o_p] != N[0]) /* correlation de l'autre cote */
                    {
                      if (multi) abort(); //pas gere
                      const Probleme_base& pbm = op_ext[o_p]->equation().probleme();
                      const DoubleTab* alpha = sub_type(Pb_Multiphase, pbm) ? &ref_cast(Pb_Multiphase, pbm).equation_masse().inconnue().passe() : nullptr, &dh = pbm.milieu().diametre_hydraulique_elem(),
                                       &press = ref_cast(Navier_Stokes_std, pbm.equation(0)).pression().passe(), &vit = pbm.equation(0).inconnue().passe(),
                                        &lambda = pbm.milieu().conductivite().passe(), &mu = ref_cast(Fluide_base, pbm.milieu()).viscosite_dynamique().passe(),
                                         &rho = pbm.milieu().masse_volumique().passe(), &Cp = pbm.milieu().capacite_calorifique().passe();
                      const int Clambda = lambda.dimension(0) == 1, Cmu = mu.dimension(0) == 1, Crho = rho.dimension(0) == 1, Ccp = Cp.dimension(0) == 1;

                      DoubleTrav qpk(N[o_p]), dTf_qpk(N[o_p], N[o_p]), dTp_qpk(N[o_p]), qpi(N[o_p], N[o_p]), dTf_qpi(N[o_p], N[o_p], N[o_p]), dTp_qpi(N[o_p], N[o_p]), nv(N[o_p]);
                      for (d = 0; d < D; d++)
                        for (n = 0; n < N[o_p]; n++)
                          nv(n) += vit(o_f, N[o_p] * d + n) * vit(o_f, N[o_p] * d + n);
                      for (n = 0; n < N[o_p]; n++) nv(n) = sqrt(nv[n]);
                      Flux_parietal_base::input_t in;
                      Flux_parietal_base::output_t out;
                      int wall = corr[o_p]->T_at_wall();
                      in.N = N[o_p], in.f = o_f, in.D_h = dh(o_e), in.D_ch = dh(o_e), in.alpha = alpha ? &(*alpha)(o_e, 0) : nullptr, in.T = wall ? &v_aux[o_p](o_s, 0) : &inco[o_p](o_e, 0), in.p = press(o_e), in.v = nv.addr();
                      in.y = dom[o_p].get().dist_norm_bord(o_f) / 2; //ok?
                      in.Tp = v_aux[0](s, 0), in.lambda = &lambda(!Clambda * o_e, 0), in.mu = &mu(!Cmu * o_e, 0), in.rho = &rho(!Crho * o_e, 0), in.Cp = &Cp(!Ccp * o_e, 0);
                      out.qpk = &qpk, out.dTf_qpk = &dTf_qpk, out.dTp_qpk = &dTp_qpk, out.qpi = &qpi, out.dTf_qpi = &dTf_qpi, out.dTp_qpi = &dTp_qpi, out.nonlinear = &j;
                      //appel : on n'est implicite qu'en les temperatures
                      corr[o_p]->qp(in, out);
                      for (n = 0; n < N[o_p]; n++)
                        secmem(!aux_only * ne_tot[0] + s, 0) -= surf * qpk(n); //second membre
                      if (f < dom[0].get().premiere_face_int())
                        for (n = 0; n < N[0]; n++)
                          flux_bords_(f, n) += surf * qpk(n);
                      if (mat[o_p] && !semi && (wall || !aux_only))
                        for (k1 = 0; k1 < N[o_p]; k1++)
                          for (k2 = 0; k2 < N[o_p]; k2++) //derivees en Tfluide
                            (*mat[o_p])(!aux_only * ne_tot[0] + s, N[o_p] * (wall ? !aux_only * ne_tot[o_p] + o_s : o_e) + k2) += surf * dTf_qpk(k1, k2);
                      if (mat[0] && !semi)
                        for (n = 0; n < N[o_p]; n++) /* derivees en Tparoi */
                          (*mat[0])(!aux_only * ne_tot[0] + s, !aux_only * ne_tot[0] + s) += surf * dTp_qpk(n);
                    }
                  else for (n = 0; n < N[0]; n++)/* ajout du h(T'-T) */
                      {
                        double h = ech.invh_paroi ? 1. / ech.invh_paroi : 1e8;
                        secmem(!aux_only * ne_tot[0] + s, n) -= surf * h * (v_aux[0](s, n) - v_aux[o_p](o_s, n)); //second membre
                        if (f < dom[0].get().premiere_face_int())
                          flux_bords_(f, n) += surf * h * (v_aux[0](s, n) - v_aux[o_p](o_s, n));
                        if (mat[0] && !semi)
                          (*mat[0])(N[0] * (!aux_only * ne_tot[0] + s) + n, N[0] * (!aux_only * ne_tot[0] + s) + n) += surf * h; //derivees
                        if (mat[o_p] && !semi)
                          (*mat[o_p])(N[0] * (!aux_only * ne_tot[0] + s) + n, N[o_p] * (!aux_only * ne_tot[o_p] + o_s) + n) -= surf * h;
                      }
                }
              else if (fcl[0](f, 0) == 4)
                {
                  for (n = 0; n < N[0]; n++)
                    secmem(!aux_only * ne_tot[0] + s, n) -= surf * ref_cast(Neumann, cls[0].get()[fcl[0](f, 1)].valeur()).flux_impose(fcl[0](f, 2), n);
                  if (f < dom[0].get().premiere_face_int())
                    for (n = 0; n < N[0]; n++)
                      flux_bords_(f, n)+= surf * ref_cast(Neumann, cls[0].get()[fcl[0](f, 1)].valeur()).flux_impose(fcl[0](f, 2), n);
                }
              else abort();
            }
    }

  /* autres equations : sommets de Dirichlet */
  if (!semi && mat[0])
    for (s = 0; s < dom[0].get().nb_som(); s++)
      for (i = scl_d[0](s); i < scl_d[0](s + 1); i++)
        for (n = 0; n < N[0]; n++)
          {
            secmem(!aux_only * ne_tot[0] + s, n) += (sub_type(Dirichlet, cls[0].get()[scl_c[0](i, 0)].valeur()) ? ref_cast(Dirichlet, cls[0].get()[scl_c[0](i, 0)].valeur()).val_imp(scl_c[0](i, 1), n) : 0) - v_aux[0](s, n);
            (*mat[0])(N[0] * (!aux_only * ne_tot[0] + s) + n, N[0] * (!aux_only * ne_tot[0] + s) + n)++;
          }

  /* equations triviales aux aretes si semi-implicite */
  if (!aux_only && semi && mat[0])
    for (s = 0; s < dom[0].get().domaine().nb_som(); s++)
      for (n = 0; n < N[0]; n++) //semi-implicite : T_f^+ = var_aux
        secmem(ne_tot[0] + s, n) += v_aux[0](s, n) - (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : equation().inconnue().valeurs())(ne_tot[0] + s, n), (*mat[0])(N[0] * (ne_tot[0] + s) + n, N[0] * (ne_tot[0] + s) + n)++;
}
