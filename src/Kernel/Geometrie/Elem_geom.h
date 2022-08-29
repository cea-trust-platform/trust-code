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

#ifndef Elem_geom_included
#define Elem_geom_included




#include <Elem_geom_base.h>


/*! @brief Classe Elem_geom Classe generique de la hierarchie des elements geometriques, un objet
 *
 *      Elem_geom peut referencer n'importe quel objet derivant de
 *      Elem_geom_base.
 *      La plupart des methodes appellent les methodes de l'objet Elem_geom_base
 *      sous-jacent via la methode valeur() declaree grace a la macro
 * Declare_deriv().;
 *
 */
Declare_deriv(Elem_geom_base);
class Elem_geom : public DERIV(Elem_geom_base)
{

  Declare_instanciable(Elem_geom);

public:

  inline void associer_zone(const Zone& );
  inline void creer_faces_elem(Faces& ,int ) const;
  inline void creer_faces_elem(Faces& ,int ,Type_Face ) const;
  inline void face_sommet(int i, int j) const;
  inline int nb_som() const;
  inline int nb_faces(int=0) const;
  inline int nb_som_face(int=0) const;
  inline int est_structure() const;
  inline const Nom& nom_lml() const;
  inline int contient(const ArrOfDouble&, int ) const;
  inline int contient(const ArrOfInt&, int ) const;
  inline int nb_type_face() const;
  inline Type_Face type_face(int=0) const;
  inline void calculer_centres_gravite(DoubleTab& ) const;
  inline void reordonner() ;
  inline void calculer_volumes(DoubleVect& ) const;
  inline void calculer_normales(const IntTab& , DoubleTab& ) const;

};

/*! @brief Appel a l'objet sous-jacent Associe l'element geometrique a sa zone.
 *
 * @param (Zone&  z) la zone dont fait partie l'element
 */
inline void Elem_geom::associer_zone(const Zone&  z)
{
  valeur().associer_zone(z);
}


/*! @brief Appel a l'objet sous-jacent Renvoie le j-ieme sommet de la i-ieme face de
 *
 *     l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 */
inline void Elem_geom::face_sommet(int i, int j) const
{
  valeur().face_sommet(i, j);
}


/*! @brief Appel a l'objet sous-jacent Cree les faces de l'element specifie de la zone.
 *
 *     (sans specifier le type de face a creer)
 *
 * @param (Faces& les_faces) les faces de l'element
 * @param (int num_elem) le numero de l'element de la zone dont on doit creer les faces
 */
inline void Elem_geom::creer_faces_elem(Faces& les_faces,int num_elem) const
{
  valeur().creer_faces_elem(les_faces,num_elem);
}


/*! @brief Appel a l'objet sous-jacent Cree les faces de l'element specifie de la zone
 *
 *     en specifiant le type de de face a creer.
 *
 * @param (Faces& les_faces) les faces de l'element
 * @param (int num_elem) le numero de l'element de la zone dont on doit creer les faces
 * @param (Type_Face typ) le type de face a creer
 */
inline void Elem_geom::creer_faces_elem(Faces& les_faces,int num_elem,Type_Face typ) const
{
  valeur().creer_faces_elem(les_faces,num_elem,typ);
}


/*! @brief Appel a l'objet sous-jacent Calcule les centres de gravites de tous les elements
 *
 *     de la zone geometrique associee.
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des des centres de gravites.
 */
inline void Elem_geom::calculer_centres_gravite(DoubleTab& xp) const
{
  valeur().calculer_centres_gravite(xp);
}


/*! @brief Appel a l'objet sous-jacent Calcule les volumes des elements de la zone associee.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements de la zone
 */
inline void Elem_geom::calculer_volumes(DoubleVect& volumes) const
{
  valeur().calculer_volumes(volumes);
}

/*! @brief Appel a l'objet sous-jacent Calcule les normales aux faces des elements de la zone associee.
 *
 * @param (DoubleTab& face_sommets) les numeros des sommets des faces dans la liste des sommets de la zone
 * @param (DoubleTab& face_normales) le vecteur contenant les normales aux faces des elements de la zone
 */
inline void Elem_geom::calculer_normales(const IntTab& face_sommets, DoubleTab& face_normales) const
{
  valeur().calculer_normales(face_sommets,face_normales);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de faces du type specifie
 *
 *     que possede l'element geometrique.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de face du type specifie dans l'element geometrique courant
 */
inline int Elem_geom::nb_faces(int i) const
{
  assert(i<nb_type_face());
  return valeur().nb_faces(i);
}


/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de sommets de l'element
 *
 * @return (int) le nombre de sommets de l'element
 */
inline int Elem_geom::nb_som() const
{
  return valeur().nb_som();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de sommets des faces
 *
 *     du type specifie.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces du type specifie
 */
inline int Elem_geom::nb_som_face(int i) const
{
  assert(i<nb_type_face());
  return valeur().nb_som_face(i);
}


/*! @brief Appel a l'objet sous-jacent Renvoie 1 si l'element geometrique est structure,
 *
 *             0 sinon
 *
 * @return (int) renvoie 1 si l'element geometrique est structure, 0 sinon
 */
inline int Elem_geom::est_structure() const
{
  return valeur().est_structure();
}


/*! @brief Appel a l'objet sous-jacent Renvoie le nom de l'element geometrique
 *
 *     au format LML.
 *
 * @return (Nom&) le nom de l'element geometrique au format LML
 */
inline const Nom& Elem_geom::nom_lml() const
{
  return valeur().nom_lml();
}


/*! @brief Appel a l'objet sous-jacent Renvoie 1 si l'element i de la zone associee a
 *
 *               l'element geometrique contient le point
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int i) le numero de l'element de la zone dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element i 0 sinon
 */
inline int Elem_geom::contient(const ArrOfDouble& pos, int i ) const
{
  return valeur().contient(pos, i);
}


/*! @brief Appel a l'objet sous-jacent Renvoie 1 si les sommets specifies par le parametre "pos"
 *
 *     sont les sommets de l'element i de la zone associee a
 *     l'element geometrique.
 *
 * @param (IntVect& pos) les numeros des sommets a comparer avec ceux de l'elements i
 * @param (int i) le numero de l'element de la zone dont on veut comparer les sommets
 * @return (int) 1 si les sommets passes en parametre sont ceux de l'element i, 0 sinon
 */
inline int Elem_geom::contient(const ArrOfInt& pos, int i ) const
{
  return valeur().contient(pos, i);
}


/*! @brief Appel a l'objet sous-jacent Renvoie le i-eme type de face de l'element
 *
 *     geometrique.
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) un objet Type_Face correspondant au type demande
 */
inline Type_Face Elem_geom::type_face(int i) const
{
  assert(i<nb_type_face());
  return valeur().type_face(i);
}

/*! @brief Appel a l'objet sous-jacent Reordonne les sommets du maillage.
 *
 */
inline void Elem_geom::reordonner()
{
  valeur().reordonner();
}


/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de type de face
 *
 *     de l'element geometrique.
 *
 */
inline int Elem_geom::nb_type_face() const
{
  return valeur().nb_type_face();
}

#endif
