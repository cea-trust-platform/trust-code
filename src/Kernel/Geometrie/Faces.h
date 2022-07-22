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

#ifndef Faces_included
#define Faces_included

#include <TRUSTTab.h>
#include <Ref_Zone.h>
class Motcle;

/*! @brief enum Type_Face Les differents types geometriques de faces.
 *
 * @sa Faces
 */

/*! @brief Classe Faces Faces decrit un ensemble de faces par leur type (point ,segment,
 *
 *     triangle ou quadrangle), leurs sommets et leurs elements
 *     adjacents.
 *
 * @sa IntTab Frontiere
 */
class Faces : public Objet_U
{
  Declare_instanciable(Faces);

public:

  enum Type_Face
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

  inline Type_Face type_face() const;
  inline int nb_faces() const;
  inline int nb_faces_tot() const;
  inline int nb_som_faces() const;
  inline int sommet(int, int) const;
  inline int& sommet(int, int);
  inline IntTab& les_sommets();
  inline const IntTab& les_sommets() const;
  Type_Face type(const Motcle&) const;
  Motcle& type(const Type_Face&) const;
  void ajouter(const IntTab&);
  void typer(const Motcle&);
  void typer(const Type_Face&);
  int dimensionner(int);
  inline int voisin(int, int) const;
  inline int& voisin(int, int);
  inline IntTab& voisins();
  inline const IntTab& voisins() const;
  void completer(int, int);
  inline void associer_zone(const Zone&);
  inline const Zone& zone() const;
  void calculer_surfaces(DoubleVect& ) const;
  void calculer_centres_gravite(DoubleTab& ) const;
  void reordonner();
  IntVect& compare(const Faces&, IntVect&);
  void initialiser_faces_joint(int);
  void initialiser_sommets_faces_joint(int);

  Entree& lit(Entree&);
  Sortie& ecrit(Sortie&) const ;

private :

  Type_Face type_face_;
  int nb_som_face;
  IntTab sommets;
  IntTab faces_voisins;
  REF(Zone) ma_zone;
};

typedef Faces::Type_Face Type_Face;

extern void
calculer_centres_gravite(DoubleTab& xv,
                         Type_Face type_face_,
                         const DoubleTab& coord,
                         const IntTab& sommet);
/*! @brief Renvoie le type des Faces
 *
 * @return (Type_Face) le type des Faces
 */
inline Type_Face Faces::type_face() const
{
  return type_face_;
}

/*! @brief Renvoie le nombre de Faces
 *
 * @return (int) le nombre de Faces
 */
inline int Faces::nb_faces() const
{
  return sommets.dimension(0);
}

/*! @brief Renvoie le nombre total de Faces i.
 *
 * e. Le nombre de faces reelles et virtuelles
 *     sur le processeur courant.
 *
 * @return (int) le nombre total de Faces
 */
inline int Faces::nb_faces_tot() const
{
  return sommets.dimension_tot(0);
}

/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face
 *
 * @param (int i) indice de la face du sommet a renvoyer
 * @param (int j) indice du sommet a renvoyer
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
inline int Faces::sommet(int i, int j) const
{
  return sommets(i,j);
}

/*! @brief Renvoie une reference sur le numero du j-ieme sommet de la i-ieme face
 *
 * @param (int i) indice de la face du sommet a renvoyer
 * @param (int j) indice du sommet a renvoyer
 * @return (int&) reference sur le numero du j-ieme sommet de la i-ieme face
 */
inline int& Faces::sommet(int i, int j)
{
  return sommets(i,j);
}

/*! @brief Renvoie le nombre de sommet par face.
 *
 * @return (int) le nombre de sommet par face
 */
inline int Faces::nb_som_faces() const
{
  if (nb_som_face!=-1)
    return nb_som_face;
  else
    return sommets.dimension(1);
}

/*! @brief Renvoie le tableau des sommets de toutes les faces
 *
 * @return (IntTab&) le tableau des sommets de toutes les faces
 */
inline const IntTab& Faces::les_sommets() const
{
  return sommets;
}
inline IntTab& Faces::les_sommets()
{
  return sommets;
}

/*! @brief Renvoie le numero du i-ieme voisin de face.
 *
 * @param (int face) le numero de la face dont on cherche un voisin
 * @param (int i) l'indice du voisin a renvoyer
 * @return (int) le numero i-ieme voisin de face
 */
inline int Faces::voisin(int face, int i) const
{
  return faces_voisins(face,i);
}

/*! @brief Renvoie une reference le numero du i-ieme voisin de face.
 *
 * @param (int face) le numero de la face dont on cherche un voisin
 * @param (int i) l'indice du voisin a renvoyer
 * @return (int&) reference sur le numero du i-ieme voisin de face
 */
inline int& Faces::voisin(int face, int i)
{
  return faces_voisins(face,i);
}

/*! @brief Associe les faces a une Zone.
 *
 * @param (Zone& z) la zone a laquelle l'objet s'associe
 */
inline void Faces::associer_zone(const Zone& z)
{
  ma_zone=z;
}

/*! @brief Renvoie la zone associee.
 *
 * @return (Zone&) la zone associee
 */
inline const Zone& Faces::zone() const
{
  return ma_zone.valeur();
}

/*! @brief Renvoie le tableau des voisins (des faces).
 *
 * @return (IntTab&) le tableau des voisins (des faces)
 */
inline IntTab& Faces::voisins()
{
  return faces_voisins;
}
inline const IntTab& Faces::voisins() const
{
  return faces_voisins;
}

#endif
