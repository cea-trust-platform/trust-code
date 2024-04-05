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

#include <Point.h>
#include <Domaine.h>

Implemente_instanciable_32_64(Point_32_64,"Point",Elem_geom_base_32_64<_T_>);
// XD point points point 0 Point as class-daughter of Points.

/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
template<typename _SIZE_>
Sortie& Point_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
template<typename _SIZE_>
Entree& Point_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}

/*! @brief Renvoie le nom LML d'un triangle = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
template<typename _SIZE_>
const Nom& Point_32_64<_SIZE_>::nom_lml() const
{
  static Nom nom="POINT";
  return nom;
}


/*! @brief Renvoie 1 si l'element ielem du dom associe a l'element geometrique contient le point
 *
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int element) le numero de l'element du dom dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element "element" 0 sinon
 */
template<typename _SIZE_>
int Point_32_64<_SIZE_>::contient(const ArrOfDouble& pos, int_t element ) const
{
  assert(pos.size_array()==this->dimension);

  const Domaine_t& dom=this->mon_dom.valeur();
  const IntTab_t& elem=dom.les_elems();
  int ok=1;
  for (int d=0; (d<3)&&(ok==1); d++)
    {
      double ps = dom.coord(elem(element,0), d);
      double pv = pos[d];
      if( !est_egal(ps,pv)) ok=0;
    }
  return ok;
}

/*! @brief Renvoie 1 si les sommets specifies par le parametre "pos" sont les sommets de l'element "element" du dom associe a
 *
 *     l'element geometrique.
 *
 * @param (IntVect& pos) les numeros des sommets a comparer avec ceux de l'elements "element"
 * @param (int element) le numero de l'element du dom dont on veut comparer les sommets
 * @return (int) 1 si les sommets passes en parametre sont ceux de l'element specifie, 0 sinon
 */
template<typename _SIZE_>
int Point_32_64<_SIZE_>::contient(const SmallArrOfTID_t& pos, int_t element ) const
{
  abort();

  assert(pos.size_array()==1);
  const Domaine_t& dom=this->mon_dom.valeur();
  if((dom.sommet_elem(element,0)==pos[0])&&
      (dom.sommet_elem(element,1)==pos[1]))
    return 1;
  else
    return 0;
}


/*! @brief voir ElemGeomBase::get_tab_faces_sommets_locaux
 */
template<typename _SIZE_>
int Point_32_64<_SIZE_>::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(1,1);
  faces_som_local[0]=0;
  return 1;
}



template class Point_32_64<int>;
#if INT_is_64_ == 2
template class Point_32_64<trustIdType>;
#endif

