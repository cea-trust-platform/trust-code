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

#include <Rectangle_axi.h>
#include <Zone.h>

Implemente_instanciable(Rectangle_axi,"Rectangle_axi",Rectangle);


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Rectangle_axi::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
Entree& Rectangle_axi::readOn(Entree& s )
{
  return s;
}


/*! @brief Calcule les centres de gravites de tous les elements de la zone associee a l'element goemetrique.
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des centres de gravite
 */
void Rectangle_axi::calculer_centres_gravite(DoubleTab& xp) const
{
  const IntTab& les_Polys = mon_dom->les_elems();
  const Zone& le_domaine = mon_dom.valeur();
  int nb_elem = mon_dom->nb_elem();
  int num_som;

  xp.resize(nb_elem,dimension);
  xp=0;
  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      double d_teta;
      int i, s;
      d_teta = le_domaine.coord(les_Polys(num_elem,3),1) -
               le_domaine.coord(les_Polys(num_elem,1),1);
      if( d_teta<0 )
        {
          // Modif axi !

          for( s=0; s<nb_som(); s++)
            {
              num_som = les_Polys(num_elem,s);
              for( i=0; i<dimension; i++)
                xp(num_elem,i) += le_domaine.coord(num_som,i);
              if(le_domaine.coord(num_som,1) == 0.)
                xp(num_elem,1) += 2*M_PI;
            }

        }
      else
        for( s=0; s<nb_som(); s++)
          {
            num_som = les_Polys(num_elem,s);
            for( i=0; i<dimension; i++)
              xp(num_elem,i) += le_domaine.coord(num_som,i);
          }
    }
  double x=1./nb_som();
  xp*=x;
}

/*! @brief NE FAIT RIEN A CODER
 *
 */
void Rectangle_axi::reordonner()
{
  // A CODER
}


/*! @brief Renvoie le nom LML d'un rectangle_axi = "VOXEL8".
 *
 * @return (Nom&) toujours egal a "VOXEL8"
 */
const Nom& Rectangle_axi::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}



/*! @brief Calcule les volumes des elements de la zone associee.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements de la zone
 */
void Rectangle_axi::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=mon_dom.valeur();
  const Zone& dom=zone;
  double r,dr,d_teta;
  int S1,S2,S4;

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = zone.sommet_elem(num_poly,0);
      S2 = zone.sommet_elem(num_poly,1);
      S4 = zone.sommet_elem(num_poly,3);
      r = 0.5*(dom.coord(S2,0) + dom.coord(S1,0));
      dr = dom.coord(S2,0) - dom.coord(S1,0);
      d_teta = dom.coord(S4,1) - dom.coord(S2,1);
      if (d_teta<0) d_teta+=2.*M_PI;
      volumes[num_poly]= dr*r*d_teta;
    }
}

/*! @brief //    Renvoie 1 si l'element ielem de la zone associee a //              l'element geometrique contient le point
 *
 *  //              de coordonnees specifiees par le parametre "pos".
 *  //    Renvoie 0 sinon.
 *  // Precondition:
 *  // Parametre: DoubleVect& pos
 *  //    Signification: coordonnees du point que l'on
 *  //                   cherche a localiser
 *  //    Valeurs par defaut:
 *  //    Contraintes: reference constante
 *  //    Acces: entree
 *  // Parametre: int ielem
 *  //    Signification: le numero de l'element de la zone
 *  //                   dans lequel on cherche le point.
 *  //    Valeurs par defaut:
 *  //    Contraintes:
 *  //    Acces:
 *  // Retour: int
 *  //    Signification: 1 si le point de coordonnees specifiees
 *  //                   appartient a l'element ielem
 *  //                   0 sinon
 *  //    Contraintes:
 *  // Exception:
 *  // Effets de bord:
 *  // Postcondition: la methode ne modifie pas l'objet
 *
 *
 */
int Rectangle_axi::contient(const ArrOfDouble& pos, int element ) const
{
  assert(pos.size_array()==2);
  const Zone& zone=mon_dom.valeur();
  const Zone& dom=zone;
  int som0 = zone.sommet_elem(element,0);
  int som3 = zone.sommet_elem(element,3);
  double r0,r3,t0,t3;
  r0=dom.coord(som0,0);
  r3=dom.coord(som3,0);
  t0=dom.coord(som0,1);
  t3=dom.coord(som3,1);
  if (t3<t0) t3+=2*M_PI;
  if (    inf_ou_egal(r0,pos[0]) && inf_ou_egal(pos[0],r3)
          && inf_ou_egal(t0,pos[1]) && inf_ou_egal(pos[1],t3) )
    return 1;
  else
    return 0;
}

/*! @brief Renvoie 1 si les sommets specifies par le parametre "pos" sont les sommets de l'element "element" de la zone associee a
 *
 *     l'element geometrique.
 *
 * @param (IntVect& pos) les numeros des sommets a comparer avec ceux de l'elements "element"
 * @param (int element) le numero de l'element de la zone dont on veut comparer les sommets
 * @return (int) 1 si les sommets passes en parametre sont ceux de l'element specifie, 0 sinon
 */
int Rectangle_axi::contient(const ArrOfInt& pos, int element ) const
{
  return Rectangle::contient(pos,element);
}

