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
// Le champ de diffusivite est constant.

//
// .SECTION voir aussi Eval_Diff_VDF_const

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
  inline double nu_mean_2pts(int i=0, int j=0) const;
  inline double nu_mean_4pts(int i, int j, int k, int l) const;
  inline double nu_lam(int i, int j=0) const;
  inline double nu_lam_mean_4pts(int i, int j, int k, int l) const;
  inline double nu_lam_mean_2pts(int i, int j) const;
  inline double nu_turb(int i) const;
  inline double tau_tan(int,int) const;
  inline bool uses_wall_law() const;

  inline int calculer_fa7_sortie_libre() const
  {
    return DERIVED_T::IS_TURB ? 1 : 0;
  }
  inline int calculer_arete_fluide() const
  {
    return 1;
  }
  inline int calculer_arete_paroi() const
  {
    return 1;
  }
  inline int calculer_arete_paroi_fluide() const
  {
    return DERIVED_T::IS_TURB ? 0 : 1;
  }
  inline int calculer_arete_symetrie() const
  {
    return 0;
  }
  inline int calculer_arete_interne() const
  {
    return 1;
  }
  inline int calculer_arete_mixte() const
  {
    return 1;
  }
  inline int calculer_arete_periodicite() const
  {
    return 1;
  }
  inline int calculer_arete_symetrie_paroi() const
  {
    return 1;
  }
  inline int calculer_arete_symetrie_fluide() const
  {
    return 1;
  }

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles renvoient le flux calcule

  inline double flux_fa7_sortie_libre(const DoubleTab&, int ,
                                      const Neumann_sortie_libre&, int ) const
  {
    return 0;
  }

  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const
  {
    return 0;
  }
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int,
                                double&, double&) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int,
                                      double&, double&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const ;


  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&,
                                    int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void flux_arete_interne(const DoubleTab&, int, int, int,
                                 int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void flux_arete_mixte(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int,
                                  int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void flux_arete_paroi(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void flux_arete_fluide(const DoubleTab&, int, int, int,
                                int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect& ) const
  {
    /* Do nothing */
  }
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                         int, int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int,
                                        int, DoubleVect& flux) const
  {
    /* Do nothing */
  }

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj ) const
  {
    aii = ajj = 0;
  }
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int,
                                           double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, double& aii1_2,
                                          double& aii3_4, double& ajj1_2) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas scalaire.

  inline double secmem_fa7_elem( int, int, int) const
  {
    return 0;
  }
  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const
  {
    return 0;
  }
  inline double secmem_arete_interne(int, int, int, int) const
  {
    return 0;
  }
  inline double secmem_arete_mixte(int, int, int, int) const
  {
    return 0;
  }
  inline double secmem_arete_symetrie(int, int, int, int) const
  {
    return 0;
  }
  inline double secmem_arete_paroi(int, int, int, int ) const;
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const;
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const
  {
    /* Do nothing */
  }
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_symetrie_fluide(int, int, int, int,
                                           DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    /* Do nothing */
  }
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    /* Do nothing */
  }

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas vectoriel.

  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    /* Do nothing */
  }
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const
  {
    /* Do nothing */
  }
};

//************************
// CRTP pattern
//************************

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_mean_2pts(int i, int j) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_1_impl_face(i, j);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_mean_4pts(int i, int j, int k, int l) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_2_impl_face(i, j, k, l);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_lam(int i, int j) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_2_impl(i,j); // Attention nu_2_impl and not nu_1_impl for Dift ...
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_lam_mean_4pts(int i, int j, int k, int l) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_lam_impl_face(i,j,k,l);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_lam_mean_2pts(int i, int j) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_lam_impl_face2(i,j);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_turb(int i) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_t_impl(i);
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

