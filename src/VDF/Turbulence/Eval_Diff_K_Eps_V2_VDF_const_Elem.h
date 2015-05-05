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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_Diff_K_Eps_V2_VDF_const_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Eval_Diff_K_Eps_V2_VDF_const_Elem_included
#define Eval_Diff_K_Eps_V2_VDF_const_Elem_included

#include <Eval_Diff_K_Eps_Bas_Re_VDF_const.h>
#include <Champ_Fonc.h>
#include <Eval_VDF_Elem.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Eval_Diff_K_Eps_V2_VDF_const_Elem
//
//////////////////////////////////////////////////////////////////////////////

class Eval_Diff_K_Eps_V2_VDF_const_Elem : public Eval_Diff_K_Eps_Bas_Re_VDF_const, public Eval_VDF_Elem
{

public:

  inline Eval_Diff_K_Eps_V2_VDF_const_Elem();

  inline int calculer_flux_faces_echange_externe_impose() const ;
  inline int calculer_flux_faces_echange_global_impose() const ;
  inline int calculer_flux_faces_entree_fluide() const ;
  inline int calculer_flux_faces_paroi() const ;
  inline int calculer_flux_faces_paroi_adiabatique() const ;
  inline int calculer_flux_faces_paroi_defilante() const ;
  inline int calculer_flux_faces_paroi_fixe() const ;
  inline int calculer_flux_faces_sortie_libre() const ;
  inline int calculer_flux_faces_symetrie() const ;
  inline int calculer_flux_faces_periodique() const ;
  inline int calculer_flux_faces_NSCBC() const ;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const;
  inline double flux_face(const DoubleTab&, int , int , int, const Echange_externe_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Echange_global_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Symetrie&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Periodique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const NSCBC&, int ) const;
  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux


  inline void flux_face(const DoubleTab&, int , const Symetrie&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Periodique&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Echange_global_impose&,
                        int, DoubleVect& flux) const;
  //inline void flux_face(const DoubleTab&, int ,const Nouvelle_Cl_VDF&,
  //                      int, DoubleVect& flux) const=0;

  inline void flux_faces_interne(const DoubleTab&, int ,
                                 DoubleVect& flux) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_face(int,int, const Symetrie&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_fixe&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi_adiabatique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const NSCBC&, double& aii, double& ajj ) const;
  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite pour les grandeurs scalaires.

  inline double secmem_face(int, const Symetrie&, int ) const;
  inline double secmem_face(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int ) const;
  inline double secmem_face(int, const Dirichlet_paroi_fixe&, int ) const;
  inline double secmem_face(int, const Dirichlet_paroi_defilante&, int ) const;
  inline double secmem_face(int, const Neumann_paroi_adiabatique&, int ) const;
  inline double secmem_face(int, const Neumann_paroi&, int ) const;
  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const;
  inline double secmem_face(int, const Echange_global_impose&, int ) const;
  inline double secmem_face(int, const Periodique&, int ) const;
  inline double secmem_face(int, const NSCBC&, int ) const;
  inline double secmem_faces_interne(int ) const;


  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_face(int,int, const Symetrie&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int, int,const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi_adiabatique&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const;

  //virtual void coeffs_face(const DoubleTab&, int , const Nouvelle_Cl_VDF&, int,
  //                           DoubleVect& aii, DoubleVect& ajj ) const;

  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite pour les grandeurs vectorielles.

  inline void secmem_face(int, const Symetrie&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi_adiabatique&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi&, int, DoubleVect& ) const;
  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Echange_global_impose&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Periodique&, int, DoubleVect& ) const;
  inline void secmem_faces_interne(int, DoubleVect& ) const;

  //inline double secmem_face(const DoubleTab&, int , const Nouvelle_Cl_VDF&, int, DoubleVect& ) const;

private:
  REF(Champ_Inc) KEps;
};



//
// Fonctions inline de la classe Eval_Diff_K_Eps_V2_VDF_const_Elem
//

