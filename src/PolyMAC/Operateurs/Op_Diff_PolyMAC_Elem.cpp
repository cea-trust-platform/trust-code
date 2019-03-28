/****************************************************************************
* Copyright (c) 2015, CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Op_Diff_PolyMAC_Elem.cpp
// Directory : $POLYMAC_ROOT/src/Operateurs
//
/////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_PolyMAC_Elem.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <cmath>
#include <Champ_P0_PolyMAC.h>
#include <Modele_turbulence_scal_base.h>

Implemente_instanciable( Op_Diff_PolyMAC_Elem, "Op_Diff_PolyMAC_Elem|Op_Dift_PolyMAC_P0_PolyMAC", Op_Diff_PolyMAC_base ) ;

Sortie& Op_Diff_PolyMAC_Elem::printOn( Sortie& os ) const
{
  Op_Diff_PolyMAC_base::printOn( os );
  return os;
}

Entree& Op_Diff_PolyMAC_Elem::readOn( Entree& is )
{
  Op_Diff_PolyMAC_base::readOn( is );
  return is;
}

void Op_Diff_PolyMAC_Elem::completer()
{
  Op_Diff_PolyMAC_base::completer();
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_PolyMAC_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.init_cl();
  int nb_comp = (equation().que_suis_je() == "Transport_K_Eps") ? 2 : 1;
  flux_bords_.resize(zone.premiere_face_int(), nb_comp);

  if (que_suis_je() == "Op_Diff_PolyMAC_Elem") return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
  const Champ_Fonc& alpha_t = mod_turb.diffusivite_turbulente();
  associer_diffusivite_turbulente(alpha_t);
}

void Op_Diff_PolyMAC_Elem::dimensionner(Matrice_Morse& mat) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = ch.valeurs().line_size();


  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  /* blocs inferieurs de la matrice : grad T = M2 / nu sauf aux faces de bord de Neumann */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) != 3 && ch.icl(f, 0) != 4)
      {
        //gradient
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (n = 0; n < N; n++) stencil.append_line(N * (ne_tot + f) + n, N * e + n);
        //m2
        for (i = zone.m2deb(f); i < zone.m2deb(f + 1); i++) for (n = 0; n < N; n++)
            stencil.append_line(N * (ne_tot + f) + n, N * (ne_tot + zone.m2ji(i, 0)) + n);
      }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * (ne_tot + nf_tot), N * (ne_tot + nf_tot), stencil, mat);
}

void Op_Diff_PolyMAC_Elem::calculer_flux_bord(const DoubleTab& inco) const
{
  abort();
}

// Description:
// ajoute la contribution de la diffusion au second membre resu
// renvoie resu
DoubleTab& Op_Diff_PolyMAC_Elem::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  //si on trouve Melange_Thermique_Aiguilles_PolyMAC dans les sources, alors on le laisse faire l'appel
  for (CONST_LIST_CURSEUR(Source) curseur(equation().sources()); curseur; ++curseur)
    if (curseur.valeur().que_suis_je() == "Melange_Thermique_Aiguilles_P0_PolyMAC") return resu;
  return ajouter_mod(inco, resu, la_zone_poly_.valeur().porosite_face());
}

