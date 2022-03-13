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
// File:        Op_Diff_VEF_Face_Penalise.h
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_VEF_Face_Penalise_included
#define Op_Diff_VEF_Face_Penalise_included
#include <Op_Diff_VEF_Face.h>
#include <Zone_Cl_VEF.h>
#include <TRUSTList.h>
#include <Domaine.h>

class Op_Diff_VEF_Face_Penalise : public Op_Diff_VEF_Face
{

  Declare_instanciable(Op_Diff_VEF_Face_Penalise);

public :
  /* Fonction membre calculant la diffusion penalisee.
   * Elle est surchargee par rapport a sa classe mere.
   */
  DoubleTab& ajouter(const DoubleTab& inconnue, DoubleTab& resu) const override;
  DoubleTab& calculer(const DoubleTab& inconnue, DoubleTab& resu) const override;

protected:

  /* Fonction membre qui renvoie dans la liste Voisinage, l'ensemble
   * des numeros des faces voisines de la face Numero_face.
   * On entend par faces voisines, des faces appartenant aux triangles
   * contenant la face Numero_face.
   * Rem: Voisinage contient egalement la face Numero_face.
   */
  void voisinage(const int& Numero_face, IntList& Voisinage) const;

  /* Fonction membre qui renvoie dans la liste Voisinage,
   * l'ensemble des faces qui constitue le voisinage de la liste
   * Ensemble_faces.
   * Rem: Voisinage contient egalement les elements de Ensemble_faces.
   */
  void voisinage(const IntList& Ensemble_faces, IntList& Voisinage) const;

  /* Fonction membre qui renvoie le signe adequat pour
   * le calcul des interactions des fonctions de base des faces
   * Face1 et Face2 le long d'une autre face.
   */
  double signe(const int& Face1, const int& Face2 ) const;

  /* Fonction membre qui renvoie le coefficient de penalisation
   * a passer sur chaque arete du maillage primaire.
   */
  double coefficient_penalisation(const int& Numero_face) const;

  /* Fonction membre qui renvoie la liste Faces_communes des faces
   * qui appartiennent au voisinage de Face1 ET de Face2
   */
  void faces_communes(const int& Face1,const int& Face2,
                      IntList& Face_commune) const;

  /* Fonction membre qui renvoie la liste Liste_reduite qui est
   * la soustraction des listes Liste1, Liste2.
   * Rem: cette fonction regarde dynamiquement les longueurs de
   * chacune des listes Liste1 et Liste2 avant de s'executer
   */
  void reduction(const IntList& Liste1,const IntList& Liste2,
                 IntList& Liste_reduite) const;

  /* Fonction membre qui renvoie le numero de l'element commun
   * a Face1 et a Face2 s'il existe et renvoie -1 sinon
   */
  int element_commun(const int& Face1,const int& Face2) const;

  /* Fonction membre qui retourne le numero de la 3eme face
   * d'un element, si Face1 et Face2 appartiennent au meme element.
   * Sinon retourne -1.
   */
  int autre_face(const int& Face1, const int& Face2) const;

  /* Fonction membre qui retourne le diametre de l'element Element
   * Rem: on considere que l'on opere en 2D.
   */
  inline double diametre(const int& Element) const;

  /* Fonction membre qui renvoie la longueur de la face Face
   * Rem: on considere que l'on opere en 2D.
   */
  inline double longueur(const int& Face) const;


private:
  /* Fonction membre qui retourne la Zone_VEF du domaine. */
  inline const Zone_VEF& zone_vef() const;

  /* Fonction membre qui retourne le Domaine du probleme. */
  inline const Domaine&  domaine() const;

  /* Fonction membre qui renvoie la zone du probleme. */
  inline const Zone& zone() const;

  /* Fonction membre qui renvoie la zone de conditions aux limitex. */
  inline const Zone_Cl_VEF& zone_cl() const;

};




inline double Op_Diff_VEF_Face_Penalise::longueur(const int& Face) const
{
  double x_sommet1,x_sommet2;
  double y_sommet1,y_sommet2;

  int sommet1 = zone_vef().face_sommets(Face,0);
  int sommet2 = zone_vef().face_sommets(Face,1);

  x_sommet1 = domaine().coord(sommet1,0);
  y_sommet1 = domaine().coord(sommet1,1);

  x_sommet2 = domaine().coord(sommet2,0);
  y_sommet2 = domaine().coord(sommet2,1);

  return sqrt( pow(x_sommet1 - x_sommet2,2) + pow(y_sommet1 - y_sommet2,2) );
}

inline double Op_Diff_VEF_Face_Penalise::diametre(const int& Element) const
{
  int face1,face2,face3;
  double longueur_face1,longueur_face2,longueur_face3;
  double longueur_max,diametre_element;

  face1 = zone_vef().elem_faces(Element,0);
  face2 = zone_vef().elem_faces(Element,1);
  face3 = zone_vef().elem_faces(Element,2);

  longueur_face1 = longueur(face1);
  longueur_face2 = longueur(face2);
  longueur_face3 = longueur(face3);

  longueur_max = longueur_face1 >= longueur_face2 ? longueur_face1 :
                 longueur_face2;
  diametre_element = longueur_face3 >= longueur_max ? longueur_face3 :
                     longueur_max;

  return diametre_element;
}

inline const Zone_VEF& Op_Diff_VEF_Face_Penalise::zone_vef() const
{
  return la_zone_vef.valeur();
}

inline const Zone_Cl_VEF& Op_Diff_VEF_Face_Penalise::zone_cl() const
{
  return la_zcl_vef.valeur();
}

inline const Zone&  Op_Diff_VEF_Face_Penalise::zone() const
{
  return zone_vef().zone();
}

inline const Domaine&  Op_Diff_VEF_Face_Penalise::domaine() const
{
  return zone().domaine();
}


#endif