inline Eval_Diff_K_Eps_V2_VDF_const_Elem::Eval_Diff_K_Eps_V2_VDF_const_Elem()
  : Eval_Diff_K_Eps_Bas_Re_VDF_const() {}

//// calculer_flux_faces_echange_externe_impose
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_echange_externe_impose() const
{
  return 0;
}


//// calculer_flux_faces_echange_global_impose
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_echange_global_impose() const
{
  return 0;
}


//// calculer_flux_faces_entree_fluide
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_entree_fluide() const
{
  return 1;
}


//// calculer_flux_faces_paroi
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_paroi() const
{
  return 0;
}


//// calculer_flux_faces_paroi_adiabatique
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_paroi_adiabatique() const
{
  return 0;
}


//// calculer_flux_faces_paroi_defilante
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_paroi_defilante() const
{
  return 0;
}


//// calculer_flux_faces_paroi_fixe
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_paroi_fixe() const
{
  return 1;
}


//// calculer_flux_faces_sortie_libre
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_sortie_libre() const
{
  return 1;
}


//// calculer_flux_faces_symetrie
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_symetrie() const
{
  return 1;
}

//// calculer_flux_faces_periodique
//

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_periodique() const
{
  return 0;
}

////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une grandeur scalaire
////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                           const Dirichlet_entree_fluide& la_cl,
                                                           int num1) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,
                                                           const Dirichlet_entree_fluide& la_cl,
                                                           double& aii, double& ajj) const
{
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                             int num1) const
{
  return 0;
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                           const Dirichlet_paroi_defilante&,
                                                           int ) const
{
  return 0;
}


//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Dirichlet_paroi_defilante&,
                                                           double&, double& ) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                             int ) const
{
  return 0;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                           const Dirichlet_paroi_fixe&,
                                                           int ) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,
                                                           const Dirichlet_paroi_fixe& la_cl,
                                                           double& aii, double& ajj) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Dirichlet_paroi_fixe&,
                                                             int num1) const
{
  return 0;
}

//// flux_face avec Echange_externe_impose
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                           const Echange_externe_impose& la_cl,
                                                           int num1) const
{
  return 0;
}

//// coeffs_face avec Echange_externe_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                           const Echange_externe_impose& la_cl,
                                                           double& aii, double& ajj) const
{
}

//// secmem_face avec Echange_externe_impose
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                             int num1) const
{
  return 0;
}

//// flux_face avec Echange_global_impose
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                           const Echange_global_impose& la_cl,
                                                           int num1) const
{
  return 0;
}


//// coeffs_face avec Echange_global_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,
                                                           const Echange_global_impose& la_cl,
                                                           double& aii, double& ajj) const
{

}

//// secmem_face avec Echange_global_impose
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Echange_global_impose& la_cl,
                                                             int num1) const
{
  return 0;
}

//// flux_face avec Neumann_paroi
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& , int face,
                                                           const Neumann_paroi& la_cl,
                                                           int num1) const
{
  return 0;
}

//// coeffs_face avec Neumann_paroi
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,
                                                           const Neumann_paroi& la_cl,
                                                           double& aii, double& ajj) const
{
  ;
}

//// secmem_face avec Neumann_paroi
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Neumann_paroi& la_cl,
                                                             int num1) const
{
  return 0;
}

//// flux_face avec Neumann_paroi_adiabatique
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                           const Neumann_paroi_adiabatique&,
                                                           int ) const
{
  return 0;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Neumann_paroi_adiabatique&,
                                                           double&, double&) const
{
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                             int ) const
{
  return 0;
}

//// flux_face avec Neumann_sortie_libre
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& , int ,
                                                           const Neumann_sortie_libre& ,
                                                           int ) const
{
  return 0;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Neumann_sortie_libre& ,
                                                           double&, double&) const
{
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int, const Neumann_sortie_libre& ,
                                                             int ) const
{
  return 0;
}

//// flux_face avec Symetrie
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                           const Symetrie&, int ) const
{
  return 0;
}

