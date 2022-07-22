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


/*! @brief Classe Prisme Cette represente l'element geometrique Prisme.
 *
 *     Un prisme est un polyedre ayant 5 faces, 6 sommets et deux types
 *     de face:
 *        2 triangles a 3 sommets
 *        3 quadrangles a 4 sommets
 *
 * @sa Elem_geom_base Elem_geom
 */
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


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
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


/*! @brief Renvoie le nombre de sommets d'un Prisme: 6.
 *
 * @return (int) le nombre de sommets d'un prisme
 */
inline int Prisme::nb_som() const
{
  return 6;
}


/*! @brief Renvoie le nombre de faces du type specifie que possede l'element geometrique.
 *
 *     Un Prisme a 2 types de faces: quadrangle ou triangle.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de faces de type i
 */
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


/*! @brief Renvoie le nombre de sommets des faces du type specifie.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces de type i
 */
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


/*! @brief Renvoie le numero du i-ieme sommet de la face 0
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 0
 */
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


/*! @brief Renvoie le numero du i-ieme sommet de la face 1
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 1
 */
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


/*! @brief Renvoie le numero du i-ieme sommet de la face 2
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 2
 */
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


/*! @brief Renvoie le numero du i-ieme sommet de la face 3
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 3
 */
inline int Prisme::face_sommet3(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=1;
  //face_sommet0(2)=2;
  assert(i>=0);
  assert(i<3);
  return i;
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 4
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 4
 */
inline int Prisme::face_sommet4(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=1;
  //face_sommet0(2)=2;
  assert(i>=0);
  assert(i<3);
  return i+3;
}


/*! @brief Renvoie toujours 0, car l'element geometrique Prisme n'est pas de type structure.
 *
 * @return (int) 1 si l'element est de type structure 0 sinon
 */
inline int Prisme::est_structure() const
{
  return 0;
}


/*! @brief Renvoie le i-ieme type de face.
 *
 * Un prisme a 2 types de face: quadrangle et triangle
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) le type de la face i
 */
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
