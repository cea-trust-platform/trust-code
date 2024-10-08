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

#ifndef Faces_included
#define Faces_included

#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Motcle;
#include <Domaine_forward.h>

/*! @brief enum Type_Face Les differents types geometriques de faces.
 * @sa Faces
 */
enum class Type_Face
{
  vide_0D,
  point_1D,
  point_1D_axi,
  segment_2D,
  segment_2D_axi,
  triangle_3D,
  quadrilatere_2D_axi,
  quadrangle_3D,
  quadrangle_3D_axi,
  polygone_3D
};


/*! @brief Classe Faces Faces decrit un ensemble de faces par leur type (point ,segment, triangle ou quadrangle),
 * leurs sommets et leurs elements adjacents.
 *
 * @sa IntTab Frontiere
 */
template <typename _SIZE_>
class Faces_32_64 : public Objet_U
{
  Declare_instanciable_32_64(Faces_32_64);

public:

  using int_t = _SIZE_;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;
  using DoubleVect_t = DoubleVect_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;
  using DomaineAxi1d_t = DomaineAxi1d_32_64<_SIZE_>;


  inline Type_Face type_face() const {   return type_face_; }
  inline int_t nb_faces() const {  return sommets.dimension(0); }
  /// @brief Renvoie le nombre total de Faces i (reelles et virt) sur le proc courant
  inline int_t nb_faces_tot() const { return sommets.dimension_tot(0); }
  inline int nb_som_faces() const;

  inline int_t sommet(int_t, int) const;
  inline int_t& sommet(int_t, int);
  /// Renvoie le tableau des sommets de toutes les faces
  inline const IntTab_t& les_sommets() const { return sommets; }
  inline IntTab_t& les_sommets() { return sommets; }

  Type_Face type(const Motcle&) const;
  Motcle& type(const Type_Face&) const;

  void ajouter(const IntTab_t&);
  void typer(const Motcle&);
  void typer(const Type_Face&);

  int_t dimensionner(int_t);

  inline int_t voisin(int_t, int) const;
  inline int_t& voisin(int_t, int);
  /// @brief Renvoie le tableau des voisins (des faces).
  inline IntTab_t& voisins() { return faces_voisins; }
  inline const IntTab_t& voisins() const { return faces_voisins; }

  void completer(int_t face, int_t num_elem);

  inline void associer_domaine(const Domaine_t& z)  { mon_dom=z; }
  inline const Domaine_t& domaine() const { return mon_dom.valeur(); }

  void calculer_surfaces(DoubleVect_t& surf) const;
  void calculer_centres_gravite(DoubleTab_t& xv) const;
  void reordonner();
  IntVect_t& compare(const Faces_32_64& other_fac, IntVect_t& renum);
  void initialiser_faces_joint(int_t nb_faces_joints);
  void initialiser_sommets_faces_joint(int_t nb_faces_joints);

  Entree& lit(Entree&);
  Sortie& ecrit(Sortie&) const ;

  // Static because used by others (like DecoupeBord):
  static void Calculer_centres_gravite(DoubleTab_t& xv, Type_Face type_face_, const DoubleTab_t& coord,  const IntTab_t& sommet);

private :
  // Useful tool methods:
  int_t ppsf(int_t face, int nb_som) const;
  bool same_face(int_t f1, const Faces_32_64& faces2, int_t f2, int nb_som) const;

  Type_Face type_face_ = Type_Face::vide_0D;
  int nb_som_face = 0;  ///< Number of vertices per face
  IntTab_t sommets;
  IntTab_t faces_voisins;
  OBS_PTR(Domaine_t) mon_dom;
};


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face
 *
 * @param (int i) indice de la face du sommet a renvoyer
 * @param (int j) indice du sommet a renvoyer
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
template <typename _SIZE_>
inline typename Faces_32_64<_SIZE_>::int_t Faces_32_64<_SIZE_>::sommet(int_t i, int j) const
{
  return sommets(i,j);
}

/*! @brief Renvoie une reference sur le numero du j-ieme sommet de la i-ieme face
 *
 * @param (int i) indice de la face du sommet a renvoyer
 * @param (int j) indice du sommet a renvoyer
 * @return (int&) reference sur le numero du j-ieme sommet de la i-ieme face
 */
template <typename _SIZE_>
inline typename Faces_32_64<_SIZE_>::int_t& Faces_32_64<_SIZE_>::sommet(int_t i, int j)
{
  return sommets(i,j);
}

/// @brief Renvoie le nombre de sommet par face.
template <typename _SIZE_>
inline int Faces_32_64<_SIZE_>::nb_som_faces() const
{
  if (nb_som_face!=-1)
    return nb_som_face;
  else
    return (int)sommets.dimension(1);
}


/*! @brief Renvoie le numero du i-ieme voisin de face.
 *
 * @param (int face) le numero de la face dont on cherche un voisin
 * @param (int i) l'indice du voisin a renvoyer
 * @return (int) le numero i-ieme voisin de face
 */
template <typename _SIZE_>
inline typename Faces_32_64<_SIZE_>::int_t Faces_32_64<_SIZE_>::voisin(int_t face, int i) const
{
  return faces_voisins(face,i);
}

/*! @brief Renvoie une reference le numero du i-ieme voisin de face.
 *
 * @param (int face) le numero de la face dont on cherche un voisin
 * @param (int i) l'indice du voisin a renvoyer
 * @return (int&) reference sur le numero du i-ieme voisin de face
 */
template <typename _SIZE_>
inline typename Faces_32_64<_SIZE_>::int_t& Faces_32_64<_SIZE_>::voisin(int_t face, int i)
{
  return faces_voisins(face,i);
}


using Faces = Faces_32_64<int>;
using Faces_64 = Faces_32_64<trustIdType>;

#endif
