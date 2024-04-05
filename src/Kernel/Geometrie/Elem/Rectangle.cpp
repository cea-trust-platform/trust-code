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

#include <Rectangle.h>
#include <Domaine.h>

static int faces_sommets_rectangle[4][2] =
{
  { 0, 2 },
  { 0, 1 },
  { 1, 3 },
  { 2, 3 }
};

Implemente_instanciable_32_64(Rectangle_32_64,"Rectangle",Elem_geom_base_32_64<_T_>);

template <typename _SIZE_>
Sortie& Rectangle_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}

template <typename _SIZE_>
Entree& Rectangle_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}

/*! @brief Reordonne les sommets du rectangle.
 *
 */
template <typename _SIZE_>
void Rectangle_32_64<_SIZE_>::reordonner()
{
  if (reordonner_elem()==-1)
    {
      Cerr << "It is expected to treat rectangles but" << finl;
      Cerr << "one of the elements is not rectangle!" << finl;
      Cerr << "Check your mesh." << finl;
      Process::exit();
    }
}

/*! @brief Reordonne et verifie que l'on a bien des rectangles.
 */
template <typename _SIZE_>
int Rectangle_32_64<_SIZE_>::reordonner_elem()
{
  Domaine_t& domaine=mon_dom.valeur();
  const Domaine_t& dom=domaine;
  IntTab_t& elem=domaine.les_elems();

  SmallArrOfTID_t S(4);
  SmallArrOfTID_t NS(4);
  DoubleTab co(4,2);
  int i,j;
  double xmin, ymin;
  const int_t nb_elem=domaine.nb_elem();
  for (int_t num_poly=0; num_poly<nb_elem; num_poly++)
    {
      NS=-1;
      for(i=0; i<4; i++)
        {
          S[i] = elem(num_poly,i);
          for(j=0; j<2; j++)
            co(i,j) = dom.coord(S[i], j);
        }

      xmin=std::min(co(0, 0), co(1, 0));
      xmin=std::min(xmin, co(2, 0));
      ymin=std::min(co(0, 1), co(1, 1));
      ymin=std::min(ymin, co(2, 1));

      for(i=0; i<4; i++)
        if ( est_egal(co(i, 0),xmin) && est_egal(co(i, 1),ymin))
          NS[0]=S[i];
      for(i=0; i<4; i++)
        if ( !est_egal(co(i, 0),xmin) && est_egal(co(i, 1),ymin))
          NS[1]=S[i];
      for(i=0; i<4; i++)
        if ( est_egal(co(i, 0),xmin) && !est_egal(co(i, 1),ymin))
          NS[2]=S[i];
      for(i=0; i<4; i++)
        if ( !est_egal(co(i, 0),xmin) && !est_egal(co(i, 1),ymin))
          NS[3]=S[i];

      // Si un sommet vaut -1, ce n'est pas un Rectangle !
      if (min_array(NS)==-1)
        {
          Cerr << "The element " << num_poly << " is not a rectangle." << finl;
          return -1;
        }
      // Sinon on remplit elem
      for(i=0; i<4; i++)
        elem(num_poly, i)=NS[i];
    }
  return 0;
}

/*! @brief Renvoie le nom LML d'un rectangle = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
template <typename _SIZE_>
const Nom& Rectangle_32_64<_SIZE_>::nom_lml() const
{
  static Nom nom="VOXEL8";
  if (dimension==3) nom="QUADRANGLE_3D";
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
int Rectangle_32_64<_SIZE_>::contient(const ArrOfDouble& pos, int_t element ) const
{
  assert(pos.size_array()==2);
  const Domaine_t& dom=mon_dom.valeur();
  int_t som0 = dom.sommet_elem(element,0),
        som3 = dom.sommet_elem(element,3);
  if (    inf_ou_egal(dom.coord(som0,0),pos[0]) && inf_ou_egal(pos[0],dom.coord(som3,0))
          && inf_ou_egal(dom.coord(som0,1),pos[1]) && inf_ou_egal(pos[1],dom.coord(som3,1)) )
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
int Rectangle_32_64<_SIZE_>::contient(const SmallArrOfTID_t& som, int_t element ) const
{
  const Domaine_t& domaine=mon_dom.valeur();
  if((domaine.sommet_elem(element,0)==som[0])&&
      (domaine.sommet_elem(element,1)==som[1])&&
      (domaine.sommet_elem(element,2)==som[2])&&
      (domaine.sommet_elem(element,3)==som[3]))
    return 1;
  else
    return 0;
}


/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
template <typename _SIZE_>
void Rectangle_32_64<_SIZE_>::calculer_volumes(DoubleVect_t& volumes) const
{
  if (dimension==3)
    {
      Cerr << "Rectangle_32_64<_SIZE_>::calculer_volumes is not supported for 3D yet." << finl;
      Process::exit();
    }
  const Domaine_t& dom=mon_dom.valeur();
  double dx,dy;
  int_t S1,S2,S3;

  int_t size_tot = dom.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int_t num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = dom.sommet_elem(num_poly,0);
      S2 = dom.sommet_elem(num_poly,1);
      S3 = dom.sommet_elem(num_poly,2);
      dx = dom.coord(S2,0) - dom.coord(S1,0);
      dy = dom.coord(S3,1) - dom.coord(S1,1);
      volumes[num_poly]= dx*dy;
    }
}


/*! @brief Calcule les normales aux faces des elements du domaine associe.
 *
 * @param (IntTab& face_sommets) les numeros des sommets des faces dans la liste des sommets du domaine associe
 * @param (DoubleTab& face_normales)
 */
template <typename _SIZE_>
void Rectangle_32_64<_SIZE_>::calculer_normales(const IntTab_t& Face_sommets, DoubleTab_t& face_normales) const
{
  const Domaine_t& domaine_geom = mon_dom.valeur();
  const DoubleTab_t& les_coords = domaine_geom.coord_sommets();
  int_t nbfaces = Face_sommets.dimension(0);
  double x1,y1;
  int_t n0,n1;
  for (int_t numface=0; numface<nbfaces; numface++)
    {
      n0 = Face_sommets(numface,0);
      n1 = Face_sommets(numface,1);
      x1 = les_coords(n0,0) - les_coords(n1,0);
      y1 = les_coords(n0,1) - les_coords(n1,1);
      face_normales(numface,0) = -y1;
      face_normales(numface,1) = x1;
    }
}

/*! @brief voir ElemGeomBase::get_tab_faces_sommets_locaux
 *
 */
template <typename _SIZE_>
int Rectangle_32_64<_SIZE_>::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(4,2);
  for (int i=0; i<4; i++)
    for (int j=0; j<2; j++)
      faces_som_local(i,j) = faces_sommets_rectangle[i][j];
  return 1;
}


template class Rectangle_32_64<int>;
#if INT_is_64_ == 2
template class Rectangle_32_64<trustIdType>;
#endif

