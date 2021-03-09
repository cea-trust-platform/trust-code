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
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_eval
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

  inline double surface_(int i,int j) const;
  inline double porosity_(int i,int j) const;

  // CRTP pattern to static_cast the appropriate class and get the implementation
  // This is magic !
  inline double nu_1(int i=0, int j=0) const;
  inline double nu_2(int i, int j, int k, int l) const;
  inline double nu_3(int i, int j=0) const;

  inline int calculer_fa7_sortie_libre() const ;
  inline int calculer_arete_fluide() const ;
  inline int calculer_arete_paroi() const ;
  inline int calculer_arete_paroi_fluide() const ;
  inline int calculer_arete_symetrie() const ;
  inline int calculer_arete_interne() const ;
  inline int calculer_arete_mixte() const ;
  inline int calculer_arete_periodicite() const;
  inline int calculer_arete_symetrie_paroi() const;
  inline int calculer_arete_symetrie_fluide() const;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles renvoient le flux calcule

  inline double flux_fa7_sortie_libre(const DoubleTab&, int ,
                                      const Neumann_sortie_libre&, int ) const;
  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const ;
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

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const;
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&,
                                    int, DoubleVect& flux) const;
  inline void flux_arete_interne(const DoubleTab&, int, int, int,
                                 int, DoubleVect& flux) const ;
  inline void flux_arete_mixte(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int,
                                  int, DoubleVect& flux) const ;
  inline void flux_arete_paroi(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int,
                                int, DoubleVect&, DoubleVect&) const ;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, DoubleVect&, DoubleVect&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                         int, int, DoubleVect&, DoubleVect&) const;
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int,
                                        int, DoubleVect& flux) const ;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
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

  inline double secmem_fa7_elem( int, int, int) const;
  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_arete_interne(int, int, int, int) const;
  inline double secmem_arete_mixte(int, int, int, int) const;
  inline double secmem_arete_symetrie(int, int, int, int) const;
  inline double secmem_arete_paroi(int, int, int, int ) const;
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const;
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int,
                                           DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas vectoriel.

  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const;
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const;
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const;
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const;
};


//************************
// CRTP pattern
//************************
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_1(int i, int j) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_1_impl_face(i, j);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_2(int i, int j, int k, int l) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_2_impl_face(i, j, k, l);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::nu_3(int i, int j) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_1_impl(i,j);
  return nu;
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

//// calculer_fa7_sortie_libre
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_fa7_sortie_libre() const
{
  return 0;
}

//// calculer_arete_fluide
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_fluide() const
{
  return 1;
}

//// calculer_arete_paroi
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_paroi() const
{
  return 1;
}


//// calculer_arete_paroi_fluide
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_paroi_fluide() const
{
  return 1;
}

//// calculer_arete_periodicite
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_periodicite() const
{
  return 1;
}


//// calculer_arete_symetrie
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_interne() const
{
  return 1;
}

//// calculer_arete_mixte
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_mixte() const
{
  return 1;
}

//// calculer_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_symetrie_paroi() const
{
  return 1;
}

//// calculer_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Face<DERIVED_T>::calculer_arete_symetrie_fluide() const
{
  return 1;
}

//////////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une inconnue scalaire
/////////////////////////////////////////////////////////////////

//// flux_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_fluide(const DoubleTab& inco, int fac1,
                                                             int fac2, int fac3, int signe,
                                                             double& flux3, double& flux1_2) const
{
  double tau,dist;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=nu_1(elem_(fac3,0),elem_(fac3,1));
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

//// coeffs_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                               double& aii1_2, double& aii3_4,
                                                               double& ajj1_2) const
{
  double dist;
  int k= orientation(fac3);
  double diffus=nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  // Calcul de aii3_4

  dist = dist_norm_bord(fac1);
  aii3_4 = signe*surf*diffus*poros/dist;

  // Calcul de aii1_2 et ajj1_2
  dist = dist_face(fac1,fac2,k);
  aii1_2 = ajj1_2  = (diffus*porosite(fac3)*surface(fac3))/dist;
}