//// coeffs_face avec Symetrie
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Symetrie&, double&, double& ) const
{
  ;
}

//// secmem_face avec Symetrie
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int, const Symetrie&, int ) const
{
  return 0;
}

//// flux_face avec Periodique
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco,
                                                           int face,
                                                           const Periodique& la_cl,
                                                           int ) const
{
  return 0;
}

//// coeffs_face avec Periodique

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,
                                                           const Periodique& la_cl,
                                                           double& aii, double& ajj) const
{
  ;
}

//// secmem_face avec Periodique
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face,
                                                             const Periodique& la_cl,
                                                             int ) const
{
  return 0;
}

//// flux_faces_interne
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_faces_interne(const DoubleTab& inco, int face) const
{
  return 0;
}


//// coeffs_faces_interne

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_faces_interne(int face,double& aii, double& ajj) const
{
  ;
}


//// secmem_faces_interne
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_faces_interne( int ) const
{
  return 0;
}

////////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une grandeur vectorielle
///////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                         const Dirichlet_entree_fluide& la_cl,
                                                         int num1,DoubleVect& flux) const
{
  // Cerr << " coucou dans Dirichlet_entree_fluide Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face " << finl;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  //int k;
  double dist = dist_norm_bord(face);
  if (n0 != -1)
    {
      flux(0) = (la_cl.val_imp(face-num1,0)-inco(n0,0))
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(n0)/Prdt_K)/dist;
      flux(1) = (la_cl.val_imp(face-num1,1)-inco(n0,1))
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(n0)/Prdt_Eps)/dist;
    }
  else  // n1 != -1
    {
      flux(0) = (inco(n1,0)-la_cl.val_imp(face-num1,0))
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(n1)/Prdt_K)/dist;
      flux(1) = (inco(n1,1)-la_cl.val_imp(face-num1,1))
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(n1)/Prdt_Eps)/dist;
    }
}


//// coeffs_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,const Dirichlet_entree_fluide& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj) const
{
  //int k;
  int i = elem_(face,0);
  int j = elem_(face,1);
  //double dist = dist_norm_bord(face);

  if (i != -1)
    {
      aii(0) = surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_K);
      aii(1) = surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_Eps);
      ajj(0) =  ajj(1) = 0;
    }
  else  // j != -1
    {

      ajj(0) = surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_K);
      ajj(1) = surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_Eps);
      aii(0) =  aii(1) = 0;
    }
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                           int num1,DoubleVect& flux) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  //int k;
  double dist = dist_norm_bord(face);

  if (i != -1)
    {

      flux(0) = la_cl.val_imp(face-num1,0)
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_K)/dist;
      flux(1) = la_cl.val_imp(face-num1,1)
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_Eps)/dist;
      /*   for (k=0; k<flux.size(); k++) */
      /*       flux(k) = la_cl.val_imp(face-num1,k) */
      /*         *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_K)/dist; */
    }
  else // j != -1
    {

      flux(0) = -la_cl.val_imp(face-num1,0)
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_K)/dist;
      flux(1) = -la_cl.val_imp(face-num1,1)
                *surface(face)*porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_Eps)/dist;
      /* for (k=0; k<flux.size(); k++)  */
      /*       flux(k) = -la_cl.val_imp(face-num1,k) */
      /*         *surface(face)*(porosite(face)*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_K))/dist; */
    }
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                         const Dirichlet_paroi_defilante&,
                                                         int, DoubleVect& ) const
{
  ;
}