//************************
// Internal methods
//************************

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::surface_(int i,int j) const
{
  const double surf = DERIVED_T::IS_VAR ? 0.5*(surface(i)+surface(j)) :
                      0.5*(surface(i)*porosite(i)+surface(j)*porosite(j));
  return surf;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::porosity_(int i,int j) const
{
  const double poros = DERIVED_T::IS_VAR ? 0.5*(porosite(i)+porosite(j)) : 1.0;
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
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe * (vit_imp - inco[fac3])/dist;
          double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = ((tau+tau_tr)*visc_lam + reyn);
          flux3 = coef*surf*poros;
          flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe * (vit_imp - inco[fac3])/dist;
          double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = (tau*visc_lam + reyn);
          flux3 = coef*surf*poros;
          flux1_2 = (tau_tr*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
    }
  else
    {
      double tau,dist;
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);
      double diffus=nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);

      // Calcul de flux3
      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));

      dist = dist_norm_bord(fac1);
      tau = signe * (vit_imp - inco[fac3])/dist;
      flux3 = tau*surf*poros*diffus;

      // Calcul de flux1_2
      dist = dist_face(fac1,fac2,k);
      tau = (inco(fac2) - inco(fac1))/dist;
      flux1_2 = tau*diffus*porosite(fac3)*surface(fac3);
    }
}

//// coeffs_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                               double& aii1_2, double& aii3_4,
                                                               double& ajj1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double dist = dist_norm_bord(fac1);
          double tau = signe/dist;
          double tau_tr = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = ((tau+tau_tr)*visc_lam + reyn);

          // Calcul de aii3_4
          aii3_4 = coef*surf*poros;

          // Calcul de aii1_2 et ajj1_2
          aii1_2 = ajj1_2  = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
      else
        {
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double dist = dist_norm_bord(fac1);
          double tau = signe/dist;
          double tau_tr = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = (tau*visc_lam + reyn);

          // Calcul de aii3_4
          aii3_4 = coef*surf*poros;

          // Calcul de aii1_2 et ajj1_2
          aii1_2 = ajj1_2  = (tau_tr*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
    }
  else
    {
      double dist;
      int k= orientation(fac3);
      double diffus=nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);

      // Calcul de aii3_4
      dist = dist_norm_bord(fac1);
      aii3_4 = signe*surf*diffus*poros/dist;

      // Calcul de aii1_2 et ajj1_2
      dist = dist_face(fac1,fac2,k);
      aii1_2 = ajj1_2  = (diffus*porosite(fac3)*surface(fac3))/dist;
    }
}

