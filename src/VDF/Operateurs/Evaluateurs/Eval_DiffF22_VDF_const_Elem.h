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
// File:        Eval_DiffF22_VDF_const_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_DiffF22_VDF_const_Elem_included
#define Eval_DiffF22_VDF_const_Elem_included

#include <Eval_Diff_VDF_const.h>
#include <Champ_Inc.h>
#include <Eval_VDF_Elem.h>


//
// .DESCRIPTION class Eval_DiffF22_VDF_const_Elem
//
// Evaluateur VDF pour la diffusion dans l'equation de F22 (modele V2F)
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.

//
// .SECTION voir aussi Eval_DiffF22_VDF_const


class Eval_DiffF22_VDF_const_Elem : public Eval_Diff_VDF_const, public Eval_VDF_Elem
{

public:

  inline void associer_keps(const Champ_Inc& keps, const Champ_Inc& champv2)
  {
    KEps = keps;
    v2 = champv2;
  }

  inline Eval_DiffF22_VDF_const_Elem();

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
  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const;
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
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const;
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


protected:

private:
  REF(Champ_Inc) KEps;
  REF(Champ_Inc) v2;
};

//
// Fonctions inline de la classe Eval_DiffF22_VDF_const_Elem
//

inline Eval_DiffF22_VDF_const_Elem::Eval_DiffF22_VDF_const_Elem() {}


//// calculer_flux_faces_echange_externe_impose
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_echange_externe_impose() const
{
  return 0;
}


//// calculer_flux_faces_echange_global_impose
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_echange_global_impose() const
{
  return 0;
}


//// calculer_flux_faces_entree_fluide
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_entree_fluide() const
{
  return 1;
}


//// calculer_flux_faces_paroi
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_paroi() const
{
  return 0;
}


//// calculer_flux_faces_paroi_adiabatique
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_paroi_adiabatique() const
{
  return 0;
}


//// calculer_flux_faces_paroi_defilante
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_paroi_defilante() const
{
  return 0;
}


//// calculer_flux_faces_paroi_fixe
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_paroi_fixe() const
{
  return 1 ;
}


//// calculer_flux_faces_sortie_libre
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_sortie_libre() const
{
  return 1;
}


//// calculer_flux_faces_symetrie
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_symetrie() const
{
  return 1;
}

//// calculer_flux_faces_periodique
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_periodique() const
{
  return 0;
}


//// calculer_flux_faces_NSCBC
//

inline int Eval_DiffF22_VDF_const_Elem::calculer_flux_faces_NSCBC() const
{
  return 0;
}

////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une grandeur scalaire
////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                     const Dirichlet_entree_fluide& la_cl,
                                                     int num1) const
{
  Cerr << " si je suis ici en flux, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Dirichlet_entree_fluide
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Dirichlet_entree_fluide& la_cl,
                                                     double& aii, double& ajj) const
{
  aii = ajj = 0.;
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                       int num1) const
{
  return 0;
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                     const Dirichlet_paroi_defilante&,
                                                     int ) const
{
  Cerr << " si je suis ici en flux, ce n'est pas bon " << finl ;
  return 0;
}


//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int , int,
                                                     const Dirichlet_paroi_defilante&,
                                                     double&, double& ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return   ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                       int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                     const Dirichlet_paroi_fixe&,
                                                     int ) const
{
  Cerr << " si je suis ici en flux face, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Dirichlet_paroi_fixe& la_cl,
                                                     double& aii, double& ajj) const
{
  //Cerr << "Dirichlet_paroi_fixe coeffs_face  " << finl;
  int i = elem_(face,0);
  //int j = elem(face,1);
  double dist = dist_norm_bord(face);
  //double temp1,  L,k,eps;

  if (i != -1)
    {
      aii = porosite(face)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj = porosite(face)*surface(face)/dist;
      aii = 0;
    }

}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Dirichlet_paroi_fixe& la_cl,
                                                       int num1) const
{
  // Cerr << "Dirichlet_paroi_fixe secmem_face " << finl;

  /* double cl = -20*db_diffusivite*db_diffusivite; */
  /*   int i = elem(face,0);   */
  /*   int j = elem(face,1); */
  /*   double dist = dist_norm_bord(face); */
  /*   double d4 = dist*dist; */
  /*   double temp1,  L,k,eps; */
  /*   d4*=d4;  */
  /*   double flux; */
  /*   if (i != -1) { */
  /*     // le epsilon a la paroi */
  /*     // eps = (2*db_diffusivite*KEps->valeurs()(i,0))/d4; */
  /*     // cl = (cl*v2->valeurs()(i))/(eps*d4); */
  /*     cl =  (cl*v2->valeurs()(i))/(KEps->valeurs()(i,1)*d4); */
  /*     flux = cl*surface(face)*porosite(face)/dist; */
  /*     //        flux = cl*L*surface(face)*porosite(face)/dist; */
  /*   } */
  /*   else { // j != -1  */
  /*     // eps = (2*db_diffusivite*KEps->valeurs()(j,0))/d4; */
  /*     // cl = (cl*v2->valeurs()(j))/(eps*d4); */
  /*     cl =  (cl*v2->valeurs()(j))/(KEps->valeurs()(j,1)*d4);         */
  /*     flux = -cl*surface(face)*porosite(face)/dist; */
  /*     //        flux = -cl*L*surface(face)*porosite(face)/dist; */
  /*   }         */
  /*   return flux; */

  return 0;
}