//// secmem_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                               double& flux3, double& flux1_2) const
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

  double diffus=nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);

  flux3 = tau*surf*diffus*poros;
  flux1_2 = 0;
}

//// flux_arete_interne
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                                int fac2, int fac3, int fac4) const
{
  //Cout << " Dans Eval_Diff_VDF_Face " << porosite(fac1) << " " << porosite(fac2) << finl;
  double flux;
  int ori=orientation(fac1);
  double diffus=nu_2(elem_(fac3,0),elem_(fac3,1),elem_(fac4,0),elem_(fac4,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
  flux = diffus*surf*poros*(inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori);

//
//  flux = 0.5 * (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori) *
//         (surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2))*nu_1();
  return flux;
}

//// coeffs_arete_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_interne(int fac1, int fac2, int fac3, int fac4,
                                                                double& aii, double& ajj) const
{
  int ori=orientation(fac1);
  double diffus=nu_2(elem_(fac3,0),elem_(fac3,1),elem_(fac4,0),elem_(fac4,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);

  aii = ajj = diffus*surf*poros/dist_face(fac3,fac4,ori);
//  aii = ajj = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2))*nu_1()/dist_face(fac3,fac4,ori);
}

//// secmem_arete_interne
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_interne(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}

//// flux_arete_mixte
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_mixte(const DoubleTab& inco, int fac1,
                                                              int fac2, int fac3, int fac4) const
{
  double flux=0;
  //if (inco[fac4]*inco[fac3] != 0) {
  //il faudrait reflechir pour mieux traiter les arete mixte...

  // TODO : FIXME
  // This should be factorized
  if (DERIVED_T::IS_VAR)
    {
      if (inco[fac4]*inco[fac3] != 0)
        {
          double diffus=0;
          int element;

          if ((element=elem_(fac3,0)) != -1)
            diffus+=nu_3(element);
          if ((element=elem_(fac3,1)) != -1)
            diffus+=nu_3(element);
          if ((element=elem_(fac4,0)) != -1)
            diffus+=nu_3(element);
          if ((element=elem_(fac4,1)) != -1)
            diffus+=nu_3(element);

          diffus/=3.0;

          int ori=orientation(fac1);
          double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori);
          flux = 0.25*tau*(surface(fac1)+surface(fac2))*
                 diffus*(porosite(fac1)+porosite(fac2));
        }
    }
  else
    {
      if (inco[fac4]*inco[fac3] != 0)
        {
          int ori=orientation(fac1);
          double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori);
          flux = 0.5*tau*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2))*
                 nu_1();
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
  if (inconnue->valeurs()[fac4]*inconnue->valeurs()[fac3] != 0)
    {
      // TODO : FIXME
      // This should be factorized
      int ori=orientation(fac1);
      if (DERIVED_T::IS_VAR)
        {
          double diffus=0;
          int element;

          if ((element=elem_(fac3,0)) != -1)
            diffus+=nu_3(element);
          if ((element=elem_(fac3,1)) != -1)
            diffus+=nu_3(element);
          if ((element=elem_(fac4,0)) != -1)
            diffus+=nu_3(element);
          if ((element=elem_(fac4,1)) != -1)
            diffus+=nu_3(element);

          diffus/=3.0;

          aii = ajj= 0.25*(surface(fac1)+surface(fac2))*diffus*
                     (porosite(fac1)+porosite(fac2))/dist_face(fac3,fac4,ori);
        }
      else
        {
          aii = ajj= 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2))*nu_1()/dist_face(fac3,fac4,ori);
        }
    }
  else
    {
      aii=ajj=0;
    }
}

//// secmem_arete_mixte
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_mixte(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
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
  int k= orientation(fac3);

  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  if (elem1==-1)
    elem1 = elem2;
  else if (elem2==-1)
    elem2 = elem1;

  double diffus= nu_1(elem1,elem2); //0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));


  double dist = dist_norm_bord(fac1);
  double tau  = signe * (vit_imp - inco[fac3])/dist;
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  double surf = surface_(fac1,fac2);
  flux = tau*surf*diffus;
  return flux;
}