//// secmem_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                               double& flux3, double& flux1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe*vit_imp/dist;
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = tau*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = 0;
        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe*vit_imp/dist;
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = tau*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = 0;
        }
    }
  else
    {
      double tau,dist;
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      // Calcul de flux3
      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));

      dist = dist_norm_bord(fac1);
      tau = signe * vit_imp/dist;

      double diffus=nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);

      flux3 = tau*surf*diffus*poros;
      flux1_2 = 0;
    }
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

  // TODO : FACTORIZE AND MAKE GENERAL

  if (DERIVED_T::IS_TURB)
    {
      double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4);
      double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4);
      double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1);
      double tau_tr = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori3);
      double reyn = (tau + tau_tr)*visc_turb;
      if (DERIVED_T::IS_VAR)
        flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
               *(porosite(fac1)+porosite(fac2));
      else
        flux = 0.25*(reyn + visc_lam*tau)*(surface(fac1)+surface(fac2))
               *(porosite(fac1)+porosite(fac2));

    }
  else
    {
      double diffus=nu_mean_4pts(elem1,elem2,elem3,elem4);
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);

      flux = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1) * poros  * surf * diffus;
      // XXX XXX XXX : can any one explain the difference with these expressions ...
      // test case Champ_fonc_reprise_singlehdf was broken because of this !!!!!
      //  flux = poros  * surf  * diffus * (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori) ;
      //  flux = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori) * diffus * surf  * poros;
    }


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

  // TODO : FACTORIZE AND MAKE GENERAL

  if (DERIVED_T::IS_TURB)
    {
      double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4);
      double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4);
      double tau = 1/dist_face(fac3,fac4,ori1);
      double tau_tr = 1/dist_face(fac1,fac2,ori3);
      double reyn = (tau + tau_tr)*visc_turb;

      if (DERIVED_T::IS_VAR)
        aii = ajj = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
                    *(porosite(fac1)+porosite(fac2));
      else
        aii = ajj = 0.25*(reyn + visc_lam*tau)*(surface(fac1)+surface(fac2))
                    *(porosite(fac1)+porosite(fac2));
    }
  else
    {
      double diffus=nu_mean_4pts(elem1,elem2,elem3,elem4);
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);
      aii = ajj = diffus*surf*poros/dist_face(fac3,fac4,ori1);
    }
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
  if(DERIVED_T::IS_TURB)
    {
      if (DERIVED_T::IS_VAR)
        {
          if (inco[fac4]*inco[fac3] != 0)
            {
              int ori1 = orientation(fac1);
              int ori3 = orientation(fac3);
              int elem[4];
              elem[0] = elem_(fac3,0);
              elem[1] = elem_(fac3,1);
              elem[2] = elem_(fac4,0);
              elem[3] = elem_(fac4,1);

              double visc_lam=0;
              double visc_turb=0;
              for (int i=0; i<4; i++)
                if (elem[i] != -1)
                  {
                    visc_lam += nu_lam(elem[i]);
                    visc_turb += nu_turb(elem[i]);
                  }
              visc_lam/=3.0;
              visc_turb/=3.0;

              double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1);
              double tau_tr = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori3);
              double reyn = (tau + tau_tr)*visc_turb;
              flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
                     *(porosite(fac1)+porosite(fac2));
            }
        }
      else
        {
          if (inco[fac4]*inco[fac3] != 0)
            {
              int ori=orientation(fac1);
              double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori);
              flux = 0.25*tau*(surface(fac1)+surface(fac2))*
                     nu_lam(0)*(porosite(fac1)+porosite(fac2));
            }
        }
    }
  else
    {
      double diffus=0;

      if (inco[fac4]*inco[fac3] != 0)
        {
          if (DERIVED_T::IS_VAR)
            {
              int element;

              if ((element=elem_(fac3,0)) != -1)
                diffus+=nu_lam(element);
              if ((element=elem_(fac3,1)) != -1)
                diffus+=nu_lam(element);
              if ((element=elem_(fac4,0)) != -1)
                diffus+=nu_lam(element);
              if ((element=elem_(fac4,1)) != -1)
                diffus+=nu_lam(element);

              diffus/=3.0;
            }
          else
            {
              diffus=nu_mean_2pts();
            }

          int ori=orientation(fac1);
          double surf = surface_(fac1,fac2);
          double poros = porosity_(fac1,fac2);
          double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori);
          flux = tau*diffus*surf*poros;
        }
    }
  return flux;
}


//// coeffs_arete_mixte
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4,
                                                              double& aii, double& ajj) const
{
  if(DERIVED_T::IS_TURB)
    {
      if (DERIVED_T::IS_VAR)
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

              double visc_lam=0;
              double visc_turb=0;
              for (int i=0; i<4; i++)
                if (elem[i] != -1)
                  {
                    visc_lam += nu_lam(elem[i]);
                    visc_turb += nu_turb(elem[i]);
                  }
              visc_lam/=3.0;
              visc_turb/=3.0;

              double tau = 1/dist_face(fac3,fac4,ori1);
              double tau_tr = 1/dist_face(fac1,fac2,ori3);
              double reyn = (tau + tau_tr)*visc_turb;

              aii = ajj = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
                          *(porosite(fac1)+porosite(fac2));
            }
          else
            {
              aii=ajj=0;
            }
        }
      else
        {
          if (inconnue->valeurs()[fac4]*inconnue->valeurs()[fac3] != 0)
            {
              int ori=orientation(fac1);
              aii = ajj= 0.25*(surface(fac1)+surface(fac2))*nu_lam(0)*
                         (porosite(fac1)+porosite(fac2))/dist_face(fac3,fac4,ori);
            }
          else
            {
              aii=ajj=0;
            }
        }
    }
  else
    {
      if (inconnue->valeurs()[fac4]*inconnue->valeurs()[fac3] != 0)
        {
          int ori=orientation(fac1);
          double diffus=0;
          if (DERIVED_T::IS_VAR)
            {
              int element;

              if ((element=elem_(fac3,0)) != -1)
                diffus+=nu_lam(element);
              if ((element=elem_(fac3,1)) != -1)
                diffus+=nu_lam(element);
              if ((element=elem_(fac4,0)) != -1)
                diffus+=nu_lam(element);
              if ((element=elem_(fac4,1)) != -1)
                diffus+=nu_lam(element);

              diffus/=3.0;
            }
          else
            {
              diffus=nu_mean_2pts();
            }
          double surf = surface_(fac1,fac2);
          double poros = porosity_(fac1,fac2);
          aii = ajj= surf*diffus*poros/dist_face(fac3,fac4,ori);
        }
      else
        {
          aii=ajj=0;
        }
    }
}

