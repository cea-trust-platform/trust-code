/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Eval_Div_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Div_VDF_Elem_included
#define Eval_Div_VDF_Elem_included

#include <Eval_Div_VDF.h>
#include <Eval_VDF_Elem.h>
#include <Zone_VDF.h>

//
// .DESCRIPTION class Eval_Div_VDF_Elem
//
// Evaluateur VDF pour la divergence

//
// .SECTION voir aussi Eval_Div_VDF



class Eval_Div_VDF_Elem : public Eval_Div_VDF, public Eval_VDF_Elem
{

public:

  inline Eval_Div_VDF_Elem();

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


};

//
// Fonctions inline de la classe Eval_Div_VDF_Elem
//

//// Eval_Div_VDF_Elem
//

inline Eval_Div_VDF_Elem::Eval_Div_VDF_Elem()
{
}


//// calculer_flux_faces_echange_externe_impose
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_echange_externe_impose() const
{
  return 1;
}


//// calculer_flux_faces_echange_global_impose
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_echange_global_impose() const
{
  return 1;
}



//// calculer_flux_faces_entree_fluide
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_entree_fluide() const
{
  return 1;
}


//// calculer_flux_faces_paroi
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_paroi() const
{
  return 0;
}


//// calculer_flux_faces_paroi_adiabatique
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_paroi_adiabatique() const
{
  return 0;
}


//// calculer_flux_faces_paroi_defilante
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_paroi_defilante() const
{
  return 0;
}


//// calculer_flux_faces_paroi_fixe
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_paroi_fixe() const
{
  return 0;
}


//// calculer_flux_faces_sortie_libre
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_sortie_libre() const
{
  return 1;
}


//// calculer_flux_faces_symetrie
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_symetrie() const
{
  return 0;
}

//// calculer_flux_faces_periodique
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_periodique() const
{
  return 1;
}

//// calculer_flux_faces_NSCBC
//

inline int Eval_Div_VDF_Elem::calculer_flux_faces_NSCBC() const
{
  return 1;
}


// Fonctions de calcul des flux pour une grandeur scalaire

//// flux_face avec Dirichlet_entree_fluide
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int face,
                                           const Dirichlet_entree_fluide&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}


//// flux_face avec Dirichlet_paroi_defilante
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab&, int ,
                                           const Dirichlet_paroi_defilante&, int ) const
{
  return 0;
}


//// flux_face avec Dirichlet_paroi_fixe
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab&, int ,
                                           const Dirichlet_paroi_fixe&, int ) const
{
  return 0;
}


//// flux_face avec Echange_externe_impose
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int boundary_index, int face, int local_face,
                                           const Echange_externe_impose&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}


//// flux_face avec Echange_global_impose
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int face,
                                           const Echange_global_impose&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}



//// flux_face avec Neumann_paroi
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int face,
                                           const Neumann_paroi&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int face,
                                           const Neumann_paroi_adiabatique&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}


//// flux_face avec Neumann_sortie_libre
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int face,
                                           const Neumann_sortie_libre&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}


//// flux_face avec Symetrie
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab&, int , const Symetrie&, int ) const
{
  return 0;
}


//// flux_face avec Periodique
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int face,
                                           const Periodique&, int ) const
{
  return vit[face]*surface[face]*porosite[face];
}


//// flux_face avec NSCBC
//

inline double Eval_Div_VDF_Elem::flux_face(const DoubleTab& vit, int faceD,
                                           const NSCBC&, int ) const
{
  /*  // On va retirer la contribution du flux interieur
      const Zone_VDF& zvdf = la_zone;
      int oriD = zvdf.orientation(faceD);
      int faceC;

      int elem0 = zvdf.face_voisins(faceD,0);
      double signe = -1.;
      if (elem0 == -1)
      {
      elem0 = zvdf.face_voisins(faceD,1);
      faceC = zvdf.elem_faces(elem0,oriD+dimension);
      signe = 1.;
      }
      else
      faceC = zvdf.elem_faces(elem0,oriD);

      double flux = signe*flux_faces_interne(vit,faceC);


      // On retourne -flux car il faut qu'il soit retire a ce qui a deja ete ajoute
  // pour la maille de bord consideres.
  return -flux;*/

  // Finalement, on calcul la divergence "standard" comme si la condition NSCBC etait
  // une sortie libre.
  return vit[faceD]*surface[faceD]*porosite[faceD];
}



//// flux_faces_interne
//

inline double Eval_Div_VDF_Elem::flux_faces_interne(const DoubleTab& vit, int face) const
{
  /*  const Zone_VDF& zvdf = la_zone;
      int ori = zvdf.orientation(face);

      int ori_conj;

      if (ori == 0)
      ori_conj = 1;
      else
      ori_conj = 0;

      int f0,f1;
      f0 = zvdf.face_amont_conj(face,ori_conj,0);
      f1 = zvdf.face_amont_conj(face,ori_conj,1);

      double val;

      if ((f0 != -1) && (f1 != -1))
      // On est dans le domaine
      val = 0.75*vit(face) + 0.125*vit(f0) + 0.125*vit(f1);
      else
      {
      if (f0 == -1)
      {
      val = 0.875*vit(face) + 0.125*vit(f1);
      }
      else if (f1 == -1)
      {
      val = 0.875*vit(face) + 0.125*vit(f0);
      }
      }

      val = 0.5*(val + vit[face]);

      //  return val*surface[face]*porosite[face]; */

  return vit[face]*surface[face]*porosite[face];
}

// Fonctions de calcul des flux pour une grandeur vectorielle

//// flux_face avec Dirichlet_entree_fluide
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int face,
                                         const Dirichlet_entree_fluide&,
                                         int , DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


//// flux_face avec Dirichlet_paroi_defilante
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab&, int ,
                                         const Dirichlet_paroi_defilante&,
                                         int, DoubleVect& ) const
{
  ;
}


//// flux_face avec Dirichlet_paroi_fixe
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab&, int ,
                                         const Dirichlet_paroi_fixe&,
                                         int, DoubleVect& ) const
{
  ;
}


//// flux_face avec Echange_externe_impose
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                         const Echange_externe_impose&,
                                         int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


//// flux_face avec Echange_global_impose
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int face,
                                         const Echange_global_impose&,
                                         int, DoubleVect& flux ) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}



//// flux_face avec Neumann_paroi
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int face,
                                         const Neumann_paroi&,
                                         int , DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int face,
                                         const Neumann_paroi_adiabatique&,
                                         int , DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


//// flux_face avec Neumann_sortie_libre
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int face,
                                         const Neumann_sortie_libre&,
                                         int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


//// flux_face avec Symetrie
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab&, int ,
                                         const Symetrie&,
                                         int, DoubleVect&) const
{
  ;
}


//// flux_face avec Periodique
//

inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int face,
                                         const Periodique&,
                                         int , DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


//// flux_faces_interne
//

inline void Eval_Div_VDF_Elem::flux_faces_interne(const DoubleTab& inco,
                                                  int face,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(face,k)*surface[face]*porosite[face];
}


#endif
