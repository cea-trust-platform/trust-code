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
// File:        Rectangle.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Rectangle_included
#define Rectangle_included


#include <Elem_geom_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Rectangle
//    Cette classe represente l'element geometrique Rectangle.
//    Un rectangle a 4 faces, 4 sommets et 1 seul type de face ayant 2
//    sommets par face.
// .SECTION voir aussi
//    Elem_geom_base Elem_geom Rectangle_axi
//////////////////////////////////////////////////////////////////////////////
class Rectangle : public Elem_geom_base
{

  Declare_instanciable(Rectangle);

public :

  inline int face_sommet(int i, int j) const override;
  inline int face_sommet0(int i) const;
  inline int face_sommet1(int i) const;
  inline int face_sommet2(int i) const;
  inline int face_sommet3(int i) const;

  inline int nb_som() const override;
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  inline int est_structure() const override;
  const Nom& nom_lml() const override;
  int contient(const ArrOfDouble&, int ) const override;
  int contient(const ArrOfInt&, int ) const override;
  inline Type_Face type_face(int=0) const override;
  void reordonner() override ;
  int reordonner_elem();
  void calculer_volumes(DoubleVect& ) const override;
  void calculer_normales(const IntTab& faces_sommets ,
                         DoubleTab& face_normales) const override;

  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
};


// Description:
//    Renvoie le numero du j-ieme sommet de la i-ieme face de
//    l'element.
// Precondition:
// Parametre: int i
//    Signification: un numero de face
//    Valeurs par defaut:
//    Contraintes: i < 4
//    Acces:
// Parametre: int j
//    Signification: un numero de sommet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero du j-ieme sommet de la i-ieme face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::face_sommet(int i, int j) const
{
  assert(i<4);
  switch(i)
    {
    case 0:
      return face_sommet0(j);
    case 1:
      return face_sommet1(j);
    case 2:
      return face_sommet2(j);
    case 3:
      return face_sommet3(j);
    default :
      return -1;
    }
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 0
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::face_sommet0(int i) const
{
  // face_sommet0(0)=0;
  // face_sommet0(1)=2;
  assert(i>=0);
  assert(i<2);
  if(i>=2)
    {
      Cerr << "A rectangle has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*i;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 1
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::face_sommet1(int i) const
{
  // face_sommet1(0)=0;
  // face_sommet1(1)=1;
  assert(i>=0);
  assert(i<2);
  if(i>=2)
    {
      Cerr << "A rectangle has not " << i << " nodes per face" << finl;
      exit();
    }
  return i;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 2
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 2
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::face_sommet2(int i) const
{
  // face_sommet2(0)=1;
  // face_sommet2(1)=3;
  assert(i>=0);
  assert(i<2);
  if(i>=2)
    {
      Cerr << "A rectangle has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*i+1;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 3
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 3
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::face_sommet3(int i) const
{
  // face_sommet3(0)=2;
  // face_sommet3(1)=3;
  assert(i>=0);
  assert(i<2);
  if(i>=2)
    {
      Cerr << "A rectangle has not " << i << " nodes per face" << finl;
      exit();
    }
  return i+2;
}


// Description:
//    Renvoie le nombre de sommets d'un triangle: 3.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets d'un triangle
//    Contraintes: toujours egal a 3
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::nb_som() const
{
  return 4;
}


// Description:
//    Renvoie le nombre de faces du type specifie
//    que possede l'element geometrique.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Contraintes: i=0
//    Acces:
// Retour: int
//    Signification: le nombre de faces de type i
//    Contraintes: toujours egal a 4 (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::nb_faces(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A rectangle has not " << i << " types of face" << finl;
      exit();
    }
  return 4;
}


// Description:
//    Renvoie le nombre de sommets des faces
//    du type specifie.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Contraintes:  i=0
//    Acces:
// Retour: int
//    Signification: le nombre de sommets des faces de type i
//    Contraintes:  toujours egal a 2 (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::nb_som_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A rectangle has not " << i << " types of face" << finl;
      exit();
    }
  return 2;
}


// Description:
//    Renvoie toujours 1, car l'element geometrique
//    Rectangle est de type structure.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si l'element est de type structure
//                   0 sinon
//    Contraintes: toujours egal a 1
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Rectangle::est_structure() const
{
  return 1;
}


// Description:
//    Renvoie le i-ieme type de face.
//    Un rectangle n'a qu'un seul type de face.
// Precondition:
// Parametre: int i
//    Signification: le rang du type de face a renvoyer
//    Valeurs par defaut: 0
//    Contraintes: i=0
//    Acces:
// Retour: Type_Face
//    Signification: un type de face
//    Contraintes: toujours egal a Faces::segment_2D (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Type_Face Rectangle::type_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A rectangle has not " << i << " types of face" << finl;
      exit();
    }
  //  Cerr << finl;
  //  Cerr << "on ait dans Type_Face Rectangle::type_face(int i) const" << finl;
  return Faces::segment_2D;
}

#endif