//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Dirichlet_paroi_defilante&,
                                                           DoubleVect&, DoubleVect& ) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                           int, DoubleVect& ) const
{
  ;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face ,
                                                         const Dirichlet_paroi_fixe& la_cl,
                                                         int num1, DoubleVect& flux) const
{

  // Cerr << " coucou dans Dirichlet_paroi_fixe Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face " << finl;

  assert(dv_diffusivite_turbulente.ref_count() >=2);
  assert(diffusivite_turbulente_->valeurs().addr() == dv_diffusivite_turbulente.addr() );
  // Cerr << " DANS EVAL_DIFF POUR V2 " << finl;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  /*   Cerr << "n0 " << n0 << finl; */
  /*   Cerr << "n1 " << n1 << finl; */
  //const DoubleVect& porosite_surf = la_zone->porosite_face();
  //const DoubleVect& volume_entrelaces = la_zone->volumes_entrelaces();
  double dist = dist_norm_bord(face);
  /*   Cerr << "dist " << dist << finl; */
  /*   Cerr << "db_diffusivite = " << db_diffusivite << finl; */

  double temp;
  double coef = surface(face)*porosite(face)/dist;

  if (n0 != -1)
    {
      // k=0 a la paroi
      //  Cerr << "dv_diffusivite_turbulente(n0)  = " << dv_diffusivite_turbulente(n0)<< finl;
      flux(0) = -inco(n0,0)*coef*(db_diffusivite+dv_diffusivite_turbulente(n0)/Prdt_K);
      //Cerr << " pour elem " << n0 << " le flux de k = " << flux(0) << finl;
      // epsilon = 2*db_diffusivite*k/dist^2
      temp = 2*db_diffusivite*inco(n0,0)/(dist*dist);
      flux(1) = (temp-inco(n0,1))*coef*(db_diffusivite+dv_diffusivite_turbulente(n0)/Prdt_Eps);
      // Cerr << " pour elem " << n0 << " le flux de eps = " << flux(1) << finl;
    }
  else  // n1 != -1
    {
      // k=0 a la paroi
      // Cerr << "dv_diffusivite_turbulente(n1)  = " << dv_diffusivite_turbulente(n1)<< finl;
      flux(0) = inco(n1,0)*coef*(db_diffusivite+dv_diffusivite_turbulente(n1)/Prdt_K);
      // Cerr << " pour elem " << n1 << " le flux de k = " << flux(0) << finl;
      // epsilon = 2*db_diffusivite*k/dist^2
      temp = 2*db_diffusivite*inco(n1,0)/(dist*dist);
      //Cerr << " valeur de epsilon a la paroi " << temp << finl;
      flux(1) = (inco(n1,1)-temp)*coef*(db_diffusivite+dv_diffusivite_turbulente(n1)/Prdt_Eps);
      //Cerr << " pour elem " << n1 << " le flux de eps = " << flux(1) << finl;
    }
}


//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face,int num1, const Dirichlet_paroi_fixe& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj) const
{
  assert(dv_diffusivite_turbulente.ref_count() >=2);
  assert(diffusivite_turbulente_->valeurs().addr() == dv_diffusivite_turbulente.addr() );
  int i = elem_(face,0);
  int j = elem_(face,1);
  //const DoubleVect& porosite_surf = la_zone->porosite_face();
  //const DoubleVect& volume_entrelaces = la_zone->volumes_entrelaces();
  double dist = dist_norm_bord(face);
  double coef = surface(face)*porosite(face)/dist;

  if (i != -1)
    {
      aii(0) = coef*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_K);
      aii(1) = coef*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_Eps);
      ajj(0) =  ajj(1) = 0;
    }
  else
    {
      // j != -1

      ajj(0) = coef*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_K);
      ajj(1) = coef*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_Eps);
      aii(0) =  aii(1) = 0;
    }
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Dirichlet_paroi_fixe& la_cl,
                                                           int num1, DoubleVect& flux) const
{
  assert(dv_diffusivite_turbulente.ref_count() >=2);
  assert(diffusivite_turbulente_->valeurs().addr() == dv_diffusivite_turbulente.addr() );

  //Cerr << "dans Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem::secmem_face " << finl;

  int i = elem_(face,0);
  int j = elem_(face,1);
  //const DoubleVect& porosite_surf = la_zone->porosite_face();
  //const DoubleVect& volume_entrelaces = la_zone->volumes_entrelaces();
  double dist = dist_norm_bord(face);
  double coef = surface(face)*porosite(face)/dist;
  double temp;

  if (i != -1)
    {
      flux(0) = 0.;
      temp = 2*db_diffusivite*KEps->valeurs()(i,0)/(dist*dist);
      flux(1) = temp*coef*(db_diffusivite+dv_diffusivite_turbulente(i)/Prdt_Eps);
    }
  else   // j != -1
    {
      flux(0) = 0.;

      temp = 2*db_diffusivite*KEps->valeurs()(j,0)/(dist*dist);
      flux(1) = -temp*coef*(db_diffusivite+dv_diffusivite_turbulente(j)/Prdt_Eps);
    }
}