/* version permettant a Melange_Thermique_Aiguilles_PolyMAC de modifier certaines porosites aux faces */
DoubleTab& Op_Diff_PolyMAC_Elem::ajouter_mod(const DoubleTab& inco,  DoubleTab& resu, const DoubleVect& pf_mod) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces_const();
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  int i, e, f, fb, ne_tot = zone.nb_elem_tot(), n, N = inco.line_size();

  remplir_nu_faces();
  for (f = 0; f < zone.nb_faces_tot(); f++) for (n = 0; n < N; n++) nu_faces.addr()[N * f + n] *= pf_mod(f);

  /* blocs inferieurs de la matrice : grad T - m2 / nu sauf si Neumann */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) != 3 && ch.icl(f, 0) != 4)
      {
        // grad T
        for (i = 0; i < 2; i++)
          if ((e = f_e(f, i)) >= 0) for (n = 0; n < N; n++)
              resu.addr()[N * (ne_tot + f) + n] -= (i ? 1 : -1) * fs(f) * inco.addr()[N * e + n];
          else if (ch.icl(f, 0) == 1 || ch.icl(f, 0) == 2) for (n = 0; n < N; n++) //Echange_impose_base
              resu.addr()[N * (ne_tot + f) + n] -= fs(f) * ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).T_ext(ch.icl(f, 2), n);
          else if (ch.icl(f, 0) == 5) for (n = 0; n < N; n++) //Dirichlet
              resu.addr()[N * (ne_tot + f) + n] -= fs(f) * ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), n);

        // m2 / nu
        for (i = zone.m2deb(f); i < zone.m2deb(f + 1); i++) for (n = 0, fb = zone.m2ji(i, 0); n < N; n++)
            {
              double fac = zone.m2ci(i) / nu_faces.addr()[N * fb + n];
              if (f == fb && (ch.icl(f, 0) == 1 || ch.icl(f, 0) == 2)) //si Echange_impose_base : on modifie le terme diagonal
                fac = (ch.icl(f, 0) == 1) * fac + fs(f) / ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n);
              resu.addr()[N * (ne_tot + f) + n] -= fac * inco.addr()[N * (ne_tot + fb) + n];
            }
      }


  /* remplissage de flux_bords */
  for (f = 0; f < zone.premiere_face_int(); f++) for (n = 0; n < N; n++) flux_bords_(f, n) = fs(f) * inco.addr()[N * (ne_tot + f) + n];

  return resu;
}

//Description:
//on assemble la matrice.
void Op_Diff_PolyMAC_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  //si on trouve Melange_Thermique_Aiguilles_PolyMAC dans les sources, alors on le laisse faire l'appel
  for (CONST_LIST_CURSEUR(Source) curseur(equation().sources()); curseur; ++curseur)
    if (curseur.valeur().que_suis_je() == "Melange_Thermique_Aiguilles_P0_PolyMAC") return;
  contribuer_a_avec_mod(inco, matrice, la_zone_poly_.valeur().porosite_face());
}

/* version permettant a Melange_Thermique_Aiguilles_PolyMAC de modifier certaines porosites aux faces */
void Op_Diff_PolyMAC_Elem::contribuer_a_avec_mod(const DoubleTab& inco, Matrice_Morse& matrice, const DoubleVect& pf_mod) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces_const();
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  int i, e, f, fb, ne_tot = zone.nb_elem_tot(), n, N = inco.line_size();

  remplir_nu_faces();
  for (f = 0; f < zone.nb_faces_tot(); f++) for (n = 0; n < N; n++) nu_faces.addr()[N * f + n] *= pf_mod(f);

  /* blocs inferieurs de la matrice : grad T = M2 / nu sauf si Neumann */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.icl(f, 0) != 3 && ch.icl(f, 0) != 4)
      {
        /* grad T */
        for (i = 0; i < 2; i++) if ((e = f_e(f, i)) >= 0)
            for (n = 0; n < N; n++) matrice(N * (ne_tot + f) + n, N * e + n) += (i ? 1 : -1) * fs(f);
        /* m2 / nu */
        for (i = zone.m2deb(f); i < zone.m2deb(f + 1); i++) for (n = 0, fb = zone.m2ji(i, 0); n < N; n++)
            {
              double fac = zone.m2ci(i) / nu_faces.addr()[N * fb + n];
              if (f == fb && (ch.icl(f, 0) == 1 || ch.icl(f, 0) == 2)) //si Echange_impose_base : on modifie le terme diagonal
                fac = (ch.icl(f, 0) == 1) * fac + fs(f) / ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n);
              matrice(N * (ne_tot + f) + n, N * (ne_tot + fb) + n) += fac;
            }
      }
}

void Op_Diff_PolyMAC_Elem::modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  //en principe, rien a faire!
  return;
}

//Description:
//on ajoute la contribution du second membre.
void Op_Diff_PolyMAC_Elem::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();
}
