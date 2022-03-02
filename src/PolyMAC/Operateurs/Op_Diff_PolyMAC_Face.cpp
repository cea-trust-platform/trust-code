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
// File:        Op_Diff_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Linear_algebra_tools_impl.h>
#include <Op_Diff_PolyMAC_Face.h>

#include <Dirichlet_homogene.h>

#include <Champ_Face_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Mod_turb_hyd_base.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <MD_Vector_base.h>
#include <Pb_Multiphase.h>
#include <Synonyme_info.h>
#include <Zone_PolyMAC.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <ConstDoubleTab_parts.h>
#include <Schema_Implicite_base.h>
#include <Simpler_Base.h>

Implemente_instanciable( Op_Diff_PolyMAC_Face, "Op_Diff_PolyMAC_Face|Op_Dift_PolyMAC_Face_PolyMAC", Op_Diff_PolyMAC_base ) ;
Add_synonym(Op_Diff_PolyMAC_Face, "Op_Diff_PolyMAC_var_Face");
Add_synonym(Op_Diff_PolyMAC_Face, "Op_Dift_PolyMAC_var_Face_PolyMAC");

Sortie& Op_Diff_PolyMAC_Face::printOn( Sortie& os ) const
{
  Op_Diff_PolyMAC_base::printOn( os );
  return os;
}

Entree& Op_Diff_PolyMAC_Face::readOn( Entree& is )
{
  Op_Diff_PolyMAC_base::readOn( is );
  return is;
}

void Op_Diff_PolyMAC_Face::completer()
{
  Op_Diff_PolyMAC_base::completer();
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  Equation_base& eq = equation();
  Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, eq.inconnue().valeur());
  ch.init_auxiliary_variables(); /* ajout des inconnues auxiliaires (vorticites aux aretes) */
  flux_bords_.resize(zone.premiere_face_int(), dimension * ch.valeurs().line_size());
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_PolyMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  porosite_e.ref(zone.porosite_elem());
  porosite_f.ref(zone.porosite_face());
}