//// flux_arete_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_paroi(const DoubleTab& inco, int fac1,
                                                              int fac2, int fac3, int signe) const
{
  double flux;
  if (DERIVED_T::IS_TURB)
    {
      if (DERIVED_T::IS_VAR)
        {
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
              double visc_lam = nu_lam_mean_2pts(elem1,elem2);
              double visc_turb = nu_mean_2pts(elem1,elem2);
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*(visc_lam+visc_turb);
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
        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int ori = orientation(fac3);
          double vit = inco(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));
          if ( !uses_wall_law() )
            {
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*nu_lam(0);
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
        }
    }
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      if (elem1==-1)
        elem1 = elem2;
      else if (elem2==-1)
        elem2 = elem1;

      double diffus= nu_mean_2pts(elem1,elem2); //0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));

      double dist = dist_norm_bord(fac1);
      double tau  = signe * (vit_imp - inco[fac3])/dist;
      double surf = surface_(fac1,fac2);
      flux = tau*surf*diffus;
    }
  return flux;
}

//// coeffs_arete_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4,
                                                              double& ajj1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          if ( !uses_wall_law())
            {
              int elem1 = elem_(fac3,0);
              int elem2 = elem_(fac3,1);
              if (elem1==-1)
                elem1 = elem2;
              else if (elem2==-1)
                elem2 = elem1;

              double visc_lam = nu_lam_mean_2pts(elem1,elem2);
              double visc_turb = nu_mean_2pts(elem1,elem2);
              double dist = dist_norm_bord(fac1);
              double surf = 0.5*(surface(fac1)+surface(fac2));
              aii3_4 = signe*surf*(visc_lam+visc_turb)/dist;
              aii1_2 = 0;
              ajj1_2 = 0;
            }
          else
            {
              aii3_4 = 0;
              aii1_2 = 0;
              ajj1_2 = 0;
            }
        }
      else
        {
          aii3_4 = 0;
          aii1_2 = 0;
          ajj1_2 = 0;
        }
    }
  else
    {
      double dist = dist_norm_bord(fac1);
      double surf = surface_(fac1,fac2);
      double diffus= nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      aii3_4 = signe*surf*diffus/dist;
      aii1_2 = 0;
      ajj1_2 = 0;
    }
}


//// secmem_arete_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int ori = orientation(fac3);
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int k= orientation(fac3);
          const DoubleTab& inco = inconnue->valeurs();
          double vit = inco(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);

          if ( !uses_wall_law() )
            {
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*(visc_lam+visc_turb);
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
        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int ori = orientation(fac3);
          const DoubleTab& inco = inconnue->valeurs();
          double vit = inco(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));
          if ( !uses_wall_law())
            {
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*nu_lam(0);
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
        }
    }
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
      double dist = dist_norm_bord(fac1);
      double tau  = signe * vit_imp/dist;
      double diffus= nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      flux = tau*surf*diffus;
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
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);

          // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
          double vit_imp;
          if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
          else  // fac2 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

          double dist = dist_norm_bord(fac1);
          double tau = signe * (vit_imp - inco[fac3])/dist;
          double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = ((tau+tau_tr)*visc_lam + reyn);
          flux3 = coef*surf*poros;
          flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);

        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);

          // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
          double vit_imp;
          if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
          else  // fac2 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

          double dist = dist_norm_bord(fac1);
          double tau = signe * (vit_imp - inco[fac3])/dist;
          double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = (tau*visc_lam + reyn);
          flux3 = coef*surf*poros;
          flux1_2 = (tau_tr*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
    }
  else
    {
      double tau,dist,vit_imp;
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      // Calcul de flux
      // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
      if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
        vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl);
      else  // fac2 est la face de paroi
        vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl);

      dist = dist_norm_bord(fac1);
      tau = signe * (vit_imp - inco[fac3])/dist;
      double diffus= nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1, fac2);
      flux3 = tau*surf*diffus*poros;

      // Calcul de flux1_2
      dist = dist_face(fac1,fac2,k);
      tau = (inco[fac2] - inco[fac1])/dist;
      flux1_2 = tau*diffus*porosite(fac3)*surface(fac3);
    }
}

