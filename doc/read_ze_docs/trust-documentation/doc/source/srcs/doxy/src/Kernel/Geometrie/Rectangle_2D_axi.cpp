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

#include <Rectangle_2D_axi.h>
#include <Domaine.h>
#include <math.h>
Implemente_instanciable(Rectangle_2D_axi,"Rectangle_2D_axi",Rectangle);


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Rectangle_2D_axi::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
Entree& Rectangle_2D_axi::readOn(Entree& s )
{
  return s;
}

/*! @brief Renvoie le nom LML d'un rectangle_2D_axi = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
const Nom& Rectangle_2D_axi::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}

/*! @brief NE FAIT RIEN A CODER
 *
 */
void Rectangle_2D_axi::reordonner()
{
  Rectangle::reordonner();
  // A CODER
}


/*! @brief Calcule les centres de gravites de tous les elements du domaine associe a l'element goemetrique.
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des centres de gravite
 */
void Rectangle_2D_axi::calculer_centres_gravite(DoubleTab& xp) const
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
  Rectangle::calculer_centres_gravite( xp);


}

/*! @brief Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
void Rectangle_2D_axi::calculer_volumes(DoubleVect& volumes) const
{
  const Domaine& domaine=mon_dom.valeur();// domaine.valeur() permet d'acceder a                                            // le domaine qui est en cours de traitement
  const Domaine& dom=domaine;
  double r,r1,r2,dr,dz;
  int S1,S2,S3;
  int size_tot = domaine.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = domaine.sommet_elem(num_poly,0);
      S2 = domaine.sommet_elem(num_poly,1);
      S3 = domaine.sommet_elem(num_poly,2);
      r1= dom.coord(S1,0);
      r2= dom.coord(S2,0);
      if (r1< r2) r =r1;
      else r =r2;
      dr =std::fabs( dom.coord(S2,0) - dom.coord(S1,0));
      dz =std::fabs( dom.coord(S3,1) - dom.coord(S1,1));
      volumes[num_poly]= 2*M_PI*(r*dr+0.5*dr*dr)*dz;
    }
  /*
    Rectangle::calculer_volumes(volumes);
  */
}