double Op_Diff_PolyMAC_Face::calculer_dt_stab() const
{
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces();
  const DoubleTab& nf = zone.face_normales(),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL,
                     *a_r = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.champ_conserve().passe() : NULL; /* produit alpha * rho */
  const DoubleVect& pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  update_nu();

  int i, e, f, n, N = equation().inconnue().valeurs().line_size();
  double dt = 1e10;
  DoubleTrav flux(N), vol(N);
  for (e = 0; e < zone.nb_elem(); e++)
    {
      for (flux = 0, vol = pe(e) * ve(e), i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (n = 0; n < N; n++)
            flux(n) += zone.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
      for (n = 0; n < N; n++) if ((!alp || (*alp)(e, n) > 0.25) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = min(dt, vol(n) * (a_r ? (*a_r)(e, n) : 1) / flux(n));
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces(), &f_s = zone.face_sommets(), &e_a = zone.zone().elem_aretes(), &fcl = ch.fcl();
  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco)) return; //pas de bloc diagonal -> rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;
  update_nu();
  int i, j, k, e, f, fb, a, ab, s, n, N = ch.valeurs().line_size(), nf_tot = zone.nb_faces_tot(), d, db, D = dimension, N_nu = nu_.line_size(), semi = semi_impl.count(nom_inco);

  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  Cerr << "Op_Diff_PolyMAC_Face::dimensionner() : ";

  /* bloc (faces, aretes) : rot [(lambda grad)^u]*/
  if (!semi) for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
    for (a = D < 3 ? s : zone.som_arete[s].at(f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0)), n = 0; n < N; n++)
      stencil.append_line(N * f + n, N * (nf_tot + a) + n);

  /* blocs (aretes, faces) et (aretes, aretes) : avec M2 et W1 dans chaque element */
  Matrice33 L(0, 0, 0, 0, 0, 0, 0, 0, D < 3), iL; //tenseur de diffusion dans chaque element, son inverse
  DoubleTrav inu, m2, w1; //au format compris par zone.nu_dot
  nu_.nb_dim() == 2 ? inu.resize(1, N) : nu_.nb_dim() == 3 ? inu.resize(1, N, D) : inu.resize(1, N, D, D);
  m2.set_smart_resize(1), w1.set_smart_resize(1);
  if (!semi) for (e = 0; e < zone.nb_elem_tot(); e++)
    {
      //tenseur de diffusion diagonal ou anisotrope diagonal : inverse faciles!
      if (nu_.nb_dim() < 4) for (i = 0; i < N_nu; i++) inu.addr()[i] = 1. / nu_.addr()[N_nu * e + i]; 
      else for (n = 0; n < N; n++) //sinon : une matrice a inverser par composante
        {
          for (d = 0; d < D; d++) for (db = 0; db < D; db++) L(d, db) = nu_(e, n, d, db);
          Matrice33::inverse(L, iL);
          for (d = 0; d < D; d++) for (db = 0; db < D; db++) inu(0, n, d, db) = iL(d, db);
        }
      zone.M2(&inu, e, m2);
      if (D > 2) zone.W1(&nu_, e, w1); //uniquement en 3D: en 2D, matrice diagonale

      //bloc (aretes, faces): en parcourant les aretes de chaque face
      for (i = 0; i < m2.dimension(0); i++) for (f = e_f(e, i), j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
        if ((a = D < 3 ? s : zone.som_arete[s].at(f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0))) < (D < 3 ? zone.zone().nb_som() : zone.zone().nb_aretes()))
          for (k = 0; k < m2.dimension(1); k++) for (fb = e_f(e, k), n = 0; n < N; n++)
            if (fcl(f, 0) == 2 || m2(i, k, n)) //si f est Symetrie, alors il y a aussi une partie en ve -> dependance complete
              stencil.append_line(N * (nf_tot + a) + n, N * fb + n);
      //bloc (aretes, aretes) : avec m1 si D = 3 (sinon, fait ensuite)
      if (D > 2) for (i = 0; i < w1.dimension(0); i++) if ((a = e_a(e, i)) < zone.zone().nb_aretes()) for (j = 0; j < w1.dimension(1); j++) for (ab = e_a(e, j), n = 0; n < N; n++)
        if (w1(i, j, n)) stencil.append_line(N * (nf_tot + a) + n, N * (nf_tot + ab) + n);
    }
  if (semi || D < 3) for (s = 0; s < (D < 3 ? zone.nb_som() : zone.zone().nb_aretes()); s++) for (n = 0; n < N; n++) stencil.append_line(N * (nf_tot + s) + n, N * (nf_tot + s) + n);

  tableau_trier_retirer_doublons(stencil);
  Cerr << "OK" << finl;
  Matrix_tools::allocate_morse_matrix(ch.valeurs().size_totale(), ch.valeurs().size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Diff_PolyMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim &cls = ch.zone_Cl_dis().les_conditions_limites();
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces(), &f_s = zone.face_sommets(), &e_a = zone.zone().elem_aretes(), &fcl = ch.fcl(), &f_e = zone.face_voisins();
  const std::string& nom_inco = ch.le_nom().getString();
  Matrice_Morse* mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
  update_nu();
  int i, j, k, e, f, fb, a, ab, s, n, N = ch.valeurs().line_size(), nf_tot = zone.nb_faces_tot(), d, db, D = dimension, N_nu = nu_.line_size(), semi = semi_impl.count(nom_inco);
  double vecz[3] = { 0, 0, 1 }, v_cl[3], t = equation().schema_temps().temps_courant();
  const DoubleTab &xp = zone.xp(), &xv = zone.xv(), &xs = zone.zone().domaine().coord_sommets(), &xa = D < 3 ? xs : zone.xa(), &ta = zone.ta(), &nf = zone.face_normales(),
                  &inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs();
  const DoubleVect &la = zone.longueur_aretes(), &vf = zone.volumes_entrelaces(), &fs = zone.face_surfaces(), &ve = zone.volumes();

  /* que faire avec les variables auxiliaires ? */
  if (t_last_aux_ < t && mat && !semi) t_last_aux_ = t, use_aux_ = 0; /* 1) on est en implicite complet : pas besoin de mat_aux / var_aux */
  else if (t_last_aux_ < t) update_aux(); /* 2) premier pas a ce temps en semi-implicite : on calcule les variables auxiliaires a t et on les stocke dans var_aux */
  ConstDoubleTab_parts p_inco(inco); /* deux parties de l'inconnue */
  const DoubleTab &omega = use_aux_ ? var_aux : p_inco[1]; /* les variables auxiliaires peuvent etre soit dans inco/semi_impl (cas 1), soit dans var_aux (cas 2) */

  /* bloc (faces, aretes) : rot [(lambda grad)^u]*/
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
    {
      a = D < 3 ? s : zone.som_arete[s].at(f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0)); //indice d'arete
      auto vec = zone.cross(3, D, D < 3 ? vecz : &ta(a, 0), &xv(f, 0), NULL, &xa(a, 0));
      int sgn = zone.dot(&nf(f, 0), &vec[0]) > 0 ? 1 : -1; //orientation arete-face
      for (n = 0; n < N; n++) secmem(f, n) -= sgn * vf(f) / fs(f) * (D < 3 ? 1 : la(a)) * omega(a, n);
      if (mat && !semi) for (n = 0; n < N; n++) (*mat)(N * f + n, N * (nf_tot + a) + n) += sgn * vf(f) / fs(f) * (D < 3 ? 1 : la(a));
    }

  /* blocs (aretes, faces) et (aretes, aretes) : avec M2 et W1 dans chaque element */
  Matrice33 L(0, 0, 0, 0, 0, 0, 0, 0, D < 3), iL; //tenseur de diffusion dans chaque element, son inverse et le carre de celui-ci
  DoubleTrav dL(N), inu, m2, w1; //determinant, inverse (au format compris par zone.nu_dot), matrices M2(iL) / W1(L)
  nu_.nb_dim() == 2 ? inu.resize(1, N) : nu_.nb_dim() == 3 ? inu.resize(1, N, D) : inu.resize(1, N, D, D);
  m2.set_smart_resize(1), w1.set_smart_resize(1);
  if (mat && semi) for (a = 0; a < xa.dimension(0); a++) for (n = 0; n < N; n++) /* en semi-implicite : egalites w_a^+ = var_aux */
      secmem(nf_tot + a, n) += omega(a, n) - inco(nf_tot + a, n), (*mat)(N * (nf_tot + a) + n, N * (nf_tot + a) + n) += 1;
  else if (mat && !semi) for (e = 0; e < zone.nb_elem_tot(); e++) /* en implicite : vraies equations */
    {
      //tenseur de diffusion diagonal ou anisotrope diagonal : inverses faciles!
      if (nu_.nb_dim() == 2) for (n = 0; n < N; n++) inu(0, n) = 1. / nu_(e, n), dL(n) = std::pow(nu_(e, n), D);
      else if (nu_.nb_dim() == 3) for (n = 0; n < N; n++) for (d = 0, dL(n) = 1; d < D; d++) inu(0, n, d) = 1. / nu_(e, n, d), dL(n) *= nu_(e, n, d);
      if (nu_.nb_dim() < 4) for (i = 0; i < N_nu; i++) inu.addr()[i] = 1. / nu_.addr()[N_nu * e + i]; 
      else for (n = 0; n < N; n++) //sinon : une matrice a inverser par composante
        {
          for (d = 0; d < D; d++) for (db = 0; db < D; db++) L(d, db) = nu_(e, n, d, db);
          dL(n) = Matrice33::inverse(L, iL); //renvoie le determinant!
          for (d = 0; d < D; d++) for (db = 0; db < D; db++) inu(0, n, d, db) = iL(d, db);
        }
      zone.M2(&inu, e, m2);
      if (D > 2) zone.W1(&nu_, e, w1); //uniquement en 3D: en 2D, matrice diagonale

      //bloc (aretes, faces): en parcourant les aretes de chaque face
      for (i = 0; i < m2.dimension(0); i++) for (f = e_f(e, i), j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
        if ((a = D < 3 ? s : zone.som_arete[s].at(f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0))) < (D < 3 ? zone.zone().nb_som() : zone.zone().nb_aretes()))
          {
            auto vec = zone.cross(3, D, D < 3 ? vecz : &ta(a, 0), &xv(f, 0), NULL, &xa(a, 0));
            int sgn = (e == f_e(f, 0) ? 1 : -1) * zone.dot(&nf(f, 0), &vec[0]) > 0 ? 1 : -1; //orientation arete-face (dans le sens sortant de e)
            for (k = 0; k < m2.dimension(1); k++) for (fb = e_f(e, k), n = 0; n < N; n++)//partie x_e -> x_f avec m2 + partie x_f -> x_a avec v_e si bord de Neumann / Symetrie
              {
                double coeff = m2(i, k, n) + (fcl(f, 0) == 2 ? zone.nu_dot(&inu, 0, n, &xa(a, 0), &xv(fb, 0), &xv(f, 0), &xp(e, 0)) / ve(e) : 0);
                if (!coeff) continue;
                secmem(nf_tot + a, n) += sgn * coeff * inco(fb, n) * fs(fb) * (e == f_e(fb, 0) ? 1 : -1);
                (*mat)(N * (nf_tot + a) + n, N * fb + n) -= sgn * coeff * fs(fb) * (e == f_e(fb, 0) ? 1 : -1);
              }
            if (fcl(f, 0) == 3) for (n = 0; n < N; n++) //si bord de Dirichlet : partie x_f -> x_a avec la vitesse donnee par la CL
              {
                for (d = 0; d < D; d++) v_cl[d] = ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n); //v impose
                secmem(nf_tot + a, n) += sgn * zone.nu_dot(&inu, 0, n, &xa(a, 0), v_cl, &xv(f, 0));
              }
          }

      //bloc (aretes, aretes) : avec m1 si D = 3, diagonale * surface si D = 2
      if (D == 2)
        {
          for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < 2; j++) if ((s = f_s(f, j)) < zone.nb_som())
            {
              auto vec = zone.cross(D, D, &xv(f, 0), &xs(s, 0), &xp(e, 0), &xp(e, 0));
              double surf = dabs(vec[2]) / 2; //surface du triangle (e, f, s)
              for (n = 0; n < N; n++) secmem(nf_tot + s, n) -= surf * inco(nf_tot + s, n)  / dL(n);
              for (n = 0; n < N; n++) (*mat)(N * (nf_tot + s) + n, N * (nf_tot + s) + n) += surf / dL(n);
            }          
        }
      else for (i = 0; i < w1.dimension(0); i++) if ((a = e_a(e, i)) < zone.zone().nb_aretes()) for (j = 0; j < w1.dimension(1); j++)
        for (ab = e_a(e, j), n = 0; n < N; n++) if (w1(i, j, n))
          {
            secmem(nf_tot + a, n) -= w1(i, j, n) * la(ab) * inco(nf_tot + ab, n) / dL(n);
            (*mat)(N * (nf_tot + a) + n, N * (nf_tot + ab) + n) += w1(i, j, n) * la(ab) / dL(n);
          }
    }
}

