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

#include <Rectangle_2D_axi.h>
#include <Domaine.h>
#include <math.h>

Implemente_instanciable_32_64(Rectangle_2D_axi_32_64,"Rectangle_2D_axi",Rectangle_32_64<_T_>);

template <typename _SIZE_>
Sortie& Rectangle_2D_axi_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}


template <typename _SIZE_>
Entree& Rectangle_2D_axi_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}

/*! @brief Renvoie le nom LML d'un rectangle_2D_axi = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
template <typename _SIZE_>
const Nom& Rectangle_2D_axi_32_64<_SIZE_>::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}


/*! @brief Calcule les centres de gravites de tous les elements du domaine associe a l'element goemetrique.
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des centres de gravite
 */
template <typename _SIZE_>
void Rectangle_2D_axi_32_64<_SIZE_>::calculer_centres_gravite(DoubleTab_t& xp) const
{
  /*
    const IntTab& les_Polys = mon_dom->les_elems();
    const Domaine& le_domaine = mon_dom->domaine();
    int nb_elem = mon_dom->nb_elem_tot();
    int num_som;

    xp.resize(nb_elem,dimension);
    xp=0;
    for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {
    int i, s;
    for( s=0; s<nb_som(); s++)
    {
    num_som = les_Polys(num_elem,s);
    for( i=0; i<dimension; i++)
    xp(num_elem,i) += le_domaine.coord(num_som,i);
    }
    }
    double x=1./nb_som();
    xp*=x;
  */
  Rectangle_32_64<_SIZE_>::calculer_centres_gravite(xp);
}

/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
template <typename _SIZE_>
void Rectangle_2D_axi_32_64<_SIZE_>::calculer_volumes(DoubleVect_t& volumes) const
{
  const Domaine_t& domaine=mon_dom.valeur();
  double r,r1,r2,dr,dz;
  int_t S1,S2,S3;
  int_t size_tot = domaine.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int_t num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = domaine.sommet_elem(num_poly,0);
      S2 = domaine.sommet_elem(num_poly,1);
      S3 = domaine.sommet_elem(num_poly,2);
      r1= domaine.coord(S1,0);
      r2= domaine.coord(S2,0);
      if (r1< r2) r =r1;
      else r =r2;
      dr =std::fabs( domaine.coord(S2,0) - domaine.coord(S1,0));
      dz =std::fabs( domaine.coord(S3,1) - domaine.coord(S1,1));
      volumes[num_poly]= 2*M_PI*(r*dr+0.5*dr*dr)*dz;
    }
  /*
    Rectangle::calculer_volumes(volumes);
  */
}


template class Rectangle_2D_axi_32_64<int>;
#if INT_is_64_ == 2
template class Rectangle_2D_axi_32_64<trustIdType>;
#endif

