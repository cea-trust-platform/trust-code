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

#include <Evaluateur_VDF.h>
#include <Eval_VDF_Face2.h>
#include <Neumann_sortie_libre.h>
#include <Zone_VDF.h>

//
// .DESCRIPTION class Eval_Diff_VDF_Face
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_Face

//
// .SECTION voir aussi Eval_VDF_Face2, Evaluateur_VDF

template <typename DERIVED_T>
class Eval_Diff_VDF_Face : public Eval_VDF_Face2, public Evaluateur_VDF
{

public:
  static constexpr bool IS_VAR = false;
  static constexpr bool IS_TURB = false;

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

  inline int calculer_fa7_sortie_libre() const { return DERIVED_T::IS_TURB ? 1 : 0; }
  inline int calculer_arete_fluide() const { return 1; }
  inline int calculer_arete_paroi() const { return 1; }
  inline int calculer_arete_paroi_fluide() const { return DERIVED_T::IS_TURB ? 0 : 1; }
  inline int calculer_arete_symetrie() const { return 0; }
  inline int calculer_arete_interne() const { return 1; }
  inline int calculer_arete_mixte() const { return 1; }
  inline int calculer_arete_periodicite() const { return 1; }
  inline int calculer_arete_symetrie_paroi() const { return 1; }
  inline int calculer_arete_symetrie_fluide() const { return 1; }
  inline int calculer_arete_coin_fluide() const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
    return 1;
  }

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles renvoient le flux calcule

  inline double flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const { return 0; }
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const { return 0; }
  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, double&, double&) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, double&, double&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const ;
  inline void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int, double&, double&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
  }

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_arete_interne(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_arete_mixte(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_arete_paroi(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const { /* Do nothing */ }
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int, DoubleVect& flux) const  { /* Do nothing */ }
  inline void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
  };

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj ) const { aii = ajj = 0; }
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_coin_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const { /* Do nothing */ }

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas scalaire.

  inline double secmem_fa7_elem( int, int, int) const { return 0; }
  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const { return 0; }
  inline double secmem_arete_interne(int, int, int, int) const { return 0; }
  inline double secmem_arete_mixte(int, int, int, int) const { return 0; }
  inline double secmem_arete_symetrie(int, int, int, int) const { return 0; }
  inline double secmem_arete_paroi(int, int, int, int ) const;
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const { /* Do nothing */ }
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const;
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const;
  inline void secmem_arete_coin_fluide(int, int, int, int, double&, double&) const { /* Do nothing */ }

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const { /* Do nothing */ }
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const { /* Do nothing */ }
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const { /* Do nothing */ }
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const { /* Do nothing */ }
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const { /* Do nothing */ }
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }
  inline void coeffs_arete_coin_fluide(int, int, int, int,DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* Do nothing */ }


  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas vectoriel.

  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const { /* Do nothing */ }
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const { /* Do nothing */ }
  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const { /* Do nothing */ }
  inline void secmem_arete_coin_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* Do nothing */ }

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

//// flux_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_fluide(const DoubleTab& inco, int fac1,
                                                             int fac2, int fac3, int signe,
                                                             double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  double surfporos = surface(fac3)*porosite(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double dist1 = dist_norm_bord(fac1);
  double dist2 = dist_face(fac1,fac2,ori);

  double tau = signe * (vit_imp - inco[fac3])/dist1;
  double tau_tr = (inco[fac2] - inco[fac1])/dist2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// coeffs_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                               double& aii1_2, double& aii3_4,
                                                               double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  double surfporos = surface(fac3)*porosite(fac3);
  double dist1 = dist_norm_bord(fac1);
  double dist2 = dist_face(fac1,fac2,ori);
  double tau = signe/dist1;
  double tau_tr = 1/dist2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// secmem_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                               double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  double dist = dist_norm_bord(fac1);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double tau = signe*vit_imp/dist;
  double reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;

  // XXX : if we regroup => SOME TEST CASES BROKE UP !
  if(DERIVED_T::IS_TURB)
    flux3 = (tau*visc_lam + reyn)*surf*poros;
  else
    flux3 = tau*surf*visc_lam*poros;

  flux1_2 = 0;
}

//// flux_arete_interne
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                                int fac2, int fac3, int fac4) const
{
  double flux;
  int ori1=orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);
  double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);

  double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1);
  double tau_tr = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori3);
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    flux = (reyn + visc_lam*(tau+tau_tr))* surf*poros;
  else
    flux = (reyn + visc_lam*tau)*surf*poros;

  return flux;
}

