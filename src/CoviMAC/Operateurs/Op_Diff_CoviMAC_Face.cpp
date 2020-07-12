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
  ch.init_cl(), zone.init_equiv();
  flux_bords_.resize(zone.premiere_face_int(), dimension * ch.valeurs().line_size());
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();

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
  int i, j, k, e, eb, f, fb, fc, n, N = ch.valeurs().line_size(), N_nu = nu_.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  IntTrav stencil(0, 2), tpfa(0, N);
  stencil.set_smart_resize(1), zone.creer_tableau_faces(tpfa), tpfa = 1;

  update_nu();

  Cerr << "Op_Diff_CoviMAC_Elem::dimensionner() : ";

  for (f = 0; f < nf_tot; f++) if (ch.fcl(f, 0) > 2 && (e = f_e(f, 0)) < zone.nb_elem()) //face de bord (hors Neumann / Symetrie) : flux a deux points
      {
        for (i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (ch.fcl(fb, 0) < 2) for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb))
                for (n = 0; n < N; n++) stencil.append_line(N * fb + n, N * (nf_tot + ne_tot * d+ e) + n); //elem -> face
        for (d = 0; d < D; d++) for (n = 0; n < N; n++) stencil.append_line(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + e) + n); //elem -> elem
      }
    else if (!ch.fcl(f, 0)) for (i = 0; i < 2; i++) if ((e = f_e(f, i)) < zone.nb_elem()) //faces internes -> flux multipoints
          for (j = zone.feb_d(f); j < zone.feb_d(f + 1); j++)
            {
              for (eb = zone.feb_j(j), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (ch.fcl(fb, 0) < 2)
                  {
                    if (j < zone.feb_d(f) + 2 && (fc = zone.equiv(f, i, k)) >= 0)
                      {
                        if (ch.fcl(fc, 0) < 2) for (n = 0; n < N; n++) if (nu_constant_ || N_nu <= N ? dabs(phif_c(j, n)) > 1e-8 : 1) //equivalence : face-face
                              stencil.append_line(N * fb + n, N * (j == i + zone.feb_d(f) ? fb : fc) + n);
                      }
                    else for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) if (nu_constant_ || N_nu <= N ? dabs(phif_c(j, n)) > 1e-8 : 1) //sinon : elem-face
                              stencil.append_line(N * fb + n, N * (nf_tot + ne_tot * d + (eb < ne_tot ? eb : f_e(eb - ne_tot, 0))) + n);
                  }
              for (d = 0; d < D; d++) for (n = 0; n < N; n++) if (nu_constant_ || N_nu <= N ? dabs(phif_c(j, n)) > 1e-8 : 1) //elem -> elem
                    stencil.append_line(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + (eb < ne_tot ? eb : f_e(eb - ne_tot, 0))) + n);
              for (n = 0; n < N; n++) if (nu_constant_ || N_nu <= N ? dabs(phif_c(j, n)) > 1e-8 : 1) tpfa(f, n) &= (j < zone.feb_d(f) + 2);
            }

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * D * zone.zone().md_vector_elements().valeur().nb_items_seq_tot())
       << " " << mp_somme_vect(tpfa) * 100. / (N * zone.md_vector_faces().valeur().nb_items_seq_tot()) << "% TPFA " << finl;
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
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  const DoubleTab& nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  int i, j, k, e, eb, f, fb, fc, fd, n, N = inco.line_size(), d, D = dimension, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  double mult;

  /* faces de bord : flux a deux points + valeurs aux bord */
  DoubleTrav vb(zone.nb_faces_tot(), D, N), h_int(N), phi(N, D);
  for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0)) //faces de bord seulement
      {
        /* h_int : coefficent d'echange element-face */
        for (e = f_e(f, 0), n = 0; n < N; n++)
          h_int(n) = zone.nu_dot(nu_, e, n, N, &nf(f, 0), &nf(f, 0)) / (zone.dist_norm_bord(f) * fs(f) * fs(f));

        /* phi / vb : selon CLs */
        if (ch.fcl(f, 0) < 3) for (d = 0; d < D; d++) for (n = 0; n < N; n++) //Neumann ou Symetrie
              phi(n, d) = 0, vb(f, d, n) = inco.addr()[N * (nf_tot + ne_tot * d + e) + n];
        else for (d = 0; d < D; d++) for (n = 0; n < N; n++) //Dirichlet ou Dirichlet_homogene
              {
                vb(f, d, n) = ch.fcl(f, 0) == 3 ? ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), D * n + d) : 0;
                phi(n, d) = h_int(n) * (inco.addr()[N * (nf_tot + ne_tot * d + e) + n] - vb(f, d, n));
              }

        /* contributions */
        for (i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (ch.fcl(fb, 0) < 2) for (n = 0; n < N; n++) //elem->face
              resu.addr()[N * fb + n] -= mu_f(fb, n, e != f_e(fb, 0)) * zone.dot(&nf(fb, 0), &phi(n, 0)) * vf(fb) / ve(e);
        for (d = 0; d < D; d++) for (n = 0; n < N; n++) resu.addr()[N * (nf_tot + ne_tot * d + e) + n] -= fs(f) * phi(n, d); //elem->elem
        if (f < zone.premiere_face_int()) for (d = 0; d < D; d++) for (n = 0; n < N; n++) flux_bords_(f, D * n + d) = - fs(f) * phi(n, d);
      }

  /* faces internes : interpolation -> flux amont/aval -> combinaison convexe */
  for (f = zone.premiere_face_int(); f < zone.nb_faces(); f++) for (i = 0; i < 2; i++) for (e = f_e(f, i), j = zone.feb_d(f); j < zone.feb_d(f + 1); j++)
        {
          //elem -> face
          for (eb = zone.feb_j(j), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (ch.fcl(fb, 0) < 2)
              {
                if (j < zone.feb_d(f) + 2 && (fc = zone.equiv(f, i, k)) >= 0) //equivalence : face-face
                  for (fd = (j == i + zone.feb_d(f) ? fb : fc), mult = pf(fd) / pe(f_e(f, j - zone.feb_d(f))) * (zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1), n = 0; n < N; n++)
                    resu.addr()[N * fb + n] -= (i ? 1 : -1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * mult * phif_c(j, n) * inco.addr()[N * fd + n];
                else for (d = 0; d < D; d++) for (n = 0; n < N; n++) //pas d'equivalence: elem -> face
                      resu.addr()[N * fb + n] -= (i ? 1 : -1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * nf(fb, d) / fs(fb) * phif_c(j, n)
                                                 * (eb < ne_tot ? inco.addr()[N * (nf_tot + ne_tot * d + eb) + n] : vb(eb - ne_tot, d, n));
              }

          //elem -> elem
          for (d = 0; d < D; d++) for (n = 0; n < N; n++)
              resu.addr()[N * (nf_tot + ne_tot * d + e) + n] -= (i ? 1 : -1) * phif_c(j, n)
                                                                * (eb < ne_tot ? inco.addr()[N * (nf_tot + ne_tot * d + eb) + n] : vb(eb - ne_tot, d, n));
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
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  const DoubleTab& nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  int i, j, k, e, eb, ebi, f, fb, fc, fd, n, N = inco.line_size(), d, D = dimension, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  double mult;

  /* faces de bord : flux a deux points + valeurs aux bord */
  DoubleTrav dvb(zone.nb_faces_tot(), D, N), h_int(N);
  for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0) > 2) //Dirichlet : dvb = 0, mais on doit remplir la matrice
      {
        /* h_int : coefficent d'echange element-face */
        for (e = f_e(f, 0), n = 0; n < N; n++)
          h_int(n) = zone.nu_dot(nu_, e, n, N, &nf(f, 0), &nf(f, 0)) / (zone.dist_norm_bord(f) * fs(f) * fs(f));

        //elem -> face
        for (i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2)
            for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++)
                  matrice(N * fb + n, N * (nf_tot + ne_tot * d + e) + n) += mu_f(fb, n, e != f_e(fb, 0)) * nf(fb, d) * h_int(n) * vf(fb) / ve(e);

        //elem -> elem
        if (e < zone.nb_elem()) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
              matrice(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + e) + n) += fs(f) * h_int(n);
      }
    else if (ch.fcl(f, 0)) for (d = 0; d < D; d++) for (n = 0; n < N; n++) dvb(f, d, n) = 1; //Neumann / Symetrie : flux nul, mais on doit remplir dvb

  /* faces internes : interpolation -> flux amont/aval -> combinaison convexe */
  for (f = zone.premiere_face_int(); f < zone.nb_faces(); f++) for (i = 0; i < 2; i++) for (e = f_e(f, i), j = zone.feb_d(f); j < zone.feb_d(f + 1); j++)
        {
          //elem -> face
          for (eb = zone.feb_j(j), ebi = eb < ne_tot ? eb : f_e(eb - ne_tot, 0), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (ch.fcl(fb, 0) < 2)
              {
                if (j < zone.feb_d(f) + 2 && (fc = zone.equiv(f, i, k)) >= 0) //equivalence : face-face
                  {
                    if (ch.fcl(fd = (j == i + zone.feb_d(f) ? fb : fc), 0) < 2) for (mult = pf(fd) / pe(f_e(f, j - zone.feb_d(f))) * (zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1), n = 0; n < N; n++)
                        if (dabs(phif_c(j, n)) > 1e-8) matrice(N * fb + n, N * fd + n) += (i ? 1 : -1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * mult * phif_c(j, n);
                  }
                else for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) if (dabs(phif_c(j, n)) > 1e-8) //pas d'equivalence: elem -> face
                          matrice(N * fb + n, N * (nf_tot + ne_tot * d + ebi) + n) += (i ? 1 : -1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * nf(fb, d) / fs(fb) * phif_c(j, n) * (eb < ne_tot ? 1 : dvb(eb - ne_tot, d, n));
              }

          //elem -> elem
          for (d = 0; d < D; d++) for (n = 0; n < N; n++) if (dabs(phif_c(j, n)) > 1e-8)
                matrice(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + ebi) + n) += (i ? 1 : -1) * phif_c(j, n) * (eb < ne_tot ? 1 : dvb(eb - ne_tot, d, n));
        }
}

//Description:
//on ajoute la contribution du second membre.

void Op_Diff_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
