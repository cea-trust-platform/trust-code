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

#include <Hexaedre.h>
#include <Domaine.h>

// Convention de numerotation
//    sommets         faces         5(face z=1)
//      6------7            *------*
//     /|     /|           /| 4   /|
//    2------3 |          *------* |
//    | |    | |          |0|    |3|
//    | 4----|-5          | *----|-*
//    |/     |/           |/  1  |/
//    0------1            *------*
//                       2(face z=0)
static int faces_sommets_hexa[6][4] =
{
  { 0, 2, 4, 6 },
  { 0, 1, 4, 5 },
  { 0, 1, 2, 3 },
  { 1, 3, 5, 7 },
  { 2, 3, 6, 7 },
  { 4, 5, 6, 7 }
};

Implemente_instanciable_32_64(Hexaedre_32_64,"Hexaedre",Elem_geom_base_32_64<_T_>);


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
template <typename _SIZE_>
Sortie& Hexaedre_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
template <typename _SIZE_>
Entree& Hexaedre_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}

/*! @brief Reordonne les sommets de l'hexaedre
 *
 */
template <typename _SIZE_>
void Hexaedre_32_64<_SIZE_>::reordonner()
{
  if (this->reordonner_elem()==-1)
    {
      Cerr << "This mesh is not composed of regular hexahedra\n";
      Cerr << "This seems to be VEF hexahedra (Hexaedre_VEF)\n";
      Cerr << "Check your mesh." << finl;
      Process::exit();
    }
}

/*! @brief Reordonne les sommets de l'hexaedre
 *
 */
template <typename _SIZE_>
int Hexaedre_32_64<_SIZE_>::reordonner_elem()
{
  Domaine_t& domaine = this->mon_dom.valeur();
  const DoubleTab_t& dom_coord = domaine.les_sommets();
  IntTab_t& elem = domaine.les_elems();
  SmallArrOfTID_t S(8);
  SmallArrOfTID_t NS(8);
  double coord[8][3];
  double xmin[3];
  const int_t nb_elem = domaine.nb_elem();
  const int delta[3] = {1, 2, 4};
  int changed_count = 0;

  for (int_t num_poly = 0; num_poly < nb_elem; num_poly++)
    {

      xmin[0] = xmin[1] = xmin[2] = 1e40;
      for(int i=0; i<8; i++)
        {
          int_t s = elem(num_poly,i);
          S[i] = s;
          NS[i] = -1;
          for(int j=0; j<3; j++)
            {
              double x = dom_coord(s, j);
              coord[i][j] = x;
              if (x < xmin[j])
                xmin[j] = x;
            }
        }

      // Pour chaque sommet, chercher rang dans l'element
      // en fonction de ses coordonnees
      for (int i=0; i<8; i++)
        {
          int num_sommet = 0;
          for (int j=0; j<3; j++)
            {
              double x = coord[i][j];
              if (!est_egal(x, xmin[j]))
                num_sommet += delta[j];
            }
          if (NS[num_sommet] == -1)
            NS[num_sommet] = S[i];
          else
            return -1;
        }
      // Est ce bien un hexaedre regulier ?
      if (min_array(NS)==-1)
        return -1;
      // Tous les sommets ont-ils ete trouves ?
      int updated = 0;
      for(int i=0; i<8; i++)
        {
          if (S[i] != NS[i])
            updated = 1;
          elem(num_poly, i) = NS[i];
        }
      if (updated)
        changed_count++;
    }
  changed_count = Process::mp_sum(changed_count);
  if (Process::je_suis_maitre())
    Cerr << "Hexaedre_32_64<_SIZE_>::reordonner : " << changed_count << " elements reversed" << finl;
  return 0;
}

/*! @brief Renvoie le nom LML d'un triangle = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
template <typename _SIZE_>
const Nom& Hexaedre_32_64<_SIZE_>::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}


/*! @brief Renvoie 1 si l'element "elemen" du domaine associe a l'element geometrique contient le point
 *
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int element) le numero de l'element du domaine dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element "element" 0 sinon
 */