//// coeffs_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                     double& aii1_2, double& aii3_4,
                                                                     double& ajj1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {

          double dist;
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          int ori= orientation(fac3);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);

          //Calcul des aii et ajj 3_4
          // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
          dist = dist_norm_bord(fac1);
          double tau = signe/dist;
          double tau_tr = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = ((tau+tau_tr)*visc_lam + reyn);

          aii3_4 = coef*surf*poros;
          aii1_2 = ajj1_2 =((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
      else
        {
          double dist;
          int ori= orientation(fac3);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);

          //Calcul des aii et ajj 3_4
          // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
          dist = dist_norm_bord(fac1);
          double tau = signe/dist;
          double tau_tr = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = (tau*visc_lam + reyn);

          aii3_4 = coef*surf*poros;
          aii1_2 = ajj1_2 =(tau_tr*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
    }
  else
    {
      double dist;
      int k= orientation(fac3);

      //Calcul des aii et ajj 3_4
      // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
      dist = dist_norm_bord(fac1);
      double diffus= nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1, fac2);
      aii3_4 = signe*surf*diffus*poros/dist;

      // Calcul des aii et ajj 1_2
      dist = dist_face(fac1,fac2,k);
      aii1_2 = ajj1_2 = diffus*porosite(fac3)*surface(fac3)/dist;
    }
}

//// secmem_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                     double& flux3, double& flux1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);

          // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
          double vit_imp;
          if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
          else  // fac2 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

          double dist = dist_norm_bord(fac1);
          double tau = signe*vit_imp/dist;
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = tau*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = 0;

        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);

          // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

          double vit_imp;
          if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
          else  // fac2 est la face de paroi
            vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

          double dist = dist_norm_bord(fac1);
          double tau = signe*vit_imp/dist;
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = tau*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = 0;
        }
    }
  else
    {
      double tau,dist,vit_imp;
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      // Calcul de flux
      // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
      if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
        vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl);
      else  // fac2 est la face de paroi
        vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl);

      dist = dist_norm_bord(fac1);
      tau = signe*vit_imp/dist;

      double diffus= nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1, fac2);
      flux3 = tau*surf*diffus*poros;
      flux1_2 = 0;
    }
}

//// flux_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_periodicite(const DoubleTab& inco,
                                                                  int fac1, int fac2, int fac3, int fac4,
                                                                  double& flux3_4, double& flux1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {

          double flux;
          int ori1 = orientation(fac1);
          int ori3 = orientation(fac3);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          int elem3 = elem_(fac4,0);
          int elem4 = elem_(fac4,1);
          double dist3_4 = dist_face_period(fac3,fac4,ori1);
          double dist1_2 = dist_face_period(fac1,fac2,ori3);
          double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4);
          double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4);
          double tau = (inco[fac4]-inco[fac3])/dist3_4;
          double tau_tr = (inco[fac2]-inco[fac1])/dist1_2;
          double reyn = (tau + tau_tr)*visc_turb;

          flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          flux3_4 = flux;

          flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac3)+surface(fac4))
                 *(porosite(fac3)+porosite(fac4));

          flux1_2 = flux;
        }
      else
        {
          double flux;
          int ori1 = orientation(fac1);
          int ori3 = orientation(fac3);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          int elem3 = elem_(fac4,0);
          int elem4 = elem_(fac4,1);
          double dist3_4 = dist_face_period(fac3,fac4,ori1);
          double dist1_2 = dist_face_period(fac1,fac2,ori3);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4);
          double tau = (inco[fac4]-inco[fac3])/dist3_4;
          double tau_tr = (inco[fac2]-inco[fac1])/dist1_2;
          double reyn = (tau + tau_tr)*visc_turb;

          flux = 0.25*(reyn + visc_lam*tau)*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          flux3_4 = flux;

          flux = 0.25*(reyn + visc_lam*tau_tr)*(surface(fac3)+surface(fac4))
                 *(porosite(fac3)+porosite(fac4));

          flux1_2 = flux;
        }
    }
  else
    {
      double flux, diffus,surf,poros;
      int ori;
      ori =orientation(fac1);

      diffus= nu_mean_4pts(elem_(fac3,0),elem_(fac3,1),elem_(fac4,0),elem_(fac4,1));
      surf = surface_(fac1,fac2);
      poros = porosity_(fac1, fac2);
      flux = diffus*surf*poros*(inco[fac4]-inco[fac3])/dist_face_period(fac3,fac4,ori);
      flux3_4 = flux;

      ori=orientation(fac3);
      diffus= nu_mean_4pts(elem_(fac1,0),elem_(fac1,1),elem_(fac2,0),elem_(fac2,1));
      surf = surface_(fac3,fac4);
      poros = porosity_(fac3, fac4);
      flux = diffus*surf*poros*(inco[fac2]-inco[fac1])/dist_face_period(fac1,fac2,ori);
      flux1_2 = flux;
    }

}

