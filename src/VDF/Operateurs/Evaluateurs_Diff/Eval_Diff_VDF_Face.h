/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_Diff_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Face_included
#define Eval_Diff_VDF_Face_included

#include <Neumann_sortie_libre.h>
#include <Evaluateur_VDF.h>
#include <Eval_VDF_Face.h>

// .DESCRIPTION class Eval_Diff_VDF_Face
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_Face
// .SECTION voir aussi Eval_VDF_Face, Evaluateur_VDF
template <typename DERIVED_T>
class Eval_Diff_VDF_Face : public Eval_VDF_Face, public Evaluateur_VDF
{

public:
  static constexpr bool IS_VAR = false, IS_TURB = false;
  static constexpr bool CALC_FA7_SORTIE_LIB = DERIVED_T::IS_TURB ? true : false, CALC_ARR_PAR_FL = DERIVED_T::IS_TURB ? false : true;

  inline double surface_(int i,int j) const;
  inline double porosity_(int i,int j) const;

  // CRTP pattern to static_cast the appropriate class and get the implementation
  // This is magic !
  inline double nu_mean_2pts(int i=0, int j=0, int compo=0) const;
  inline double nu_mean_4pts(int i, int j, int k, int l, int compo=0) const;
  inline double nu_lam(int i, int j=0) const;
  inline double nu_lam_mean_4pts(int i, int j, int k, int l, int compo=0) const;
  inline double nu_lam_mean_2pts(int i, int j, int compo=0) const;
  inline double nu_turb(int i, int compo=0) const;
  inline double tau_tan(int,int) const;
  inline bool uses_wall_law() const;
  inline bool uses_mod_turb() const;
  inline DoubleTab k_elem() const;

  /* ****************************************************************************** *
   * YES, we do magic ! Its all about : Substition Failure Is Not An Error (SFINAE) *
   * ****************************************************************************** */

  /* ******************* *
   * CAS SCALAIRE : FLUX *
   * ******************* */

  // TODO : FIXME : a faire void un jour !!!!!!
  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, double>
  flux_fa7(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const { return 0; }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, double>
  flux_fa7(const DoubleTab&, int, int, int) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, double>
  flux_arete(const DoubleTab&, int, int, int, int) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, double>
  flux_arete(const DoubleTab&, int, int, int, int) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, double>
  flux_arete(const DoubleTab&, int, int, int, int) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, double>
  flux_arete(const DoubleTab&, int, int, int, int) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE, double>
  flux_arete(const DoubleTab&, int, int, int, int) const { return 0; }

  // void
  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
  }

