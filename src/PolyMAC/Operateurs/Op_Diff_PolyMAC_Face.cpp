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

#include <Op_Diff_PolyMAC_Face.h>
#include <Dirichlet_homogene.h>
#include <Champ_Face_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Mod_turb_hyd_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Domaine_PolyMAC.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Symetrie.h>

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
  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_PolyMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.init_ra(), domaine.init_rf(), domaine.init_m1(), domaine.init_m2();

  if (que_suis_je() == "Op_Diff_PolyMAC_Face") return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
  associer_diffusivite_turbulente(alpha_t);
}

void Op_Diff_PolyMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const IntTab& e_f = domaine.elem_faces();
  int i, j, k, a, e, f, fb, nf_tot = domaine.nb_faces_tot(), na_tot = dimension < 3 ? domaine.domaine().nb_som_tot() : domaine.domaine().nb_aretes_tot(), idx;

  domaine.init_m2();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  //partie vitesses : m2 Rf
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = domaine.m2d(e), idx = 0; i < domaine.m2d(e + 1); i++, idx++)
      for (f = e_f(e, idx), j = domaine.m2i(i); f < domaine.nb_faces() && ch.icl(f, 0) < 2 && j < domaine.m2i(i + 1); j++)
        for (fb = e_f(e, domaine.m2j(j)), k = domaine.rfdeb(fb); k < domaine.rfdeb(fb + 1); k++) stencil.append_line(f, nf_tot + domaine.rfji(k));

  //partie vorticites : Ra m2 - m1 / nu
  for (a = 0; a < (dimension < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()); a++)
    {
      for (i = ch.radeb(a, 0); i < ch.radeb(a + 1, 0); i++) stencil.append_line(nf_tot + a, ch.raji(i));
      for (i = domaine.m1deb(a); i < domaine.m1deb(a + 1); i++) stencil.append_line(nf_tot + a, nf_tot + domaine.m1ji(i, 0));
    }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nf_tot + na_tot, nf_tot + na_tot, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Diff_PolyMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();
  int i, j, k, e, f, fb, a, nf_tot = domaine.nb_faces_tot(), idx;

  update_nu();
  //partie vitesses : m2 Rf
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = domaine.m2d(e), idx = 0; i < domaine.m2d(e + 1); i++, idx++)
      for (f = e_f(e, idx), j = domaine.m2i(i); f < domaine.nb_faces() && ch.icl(f, 0) < 2 && j < domaine.m2i(i + 1); j++)
        for (fb = e_f(e, domaine.m2j(j)), k = domaine.rfdeb(fb); k < domaine.rfdeb(fb + 1); k++)
          resu(f) -= domaine.m2c(j) * ve(e) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * pe(e) * domaine.rfci(k) * inco(nf_tot + domaine.rfji(k));

  //partie vorticites : Ra m2 - m1 / nu
  if (resu.dimension_tot(0) == nf_tot) return resu; //resu ne contient que la partie "faces"
  for (a = 0; a < (dimension < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()); a++)
    {
      //rotationnel : vitesses internes
      for (i = ch.radeb(a, 0); i < ch.radeb(a + 1, 0); i++)
        resu(nf_tot + a) -= ch.raci(i) * inco(ch.raji(i));
      //rotationnel : vitesses aux bords
      for (i = ch.radeb(a, 1); i < ch.radeb(a + 1, 1); i++)
        for (k = 0; k < dimension; k++)
          resu(nf_tot + a) -= ch.racf(i, k) * ref_cast(Dirichlet, cls[ch.icl(ch.rajf(i), 1)].valeur()).val_imp(ch.icl(ch.rajf(i), 2), k);
      // -m1 / nu
      for (i = domaine.m1deb(a); i < domaine.m1deb(a + 1); i++)
        resu(nf_tot + a) += domaine.m1ci(i) / (pe(domaine.m1ji(i, 1)) * nu_(domaine.m1ji(i, 1), 0)) * inco(nf_tot + domaine.m1ji(i, 0));
    }
  return resu;
}

/*! @brief on assemble la matrice.
 *
 */
inline void Op_Diff_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const DoubleVect& pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();
  int i, j, k, e, f, fb, a, nf_tot = domaine.nb_faces_tot(), idx;

  update_nu();
  //partie vitesses : m2 Rf
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = domaine.m2d(e), idx = 0; i < domaine.m2d(e + 1); i++, idx++)
      for (f = e_f(e, idx), j = domaine.m2i(i); f < domaine.nb_faces() && ch.icl(f, 0) < 2 && j < domaine.m2i(i + 1); j++)
        for (fb = e_f(e, domaine.m2j(j)), k = domaine.rfdeb(fb); k < domaine.rfdeb(fb + 1); k++)
          matrice(f, nf_tot + domaine.rfji(k)) += domaine.m2c(j) * ve(e) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * pe(e) * domaine.rfci(k);

  //partie vorticites : Ra m2 - m1 / nu
  for (a = 0; a < (dimension < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()); a++)
    {
      //rotationnel : vitesses internes
      for (i = ch.radeb(a, 0); i < ch.radeb(a + 1, 0); i++)
        if (ch.icl(f = ch.raji(i), 0) < 2)
          matrice(nf_tot + a, f) += ch.raci(i);
      // -m1 / nu
      for (i = domaine.m1deb(a); i < domaine.m1deb(a + 1); i++)
        matrice(nf_tot + a, nf_tot + domaine.m1ji(i, 0)) -= domaine.m1ci(i) / (pe(domaine.m1ji(i, 1)) * nu_(domaine.m1ji(i, 1), 0));
    }
}

/*! @brief on ajoute la contribution du second membre.
 *
 */
void Op_Diff_PolyMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
