/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_CoviMAC_Face.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Champ_Face_CoviMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Mod_turb_hyd_base.h>
#include <Synonyme_info.h>
#include <MD_Vector_base.h>
#include <Masse_CoviMAC_Face.h>

Implemente_instanciable( Op_Diff_CoviMAC_Face, "Op_Diff_CoviMAC_Face|Op_Dift_CoviMAC_Face_CoviMAC", Op_Diff_CoviMAC_base ) ;
Add_synonym(Op_Diff_CoviMAC_Face, "Op_Diff_CoviMAC_var_Face");
Add_synonym(Op_Diff_CoviMAC_Face, "Op_Dift_CoviMAC_var_Face_CoviMAC");

Sortie& Op_Diff_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Diff_CoviMAC_base::printOn( os );
  return os;
}

Entree& Op_Diff_CoviMAC_Face::readOn( Entree& is )
{
  Op_Diff_CoviMAC_base::readOn( is );
  return is;
}

void Op_Diff_CoviMAC_Face::completer()
{
  Op_Diff_CoviMAC_base::completer();
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.init_cl();
  flux_bords_.resize(zone.premiere_face_int(), dimension * ch.valeurs().line_size());

  if (que_suis_je() == "Op_Diff_CoviMAC_Face") return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
  associer_diffusivite_turbulente(alpha_t);
}

void Op_Diff_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();
  int i, j, k, e, eb, f, fb, n, N = ch.valeurs().line_size(), N_nu = nu_.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  IntTrav stencil(0, 2), tpfa(0, D * N);
  stencil.set_smart_resize(1), zone.creer_tableau_faces(tpfa), tpfa = 1;

  update_nu();

  Cerr << "Op_Diff_CoviMAC_Elem::dimensionner() : ";

  /* flux a deux points aux faces de bord (sauf Neumann ou Symetrie) */
  for (f = 0; f < nf_tot; f++) if (ch.fcl(f, 0) > 2 && (e = f_e(f, 0)) < zone.nb_elem()) //face de bord (hors Neumann / Symetrie) : flux a deux points
      {
        for (i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (ch.fcl(fb, 0) < 2) for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb))
                for (n = 0; n < N; n++) stencil.append_line(N * fb + n, N * (nf_tot + ne_tot * d+ e) + n); //elem -> face
        for (d = 0; d < D; d++) for (n = 0; n < N; n++) stencil.append_line(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + e) + n); //elem -> elem
      }
    else if (!ch.fcl(f, 0)) for (i = 0; i < 2; i++) if ((e = f_e(f, i)) < zone.nb_elem()) //faces internes -> flux multipoints
          for (d = 0; d < D; d++) for (n = 0; n < N; n++) for (j = zone.feb_d(f); j < zone.feb_d(f + 1); j++) if (nu_constant_ || N_nu <= N ? dabs(phif_c(j, D * n + d)) > 1e-8 : 1)
                  {
                    for (eb = zone.feb_j(j), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (ch.fcl(fb, 0) < 2 && dabs(nf(fb, d)) > 1e-6 * fs(fb)) //elem -> face
                        stencil.append_line(N * fb + n, N * (nf_tot + ne_tot * d + (eb < ne_tot ? eb : f_e(eb - ne_tot, 0))) + n);
                    stencil.append_line(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + (eb < ne_tot ? eb : f_e(eb - ne_tot, 0))) + n); //elem -> elem
                    tpfa(f, D * n + d) &= (j < zone.feb_d(f) + 2);
                  }

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * D * zone.zone().md_vector_elements().valeur().nb_items_seq_tot())
       << " " << mp_somme_vect(tpfa) * 100. / (N * D * zone.md_vector_faces().valeur().nb_items_seq_tot()) << "% TPFA " << finl;
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Diff_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  update_nu();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  int i, j, e, f, fb, n, N = inco.line_size(), d, D = dimension, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  /* faces de bord : flux a deux points + valeurs aux bord */
  DoubleTrav vb(D, zone.nb_faces_tot(), N), h_int(N), phi(N);
  for (d = 0; d < D; d++) for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0)) //faces de bord seulement
        {
          /* h_int : coefficent d'echange element-face */
          for (e = f_e(f, 0), n = 0; n < N; n++)
            h_int(n) = zone.nu_dot(nu_, e, D * n + d, N * D, &nf(f, 0), &nf(f, 0)) / (zone.dist_norm_bord(f) * fs(f) * fs(f));

          /* phi / vb : selon CLs */
          if (ch.fcl(f, 0) < 3) for (n = 0; n < N; n++) //Neumann ou Symetrie
              phi(n) = 0, vb(d, f, n) = inco.addr()[N * (nf_tot + ne_tot * d + e) + n];
          else for (n = 0; n < N; n++) //Dirichlet ou Dirichlet_homogene
              {
                vb(d, f, n) = ch.fcl(f, 0) == 3 ? ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), D * n + d) : 0;
                phi(n) = h_int(n) * (inco.addr()[N * (nf_tot + ne_tot * d + e) + n] - vb(d, f, n));
              }

          /* contributions */
          for (i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (ch.fcl(fb, 0) < 2) for (n = 0; n < N; n++) //elem->face
                resu.addr()[N * fb + n] -= mu_f(fb, n, e != f_e(fb, 0)) * nf(fb, d) * phi(n) * vf(fb) / ve(e);
          for (n = 0; n < N; n++) resu.addr()[N * (nf_tot + ne_tot * d + e) + n] -= fs(f) * phi(n); //elem->elem
          if (f < zone.premiere_face_int()) for (n = 0; n < N; n++) flux_bords_(f, D * n + d) = - fs(f) * phi(n);
        }

  /* faces internes : interpolation -> flux amont/aval -> combinaison convexe */
  for (d = 0; d < D; d++) for (f = zone.premiere_face_int(); f < zone.nb_faces(); f++)
      {
        /* phi = |f|nu.grad v */
        for (phi = 0, i = zone.feb_d(f); i < zone.feb_d(f + 1); i++) for (e = zone.feb_j(i), n = 0; n < N; n++)
            phi(n) += phif_c(i, D * n + d) * (e < ne_tot ? inco.addr()[N * (nf_tot + ne_tot * d + e) + n] : vb(d, e - ne_tot, n));

        /* contributions */
        for (i = 0; i < 2; i++)
          {
            for (e = f_e(f, i), j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) if (ch.fcl(fb, 0) < 2) for (n = 0; n < N; n++) //elem->face
                  resu.addr()[N * fb + n] -= (i ? 1 : -1) * mu_f(fb, n, e != f_e(fb, 0)) * nf(fb, d) / fs(fb) * phi(n) * vf(fb) / ve(e);
            for (n = 0; n < N; n++) resu.addr()[N * (nf_tot + ne_tot * d + f_e(f, i)) + n] -= (i ? 1 : -1) * phi(n); //elem->elem
          }

      }

  return resu;
}

