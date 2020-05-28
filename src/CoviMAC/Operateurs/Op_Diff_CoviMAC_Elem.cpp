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
#include <MD_Vector_base.h>
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

//appelle update_nu() de Op_Diff_CoviMAC_base et met a jour les interpolations
void Op_Diff_CoviMAC_Elem::update_nu(IntTab *tpfa) const
{
  if (nu_a_jour_) return; //travail deja fait
  Op_Diff_CoviMAC_base::update_nu(tpfa);
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  zone.interp_flux(zone.nb_faces(), nu_, ch.valeurs().line_size(), ch.icl, { 0, 1, 1, 1, 1, 1, 0, 0 }, fef_ce, fef_cf, tpfa);
  nu_a_jour_ = 1;
}

void Op_Diff_CoviMAC_Elem::dimensionner(Matrice_Morse& mat) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, f, n, N = ch.valeurs().line_size(), N_nu = nu_.line_size();

  IntTrav stencil(0, 2), tpfa;
  stencil.set_smart_resize(1), zone.creer_tableau_faces(tpfa);

  update_nu(&tpfa); //

  Cerr << "Op_Diff_CoviMAC_Elem::dimensionner() : ";

  /* dependance du (nu.grad T) aux faces (hors Neumann) en les variables aux elements */
  /* si nu est constant ou si nu est isotrope avec tpfa, alors on peut reduire le stencil */
  for (f = 0; f < zone.nb_faces(); f++) if (!ch.icl(f, 0) || ch.icl(f, 0) > 5)
      for (i = zone.fef_d(f, 0); i < zone.fef_d(f + 1, 0); i++) for (n = 0; n < N; n++)
          if ((nu_constant_ || (N_nu <= N && tpfa(f))) ? (dabs(fef_ce(i, n)) > 1e-12) : 1)
            for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
              stencil.append_line(N * e + n, N * zone.fef_e(i) + n);

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * zone.zone().md_vector_elements().valeur().nb_items_seq_tot())
       << " " << mp_somme_vect(tpfa) * 100. / zone.md_vector_faces().valeur().nb_items_seq_tot() << "% TPFA " << finl;
  Matrix_tools::allocate_morse_matrix(N * zone.nb_elem_tot(), N * zone.nb_elem_tot(), stencil, mat);
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
  update_nu();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales();
  int i, e, f, fb, n, N = inco.line_size();

  /* tableau contenant les valeurs aux faces de bord (valeurs imposees ou flux) */
  DoubleTrav val_fb(zone.nb_faces_tot(), N), nu_nf(N, dimension);
  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (!ch.icl(f, 0)) continue; //face interne
    else if (ch.icl(f, 0) < 3) for (n = 0, zone.nu_prod(e = f_e(f, 0), nu_, &nf(f, 0), nu_nf); n < N; n++) //Echange_impose_base -> flux a deux points
        {
          double invh = 1. / ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n);
          if (ch.icl(f, 0) == 1) invh += zone.dist_norm_bord(f) * fs(f) * fs(f) / zone.dot(&nf(f, 0), &nu_nf(n, 0));
          val_fb(f, n) = (inco.addr()[N * e + n] - ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).T_ext(ch.icl(f, 2), n)) / invh;
        }
    else if (ch.icl(f, 0) == 3) abort(); //monolithique
    else if (ch.icl(f, 0) < 6) for (n = 0; n < N; n++) //Neumann
        val_fb(f, n) = ref_cast(Neumann, cls[ch.icl(f, 1)].valeur()).flux_impose(ch.icl(f, 2), n);
    else if (ch.icl(f, 0) == 6) for (n = 0; n < N; n++) //Dirichlet
        val_fb(f, n) = ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), n);

  /* contribution face par face */
  DoubleTrav phi(N);
  for (f = 0; f < zone.nb_faces(); f++)
    {
      if (!ch.icl(f, 0) || ch.icl(f, 0) > 5) //interne ou Dirichlet -> interpolations
        {
          /* partie "elements" : avec inco */
          for (i = zone.fef_d(f, 0), phi = 0; i < zone.fef_d(f + 1, 0); i++) for (n = 0, e = zone.fef_e(i); n < N; n++)
              phi(n) += fef_ce(i, n) * inco.addr()[N * e + n];
          /* partie "faces de bord" : avec val_fb */
          for (i = zone.fef_d(f, 1); i < zone.fef_d(f + 1, 1); i++) for (n = 0, fb = zone.fef_f(i); n < N; n++)
              phi(n) += fef_cf(i, n) * val_fb(fb, n);
          phi *= nu_fac_(f); //prise en compte de nu_fac_
        }
      else for (n = 0; n < N; n++) phi(n) = val_fb(f, n); //Echange_impose_base ou Neumann -> val_fb

      phi *= -fs(f); //phi = - nu grad T
      if (f < zone.premiere_face_int()) for (n = 0; n < N; n++) flux_bords_(f, n) = phi(n);
      //contributions
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (n = 0; n < N; n++)
          resu.addr()[N * e + n] -= (i ? -1 : 1) * phi(n);
    }

  return resu;
}