//// flux_face avec Echange_externe_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                         const Echange_externe_impose& la_cl,
                                                         int num1,DoubleVect& flux) const
{
}

//// coeffs_face avec Echange_externe_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                           const Echange_externe_impose& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj) const
{
}

//// secmem

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                           int num1,DoubleVect& flux) const
{
}

//// flux_face avec Echange_global_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                         const Echange_global_impose& la_cl,
                                                         int num1,DoubleVect& flux) const
{
}

//// coeffs_face avec Echange_global_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,
                                                           const Echange_global_impose& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj ) const
{
}

//// secmem_face avec Echange_global_impose
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Echange_global_impose& la_cl,
                                                           int num1,DoubleVect& flux) const
{
}

//// flux_face avec Neumann_paroi
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& , int face,
                                                         const Neumann_paroi& la_cl,
                                                         int num1,DoubleVect& flux) const
{
  ;
}

//// coeffs_face avec Neumann_paroi
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Neumann_paroi& ,
                                                           DoubleVect& , DoubleVect& ) const
{
  ;
}

//// secmem_face avec Neumann_paroi
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Neumann_paroi& la_cl,
                                                           int num1, DoubleVect& flux) const
{
  ;
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                         const Neumann_paroi_adiabatique&,
                                                         int, DoubleVect& ) const
{
  ;
}


//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int , int,
                                                           const Neumann_paroi_adiabatique&,
                                                           DoubleVect&, DoubleVect& ) const
{
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                           int, DoubleVect& ) const
{
  ;
}

//// flux_face avec Neumann_sortie_libre
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                         const Neumann_sortie_libre& la_cl,
                                                         int num1, DoubleVect& flux ) const
{
  // Cerr << "coucou dans Neumann_sortie_libre Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face" << finl;
  flux = 0 ;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int num1,const Neumann_sortie_libre& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj) const
{
  aii=ajj=0;
}

//// secmem_face avec Neumann_sortie_libre
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Neumann_sortie_libre& la_cl,
                                                           int num1 , DoubleVect& flux) const
{
  flux = 0 ;
}

//// flux_face avec Symetrie
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                         const Symetrie& la_cl,
                                                         int num1, DoubleVect& flux) const
{
  //  Cerr << "coucou dans symetrie Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face" << finl;
  flux = 0;
}

//// coeffs_face avec Symetrie
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face,int num1, const Symetrie& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj) const
{
  aii=ajj=0;
}

//// secmem_face avec Symetrie
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Symetrie& la_cl,
                                                           int num1, DoubleVect& flux) const
{
  flux = 0;
}

//// flux_face avec Periodique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                         const Periodique& la_cl,
                                                         int, DoubleVect& flux) const
{

}

//// coeffs_face avec Periodique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face,int, const Periodique& la_cl,
                                                           DoubleVect& aii, DoubleVect& ajj ) const
{
}


//// secmem_face avec Periodique
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int face, const Periodique& la_cl,
                                                           int, DoubleVect& flux) const
{
  ;
}

