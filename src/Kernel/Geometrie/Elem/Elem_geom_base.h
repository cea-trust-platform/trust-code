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

#ifndef Elem_geom_base_included
#define Elem_geom_base_included

#include <TRUSTTabs_forward.h>
#include <Faces.h>

template<class _SIZE_> class Domaine_32_64;

/*! @brief Classe Elem_geom_base Cette classe est la classe de base pour la definition d'elements
 *
 *     geometriques constituant un maillage (i.e. un Domaine geometrique)
 *     Un element geometrique a un Domaine associe auquel il fournit
 *     les routines de base pour la manipulation des ses elements.
 *     (Un domaine n'a qu'un seul type d'element geometrique)
 *
 * @sa Hexaedre Prisme Rectangle Segment Tetraedre Triangle, Domaine, Classe abstraite, Methodes abstraites:, int face_sommet(int i, int j) const, int nb_som() const, int nb_faces(int=0) const, int nb_som_face(int=0) const, int est_structure() const, const Nom& nom_lml() const, int contient(const ArrOfDouble&, int ) const, int contient(const ArrOfInt&, int ) const, Type_Face type_face(int=0) const, void calculer_volumes(DoubleVect& ) const, void calculer_normales(const IntTab& , DoubleTab& ) const
 */
template <typename _SIZE_>
class Elem_geom_base_32_64 : public Objet_U
{

  Declare_base_32_64(Elem_geom_base_32_64);

public:

  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using IntVect_t = IVect_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallAOTID_T<_SIZE_>;
  using ArrOfDouble_t= AODouble_T<_SIZE_>;
  using DoubleVect_t = DVect_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Faces_t = Faces_32_64<_SIZE_>;


  virtual void creer_faces_elem(Faces_t& ,int_t ,Type_Face ) const;
  inline void creer_faces_elem(Faces_t& ,int_t ) const;
  /// Renvoie le numero du j-ieme sommet de la i-ieme face de l'element
  virtual int face_sommet(int i, int j) const=0;
  inline void associer_domaine(const Domaine_32_64<int_t>& dom) { mon_dom = dom; }
  /// Nb of vertices for the element
  virtual int nb_som() const=0;
  /// Nb of faces for the element
  virtual int nb_faces(int=0) const=0;
  /// Nb of vertices for one face of the element
  virtual int nb_som_face(int=0) const=0;
  virtual bool est_structure() const=0;
  virtual const Nom& nom_lml() const=0;
  /// DOes the element 'elem' contains the point 'pos'
  virtual int contient(const ArrOfDouble& pos, int_t elem) const=0;
  /// Renvoie 1 si les sommets specifies par le parametre "pos" sont les sommets de l'element "element" du domaine associe
  virtual int contient(const SmallArrOfTID_t& soms, int_t elem) const=0;
  /// Nb of face types of the elemnt (for example 2 for a prism)
  virtual int nb_type_face() const;
  virtual int num_face(int face, Type_Face& type) const;
  /// Type of the face of the element - face_typ < nb_type_face()
  virtual Type_Face type_face(int face_typ=0) const=0;
  /// Compute all centers of mass of all elements in the domain
  virtual void calculer_centres_gravite(DoubleTab_t& ) const ;
  virtual void reordonner() { }
  /// Compute vols of all elements in the domain
  virtual void calculer_volumes(DoubleVect_t& vols) const =0;
  virtual void calculer_normales(const IntTab_t& faces_sommets , DoubleTab_t& face_normales) const;
  virtual int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const;
  virtual void get_tab_aretes_sommets_locaux(IntTab& aretes_som_local) const;

protected:
  REF(Domaine_t) mon_dom;
};

/*! @brief Cree les faces de l'element du domaine specifie.
 *
 * @param (Faces_t& faces) les faces de l'elements a creer
 * @param (int elem) le numero de element du domaine dont veut creer les faces
 */
template <typename _SIZE_>
inline void Elem_geom_base_32_64<_SIZE_>::creer_faces_elem(Faces_t& faces, _SIZE_ elem) const
{
  assert(nb_type_face() == 1);
  creer_faces_elem(faces, elem, type_face());
}

using Elem_geom_base = Elem_geom_base_32_64<int>;
using Elem_geom_base_64 = Elem_geom_base_32_64<trustIdType>;

#endif