  /* ********************* *
   * CAS SCALAIRE : COEFFS *
   * ********************* */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  coeffs_fa7(int, const Neumann_sortie_libre&, double& aii, double& ajj ) const { aii = ajj = 0; }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  coeffs_fa7(int, int, int, double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
  coeffs_arete(int, int, int, int, double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
  coeffs_arete(int, int, int, int, double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  coeffs_arete(int, int, int, int, double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  coeffs_arete(int, int, int, int, double& , double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  coeffs_arete(int, int, int, int, double& , double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
  coeffs_arete(int, int, int, int, double& , double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  coeffs_arete(int, int, int, int, double& , double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  coeffs_arete(int, int, int, int, double& , double& , double& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< (Arete_Type == SYMETRIE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE), void>
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* Do nothing */ }

  /* ********************* *
   * CAS SCALAIRE : SECMEM *
   * ********************* */

  // TODO : FIXME : a faire void un jour !!!!!!
  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, double>
  secmem_fa7(int, const Neumann_sortie_libre&, int ) const { return 0; }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, double>
  secmem_fa7( int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type> inline enable_if_t<(Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == MIXTE || Arete_Type == SYMETRIE), double>
  secmem_arete(int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, double>
  secmem_arete(int, int, int, int ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, double>
  secmem_arete(int, int, int, int ) const;

  // void
  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
  secmem_arete(int, int, int, int, double&, double&) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  secmem_arete(int, int, int, int, double&, double&) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  secmem_arete(int, int, int, int, double&, double&) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t<(Arete_Type == PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE), void>
  secmem_arete(int, int, int, int, double&, double&) const { /* Do nothing */ }

  /* ******************** *
   * CAS VECTORIEL : FLUX *
   * ******************** */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  flux_fa7(const DoubleTab&, int , const Neumann_sortie_libre&, int, DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  flux_fa7(const DoubleTab&, int, int, int, DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PAROI ||
                      Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI), void>
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE), void>
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
  }

  /* ********************** *
   * CAS VECTORIEL : COEFFS *
   * ********************** */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  coeffs_fa7(int , const Neumann_sortie_libre&, DoubleVect& , DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  coeffs_fa7(int, int, int, DoubleVect& , DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type> inline enable_if_t<(Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PERIODICITE), void>
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI || Arete_Type == Type_Flux_Arete::FLUIDE ||
                      Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE), void>
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* Do nothing */ }

  /* ********************** *
   * CAS VECTORIEL : SECMEM *
   * ********************** */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  secmem_fa7(int , const Neumann_sortie_libre&, int, DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  secmem_fa7(int, int, int, DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PAROI ||
                      Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI), void>
  secmem_arete(int, int, int, int, DoubleVect& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PERIODICITE ||
                      Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE), void>
  secmem_arete(int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
};

//************************
// CRTP pattern
//************************

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_mean_2pts(int i, int j,int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_1_impl_face(i, j, compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_mean_4pts(int i, int j, int k, int l,int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_2_impl_face(i, j, k, l,compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_lam(int i, int j) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_2_impl(i,j); // Attention nu_2_impl and not nu_1_impl for Dift ...
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_lam_mean_4pts(int i, int j, int k, int l,int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_lam_impl_face(i,j,k,l,compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_lam_mean_2pts(int i, int j,int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_lam_impl_face2(i,j,compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_turb(int i,int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_t_impl(i,compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::tau_tan(int i, int j) const
{
  double tt = static_cast<const DERIVED_T *>(this)->tau_tan_impl(i,j);
  return tt;
}

template <typename DERIVED_T>
inline bool Eval_Diff_VDF_Face<DERIVED_T>::uses_wall_law() const
{
  bool wl = static_cast<const DERIVED_T *>(this)->uses_wall();
  return wl;
}

template <typename DERIVED_T>
inline bool Eval_Diff_VDF_Face<DERIVED_T>::uses_mod_turb() const
{
  bool nm = static_cast<const DERIVED_T *>(this)->uses_mod();
  return nm;
}

template <typename DERIVED_T>
inline DoubleTab Eval_Diff_VDF_Face<DERIVED_T>::k_elem() const
{
  DoubleTab k = static_cast<const DERIVED_T *>(this)->get_k_elem();
  return k;
}

//************************
// Internal methods
//************************

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::surface_(int i,int j) const
{
  const double surf = (DERIVED_T::IS_VAR || DERIVED_T::IS_TURB) ? 0.5*(surface(i)+surface(j)) :
                      0.5*(surface(i)*porosite(i)+surface(j)*porosite(j));
  return surf;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::porosity_(int i,int j) const
{
  const double poros = (DERIVED_T::IS_VAR || DERIVED_T::IS_TURB) ? 0.5*(porosite(i)+porosite(j)) : 1.0;
  return poros;
}

//********************************
// Class templates specializations
//********************************

/* Function templates specialization for flux_arete_fluide */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, double& flux3, double& flux1_2) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord),
            elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori),
         surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));
  double dist1 = dist_norm_bord(fac1), dist2 = dist_face(fac1,fac2,ori), tau = signe * (vit_imp - inco(fac3))/dist1,
         tau_tr = (inco(fac2) - inco(fac1))/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      flux3 = ((tau+tau_tr)*visc_lam + reyn)*surf*poros;
      flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surfporos;
    }
  else
    {
      flux3 = (tau*visc_lam + reyn)*surf*poros;
      flux1_2 = (tau_tr*visc_lam + reyn)*surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2),
         poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3), dist1 = dist_norm_bord(fac1), dist2 = dist_face(fac1,fac2,ori),
         tau = signe/dist1, tau_tr = 1/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      aii3_4 = ((tau+tau_tr)*visc_lam + reyn)*surf*poros;
      aii1_2 = ajj1_2  = ((tau+tau_tr)*visc_lam + reyn)*surfporos;
    }
  else
    {
      aii3_4 = (tau*visc_lam + reyn)*surf*poros;
      aii1_2 = ajj1_2  = (tau_tr*visc_lam + reyn)*surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe, double& flux3, double& flux1_2) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2),
         poros = porosity_(fac1,fac2), dist = dist_norm_bord(fac1);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double tau = signe*vit_imp/dist, reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;

  // XXX : if we regroup => SOME TEST CASES BROKE UP !
  flux3 = DERIVED_T::IS_TURB ? (tau*visc_lam + reyn)*surf*poros : tau*surf*visc_lam*poros;

  flux1_2 = 0;
}

/* Function templates specialization for flux_arete_interne */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, double>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4) const
{
  const int ori1=orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);
  double flux, visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3),
               surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), tau = (inco(fac4)-inco(fac3))/dist_face(fac3,fac4,ori1),
               tau_tr = (inco(fac2)-inco(fac1))/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  flux = (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) ? (reyn + visc_lam*(tau+tau_tr))* surf*poros : (reyn + visc_lam*tau)*surf*poros;

  return flux;
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, double& aii, double& ajj) const
{
  const int ori1=orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);
  double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3),
         surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), tau = 1/dist_face(fac3,fac4,ori1),
         tau_tr = 1/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) aii = ajj = (reyn + visc_lam*(tau+tau_tr))*surf*poros;
  else aii = ajj = (reyn + visc_lam*tau)*surf*poros;
}

/* Function templates specialization for flux_arete_mixte */
// DIFT : Sur les aretes mixtes les termes croises du tenseur de Reynolds sont nuls: il ne reste donc que la diffusion laminaire
// E Saikali ???? Are you sure ?
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, double>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4) const
{
  double flux=0;
  if (inco(fac4)*inco(fac3) != 0)
    {
      const int ori1 = orientation(fac1), ori3 = orientation(fac3);
      int elem[4];
      elem[0] = elem_(fac3,0);
      elem[1] = elem_(fac3,1);
      elem[2] = elem_(fac4,0);
      elem[3] = elem_(fac4,1);

      double visc_lam_temp = 0, visc_turb_temp = 0;
      for (int i=0; i<4; i++)
        if (elem[i] != -1)
          {
            visc_lam_temp += nu_lam(elem[i]);
            visc_turb_temp += nu_turb(elem[i]);
          }
      visc_lam_temp/=3.0;
      visc_turb_temp/=3.0;

      double visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0), visc_turb = visc_turb_temp, surf = surface_(fac1,fac2),
             poros = porosity_(fac1,fac2), tau = (inco(fac4)-inco(fac3))/dist_face(fac3,fac4,ori1),
             tau_tr = (inco(fac2)-inco(fac1))/dist_face(fac1,fac2,ori3), reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      flux = (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) ? (reyn + visc_lam*(tau+tau_tr)) * surf * poros : tau * surf * visc_lam * poros;
    }
  return flux;
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, double& aii, double& ajj) const
{
  if (inconnue->valeurs()(fac4) * inconnue->valeurs()(fac3) != 0)
    {
      const int ori1 = orientation(fac1), ori3 = orientation(fac3);
      int elem[4];
      elem[0] = elem_(fac3,0);
      elem[1] = elem_(fac3,1);
      elem[2] = elem_(fac4,0);
      elem[3] = elem_(fac4,1);

      double visc_lam_temp = 0, visc_turb_temp = 0;
      for (int i=0; i<4; i++)
        if (elem[i] != -1)
          {
            visc_lam_temp += nu_lam(elem[i]);
            visc_turb_temp += nu_turb(elem[i]);
          }
      visc_lam_temp/=3.0;
      visc_turb_temp/=3.0;

      double visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0), visc_turb = visc_turb_temp, surf = surface_(fac1,fac2),
             poros = porosity_(fac1,fac2), tau = 1/dist_face(fac3,fac4,ori1), tau_tr = 1/dist_face(fac1,fac2,ori3),
             reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      // XXX : if we regroup => SOME TEST CASES BROKE UP !
      if (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR)
        aii = ajj = (reyn + visc_lam*(tau+tau_tr))* surf * poros;
      else
        aii = ajj= surf * visc_lam * poros * tau;
    }
  else aii=ajj=0;
}

/* Function templates specialization for flux_arete_paroi */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, double>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), ori = orientation(fac3);
  double flux, vit = inco(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  if ( !uses_wall_law() )
    {
      int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);

      if (elem1==-1) elem1 = elem2;
      else if (elem2==-1) elem2 = elem1;

      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), dist = dist_norm_bord(fac1),
             tau = signe*(vit_imp - inco(fac3))/dist, surf = surface_(fac1,fac2);

