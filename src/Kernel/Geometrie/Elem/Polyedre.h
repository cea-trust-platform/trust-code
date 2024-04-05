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

#ifndef Polyedre_included
#define Polyedre_included

#include <Poly_geom_base.h>

/*! @brief Classe Polyedre Cette represente l'element geometrique Polyedre.
 *
 * Un polyedre est un element defini par ses faces de type Faces::polygone_3D
 *
 * @sa Poly_geom_base Elem_geom
 */
template <typename _SIZE_>
class Polyedre_32_64 : public Poly_geom_base_32_64<_SIZE_>
{

  Declare_instanciable_32_64(Polyedre_32_64);

public :
  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using BigArrOfInt_t = TRUSTArray<int, _SIZE_>; // Very specific to polyhedrons: a big array of small values
  using IntTab_t = ITab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallAOTID_T<_SIZE_>;
  using DoubleVect_t = DVect_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  void calculer_un_centre_gravite(const int_t elem,DoubleVect& xp) const override;
  // Fonctions d'acces aux membres prives:
  inline int face_sommet(int i, int j) const override;
  inline int nb_som() const override;
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  const Nom& nom_lml() const override;

  int contient(const ArrOfDouble& pos, int_t elem) const override;
  int contient(const SmallArrOfTID_t& soms, int_t elem) const override;
  inline Type_Face type_face(int=0) const override;
  void calculer_volumes(DoubleVect_t& vols) const override;
  void calculer_centres_gravite(DoubleTab_t& xp) const override;

  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local, int_t elem) const override;
  void affecte_connectivite_numero_global(const ArrOfInt_t& Nodes, const ArrOfInt_t& FacesIndex,const ArrOfInt_t& PolyhedronIndex,IntTab_t& les_elems);
  inline int get_nb_som_face_max() const  { return nb_som_face_max_ ;  }
  int_t get_somme_nb_faces_elem() const override;

  inline const ArrOfInt_t& getPolyhedronIndex() const     { return PolyhedronIndex_; }
  inline const ArrOfInt_t& getElemIndex() const override  { return PolyhedronIndex_; }
  void remplir_Nodes_glob(ArrOfInt_t& Nodes_glob,const IntTab_t& les_elems ) const;
  void ajouter_elements(const Elem_geom_base_32_64<_SIZE_>& new_elem, const IntTab_t& new_elems, IntTab_t& les_elems);
  void build_reduced(Elem_geom_32_64<_SIZE_>& type_elem, const ArrOfInt_t& elems_sous_part) const override;
  void compute_virtual_index() override;

protected:
  // Members herited from top classes:
  using Objet_U::dimension;
  using Elem_geom_base_32_64<_SIZE_>::mon_dom;
  // FacesIndex_[f] premier indice de sommet de la face f dans Nodes
  using Poly_geom_base_32_64<_SIZE_>::FacesIndex_;
  using Poly_geom_base_32_64<_SIZE_>::nb_som_elem_max_;
  using Poly_geom_base_32_64<_SIZE_>::nb_face_elem_max_;

  /// PolyhedronIndex_[ele] gives the index of the first face of the polyedron 'ele' in FaceIndex_ (see base class)
  ArrOfInt_t PolyhedronIndex_;
  /// Nodes_[s] local index (in the reference frame of an element) of the vertex of a given face, where as elem(ele,s) will give the global index.
  BigArrOfInt_t Nodes_;

  int nb_som_face_max_;
};


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
template <typename _SIZE_>
inline int Polyedre_32_64<_SIZE_>::face_sommet(int face, int sommet) const
{
  BLOQUE;
  return -1;
}


/*! @brief Renvoie le nombre de sommets d'un Polyedre
 *
 * @return (int) le nombre de sommets maximum d'un polyedre
 */
template <typename _SIZE_>
inline int Polyedre_32_64<_SIZE_>::nb_som() const
{
  return Poly_geom_base_32_64<_SIZE_>::get_nb_som_elem_max();
}


/*! @brief Renvoie le nombre de faces du type specifie que possede l'element geometrique.
 *
 *     Un Polyedre a 1 type de faces: polygon_3D
 *
 * @param (int i) le type de face
 */
template <typename _SIZE_>
inline int Polyedre_32_64<_SIZE_>::nb_faces(int i) const
{
  assert(i==0);
  switch(i)
    {
    case 0:
      return Poly_geom_base_32_64<_SIZE_>::get_nb_face_elem_max();
    default :
      Cerr << "Error, a polyhedron has 1 type of faces and not " << i << finl;
      Process::exit();
      return -1;
    }
}


/*! @brief Renvoie le nombre maximum de sommets des faces du type specifie.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces de type i
 */
template <typename _SIZE_>
inline int Polyedre_32_64<_SIZE_>::nb_som_face(int i) const
{
  assert(i==0);
  return get_nb_som_face_max();
}


/*! @brief Renvoie le i-ieme type de face.
 *
 * Un polyedre a 2 types de face: quadrangle et triangle
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) le type de la face i
 */
template <typename _SIZE_>
inline Type_Face Polyedre_32_64<_SIZE_>::type_face(int i) const
{
  assert(i<=0);
  switch(i)
    {
    case 0:
      return Faces::polygone_3D;
    default :
      Cerr << "Error, a polyhedron has 1 type of faces and not " << i << finl;
      Process::exit();
      return Faces::quadrangle_3D;
    }
}


using Polyedre = Polyedre_32_64<int>;
using Polyedre_64 = Polyedre_32_64<trustIdType>;

#endif
