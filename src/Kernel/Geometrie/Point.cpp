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

#include <Point.h>
#include <Domaine.h>

Implemente_instanciable(Point,"Point",Elem_geom_base);
// XD point points point 0 Point as class-daughter of Points.





/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Point::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
Entree& Point::readOn(Entree& s )
{
  return s;
}

/*! @brief Reordonne les sommets du segment.
 *
 */
void Point::reordonner()
{
  return;
}


/*! @brief Renvoie le nom LML d'un triangle = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
const Nom& Point::nom_lml() const
{
  static Nom nom="POINT";
  return nom;
}


/*! @brief Renvoie 1 si l'element ielem du domaine associe a l'element geometrique contient le point
 *
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int element) le numero de l'element du domaine dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element "element" 0 sinon
 */
int Point::contient(const ArrOfDouble& pos, int element ) const
{
  // on  teste si la positon correspond au sommet

  assert(pos.size_array()==dimension);


  const Domaine& domaine=mon_dom.valeur();
  const Domaine& dom=domaine;
  const IntTab& elem=domaine.les_elems();
  for (int s=0; s<1; s++)
    {
      int ok=1;
      for (int d=0; (d<3)&&(ok==1); d++)
        {
          double ps = dom.coord(elem(element,s), d);
          double pv=pos[d];
          //Cerr<<ps<<" "<<pv<<finl;
          if( !est_egal(ps,pv)) ok=0;
        }
      if (ok==1)
        return 1;
    }
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
int Point::contient(const ArrOfInt& pos, int element ) const
{
  abort();
  assert(pos.size_array()==1);
  const Domaine& domaine=mon_dom.valeur();
  if((domaine.sommet_elem(element,0)==pos[0])&&
      (domaine.sommet_elem(element,1)==pos[1]))
    return 1;
  else
    return 0;
}

/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
void Point::calculer_volumes(DoubleVect& volumes) const
{
  volumes=1;
}
/*! @brief voir ElemGeomBase::get_tab_faces_sommets_locaux
 *
 */
int Point::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(1,1);
  faces_som_local[0]=0;
  return 1;
}