//// coeffs_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                                    double& aii, double& ajj) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {

          int ori1 = orientation(fac1);
          int ori3 = orientation(fac3);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          int elem3 = elem_(fac4,0);
          int elem4 = elem_(fac4,1);
          double dist3_4 = dist_face_period(fac3,fac4,ori1);
          double dist1_2 = dist_face_period(fac1,fac2,ori3);
          double visc_lam = nu_lam_mean_4pts(elem1,elem2,elem3,elem4);
          double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4);
          double tau = 1/dist3_4;
          double tau_tr = 1/dist1_2;
          double reyn = (tau + tau_tr)*visc_turb;

          aii = ajj =0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
        }
      else
        {
          int ori1 = orientation(fac1);
          int ori3 = orientation(fac3);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          int elem3 = elem_(fac4,0);
          int elem4 = elem_(fac4,1);
          double dist3_4 = dist_face_period(fac3,fac4,ori1);
          double dist1_2 = dist_face_period(fac1,fac2,ori3);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_4pts(elem1,elem2,elem3,elem4);
          double tau = 1/dist3_4;
          double tau_tr = 1/dist1_2;
          double reyn = (tau + tau_tr)*visc_turb;

          aii = ajj =0.25*(reyn + visc_lam*tau)*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
        }
    }
  else
    {
      int ori=orientation(fac1);
      double diffus= nu_mean_4pts(elem_(fac3,0),elem_(fac3,1),elem_(fac4,0),elem_(fac4,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1, fac2);
      aii = ajj =diffus*surf*poros /dist_face_period(fac3,fac4,ori);
    }
}

//// flux_fa7_elem
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7_elem(const DoubleTab& inco, int elem,
                                                           int fac1, int fac2) const
{
  double flux;
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int ori=orientation(fac1);
          double tau = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
          double reyn = - 2.*nu_turb(elem)*tau ;
          flux = (-reyn + 2.*nu_lam(elem)*tau) * surf ;

        }
      else
        {
          int ori=orientation(fac1);
          double tau = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
          double reyn =  - 2.*nu_turb(elem)*tau ;// On considere que l energie est dans la pression
          // On verifie que les termes diagonaux du tenseur de reynolds sont bien positifs
          // Sinon on annulle :
          //  if (reyn < 0) reyn=0. ;

          // E Saikali : Les commentaires précédentes ....  On comprend rien du tout !
          flux = (-reyn + nu_lam(0)*tau) * surf ;
        }
    }
  else
    {
      int ori=orientation(fac1);
      double diffus = nu_lam(elem,0);
      flux = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) * diffus *
             (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);
    }
  return flux;
}