//// coeffs_arete_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_interne(int fac1, int fac2, int fac3, int fac4,
                                                                double& aii, double& ajj) const
{
  int ori1=orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);
  double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);

  double tau = 1/dist_face(fac3,fac4,ori1);
  double tau_tr = 1/dist_face(fac1,fac2,ori3);
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

  if (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    aii = ajj = (reyn + visc_lam*(tau+tau_tr))*surf*poros;
  else
    aii = ajj = (reyn + visc_lam*tau)*surf*poros;
}

//// flux_arete_mixte
//
// DIFT : Sur les aretes mixtes les termes croises du tenseur de Reynolds
// sont nuls: il ne reste donc que la diffusion laminaire
// E Saikali ???? Are you sure ?
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_mixte(const DoubleTab& inco, int fac1,
                                                              int fac2, int fac3, int fac4) const
{
  double flux=0;

  if (inco[fac4]*inco[fac3] != 0)
    {
      int ori1 = orientation(fac1);
      int ori3 = orientation(fac3);
      int elem[4];
      elem[0] = elem_(fac3,0);
      elem[1] = elem_(fac3,1);
      elem[2] = elem_(fac4,0);
      elem[3] = elem_(fac4,1);

      double visc_lam_temp=0;
      double visc_turb_temp=0;
      for (int i=0; i<4; i++)
        if (elem[i] != -1)
          {
            visc_lam_temp += nu_lam(elem[i]);
            visc_turb_temp += nu_turb(elem[i]);
          }
      visc_lam_temp/=3.0;
      visc_turb_temp/=3.0;

      double visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0);
      double visc_turb = visc_turb_temp;
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);

      double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1);
      double tau_tr = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori3);
      double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      if (DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
        flux = (reyn + visc_lam*(tau+tau_tr)) * surf * poros;
      else
        flux = tau * surf * visc_lam * poros;
    }
  return flux;
}

//// coeffs_arete_mixte
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4,
                                                              double& aii, double& ajj) const
{
  if (inconnue->valeurs()[fac4]*inconnue->valeurs()[fac3] != 0)
    {
      int ori1 = orientation(fac1);
      int ori3 = orientation(fac3);
      int elem[4];
      elem[0] = elem_(fac3,0);
      elem[1] = elem_(fac3,1);
      elem[2] = elem_(fac4,0);
      elem[3] = elem_(fac4,1);

      double visc_lam_temp=0;
      double visc_turb_temp=0;
      for (int i=0; i<4; i++)
        if (elem[i] != -1)
          {
            visc_lam_temp += nu_lam(elem[i]);
            visc_turb_temp += nu_turb(elem[i]);
          }
      visc_lam_temp/=3.0;
      visc_turb_temp/=3.0;

      double visc_lam = DERIVED_T::IS_VAR ? visc_lam_temp : nu_lam(0);
      double visc_turb = visc_turb_temp;
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);

      double tau = 1/dist_face(fac3,fac4,ori1);
      double tau_tr = 1/dist_face(fac1,fac2,ori3);
      double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

      // XXX : if we regroup => SOME TEST CASES BROKE UP !
      if (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR)
        aii = ajj = (reyn + visc_lam*(tau+tau_tr))* surf * poros;
      else
        aii = ajj= surf * visc_lam * poros * tau;
    }
  else
    aii=ajj=0;
}

//// flux_arete_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_paroi(const DoubleTab& inco, int fac1,
                                                              int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori = orientation(fac3);
  double vit = inco(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  if ( !uses_wall_law() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      if (elem1==-1)
        elem1 = elem2;
      else if (elem2==-1)
        elem2 = elem1;

      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
      double visc_turb = nu_mean_2pts(elem1,elem2,ori);
      double dist = dist_norm_bord(fac1);
      double tau  = signe*(vit_imp - inco[fac3])/dist;
      double surf = surface_(fac1,fac2);
      if (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR)
        flux = tau*surf*(visc_lam+visc_turb);
      else
        flux = tau*surf*visc_lam;
    }
  else
    {
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      int signe_terme;
      if ( vit < vit_imp )
        signe_terme = -1;
      else
        signe_terme = 1;

      //30/09/2003  YB : influence de signe terme eliminee, signe pris en compte dans la loi de paroi
      signe_terme = 1;
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      double coef = tau1+tau2;
      flux = signe_terme*coef;
    }

  return flux;
}

