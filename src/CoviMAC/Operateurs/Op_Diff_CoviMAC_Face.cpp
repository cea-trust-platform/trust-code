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
  ch.init_cl(), zone.init_w2();

  if (que_suis_je() == "Op_Diff_CoviMAC_Face") return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
  associer_diffusivite_turbulente(alpha_t);
}

void Op_Diff_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  // const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  // const IntTab& f_e = zone.face_voisins();
  // int i, j, k, r, e, eb, f, fb, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  //
  // IntTab stencil(0, 2);
  // stencil.set_smart_resize(1);
  //
  // /* operateur aux elements dans chaque direction */
  // for (f = 0; f < zone.nb_faces(); f++) for (i = zone.w1d(f); i < zone.w1d(f + 1); i++)
  //     for (j = 0, fb = zone.w1j(i); j < 2 && (e = f_e(f, j)) >= 0; j++) if (e < zone.nb_elem())
  //         for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++) if (eb < ne_tot) for (r = 0; r < dimension; r++)
  //               stencil.append_line(nf_tot + ne_tot * r + e, nf_tot + ne_tot * r + eb);
  //
  // tableau_trier_retirer_doublons(stencil);
  // Matrix_tools::allocate_morse_matrix(nf_tot + dimension * ne_tot, nf_tot + dimension * ne_tot, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Diff_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  // const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  // const IntTab& f_e = zone.face_voisins();
  // const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  // const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  // const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  // int i, r, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  //
  // update_nu();
  // DoubleTrav nu_grad(nf_tot, dimension), phi(dimension); //champ -nu grad v aux faces duales
  //
  // /* operateur aux elements dans chaque direction */
  // // - nu grad v_i aux faces duales
  // for (f = 0; f < zone.nb_faces_tot(); f++) if (!ch.icl(f, 0) || ch.icl(f, 0) > 2) //pas de flux si Neumann ou Symetrie
  //     {
  //       for (i = 0; i < 2; i++) if ((e = f_e(f, i)) >= 0 && e < ne_tot) for (r = 0; r < dimension; r++) //contribution de l'element
  //             nu_grad(f, r) += (i ? -1 : 1) * inco(nf_tot + ne_tot * r + e);
  //         else if (ch.icl(f, 0) == 3) for (r = 0; r < dimension; r++) //diffusion avec la CL
  //             nu_grad(f, r) += (i ? -1 : 1) * ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), r);
  //       for (r = 0; r < dimension; r++) nu_grad(f, r) *= nu_faces_(f, 0) * fs(f) / vf(f);
  //     }
  //
  // //interpolation et divergence
  // for (f = 0; f < zone.nb_faces(); f++)
  //   {
  //     for (i = zone.w1d(f), phi = 0; i < zone.w1d(f + 1); i++) for (r = 0; r < dimension; r++) phi(r) += zone.w1c(i) * nu_grad(zone.w1j(i), r);
  //     for (i = 0, phi *= fs(f); i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem())
  //         for (r = 0; r < dimension; r++) resu(nf_tot + ne_tot * r + e) -= (i ? -1 : 1) * phi(r);
  //   }

  return resu;
}

//Description:
//on assemble la matrice.

inline void Op_Diff_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  // const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  // const IntTab& f_e = zone.face_voisins();
  // const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  // const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  // int i, j, k, r, e, eb, f, fb, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  //
  // update_nu();
  // DoubleTrav dnu_grad(nf_tot, dimension); //derivee du champ -nu grad v aux faces duales
  //
  // /* operateur aux elements dans chaque direction */
  // // - nu grad v_i aux faces duales :
  // for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.icl(f, 0) == 0 || ch.icl(f, 0) > 2)
  //     for (r = 0; r < dimension; r++) dnu_grad(f, r) = nu_faces_(f, 0) * fs(f) / vf(f);
  //
  // //interpolation et divergence
  // for (f = 0; f < zone.nb_faces(); f++) for (j = zone.w1d(f); j < zone.w1d(f + 1); j++)
  //     for (i = 0, fb = zone.w1j(j); i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem())
  //         for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++) if (eb < ne_tot) for (r = 0; r < dimension; r++)
  //               matrice(nf_tot + ne_tot * r + e, nf_tot + r * ne_tot + eb) += (i ? 1 : -1) * (k ? 1 : -1) * fs(f) * zone.w1c(j) * dnu_grad(fb, r);
}

//Description:
//on ajoute la contribution du second membre.

void Op_Diff_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