//Description:
//on assemble la matrice.
void Op_Diff_CoviMAC_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  update_nu();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales();
  int i, e, f, fb, n, N = inco.line_size(), do_nu_fac;

  /* tableau contenant les derivees des valeurs aux faces par rapport a leur element voisin (Echange_impose_base seulement) */
  DoubleTrav dval_fb(zone.nb_faces_tot(), N), nu_nf(N, dimension);
  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (ch.icl(f, 0) && ch.icl(f, 0) < 3) for (n = 0, zone.nu_prod(e = f_e(f, 0), nu_, &nf(f, 0), nu_nf); n < N; n++)
        {
          double invh = 1. / ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n);
          if (ch.icl(f, 0) == 1) invh += zone.dist_norm_bord(f) * fs(f) * fs(f) / zone.dot(&nf(f, 0), &nu_nf(n, 0));
          dval_fb(f, n) = 1. / invh;
        }

  /* contribution face par face */
  std::vector<std::map<int, double>> dphi(N); //dphi[n][e] : derivee du flux a la face de la composante n par rapport a l'element e
  for (f = 0; f < zone.nb_faces(); f++)
    {
      for (n = 0, do_nu_fac = 0; n < N; n++) dphi[n].clear();
      if (!ch.icl(f, 0) || ch.icl(f, 0) > 5) //interne ou Dirichlet -> interpolations
        {
          /* partie "elements" : avec inco */
          for (i = zone.fef_d(f, 0), do_nu_fac = 1; i < zone.fef_d(f + 1, 0); i++) for (n = 0, e = zone.fef_e(i); n < N; n++)
              if (dabs(fef_ce(i, n)) > 1e-12) dphi[n][e] += fef_ce(i, n);
          /* partie "faces de bord" : avec val_fb */
          for (i = zone.fef_d(f, 1); i < zone.fef_d(f + 1, 1); i++) for (n = 0, fb = zone.fef_f(i); n < N; n++)
              if (dabs(fef_cf(i, n) * dval_fb(fb, n)) > 1e-12) dphi[n][f_e(fb, 0)] += fef_cf(i, n) * dval_fb(fb, n);
        }
      else for (n = 0; n < N; n++) if (dval_fb(f, n)) dphi[n][f_e(f, 0)] += dval_fb(f, n); //Echange_impose_base ou Neumann -> val_fb

      //contributions
      for (n = 0; n < N; n++) for (auto &&e_c : dphi[n]) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            matrice(N * e + n, N * e_c.first + n) += (i ? 1 : -1) * fs(f) * (do_nu_fac ? nu_fac_(f) : 1) * e_c.second;
    }
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