//// coeffs_fa7_elem
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7_elem(int elem,int fac1,
                                                           int fac2, double& aii, double& ajj) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int ori=orientation(fac1);
          double tau = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
          double reyn = - 2.*nu_turb(elem)*tau ;
          aii = ajj =(-reyn +2.*nu_lam(elem)*tau) * surf;

        }
      else
        {
          int ori=orientation(fac1);
          double tau = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
          double reyn =  - 2.*nu_turb(elem)*tau ;  // On considere que l energie est dans la pression
          //  double reyn = 2./3.*k_elem - 2.*dv_diffusivite_turbulente(elem)*tau ;
          // On verifie que les termes diagonaux du tenseur de reynolds sont bien positifs
          // Sinon on annulle :
          //  if (reyn < 0) reyn=0. ;

          aii = ajj =(-reyn + nu_lam(0)*tau) * surf;
        }
    }
  else
    {
      int ori=orientation(fac1);
      double diffus = nu_lam(elem,0);
      aii = ajj = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) *
                  diffus/dist_face(fac1,fac2,ori);
    }
}

//// flux_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1,
                                                                      int fac2, int fac3, int signe,
                                                                      double& flux3, double& flux1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe * (vit_imp - inco[fac3])/dist;
          double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = ((tau+tau_tr)*visc_lam + reyn);
          flux3 = coef*surf*poros;
          flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);

        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe * (vit_imp - inco[fac3])/dist;
          double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = (tau_tr*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
    }
  else
    {
      double tau,dist;
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      // Calcul de flux3
      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,k,la_zcl));

      dist = dist_norm_bord(fac1);
      tau = signe * (vit_imp - inco[fac3])/dist;
      double diffus = nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);
      flux3 = tau*surf*diffus*poros;

      // Calcul de flux1_2
      dist = dist_face(fac1,fac2,k);
      tau = (inco(fac2) - inco(fac1))/dist;
      flux1_2 =  DERIVED_T::IS_VAR ? 0.5*tau*diffus*porosite(fac3)*surface(fac3) :
                 tau*diffus*porosite(fac3)*surface(fac3);
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
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double dist = dist_norm_bord(fac1);
          double tau = signe/dist;
          double tau_tr = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = ((tau+tau_tr)*visc_lam + reyn);

          // Calcul de aii3_4
          aii3_4 = coef*surf*poros;

          // Calcul de aii1_2 et ajj1_2
          aii1_2 = ajj1_2  = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);

        }
      else
        {
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double dist = dist_norm_bord(fac1);
          double tau = signe/dist;
          double tau_tr = 1/dist_face(fac1,fac2,ori);
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = (tau + tau_tr)*visc_turb;
          double coef = (tau*visc_lam + reyn);

          // Calcul de aii3_4
          aii3_4 = coef*surf*poros;

          // Calcul de aii1_2 et ajj1_2
          aii1_2 = ajj1_2  = (tau_tr*visc_lam + reyn)*surface(fac3)*porosite(fac3);
        }
    }
  else
    {
      double dist;
      int k= orientation(fac3);

      // Calcul de aii3_4
      dist = dist_norm_bord(fac1);
      double diffus = nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);
      aii3_4 = (signe*surf*diffus*poros)/dist;

      // Calcul de aii1_2 et ajj1_2
      dist = dist_face(fac1,fac2,k);
      aii1_2 = ajj1_2  = DERIVED_T::IS_VAR ? (0.5*diffus*porosite(fac3)*surface(fac3))/dist :
                         (diffus*porosite(fac3)*surface(fac3))/dist;
    }
}

//// secmem_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3,
                                                                        int signe,
                                                                        double& flux3, double& flux1_2) const
{
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe*vit_imp/dist;
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = tau*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = 0;

        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam(0);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori= orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          double dist = dist_norm_bord(fac1);
          double tau = signe*vit_imp/dist;
          double surf = 0.5*(surface(fac1)+surface(fac2));
          double poros = 0.5*(porosite(fac1)+porosite(fac2));
          double reyn = tau*visc_turb;
          double coef = (tau*visc_lam + reyn);

          flux3 = coef*surf*poros;
          flux1_2 = 0;
        }
    }
  else
    {
      double tau,dist;
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      // Calcul de flux3
      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
      dist = dist_norm_bord(fac1);
      tau = signe * vit_imp/dist;
      double diffus = nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      double poros = porosity_(fac1,fac2);
      flux3 = tau*surf*diffus*poros;
      flux1_2 = 0;
    }
}

