/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <Champ_Uniforme.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Mod_turb_hyd_base.h>
#include <Synonyme_info.h>
#include <MD_Vector_base.h>
#include <Op_Grad_CoviMAC_Face.h>
#include <Pb_Multiphase.h>

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
  const Equation_base& eq = equation();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, eq.inconnue().valeur());
  zone.init_equiv();
  flux_bords_.resize(zone.premiere_face_int(), dimension * ch.valeurs().line_size());
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  porosite_e.ref(zone.porosite_elem());
  porosite_f.ref(zone.porosite_face());


  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  if (modele_turbulence.non_nul())
    {
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
      const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
      associer_diffusivite_turbulente(alpha_t);
    }
}

void Op_Diff_CoviMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();

  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //semi-implicite ou pas de bloc diagonal -> rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, k, e, eb, f, fb, fc, n, N = ch.valeurs().line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  IntTrav stencil(0, 2), tpfa(0, N);
  stencil.set_smart_resize(1), zone.creer_tableau_faces(tpfa), tpfa = 1;

  /* stencils du flux : ceux (reduits) de update_nu si nu constant ou scalaire, ceux (complets) de la zone sinon */
  Cerr << "Op_Diff_CoviMAC_Face::dimensionner() : ";
  update_phif(!nu_constant_); //si nu variable, stencil complet

  for (f = 0; f < nf_tot; f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (j = phif_d(f, 0); j < phif_d(f + 1, 0); j++) //flux a travers la face f
        {
          for (eb = phif_e(j), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && fcl(fb, 0) < 2) //equation a la face fb
              {
                if (phif_d(f + 1, 0) == phif_d(f, 0) + 2 && (fc = zone.equiv(f, i, k)) >= 0 && fcl(fc, 0) < 2) for (n = 0; n < N; n++) //equivalence avec la face fc
                    stencil.append_line(N * fb + n, N * (eb == e ? fb : fc) + n);
                else if (eb < ne_tot) for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) //pas d'equivalence -> elem/face
                        stencil.append_line(N * fb + n, N * (nf_tot + D * eb + d) + n);
              }
          if (eb < ne_tot) for (d = 0; d < D; d++) for (n = 0; n < N; n++) //equation a l'element e
                stencil.append_line(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * eb + d) + n);
          for (n = 0; n < N; n++) if (phif_d(f, 0) + 2 < phif_d(f + 1, 0)) tpfa(f, n) = 0;
        }

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * (nf_tot + D * ne_tot))
       << " " << mp_somme_vect(tpfa) * 100. / (N * zone.md_vector_faces().valeur().nb_items_seq_tot()) << "% TPFA " << finl;
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Diff_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& resu, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices[nom_inco] : NULL; //facultatif
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces(), &fcl = ch.fcl();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &pf = porosite_f, &pe = porosite_e;
  const DoubleTab& nf = zone.face_normales(), &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f();
  int i, j, k, e, eb, feb, f, fb, fc, fd, n, N = inco.line_size(), d, D = dimension, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  double mult, f_eps;

  update_phif();

  DoubleTrav coeff(N);
  for (f = 0; f < nf_tot; f++) for (f_eps = min(vf(f) / fs(f), eps), i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (j = phif_d(f, 0); j < phif_d(f + 1, 0); j++)
        {
          for (n = 0; n < N; n++) coeff(n) = (i ? 1 : -1) * fs(f) * (phif_w(f, n) * phif_c(j, n, 0) + (1 - phif_w(f, n)) * phif_c(j, n, 1)); //|f|[nu grad v]_f
          for (eb = phif_e(j), feb = eb - ne_tot, k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && fcl(fb, 0) < 2) //equations a la face fb
              {
                if (phif_d(f + 1, 0) == phif_d(f, 0) + 2 && (fc = zone.equiv(f, i, k)) >= 0) //equivalence : face-face
                  for (fd = (eb == e ? fb : fc), mult = pf(fd) / pe(f_e(f, eb == e ? i : !i)) * (zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1), n = 0; n < N; n++)
                    {
                      double fac = mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * mult * coeff(n);
                      resu(fb, n) -= fac * inco(fd, n);
                      if (mat && fcl(fd, 0) < 2) (*mat)(N * fb + n, N * fd + n) += fac;
                    }
                else
                  {
                    for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) //pas d'equivalence: elem -> face
                          {
                            double fac = mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * nf(fb, d) / fs(fb) * coeff(n) * (eb == e ? 1 - f_eps : 1);
                            resu(fb, n) -= fac * (feb < 0 ? inco(nf_tot + D * eb + d, n) : fcl(feb, 0) == 3 ? ref_cast(Dirichlet, cls[fcl(feb, 1)].valeur()).val_imp(fcl(feb, 2), N * d + n) : 0);
                            if (mat && feb < 0) (*mat)(N * fb + n, N * (nf_tot + D * eb + d) + n) += fac;
                          }
                    if (eb == e) for (n = 0; n < N; n++)
                        {
                          double fac = mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * coeff(n) * f_eps;
                          resu(fb, n) -= fac * inco(fb, n);
                          if (mat) (*mat)(N * fb + n, N * fb + n) += fac;
                        }
                  }
              }

          for (d = 0; d < D; d++) for (n = 0; n < N; n++) //equations a l'element e
              {
                resu(nf_tot + D * e + d, n) -= coeff(n) * (feb < 0 ? inco(nf_tot + D * eb + d, n) : fcl(feb, 0) == 3 ? ref_cast(Dirichlet, cls[fcl(feb, 1)].valeur()).val_imp(fcl(feb, 2), N * d + n) : 0);
                if (mat && feb < 0) (*mat)(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * eb + d) + n) += coeff(n);
              }
        }
}
