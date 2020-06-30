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
// File:        Op_Conv_EF_Stab_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_Stab_CoviMAC_Face.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_Face_CoviMAC.h>
#include <Param.h>
#include <cmath>

Implemente_instanciable_sans_constructeur( Op_Conv_EF_Stab_CoviMAC_Face, "Op_Conv_EF_Stab_CoviMAC_Face_CoviMAC", Op_Conv_CoviMAC_base ) ;

Op_Conv_EF_Stab_CoviMAC_Face::Op_Conv_EF_Stab_CoviMAC_Face()
{
  alpha = 1; //par defaut, on fait de l'amont
}

// XD Op_Conv_EF_Stab_CoviMAC_Face interprete Op_Conv_EF_Stab_CoviMAC_Face 1 Class Op_Conv_EF_Stab_CoviMAC_Face_CoviMAC
Sortie& Op_Conv_EF_Stab_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Entree& Op_Conv_EF_Stab_CoviMAC_Face::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Conv_EF_Stab_CoviMAC_Face::completer()
{
  Op_Conv_CoviMAC_base::completer();
  /* au cas ou... */
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Conv_EF_Stab_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  ch.init_cl();
}

void Op_Conv_EF_Stab_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_face();
  int i, j, k, e, eb, f, fb, r, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* agit uniquement aux elements; diagonale omise */
  for (r = 0; r < dimension; r++) for (f = 0; f < zone.nb_faces(); f++)
    for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
      {
        for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++)  //elem->face
          stencil.append_line(fb, nf_tot + r * ne_tot + eb);
        stencil.append_line(nf_tot + r * ne_tot + e, nf_tot + r * ne_tot + eb); //elem->elem
      }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nf_tot + dimension * ne_tot, nf_tot + dimension * ne_tot, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Conv_EF_Stab_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_face();
  const DoubleTab& vit = vitesse_->valeurs(), &nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  const DoubleVect& fs = zone.face_surfaces(), &pe = zone.porosite_elem();
  int i, j, e, f, fb, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  /* operateur aux elements dans chaque direction */
  DoubleTrav fac(D), pfac(D);
  for (f = 0; f < zone.nb_faces(); f++)
      {
        for (fac = 0, pfac = 0, i = 0; i < 2; i++, fac += pfac, pfac = 0)
          {
            e = f_e(f, i) >= 0 ? f_e(f, i) : (ch.fcl(f, 0) == 1 ? f_e(f, 0) : -1);
            if (e >= 0) for (d = 0; d < D; d++) pfac(d) = pe(e) * inco(nf_tot + ne_tot * d + e);
            else if (ch.fcl(f, 0) == 3) for (d = 0; d < D; d++) pfac(d) = ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), d);
            else pfac = 0;
            pfac *= (1. + (vit(f) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
          }
        fac *= fs(f) * vit(f);
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) //elem -> face
              resu(fb) -= (i ? -1 : 1) * mu_f(fb, 0, e != f_e(fb, 0)) * zone.dot(fac, &nf(fb, 0)) / fs(f);
            for (d = 0; d < D; d++) resu(nf_tot + ne_tot * d + e) -= (i ? -1 : 1) * fac(d); //elem->elem
          }
      }

  return resu;
}

//Description:
//on assemble la matrice.

inline void Op_Conv_EF_Stab_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_face();
  const DoubleTab& vit = vitesse_->valeurs(), &nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  const DoubleVect& fs = zone.face_surfaces(), &pe = zone.porosite_elem();
  int i, j, e, f, fb, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  /* operateur aux elements dans chaque direction */
  DoubleTrav dfac(2, D), pdfac(2, D);
  for (f = 0; f < zone.nb_faces(); f++)
      {
        for (i = 0, dfac = 0, pdfac = 0; i < 2; i++, dfac += pdfac, pdfac = 0)
          {
            if ((e = f_e(f, i)) >= 0) for (d = 0; d < D; d++) pdfac(i, d) = pe(e);
            else if (ch.fcl(f, 0) == 1) for (d = 0; d < D; d++) pdfac(0, d) = pe(f_e(f, 0));
            pdfac *= (1. + (vit(f) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
          }
        dfac *= fs(f) * vit(f);
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces()) for (j = 0; j < 2; j++) for (d = 0; d < D; d++) //elem -> face
              matrice(fb, nf_tot + ne_tot * d + f_e(f, j)) += (i ? 1 : -1) * mu_f(fb, 0, e != f_e(fb, 0)) * dfac(j, d) * nf(fb, d) / fs(f);
            if (e < zone.nb_elem()) for (j = 0; j < 2; j++) for (d = 0; d < D; d++) //elem -> elem
              matrice(nf_tot + ne_tot * d + e, nf_tot + ne_tot * d + f_e(f, j)) += (i ? -1 : 1) * dfac(j, d);
          }
      }
}
//Description:
//on ajoute la contribution du second membre.

void Op_Conv_EF_Stab_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
