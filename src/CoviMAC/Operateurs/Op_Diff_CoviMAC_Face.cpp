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
  ch.init_cl(), zone.init_w1();

  if (que_suis_je() == "Op_Diff_CoviMAC_Face") return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
  associer_diffusivite_turbulente(alpha_t);
}

void Op_Diff_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& tf = zone.face_tangentes(), &xp = zone.xp(), &xvm = zone.xvm();
  int i, j, k, l, r, e, eb, f, fb, fc, nf_tot = zone.nb_faces_tot();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* qdm a la face f -> element amont/aval e -> face fb de e -> flux a la face fc -> elements amont/aval eb */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) < 2) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          std::array<double, 6> t_x = {{ 0, }}; //direction / position relative a l'element e
          for (r = 0; r < dimension; r++) t_x[r] = tf(f, r), t_x[3 + r] = xvm(f, r) - xp(e, r);
          /* decomposition du gradient a la face fb a l'aide de w1 */
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) for (k = zone.w1d(fb); k < zone.w1d(fb + 1); k++)
              for (l = 0, fc = zone.w1j(k); l < 2 && (eb = f_e(fc, l)) >= 0; l++)
                for (auto &&f_v : zone.interp_dir(eb, t_x, 1)) if (ch.icl(f_v.first, 0) < 2) stencil.append_line(f, f_v.first);
        }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nf_tot, nf_tot, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Diff_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const DoubleVect& ve = zone.volumes(), &fs = zone.face_surfaces(), &pf = zone.porosite_face(), &vf = zone.volumes_entrelaces();
  const DoubleTab& tf = zone.face_tangentes(), &xp = zone.xp(), &xvm = zone.xvm(), &vfd = zone.volumes_entrelaces_dir();
  int i, j, k, l, r, e, eb, f, fb, fc;

  update_nu();

  /* qdm a la face f -> element amont/aval e -> face fb de e -> flux a la face fc -> elements amont/aval eb */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) < 2) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          std::array<double, 6> t_x = {{ 0, }}; //direction / position relative a l'element e
          for (r = 0; r < dimension; r++) t_x[r] = tf(f, r), t_x[3 + r] = xvm(f, r) - xp(e, r);
          /* decomposition du gradient a la face fb a l'aide de w1 */
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) for (k = zone.w1d(fb); k < zone.w1d(fb + 1); k++)
              {
                fc = zone.w1j(k);
                double fac = vfd(f, i) / ve(e) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) * zone.w1c(k) * nu_faces_(fc, 0) * pf(fc) * fs(fc) / vf(fc);
                for (l = 0; l < 2; l++)
                  {
                    eb = f_e(fc, l) >= 0 ? f_e(fc, l) : (ch.icl(fc, 0) < 3 ? f_e(fc, 0) : -1); //ne vaut -1 que pour Dirichlet
                    if (eb >= 0) for (auto &&f_v : zone.interp_dir(eb, t_x)) resu(f) -= fac * (l ? -1 : 1) * f_v.second * inco(f_v.first);
                    else if (ch.icl(fc, 0) == 3) for (r = 0; r < dimension; r++) //diffusion avec la CL
                        resu(f) -= fac * (l ? -1 : 1) * t_x[r] * ref_cast(Dirichlet, cls[ch.icl(fc, 1)].valeur()).val_imp(ch.icl(fc, 2), r);
                  }
              }
        }
  return resu;
}

//Description:
//on assemble la matrice.

inline void Op_Diff_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const DoubleVect& ve = zone.volumes(), &fs = zone.face_surfaces(), &pf = zone.porosite_face(), &vf = zone.volumes_entrelaces();
  const DoubleTab& tf = zone.face_tangentes(), &xp = zone.xp(), &xvm = zone.xvm(), &vfd = zone.volumes_entrelaces_dir();
  int i, j, k, l, r, e, eb, f, fb, fc;

  update_nu();

  /* qdm a la face f -> element amont/aval e -> face fb de e -> flux a la face fc -> elements amont/aval eb */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) < 2) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          std::array<double, 6> t_x = {{ 0, }}; //direction / position relative a l'element e
          for (r = 0; r < dimension; r++) t_x[r] = tf(f, r), t_x[3 + r] = xvm(f, r) - xp(e, r);
          /* decomposition du gradient a la face fb a l'aide de w1 */
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) for (k = zone.w1d(fb); k < zone.w1d(fb + 1); k++)
              {
                fc = zone.w1j(k);
                double fac = vfd(f, i) / ve(e) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) * zone.w1c(k) * nu_faces_(fc, 0) * pf(fc) * fs(fc) / vf(fc);
                for (l = 0; l < 2; l++)
                  {
                    eb = f_e(fc, l) >= 0 ? f_e(fc, l) : (ch.icl(fc, 0) < 3 ? f_e(fc, 0) : -1); //ne vaut -1 que pour Dirichlet
                    if (eb >= 0) for (auto &&f_v : zone.interp_dir(eb, t_x)) if (ch.icl(f_v.first, 0) < 2) matrice(f, f_v.first) += fac * (l ? -1 : 1) * f_v.second;
                  }
              }
        }
}

//Description:
//on ajoute la contribution du second membre.

void Op_Diff_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
