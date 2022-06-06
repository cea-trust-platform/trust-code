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

#ifndef Joint_included
#define Joint_included

#include <Frontiere.h>
#include <Joint_Items.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    La classe Joint est une Frontiere qui contient les faces
//    et les sommets de joint avec la zone PEvoisin() (pour les maillages
//    distribues en parallele).
//    Elle porte en plus dans les Joint_Items les informations
//    permettant de construire les tableaux distribues indexes
//    par les items geometriques (sommets, faces, elements, aretes,
//    faces_frontieres)
//
// .SECTION voir aussi
//    Scatter Joint_Item
//////////////////////////////////////////////////////////////////////////////

class Joint : public Frontiere
{

  Declare_instanciable(Joint);
public:

  void affecte_PEvoisin(int);
  void affecte_epaisseur(int);
  int PEvoisin() const;
  int epaisseur() const;

  void dimensionner(int);
  void ajouter_faces(const IntTab&);

  // Accesseurs pour compatibilite avec la version precedente
  // (supprimes bientot)
  const IntTab&    renum_virt_loc() const;
  const ArrOfInt& esp_dist_elems() const;
  const ArrOfInt& esp_dist_sommets() const;
  const ArrOfInt& esp_dist_faces() const;

  // Nouvelle interface pour acceder aux donnees de joint:
  enum Type_Item { SOMMET, ELEMENT, FACE, ARETE, FACE_FRONT };
  Joint_Items&        set_joint_item(Type_Item type);
  const Joint_Items& joint_item(Type_Item type) const;

private:
  // Numero du PEvoisin correspondant au joint
  int PEvoisin_;

  // Epaisseur de joint, utilise par Scatter::calculer_espace_distant_elements
  int epaisseur_;

  Joint_Items joint_sommets_;
  Joint_Items joint_elements_;
  Joint_Items joint_faces_;
  Joint_Items joint_faces_front_;
  Joint_Items joint_aretes_;
};

#endif
