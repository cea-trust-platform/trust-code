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
// File:        Op_Diff_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_CoviMAC_Elem.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_contact_CoviMAC.h>
#include <Echange_externe_impose.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_CoviMAC.h>
#include <Champ_front_calc.h>
#include <Modele_turbulence_scal_base.h>
#include <Synonyme_info.h>
#include <communications.h>
#include <cmath>

Implemente_instanciable( Op_Diff_CoviMAC_Elem          , "Op_Diff_CoviMAC_Elem|Op_Diff_CoviMAC_var_Elem"                                , Op_Diff_CoviMAC_base ) ;
Implemente_instanciable( Op_Dift_CoviMAC_Elem          , "Op_Dift_CoviMAC_P0_CoviMAC|Op_Dift_CoviMAC_var_P0_CoviMAC"                    , Op_Diff_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Diff_Nonlinear_CoviMAC_Elem, "Op_Diff_nonlinear_CoviMAC_Elem|Op_Diff_nonlinear_CoviMAC_var_Elem"            , Op_Diff_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Dift_Nonlinear_CoviMAC_Elem, "Op_Dift_CoviMAC_P0_CoviMAC_nonlinear|Op_Dift_CoviMAC_var_P0_CoviMAC_nonlinear", Op_Diff_CoviMAC_Elem ) ;

Sortie& Op_Diff_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Dift_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Diff_Nonlinear_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Dift_Nonlinear_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Entree& Op_Diff_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Diff_Nonlinear_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Dift_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Dift_Nonlinear_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

void Op_Diff_CoviMAC_Elem::completer()
{
  Op_Diff_CoviMAC_base::completer();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.init_cl();
  int nb_comp = (equation().que_suis_je() == "Transport_K_Eps") ? 2 : 1;
  flux_bords_.resize(zone.premiere_face_int(), nb_comp);

  if (!que_suis_je().debute_par("Op_Dift")) return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
  const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
  associer_diffusivite_turbulente(lambda_t);
}

void Op_Diff_CoviMAC_Elem::dimensionner(Matrice_Morse& mat) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, k, e, eb, f, fb, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = ch.valeurs().line_size();

  zone.init_w1();

  /* pour chaque W1_{ff'}, l'amont/aval de f dependent de l'amont/aval de f' */
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (f = 0; f < nf_tot; f++) for (j = zone.w1d(f); j < zone.w1d(f + 1); j++)
      for (i = 0, fb = zone.w1j(j); i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++)
            for (n = 0; n < N; n++) stencil.append_line(N * e + n, N * eb + n);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * ne_tot, N * ne_tot, stencil, mat);
}

void Op_Diff_CoviMAC_Elem::calculer_flux_bord(const DoubleTab& inco) const
{
  abort();
}

// Description:
// ajoute la contribution de la diffusion au second membre resu
// renvoie resu
DoubleTab& Op_Diff_CoviMAC_Elem::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  int i, j, e, f, n, N = inco.line_size();

  //prerequis : nu aux faces
  update_nu();
  const DoubleTab& nu_faces = get_nu_faces();
  DoubleTrav nu_grad(zone.nb_faces_tot(), N), phi(N); //champ -nu grad(T) -> a interpoler avec W1, flux

  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (ch.icl(f, 0) == 0) for (n = 0; n < N; n++) nu_grad(f, n) = nu_faces(f, n) * (inco.addr()[N * f_e(f, 0) + n] - inco.addr()[N * f_e(f, 1) + n]) * fs(f) / vf(f);
    else if (ch.icl(f, 0) < 3) for (n = 0; n < N; n++) //Echange_impose_base
        nu_grad(f, n) = (inco.addr()[N * f_e(f, 0) + n] - ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).T_ext(ch.icl(f, 2), n))
                        / (1. / ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n) + (ch.icl(f, 0) < 2 ? vf(f) / (nu_faces(f, n) * fs(f)) : 0));
    else if (ch.icl(f, 0) == 3) abort(); //monolithique
    else if (ch.icl(f, 0) < 6) for (n = 0; n < N; n++) //Neumann
        nu_grad(f, n) = ref_cast(Neumann, cls[ch.icl(f, 1)].valeur()).flux_impose(ch.icl(f, 2), n);
    else for (n = 0; n < N; n++) //Dirichlet
        nu_grad(f, n) = nu_faces(f, n) * (inco.addr()[N * f_e(f, 0) + n] - ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), n)) * fs(f) / vf(f);

  /* interpolation et divergence */
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      for (j = zone.w1d(f), phi = 0; j < zone.w1d(f + 1); j++) for (n = 0; n < N; n++) phi(n) += zone.w1c(j) * nu_grad(zone.w1j(j), n);
      for (i = 0, phi *= fs(f); i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem())
          for (n = 0; n < N; n++) resu.addr()[N * e + n] -= (i ? -1 : 1) * phi(n);
      for (n = 0; f < zone.premiere_face_int() && n < N; n++) flux_bords_(f, n) = phi(n);
    }

  return resu;
}

//Description:
//on assemble la matrice.
void Op_Diff_CoviMAC_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  int i, j, k, e, eb, f, fb, n, N = inco.line_size();

  //prerequis : nu aux faces
  update_nu();
  const DoubleTab& nu_faces = get_nu_faces();
  DoubleTrav dnu_grad(zone.nb_faces_tot(), N); //derivee du champ -nu grad(T) -> a interpoler avec W1, flux

  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (ch.icl(f, 0) == 0 || ch.icl(f, 0) > 5) for (n = 0; n < N; n++) dnu_grad(f, n) = nu_faces(f, n) * fs(f) / vf(f); //interne ou Dirichlet
    else if (ch.icl(f, 0) < 3) for (n = 0; n < N; n++) //Echange_impose_base
        dnu_grad(f, n) = 1. / (1. / ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n) + (ch.icl(f, 0) < 2 ? vf(f) / (nu_faces(f, n) * fs(f)) : 0));
    else if (ch.icl(f, 0) == 3) abort(); //monolithique
    else if (ch.icl(f, 0) < 6) for (n = 0; n < N; n++) dnu_grad(f, n) = 0;//Neumann

  /* interpolation et divergence */
  for (f = 0; f < zone.nb_faces_tot(); f++) for (j = zone.w1d(f); j < zone.w1d(f + 1); j++)
      for (i = 0, fb = zone.w1j(j); i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++)
            for (n = 0; n < N; n++) matrice(N * e + n, N * eb + n) += (i ? 1 : -1) * (k ? 1 : -1) * fs(f) * zone.w1c(j) * dnu_grad(fb, n);
  i++;
}

void Op_Diff_CoviMAC_Elem::modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  //en principe, rien a faire!
  return;
}

//Description:
//on ajoute la contribution du second membre.
void Op_Diff_CoviMAC_Elem::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();
}
