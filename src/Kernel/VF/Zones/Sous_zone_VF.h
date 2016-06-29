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
// File:        Sous_zone_VF.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sous_zone_VF_included
#define Sous_zone_VF_included

#include <Sous_zone_dis_base.h>
#include <Ref_Zone_VF.h>
#include <Zone_VF.h>

//! Cette classe abstraite contient les informations geometrique de sous-zone communes aux methodes de volumes finis (VDF et VEF par exemple)
/**
   On suppose que chaque face a au plus deux elements voisins dans la
   zone, ce qui est toujours le cas en maillage conforme.

   Le tableau les_faces contient l'ensemble des faces de la Zone_dis qui
   appartiennent a cette sous-zone, classees de la facon suivante :
   * Les faces internes a la sous-zone
   * Les faces internes a la zone, mais dont seul le premier element voisin
   appartient a la sous-zone
   * Les faces internes a la zone, mais dont seul le deuxieme element voisin
   appartient a la sous-zone
   * Les faces qui ne presentent qu'un element dans la zone.
   Les separations sont indiquees respectivement par les variables
   premiere_face_bord_0, premiere_face_bord_1, et premiere_face_bord.

   les_faces : nb_dim=1
   dimension(0) = le nombre de faces de la sous_zone
   valeur de l'entier = indice dans les tableaux la_zone_VF->face_voisins_,
   la_zone_VF->face_sommets_,...

   volumes_entrelaces(int face) renvoie le volume entrelace restreint
   a la sous-zone. Le numero de la face fait reference au tableau
   les_faces. Seuls sont stockes localement les volumes entrelaces
   differents de ceux de la zone.
*/

class Sous_zone_VF : public Sous_zone_dis_base
{

  Declare_instanciable(Sous_zone_VF);

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
  //! Renvoie le volume entrelace restreint a la sous-zone. face est l'indice dans le tableau les_faces_.
  inline double volumes_entrelaces(int) const;

  // Methodes propres

  //! Genere les_faces en parcourant les faces de la zone_dis et en cherchant quels voisins appartiennent a la_sous_zone.
  virtual void discretiser();

protected:
  REF(Zone_VF) la_zone_VF;
  IntTab les_faces_;
  int premiere_face_bord_0_;
  int premiere_face_bord_1_;
  int premiere_face_bord_;
  DoubleTab volumes_entrelaces_;
};

inline double Sous_zone_VF::volumes_entrelaces(int face) const
{
  if (face<premiere_face_bord_0_ || face>=premiere_face_bord_)
    return la_zone_VF->volumes_entrelaces(les_faces_(face));
  else
    return volumes_entrelaces_(face-premiere_face_bord_0_);
}

#endif
