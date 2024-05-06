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

#ifndef Octree_Double_included
#define Octree_Double_included

#include <Octree_Int.h>
#include <TRUSTTab.h>

/*! @brief : Un octree permettant de chercher dans l'espace des elements ou des points decrits par des coordonnees reeles.
 *
 * Cet objet est base sur Octree_Int.
 *   Astuces:
 *    Pour chercher des points a epsilon pres on peut faire:
 *     1)  build_nodes(coord, include_virt, 0.)
 *       suivi de
 *         search_elements_box(center, epsilon, elements);
 *
 *     2)  build_nodes(coord, include_virt, epsilon)
 *       suivi de
 *         search_elements(x,y,z,...)
 *     La premiere solution prend plus de temps pour construire l'octree mais la recherche est plus rapide
 *     La deuxieme, c'est inverse... et on peut choisir epsilon pour chaque point.
 *
 */
template <typename _SIZE_>
class Octree_Double_32_64
{
public:
  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using ArrOfDouble_t = AODouble_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;

  void reset();

  template<class _TAB_TYPE_>
  void build_elements(const _TAB_TYPE_& coords, const IntTab_t& elements, const double epsilon, const bool include_virtual);

  void build_nodes(const DoubleTab_t& coords, const bool include_virtual, const double epsilon = 0.);
  int_t search_elements(double x, double y, double z, int_t& index) const;
  int_t search_elements_box(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, ArrOfInt_t& elements) const;
  static int_t search_nodes_close_to(double x, double y, double z, const DoubleTab_t& coords, ArrOfInt_t& node_list, double epsilon);
  int_t search_elements_box(const ArrOfDouble& center, const double radius, ArrOfInt_t& elements) const;
  static int_t search_nodes_close_to(const ArrOfDouble& point, const DoubleTab_t& coords, ArrOfInt_t& node_list, double epsilon);
  int dimension() const
  {
    assert(dim_ > 0);
    return dim_;
  }
  inline const ArrOfInt_t& floor_elements() const  { return octree_int_.floor_elements(); }

protected:
  inline bool integer_position(double x, int direction, int& ix) const;
  inline bool integer_position_clip(double xmin, double xmax, int& x0, int& x1, int direction) const;

  template<class _TAB_TYPE_>
  void compute_origin_factors(const _TAB_TYPE_& coords, const double epsilon, const int include_virtual);

  Octree_Int_32_64<_SIZE_> octree_int_;
  // Ces deux tableaux sont toujours de taille 3 par commodite
  ArrOfDouble origin_, factor_;
  int dim_ = 0;
};

//////////////////// INLINE METHODS /////////////////////////////

/*! @brief Convertit une coordonnee reelle en coordonnee entiere pour l'octree_int
 *
 * Valeur de retour: 1 si ok, 0 si coordonnee hors de l'octree
 */
template <typename _SIZE_>
inline bool Octree_Double_32_64<_SIZE_>::integer_position(double x, int direction, int& ix) const
{
  const double coord_max = (double) Octree_Int_32_64<_SIZE_>::coord_max_;
  double rnd_x = (x - origin_[direction]) * factor_[direction];
  // 0.49 permet d'accepter une coordonnee x egale a xmin ou xmax de l'octree,
  //  sinon pour un octree cree a partir de sommets, il y a un risque
  //  de ne pas trouver les coordonnees des points qu'on avait mis au bord de l'octree.
  if (rnd_x >= -0.49 && rnd_x <= coord_max + 0.49)
    {
      ix = (int) floor(rnd_x + 0.5);
      return true;
    }
  return false;
}

// Valeur de retour: 1 s'il y a une intersection non vide avec l'octree, 0 sinon
template <typename _SIZE_>
inline bool Octree_Double_32_64<_SIZE_>::integer_position_clip(double xmin, double xmax,
                                                               int& x0, int& x1,
                                                               int direction) const
{
  const double coord_max = (double) Octree_Int_32_64<_SIZE_>::coord_max_;
  xmin = (xmin - origin_[direction]) * factor_[direction];
  xmax = (xmax - origin_[direction]) * factor_[direction];
  // pas de marge ici comme on cherche avec une boite, l'epsilon est deja
  // dans la dimension de la boite.
  if (xmin > coord_max || xmax < 0.)
    return false;
  if (xmin < .0)
    x0 = 0;
  else
    x0 = (int) (floor(xmin+0.5));
  if (xmax > coord_max)
    x1 = Octree_Int_32_64<_SIZE_>::coord_max_;
  else
    x1 = (int) (floor(xmax+0.5));
  return true;
}


//////////////////// TEMPLATE METHOD IMPL ////////////////////////

#include <Octree_Double.tpp>

using Octree_Double = Octree_Double_32_64<int>;
using Octree_Double_64 = Octree_Double_32_64<trustIdType>;

#endif
