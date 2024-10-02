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

Implemente_instanciable_sans_constructeur(Op_Diff_PolyVEF_P0P1_Elem, "Op_Diff_PolyVEF_P0P1_Elem|Op_Diff_PolyVEF_P0P1_var_Elem", Op_Diff_PolyMAC_P0P1NC_base);

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
  Champ_Elem_PolyVEF_P0P1& ch = ref_cast(Champ_Elem_PolyVEF_P0P1, le_champ_inco.non_nul() ? le_champ_inco.valeur() : eq.inconnue().valeur());
  ch.init_auxiliary_variables();
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  if (dom.nb_joints() && dom.joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyVEF_P0P1_Elem : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  flux_bords_.resize(dom.premiere_face_int(), ch.valeurs().line_size());
}

void Op_Diff_PolyVEF_P0P1_Elem::init_op_ext() const
{
  if (op_ext.size()) return; //deja fait
  /* recherche d'operateurs connectes par des Echange_contact_PolyMAC_P0P1NC (eventuellement indirectement) */
  std::set<const Op_Diff_PolyMAC_P0P1NC_base*> ops_tbd = { this }, ops; //operateurs a scanner pour remplir op_ext, operateurs trouves
  while (ops_tbd.size())
    {
      const Op_Diff_PolyMAC_P0P1NC_base* op = *ops_tbd.begin();
      ops_tbd.erase(ops_tbd.begin()), ops.insert(op);
      const Conds_lim& cls = op->equation().domaine_Cl_dis().les_conditions_limites();
      for (int i = 0; i < cls.size(); i++)
        if (sub_type(Echange_contact_PolyMAC_P0P1NC, cls[i].valeur()))
          {
            const Echange_contact_PolyMAC_P0P1NC& cl = ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[i].valeur());
            cl.init_op();
            if (!ops.count(&cl.o_diff.valeur())) ops_tbd.insert(&cl.o_diff.valeur()); //on a un nouvel operateur a scanner
          }
    }
  op_ext = { this };
  for (auto &&op : ops)
    if (op != this) op_ext.push_back(op); /* remplissage de op_ext avec l'operateur local en 1er */

  /* remplissage des o_idx des Echange_contact_PolyMAC_P0P1NC connectes */
  const Conds_lim& cls = equation().domaine_Cl_dis().les_conditions_limites();
  for (int i = 0; i < cls.size(); i++)
    if (sub_type(Echange_contact_PolyMAC_P0P1NC, cls[i].valeur()))
      {
        const Echange_contact_PolyMAC_P0P1NC& cl = ref_cast(Echange_contact_PolyMAC_P0P1NC, cls[i].valeur());
        cl.o_idx = (int)(std::find(op_ext.begin(), op_ext.end(), &cl.o_diff.valeur()) - op_ext.begin());
      }
}

