/****************************************************************************
* Copyright (c) 2024, CEA
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

template <typename _SIZE_> class Domaine_32_64;

/*! @brief Classe Prisme Cette represente l'element geometrique Prisme.
 *
 *     Un prisme est un polyedre ayant 5 faces, 6 sommets et deux types
 *     de face:
 *        2 triangles a 3 sommets
 *        3 quadrangles a 4 sommets
 *
 * @sa Elem_geom_base Elem_geom
 */
template <typename _SIZE_>
class Prisme_32_64  : public Elem_geom_base_32_64<_SIZE_>
{

  Declare_instanciable_32_64(Prisme_32_64);

public :

  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;
  using DoubleVect_t = DoubleVect_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  inline int face_sommet(int i, int j) const override;
  inline int face_sommet0(int i) const;
  inline int face_sommet1(int i) const;
  inline int face_sommet2(int i) const;
  inline int face_sommet3(int i) const;
  inline int face_sommet4(int i) const;

  inline int nb_som() const override { return 6; }
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  inline bool est_structure() const override { return false; }
  const Nom& nom_lml() const override;

  int contient(const ArrOfDouble& pos, int_t elem) const override;
  int contient(const SmallArrOfTID_t& soms, int_t elem) const override;
  inline Type_Face type_face(int=0) const override;
  void calculer_volumes(DoubleVect_t& vols) const override;

  void reordonner() override ;

  int nb_type_face() const override { return 2; }
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
};


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::face_sommet(int face, int sommet) const
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


/*! @brief Renvoie le nombre de faces du type specifie que possede l'element geometrique.
 *
 *     Un Prisme a 2 types de faces: quadrangle ou triangle.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de faces de type i
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::nb_faces(int i) const
{
  assert(i < nb_type_face());
  switch(i)
    {
    case 0:
      return 3; //les quadrangles
    case 1:
      return 2; //les triangles
    default :
      Cerr << "Error, a prism has 2 types of faces and not " << i << finl;
      Process::exit();
      return -1;
    }

}


/*! @brief Renvoie le nombre de sommets des faces du type specifie.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces de type i
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::nb_som_face(int i) const
{
  assert(i < nb_type_face());
  switch(i)
    {
    case 0:
      return 4;
    case 1:
      return 3;
    default :
      Cerr << "Error, a prism has 2 types of faces and not " << i << finl;
      Process::exit();
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 0
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 0
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::face_sommet0(int i) const
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
      Cerr << "Error in Prisme_32_64<_SIZE_>::face_sommet0" << finl;
      Process::exit();
      // Pour qq compilos :
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 1
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 1
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::face_sommet1(int i) const
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
      Cerr << "Error in Prisme_32_64<_SIZE_>::face_sommet1" << finl;
      Process::exit();
      // Pour qq compilos :
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 2
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 2
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::face_sommet2(int i) const
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
      Cerr << "Error in Prisme_32_64<_SIZE_>::face_sommet2" << finl;
      Process::exit();
      // Pour qq compilos :
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 3
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) le numero du i-ieme sommet de la face 3
 */
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::face_sommet3(int i) const
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
template <typename _SIZE_>
inline int Prisme_32_64<_SIZE_>::face_sommet4(int i) const
{
  //face_sommet0(0)=0;
  //face_sommet0(1)=1;
  //face_sommet0(2)=2;
  assert(i>=0);
  assert(i<3);
  return i+3;
}

/*! @brief Renvoie le i-ieme type de face.
 *
 * Un prisme a 2 types de face: quadrangle et triangle
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) le type de la face i
 */
template <typename _SIZE_>
inline Type_Face Prisme_32_64<_SIZE_>::type_face(int i) const
{
  assert(i<=1);
  switch(i)
    {
    case 0:
      return Type_Face::quadrangle_3D;
    case 1:
      return Type_Face::triangle_3D;
    default :
      Cerr << "Error, a prism has 2 types of faces and not " << i << finl;
      Process::exit();
      return Type_Face::quadrangle_3D;
    }
}


using Prisme = Prisme_32_64<int>;
using Prisme_64 = Prisme_32_64<trustIdType>;

#endif