/* calcu: des variables auxiliaires en semi-implicite : necessaire seulement en 3D */
void Op_Diff_PolyMAC_Face::update_aux() const
{
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim &cls = ch.zone_Cl_dis().les_conditions_limites();
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces(), &f_s = zone.face_sommets(), &e_a = zone.zone().elem_aretes(), &fcl = ch.fcl(), &f_e = zone.face_voisins();
  int i, j, k, e, f, fb, a, ab, s, n, N = ch.valeurs().line_size(), nf_tot = zone.nb_faces_tot(), d, db, D = dimension, N_nu = nu_.line_size();
  double vecz[3] = { 0, 0, 1 }, v_cl[3];
  const DoubleTab &xp = zone.xp(), &xv = zone.xv(), &xs = zone.zone().domaine().coord_sommets(), &xa = D < 3 ? xs : zone.xa(), &ta = zone.ta(), &nf = zone.face_normales(),
                  &inco = ch.passe();
  const DoubleVect &la = zone.longueur_aretes(), &fs = zone.face_surfaces(), &ve = zone.volumes();
  
  Matrice33 L(0, 0, 0, 0, 0, 0, 0, 0, 0), iL; //tenseur de diffusion dans chaque element, son inverse et le carre de celui-ci
  DoubleTrav dL(N), inu, m2, w1, secmem(var_aux); //determinant, inverse (au format compris par zone.nu_dot), matrices M2(iL) / W1(L), second membre
  nu_.nb_dim() == 2 ? inu.resize(1, N) : nu_.nb_dim() == 3 ? inu.resize(1, N, D) : inu.resize(1, N, D, D);
  m2.set_smart_resize(1), w1.set_smart_resize(1);
  
  if (!var_aux.get_md_vector().non_nul()) /* var_aux non dimensionne -> on doit tout initialiser */
    {
      var_aux.resize(0,N), zone.creer_tableau_aretes(var_aux);
      IntTrav stencil(0, 2);
      stencil.set_smart_resize(1);
      if (D < 3) for (a = 0; a < zone.zone().nb_som(); a++) for (n = 0; n < N; n++) stencil.append_line(N * a + n, N * a + n);
      else for (e = 0; e < zone.nb_elem_tot(); e++) for (zone.W1(&nu_, e, w1), i = 0; i < w1.dimension(0); i++)
        if ((a = e_a(e, i)) < zone.zone().nb_aretes()) for (j = 0; j < w1.dimension(1); j++) for (ab = e_a(e, j), n = 0; n < N; n++)
            if (w1(i, j, n)) stencil.append_line(N * a + n, N * ab + n);
      Matrix_tools::allocate_morse_matrix(var_aux.size_totale(), var_aux.size_totale(), stencil, mat_aux);
      //copie du solveur de l'equation
      solv_aux = ref_cast(Parametre_implicite, equation().parametre_equation().valeur()).solveur();
    }

  for (mat_aux.get_set_coeff() = 0, e = 0; e < zone.nb_elem_tot(); e++)
    {
      //tenseur de diffusion diagonal ou anisotrope diagonal : inverses faciles!
      if (nu_.nb_dim() == 2) for (n = 0; n < N; n++) inu(0, n) = 1. / nu_(e, n), dL(n) = std::pow(nu_(e, n), D);
      else if (nu_.nb_dim() == 3) for (n = 0; n < N; n++) for (d = 0, dL(n) = 1; d < D; d++) inu(0, n, d) = 1. / nu_(e, n, d), dL(n) *= nu_(e, n, d);
      if (nu_.nb_dim() < 4) for (i = 0; i < N_nu; i++) inu.addr()[i] = 1. / nu_.addr()[N_nu * e + i]; 
      else for (n = 0; n < N; n++) //sinon : une matrice a inverser par composante
        {
          for (d = 0; d < D; d++) for (db = 0; db < D; db++) L(d, db) = nu_(e, n, d, db);
          dL(n) = Matrice33::inverse(L, iL); //renvoie le determinant!
          for (d = 0; d < D; d++) for (db = 0; db < D; db++) inu(0, n, d, db) = iL(d, db);
        }
      zone.M2(&inu, e, m2);
      if (D > 2) zone.W1(&nu_, e, w1);

      //(aretes, faces): en parcourant les aretes de chaque face -> contribution a secmem
      for (i = 0; i < m2.dimension(0); i++) for (f = e_f(e, i), j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
        if ((a = D < 3 ? s : zone.som_arete[s].at(f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0))) < (D < 3 ? zone.zone().nb_som() : zone.zone().nb_aretes()))
          {
            auto vec = zone.cross(3, D, D < 3 ? vecz : &ta(a, 0), &xv(f, 0), NULL, &xa(a, 0));
            int sgn = (e == f_e(f, 0) ? 1 : -1) * zone.dot(&nf(f, 0), &vec[0]) > 0 ? 1 : -1; //orientation arete-face (dans le sens sortant de e)
            for (k = 0; k < m2.dimension(1); k++) for (fb = e_f(e, k), n = 0; n < N; n++)//partie x_e -> x_f avec m2 + partie x_f -> x_a avec v_e si bord de Neumann / Symetrie
              {
                double coeff = m2(i, k, n) + (fcl(f, 0) == 2 ? zone.nu_dot(&inu, 0, n, &xa(a, 0), &xv(fb, 0), &xv(f, 0), &xp(e, 0)) / ve(e) : 0);
                secmem(nf_tot + a, n) += sgn * coeff * inco(fb, n) * fs(fb) * (e == f_e(fb, 0) ? 1 : -1);
              }
            if (fcl(f, 0) == 3) for (n = 0; n < N; n++) //si bord de Dirichlet : partie x_f -> x_a avec la vitesse donnee par la CL
              {
                for (d = 0; d < D; d++) v_cl[d] = ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n); //v impose
                secmem(nf_tot + a, n) += sgn * zone.nu_dot(&inu, 0, n, &xa(a, 0), v_cl, &xv(f, 0));
              }
          }
      //(faces, faces) : remplissage de mat_aux
      if (D < 3)
      {
        for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < 2; j++) if ((s = f_s(f, j)) < zone.nb_som())
          {
            auto vec = zone.cross(D, D, &xv(f, 0), &xs(s, 0), &xp(e, 0), &xp(e, 0));
            double surf = dabs(vec[2]) / 2; //surface du triangle (e, f, s)
            for (n = 0; n < N; n++) mat_aux(N * (nf_tot + s) + n, N * (nf_tot + s) + n) += surf / dL(n);
          }
      }
      else for (i = 0; i < w1.dimension(0); i++) if ((a = e_a(e, i)) < zone.zone().nb_aretes()) for (j = 0; j < w1.dimension(1); j++)
        for (ab = e_a(e, j), n = 0; n < N; n++) if (w1(i, j, n)) mat_aux(N * (nf_tot + a) + n, N * (nf_tot + ab) + n) += w1(i, j, n) * la(ab) / dL(n);
    }

  solv_aux.valeur().reinit();
  solv_aux.resoudre_systeme(mat_aux, secmem, var_aux); /* resolution */
  t_last_aux_ = equation().schema_temps().temps_courant(); /* on est maintenant a jour */
  use_aux_ = 1;
}

