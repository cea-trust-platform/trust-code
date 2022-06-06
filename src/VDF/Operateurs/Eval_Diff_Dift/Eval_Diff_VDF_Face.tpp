/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Eval_Diff_VDF_Face_TPP_included
#define Eval_Diff_VDF_Face_TPP_included

#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_Cl_VDF.h>

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int elem, int fac1, int fac2, Type_Double& flux) const
{
  const int ori = orientation(fac1), ncomp = flux.size_array();
  const double dist = dist_face(fac1,fac2,ori), surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)),
               visc_lam = nu_lam(elem), visc_turb = DERIVED_T::IS_TURB ? nu_turb(elem) : 0.;
  for (int k = 0; k < ncomp; k++)
    {
      const double tau = (inco(fac2,k)-inco(fac1,k))/dist, tau_tr = ACTIVATE_TAU_TR ? tau : 0.0;
      flux[k] = ((tau + tau_tr) * (visc_lam + visc_turb)) * surf;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, Type_Double& flux) const
{
  const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1), ncomp = flux.size_array();
  const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
               visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = DERIVED_T::IS_TURB ? nu_mean_4pts(elem1,elem2,elem3,elem4,ori3) : 0.0;

  for (int k = 0; k < ncomp; k++)
    {
      const double tau = (inco(fac4,k)-inco(fac3,k))/dist_face(fac3,fac4,ori1), tau_tr = ACTIVATE_TAU_TR ? (inco(fac2,k)-inco(fac1,k))/dist_face(fac1,fac2,ori3) : 0.0;
      flux[k] = ((tau + tau_tr) * (visc_lam + visc_turb)) * surf * poros;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, Type_Double& flux) const
{
  int elem[4], ori1 = orientation(fac1), ori3 = orientation(fac3);
  elem[0] = elem_(fac3,0), elem[1] = elem_(fac3,1), elem[2] = elem_(fac4,0), elem[3] = elem_(fac4,1);
  double visc_lam_temp = 0, visc_turb_temp = 0;
  for (int i = 0; i < 4; i++)
    if (elem[i] != -1)
      {
        visc_lam_temp += nu_lam(elem[i]);
        visc_turb_temp += nu_turb(elem[i]);
      }
  visc_lam_temp /= 3.0;
  visc_turb_temp /= 3.0;
  const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
               visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0), visc_turb = DERIVED_T::IS_TURB ? visc_turb_temp : 0.0;

  for (int k = 0; k < flux.size_array(); k++)
    if (inco(fac4,k)*inco(fac3,k) != 0)
      {
        const double tau = (inco(fac4,k)-inco(fac3,k))/dist_face(fac3,fac4,ori1), tau_tr = ACTIVATE_TAU_TR ? (inco(fac2,k)-inco(fac1,k))/dist_face(fac1,fac2,ori3) : 0.0;
        flux[k] = ((tau + tau_tr) * (visc_lam + visc_turb)) * surf * poros;
      }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, Type_Double& flux) const
{
  constexpr bool is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), ori = orientation(fac3), ncomp = flux.size_array();
  if ( !uses_wall_law() )
    {
      int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
      if (is_PAROI)
        {
          if (elem1 == -1) elem1 = elem2;
          else if (elem2 == -1) elem2 = elem1;
        }

      const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), dist = dist_norm_bord(fac1), tps = inconnue->temps(),
                   visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = DERIVED_T::IS_TURB ? nu_mean_2pts(elem1,elem2,ori) : 0.0;

      const double vit_imp = is_PAROI ? 0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl)) :
                             0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang2,ori,la_zcl));

      for (int k = 0; k < ncomp; k++)
        {
          const double tau  = signe*(vit_imp-inco(fac3,k))/dist, tau_tr = 0.; // XXX : Yannick, pas de tau_tr ?? TODO : FIXME
          flux[k] = ((tau + tau_tr) * (visc_lam + visc_turb)) * surf * poros;
        }
    }
  else
    {
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      for (int k = 0; k < ncomp; k++) flux[k] = tau1 + tau2;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, Type_Double& flux3, Type_Double& flux1_2) const
{
  assert (flux3.size_array() == flux1_2.size_array());
  constexpr bool is_SYM_FL = (Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE), is_PAR_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3), ncomp = flux3.size_array();
  const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3), tps = inconnue->temps(),
               dist1 = dist_norm_bord(fac1), dist2 = dist_face(fac1,fac2,ori),
               visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = DERIVED_T::IS_TURB ? nu_mean_2pts(elem1,elem2,ori) : 0.0;

  double vit_imp;
  for (int k = 0; k < ncomp; k++)
    {
      if (is_SYM_FL) vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang1,ori,la_zcl)+ Champ_Face_get_val_imp_face_bord_sym(inco,tps,rang2,ori,la_zcl));
      else if (is_PAR_FL) // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
        {
          if (est_egal(inco(fac1,k),0)) vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl); // fac1 est la face de paroi
          else vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl); // fac2 est la face de paroi
        }
      else vit_imp =  0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl));

      //         |
      // fac 3   | fac 2
      //  --------
      //         | fac 1
      //         |

      // fac3 est la face interne et fac1 et fac2 sont au bord
      const double tau_3 = signe*(vit_imp-inco(fac3,k))/dist1, tau_12 = (inco(fac2,k)-inco(fac1,k))/dist2, tau_tr_3 = ACTIVATE_TAU_TR ? tau_12 : 0.0, tau_tr_12 = ACTIVATE_TAU_TR ? tau_3 : 0.0;
      flux3[k] = ((tau_3 + tau_tr_3) * (visc_lam + visc_turb)) * surf * poros;
      flux1_2[k] = ((tau_12 + tau_tr_12) * (visc_lam + visc_turb)) * surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, Type_Double& flux3_4, Type_Double& flux1_2) const
{
  assert (flux3_4.size_array() == flux1_2.size_array());
  const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1), ncomp = flux3_4.size_array();
  const double dist3_4 = dist_face_period(fac3,fac4,ori1), dist1_2 = dist_face_period(fac1,fac2,ori3),
               surf1_2 = surface_(fac1,fac2), poros1_2 = porosity_(fac1, fac2), surf3_4 = surface_(fac3,fac4), poros3_4 = porosity_(fac3, fac4),
               visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = DERIVED_T::IS_TURB ? nu_mean_4pts(elem1,elem2,elem3,elem4,ori3) : 0.0;

  for (int k = 0; k < ncomp; k++)
    {
      const double tau_34 = (inco(fac4,k)-inco(fac3,k))/dist3_4, tau_12 = (inco(fac2,k)-inco(fac1,k))/dist1_2, tau_tr_34 = ACTIVATE_TAU_TR ? tau_12 : 0.0, tau_tr_12 = ACTIVATE_TAU_TR ? tau_34 : 0.0;
      flux3_4[k] = ((tau_34 + tau_tr_34) * (visc_lam + visc_turb)) * surf1_2 * poros1_2;
      flux1_2[k] = ((tau_12 + tau_tr_12) * (visc_lam + visc_turb)) * surf3_4 * poros3_4;
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7(int elem,int fac1, int fac2, Type_Double& f1, Type_Double& f2) const
{
  assert(f1.size_array() == f2.size_array());
  const int ori = orientation(fac1), ncomp = f1.size_array();
  const double dist = dist_face(fac1,fac2,ori), surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)),
               visc_lam = nu_lam(elem), visc_turb = DERIVED_T::IS_TURB ? nu_turb(elem) : 0.,
               d_tau = 1. / dist, d_tau_tr = ACTIVATE_TAU_TR ? d_tau : 0.0; // On derive par rapport a fac1 et fac2

  for (int k = 0; k < ncomp; k++) f1[k] = f2[k] = ((d_tau + d_tau_tr) * (visc_lam + visc_turb)) * surf;

  if (TEST_COEFFS) test_coeffs_fa7<Fa7_Type,Type_Double>(elem,fac1,fac2,f1);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, Type_Double& aii, Type_Double& ajj) const
{
  assert(aii.size_array() == ajj.size_array());
  const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1), ncomp = aii.size_array();
  const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
               visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = DERIVED_T::IS_TURB ? nu_mean_4pts(elem1,elem2,elem3,elem4,ori3) : 0.0,
               d_tau = 1.0 / dist_face(fac3,fac4,ori1), d_tau_tr = 0.; // On derive par rapport a fac3 et fac4

  for (int k = 0; k < ncomp; k++) aii[k] = ajj[k] = ((d_tau + d_tau_tr) * (visc_lam + visc_turb)) * surf * poros;

  if (TEST_COEFFS) test_coeffs_arete<Arete_Type,Type_Double>(fac1,fac2,fac3,fac4,aii);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, Type_Double& aii, Type_Double& ajj) const
{
  assert(aii.size_array() == ajj.size_array());
  const int ori1 = orientation(fac1), ncomp = aii.size_array();
  int elem[4];
  elem[0] = elem_(fac3,0), elem[1] = elem_(fac3,1), elem[2] = elem_(fac4,0), elem[3] = elem_(fac4,1);

  double visc_lam_temp = 0, visc_turb_temp = 0;
  for (int i=0; i<4; i++)
    if (elem[i] != -1)
      {
        visc_lam_temp += nu_lam(elem[i]);
        visc_turb_temp += nu_turb(elem[i]);
      }
  visc_lam_temp /= 3.0;
  visc_turb_temp /= 3.0;

  const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2),
               visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0), visc_turb =  DERIVED_T::IS_TURB ? visc_turb_temp : 0.,
               d_tau = 1. / dist_face(fac3,fac4,ori1), d_tau_tr = 0.; // On derive par rapport a fac3 et fac4

  const DoubleTab& inco = inconnue->valeurs();
  for (int k = 0; k < ncomp; k++)
    if (inco(fac4,k) * inco(fac3,k) != 0) aii[k] = ajj[k] = ((d_tau + d_tau_tr) * (visc_lam + visc_turb)) * surf * poros;

  if (TEST_COEFFS) test_coeffs_arete<Arete_Type,Type_Double>(fac1,fac2,fac3,fac4,aii);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, Type_Double& aii1_2, Type_Double& aii3_4, Type_Double& ajj1_2) const
{
  assert(aii1_2.size_array() == aii3_4.size_array() && aii1_2.size_array() == ajj1_2.size_array());
  constexpr bool is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
  const int ncomp = aii1_2.size_array();
  if ( !uses_wall_law())
    {
      int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori = orientation(fac3);
      if(is_PAROI)
        {
          if (elem1 == -1) elem1 = elem2;
          else if (elem2 == -1) elem2 = elem1;
        }

      const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), dist = dist_norm_bord(fac1),
                   visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = DERIVED_T::IS_TURB ? nu_mean_2pts(elem1,elem2,ori) : 0.,
                   d_tau = signe / dist, d_tau_tr = 0.; // On a pas derive ... deja nul dans le flux !

      for (int k = 0; k < ncomp; k++)
        {
          aii1_2[k] = ajj1_2[k] = 0.;
          aii3_4[k] = ((d_tau + d_tau_tr) * (visc_lam + visc_turb)) * surf * poros;
        }
    }
  else for (int k = 0; k < ncomp; k++) aii3_4[k] = aii1_2[k] = ajj1_2[k] = 0.;

  if (TEST_COEFFS) test_coeffs_arete<Arete_Type,Type_Double>(fac1,fac2,fac3,signe,aii3_4);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, Type_Double& aii1_2, Type_Double& aii3, Type_Double& ajj1_2) const
{
  assert(aii1_2.size_array() == aii3.size_array() && aii1_2.size_array() == ajj1_2.size_array());
  const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3), ncomp = aii1_2.size_array();
  const double surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3),
               dist1 = dist_norm_bord(fac1), dist2 = dist_face(fac1,fac2,ori),
               visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = DERIVED_T::IS_TURB ? nu_mean_2pts(elem1,elem2,ori) : 0.,
               d_tau_3 = signe / dist1, d_tau_tr_3 = 0., // On derive par rapport a fac3
               d_tau_12 = 1. / dist2, d_tau_tr_12 = 0.; // On derive par rapport a fac1 et fac2

  for (int k = 0; k < ncomp; k++)
    {
      aii3[k] = ((d_tau_3 + d_tau_tr_3) * (visc_lam + visc_turb)) * surf * poros;
      aii1_2[k] = ajj1_2[k] = ((d_tau_12 + d_tau_tr_12) * (visc_lam + visc_turb)) * surfporos;
    }

  if (TEST_COEFFS) test_coeffs_arete<Arete_Type,Type_Double>(fac1,fac2,fac3,signe,aii1_2,aii3);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, Type_Double& aii, Type_Double& ajj) const
{
  assert(aii.size_array() == ajj.size_array());
  const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1), ncomp = aii.size_array();
  const double dist3_4 = dist_face_period(fac3,fac4,ori1), surf = surface_(fac1,fac2), poros = porosity_(fac1, fac2),
               visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = DERIVED_T::IS_TURB ? nu_mean_4pts(elem1,elem2,elem3,elem4,ori3) : 0.,
               d_tau = 1. / dist3_4, d_tau_tr = 0.; // On derive par rapport a fac3 et fac4

  for (int k = 0; k < ncomp; k++) aii[k] = ajj[k] = ((d_tau + d_tau_tr) * (visc_lam + visc_turb)) * surf * poros;

  if (TEST_COEFFS) test_coeffs_arete<Arete_Type,Type_Double>(fac1,fac2,fac3,fac4,aii);
}

