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

#ifndef Point_included
#define Point_included

#include <Elem_geom_base.h>

/*! @brief Classe Point Cette classe represente l'element geometrique segment.
 *
 *     Un Point a 1 face et 1 sommets.
 *
 * @sa Elem_geom_base Elem_geom, .CONTRAINTES, .INVARIANTS, .HTML, .EPS
 */
template <typename _SIZE_>
class Point_32_64 : public Elem_geom_base_32_64<_SIZE_>
{

  Declare_instanciable_32_64(Point_32_64);

public :

  using int_t = _SIZE_;
  using IntTab_t = ITab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallAOTID_T<_SIZE_>;
  using DoubleVect_t = DVect_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  inline int face_sommet(int i, int j) const override;
  inline int face_sommet0(int i) const;

  inline int nb_som() const override { return 1; }
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  inline bool est_structure() const override { return true; }
  const Nom& nom_lml() const override;
  int contient(const ArrOfDouble& pos, int_t elem) const override;
  int contient(const SmallArrOfTID_t& soms, int_t elem) const override;
  inline Type_Face type_face(int=0) const override;
  void reordonner() override { }
  void calculer_volumes(DoubleVect_t& vols) const override { vols = 1.; }
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
};


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
template <typename _SIZE_>
inline int Point_32_64<_SIZE_>::face_sommet(int i, int j) const
{
  assert(i<1);
  switch(i)
    {
    case 0:
      return face_sommet0(j);
    default :
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 0 LE CODE NE FAIT PAS CE QU'ON ATTENDRAIT.
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) renvoie toujours 0 (si i=0)
 */
template <typename _SIZE_>
inline int Point_32_64<_SIZE_>::face_sommet0(int i) const
{
  assert(i==0);
  return 0;
}


/*! @brief Renvoie le nombre de faces du type specifie que possede l'element geometrique.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de faces de type i
 */
template <typename _SIZE_>
inline int Point_32_64<_SIZE_>::nb_faces(int i) const
{
  assert(i==0);
  return 1;
}


/*! @brief Renvoie le nombre de sommets des faces du type specifie.
 *
 *     LE CODE NE FAIT PAS CE QU'ON ATTENDRAIT.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces de type i
 */
template <typename _SIZE_>
inline int Point_32_64<_SIZE_>::nb_som_face(int i) const
{
  assert(i==0);
  return 1;
}


/*! @brief Renvoie le i-ieme type de face.
 *
 * Un segment n'a qu'un seul type de face.
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) un type de face
 */
template <typename _SIZE_>
inline Type_Face Point_32_64<_SIZE_>::type_face(int i) const
{
  assert(i==0);
  return Faces::point_1D;
}

using Point = Point_32_64<int>;
using Point_64 = Point_32_64<trustIdType>;


#endif
