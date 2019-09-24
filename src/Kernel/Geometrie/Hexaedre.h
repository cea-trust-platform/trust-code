/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Hexaedre.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Hexaedre_included
#define Hexaedre_included




#include <Elem_geom_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Hexaedre
//    Cette represente un element geometrique a 6 faces, 8 sommets et
//    un seul type de face (Faces::quadrangle_3D) a 4 sommets.
// .SECTION voir aussi
//    Hexaedre_axi Elem_geom_base Elem_geom
//    Un hexaedre est un element 3D
//////////////////////////////////////////////////////////////////////////////
class Hexaedre  : public Elem_geom_base
{

  Declare_instanciable(Hexaedre);

public :


  // Fonctions d'acces aux membres prives:
  int face_sommet(int i, int j) const; // Cette methode n'est plus inlinee car probleme au link symbol "reference dans une section .rodata" sur certains compilateurs gcc 3.3.x
  inline int face_sommet0(int i) const;
  inline int face_sommet1(int i) const;
  inline int face_sommet2(int i) const;
  inline int face_sommet3(int i) const;
  inline int face_sommet4(int i) const;
  inline int face_sommet5(int i) const;
  inline int nb_som() const;
  inline int nb_faces(int=0) const;
  inline int nb_som_face(int=0) const;
  inline int est_structure() const;
  virtual const Nom& nom_lml() const;
  virtual int contient(const ArrOfDouble&, int ) const;
  virtual int contient(const ArrOfInt&, int ) const;
  inline Type_Face type_face(int=0) const;
  virtual void reordonner() ;
  int reordonner_elem();
  virtual void calculer_volumes(DoubleVect& ) const;
  void calculer_normales(const IntTab& faces_sommets ,
                         DoubleTab& face_normales) const;

  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const;
};

// Description:
//    Renvoie le nombre de sommets d'un hexaedre: 8.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets d'un hexaedre
//    Contraintes: toujours egal a 8
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::nb_som() const
{
  return 8;
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
//    Contraintes: toujours egal a 6 (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::nb_faces(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A hexahedron has not " << i << " types of face" << finl;
      exit();
    }
  return 6;
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
//    Contraintes:  toujours egal a 4 (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::nb_som_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A hexahedron has not " << i << " types of face" << finl;
      exit();
    }
  return 4;
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
inline int Hexaedre::face_sommet0(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=2;
  //face_sommet0(2)=4;
  //face_sommet0(3)=6;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A hexahedron has not " << i << " nodes per face" << finl;
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
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::face_sommet1(int i) const
{
  //face_sommet1(0)=0;
  //face_sommet1(1)=1;
  //face_sommet1(2)=4;
  //face_sommet1(3)=5;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A hexahedron has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*(i/2)+i;
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
inline int Hexaedre::face_sommet2(int i) const
{
  //face_sommet2(0)=0;
  //face_sommet2(1)=1;
  //face_sommet2(2)=2;
  //face_sommet2(3)=3;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A hexahedron has not " << i << " nodes per face" << finl;
      exit();
    }
  return i;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 3
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 3
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::face_sommet3(int i) const
{
  //face_sommet3(0)=1;
  //face_sommet3(1)=3;
  //face_sommet3(2)=5;
  //face_sommet3(3)=7;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A hexahedron has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*i+1;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 4
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 4
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::face_sommet4(int i) const
{
  //face_sommet4(0)=2;
  //face_sommet4(1)=3;
  //face_sommet4(2)=6;
  //face_sommet4(3)=7;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A hexahedron has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*(i/2)+i+2;
}


// Description:
//    Renvoie le numero du i-ieme sommet de la face 5
// Precondition:
// Parametre: int i
//    Signification: le numero du sommet a renvoyer
//    Valeurs par defaut:
//    Contraintes: i=0,1,2,3
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme sommet de la face 5
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Hexaedre::face_sommet5(int i) const
{
  //face_sommet4(0)=4;
  //face_sommet4(1)=5;
  //face_sommet4(2)=6;
  //face_sommet4(3)=7;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A hexahedron has not " << i << " nodes per face" << finl;
      exit();
    }
  return i+4;
}


// Description:
//    Renvoie toujours 1, car l'element geometrique
//    Hexaedre est de type structure.
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
inline int Hexaedre::est_structure() const
{
  return 1;
}


// Description:
//    Renvoie le i-ieme type de face.
//    Un hexaedre n'a qu'un seul type de face.
// Precondition:
// Parametre: int i
//    Signification: le rang du type de face a renvoyer
//    Valeurs par defaut: 0
//    Contraintes: i=0
//    Acces:
// Retour: Type_Face
//    Signification: un type de face
//    Contraintes: toujours egal a Faces::quadrangle_3D (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Type_Face Hexaedre::type_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A hexahedron has not " << i << " types of face" << finl;
      exit();
    }
  return Faces::quadrangle_3D;
}


#endif
