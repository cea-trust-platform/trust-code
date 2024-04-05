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

#include <Triangle.h>
#include <Domaine.h>

Implemente_instanciable_32_64(Triangle_32_64,"Triangle",Elem_geom_base_32_64<_T_>);

/*! @brief des faces du triangle de reference: 3 faces de deux sommets.
 *
 *   La face i est la face opposee au sommet i
 *  (voir get_tab_faces_sommets_locaux)
 *
 */
static int faces_sommets_triangle[3][2] =
{
  { 1, 2 },
  { 2, 0 },
  { 0, 1 }
};

template <typename _SIZE_>
Sortie& Triangle_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}

template <typename _SIZE_>
Entree& Triangle_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}


/*! @brief Renvoie le nom LML d'un triangle = "PRISM6".
 *
 * @return (Nom&) toujours egal a "PRISM6"
 */
template <typename _SIZE_>
const Nom& Triangle_32_64<_SIZE_>::nom_lml() const
{
  static Nom nom="PRISM6";
  if (dimension==3) nom="TRIANGLE_3D";
  return nom;
}


/*! @brief Renvoie 1 si l'element ielem du domaine associe a l'element geometrique contient le point
 *
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int ielem) le numero de l'element du domaine dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element ielem 0 sinon
 */
template <typename _SIZE_>
int Triangle_32_64<_SIZE_>::contient(const ArrOfDouble& pos, int_t ielem) const
{
  assert(pos.size_array()==2);
  const Domaine_t& dom=this->mon_dom.valeur();
  assert(ielem<dom.nb_elem_tot());
  int_t som0 = dom.sommet_elem(ielem,0);
  int_t som1 = dom.sommet_elem(ielem,1);
  int_t som2 = dom.sommet_elem(ielem,2);
  assert((som0>=0) && (som0<dom.nb_som_tot()));
  assert((som1>=0) && (som1<dom.nb_som_tot()));
  assert((som2>=0) && (som2<dom.nb_som_tot()));
  double prod,p0,p1,p2;

  // On regarde tout d'abord si le point cherche n'est pas un des
  // sommets du triangle
  // GF on retire le test pour etre coherent avec tetraedre contient et pour eviter des erreurs dans Champ_implementation_P1::form_function, ou il n'y a pas ce test
  /*
    if( (est_egal(dom.coord(som0,0),pos(0)) && est_egal(dom.coord(som0,1),pos(1)))
    || (est_egal(dom.coord(som1,0),pos(0)) && est_egal(dom.coord(som1,1),pos(1)))
    || (est_egal(dom.coord(som2,0),pos(0)) && est_egal(dom.coord(som2,1),pos(1))) )
    return 1;

  */
  // Attention les sommets sont ranges de facon quelconque.
  // Il faut donc determiner le sens (trigo ou anti trigo) pour la numerotation :
  // Calcul de prod = 01 vectoriel 02 selon z
  // prod > 0 : sens trigo
  // prod < 0 : sens anti trigo
  prod = (dom.coord(som1,0)-dom.coord(som0,0))*(dom.coord(som2,1)-dom.coord(som0,1))
         - (dom.coord(som1,1)-dom.coord(som0,1))*(dom.coord(som2,0)-dom.coord(som0,0));
  double signe;
  if (prod >= 0)
    signe = 1;
  else
    signe = -1;
  // Calcul de p0 = 0M vectoriel 1M selon z
  p0 = (pos[0]-dom.coord(som0,0))*(pos[1]-dom.coord(som1,1))
       - (pos[1]-dom.coord(som0,1))*(pos[0]-dom.coord(som1,0));
  p0 *= signe;
  // Calcul de p1 = 1M vectoriel 2M selon z
  p1 = (pos[0]-dom.coord(som1,0))*(pos[1]-dom.coord(som2,1))
       - (pos[1]-dom.coord(som1,1))*(pos[0]-dom.coord(som2,0));
  p1 *= signe;
  // Calcul de p2 = 2M vectoriel 0M selon z
  p2 = (pos[0]-dom.coord(som2,0))*(pos[1]-dom.coord(som0,1))
       - (pos[1]-dom.coord(som2,1))*(pos[0]-dom.coord(som0,0));
  p2 *= signe;
  double epsilon=std::fabs(prod)*Objet_U::precision_geom;
  if ((p0>-epsilon) && (p1>-epsilon) && (p2>-epsilon))
    return 1;
  else
    return 0;
}


/*! @brief Renvoie 1 si les sommets specifies par le parametre "pos" sont les sommets de l'element "element" du domaine associe a
 *
 *     l'element geometrique.
 *
 * @param (IntVect& pos) les numeros des sommets a comparer avec ceux de l'elements "element"
 * @param (int element) le numero de l'element du domaine dont on veut comparer les sommets
 * @return (int) 1 si les sommets passes en parametre sont ceux de l'element specifie, 0 sinon
 */
template <typename _SIZE_>
int Triangle_32_64<_SIZE_>::contient(const SmallArrOfTID_t& som, int_t element ) const
{
  const Domaine_t& domaine=this->mon_dom.valeur();
  if((domaine.sommet_elem(element,0)==som[0])&&
      (domaine.sommet_elem(element,1)==som[1])&&
      (domaine.sommet_elem(element,2)==som[2]))
    return 1;
  else
    return 0;
}

/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
template <typename _SIZE_>
void Triangle_32_64<_SIZE_>::calculer_volumes(DoubleVect_t& volumes) const
{
  const Domaine_t& domaine=this->mon_dom.valeur();
  const DoubleTab_t& coord = domaine.coord_sommets();
  DoubleTab pos(3,dimension);
  int_t size_tot = domaine.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int_t num_poly=0; num_poly<size_tot; num_poly++)
    {
      for (int i=0; i<3; i++)
        {
          int_t Si = domaine.sommet_elem(num_poly,i);
          for (int j=0; j<dimension; j++)
            pos(i,j) = coord(Si,j);
        }
      volumes[num_poly] = aire_triangle(pos);
    }
}

/*! @brief Calcule les normales aux faces des elements du domaine associe.
 *
 * @param (IntTab& face_sommets) les numeros des sommets des faces dans la liste des sommets du domaine associe
 * @param (DoubleTab& face_normales)
 */
template <typename _SIZE_>
void Triangle_32_64<_SIZE_>::calculer_normales(const IntTab_t& Face_sommets, DoubleTab_t& face_normales) const
{
  const Domaine_t& domaine_geom = this->mon_dom.valeur();
  const DoubleTab_t& les_coords = domaine_geom.coord_sommets();
  int_t nbfaces = Face_sommets.dimension(0);
  double x1,y1;
  int_t n0,n1;
  for (int_t numface=0; numface<nbfaces; numface++)
    {
      n0 = Face_sommets(numface,0);
      n1 = Face_sommets(numface,1);
      x1 = les_coords(n0,0)-les_coords(n1,0);
      y1 = les_coords(n0,1)-les_coords(n1,1);
      face_normales(numface,0) = -y1;
      face_normales(numface,1) = x1;
    }
}

/*! @brief voir ElemGeomBase::get_tab_faces_sommets_locaux
 *
 */
template <typename _SIZE_>
int Triangle_32_64<_SIZE_>::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(3,2);
  for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
      faces_som_local(i,j) = faces_sommets_triangle[i][j];
  return 1;
}


template class Triangle_32_64<int>;
#if INT_is_64_ == 2
template class Triangle_32_64<trustIdType>;
#endif