//// coeffs_arete_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4,
                                                              double& ajj1_2) const
{
  if ( !uses_wall_law())
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      if (elem1==-1)
        elem1 = elem2;
      else if (elem2==-1)
        elem2 = elem1;

      int ori = orientation(fac3);
      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
      double visc_turb = nu_mean_2pts(elem1,elem2,ori);
      double dist = dist_norm_bord(fac1);
      double surf = surface_(fac1,fac2);

      aii1_2 = ajj1_2 = 0;

      aii3_4 = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? signe*surf*(visc_lam+visc_turb)/dist :
               signe*surf*visc_lam/dist;
    }
  else
    aii3_4 = aii1_2 = ajj1_2 = 0;
}


//// secmem_arete_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int ori = orientation(fac3);
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);

  const DoubleTab& inco = inconnue->valeurs();
  double vit = inco(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);

  if ( !uses_wall_law() )
    {
      double dist = dist_norm_bord(fac1);
      double surf = surface_(fac1,fac2);
      double tau  = DERIVED_T::IS_TURB ? signe*(vit_imp - inco[fac3])/dist : signe * vit_imp/dist;

      flux = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau*surf*(visc_lam+visc_turb) :
             tau * surf * visc_lam;
    }
  else
    {
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      int signe_terme;
      if ( vit < vit_imp )
        signe_terme = -1;
      else
        signe_terme = 1;

      //30/09/2003  YB : influence de signe terme eliminee, signe pris en compte dans la loi de paroi
      signe_terme = 1;
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      double coef = tau1+tau2;
      flux = signe_terme*coef;
    }

  return flux;
}

//// flux_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_paroi_fluide(const DoubleTab& inco, int fac1,
                                                                   int fac2, int fac3, int signe,
                                                                   double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori= orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double vit_imp;

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
  if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

  double dist1 = dist_norm_bord(fac1);
  double dist2 =dist_face(fac1,fac2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);
  double surfporos = surface(fac3)*porosite(fac3);

  double tau = signe * (vit_imp - inco[fac3])/dist1;
  double tau_tr = (inco[fac2] - inco[fac1])/dist2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// coeffs_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                     double& aii1_2, double& aii3_4,
                                                                     double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);
  double surfporos = surface(fac3)*porosite(fac3);
  double dist1 = dist_norm_bord(fac1);
  double dist2 = dist_face(fac1,fac2,ori);

  double tau = signe/dist1;
  double tau_tr = 1/dist2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// secmem_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                     double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori= orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);
  double dist = dist_norm_bord(fac1);
  double vit_imp;

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
  if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

  double tau = signe*vit_imp/dist;
  double reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;

  if(DERIVED_T::IS_TURB)
    flux3 = (tau*visc_lam + reyn) * surf * poros;
  else
    flux3 = tau * surf * visc_lam * poros;

  flux1_2 = 0;
}

//// flux_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_periodicite(const DoubleTab& inco,
                                                                  int fac1, int fac2, int fac3, int fac4,
                                                                  double& flux3_4, double& flux1_2) const
{
  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);
  double dist3_4 = dist_face_period(fac3,fac4,ori1);
  double dist1_2 = dist_face_period(fac1,fac2,ori3);

  double surf1_2 = surface_(fac1,fac2);
  double poros1_2 = porosity_(fac1, fac2);
  double surf3_4 = surface_(fac3,fac4);
  double poros3_4 = porosity_(fac3, fac4);
  double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3);

  double tau = (inco[fac4]-inco[fac3])/dist3_4;
  double tau_tr = (inco[fac2]-inco[fac1])/dist1_2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// coeffs_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                                    double& aii, double& ajj) const
{
  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);
  double dist3_4 = dist_face_period(fac3,fac4,ori1);
  double dist1_2 = dist_face_period(fac1,fac2,ori3);
  double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4,ori3);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);

  double tau = 1/dist3_4;
  double tau_tr = 1/dist1_2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr) * visc_turb : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    aii = ajj = (reyn + visc_lam*(tau+tau_tr))* surf * poros;
  else
    aii = ajj = (reyn + visc_lam*tau) * surf * poros;
}

//// flux_fa7_elem
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7_elem(const DoubleTab& inco, int elem,
                                                           int fac1, int fac2) const
{
  double flux;
  int ori=orientation(fac1);
  double dist = dist_face(fac1,fac2,ori);
  double tau = (inco[fac2]-inco[fac1])/dist;
  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  double visc_lam = nu_lam(elem);

  // On considere que l energie est dans la pression
  double reyn = DERIVED_T::IS_TURB ? - 2.*nu_turb(elem)*tau : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    flux = (-reyn + 2.*visc_lam*tau) * surf ;
  else
    flux = (-reyn + visc_lam*tau) * surf ;

  return flux;
}

