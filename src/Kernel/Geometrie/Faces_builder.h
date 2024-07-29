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

#ifndef Faces_builder_included
#define Faces_builder_included

#include <TRUST_Ref.h>
#include <TRUSTTab.h>
#include <Domaine_forward.h>
#include <Static_Int_Lists.h>

/*! @brief classe outil pour construire les faces d'un domaine
 * (utilisee uniquement pour creer les tableau des faces reelles)
 *
 */
class Faces_builder
{
public:
  Faces_builder();
  void reset();
  void creer_faces_reeles(Domaine& domaine, const Static_Int_Lists& connect_som_elem, Faces& les_faces, IntTab& elem_faces);

  // This static method is also used by 64b objects like Raffiner_Simplexes:
  template <typename _SIZE_>
  static int chercher_face_element(const ITab_T<_SIZE_>& elem_som, const IntTab& faces_element_ref,
                                   const SmallAOTID_T<_SIZE_>& une_face, const _SIZE_ elem);

private:
  static int ajouter_une_face(const ArrOfInt& une_face, const int elem0, const int elem1, IntTab& faces_sommets, IntTab& faces_voisins);

  int chercher_face_element(const ArrOfInt& une_face, const int elem) const;

  void check_erreur_faces(const char *message, const ArrOfInt& liste_faces) const;
  void creer_faces_frontiere(const int nb_voisins_attendus, Frontiere& frontiere, IntTab& faces_sommets, IntTab& faces_voisins, IntTab& elem_faces) const;
  void creer_faces_internes(IntTab& faces_sommets, IntTab& elem_faces, IntTab& faces_voisins) const;
  void identification_groupe_faces(Groupe_Faces& groupe_int, const IntTab& elem_faces) const;

  const IntTab& les_elements() const { return *les_elements_ptr_; }
  const Static_Int_Lists& connectivite_som_elem() const { return *connectivite_som_elem_ptr_; }
  const IntTab& faces_element_reference(int elem) const; // redonne les faces pour un element, dans le cas des polyhedres cela depend du numero de l'element.


  // Tous les membres suivants sont initialises dans creer_faces_reeles:

  // Raccourci vers les elements du domaine
  const IntTab * les_elements_ptr_;
  // La connectivite elements-sommets (pour chaque sommet, liste des
  // elements adjacents, y compris les sommets et elements virtuels)
  const Static_Int_Lists * connectivite_som_elem_ptr_;
  /*! @brief des faces de l'element de reference (voir elem_geom_base::get_tab_faces_sommets_locaux)
   *
   */
  IntTab faces_element_reference_old_;
  int is_polyedre_;
  // pour check_erreur_faces :
  REF(Domaine)   ref_domaine_;
  REF(IntTab) faces_sommets_;
  REF(IntTab) face_elem_;
};
#endif