/* ************************************** *
 * *********   For checking   *********** *
 * ************************************** */

template <typename DERIVED_T> template<typename Type_Double>
void Eval_Diff_VDF_Face<DERIVED_T>::check_error(const char * nom_funct, const int Type_Flux, const int ncomp, const Type_Double& f1, const Type_Double& flux_p, const Type_Double& flux_m) const
{
  for (int k = 0; k < ncomp; k++)
    {
      const double err = std::fabs(((flux_p[0] - flux_m[0]) / (2.0 * EPS)) - f1[0]);
      if (err > EPS)
        {
          std::cerr << "Error in function : " << nom_funct << "_Type_Flux_" << Type_Flux << " : " << f1[0] << " " << (flux_p[0] - flux_m[0]) / (2.0 * EPS) << std::endl;
          Process::exit();
        }
    }

  if (la_zcl->equation().probleme().schema_temps().nb_pas_dt() > 10)
    {
      Cerr << "All OK ! Coeffs and fluxes implementations are coherent !" << finl;
      Process::exit();
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_common(const int fac1, const int fac2, const int fac3, const int fac4, Type_Double& flux_p, Type_Double& flux_m) const
{
  DoubleTab inco_pert = inconnue->valeurs();

  inco_pert(fac4) += EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,fac4,flux_p);

  inco_pert(fac4) -= 2.0 * EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,fac4,flux_m);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_common(const int fac1, const int fac2, const int fac3, const int signe, Type_Double& flux_p, Type_Double& flux_m) const
{
  DoubleTab inco_pert = inconnue->valeurs();

  inco_pert(fac3) -= EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,signe,flux_p);

  inco_pert(fac3) += 2.0 * EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,signe,flux_m);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_common(const int fac1, const int fac2, const int fac3, const int signe, Type_Double& flux_p3, Type_Double& flux_m3, Type_Double& flux_p12, Type_Double& flux_m12) const
{
  DoubleTab inco_pert = inconnue->valeurs();
  Type_Double poubelle(flux_p3.size_array());

  // pour flux3
  inco_pert(fac3) -= EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,signe,flux_p3,poubelle);

  inco_pert(fac3) += 2.0 * EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,signe,flux_m3,poubelle);

  // pour flux1_2
  inco_pert = inconnue->valeurs(); // back to real values
  inco_pert(fac2) += EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,signe,poubelle,flux_p12);

  inco_pert(fac2) -= 2.0 * EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,signe,poubelle,flux_m12);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_common(const int fac1, const int fac2, const int fac3, const int fac4, Type_Double& flux_p, Type_Double& flux_m) const
{
  DoubleTab inco_pert = inconnue->valeurs();
  Type_Double poubelle(flux_p.size_array());

  inco_pert(fac4) += EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,fac4,flux_p,poubelle);

  inco_pert(fac4) -= 2.0 * EPS; // XXX : ATTENTION SIGNE
  flux_arete<Arete_Type,Type_Double>(inco_pert,fac1,fac2,fac3,fac4,flux_m,poubelle);
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_fa7(const int elem, const int fac1, const int fac2, const Type_Double& f1) const
{
  const int ncomp = f1.size_array();
  Type_Double flux_p(ncomp), flux_m(ncomp);
  DoubleTab inco_pert = inconnue->valeurs();

  inco_pert(fac2) += EPS; // XXX : ATTENTION SIGNE
  flux_fa7<Fa7_Type,Type_Double>(inco_pert,elem,fac1,fac2,flux_p);

  inco_pert(fac2) -= 2.0 * EPS; // XXX : ATTENTION SIGNE
  flux_fa7<Fa7_Type,Type_Double>(inco_pert,elem,fac1,fac2,flux_m);

  check_error<Type_Double>(__func__,(int)Fa7_Type,ncomp,f1,flux_p,flux_m);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_arete(const int fac1, const int fac2, const int fac3, const int fac4, const Type_Double& aii) const
{
  const int ncomp = aii.size_array();
  Type_Double flux_p(ncomp), flux_m(ncomp);
  test_coeffs_common<Arete_Type,Type_Double>(fac1,fac2,fac3,fac4,flux_p,flux_m);
  check_error<Type_Double>(__func__,(int)Arete_Type,ncomp,aii,flux_p,flux_m);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_arete(const int fac1, const int fac2, const int fac3, const int fac4, const Type_Double& aii) const
{
  const int ncomp = aii.size_array();
  Type_Double flux_p(ncomp), flux_m(ncomp);
  test_coeffs_common<Arete_Type,Type_Double>(fac1,fac2,fac3,fac4,flux_p,flux_m);
  if (inconnue->valeurs()(fac4,0) * inconnue->valeurs()(fac3,0) != 0) check_error<Type_Double>(__func__,(int)Arete_Type,ncomp,aii,flux_p,flux_m);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_arete(const int fac1, const int fac2, const int fac3, const int signe, const Type_Double& aii3_4) const
{
  const int ncomp = aii3_4.size_array();
  Type_Double flux_p(ncomp), flux_m(ncomp);
  test_coeffs_common<Arete_Type,Type_Double>(fac1,fac2,fac3,signe,flux_p,flux_m);
  if ( !uses_wall_law() ) check_error<Type_Double>(__func__,(int)Arete_Type,ncomp,aii3_4,flux_p,flux_m);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_arete(const int fac1, const int fac2, const int fac3, const int signe, const Type_Double& aii1_2, const Type_Double& aii3) const
{
  const int ncomp = aii1_2.size_array();
  Type_Double flux_p3(ncomp), flux_m3(ncomp), flux_p12(ncomp), flux_m12(ncomp);
  test_coeffs_common<Arete_Type,Type_Double>(fac1,fac2,fac3,signe,flux_p3,flux_m3,flux_p12,flux_m12);
  check_error<Type_Double>(__func__,(int)Arete_Type,ncomp,aii3,flux_p3,flux_m3);
  check_error<Type_Double>(__func__,(int)Arete_Type,ncomp,aii1_2,flux_p12,flux_m12);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::test_coeffs_arete(const int fac1, const int fac2, const int fac3, const int fac4, const Type_Double& aii) const
{
  const int ncomp = aii.size_array();
  Type_Double flux_p(ncomp), flux_m(ncomp);
  test_coeffs_common<Arete_Type,Type_Double>(fac1,fac2,fac3,fac4,flux_p,flux_m);
  check_error<Type_Double>(__func__,(int)Arete_Type,ncomp,aii,flux_p,flux_m);
}

#endif /* Eval_Diff_VDF_Face_TPP_included */
