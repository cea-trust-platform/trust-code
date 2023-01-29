/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Sous_domaine_VF_included
#define Sous_domaine_VF_included

#include <Sous_domaine_dis_base.h>
#include <Domaine_VF.h>
#include <TRUST_Ref.h>

//! Cette classe abstraite contient les informations geometrique de sous-domaine communes aux methodes de volumes finis (VDF et VEF par exemple)
/**
   On suppose que chaque face a au plus deux elements voisins dans la
   domaine, ce qui est toujours le cas en maillage conforme.

   Le tableau les_faces contient l'ensemble des faces de la Domaine_dis qui
   appartiennent a cette sous-domaine, classees de la facon suivante :
   * Les faces internes a la sous-domaine
   * Les faces internes au domaine, mais dont seul le premier element voisin
   appartient a la sous-domaine
   * Les faces internes au domaine, mais dont seul le deuxieme element voisin
   appartient a la sous-domaine
   * Les faces qui ne presentent qu'un element dans le domaine.
   Les separations sont indiquees respectivement par les variables
   premiere_face_bord_0, premiere_face_bord_1, et premiere_face_bord.

   les_faces : nb_dim=1
   dimension(0) = le nombre de faces de la sous_domaine
   valeur de l'entier = indice dans les tableaux le_dom_VF->face_voisins_,
   le_dom_VF->face_sommets_,...

   volumes_entrelaces(int face) renvoie le volume entrelace restreint
   a la sous-domaine. Le numero de la face fait reference au tableau
   les_faces. Seuls sont stockes localement les volumes entrelaces
   differents de ceux du domaine.
*/

class Sous_domaine_VF : public Sous_domaine_dis_base
{

  Declare_instanciable(Sous_domaine_VF);

public:

  // Methodes d'acces
  inline const IntTab& les_faces() const
  {
    return les_faces_;
  }
  inline IntTab& les_faces()
  {
    return les_faces_;
  }
  inline int premiere_face_bord_0() const
  {
    return premiere_face_bord_0_;
  }
  inline int premiere_face_bord_1() const
  {
    return premiere_face_bord_1_;
  }
  inline int premiere_face_bord() const
  {
    return premiere_face_bord_;
  }
  //! Renvoie le volume entrelace restreint a la sous-domaine. face est l'indice dans le tableau les_faces_.
  inline double volumes_entrelaces(int) const;

  // Methodes propres

  //! Genere les_faces en parcourant les faces du domaine_dis et en cherchant quels voisins appartiennent a la_sous_domaine.
  void discretiser() override;

protected:
  REF2(Domaine_VF) le_dom_VF;
  IntTab les_faces_;
  int premiere_face_bord_0_ = -10;
  int premiere_face_bord_1_= -10;
  int premiere_face_bord_= -10;
  DoubleTab volumes_entrelaces_;
};

inline double Sous_domaine_VF::volumes_entrelaces(int face) const
{
  if (face<premiere_face_bord_0_ || face>=premiere_face_bord_)
    return le_dom_VF->volumes_entrelaces(les_faces_(face));
  else
    return volumes_entrelaces_(face-premiere_face_bord_0_);
}

#endif
