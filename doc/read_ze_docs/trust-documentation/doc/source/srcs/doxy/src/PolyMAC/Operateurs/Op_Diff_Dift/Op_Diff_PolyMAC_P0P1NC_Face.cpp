/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Op_Diff_PolyMAC_P0P1NC_Face.h>
#include <Linear_algebra_tools_impl.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <Dirichlet_homogene.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Champ_Uniforme.h>
#include <TRUSTTab_parts.h>
#include <MD_Vector_base.h>
#include <Pb_Multiphase.h>
#include <Synonyme_info.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Symetrie.h>

Implemente_instanciable( Op_Diff_PolyMAC_P0P1NC_Face, "Op_Diff_PolyMAC_P0P1NC_Face|Op_Dift_PolyMAC_P0P1NC_Face_PolyMAC_P0P1NC", Op_Diff_PolyMAC_P0P1NC_base );
Add_synonym(Op_Diff_PolyMAC_P0P1NC_Face, "Op_Diff_PolyMAC_P0P1NC_var_Face");
Add_synonym(Op_Diff_PolyMAC_P0P1NC_Face, "Op_Dift_PolyMAC_P0P1NC_var_Face_PolyMAC_P0P1NC");

Sortie& Op_Diff_PolyMAC_P0P1NC_Face::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0P1NC_base::printOn(os); }

Entree& Op_Diff_PolyMAC_P0P1NC_Face::readOn(Entree& is) { return Op_Diff_PolyMAC_P0P1NC_base::readOn(is); }

void Op_Diff_PolyMAC_P0P1NC_Face::completer()
{
  Op_Diff_PolyMAC_P0P1NC_base::completer();
  const Domaine_PolyMAC_P0P1NC& domaine = le_dom_poly_.valeur();
  Equation_base& eq = equation();
  Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, le_champ_inco.non_nul() ? le_champ_inco.valeur() : eq.inconnue().valeur());
  ch.init_auxiliary_variables(); /* ajout des inconnues auxiliaires (vorticites aux aretes) */
  flux_bords_.resize(domaine.premiere_face_int(), dimension * ch.valeurs().line_size());
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyMAC_P0P1NC_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  porosite_e.ref(equation().milieu().porosite_elem());
  porosite_f.ref(equation().milieu().porosite_face());
  op_ext = { this };
}

