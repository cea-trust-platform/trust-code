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
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Conv_EF_Stab_CoviMAC_Face : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  ch.init_cl();
}

void Op_Conv_EF_Stab_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& xp = zone.xp(), &xvm = zone.xvm(), &tf = zone.face_tangentes();
  int i, j, k, e, eb, f, fb, r;

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* qdm a la face f -> element amont/aval e de f -> face fb de e -> element amont/aval eb de fb */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) < 2) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          std::array<double, 6> t_x = {{ 0, }}; //direction / position relative a l'element e
          for (r = 0; r < dimension; r++) t_x[r] = tf(f, r), t_x[3 + r] = xvm(f, r) - xp(e, r);
          /* dependance par rapport a l'interpolation dans l'element de l'autre cote de chaque face fb de e */
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++)
              for (auto &&f_v : zone.interp_dir(eb, t_x, 1)) if (ch.icl(f_v.first, 0) < 2) stencil.append_line(f, f_v.first);
        }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(zone.nb_faces_tot(), zone.nb_faces_tot(), stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Conv_EF_Stab_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& xp = zone.xp(), &xvm = zone.xvm(), &tf = zone.face_tangentes(), &vfd = zone.volumes_entrelaces_dir(), &vit = vitesse_->valeurs_normales();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &pe = zone.porosite_elem();

  int i, j, k, e, f, fb, r;

  /* qdm a la face f -> element amont/aval e de f -> face fb de e -> element amont/aval eb de fb */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) < 2) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          std::array<double, 6> t_x = {{ 0, }}; //direction / position relative a l'element e
          for (r = 0; r < dimension; r++) t_x[r] = tf(f, r), t_x[3 + r] = xvm(f, r) - xp(e, r);
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) for (k = 0; k < 2; k++)
              {
                int eb = f_e(fb, k) >= 0 ? f_e(fb, k) : (ch.icl(fb, 0) == 1 ? e : -1); //eb = -1 si Symetrie ou Dirichlet
                double fac = vfd(f, i) / ve(e) * fs(fb) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * (1. + (vit(fb) * (k ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
                if (!fac) continue;
                if (eb >= 0) for (auto &&f_v : zone.interp_dir(eb, t_x)) resu(f) -= fac * f_v.second * pe(eb) * inco(f_v.first);
                else if (ch.icl(fb, 0) == 3) for (r = 0; r < dimension; r++) //face de Dirichlet -> convection de la CL
                    resu(f) -= fac * t_x[r] * ref_cast(Dirichlet, cls[ch.icl(fb, 1)].valeur()).val_imp(ch.icl(fb, 2), r);
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
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& xp = zone.xp(), &xvm = zone.xvm(), &tf = zone.face_tangentes(), &vfd = zone.volumes_entrelaces_dir(), &vit = vitesse_->valeurs_normales();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &pe = zone.porosite_elem();
  int i, j, k, e, f, fb, r;

  /* qdm a la face f -> element amont/aval e de f -> face fb de e -> element amont/aval eb de fb */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) < 2) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          std::array<double, 6> t_x = {{ 0, }}; //direction / position relative a l'element e
          for (r = 0; r < dimension; r++) t_x[r] = tf(f, r), t_x[3 + r] = xvm(f, r) - xp(e, r);
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) for (k = 0; k < 2; k++)
              {
                int eb = f_e(fb, k) >= 0 ? f_e(fb, k) : (ch.icl(fb, 0) == 1 ? e : -1); //eb = -1 si Symetrie ou Dirichlet
                double fac = vfd(f, i) / ve(e) * fs(fb) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * (1. + (vit(fb) * (k ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
                if (!fac || eb < 0) continue;
                for (auto &&f_v : zone.interp_dir(eb, t_x)) if(ch.icl(f_v.first, 0) < 2) matrice(f, f_v.first) += fac * f_v.second * pe(eb);
              }
        }
}

//Description:
//on ajoute la contribution du second membre.

void Op_Conv_EF_Stab_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