      flux = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau*surf*(visc_lam+visc_turb) : tau*surf*visc_lam;
    }
  else
    {
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      int signe_terme = ( vit < vit_imp ) ? -1 : 1;

      //30/09/2003  YB : influence de signe terme eliminee, signe pris en compte dans la loi de paroi
      signe_terme = 1;
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2), coef = tau1 + tau2;
      flux = signe_terme * coef;
    }
  return flux;
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  if ( !uses_wall_law())
    {
      int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori = orientation(fac3);

      if (elem1==-1) elem1 = elem2;
      else if (elem2==-1) elem2 = elem1;

      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori),
             dist = dist_norm_bord(fac1), surf = surface_(fac1,fac2);

      aii1_2 = ajj1_2 = 0;

      aii3_4 = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? signe*surf*(visc_lam+visc_turb)/dist :
               signe*surf*visc_lam/dist;
    }
  else aii3_4 = aii1_2 = ajj1_2 = 0;
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, double>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe) const
{
  const int ori = orientation(fac3), rang1 = (fac1-premiere_face_bord),
            rang2 = (fac2-premiere_face_bord), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);

  const DoubleTab& inco = inconnue->valeurs();
  double flux, vit = inco(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori);

  if ( !uses_wall_law() )
    {
      double dist = dist_norm_bord(fac1), surf = surface_(fac1,fac2),
             tau  = DERIVED_T::IS_TURB ? signe*(vit_imp - inco(fac3))/dist : signe * vit_imp/dist;

      flux = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau*surf*(visc_lam+visc_turb) :
             tau * surf * visc_lam;
    }
  else
    {
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      int signe_terme = ( vit < vit_imp ) ? -1 : 1;

      //30/09/2003  YB : influence de signe terme eliminee, signe pris en compte dans la loi de paroi
      signe_terme = 1;
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2), coef = tau1 + tau2;
      flux = signe_terme * coef;
    }
  return flux;
}

