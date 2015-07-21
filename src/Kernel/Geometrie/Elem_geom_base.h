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
// File:        Elem_geom_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Elem_geom_base_included
#define Elem_geom_base_included




#include <Faces.h>

class DoubleTab;
class DoubleVect;
class Nom;
class ArrOfDouble;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Elem_geom_base
//    Cette classe est la classe de base pour la definition d'elements
//    geometriques constituant un maillage (i.e. une Zone geometrique)
//    Un element geometrique a une Zone associe a laquelle il fournit
//    les routines de base pour la manipulation des ses elements.
//    (Une zone n'a qu'un seul type d'element geometrique)
// .SECTION voir aussi
//    Hexaedre Prisme Rectangle Segment Tetraedre Triangle
//    Zone
//    Classe abstraite
//    Methodes abstraites:
//      int face_sommet(int i, int j) const
//      int nb_som() const
//      int nb_faces(int=0) const
//      int nb_som_face(int=0) const
//      int est_structure() const
//      const Nom& nom_lml() const
//      int contient(const ArrOfDouble&, int ) const
//      int contient(const ArrOfInt&, int ) const
//      Type_Face type_face(int=0) const
//      void calculer_volumes(DoubleVect& ) const
//      void calculer_normales(const IntTab& , DoubleTab& ) const
//////////////////////////////////////////////////////////////////////////////
class Elem_geom_base  : public Objet_U
{

  Declare_base(Elem_geom_base);

public:

  virtual void creer_faces_elem(Faces& ,int ,Type_Face ) const;
  inline void creer_faces_elem(Faces& ,int ) const;
  virtual int face_sommet(int i, int j) const=0;
  inline void associer_zone(const Zone& );
  virtual int nb_som() const=0;
  virtual int nb_faces(int=0) const=0;
  virtual int nb_som_face(int=0) const=0;
  virtual int est_structure() const=0;
  virtual const Nom& nom_lml() const=0;
  virtual int contient(const ArrOfDouble&, int ) const=0;
  virtual int contient(const ArrOfInt&, int ) const=0;
  virtual int nb_type_face() const;
  virtual int num_face(int face, Type_Face& type) const;
  virtual Type_Face type_face(int=0) const=0;
  virtual void calculer_centres_gravite(DoubleTab& ) const ;
  virtual void reordonner() ;
  virtual void calculer_volumes(DoubleVect& ) const =0;
  virtual void calculer_normales(const IntTab& faces_sommets , DoubleTab& face_normales) const;

  virtual int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const;
  virtual void get_tab_aretes_sommets_locaux(IntTab& aretes_som_local) const;

protected:

  REF(Zone) ma_zone;
};


// Description:
//    Associe l'element geometrique a sa zone.
// Precondition:
// Parametre: Zone& une_zone
//    Signification: la zone a associee a l'element
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Elem_geom_base::associer_zone(const Zone& une_zone)
{
  ma_zone=une_zone;
}


// Description:
//    Cree les faces de l'element de la zone specifie.
// Precondition: le nombre de type de face doit etre egal a 1
// Parametre: Faces& faces
//    Signification: les faces de l'elements a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Parametre: int elem
//    Signification: le numero de element de la zone dont
//                   veut creer les faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Elem_geom_base::creer_faces_elem(Faces& faces, int elem) const
{
  assert(nb_type_face() == 1);
  creer_faces_elem(faces, elem, type_face());
}

#endif