//// flux_faces_interne
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_faces_interne(const DoubleTab& inco,
                                                                  int face,DoubleVect& flux) const
{

  // Cerr << "Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_faces_interne" << finl;
  assert(dv_diffusivite_turbulente.ref_count() >=2);
  assert(diffusivite_turbulente_->valeurs().addr() == dv_diffusivite_turbulente.addr() );
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  //const DoubleVect& porosite_surf = la_zone->porosite_face();
  //const DoubleVect& volume_entrelaces = la_zone->volumes_entrelaces();
  //const DoubleVect& volumes = la_zone->volumes();
  //const DoubleVect& porosite_vol = la_zone->porosite_elem();
  double dist = la_zone->dist_norm(face);
  double coef = surface(face)*porosite(face)/dist;
  double diffu = 0.5*(dv_diffusivite_turbulente(n0)+dv_diffusivite_turbulente(n1));
  flux(0) = (db_diffusivite+diffu/Prdt_K)*coef*(inco(n1,0) - inco(n0,0));
  //  flux(0) = (inco(n1,0) - inco(n0,0))*coef;
  //        Cerr << "1.visco = " << flux(0) << " " << volume_entrelaces(face)*porosite_surf(face)/dist/dist << " " << surface(face)*porosite(face)/dist << " " << volumes(n0)*porosite_vol(n0) << " " << dv_diffusivite_turbulente(n1) << " " << coef << " " << dist << " " << inco(n1,0) << " " <<  inco(n0,0) << " " << n0 << " " << n1 << finl;
  flux(1) = (db_diffusivite+diffu/Prdt_Eps)*coef*(inco(n1,1) - inco(n0,1));
  //Cerr << "1.visco = " << db_diffusivite << finl;
  //Cerr << "1.visco turb = " <<  dv_diffusivite_turbulente(n0) << finl;

}


//// coeffs_faces_interne
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  assert(dv_diffusivite_turbulente.ref_count() >=2);
  assert(diffusivite_turbulente_->valeurs().addr() == dv_diffusivite_turbulente.addr() );
  // Cerr << "Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_faces_interne" << finl;
  int i = elem_(face,0);
  int j = elem_(face,1);
  //const DoubleVect& porosite_surf = la_zone->porosite_face();
  //const DoubleVect& volume_entrelaces = la_zone->volumes_entrelaces();
  double dist = la_zone->dist_norm(face);
  double coef = surface(face)*porosite(face)/dist;
  //  double coef = volume_entrelaces(face)*porosite_surf(face)/dist/dist;
  double diffu = 0.5*(dv_diffusivite_turbulente(i)+dv_diffusivite_turbulente(j));
  aii(0) = ajj(0) = (db_diffusivite+diffu/Prdt_K)*coef;
  aii(1) = ajj(1) = (db_diffusivite+diffu/Prdt_Eps)*coef;
  //Cerr << " aii(0) = ajj(0) = " << aii(0) << " aii(1) = ajj(1) = " << aii(1) << finl;
  //Cerr << "2.visco = " << db_diffusivite << finl;
  //        Cerr << "2.visco turb = " <<  dv_diffusivite_turbulente(i) << " " <<   dv_diffusivite_turbulente(j) << finl;

}


//// secmem_faces_interne
//

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_faces_interne( int face, DoubleVect& flux ) const
{
  assert(dv_diffusivite_turbulente.ref_count() >=2);
  assert(diffusivite_turbulente_->valeurs().addr() == dv_diffusivite_turbulente.addr() );
  flux(0) = 0;
  flux(1) = 0;
}

inline int Eval_Diff_K_Eps_V2_VDF_const_Elem::calculer_flux_faces_NSCBC() const
{
  return 0;
}

//// flux_face avec NSCBC
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                           const NSCBC& la_cl, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();


  return 0;
}

//// coeffs_face avec NSCBC

inline void Eval_Diff_K_Eps_V2_VDF_const_Elem::coeffs_face(int face, int,
                                                           const NSCBC& la_cl,
                                                           double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec NSCBC
//

inline double Eval_Diff_K_Eps_V2_VDF_const_Elem::secmem_face(int,
                                                             const NSCBC&,
                                                             int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}


#endif