/* Function templates specialization for flux_arete_paroi_fluide */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3,int signe, double& flux3, double& flux1_2) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord),
            ori = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
  double vit_imp, visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori);

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
  if (est_egal(inco(fac1),0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

  double dist1 = dist_norm_bord(fac1), dist2 =dist_face(fac1,fac2,ori), surf = surface_(fac1,fac2),
         poros = porosity_(fac1, fac2), surfporos = surface(fac3)*porosite(fac3), tau = signe * (vit_imp - inco(fac3))/dist1,
         tau_tr = (inco(fac2) - inco(fac1))/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      flux3 = ((tau+tau_tr)*visc_lam + reyn) * surf * poros;
      flux1_2 = ((tau+tau_tr)*visc_lam + reyn) * surfporos;
    }
  else
    {
      flux3 = (tau*visc_lam + reyn) * surf * poros;
      flux1_2 = (tau_tr*visc_lam + reyn) * surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2),
         poros = porosity_(fac1, fac2), surfporos = surface(fac3)*porosite(fac3), dist1 = dist_norm_bord(fac1),
         dist2 = dist_face(fac1,fac2,ori), tau = signe/dist1, tau_tr = 1/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      aii3_4 = ((tau+tau_tr)*visc_lam + reyn)*surf*poros;
      aii1_2 = ajj1_2 =((tau+tau_tr)*visc_lam + reyn)*surfporos;
    }
  else
    {
      aii3_4 = (tau*visc_lam + reyn)*surf*poros;
      aii1_2 = ajj1_2 =(tau_tr*visc_lam + reyn)*surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe, double& flux3, double& flux1_2) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord),
            ori= orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
  double vit_imp, visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori),
                  surf = surface_(fac1,fac2), poros = porosity_(fac1, fac2), dist = dist_norm_bord(fac1);

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
  if (est_egal(inconnue->valeurs()(fac1),0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

  double tau = signe*vit_imp/dist, reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;

  flux3 = DERIVED_T::IS_TURB ? (tau*visc_lam + reyn) * surf * poros : tau * surf * visc_lam * poros;
  flux1_2 = 0;
}

/* Function templates specialization for flux_arete_periodicite */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, double& flux3_4, double& flux1_2) const
{
  const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1),
            elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);

  double dist3_4 = dist_face_period(fac3,fac4,ori1), dist1_2 = dist_face_period(fac1,fac2,ori3),
         surf1_2 = surface_(fac1,fac2), poros1_2 = porosity_(fac1, fac2), surf3_4 = surface_(fac3,fac4),
         poros3_4 = porosity_(fac3, fac4), visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3),
         visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3), tau = (inco(fac4)-inco(fac3))/dist3_4,
         tau_tr = (inco(fac2)-inco(fac1))/dist1_2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      flux3_4 = (reyn + visc_lam*(tau+tau_tr)) * surf1_2 * poros1_2;
      flux1_2 = (reyn + visc_lam*(tau+tau_tr)) * surf3_4 * poros3_4;
    }
  else
    {
      flux3_4 = (reyn + visc_lam*tau) * surf1_2 * poros1_2;
      flux1_2 = (reyn + visc_lam*tau_tr) * surf3_4 * poros3_4;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int fac4, double& aii, double& ajj) const
{
  const int ori1 = orientation(fac1), ori3 = orientation(fac3), elem1 = elem_(fac3,0), elem2 = elem_(fac3,1),
            elem3 = elem_(fac4,0), elem4 = elem_(fac4,1);

  double dist3_4 = dist_face_period(fac3,fac4,ori1), dist1_2 = dist_face_period(fac1,fac2,ori3),
         visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3), visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3),
         surf = surface_(fac1,fac2), poros = porosity_(fac1, fac2), tau = 1/dist3_4, tau_tr = 1/dist1_2,
         reyn = DERIVED_T::IS_TURB ? (tau + tau_tr) * visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) aii = ajj = (reyn + visc_lam*(tau+tau_tr))* surf * poros;
  else aii = ajj = (reyn + visc_lam*tau) * surf * poros;
}

