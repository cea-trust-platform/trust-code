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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Faces_builder.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Faces_builder_included
#define Faces_builder_included

#include <Ref_IntTab.h>
#include <TRUSTTrav.h>
#include <Ref_Zone.h>
class Zone;
class Static_Int_Lists;
class Faces;
class Frontiere;

// DESCRIPTION: classe outil pour construire les faces d'une zone
// (utilisee uniquement pour creer les tableau des faces reelles)
class Faces_builder
{
public:
  Faces_builder();
  void reset();
  void creer_faces_reeles(Zone& zone,
                          const Static_Int_Lists& connect_som_elem,
                          Faces&   les_faces,
                          IntTab& elem_faces);

  static int chercher_face_element(const IntTab&    les_elements,
                                   const IntTab&    faces_element_reference,
                                   const ArrOfInt& une_face,
                                   const int     elem);

private:
  static int ajouter_une_face(const ArrOfInt& une_face,
                              const int elem0,
                              const int elem1,
                              IntTab& faces_sommets,
                              IntTab& faces_voisins);

  void check_erreur_faces(const char * message,
                          const ArrOfInt& liste_faces) const;

  int chercher_face_element(const ArrOfInt& une_face,
                            const int     elem) const;

  void creer_faces_frontiere(const int nb_voisins_attendus,
                             Frontiere& frontiere,
                             IntTab& faces_sommets,
                             IntTab& faces_voisins,
                             IntTab& elem_faces) const;
  void creer_faces_internes(IntTab& faces_sommets,
                            IntTab& elem_faces,
                            IntTab& faces_voisins) const;

  const IntTab&            les_elements() const
  {
    return *les_elements_ptr_;
  };
  const Static_Int_Lists& connectivite_som_elem() const
  {
    return *connectivite_som_elem_ptr_;
  };
  const IntTab&            faces_element_reference(int elem) const; // redonne les faces pour un element, dans le cas des polyhedres cela depend du numero de l'element.


  // Tous les membres suivants sont initialises dans creer_faces_reeles:

  // Raccourci vers les elements de la zone
  const IntTab * les_elements_ptr_;
  // La connectivite elements-sommets (pour chaque sommet, liste des
  // elements adjacents, y compris les sommets et elements virtuels)
  const Static_Int_Lists * connectivite_som_elem_ptr_;
  // Description des faces de l'element de reference
  // (voir elem_geom_base::get_tab_faces_sommets_locaux)
  IntTab faces_element_reference_old_;
  int is_polyedre_;
  // pour check_erreur_faces :
  REF(Zone)   ref_zone_;
  REF(IntTab) faces_sommets_;
  REF(IntTab) face_elem_;
};
#endif