//// coeffs_arete_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4,
                                                              double& ajj1_2) const
{
  double dist = dist_norm_bord(fac1);
  double surf = surface_(fac1,fac2);
  double diffus= nu_1(elem_(fac3,0),elem_(fac3,1));
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  aii3_4 = signe*surf*diffus/dist;
  aii1_2 = 0;
  ajj1_2 = 0;
}


//// secmem_arete_paroi
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  double dist = dist_norm_bord(fac1);
  double tau  = signe * vit_imp/dist;
  double diffus= nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  flux = tau*surf*diffus;
  return flux;
}

//// flux_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_paroi_fluide(const DoubleTab& inco, int fac1,
                                                                   int fac2, int fac3, int signe,
                                                                   double& flux3, double& flux1_2) const
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
  double diffus= nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  //  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;

  // Calcul de flux1_2

  dist = dist_face(fac1,fac2,k);
  tau = (inco[fac2] - inco[fac1])/dist;
//  flux1_2 = tau*nu_1()*porosite(fac3)*surface(fac3);
  flux1_2 = tau*diffus*porosite(fac3)*surface(fac3);
}

//// coeffs_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                     double& aii1_2, double& aii3_4,
                                                                     double& ajj1_2) const
{
  double dist;
  int k= orientation(fac3);

  //Calcul des aii et ajj 3_4
  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  dist = dist_norm_bord(fac1);
  double diffus= nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);

//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  //  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  aii3_4 = signe*surf*diffus*poros/dist;
  // Calcul des aii et ajj 1_2

  dist = dist_face(fac1,fac2,k);
  aii1_2 = ajj1_2 = diffus*porosite(fac3)*surface(fac3)/dist;
}


//// secmem_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                     double& flux3, double& flux1_2) const
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

  double diffus= nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);

//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  //  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;
  flux1_2 = 0;
}

//// flux_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_periodicite(const DoubleTab& inco,
                                                                  int fac1, int fac2, int fac3, int fac4,
                                                                  double& flux3_4, double& flux1_2) const
{
  double flux, diffus,surf,poros;
  int ori;
  ori =orientation(fac1);

  diffus= nu_2(elem_(fac3,0),elem_(fac3,1),elem_(fac4,0),elem_(fac4,1));
  surf = surface_(fac1,fac2);
  poros = porosity_(fac1, fac2);

  flux = diffus*surf*poros*(inco[fac4]-inco[fac3])/dist_face_period(fac3,fac4,ori);

//  flux = 0.5 * (inco[fac4]-inco[fac3])*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2))*nu_1()/dist_face_period(fac3,fac4,ori);
  flux3_4 = flux;

  ori=orientation(fac3);
  diffus= nu_2(elem_(fac1,0),elem_(fac1,1),elem_(fac2,0),elem_(fac2,1));
  surf = surface_(fac3,fac4);
  poros = porosity_(fac3, fac4);
  flux = diffus*surf*poros*(inco[fac2]-inco[fac1])/dist_face_period(fac1,fac2,ori);

//  flux = 0.5 * (inco[fac2]-inco[fac1])*(surface(fac3)*porosite(fac3)+surface(fac4)*porosite(fac4))*nu_1()
//         /dist_face_period(fac1,fac2,ori) ;
  flux1_2 = flux;

}

//// coeffs_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                                    double& aii, double& ajj) const
{
  int ori=orientation(fac1);
  double diffus= nu_2(elem_(fac3,0),elem_(fac3,1),elem_(fac4,0),elem_(fac4,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1, fac2);
  aii = ajj =diffus*surf*poros /dist_face_period(fac3,fac4,ori);
//  aii = ajj = 0.5 * (surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2))*nu_1()/
//              dist_face_period(fac3,fac4,ori);
}


//// secmem_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                                    double& flux3_4, double& flux1_2) const
{
  /* Do nothing */
}

//// flux_arete_symetrie
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// coeffs_arete_symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie(int, int, int, int, double&, double&, double&) const
{
  /* Do nothing */
}

//// secmem_arete_symetrie
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie(int, int, int, int) const
{
  return 0;
}

//// flux_fa7_elem
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7_elem(const DoubleTab& inco, int element,
                                                           int fac1, int fac2) const
{
  double flux;
  int ori=orientation(fac1);
  double diffus = nu_3(element,0);
  flux = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) * diffus *
         (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);

// flux = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) * nu_1() *
//         (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);


  return flux;
}