double Op_Diff_PolyVEF_P0P1_Elem::calculer_dt_stab() const
{
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  const IntTab& e_f = dom.elem_faces();
  const DoubleTab& nf = dom.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : NULL,
                    &diffu = diffusivite_pour_pas_de_temps().valeurs(), &lambda = diffusivite().valeurs();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &vf = dom.volumes_entrelaces(), &ve = dom.volumes();
  update_nu();

  int i, e, f, n, N = equation().inconnue().valeurs().dimension(1), cD = diffu.dimension(0) == 1, cL = lambda.dimension(0) == 1;
  double dt = 1e10;
  DoubleTrav flux(N);
  for (e = 0; e < dom.nb_elem(); e++)
    {
      for (flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          flux(n) += dom.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
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
  int i, j, e, eb, s, sb, f, n, M, n_ext = (int)op_ext.size();
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
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      dom.push_back(std::ref(ref_cast(Domaine_PolyVEF, op_ext[i]->equation().domaine_dis().valeur())));
      f_e.push_back(std::ref(dom[i].get().face_voisins())), e_f.push_back(std::ref(dom[i].get().elem_faces())), f_s.push_back(std::ref(dom[i].get().face_sommets()));
      cls.push_back(std::ref(op_ext[i]->equation().domaine_Cl_dis().les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyVEF_P0P1_Elem, *op_ext[i]).nu());
      const Champ_Elem_PolyVEF_P0P1& ch = ref_cast(Champ_Elem_PolyVEF_P0P1, op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue().valeur() : op_ext[i]->equation().inconnue().valeur());
      scl_d.push_back(std::ref(ch.scl_d(0))), scl_c.push_back(std::ref(ch.scl_c(0)));
      N.push_back(ch.valeurs().line_size()), fcl.push_back(std::ref(ch.fcl())), ne_tot.push_back(dom[i].get().nb_elem_tot());
      inco.push_back(ch.valeurs()), v_part.emplace_back(ch.valeurs());
      stencil[i].resize(0, 2), stencil[i].set_smart_resize(1);
    }

  for (f = 0; f < dom[0].get().nb_faces_tot(); f++)
    for (i = 0; i < 2 + f_s[0].get().dimension(1); i++)
      if (i < 2 ? !aux_only && (e = f_e[0](f, i)) >= 0 && e < dom[0].get().nb_elem() : (s = f_s[0](f, i - 2)) >= 0 && s < dom[0].get().domaine().nb_som() && scl_d[0](s) == scl_d[0](s + 1))
        for (j = 0; j < 2 + f_s[0].get().dimension(1); j++)
          if (j < 2 ? !aux_only && (eb = f_e[0](f, j)) >= 0 : (sb = f_s[0](f, j - 2)) >= 0 && scl_d[0](sb) == scl_d[0](sb + 1))
            for (n = 0; n < N[0]; n++)
              stencil[0].append_line(N[0] * (i < 2 ? e : !aux_only * ne_tot[0] + s) + n, N[0] * (j < 2 ? eb : !aux_only * ne_tot[0] + sb) + n);

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
  int i, ib, j, k, l, m, e, eb, f, s, sb, n, M, n_ext = (int)op_ext.size(), semi = (int)semi_impl.count(nom_inco), d, D = dimension, n_sf, nt, ok;
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
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      dom.push_back(std::ref(ref_cast(Domaine_PolyVEF, op_ext[i]->equation().domaine_dis().valeur())));
      f_e.push_back(std::ref(dom[i].get().face_voisins())), e_f.push_back(std::ref(dom[i].get().elem_faces())), f_s.push_back(std::ref(dom[i].get().face_sommets()));
      fs.push_back(std::ref(dom[i].get().face_surfaces())), nf.push_back(std::ref(dom[i].get().face_normales()));
      xp.push_back(std::ref(dom[i].get().xp())), xv.push_back(std::ref(dom[i].get().xv())), xs.push_back(std::ref(dom[i].get().domaine().coord_sommets()));
      vf.push_back(std::ref(dom[i].get().volumes_entrelaces()));
      cls.push_back(std::ref(op_ext[i]->equation().domaine_Cl_dis().les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyVEF_P0P1_Elem, *op_ext[i]).nu());
      const Champ_Elem_PolyVEF_P0P1& ch = ref_cast(Champ_Elem_PolyVEF_P0P1, op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue().valeur() : op_ext[i]->equation().inconnue().valeur());
      scl_d.push_back(std::ref(ch.scl_d(0))), scl_c.push_back(std::ref(ch.scl_c(0)));
      inco.push_back(std::ref(semi_impl.count(nom_mat) ? semi_impl.at(nom_mat) : ch.valeurs())), v_part.emplace_back(inco.back());
      corr.push_back(sub_type(Energie_Multiphase, op_ext[i]->equation()) && ref_cast(Pb_Multiphase, op_ext[i]->equation().probleme()).has_correlation("flux_parietal")
                     ? &ref_cast(Flux_parietal_base, ref_cast(Pb_Multiphase, op_ext[i]->equation().probleme()).get_correlation("flux_parietal").valeur()) : NULL);
      N.push_back(inco[i].get().line_size()), ne_tot.push_back(dom[i].get().nb_elem_tot()), fcl.push_back(std::ref(ch.fcl()));
    }

  /* que faire avec les variables auxiliaires ? */
  double t = equation().schema_temps().temps_courant(), dt = equation().schema_temps().pas_de_temps(), fac, prefac;
  if (aux_only) use_aux_ = 0; /* 1) on est en train d'assembler le systeme de resolution des variables auxiliaires lui-meme */
  else if (mat[0] && !semi) t_last_aux_ = t + dt, use_aux_ = 0; /* 2) on est en implicite complet : pas besoin de mat_aux / var_aux, on aura les variables a t + dt */
  else if (t_last_aux_ < t) update_aux(t); /* 3) premier pas a ce temps en semi-implicite : on calcule les variables auxiliaires a t et on les stocke dans var_aux */
  for (i = 0; i < n_ext; i++) v_aux.push_back(use_aux_ ? ref_cast(Op_Diff_PolyVEF_P0P1_Elem, *op_ext[i]).var_aux : v_part[i][1]); /* les variables auxiliaires peuvent etre soit dans inco/semi_impl (cas 1), soit dans var_aux (cas 2) */

  DoubleTrav Sa, Gf, Gfa, A; //surface par arete du diamant, gradient non stabilise (par diamant) et stabilise (par arete du diamant), forme bilineaire
  Gf.set_smart_resize(1), Gfa.set_smart_resize(1), A.set_smart_resize(1);
  for (f = 0; f < dom[0].get().nb_faces_tot(); f++)
    {
      for (ok = 0, i = 0; i < 2 + f_s[0].get().dimension(1); i++) /* si aucun element ou sommet reel autour de f, rien a faire */
        ok |= i < 2 ? (e = f_e[0](f, i)) >= 0 && e < dom[0].get().nb_elem() : (s = f_s[0](f, i - 2)) >= 0 && s < dom[0].get().nb_som() && scl_d[0](s) == scl_d[0](s + 1);
      if (!ok) continue;
      dom[0].get().surf_elem_som(f, Sa), n_sf = Sa.dimension(0), nt = 2 + n_sf, Gf.resize(nt, D), Gfa.resize(n_sf, D, nt, D), A.resize(nt, nt, N[0]);

      /* Gf : gradient non stabilise dans le diamant */
      for (Gf = 0, i = 0; i < n_sf; i++)
        for (j = 0; j < D; j++)
          if (j < 2)
            for (d = 0; d < D; d++) Gf(j, d) += Sa(i, j, d), Gf(2 + i, d) -= Sa(i, j, d); //parties s -> e
          else for (ib = i + 1 < n_sf ? i + 1 : 0, d = 0; d < D; d++) Gf(2 + ib, d) += Sa(i, j, d), Gf(2 + i, d) -= Sa(i, j, d); //partie s -> sb (3D)
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
                  A(l, m, n) += fac * dom[0].get().nu_dot(&diffu[0].get(), e, n, &Gfa(i, k ? 2 : j, l, 0), &Gfa(i, k ? 2 : j, m, 0));
      for (i = 0; i < nt; i++) //symetrisation
        for (j = i + 1; j < nt; j++)
          for (n = 0; n < N[0]; n++)
            A(i, j, n) = A(j, i, n);

      /* contributions */
      for (i = 0; i < nt; i++)
        if (i < 2 ? !aux_only && (e = f_e[0](f, i)) >= 0 && e < dom[0].get().nb_elem() : (s = f_s[0](f, i - 2)) >= 0 && s < dom[0].get().domaine().nb_som() && scl_d[0](s) == scl_d[0](s + 1))
          for (j = 0; j < 2 + n_sf; j++)
            if (j < 2 ? (eb = f_e[0](f, j)) >= 0 : (sb = f_s[0](f, j - 2)) >= 0)
              for (n = 0; n < N[0]; n++)
                {
                  secmem(i < 2 ? e : !aux_only * ne_tot[0] + s, n) -= A(i, j, n) * (j < 2 ? inco[0](eb, n) : v_aux[0](sb, n));
                  if (mat[0] && (j < 2 ? !aux_only : scl_d[0](sb) == scl_d[0](sb + 1)))
                    (*mat[0])(N[0] * (i < 2 ? e : !aux_only * ne_tot[0] + s) + n, N[0] * (j < 2 ? eb : !aux_only * ne_tot[0] + sb) + n) += A(i, j, n);
                }
    }

  /* equations triviales aux aretes si semi-implicite */
  if (!aux_only && semi && mat[0])
    for (s = 0; s < dom[0].get().domaine().nb_som(); s++)
      for (n = 0; n < N[0]; n++) //semi-implicite : T_f^+ = var_aux
        secmem(ne_tot[0] + s, n) += v_aux[0](s, n) - (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : equation().inconnue().valeurs())(ne_tot[0] + s, n), (*mat[0])(N[0] * (ne_tot[0] + s) + n, N[0] * (ne_tot[0] + s) + n)++;
}
