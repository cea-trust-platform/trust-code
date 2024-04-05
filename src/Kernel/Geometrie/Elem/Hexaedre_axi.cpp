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

#include <Hexaedre_axi.h>
#include <Domaine.h>

Implemente_instanciable_32_64(Hexaedre_axi_32_64,"Hexaedre_axi",Hexaedre_32_64<_T_>);


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
template <typename _SIZE_>
Sortie& Hexaedre_axi_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
template <typename _SIZE_>
Entree& Hexaedre_axi_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}


/*! @brief Renvoie le nom LML d'un triangle = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
template <typename _SIZE_>
const Nom& Hexaedre_axi_32_64<_SIZE_>::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}


/*! @brief Calcule les centres de gravites de tous les elements du domaine associe a l'element goemetrique.
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des centres de gravite
 */
template <typename _SIZE_>
void Hexaedre_axi_32_64<_SIZE_>::calculer_centres_gravite(DoubleTab_t& xp) const
{
  const IntTab_t& les_Polys = this->mon_dom->les_elems();
  const Domaine_t& le_dom = this->mon_dom.valeur();
  int_t nb_elem = this->mon_dom->nb_elem();

  xp.resize(nb_elem, Objet_U::dimension);
  xp=0;

  for (int_t num_elem=0; num_elem<nb_elem; num_elem++)
    {
      double d_teta;
      d_teta = le_dom.coord(les_Polys(num_elem,3),1) -
               le_dom.coord(les_Polys(num_elem,1),1);
      if( d_teta<0 )
        {
          // Modif axi !

          for(int s=0; s<this->nb_som(); s++)
            {
              int_t num_som = les_Polys(num_elem,s);
              for(int i=0; i < Objet_U::dimension; i++)
                xp(num_elem,i) += le_dom.coord(num_som,i);
              if(le_dom.coord(num_som,1) == 0.)
                xp(num_elem,1) += 2*M_PI;
            }

        }
      else
        for(int s=0; s<this->nb_som(); s++)
          {
            int_t num_som = les_Polys(num_elem,s);
            for(int i=0; i<Objet_U::dimension; i++)
              xp(num_elem,i) += le_dom.coord(num_som,i);
          }
    }
  double x=1./this->nb_som();
  xp*=x;
}


/*! @brief NE FAIT RIEN: A CODER,renvoie toujours 0
 *
 *     Renvoie 1 si l'element "elemen" du domaine associe a
 *               l'element geometrique contient le point
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int element) le numero de l'element du domaine dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element "element" 0 sinon
 */
template <typename _SIZE_>
int Hexaedre_axi_32_64<_SIZE_>::contient(const ArrOfDouble& pos, int_t element ) const
{
  assert(pos.size_array()==3);
  const Domaine_t& dom=this->mon_dom.valeur();
  int_t som0 = dom.sommet_elem(element,0);
  int_t som7 = dom.sommet_elem(element,7);
  double t7 = dom.coord(som7,1);
  if (t7<dom.coord(som0,1)) t7+=2*M_PI;
  if (    inf_ou_egal(dom.coord(som0,0),pos[0]) && inf_ou_egal(pos[0],dom.coord(som7,0))
          && inf_ou_egal(dom.coord(som0,1),pos[1]) && inf_ou_egal(pos[1],t7)
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
int Hexaedre_axi_32_64<_SIZE_>::contient(const SmallArrOfTID_t& som, int_t element ) const
{
  const Domaine_t& dom=this->mon_dom.valeur();
  if((dom.sommet_elem(element,0)==som[0])&&
      (dom.sommet_elem(element,1)==som[1])&&
      (dom.sommet_elem(element,2)==som[2])&&
      (dom.sommet_elem(element,3)==som[3])&&
      (dom.sommet_elem(element,4)==som[4])&&
      (dom.sommet_elem(element,5)==som[5])&&
      (dom.sommet_elem(element,6)==som[6])&&
      (dom.sommet_elem(element,7)==som[7]))
    return 1;
  else
    return 0;
}



/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
template <typename _SIZE_>
void Hexaedre_axi_32_64<_SIZE_>::calculer_volumes(DoubleVect_t& volumes) const
{
  const Domaine_t& dom=this->mon_dom.valeur();
  double r,dr,d_teta,dz;
  int_t S1,S2,S4,S5;

  int_t size_tot = dom.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int_t num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = dom.sommet_elem(num_poly,0);
      S2 = dom.sommet_elem(num_poly,1);
      S4 = dom.sommet_elem(num_poly,3);
      S5 = dom.sommet_elem(num_poly,4);
      r = 0.5*(dom.coord(S2,0) + dom.coord(S1,0));
      dr = dom.coord(S2,0) - dom.coord(S1,0);
      d_teta = dom.coord(S4,1) - dom.coord(S2,1);
      if (d_teta<0) d_teta+=2.*M_PI;
      dz = dom.coord(S5,2) - dom.coord(S1,2);
      volumes[num_poly]= dr*r*d_teta*dz;
    }
}


template class Hexaedre_axi_32_64<int>;
#if INT_is_64_ == 2
template class Hexaedre_axi_32_64<trustIdType>;
#endif