//// flux_face avec Echange_externe_impose
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                     const Echange_externe_impose& la_cl,
                                                     int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Echange_externe_impose
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                     const Echange_externe_impose& la_cl,
                                                     double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Echange_externe_impose
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                       int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// flux_face avec Echange_global_impose
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                     const Echange_global_impose& la_cl,
                                                     int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}


//// coeffs_face avec Echange_global_impose
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Echange_global_impose& la_cl,
                                                     double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Echange_global_impose
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Echange_global_impose& la_cl,
                                                       int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// flux_face avec Neumann_paroi
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& , int face,
                                                     const Neumann_paroi& la_cl,
                                                     int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Neumann_paroi
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Neumann_paroi& la_cl,
                                                     double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Neumann_paroi
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Neumann_paroi& la_cl,
                                                       int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// flux_face avec Neumann_paroi_adiabatique
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                     const Neumann_paroi_adiabatique&,
                                                     int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int , int,
                                                     const Neumann_paroi_adiabatique&,
                                                     double&, double&) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                       int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;

  return 0;
}

//// flux_face avec Neumann_sortie_libre
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& , int ,
                                                     const Neumann_sortie_libre& ,
                                                     int ) const
{
  Cerr << " si je suis ici en flux 2, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Neumann_sortie_libre& la_cl,
                                                     double& aii, double& ajj) const
{
  // Cerr << " dans sortie libre " << finl;
  aii = ajj = 0.;
}

//// secmem_face avec Neumann_sortie_libre
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face , const Neumann_sortie_libre& la_cl,
                                                       int num1 ) const
{
  return 0;
}

//// flux_face avec Symetrie
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                     const Symetrie&, int ) const
{
  Cerr << " si je suis ici en flux 3, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Symetrie
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face , int num1,
                                                     const Symetrie& la_cl, double& aii, double& ajj) const
{
  // Cerr << " dans eval f22 coeff_faces de symetrie " << finl;
  aii = ajj = 0;
}

//// secmem_face avec Symetrie
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Symetrie& la_cl, int num1) const
{
  // Cerr << " dans eval f22 secmem_face avec Symetrie " << finl;
  return 0;
}

//// flux_face avec Periodique
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco,
                                                     int face,
                                                     const Periodique& la_cl,
                                                     int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Periodique

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Periodique& la_cl,
                                                     double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Periodique
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int face,
                                                       const Periodique& la_cl,
                                                       int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}


//// flux_face avec NSCBC
//

inline double Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& , int ,
                                                     const NSCBC& ,
                                                     int ) const
{
  return 0;
}

//// coeffs_face avec NSCBC
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int , int,
                                                     const NSCBC& ,
                                                     double&, double&) const
{
  ;
}

//// secmem_face avec NSCBC
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_face(int, const NSCBC& ,
                                                       int ) const
{
  return 0;
}


//// flux_faces_interne
//

inline double Eval_DiffF22_VDF_const_Elem::flux_faces_interne(const DoubleTab& inco, int face) const
{
  /* double flux; */
  /*   int n0 = elem(face,0); */
  /*   int n1 = elem(face,1); */
  /*   double dist = la_zone->dist_norm(face); */
  /*   flux = (inco[n1] - inco[n0])*surface(face)/dist; */
  /*   flux *= porosite(face)*db_diffusivite; */
  /*   return flux; */
  Cerr << " dans flux_faces_interne " << finl;
  return 0.;
}