/* Function templates specialization for flux_fa7_elem */
template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, double>
Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int elem, int fac1, int fac2) const
{
  const int ori=orientation(fac1);
  double flux, dist = dist_face(fac1,fac2,ori), tau = (inco(fac2)-inco(fac1))/dist,
               surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)), visc_lam = nu_lam(elem);

  // On considere que l energie est dans la pression
  double reyn = DERIVED_T::IS_TURB ? - 2.*nu_turb(elem)*tau : 0.0;

  flux = (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) ? (-reyn + 2.*visc_lam*tau) * surf : (-reyn + visc_lam*tau) * surf ;

  return flux;
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7(int elem,int fac1, int fac2, double& aii, double& ajj) const
{
  const int ori = orientation(fac1);
  double tau = 1/dist_face(fac1,fac2,ori), visc_lam = nu_lam(elem),
         surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));

  // On considere que l energie est dans la pression
  //  double reyn = 2./3.*k_elem - 2.*dv_diffusivite_turbulente(elem)*tau ;
  double reyn = DERIVED_T::IS_TURB ? - 2.*nu_turb(elem)*tau : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB) aii = ajj =(-reyn +2.*visc_lam*tau) * surf;
  else aii = ajj =(-reyn + visc_lam*tau) * surf;
}

/* Function templates specialization for flux_arete_symetrie_fluide */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1,int fac2, int fac3, int signe, double& flux3, double& flux1_2) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), ori= orientation(fac3),
            elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);

  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), dist1 = dist_norm_bord(fac1),
         dist2 = dist_face(fac1,fac2,ori), surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

  double tau = signe * (vit_imp - inco(fac3))/dist1, tau_tr = (inco(fac2) - inco(fac1))/dist2,
         reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      flux3 = ((tau+tau_tr)*visc_lam + reyn) * surf * poros;
      flux1_2 = ((tau+tau_tr)* visc_lam + reyn) * surfporos;
    }
  else
    {
      flux3 = (tau*visc_lam + reyn) * surf * poros;
      flux1_2 = DERIVED_T::IS_VAR ? 0.5 * tau_tr * visc_lam * surfporos : (tau_tr*visc_lam + reyn) * surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2,int fac3, int signe,double& aii1_2, double& aii3_4,double& ajj1_2) const
{
  const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori),dist1 = dist_norm_bord(fac1),
         dist2 = dist_face(fac1,fac2,ori), surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2), surfporos = surface(fac3)*porosite(fac3),
         tau = signe/dist1, tau_tr = 1/dist2, reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    {
      aii3_4 = ((tau+tau_tr)*visc_lam + reyn) * surf * poros;
      aii1_2 = ajj1_2  = ((tau+tau_tr)*visc_lam + reyn) * surfporos;
    }
  else
    {
      aii3_4 = (tau*visc_lam + reyn)*surf*poros;
      aii1_2 = ajj1_2  = DERIVED_T::IS_VAR ? 0.5 * tau_tr * visc_lam * surfporos : (tau_tr * visc_lam + reyn) * surfporos;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3,int signe,double& flux3, double& flux1_2) const
{
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), elem1 = elem_(fac3,0),
            elem2 = elem_(fac3,1), ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori), dist = dist_norm_bord(fac1),
         surf = surface_(fac1,fac2), poros = porosity_(fac1,fac2);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double tau = signe*vit_imp/dist, reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;

  flux3 = DERIVED_T::IS_TURB ? (tau*visc_lam + reyn) * surf * poros : tau*surf*visc_lam*poros;
  flux1_2 = 0;
}

