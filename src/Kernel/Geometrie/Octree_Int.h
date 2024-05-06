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
#ifndef Octree_Int_included
#define Octree_Int_included

#include <TRUSTArrays.h>
#include <TRUSTTab.h>
#include <ArrOfBit.h>

template <typename _SIZE_> struct IntBoxData;

/*! @brief : Un octree permettant de retrouver des objets ponctuels ou parallelipipediques dans un espace 1D, 2D ou 3D et des coordonnees entieres
 */
template <typename _SIZE_>
class Octree_Int_32_64
{
public:
  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using ArrOfDouble_t= AODouble_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using ArrsOfInt_t = ArrsOfInt_T<_SIZE_>;
  using AOFlag_ = TRUSTArray<int, _SIZE_>;
  using AOFlagS_ = TRUST_Vector<AOFlag_>;  // A vector of arrays of small values (flags here)


  void build(const int dimension, const IntTab_t& elements_boxes);
  int_t search_elements(int x, int y, int z, int_t& floor_elements_index) const;
  int_t search_elements_box(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax, ArrOfInt_t& elements) const;
  void reset();

  inline const ArrOfInt_t& floor_elements() const { return floor_elements_; }

  // Le plus grand int autorise pour les coordonnees (du type 2^n - 1)
  static const int coord_max_;
  // Premier int de la moitie superieure de l'octree root (si coord_max_=2^n-1, half_width_=2^(n-1))
  static const int root_octree_half_width_;

protected:
  int_t build_octree_recursively(const int octree_center_x, const int octree_center_y, const int octree_center_z,
                                 const int octree_half_width,
                                 const IntTab_t& elements_boxes,
                                 ArrsOfInt_t& vect_elements_list,
                                 const int level,
                                 AOFlagS_& tmp_elem_flags);
  int_t build_octree_floor(const ArrOfInt_t& elements_list);
  int_t search_octree_floor(int x_pos, int y_pos, int z_pos) const;
  void search_elements_box_floor(IntBoxData<_SIZE_>& boxdata, int_t octree_floor_id) const;
  void search_elements_box_recursively(IntBoxData<_SIZE_>& boxdata, int_t octree_id, int cx, int cy, int cz, int half_width) const;

  // Un octree peut etre soit vide, soit subdivise en nb_octrees autres octrees,
  // soit un octree_floor contenant une liste d'elements.
  enum Octree_Type { EMPTY, OCTREE, FLOOR };

  static inline int_t octree_id(int_t index, Octree_Type type);
  static inline int_t octree_index(int_t octree_id, Octree_Type type);
  static inline Octree_Type octree_type(int_t octree_id);

  // Octree_id du cube principal : peut etre EMPTY, OCTREE ou FLOOR
  int_t root_octree_id_;
  // Nombre d'elements stockes (dimension(0) du tableau elements_boxes)
  int_t nb_elements_;
  // Tableau contenant tous les cubes qui sont divises en sous-cubes
  //  octree_structure_(i, j) decrit le contenu du sous-cube j du cube d'index i.
  //  pour 0 <= j < nombre de sous-cubes par cube.
  //  On appelle "octree_id" une valeur X=octree_structure_(i,j) (identifiant octree)
  //  L'octree id encode a la fois le type de l'octree et l'index ou
  //  il se trouve dans les tableaux (voir octree_id(int, Octree_Type))
  IntTab_t octree_structure_;

  // Tableau contenant la liste des elements de chaque sous-cube final non subdivise.
  // Si X < 0, on note i_debut = -X-1.
  // floor_elements_(i_debut) = n = nombre d'elements dans ce sous-cube
  // floor_elements_[i_debut+j] = numero d'un element qui coupe ce sous-cube pour 1 <= j <= n
  ArrOfInt_t floor_elements_;
};

#endif
