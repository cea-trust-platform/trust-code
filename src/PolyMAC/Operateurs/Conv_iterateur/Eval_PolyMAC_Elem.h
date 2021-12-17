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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_PolyMAC_Elem.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs/Conv_iterateur
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_PolyMAC_Elem_included
#define Eval_PolyMAC_Elem_included

#include <Ref_Champ_base.h>
#include <Periodique.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_entree_fluide.h>
#include <Neumann_paroi.h>
#include <Neumann_paroi_adiabatique.h>
#include <Symetrie.h>
#include <Echange_global_impose.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>

//
// .DESCRIPTION class Eval_PolyMAC_Elem
//
// Cette classe represente le prototype fonctionnel
// des evaluateurs de flux associes aux equations de
// conservation integrees sur les elements


class Eval_PolyMAC_Elem
{

public:

  inline Eval_PolyMAC_Elem();
  inline virtual ~Eval_PolyMAC_Elem() {};
  void associer_inconnue(const Champ_base& );

#ifndef _INLINE_EVALUATEURS_


  virtual int calculer_flux_faces_symetrie() const=0;
  virtual int calculer_flux_faces_sortie_libre() const=0;
  virtual int calculer_flux_faces_entree_fluide() const=0;
  virtual int calculer_flux_faces_paroi_fixe() const=0;
  virtual int calculer_flux_faces_paroi_defilante() const=0;
  virtual int calculer_flux_faces_paroi_adiabatique() const=0;
  virtual int calculer_flux_faces_paroi() const=0;
  virtual int calculer_flux_faces_echange_externe_impose() const=0;
  virtual int calculer_flux_faces_echange_global_impose() const=0;
  virtual int calculer_flux_faces_periodique() const=0;

  //virtual int calculer_flux_faces_nouvelle_Cl_PolyMAC() const=0;


  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles renvoient le flux calcule

  virtual double flux_face(const DoubleTab&, int , const Symetrie&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Neumann_paroi&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , int , int , const Echange_externe_impose&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Echange_global_impose&, int ) const=0;
  virtual double flux_face(const DoubleTab&, int , const Periodique&, int ) const=0;
  //virtual inline double flux_face(const DoubleTab&, int , const Nouvelle_Cl_PolyMAC&, int ) const=0;

  virtual inline double flux_faces_interne(const DoubleTab&, int ) const=0;


  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  virtual void flux_face(const DoubleTab&, int , const Symetrie&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Neumann_paroi&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , int , int , const Echange_externe_impose&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Echange_global_impose&,
                         int, DoubleVect& flux) const=0;
  virtual void flux_face(const DoubleTab&, int , const Periodique&,
                         int, DoubleVect& flux) const=0;

  //virtual inline void flux_face(const DoubleTab&, int ,const Nouvelle_Cl_PolyMAC&,
  //                              int, DoubleVect& flux) const=0;

  virtual inline void flux_faces_interne(const DoubleTab&, int ,
                                         DoubleVect& flux) const=0;



#endif
  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  virtual void coeffs_face(int,int, const Symetrie&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Dirichlet_paroi_fixe&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Dirichlet_paroi_defilante&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Neumann_paroi_adiabatique&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Neumann_paroi&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Echange_global_impose&, double& aii, double& ajj ) const
  {
    ;
  }
  virtual void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const
  {
    ;
  }

  // contribution de la derivee en vitesse d'une equation scalaire
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Echange_global_impose&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_paroi&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Symetrie&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Periodique&, int ) const
  {
    return 0.;
  }
  virtual double coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int ) const
  {
    return 0.;
  }

  //virtual void coeffs_face(int , const Nouvelle_Cl_PolyMAC&, int,
  //                       double& aii, double& ajj ) const { ; }

  virtual void coeffs_faces_interne(int, double& aii, double& ajj ) const
  {
    ;
  }

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite.

  virtual double secmem_face(int, const Symetrie&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Neumann_sortie_libre&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Dirichlet_entree_fluide&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Dirichlet_paroi_fixe&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Dirichlet_paroi_defilante&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Neumann_paroi_adiabatique&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Neumann_paroi&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int,int,int, const Echange_externe_impose&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Echange_global_impose&, int ) const
  {
    return 0;
  }
  virtual double secmem_face(int, const Periodique&, int) const
  {
    return 0;
  }
  virtual double secmem_faces_interne(int) const
  {
    return 0;
  }

  //virtual double secmem_face(int , const Nouvelle_Cl_PolyMAC&, int ) const { ; }


  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  virtual void coeffs_face(int,int, const Symetrie&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Neumann_paroi_adiabatique&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  virtual void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }


  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite.

  virtual void secmem_face(int, const Symetrie&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Neumann_paroi_adiabatique&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Neumann_paroi&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int,int,int, const Echange_externe_impose&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Echange_global_impose&, int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_faces_interne(int, DoubleVect& ) const
  {
    ;
  }
  virtual void secmem_face(int, const Periodique&, int, DoubleVect&   ) const
  {
    ;
  }

  //virtual void secmem_face(int , const Nouvelle_Cl_PolyMAC&, int, DoubleVect& ) const { ; }
protected:

  REF(Champ_base) inconnue;

};

//
// Fonctions inline de la classe Eval_PolyMAC_Elem
//

inline Eval_PolyMAC_Elem::Eval_PolyMAC_Elem()
{}
#endif