//// coeffs_fa7_elem
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7_elem(int element,int fac1,
                                                           int fac2, double& aii, double& ajj) const
{
  int ori=orientation(fac1);
  double diffus = nu_3(element,0);
  aii = ajj = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) *
              diffus/dist_face(fac1,fac2,ori);

//  aii = ajj = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) * nu_1()/dist_face(fac1,fac2,ori);
}


//// secmem_fa7_elem
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_fa7_elem(int ,int fac1, int fac2) const
{
  return 0;
}

//// flux_fa7_sortie_libre
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7_sortie_libre(const DoubleTab&, int ,
                                                                   const Neumann_sortie_libre&,
                                                                   int ) const
{
  return 0;
}

//// coeffs_fa7_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                                   double& aii, double& ajj ) const
{
  aii = ajj = 0;
}

//// secmem_fa7_sortie_libre
//
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Face<DERIVED_T>::secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                                     int ) const
{
  return 0;
}

//// flux_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1,
                                                                      int fac2, int fac3, int signe,
                                                                      double& flux3, double& flux1_2) const
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
  double diffus = nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);

//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  //  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;

  // Calcul de flux1_2

  dist = dist_face(fac1,fac2,k);
  tau = (inco(fac2) - inco(fac1))/dist;
  flux1_2 =  DERIVED_T::IS_VAR ? 0.5*tau*diffus*porosite(fac3)*surface(fac3) :
             tau*diffus*porosite(fac3)*surface(fac3);
}

//// coeffs_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_fluide(int fac1, int fac2,
                                                                        int fac3, int signe,
                                                                        double& aii1_2, double& aii3_4,
                                                                        double& ajj1_2) const
{
  double dist;
  int k= orientation(fac3);
  // Calcul de aii3_4

  dist = dist_norm_bord(fac1);
  double diffus = nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  //  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  aii3_4 = (signe*surf*diffus*poros)/dist;

  // Calcul de aii1_2 et ajj1_2

  dist = dist_face(fac1,fac2,k);
  aii1_2 = ajj1_2  = DERIVED_T::IS_VAR ? (0.5*diffus*porosite(fac3)*surface(fac3))/dist :
                     (diffus*porosite(fac3)*surface(fac3))/dist;
}

//// secmem_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3,
                                                                        int signe,
                                                                        double& flux3, double& flux1_2) const
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
  double diffus = nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
  double poros = porosity_(fac1,fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  //  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;
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
  int k= orientation(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,k,la_zcl));


  double dist = dist_norm_bord(fac1);
  double tau  = signe * (vit_imp - inco[fac3])/dist;
  double diffus = nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));

  flux = tau*surf*diffus;
  return flux;
}

//// coeffs_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_paroi(int fac1, int fac2, int fac3,
                                                                       int signe, double& aii1_2,
                                                                       double& aii3_4, double& ajj1_2) const
{
  double dist = dist_norm_bord(fac1);
  double diffus = nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  aii3_4 = signe*surf*diffus/dist;
  ajj1_2 = 0;

  // XXX XXX XXX : what the hell ?
  if (DERIVED_T::IS_VAR)
    aii1_2 = 0;
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
  int k= orientation(fac3);

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  double dist = dist_norm_bord(fac1);
  double tau  = signe * vit_imp/dist;
  double diffus = nu_1(elem_(fac3,0),elem_(fac3,1));
  double surf = surface_(fac1,fac2);
//  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  flux = tau*surf*diffus;
  return flux;
}


/////////////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une inconnue vectorielle
// Elles ne sont pas codees pour l'instant!
////////////////////////////////////////////////////////////////////

