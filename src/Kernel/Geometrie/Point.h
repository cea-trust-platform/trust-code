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

#ifndef Point_included
#define Point_included

#include <Elem_geom_base.h>

/*! @brief Classe Point Cette classe represente l'element geometrique segment.
 *
 *     Un Point a 1 face et 1 sommets.
 *
 * @sa Elem_geom_base Elem_geom, .CONTRAINTES, .INVARIANTS, .HTML, .EPS
 */
class Point : public Elem_geom_base
{

  Declare_instanciable(Point);

public :

  inline int face_sommet(int i, int j) const override;
  inline int face_sommet0(int i) const;

  inline int nb_som() const override;
  inline int nb_faces(int=0) const override;
  inline int nb_som_face(int=0) const override;
  inline int est_structure() const override;
  const Nom& nom_lml() const override;
  int contient(const ArrOfDouble&, int ) const override;
  int contient(const ArrOfInt&, int ) const override;
  inline Type_Face type_face(int=0) const override;
  void reordonner() override ;
  void calculer_volumes(DoubleVect& ) const override;
  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override;

};


/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
inline int Point::face_sommet(int i, int j) const
{
  assert(i<1);
  switch(i)
    {
    case 0:
      return face_sommet0(j);
    default :
      return -1;
    }
}


/*! @brief Renvoie le numero du i-ieme sommet de la face 0 LE CODE NE FAIT PAS CE QU'ON ATTENDRAIT.
 *
 * @param (int i) le numero du sommet a renvoyer
 * @return (int) renvoie toujours 0 (si i=0)
 */
inline int Point::face_sommet0(int i) const
{
  // face_sommet0(0)=0;
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A segment has not " << i << " types of face" << finl;
      exit();
    }
  return 0;
}



/*! @brief Renvoie le nombre de sommets d'un segment: 2.
 *
 * @return (int) le nombre de sommets d'un segment
 */
inline int Point::nb_som() const
{
  return 1;
}


/*! @brief Renvoie le nombre de faces du type specifie que possede l'element geometrique.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de faces de type i
 */
inline int Point::nb_faces(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A segment has not " << i << " types of face" << finl;
      exit();
    }
  return 1;
}


/*! @brief Renvoie le nombre de sommets des faces du type specifie.
 *
 *     LE CODE NE FAIT PAS CE QU'ON ATTENDRAIT.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de sommets des faces de type i
 */
inline int Point::nb_som_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A segment has not " << i << " types of face" << finl;
      exit();
    }
  return 1;
}


/*! @brief Renvoie toujours 1, car l'element geometrique Point est de type structure.
 *
 * @return (int) 1 si l'element est de type structure 0 sinon
 */
inline int Point::est_structure() const
{
  return 1;
}


/*! @brief Renvoie le i-ieme type de face.
 *
 * Un segment n'a qu'un seul type de face.
 *
 * @param (int i) le rang du type de face a renvoyer
 * @return (Type_Face) un type de face
 */
inline Type_Face Point::type_face(int i) const
{
  assert(i==0);
  if(i!=0)
    {
      Cerr << "A segment has not " << i << " types of face" << finl;
      exit();
    }
  return Faces::point_1D;
}

#endif