//// coeffs_faces_interne

inline void Eval_DiffF22_VDF_const_Elem::coeffs_faces_interne(int face,double& aii, double& ajj) const
{
  // Cerr << "dans coeffs_faces_interne " << finl;
  //int n0 = elem(face,0);
  //int n1 = elem(face,1);
  double dist = la_zone->dist_norm(face);
  //double temp,  L1,L2,k,eps;

  aii = ajj =  porosite(face)*surface(face)/dist;
}


//// secmem_faces_interne
//

inline double Eval_DiffF22_VDF_const_Elem::secmem_faces_interne( int ) const
{
  return 0;
}

////////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une grandeur vectorielle
///////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                   const Dirichlet_entree_fluide& la_cl,
                                                   int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}


//// coeffs_face avec Dirichlet_entree_fluide
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,const Dirichlet_entree_fluide& la_cl,
                                                     DoubleVect& aii, DoubleVect& ajj) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                     int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                   const Dirichlet_paroi_defilante&,
                                                   int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}


//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int , int,
                                                     const Dirichlet_paroi_defilante&,
                                                     DoubleVect&, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                     int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                   const Dirichlet_paroi_fixe&,
                                                   int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}


//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int,int, const Dirichlet_paroi_fixe&,
                                                     DoubleVect&, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Dirichlet_paroi_fixe&,
                                                     int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// flux_face avec Echange_externe_impose
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                   const Echange_externe_impose& la_cl,
                                                   int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// coeffs_face avec Echange_externe_impose
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                     const Echange_externe_impose& la_cl,
                                                     DoubleVect& aii, DoubleVect& ajj) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// secmem

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                     int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// flux_face avec Echange_global_impose
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                   const Echange_global_impose& la_cl,
                                                   int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// coeffs_face avec Echange_global_impose
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face, int num1,
                                                     const Echange_global_impose& la_cl,
                                                     DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Echange_global_impose
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Echange_global_impose& la_cl,
                                                     int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// flux_face avec Neumann_paroi
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& , int face,
                                                   const Neumann_paroi& la_cl,
                                                   int num1,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  // for (int k=0; k<flux.size(); k++)
  // flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}

//// coeffs_face avec Neumann_paroi
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int , int,
                                                     const Neumann_paroi& ,
                                                     DoubleVect& , DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// secmem_face avec Neumann_paroi
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Neumann_paroi& la_cl,
                                                     int num1, DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  // for (int k=0; k<flux.size(); k++)
  // flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                   const Neumann_paroi_adiabatique&,
                                                   int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}


//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int , int,
                                                     const Neumann_paroi_adiabatique&,
                                                     DoubleVect&, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                     int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// flux_face avec Neumann_sortie_libre
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& , int ,
                                                   const Neumann_sortie_libre& ,
                                                   int , DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int, int,const Neumann_sortie_libre& ,
                                                     DoubleVect&, DoubleVect&) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Neumann_sortie_libre& ,
                                                     int , DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// flux_face avec Symetrie
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab&, int ,
                                                   const Symetrie&,
                                                   int, DoubleVect&) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// coeffs_face avec Symetrie
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int,int, const Symetrie&,
                                                     DoubleVect&, DoubleVect&) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// secmem_face avec Symetrie
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int, const Symetrie&,
                                                     int, DoubleVect&) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// flux_face avec Periodique
//

inline void Eval_DiffF22_VDF_const_Elem::flux_face(const DoubleTab& inco, int face,
                                                   const Periodique& la_cl,
                                                   int, DoubleVect& flux) const
{

  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}

//// coeffs_face avec Periodique
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_face(int face,int, const Periodique& la_cl,
                                                     DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}


//// secmem_face avec Periodique
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_face(int face, const Periodique& la_cl,
                                                     int, DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

//// flux_faces_interne
//

inline void Eval_DiffF22_VDF_const_Elem::flux_faces_interne(const DoubleTab& inco,
                                                            int face,DoubleVect& flux) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}


//// coeffs_faces_interne
//

inline void Eval_DiffF22_VDF_const_Elem::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
}


//// secmem_faces_interne
//

inline void Eval_DiffF22_VDF_const_Elem::secmem_faces_interne( int, DoubleVect& ) const
{
  Cerr << " si je suis ici dans le vector, ce n'est pas bon " << finl ;
  return ;
  ;
}

#endif