//Description:
//on assemble la matrice.

inline void Op_Diff_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  update_nu();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  int i, j, e, f, fb, n, N = inco.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  /* faces de bord : flux a deux points + valeurs aux bord */
  std::vector<std::map<int, double>> dphi(N); //dphi[n][idx] : derivee de la composante n du flus par rapport a idx
  DoubleTrav dvb(D, zone.nb_faces_tot(), N), h_int(N);
  for (d = 0; d < D; d++) for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0)) //faces de bord seulement
        {
          /* h_int : coefficent d'echange element-face */
          for (e = f_e(f, 0), n = 0; n < N; n++)
            h_int(n) = zone.nu_dot(nu_, e, D * n + d, N * D, &nf(f, 0), &nf(f, 0)) / (zone.dist_norm_bord(f) * fs(f) * fs(f));

          /* phi / Tb : selon CLs */
          if (ch.fcl(f, 0) < 3) for (n = 0; n < N; n++) dvb(d, f, n) = 1; //Neumann ou Symetrie
          else for (n = 0; n < N; n++) //Dirichlet ou Dirichlet_homogene
              dphi[n][N * (nf_tot + ne_tot * d + e) + n] = h_int(n), dvb(d, f, n) = 0;

          //contributions a la matrice
          for (i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2 && dabs(nf(fb, d)) > 1e-6 * fs(fb))
              for (n = 0; n < N; n++) for (auto &i_c : dphi[n]) matrice(N * fb + n, i_c.first) += mu_f(fb, n, e != f_e(fb, 0)) * nf(fb, d) * i_c.second * vf(fb) / ve(e); //elem -> face

          if (e < zone.nb_elem()) for (n = 0; n < N; n++) for (auto &i_c : dphi[n]) //elem -> elem
                matrice(N * (nf_tot + ne_tot * d + e) + n, i_c.first) += fs(f) * i_c.second;
          for (n = 0; n < N; n++) dphi[n].clear(); //menage
        }

  /* faces internes : interpolation -> flux amont/aval -> combinaison convexe */
  for (d = 0; d < D; d++) for (f = zone.premiere_face_int(); f < zone.nb_faces(); f++)
      {
        /* phi = |f|nu.grad v */
        for (i = zone.feb_d(f); i < zone.feb_d(f + 1); i++) for (e = zone.feb_j(i), n = 0; n < N; n++) if (dabs(phif_c(i, D * n + d)) > 1e-8) //auxiliaires
              dphi[n][N * (nf_tot + ne_tot * d + (e < ne_tot ? e : f_e(e - ne_tot, 0))) + n] += phif_c(i, D * n + d) * (e < ne_tot ? 1 : dvb(d, e - ne_tot, n));

        /* contributions */
        for (i = 0; i < 2; i++)
          {
            for (e = f_e(f, i), j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2 && dabs(nf(fb, d)) > 1e-6 * fs(fb))
                for (n = 0; n < N; n++) for (auto &i_c : dphi[n]) matrice(N * fb + n, i_c.first) += (i ? 1 : -1) * mu_f(fb, n, e != f_e(fb, 0)) * nf(fb, d) / fs(fb) * i_c.second * vf(fb) / ve(e); //elem -> face

            if (e < zone.nb_elem()) for (n = 0; n < N; n++) for (auto &i_c : dphi[n])//elem -> elem
                  matrice(N * (nf_tot + ne_tot * d + e) + n, i_c.first) += (i ? 1 : -1) * i_c.second;
          }
        for (n = 0; n < N; n++) dphi[n].clear(); //menage
      }
}

//Description:
//on ajoute la contribution du second membre.

void Op_Diff_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