template <typename _SIZE_>
int Hexaedre_32_64<_SIZE_>::contient(const ArrOfDouble& pos, int_t element ) const
{
  assert(pos.size_array()==3);
  const Domaine_t& dom=this->mon_dom.valeur();
  int_t som0 = dom.sommet_elem(element,0),
        som7 = dom.sommet_elem(element,7);
  if (    inf_ou_egal(dom.coord(som0,0),pos[0]) && inf_ou_egal(pos[0],dom.coord(som7,0))
          && inf_ou_egal(dom.coord(som0,1),pos[1]) && inf_ou_egal(pos[1],dom.coord(som7,1))
          && inf_ou_egal(dom.coord(som0,2),pos[2]) && inf_ou_egal(pos[2],dom.coord(som7,2)) )
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
int Hexaedre_32_64<_SIZE_>::contient(const SmallArrOfTID_t& som, int_t element ) const
{
  const Domaine_t& domaine=this->mon_dom.valeur();
  if((domaine.sommet_elem(element,0)==som[0])&&
      (domaine.sommet_elem(element,1)==som[1])&&
      (domaine.sommet_elem(element,2)==som[2])&&
      (domaine.sommet_elem(element,3)==som[3])&&
      (domaine.sommet_elem(element,4)==som[4])&&
      (domaine.sommet_elem(element,5)==som[5])&&
      (domaine.sommet_elem(element,6)==som[6])&&
      (domaine.sommet_elem(element,7)==som[7]))
    return 1;
  else
    return 0;
}

/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
template <typename _SIZE_>
void Hexaedre_32_64<_SIZE_>::calculer_volumes(DoubleVect_t& volumes) const
{
  const Domaine_t& domaine=this->mon_dom.valeur();
  double dx,dy,dz;
  int_t S1,S2,S3,S4;

  int_t size_tot = domaine.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int_t num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = domaine.sommet_elem(num_poly,0);
      S2 = domaine.sommet_elem(num_poly,1);
      S3 = domaine.sommet_elem(num_poly,2);
      S4 = domaine.sommet_elem(num_poly,4);
      dx = domaine.coord(S2,0) - domaine.coord(S1,0);
      dy = domaine.coord(S3,1) - domaine.coord(S1,1);
      dz = domaine.coord(S4,2) - domaine.coord(S1,2);
      volumes[num_poly]= dx*dy*dz;
    }
}

/*! @brief Calcule les normales aux faces des elements du domaine associe.
 *
 * @param (IntTab& face_sommets) les numeros des sommets des faces dans la liste des sommets du domaine associe
 * @param (DoubleTab& face_normales)
 */
template <typename _SIZE_>
void Hexaedre_32_64<_SIZE_>::calculer_normales(const IntTab_t& Face_sommets, DoubleTab_t& face_normales) const
{
  const Domaine_t& domaine_geom = this->mon_dom.valeur();
  const DoubleTab_t& les_coords = domaine_geom.coord_sommets();
  int_t nbfaces = Face_sommets.dimension(0);
  double x1,y1,z1,x2,y2,z2;
  int_t n0,n1,n2;
  for (int numface=0; numface<nbfaces; numface++)
    {

      n0 = Face_sommets(numface,0);
      n1 = Face_sommets(numface,1);
      n2 = Face_sommets(numface,2);

      x1 = les_coords(n0,0) - les_coords(n1,0);
      y1 = les_coords(n0,1) - les_coords(n1,1);
      z1 = les_coords(n0,2) - les_coords(n1,2);

      x2 = les_coords(n2,0) - les_coords(n1,0);
      y2 = les_coords(n2,1) - les_coords(n1,1);
      z2 = les_coords(n2,2) - les_coords(n1,2);

      face_normales(numface,0) = (y1*z2 - y2*z1);
      face_normales(numface,1) = (-x1*z2 + x2*z1);
      face_normales(numface,2) = (x1*y2 - x2*y1);
    }
}


/*! @brief voir ElemGeomBase::get_tab_faces_sommets_locaux
 *
 */
template <typename _SIZE_>
int Hexaedre_32_64<_SIZE_>::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(6,4);
  for (int i=0; i<6; i++)
    for (int j=0; j<4; j++)
      faces_som_local(i,j) = faces_sommets_hexa[i][j];
  return 1;
}

/*! @brief Renvoie le numero du j-ieme sommet de la i-ieme face de l'element.
 *
 * @param (int i) un numero de face
 * @param (int j) un numero de sommet
 * @return (int) le numero du j-ieme sommet de la i-ieme face
 */
template <typename _SIZE_>
int Hexaedre_32_64<_SIZE_>::face_sommet(int i, int j) const
{
  assert(i<6);
  switch(i)
    {
    case 0:
      return face_sommet0(j);
    case 1:
      return face_sommet1(j);
    case 2:
      return face_sommet2(j);
    case 3:
      return face_sommet3(j);
    case 4:
      return face_sommet4(j);
    case 5:
      return face_sommet5(j);
    default :
      return -1;
    }
}


template class Hexaedre_32_64<int>;
#if INT_is_64_ == 2
template class Hexaedre_32_64<trustIdType>;
#endif

