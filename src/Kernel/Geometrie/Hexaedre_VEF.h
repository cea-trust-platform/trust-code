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

#ifndef Hexaedre_VEF_included
#define Hexaedre_VEF_included




#include <Elem_geom_base.h>


/*! @brief Classe Hexaedre_VEF Cette classe represente l'element geometrique Hexaedre_VEF.
 *
 *     Un Hexaedre_VEF est un polyedre qui a 4 faces, 4 sommets et
 *     un seul type de face ayant 3 sommets par face.
 *
 * @sa Elem_geom_base Elem_geom, C'est un element utilisable en 3D
 */
class Hexaedre_VEF  : public Elem_geom_base
{

  Declare_instanciable(Hexaedre_VEF);

public :

  inline int face_sommet(int i, int j) const override;
  inline int face_sommet0(int i) const;
  inline int face_sommet1(int i) const;
  inline int face_sommet2(int i) const;
  inline int face_sommet3(int i) const;
  inline int face_sommet4(int i) const;
  inline int face_sommet5(int i) const;
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

  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
};


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
inline int Hexaedre_VEF::face_sommet(int i, int j) const
{
  assert(i<6);
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
    case 4:
      return face_sommet4(j);
    case 5:
      return face_sommet5(j);
    default :
      return -1;
    }
}


/*! @brief Renvoie le nombre de sommets d'un Hexaedre_VEF: 8.
 *
 * @return (int) le nombre de sommets d'un Hexaedre_VEF
 */
inline int Hexaedre_VEF::nb_som() const
{
  return 8;
}


/*! @brief Renvoie le nombre de faces du type specifie que possede l'element geometrique.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de faces de type i
 */
inline int Hexaedre_VEF::nb_faces(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " types of face" << finl;
      exit();
    }
  return 6;
}


/*! @brief Renvoie le nombre de sommets des faces du type specifie.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces de type i
 */
inline int Hexaedre_VEF::nb_som_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " types of face" << finl;
      exit();
    }
  return 4;
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 0
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 0
 */
inline int Hexaedre_VEF::face_sommet0(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=2;
  //face_sommet0(2)=4;
  //face_sommet0(3)=6;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*i;
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 1
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 1
 */
inline int Hexaedre_VEF::face_sommet1(int i) const
{
  //face_sommet1(0)=0;
  //face_sommet1(1)=1;
  //face_sommet1(2)=4;
  //face_sommet1(3)=5;
  assert(i>=0);
  assert(i<4);
  switch(i)
    {
    case 0:
      return 0;
    case 1:
      return 1;
    case 2:
      return 4;
    case 3:
      return 5;
    default :
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " nodes per face" << finl;
      exit();
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 2
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 2
 */
inline int Hexaedre_VEF::face_sommet2(int i) const
{
  //face_sommet2(0)=0;
  //face_sommet2(1)=1;
  //face_sommet2(2)=2;
  //face_sommet2(3)=3;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " nodes per face" << finl;
      exit();
    }
  return i;
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 3
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 3
 */
inline int Hexaedre_VEF::face_sommet3(int i) const
{
  //face_sommet3(0)=1;
  //face_sommet3(1)=3;
  //face_sommet3(2)=5;
  //face_sommet3(3)=7;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " nodes per face" << finl;
      exit();
    }
  return 2*i+1;
}

/*! @brief Renvoie le numero du i-ieme sommet de la face 4
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 4
 */
inline int Hexaedre_VEF::face_sommet4(int i) const
{
  //face_sommet4(0)=2;
  //face_sommet4(1)=3;
  //face_sommet4(2)=6;
  //face_sommet4(3)=7;
  assert(i>=0);
  assert(i<4);
  switch(i)
    {
    case 0:
      return 2;
    case 1:
      return 3;
    case 2:
      return 6;
    case 3:
      return 7;
    default :
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " nodes per face" << finl;
      exit();
      return -1;
    }
}

/*! @brief Renvoie le numero du i-ieme sommet de la face 5
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 5
 */
inline int Hexaedre_VEF::face_sommet5(int i) const
{
  //face_sommet4(0)=4;
  //face_sommet4(1)=5;
  //face_sommet4(2)=6;
  //face_sommet4(3)=7;
  assert(i>=0);
  assert(i<4);
  if(i>=4)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " nodes per face" << finl;
      exit();
    }
  return i+4;
}



/*! @brief Renvoie toujours 0, car l'element geometrique Hexaedre_VEF n'est pas de type structure.
 *
 * @return (int) 1 si l'element est de type structure 0 sinon
 */
inline int Hexaedre_VEF::est_structure() const
{
  return 0;
}


/*! @brief Renvoie le i-ieme type de face.
 *
 * Un Hexaedre_VEF n'a qu'un seul type de face.
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) un type de face
 */
inline Type_Face Hexaedre_VEF::type_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A VEF hexahedron (Hexaedre_VEF) has not " << i << " types of face" << finl;
      exit();
    }
  return Faces::quadrangle_3D;
}

#endif
