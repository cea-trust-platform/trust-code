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

#ifndef Domaine_EF_included
#define Domaine_EF_included

#include <Elem_EF_base.h>
#include <TRUST_Deriv.h>
#include <Domaine_VF.h>

class Geometrie;

/*! @brief class Domaine_EF
 *
 *  	Classe instanciable qui derive de Domaine_VF.
 *  	Cette classe contient les informations geometriques que demande
 *  	la methode des Volumes Elements Finis (element de Crouzeix-Raviart)
 *  	La classe porte un certain nombre d'informations concernant les faces
 *  	Dans cet ensemble de faces on fait figurer aussi les faces du bord et
 *       des joints. Pour manipuler les faces on distingue 2 categories:
 *            - les faces non standard qui sont sur un joint, un bord ou qui sont
 *              internes tout en appartenant a un element du bord
 *            - les faces standard qui sont les faces internes n'appartenant pas
 *              a un element du bord
 *       Cette distinction correspond au traitement des conditions aux limites:les
 *       faces standard ne "voient pas" les conditions aux limites.
 *       L'ensemble des faces est numerote comme suit:
 *            - les faces qui sont sur un Domaine_joint apparaissent en premier
 *     	       (dans l'ordre du vecteur les_joints)
 *    	     - les faces qui sont sur un Domaine_bord apparaissent ensuite
 * 	       (dans l'ordre du vecteur les_bords)
 *   	     - les faces internes non standard apparaissent ensuite
 *            - les faces internes standard en dernier
 *       Finalement on trouve regroupees en premier toutes les faces non standard
 *       qui vont necessiter un traitement particulier
 *       On distingue deux types d'elements
 *            - les elements non standard : ils ont au moins une face de bord
 *            - les elements standard : ils n'ont pas de face de bord
 *       Les elements standard (resp. les elements non standard) ne sont pas ranges
 *       de maniere consecutive dans l'objet Domaine. On utilise le tableau
 *       rang_elem_non_std pour acceder de maniere selective a l'un ou
 *       l'autre des types d'elements
 *
 */

class Domaine_EF: public Domaine_VF
{
  Declare_instanciable(Domaine_EF);
public:
  void typer_elem(Domaine& domaine_geom) override;
  void discretiser() override;
  void swap(int, int, int);
  void reordonner(Faces&) override;
  void modifier_pour_Cl(const Conds_lim&) override;

  inline const Elem_EF_base& type_elem() const { return type_elem_.valeur(); }
  inline int nb_elem_Cl() const { return nb_elem() - nb_elem_std_; }
  inline int nb_faces_joint() const { return 0; /*    return nb_faces_joint_;    A FAIRE */ }
  inline int nb_faces_std() const { return nb_faces_std_; }
  inline int nb_elem_std() const { return nb_elem_std_; }
  inline double carre_pas_du_maillage() const { return h_carre; }
  inline double carre_pas_maille(int i) const { return h_carre_(i); }
  inline IntVect& rang_elem_non_std() { return rang_elem_non_std_; }

  inline const IntVect& rang_elem_non_std() const { return rang_elem_non_std_; }
  inline int oriente_normale(int face_opp, int elem2) const;
  void calculer_volumes_entrelaces();
  void calculer_volumes_sommets(const Domaine_Cl_dis_base& zcl);
  virtual void calculer_IPhi(const Domaine_Cl_dis_base& zcl);
  virtual void calculer_Bij(DoubleTab& bij_);
  virtual void calculer_Bij_gen(DoubleTab& bij);

  //  inline const DoubleVect& volumes_sommets() const { return volumes_sommets_; }
  inline const DoubleVect& volumes_thilde() const { return volumes_thilde_; }
  inline const DoubleVect& volumes_sommets_thilde() const { return volumes_sommets_thilde_; }
  inline const DoubleVect& porosite_sommet() const { return porosite_sommets_; }
  inline DoubleVect& set_porosite_sommet() { return porosite_sommets_; }

  void calculer_h_carre();
  void calculer_porosites_sommets();
  inline const DoubleTab& Bij() const { return Bij_; }
  inline const DoubleTab& Bij_thilde() const { return Bij_thilde_; }
  inline const DoubleTab& IPhi() const { return IPhi_; }
  inline const DoubleTab& IPhi_thilde() const { return IPhi_thilde_; }

  virtual void verifie_compatibilite_domaine();

protected:
  DoubleTab IPhi_, IPhi_thilde_;

private:
  DoubleVect porosite_sommets_, volumes_sommets_thilde_, volumes_thilde_;
  //  OWN_PTR(Champ_Don_base) champ_porosite_sommets_,champ_porosite_lu_;
  DoubleTab Bij_, Bij_thilde_;                         // stockage des matrice Bije

  double h_carre = 1.e30;			 // carre du pas du maillage
  DoubleVect h_carre_;			// carre du pas d'une maille
  OWN_PTR(Elem_EF_base) type_elem_;                  // type de l'element de discretisation

  Sortie& ecrit(Sortie& os) const;
  IntVect orientation_;
};

inline int Domaine_EF::oriente_normale(int face_opp, int elem2) const
{
  if (face_voisins(face_opp, 0) == elem2)
    return 1;
  else
    return -1;
}

#endif /* Domaine_EF_included */
