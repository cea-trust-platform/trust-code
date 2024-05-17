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

#include <Segment.h>
#include <Domaine.h>

Implemente_instanciable(Segment,"Segment",Elem_geom_base);
// XD segment sonde_base segment 0 Keyword to define the number of probe segment points. The file is arranged in columns.
// XD attr nbr entier nbr 0 Number of probe points of the segment, evenly distributed.
// XD attr point_deb un_point point_deb 0 First outer probe segment point.
// XD attr point_fin un_point point_fin 0 Second outer probe segment point.


static int faces_sommets_segment[2][1] =
{ { 0 } ,{ 1 }  };


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Segment::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
Entree& Segment::readOn(Entree& s )
{
  return s;
}

/*! @brief Reordonne les sommets du segment.
 *
 */
void Segment::reordonner()
{
  Domaine& domaine=mon_dom.valeur();
  IntTab& elem=domaine.les_elems();
  const Domaine& dom=domaine;
  ArrOfInt S(2);
  int i;
  const int nb_elem=domaine.nb_elem();
  for (int num_poly=0; num_poly<nb_elem; num_poly++)
    {
      for(i=0; i<2; i++)
        S[i] = elem(num_poly,i);

      if(sup_strict(dom.coord(S[0], 0),dom.coord(S[1], 0)))
        {
          int tmp=S[0];
          S[0]=S[1];
          S[1]=tmp;
        }

      for(i=0; i<2; i++)
        elem(num_poly, i)=S[i];
    }
}


/*! @brief Renvoie le nom LML d'un triangle = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
const Nom& Segment::nom_lml() const
{
  static Nom nom="SEGMENT";
  if (dimension==2)
    nom="QUADRANGLE_3D";
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
int Segment::contient(const ArrOfDouble& pos, int element ) const
{
  assert(pos.size_array()==dimension);

  const Domaine& domaine=mon_dom.valeur();
  const Domaine& dom=domaine;
  const IntTab& elem=domaine.les_elems();
  // Test whether OM = a.O1 with O and 1 the extreme points of the seg and M the point to be tested
  double autre_a = 0;
  for (int d=0; d<dimension; d++)
    {
      double O1 = dom.coord(elem(element,1), d) - dom.coord(elem(element,0), d);
      double OM = pos[d] - dom.coord(elem(element,0), d);
      if (!est_egal(O1,0))
        {
          double a = OM/O1;
          // M is outside O1?
          if (a<-Objet_U::precision_geom || a>1+Objet_U::precision_geom) return 0;
          // a is not the same as for another dimension, <=> not aligned
          if (autre_a>0 && !est_egal(a, autre_a)) return 0;
          autre_a = a;
        }
      else if (!est_egal(OM,0)) return 0; // M is not along O1
    }
  return 1;
}


/*! @brief Renvoie 1 si les sommets specifies par le parametre "pos" sont les sommets de l'element "element" du domaine associe a
 *
 *     l'element geometrique.
 *
 * @param (IntVect& pos) les numeros des sommets a comparer avec ceux de l'elements "element"
 * @param (int element) le numero de l'element du domaine dont on veut comparer les sommets
 * @return (int) 1 si les sommets passes en parametre sont ceux de l'element specifie, 0 sinon
 */
int Segment::contient(const ArrOfInt& pos, int element ) const
{
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
void Segment::calculer_volumes(DoubleVect& volumes) const
{
  const Domaine& domaine=mon_dom.valeur();
  const Domaine& dom=domaine;
  double dx;
  int S1,S2;

  int size_tot = domaine.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = domaine.sommet_elem(num_poly,0);
      S2 = domaine.sommet_elem(num_poly,1);
      dx=0;
      for (int i=0; i<dimension; i++)
        {
          double d = dom.coord(S2,i) - dom.coord(S1,i);
          dx+=d*d;
        }

      volumes[num_poly]= sqrt(dx);
    }
}
/*! @brief voir ElemGeomBase::get_tab_faces_sommets_locaux
 *
 */
int Segment::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(2,1);
  for (int j=0; j<2; j++)
    faces_som_local(j,0) = faces_sommets_segment[j][0];
  return 1;
}