//// coeffs_fa7_elem
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7_elem(int elem,int fac1,
                                                           int fac2, double& aii, double& ajj) const
{
  int ori = orientation(fac1);
  double tau = 1/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  double visc_lam = nu_lam(elem);

  // On considere que l energie est dans la pression
  //  double reyn = 2./3.*k_elem - 2.*dv_diffusivite_turbulente(elem)*tau ;
  double reyn = DERIVED_T::IS_TURB ? - 2.*nu_turb(elem)*tau : 0.0;

  if(DERIVED_T::IS_VAR && DERIVED_T::IS_TURB)
    aii = ajj =(-reyn +2.*visc_lam*tau) * surf;
  else
    aii = ajj =(-reyn + visc_lam*tau) * surf;
}

//// flux_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1,
                                                                      int fac2, int fac3, int signe,
                                                                      double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori= orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double dist1 = dist_norm_bord(fac1);
  double dist2 = dist_face(fac1,fac2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  double surfporos = surface(fac3)*porosite(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

  double tau = signe * (vit_imp - inco[fac3])/dist1;
  double tau_tr = (inco[fac2] - inco[fac1])/dist2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// coeffs_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_fluide(int fac1, int fac2,
                                                                        int fac3, int signe,
                                                                        double& aii1_2, double& aii3_4,
                                                                        double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double dist1 = dist_norm_bord(fac1);
  double dist2 = dist_face(fac1,fac2,ori);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  double surfporos = surface(fac3)*porosite(fac3);

  double tau = signe/dist1;
  double tau_tr = 1/dist2;
  double reyn = DERIVED_T::IS_TURB ? (tau + tau_tr)*visc_turb : 0.0;

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

//// secmem_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3,
                                                                        int signe,
                                                                        double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
  double visc_turb = nu_mean_2pts(elem1,elem2,ori);
  double dist = dist_norm_bord(fac1);
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double tau = signe*vit_imp/dist;
  double reyn = DERIVED_T::IS_TURB ? tau*visc_turb : 0.0;

  flux3 = DERIVED_T::IS_TURB ? (tau*visc_lam + reyn) * surf * poros : tau*surf*visc_lam*poros;
  flux1_2 = 0;
}

//// flux_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_paroi(const DoubleTab& inco, int fac1,
                                                                       int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori = orientation(fac3);

  if ( !uses_wall_law() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
      double visc_turb = nu_mean_2pts(elem1,elem2,ori);
      double surf =  surface_(fac1,fac2);
      double dist = dist_norm_bord(fac1);

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                            Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

      double tau  = signe*(vit_imp - inco[fac3])/dist;

      flux = DERIVED_T::IS_TURB ? tau * surf * (visc_lam + visc_turb) : tau * surf * visc_lam;
    }
  else
    {
      // solution retenue pour le calcul du frottement sachant que sur l'une des faces
      // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      flux = tau1+tau2;
    }
  return flux;
}

//// coeffs_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_paroi(int fac1, int fac2, int fac3,
                                                                       int signe, double& aii1_2,
                                                                       double& aii3_4, double& ajj1_2) const
{
  if ( !uses_wall_law() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      int ori = orientation(fac3);
      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
      double visc_turb = nu_mean_2pts(elem1,elem2,ori);
      double dist = dist_norm_bord(fac1);
      double surf = surface_(fac1,fac2);

      aii1_2 = ajj1_2 = 0;
      aii3_4 = DERIVED_T::IS_TURB ? ( DERIVED_T::IS_VAR ? signe * surf * (visc_lam + visc_turb) / dist : 0.0 )
                 : signe * surf * visc_lam / dist;
    }
  else
    aii3_4 = aii1_2 = ajj1_2 =0;
}

//// secmem_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_paroi(int fac1, int fac2, int fac3,
                                                                         int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori = orientation(fac3);

  if ( !uses_wall_law() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      double dist = dist_norm_bord(fac1);
      double visc_lam = nu_lam_mean_2pts(elem1,elem2,ori);
      double visc_turb = nu_mean_2pts(elem1,elem2,ori);
      double surf = surface_(fac1,fac2);
      const DoubleTab& inco = inconnue->valeurs();

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

      // WHY WHY WHY ?
      double tau  = DERIVED_T::IS_TURB ? signe * (vit_imp - inco[fac3]) / dist :
                    signe * vit_imp / dist;

      flux = (DERIVED_T::IS_TURB && DERIVED_T::IS_VAR) ? tau * surf * (visc_lam + visc_turb) :
             tau * surf * visc_lam;
    }
  else
    {
      // solution retenue pour le calcul du frottement sachant que sur l'une des faces
      // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      flux = tau1 + tau2;
    }

  return flux;
}

#endif /* Eval_Diff_VDF_Face_included */
