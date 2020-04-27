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
  ch.init_ra(), zone.init_rf(), zone.init_m1(), zone.init_m2();

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
  const IntTab& e_f = zone.elem_faces();
  int i, j, k, a, e, f, fb, nf_tot = zone.nb_faces_tot(), na_tot = dimension < 3 ? zone.zone().nb_som_tot() : zone.zone().nb_aretes_tot(), idx;

  zone.init_m2();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  //partie vitesses : m2 Rf
  for (e = 0; e < zone.nb_elem_tot(); e++) for (i = zone.m2d(e), idx = 0; i < zone.m2d(e + 1); i++, idx++)
      for (f = e_f(e, idx), j = zone.m2i(i); f < zone.nb_faces() && ch.icl(f, 0) < 2 && j < zone.m2i(i + 1); j++)
        for (fb = e_f(e, zone.m2j(j)), k = zone.rfdeb(fb); k < zone.rfdeb(fb + 1); k++) stencil.append_line(f, nf_tot + zone.rfji(k));

  //partie vorticites : Ra m2 - m1 / nu
  for (a = 0; a < (dimension < 3 ? zone.nb_som() : zone.zone().nb_aretes()); a++)
    {
      for (i = ch.radeb(a, 0); i < ch.radeb(a + 1, 0); i++) stencil.append_line(nf_tot + a, ch.raji(i));
      for (i = zone.m1deb(a); i < zone.m1deb(a + 1); i++) stencil.append_line(nf_tot + a, nf_tot + zone.m1ji(i, 0));
    }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nf_tot + na_tot, nf_tot + na_tot, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Diff_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const DoubleVect& pe = zone.porosite_elem(), &ve = zone.volumes();
  int i, j, k, e, f, fb, a, nf_tot = zone.nb_faces_tot(), idx;

  update_nu();
  //partie vitesses : m2 Rf
  for (e = 0; e < zone.nb_elem_tot(); e++) for (i = zone.m2d(e), idx = 0; i < zone.m2d(e + 1); i++, idx++)
      for (f = e_f(e, idx), j = zone.m2i(i); f < zone.nb_faces() && ch.icl(f, 0) < 2 && j < zone.m2i(i + 1); j++)
        for (fb = e_f(e, zone.m2j(j)), k = zone.rfdeb(fb); k < zone.rfdeb(fb + 1); k++)
          resu(f) -= zone.m2c(j) * ve(e) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * pe(e) * zone.rfci(k) * inco(nf_tot + zone.rfji(k));

  //partie vorticites : Ra m2 - m1 / nu
  if (resu.dimension_tot(0) == nf_tot) return resu; //resu ne contient que la partie "faces"
  for (a = 0; a < (dimension < 3 ? zone.nb_som() : zone.zone().nb_aretes()); a++)
    {
      //rotationnel : vitesses internes
      for (i = ch.radeb(a, 0); i < ch.radeb(a + 1, 0); i++)
        resu(nf_tot + a) -= ch.raci(i) * inco(ch.raji(i));
      //rotationnel : vitesses aux bords
      for (i = ch.radeb(a, 1); i < ch.radeb(a + 1, 1); i++) for (k = 0; k < dimension; k++)
          resu(nf_tot + a) -= ch.racf(i, k) * ref_cast(Dirichlet, cls[ch.icl(ch.rajf(i), 1)].valeur()).val_imp(ch.icl(ch.rajf(i), 2), k);
      // -m1 / nu
      for (i = zone.m1deb(a); i < zone.m1deb(a + 1); i++)
        resu(nf_tot + a) += zone.m1ci(i) / (pe(zone.m1ji(i, 1)) * nu_(zone.m1ji(i, 1))) * inco(nf_tot + zone.m1ji(i, 0));
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
  const DoubleVect& pe = zone.porosite_elem(), &ve = zone.volumes();
  int i, j, k, e, f, fb, a, nf_tot = zone.nb_faces_tot(), idx;

  update_nu();
  //partie vitesses : m2 Rf
  for (e = 0; e < zone.nb_elem_tot(); e++) for (i = zone.m2d(e), idx = 0; i < zone.m2d(e + 1); i++, idx++)
      for (f = e_f(e, idx), j = zone.m2i(i); f < zone.nb_faces() && ch.icl(f, 0) < 2 && j < zone.m2i(i + 1); j++)
        for (fb = e_f(e, zone.m2j(j)), k = zone.rfdeb(fb); k < zone.rfdeb(fb + 1); k++)
          matrice(f, nf_tot + zone.rfji(k)) += zone.m2c(j) * ve(e) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * pe(e) * zone.rfci(k);

  //partie vorticites : Ra m2 - m1 / nu
  for (a = 0; a < (dimension < 3 ? zone.nb_som() : zone.zone().nb_aretes()); a++)
    {
      //rotationnel : vitesses internes
      for (i = ch.radeb(a, 0); i < ch.radeb(a + 1, 0); i++) if (ch.icl(f = ch.raji(i), 0) < 2)
          matrice(nf_tot + a, f) += ch.raci(i);
      // -m1 / nu
      for (i = zone.m1deb(a); i < zone.m1deb(a + 1); i++)
        matrice(nf_tot + a, nf_tot + zone.m1ji(i, 0)) -= zone.m1ci(i) / (pe(zone.m1ji(i, 1)) * nu_(zone.m1ji(i, 1)));
    }
}

//Description:
//on ajoute la contribution du second membre.

void Op_Diff_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