/* Function templates specialization for flux_arete_symetrie_paroi */
template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, double>
Eval_Diff_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  const int rang1 = (fac1-premiere_face_bord), rang2 = (fac2-premiere_face_bord), ori = orientation(fac3);

  if ( !uses_wall_law() )
    {
      const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori),
             surf = surface_(fac1,fac2), dist = dist_norm_bord(fac1);

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                            Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

      double tau  = signe*(vit_imp - inco(fac3))/dist;

      flux = DERIVED_T::IS_TURB ? tau * surf * (visc_lam + visc_turb) : tau * surf * visc_lam;
    }
  else
    {
      // solution retenue pour le calcul du frottement sachant que sur l'une des faces
      // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      flux = tau1 + tau2;
    }
  return flux;
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3,int signe, double& aii1_2,double& aii3_4, double& ajj1_2) const
{
  if ( !uses_wall_law() )
    {
      const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1), ori = orientation(fac3);
      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori), visc_turb = nu_mean_2pts(elem1,elem2,ori),
             dist = dist_norm_bord(fac1), surf = surface_(fac1,fac2);

      aii1_2 = ajj1_2 = 0;
      aii3_4 = DERIVED_T::IS_TURB ? ( DERIVED_T::IS_VAR ? signe * surf * (visc_lam + visc_turb) / dist : 0.0 )
                 : signe * surf * visc_lam / dist;
    }
  else aii3_4 = aii1_2 = ajj1_2 =0;
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, double>
Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori = orientation(fac3);

  if ( !uses_wall_law() )
    {
      const int elem1 = elem_(fac3,0), elem2 = elem_(fac3,1);
      double dist = dist_norm_bord(fac1), visc_lam = nu_lam_mean_2pts(elem1,elem2,ori),
             visc_turb = nu_mean_2pts(elem1,elem2,ori), surf = surface_(fac1,fac2);
      const DoubleTab& inco = inconnue->valeurs();

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

      // WHY WHY WHY ?
      double tau  = DERIVED_T::IS_TURB ? signe * (vit_imp - inco(fac3)) / dist :
                    signe * vit_imp / dist;

      flux = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau * surf * (visc_lam + visc_turb) :
             tau * surf * visc_lam;
    }
  else
    {
      // solution retenue pour le calcul du frottement sachant que sur l'une des faces
      // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      flux = tau1 + tau2;
    }
  return flux;
}

#endif /* Eval_Diff_VDF_Face_included */