double Op_Diff_PolyMAC_P0P1NC_Face::calculer_dt_stab() const
{
  const Domaine_PolyMAC_P0P1NC& domaine = le_dom_poly_.valeur();
  const IntTab& e_f = domaine.elem_faces();
  const DoubleTab& nf = domaine.face_normales(), *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr,
                   *a_r = sub_type(Pb_Multiphase, equation().probleme()) ?
                          &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().champ_conserve().passe() : (has_champ_masse_volumique() ? &get_champ_masse_volumique().valeurs() : nullptr); /* produit alpha * rho */
  const DoubleVect& pe = equation().milieu().porosite_elem(), &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes();
  update_nu();

  int i, e, f, n, N = equation().inconnue().valeurs().line_size();
  double dt = 1e10;
  DoubleTrav flux(N), vol(N);
  for (e = 0; e < domaine.nb_elem(); e++)
    {
      for (flux = 0, vol = pe(e) * ve(e), i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          flux(n) += domaine.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
      for (n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 0.25) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = std::min(dt, vol(n) * (a_r ? (*a_r)(e, n) : 1) / flux(n));
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyMAC_P0P1NC_Face::dimensionner_blocs_ext(int aux_only, matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue().valeur());
  const Domaine_PolyMAC_P0P1NC& domaine = le_dom_poly_.valeur();
  const IntTab& e_f = domaine.elem_faces(), &f_s = domaine.face_sommets(), &e_a = domaine.domaine().elem_aretes(), &fcl = ch.fcl();
  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco))
    return; //pas de bloc diagonal -> rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;
  update_nu();
  ConstDoubleTab_parts p_inco(ch.valeurs());
  int i, j, k, e, f, fb, a, ab, s, n, N = ch.valeurs().line_size(), nf_tot = domaine.nb_faces_tot(), d, db, D = dimension, N_nu = nu_.line_size(), semi = (int) semi_impl.count(nom_inco);

  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  Cerr << "Op_Diff_PolyMAC_P0P1NC_Face::dimensionner() : ";

  /* bloc (faces, aretes) : rot [(lambda grad)^u]*/
  if (!semi && !aux_only)
    for (f = 0; f < domaine.nb_faces(); f++)
      for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
        for (a = D < 3 ? s : domaine.som_arete[s].at(f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0)), n = 0; n < N; n++)
          stencil.append_line(N * f + n, N * (nf_tot + a) + n);

  /* blocs (aretes, faces) et (aretes, aretes) : avec M2 et W1 dans chaque element */
  Matrice33 L(0, 0, 0, 0, 0, 0, 0, 0, D < 3), iL; //tenseur de diffusion dans chaque element, son inverse
  DoubleTrav inu, m2, w1, v_e, v_ea; //au format compris par domaine.nu_dot
  nu_.nb_dim() == 2 ? inu.resize(1, N) : nu_.nb_dim() == 3 ? inu.resize(1, N, D) : inu.resize(1, N, D, D);
  m2.set_smart_resize(1), w1.set_smart_resize(1), v_e.set_smart_resize(1), v_ea.set_smart_resize(1);
  if (!semi)
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      {
        //tenseur de diffusion diagonal ou anisotrope diagonal : inverse faciles!
        if (nu_.nb_dim() < 4)
          for (i = 0; i < N_nu; i++)
            inu.addr()[i] = 1. / nu_.addr()[N_nu * e + i];
        else
          for (n = 0; n < N; n++) //sinon : une matrice a inverser par composante
            {
              for (d = 0; d < D; d++)
                for (db = 0; db < D; db++)
                  L(d, db) = nu_(e, n, d, db);
              Matrice33::inverse(L, iL);
              for (d = 0; d < D; d++)
                for (db = 0; db < D; db++)
                  inu(0, n, d, db) = iL(d, db);
            }
        domaine.M2(&inu, e, m2);
        if (D > 2)
          domaine.W1(&nu_, e, w1, v_e, v_ea); //uniquement en 3D: en 2D, matrice diagonale

        //bloc (aretes, faces): en parcourant les aretes de chaque face
        if (!aux_only)
          for (i = 0; i < m2.dimension(0); i++)
            for (f = e_f(e, i), j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
              if ((a = D < 3 ? s : domaine.som_arete[s].at(f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0))) < (D < 3 ? domaine.domaine().nb_som() : domaine.domaine().nb_aretes()))
                for (k = 0; k < m2.dimension(1); k++)
                  for (fb = e_f(e, k), n = 0; n < N; n++)
                    if (fcl(f, 0) == 2 || m2(i, k, n)) //si f est Symetrie, alors il y a aussi une partie en ve -> dependance complete
                      stencil.append_line(N * (nf_tot + a) + n, N * fb + n);
        //bloc (aretes, aretes) : avec m1 si D = 3 (sinon, fait ensuite)
        if (D > 2)
          for (i = 0; i < w1.dimension(0); i++)
            if ((a = e_a(e, i)) < domaine.domaine().nb_aretes())
              for (j = 0; j < w1.dimension(1); j++)
                for (ab = e_a(e, j), n = 0; n < N; n++)
                  if (w1(i, j, n))
                    stencil.append_line(N * (!aux_only * nf_tot + a) + n, N * (!aux_only * nf_tot + ab) + n);
      }
  if (semi || D < 3)
    for (s = 0; s < (D < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()); s++)
      for (n = 0; n < N; n++)
        stencil.append_line(N * (!aux_only * nf_tot + s) + n, N * (!aux_only * nf_tot + s) + n);

  tableau_trier_retirer_doublons(stencil);
  Cerr << "OK" << finl;
  Matrix_tools::allocate_morse_matrix(aux_only ? p_inco[1].size_totale() : ch.valeurs().size_totale(), aux_only ? p_inco[1].size_totale() : ch.valeurs().size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Diff_PolyMAC_P0P1NC_Face::ajouter_blocs_ext(int aux_only, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue().valeur());
  const Conds_lim& cls = ch.domaine_Cl_dis().les_conditions_limites();
  const Domaine_PolyMAC_P0P1NC& domaine = le_dom_poly_.valeur();
  const IntTab& e_f = domaine.elem_faces(), &f_s = domaine.face_sommets(), &e_a = domaine.domaine().elem_aretes(), &fcl = ch.fcl(), &f_e = domaine.face_voisins();
  const std::string& nom_inco = ch.le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  update_nu();
  int i, j, k, e, f, fb, a, ab, s, n, N = ch.valeurs().line_size(), nf_tot = domaine.nb_faces_tot(), d, db, D = dimension, N_nu = nu_.line_size(), semi = (int) semi_impl.count(nom_inco);
  double vecz[3] = { 0, 0, 1 }, v_cl[3], t = equation().schema_temps().temps_courant();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &xs = domaine.domaine().coord_sommets(), &xa = D < 3 ? xs : domaine.xa(), &ta = domaine.ta(), &nf = domaine.face_normales(), &inco =
                                                                                                           semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs();
  const DoubleVect& la = domaine.longueur_aretes(), &vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces(), &ve = domaine.volumes();

  /* que faire avec les variables auxiliaires ? */
  if (aux_only)
    use_aux_ = 0; /* 1) on est en train d'assembler le systeme de resolution des variables auxiliaires lui-meme */
  else if (mat && !semi)
    t_last_aux_ = t, use_aux_ = 0; /* 2) on est en implicite complet : pas besoin de mat_aux / var_aux */
  else if (t_last_aux_ < t)
    update_aux(t); /* 3) premier pas a ce temps en semi-implicite : on calcule les variables auxiliaires a t et on les stocke dans var_aux */
  ConstDoubleTab_parts p_inco(inco); /* deux parties de l'inconnue */
  const DoubleTab& omega = use_aux_ ? var_aux : p_inco[1]; /* les variables auxiliaires peuvent etre soit dans inco/semi_impl (cas 1), soit dans var_aux (cas 2) */

  /* bloc (faces, aretes) : rot [(lambda grad)^u]*/
  if (!aux_only)
    for (f = 0; f < domaine.nb_faces(); f++)
      for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
        {
          a = D < 3 ? s : domaine.som_arete[s].at(f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0)); //indice d'arete
          auto vec = domaine.cross(3, D, D < 3 ? vecz : &ta(a, 0), &xv(f, 0), nullptr, &xa(a, 0));
          int sgn = domaine.dot(&nf(f, 0), &vec[0]) > 0 ? 1 : -1; //orientation arete-face
          for (n = 0; n < N; n++)
            secmem(f, n) -= sgn * vf(f) / fs(f) * (D < 3 ? 1 : la(a)) * omega(a, n);
          if (mat && !semi)
            for (n = 0; n < N; n++)
              (*mat)(N * f + n, N * (nf_tot + a) + n) += sgn * vf(f) / fs(f) * (D < 3 ? 1 : la(a));
        }

  /* blocs (aretes, faces) et (aretes, aretes) : avec M2 et W1 dans chaque element */
  Matrice33 L(0, 0, 0, 0, 0, 0, 0, 0, D < 3), iL; //tenseur de diffusion dans chaque element, son inverse et le carre de celui-ci
  DoubleTrav dL(N), inu, m2, w1, v_e, v_ea; //determinant, inverse (au format compris par domaine.nu_dot), matrices M2(iL) / W1(L)
  nu_.nb_dim() == 2 ? inu.resize(1, N) : nu_.nb_dim() == 3 ? inu.resize(1, N, D) : inu.resize(1, N, D, D);
  m2.set_smart_resize(1), w1.set_smart_resize(1), v_e.set_smart_resize(1), v_ea.set_smart_resize(1);
  if (!aux_only && mat && semi)
    for (a = 0; a < xa.dimension(0); a++)
      for (n = 0; n < N; n++) /* en semi-implicite : egalites w_a^+ = var_aux */
        secmem(nf_tot + a, n) += omega(a, n) - ch.valeurs()(nf_tot + a, n), (*mat)(N * (nf_tot + a) + n, N * (nf_tot + a) + n)++;
  else if (mat && !semi)
    for (e = 0; e < domaine.nb_elem_tot(); e++) /* en implicite : vraies equations */
      {
        //tenseur de diffusion diagonal ou anisotrope diagonal : inverses faciles!
        if (nu_.nb_dim() == 2)
          for (n = 0; n < N; n++)
            inu(0, n) = 1. / nu_(e, n), dL(n) = std::pow(nu_(e, n), D);
        else if (nu_.nb_dim() == 3)
          for (n = 0; n < N; n++)
            for (d = 0, dL(n) = 1; d < D; d++)
              inu(0, n, d) = 1. / nu_(e, n, d), dL(n) *= nu_(e, n, d);
        if (nu_.nb_dim() < 4)
          for (i = 0; i < N_nu; i++)
            inu.addr()[i] = 1. / nu_.addr()[N_nu * e + i];
        else
          for (n = 0; n < N; n++) //sinon : une matrice a inverser par composante
            {
              for (d = 0; d < D; d++)
                for (db = 0; db < D; db++)
                  L(d, db) = nu_(e, n, d, db);
              dL(n) = Matrice33::inverse(L, iL); //renvoie le determinant!
              for (d = 0; d < D; d++)
                for (db = 0; db < D; db++)
                  inu(0, n, d, db) = iL(d, db);
            }
        domaine.M2(&inu, e, m2);
        if (D > 2)
          domaine.W1(&nu_, e, w1, v_e, v_ea); //uniquement en 3D: en 2D, matrice diagonale

        //bloc (aretes, faces): en parcourant les aretes de chaque face
        for (i = 0; i < m2.dimension(0); i++)
          for (f = e_f(e, i), j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
            if ((a = D < 3 ? s : domaine.som_arete[s].at(f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0))) < (D < 3 ? domaine.domaine().nb_som() : domaine.domaine().nb_aretes()))
              {
                auto vec = domaine.cross(3, D, D < 3 ? vecz : &ta(a, 0), &xv(f, 0), nullptr, &xa(a, 0));
                int sgn = (e == f_e(f, 0) ? 1 : -1) * domaine.dot(&nf(f, 0), &vec[0]) > 0 ? 1 : -1; //orientation arete-face (dans le sens sortant de e)
                for (k = 0; k < m2.dimension(1); k++)
                  for (fb = e_f(e, k), n = 0; n < N; n++) //partie x_e -> x_f avec m2 + partie x_f -> x_a avec v_e si bord de Neumann / Symetrie
                    {
                      double coeff = m2(i, k, n) + (fcl(f, 0) == 2 ? domaine.nu_dot(&inu, 0, n, &xa(a, 0), &xv(fb, 0), &xv(f, 0), &xp(e, 0)) / ve(e) : 0);
                      if (!coeff)
                        continue;
                      secmem(!aux_only * nf_tot + a, n) += sgn * coeff * inco(fb, n) * fs(fb) * (e == f_e(fb, 0) ? 1 : -1);
                      if (!aux_only)
                        (*mat)(N * (nf_tot + a) + n, N * fb + n) -= sgn * coeff * fs(fb) * (e == f_e(fb, 0) ? 1 : -1);
                    }
                if (fcl(f, 0) == 3 && sub_type(Dirichlet, cls[fcl(f, 1)].valeur()))
                  for (n = 0; n < N; n++) //si bord de Dirichlet : partie x_f -> x_a avec la vitesse donnee par la CL
                    {
                      for (d = 0; d < D; d++)
                        v_cl[d] = ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n); //v impose
                      secmem(!aux_only * nf_tot + a, n) += sgn * domaine.nu_dot(&inu, 0, n, &xa(a, 0), v_cl, &xv(f, 0));
                    }
              }

        //bloc (aretes, aretes) : avec m1 si D = 3, diagonale * surface si D = 2
        if (D == 2)
          {
            for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
              for (j = 0; j < 2; j++)
                if ((s = f_s(f, j)) < domaine.nb_som())
                  {
                    auto vec = domaine.cross(D, D, &xv(f, 0), &xs(s, 0), &xp(e, 0), &xp(e, 0));
                    double surf = std::abs(vec[2]) / 2; //surface du triangle (e, f, s)
                    for (n = 0; n < N; n++)
                      secmem(!aux_only * nf_tot + s, n) -= surf * inco(nf_tot + s, n) / dL(n);
                    for (n = 0; n < N; n++)
                      (*mat)(N * (!aux_only * nf_tot + s) + n, N * (!aux_only * nf_tot + s) + n) += surf / dL(n);
                  }
          }
        else
          for (i = 0; i < w1.dimension(0); i++)
            if ((a = e_a(e, i)) < domaine.domaine().nb_aretes())
              for (j = 0; j < w1.dimension(1); j++)
                for (ab = e_a(e, j), n = 0; n < N; n++)
                  if (w1(i, j, n))
                    {
                      secmem(!aux_only * nf_tot + a, n) -= w1(i, j, n) * la(ab) * inco(nf_tot + ab, n) / dL(n);
                      (*mat)(N * (!aux_only * nf_tot + a) + n, N * (!aux_only * nf_tot + ab) + n) += w1(i, j, n) * la(ab) / dL(n);
                    }
      }
  i++;
}