//// flux_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_fluide(const DoubleTab& , int ,int ,
                                                             int , int ,
                                                             DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_fluide(int ,int ,
                                                               int , int , DoubleVect&, DoubleVect&, DoubleVect&) const
{
  /* Do nothing */
}

//// secmem_arete_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_fluide(int ,int ,int , int ,
                                                               DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_arete_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_interne(const DoubleTab& , int ,
                                                              int , int , int ,
                                                              DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_interne(int , int , int , int ,
                                                                DoubleVect&, DoubleVect& ) const
{
  /* Do nothing */
}

//// secmem_arete_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_interne(int ,
                                                                int , int , int ,
                                                                DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_arete_mixte
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_mixte(const DoubleTab& , int ,
                                                            int , int , int ,
                                                            DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_mixte
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_mixte(int , int , int , int ,
                                                              DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// secmem_arete_mixte
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_mixte(int, int , int , int ,
                                                              DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_arete_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_paroi(const DoubleTab& , int ,
                                                            int , int , int ,
                                                            DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi(int , int , int , int ,
                                                              DoubleVect&, DoubleVect&, DoubleVect&) const
{
  /* Do nothing */
}

//// secmem_arete_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi(int, int , int , int ,
                                                              DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_paroi_fluide(const DoubleTab& , int ,
                                                                   int , int , int ,
                                                                   DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}


//// coeffs_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_paroi_fluide(int, int , int , int ,
                                                                     DoubleVect&, DoubleVect&, DoubleVect&) const
{
  /* Do nothing */
}


//// secmem_arete_paroi_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_paroi_fluide(int, int , int , int ,
                                                                     DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_periodicite(const DoubleTab& , int ,
                                                                  int , int , int ,
                                                                  DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_periodicite(int , int , int , int ,
                                                                    DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// secmem_arete_periodicite
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_periodicite(int , int , int , int ,
                                                                    DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_arete_symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie(const DoubleTab&, int, int,
                                                               int, int, DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie(int, int, int, int, DoubleVect&, DoubleVect&, DoubleVect&) const
{
  /* Do nothing */
}

//// secmem_arete_symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie(int, int,
                                                                 int, int, DoubleVect& ) const
{
  /* Do nothing */
}

//// flux_fa7_elem
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7_elem(const DoubleTab& , int , int ,
                                                         int , DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_fa7_elem
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7_elem(int , int ,int , DoubleVect&, DoubleVect& ) const
{
  /* Do nothing */
}

//// secmem_fa7_elem
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_fa7_elem(int , int ,
                                                           int , DoubleVect& ) const
{
  /* Do nothing */
}

////flux_fa7_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_fa7_sortie_libre(const DoubleTab&, int ,
                                                                 const Neumann_sortie_libre&,
                                                                 int, DoubleVect&  ) const
{
  /* Do nothing */
}

////coeffs_fa7_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                                   DoubleVect&, DoubleVect& ) const
{
  /* Do nothing */
}

////secmem_fa7_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                                   int, DoubleVect&  ) const
{
  /* Do nothing */
}

//// flux_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_fluide(const DoubleTab& , int ,int ,
                                                                      int , int ,
                                                                      DoubleVect& , DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_fluide(int, int, int, int,
                                                                        DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  /* Do nothing */
}

//// secmen_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_fluide(int, int, int, int,
                                                                        DoubleVect&, DoubleVect&) const
{
  /* Do nothing */
}

//// flux_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::flux_arete_symetrie_paroi(const DoubleTab& , int ,
                                                                     int , int , int ,
                                                                     DoubleVect& ) const
{
  /* Do nothing */
}

//// coeffs_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_paroi(int , int , int , int ,
                                                                       DoubleVect&, DoubleVect&, DoubleVect&) const
{
  /* Do nothing */
}

//// secmem_arete_symetrie_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Face<DERIVED_T>::secmem_arete_symetrie_paroi(int, int , int , int ,
                                                                       DoubleVect& ) const
{
  /* Do nothing */
}

#endif /* Eval_Diff_VDF_Face_included */
