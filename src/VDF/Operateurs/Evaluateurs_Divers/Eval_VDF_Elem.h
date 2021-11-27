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
// File:        Eval_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Divers
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_VDF_Elem_included
#define Eval_VDF_Elem_included

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
#include <NSCBC.h>

//
// .DESCRIPTION class Eval_VDF_Elem
//
// Cette classe represente le prototype fonctionnel
// des evaluateurs de flux associes aux equations de
// conservation integrees sur les elements


class Eval_VDF_Elem
{

public:
  void associer_inconnue(const Champ_base& );

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  void coeffs_face(int,int, const Symetrie&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Dirichlet_paroi_fixe&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Dirichlet_paroi_defilante&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Neumann_paroi_adiabatique&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Neumann_paroi&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Echange_global_impose&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_face(int,int, const NSCBC&, double& aii, double& ajj ) const
  {
    ;
  }

  // contribution de la derivee en vitesse d'une equation scalaire
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Echange_global_impose&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_paroi&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Symetrie&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Periodique&, int ) const
  {
    return 0.;
  }
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const NSCBC&, int ) const
  {
    return 0.;
  }
  double coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int ) const
  {
    return 0.;
  }

  //void coeffs_face(int , const Nouvelle_Cl_VDF&, int,
  //                       double& aii, double& ajj ) const { ; }

  void coeffs_faces_interne(int, double& aii, double& ajj ) const
  {
    ;
  }

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite.

  double secmem_face(int, const Symetrie&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Neumann_sortie_libre&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Dirichlet_entree_fluide&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Dirichlet_paroi_fixe&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Dirichlet_paroi_defilante&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Neumann_paroi_adiabatique&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Neumann_paroi&, int ) const
  {
    return 0;
  }
  double secmem_face(int,int,int, const Echange_externe_impose&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Echange_global_impose&, int ) const
  {
    return 0;
  }
  double secmem_face(int, const Periodique&, int) const
  {
    return 0;
  }
  double secmem_face(int, const NSCBC&, int) const
  {
    return 0;
  }
  double secmem_faces_interne(int) const
  {
    return 0;
  }

  //double secmem_face(int , const Nouvelle_Cl_VDF&, int ) const { ; }


  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  void coeffs_face(int,int, const Symetrie&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Neumann_paroi_adiabatique&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }

  void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const
  {
    ;
  }


  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite.

  void secmem_face(int, const Symetrie&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Neumann_paroi_adiabatique&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Neumann_paroi&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int,int,int, const Echange_externe_impose&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Echange_global_impose&, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_faces_interne(int, DoubleVect& ) const
  {
    ;
  }
  void secmem_face(int, const Periodique&, int, DoubleVect&   ) const
  {
    ;
  }

  //void secmem_face(int , const Nouvelle_Cl_VDF&, int, DoubleVect& ) const { ; }
protected:

  REF(Champ_base) inconnue;

};

//
// Fonctions inline de la classe Eval_VDF_Elem
//
#endif
