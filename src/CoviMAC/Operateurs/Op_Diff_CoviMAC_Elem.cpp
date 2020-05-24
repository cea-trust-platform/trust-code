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
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Diff_CoviMAC_Elem : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();
  ch.init_cl(), zone.init_w2(), zone.init_finterp();
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
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  int i, j, k, l, m, e, eb, f, fb, n, N = ch.valeurs().line_size();


  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  if (nu_constant_) update_nu(); //on peut faire le stencil avec les interpolations finales

  Cerr << "Op_Diff_CoviMAC_Elem::dimensionner() : ";

  /*  element e -> contribution au flux a la face f vers l'element eb -> depend de la valeur a la face fb -> hybride ou interpolee */
  for (e = 0; e < zone.nb_elem_tot(); e++) for (i = 0, j = zone.w2d(e); j < zone.w2d(e + 1); i++, j++) if ((f = e_f(e, i)) < zone.nb_faces())
        for (k = zone.w2i(j); k < zone.w2i(j + 1); k++) for (fb = e_f(e, zone.w2j(k)), l = 0; l < 1 + (ch.icl(f, 0) < 6); l++)
            if ((eb = f_e(f, l)) < zone.nb_elem() || ch.icl(f, 0))
              {
                if (nu_constant_) //nu constant -> on prend les vraies interpolations
                  {
                    for (n = 0; n < N; n++) for (m = 0; m <= dimension; m++) if (tfi(fb, n, m) >= 0)
                          stencil.append_line(N * eb + n, N * tfi(fb, n, m) + n);
                  }
                else for (m = zone.fid(fb); m < zone.fid(fb + 1); m++) //nu variable -> on autorise tous les interpolants possibles
                    for (n = 0; n < N; n++) stencil.append_line(N * eb + n, N * zone.fie(m) + n);
              }

  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2; i++) if ((e = f_e(f, i)) < zone.nb_elem() || ch.icl(f, 0) < 6)
        for (n = 0; n < N; n++) stencil.append_line(N * e + n, N * f_e(f, !i) + n);

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * zone.mdv_ch_p0.valeur().nb_items_seq_tot()) << finl;
  Matrix_tools::allocate_morse_matrix(N * zone.nb_ch_p0_tot(), N * zone.nb_ch_p0_tot(), stencil, mat);
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
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& vfd = zone.volumes_entrelaces_dir();
  int i, j, k, e, eb, f, fb, n, N = inco.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  //prerequis : interpolations aux faces
  update_nu();

  /* 1. valeurs aux faces, par interpolation ou directement (faces de bord) */
  DoubleTrav val_f(nf_tot, N);
  for (f = 0; f < nf_tot; f++) for (n = 0; n < N; n++) for (i = 0; i <= dimension; i++) if (tfi(f, n, i) >= 0)
          val_f(f, n) += tfc(f, n, i) * inco.addr()[N * tfi(f, n, i) + n];

  /* 2. contributions des elements */
  DoubleTrav nu_ef(e_f.dimension(1), N), phi(N);
  for (e = 0; e < ne_tot; e++) for (remplir_nu_ef(e, nu_ef), i = 0, j = zone.w2d(e); j < zone.w2d(e + 1); i++, j++) if ((f = e_f(e, i)) < zone.nb_faces())
        {
          for (k = zone.w2i(j), phi = 0; k < zone.w2i(j + 1); k++) for (fb = e_f(e, zone.w2j(k)), n = 0; n < N; n++)
              phi(n) += fs(fb) * zone.w2c(k) * nu_ef(zone.w2j(k), n) * (inco.addr()[N * e + n] - val_f(fb, n));
          phi *= fs(f) / ve(e) * vfd(f, e != f_e(f, 0)) / vf(f) * (e == f_e(f, 0) ? 1 : -1);
          for (k = 0; k < 1 + (ch.icl(f, 0) < 6); k++) for (eb = f_e(f, k), n = 0; n < N; n++)
              resu.addr()[N * eb + n] += (k ? 1 : -1) * phi(n);
          if (f < zone.premiere_face_int()) for (n = 0; n < N; n++) flux_bords_(f, n) += phi(n);
        }

  return resu;
}

//Description:
//on assemble la matrice.
void Op_Diff_CoviMAC_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& vfd = zone.volumes_entrelaces_dir();
  int i, j, k, l, m, e, eb, f, fb, n, N = inco.line_size(), ne_tot = zone.nb_elem_tot();


  //prerequis : interpolations aux faces
  update_nu();

  DoubleTrav nu_ef(e_f.dimension(1), N);
  for (e = 0; e < ne_tot; e++) for (remplir_nu_ef(e, nu_ef), i = 0, j = zone.w2d(e); j < zone.w2d(e + 1); i++, j++) if ((f = e_f(e, i)) < zone.nb_faces())
        {
          double fac_f = vfd(f, e != f_e(f, 0)) / vf(f) * fs(f) / ve(e) * (e == f_e(f, 0) ? 1 : -1), fac_fb;
          for (k = zone.w2i(j); k < zone.w2i(j + 1); k++)
            {
              fb = e_f(e, zone.w2j(k)), fac_fb = fac_f * fs(fb) * zone.w2c(k);
              for (n = 0; n < N; n++) for (l = 0; l <= dimension; l++) if (tfi(fb, n, l) >= 0) for (m = 0; m < 1 + (ch.icl(f, 0) < 6); m++)
                      if ((eb = f_e(f, m)) < zone.nb_elem() || ch.icl(f, 0))
                        {
                          double coeff = (m ? -1 : 1) * fac_fb * nu_ef(zone.w2j(k), n) * tfc(fb, n, l);
                          if (ch.icl(fb, 0) < 6) matrice(N * eb + n, N * tfi(fb, n, l) + n) -= coeff;//si CL de Dirichlet, pas de derivee
                          matrice(N * eb + n, N * e + n) += coeff;
                        }
            }
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