//// flux_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_paroi(const DoubleTab& inco, int fac1,
                                                                       int fac2, int fac3, int signe) const
{
  double flux;
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          int ori = orientation(fac3);
          //  double vit = inco(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));
          if ( !uses_wall_law() )
            {
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*(visc_lam+visc_turb);
            }
          else
            {
              // solution retenue pour le calcul du frottement sachant que sur l'une des faces
              // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
              // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
              double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
              double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
              double coef = tau1+tau2;
              flux = coef;
            }

        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int ori = orientation(fac3);
          //  double vit = inco(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));
          if ( !uses_wall_law() )
            {
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*nu_lam(0);
            }
          else
            {
              // solution retenue pour le calcul du frottement sachant que sur l'une des faces
              // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
              // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
              double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
              double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
              double coef = tau1+tau2;
              flux = coef;
            }
        }
    }
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,k,la_zcl));

      double dist = dist_norm_bord(fac1);
      double tau  = signe * (vit_imp - inco[fac3])/dist;
      double diffus = nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      flux = tau*surf*diffus;
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
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {

          if ( !uses_wall_law() )
            {
              int elem1 = elem_(fac3,0);
              int elem2 = elem_(fac3,1);
              double visc_lam = nu_lam_mean_2pts(elem1,elem2);
              double visc_turb = nu_mean_2pts(elem1,elem2);
              double dist = dist_norm_bord(fac1);
              double surf = 0.5*(surface(fac1)+surface(fac2));
              aii3_4 = signe*surf*(visc_lam+visc_turb)/dist;
              aii1_2 = 0;
              ajj1_2 = 0;
            }
          else
            {
              aii3_4 = 0;
              aii1_2 = 0;
              ajj1_2 = 0;
            }
        }
      else
        {
          aii3_4 = 0;
          aii1_2 = 0;
          ajj1_2 = 0;
        }
    }
  else
    {
      double dist = dist_norm_bord(fac1);
      double diffus = nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      aii3_4 = signe*surf*diffus/dist;
      ajj1_2 = 0;

      // XXX XXX XXX : what the hell ?
      if (DERIVED_T::IS_VAR)
        aii1_2 = 0;
    }
}


//// secmem_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_paroi(int fac1, int fac2, int fac3,
                                                                         int signe) const
{
  double flux;
  if(DERIVED_T::IS_TURB)
    {
      if(DERIVED_T::IS_VAR)
        {

          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int ori = orientation(fac3);
          int elem1 = elem_(fac3,0);
          int elem2 = elem_(fac3,1);
          double visc_lam = nu_lam_mean_2pts(elem1,elem2);
          double visc_turb = nu_mean_2pts(elem1,elem2);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

          if ( !uses_wall_law() )
            {
              const DoubleTab& inco = inconnue->valeurs();
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*(visc_lam+visc_turb);
            }
          else
            {
              // solution retenue pour le calcul du frottement sachant que sur l'une des faces
              // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
              // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
              double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
              double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
              double coef = tau1+tau2;
              flux = coef;
            }
        }
      else
        {
          int rang1 = (fac1-premiere_face_bord);
          int rang2 = (fac2-premiere_face_bord);
          int ori = orientation(fac3);
          double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                                Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));
          if ( !uses_wall_law())
            {
              const DoubleTab& inco = inconnue->valeurs();
              double dist = dist_norm_bord(fac1);
              double tau  = signe*(vit_imp - inco[fac3])/dist;
              double surf = 0.5*(surface(fac1)+surface(fac2));
              flux = tau*surf*nu_lam(0);
            }
          else
            {
              // solution retenue pour le calcul du frottement sachant que sur l'une des faces
              // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
              // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
              double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
              double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
              double coef = tau1+tau2;
              flux = coef;
            }
        }
    }
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int k= orientation(fac3);

      double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                            Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
      double dist = dist_norm_bord(fac1);
      double tau  = signe * vit_imp/dist;
      double diffus = nu_mean_2pts(elem_(fac3,0),elem_(fac3,1));
      double surf = surface_(fac1,fac2);
      flux = tau*surf*diffus;
    }
  return flux;
}

#endif /* Eval_Diff_VDF_Face_included */
