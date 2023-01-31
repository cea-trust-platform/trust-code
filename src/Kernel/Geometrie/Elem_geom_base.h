/****************************************************************************
* Copyright (c) 2023, CEA
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
class Nom;

/*! @brief Classe Elem_geom_base Cette classe est la classe de base pour la definition d'elements
 *
 *     geometriques constituant un maillage (i.e. une Domaine geometrique)
 *     Un element geometrique a une Domaine associe a laquelle il fournit
 *     les routines de base pour la manipulation des ses elements.
 *     (Une domaine n'a qu'un seul type d'element geometrique)
 *
 * @sa Hexaedre Prisme Rectangle Segment Tetraedre Triangle, Domaine, Classe abstraite, Methodes abstraites:, int face_sommet(int i, int j) const, int nb_som() const, int nb_faces(int=0) const, int nb_som_face(int=0) const, int est_structure() const, const Nom& nom_lml() const, int contient(const ArrOfDouble&, int ) const, int contient(const ArrOfInt&, int ) const, Type_Face type_face(int=0) const, void calculer_volumes(DoubleVect& ) const, void calculer_normales(const IntTab& , DoubleTab& ) const
 */
class Elem_geom_base  : public Objet_U
{
  Declare_base(Elem_geom_base);
public:
  virtual void creer_faces_elem(Faces& ,int ,Type_Face ) const;
  inline void creer_faces_elem(Faces& ,int ) const;
  virtual int face_sommet(int i, int j) const=0;
  inline void associer_domaine(const Domaine& );
  virtual int nb_som() const=0;
  virtual int nb_faces(int=0) const=0;
  virtual int nb_som_face(int=0) const=0;
  virtual int est_structure() const=0;
  virtual const Nom& nom_lml() const=0;
  virtual int contient(const ArrOfDouble&, int ) const=0;
  virtual int contient(const ArrOfInt&, int ) const=0;
  virtual int nb_type_face() const;
  virtual int num_face(int face, Type_Face& type) const;
  virtual Type_Face type_face(int=0) const=0;
  virtual void calculer_centres_gravite(DoubleTab& ) const ;
  virtual void reordonner() ;
  virtual void calculer_volumes(DoubleVect& ) const =0;
  virtual void calculer_normales(const IntTab& faces_sommets , DoubleTab& face_normales) const;
  virtual int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const;
  virtual void get_tab_aretes_sommets_locaux(IntTab& aretes_som_local) const;

protected:
  REF(Domaine) mon_dom;
};


/*! @brief Associe l'element geometrique a sa domaine.
 *
 * @param (Domaine& une_domaine) le domaine a associee a l'element
 */
inline void Elem_geom_base::associer_domaine(const Domaine& une_domaine)
{
  mon_dom=une_domaine;
}


/*! @brief Cree les faces de l'element du domaine specifie.
 *
 * @param (Faces& faces) les faces de l'elements a creer
 * @param (int elem) le numero de element du domaine dont veut creer les faces
 */
inline void Elem_geom_base::creer_faces_elem(Faces& faces, int elem) const
{
  assert(nb_type_face() == 1);
  creer_faces_elem(faces, elem, type_face());
}

#endif
