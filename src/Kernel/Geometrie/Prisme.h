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

#ifndef Prisme_included
#define Prisme_included




#include <Elem_geom_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Prisme
//    Cette represente l'element geometrique Prisme.
//    Un prisme est un polyedre ayant 5 faces, 6 sommets et deux types
//    de face:
//       2 triangles a 3 sommets
//       3 quadrangles a 4 sommets
// .SECTION voir aussi
//    Elem_geom_base Elem_geom
//////////////////////////////////////////////////////////////////////////////
class Prisme  : public Elem_geom_base
{

  Declare_instanciable(Prisme);

public :


  // Fonctions d'acces aux membres prives:
  inline int face_sommet(int i, int j) const override;
  inline int face_sommet0(int i) const;
  inline int face_sommet1(int i) const;
  inline int face_sommet2(int i) const;
  inline int face_sommet3(int i) const;
  inline int face_sommet4(int i) const;
  inline int nb_som() const override;
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  inline int est_structure() const override;
  const Nom& nom_lml() const override;
  int contient(const ArrOfDouble&, int ) const override;
  int contient(const ArrOfInt&, int ) const override;
  inline Type_Face type_face(int=0) const override;
  void reordonner() override ;
  void calculer_volumes(DoubleVect& ) const override;
  int nb_type_face() const override;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
};


// Description:
//    Renvoie le numero du j-ieme sommet de la i-ieme face de
//    l'element.
// Precondition:
// Parametre: int i
//    Signification: un numero de face
//    Valeurs par defaut:
//    Contraintes: i < 5
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
inline int Prisme::face_sommet(int face, int sommet) const
{
  assert(face<5);
  switch(face)
    {
    case 0:
      return face_sommet0(sommet);
    case 1:
      return face_sommet1(sommet);
    case 2:
      return face_sommet2(sommet);
    case 3:
      return face_sommet3(sommet);
    case 4:
      return face_sommet4(sommet);
    default :
      return -1;
    }
}


// Description:
//    Renvoie le nombre de sommets d'un Prisme: 6.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets d'un prisme
//    Contraintes: toujours egal a 6
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::nb_som() const
{
  return 6;
}


// Description:
//    Renvoie le nombre de faces du type specifie
//    que possede l'element geometrique.
//    Un Prisme a 2 types de faces: quadrangle ou triangle.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Contraintes: i=0 (quadrangle), 1 (triangle)
//    Acces:
// Retour: int
//    Signification: le nombre de faces de type i
//    Contraintes: 3 si i=0, 2 si i=1
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::nb_faces(int i) const
{
  switch(i)
    {
    case 0:
      return 3; //les quadrangles
    case 1:
      return 2; //les triangles
    default :
      Cerr << "Error, a prism has 2 types of faces and not " << i << finl;
      exit();
      return -1;
    }

}


// Description:
//    Renvoie le nombre de sommets des faces
//    du type specifie.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut: 0
//    Contraintes: i=0 (quadrangle), 1 (triangle)
//    Acces:
// Retour: int
//    Signification: le nombre de sommets des faces de type i
//    Contraintes: 4 si i=0, 3 si i=1
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::nb_som_face(int i) const
{
  switch(i)
    {
    case 0:
      return 4;
    case 1:
      return 3;
    default :
      Cerr << "Error, a prism has 2 types of faces and not " << i << finl;
      exit();
      return -1;
    }
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 0
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::face_sommet0(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=1;
  //face_sommet0(2)=3;
  //face_sommet0(3)=4;
  assert(i>=0);
  assert(i<4);
  switch(i)
    {
    case 0:
      return 0;
    case 1:
      return 1;
    case 2:
      return 3;
    case 3:
      return 4;
    default :
      Cerr << "Error in Prisme::face_sommet0" << finl;
      exit();
      // Pour qq compilos :
      return -1;
    }
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 1
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::face_sommet1(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=2;
  //face_sommet0(2)=3;
  //face_sommet0(3)=5;
  assert(i>=0);
  assert(i<4);
  switch(i)
    {
    case 0:
      return 0;
    case 1:
      return 2;
    case 2:
      return 3;
    case 3:
      return 5;
    default :
      Cerr << "Error in Prisme::face_sommet1" << finl;
      exit();
      // Pour qq compilos :
      return -1;
    }
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 2
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 2
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::face_sommet2(int i) const
{
  //face_sommet0(0)=1;
  //face_sommet0(1)=2;
  //face_sommet0(2)=4;
  //face_sommet0(3)=5;
  assert(i>=0);
  assert(i<4);
  switch(i)
    {
    case 0:
      return 1;
    case 1:
      return 2;
    case 2:
      return 4;
    case 3:
      return 5;
    default :
      Cerr << "Error in Prisme::face_sommet2" << finl;
      exit();
      // Pour qq compilos :
      return -1;
    }
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 3
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 3
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::face_sommet3(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=1;
  //face_sommet0(2)=2;
  assert(i>=0);
  assert(i<3);
  return i;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 4
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 4
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::face_sommet4(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=1;
  //face_sommet0(2)=2;
  assert(i>=0);
  assert(i<3);
  return i+3;
}


// Description:
//    Renvoie toujours 0, car l'element geometrique
//    Prisme n'est pas de type structure.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si l'element est de type structure
//                   0 sinon
//    Contraintes: toujours egal a 0
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Prisme::est_structure() const
{
  return 0;
}


// Description:
//    Renvoie le i-ieme type de face.
//    Un prisme a 2 types de face: quadrangle et triangle
// Precondition:
// Parametre: int i
//    Signification: le rang du type de face a renvoyer
//    Valeurs par defaut: 0
//    Contraintes: i=0 (quadrangle), i=1 (triangle)
//    Acces:
// Retour: Type_Face
//    Signification: le type de la face i
//    Contraintes: i=0 (Faces::quadrangle_3D), i=1 (Faces::triangle_3D)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Type_Face Prisme::type_face(int i) const
{
  assert(i<=1);
  switch(i)
    {
    case 0:
      return Faces::quadrangle_3D;
    case 1:
      return Faces::triangle_3D;
    default :
      Cerr << "Error, a prism has 2 types of faces and not " << i << finl;
      exit();
      return Faces::quadrangle_3D;
    }
}

#endif
