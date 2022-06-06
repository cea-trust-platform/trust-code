/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Tetraedre.h>
#include <Domaine.h>
#include <Linear_algebra_tools_impl.h>
#include <algorithm>
using std::swap;

static int faces_sommets_tetra[4][3] =
{
  { 1, 2, 3 },
  { 2, 3, 0 },
  { 3, 0, 1 },
  { 0, 1, 2 }
};

Implemente_instanciable(Tetraedre,"Tetraedre",Elem_geom_base);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Tetraedre::printOn(Sortie& s ) const
{
  return s;
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Tetraedre::readOn(Entree& s )
{
  return s;
}




// Description:
//    Renvoie le nom LML d'un tetraedre = "TETRA4".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "TETRA4"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Tetraedre::nom_lml() const
{
  static Nom nom="TETRA4";
  return nom;
}


// Description:
//    Renvoie 1 si l'element ielem de la zone associee a
//              l'element geometrique contient le point
//              de coordonnees specifiees par le parametre "pos".
//    Renvoie 0 sinon.
// Precondition:
// Parametre: DoubleVect& pos
//    Signification: coordonnees du point que l'on
//                   cherche a localiser
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int ielem
//    Signification: le numero de l'element de la zone
//                   dans lequel on cherche le point.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le point de coordonnees specifiees
//                   appartient a l'element ielem
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Tetraedre::contient(const ArrOfDouble& pos, int ielem) const
{
  // 29/01/2010 Optimisation CPU de la methode (50% plus rapide) par PL
  assert(pos.size_array()==3);
  const Zone& zone=ma_zone.valeur();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  double prod1,prod2,xn,yn,zn;
  int som0 = zone.sommet_elem(ielem,0);
  int som1 = zone.sommet_elem(ielem,1);
  int som2 = zone.sommet_elem(ielem,2);
  int som3 = zone.sommet_elem(ielem,3);
  double X0 = coord(som0,0);
  double Y0 = coord(som0,1);
  double Z0 = coord(som0,2);
  double X1 = coord(som1,0);
  double Y1 = coord(som1,1);
  double Z1 = coord(som1,2);
  double X2 = coord(som2,0);
  double Y2 = coord(som2,1);
  double Z2 = coord(som2,2);
  double X3 = coord(som3,0);
  double Y3 = coord(som3,1);
  double Z3 = coord(som3,2);

  // On regarde tout d'abord si le point cherche n'est pas un des sommets du triangle
  // D'apres gcov sur les 1000 cas tests, on ne gagne pas beaucoup a faire cela (utile dans 0.03% des cas!!!)
  /*
    if ((est_egal(X0,pos(0)) && est_egal(Y0,pos(1)) && est_egal(Z0,pos(2)))
    || (est_egal(X1,pos(0)) && est_egal(Y1,pos(1)) && est_egal(Z1,pos(2)))
    || (est_egal(X2,pos(0)) && est_egal(Y2,pos(1)) && est_egal(Z2,pos(2)))
    || (est_egal(X3,pos(0)) && est_egal(Y3,pos(1)) && est_egal(Z3,pos(2))))
    return 1; */

  for (int j=0; j<4; j++)
    {
      switch(j)
        {
        case 0 :
          // Inutile car deja fait quelques lignes plus haut
          //som0 = zone.sommet_elem(ielem,0);
          //som1 = zone.sommet_elem(ielem,1);
          //som2 = zone.sommet_elem(ielem,2);
          //som3 = zone.sommet_elem(ielem,3);
          break;
        case 1 :
          swap(som0,som1);
          swap(som2,som3);
          swap(X0,X1);
          swap(Y0,Y1);
          swap(Z0,Z1);
          swap(X2,X3);
          swap(Y2,Y3);
          swap(Z2,Z3);
          //som0 = zone.sommet_elem(ielem,1);
          //som1 = zone.sommet_elem(ielem,0);
          //som2 = zone.sommet_elem(ielem,3);
          //som3 = zone.sommet_elem(ielem,2);
          break;
        case 2 :
          swap(som0,som1);
          swap(som1,som3);
          swap(X0,X1);
          swap(Y0,Y1);
          swap(Z0,Z1);
          swap(X1,X3);
          swap(Y1,Y3);
          swap(Z1,Z3);
          //som0 = zone.sommet_elem(ielem,0);
          //som1 = zone.sommet_elem(ielem,2);
          //som2 = zone.sommet_elem(ielem,3);
          //som3 = zone.sommet_elem(ielem,1);
          break;
        case 3 :
          swap(som0,som1);
          swap(som1,som3);
          swap(X0,X1);
          swap(Y0,Y1);
          swap(Z0,Z1);
          swap(X1,X3);
          swap(Y1,Y3);
          swap(Z1,Z3);
          //som0 = zone.sommet_elem(ielem,2);
          //som1 = zone.sommet_elem(ielem,1);
          //som2 = zone.sommet_elem(ielem,3);
          //som3 = zone.sommet_elem(ielem,0);
          break;
        }

      // Algorithme : le sommet 3 et le point M doivent pour j=0 a 3 du meme cote
      // que le plan formes par les points som0,som1,som2.
      // calcul de la normale au plan som0,som1,som2 :
      xn = ( Y1 - Y0 ) * ( Z2 - Z0 ) - ( Z1 - Z0 ) * ( Y2 - Y0 );
      yn = ( Z1 - Z0 ) * ( X2 - X0 ) - ( X1 - X0 ) * ( Z2 - Z0 );
      zn = ( X1 - X0 ) * ( Y2 - Y0 ) - ( Y1 - Y0 ) * ( X2 - X0 );
      prod1 = xn * ( X3 - X0 )
              + yn * ( Y3 - Y0 )
              + zn * ( Z3 - Z0 );
      prod2 = xn * ( pos[0] - X0 )
              + yn * ( pos[1] - Y0 )
              + zn * ( pos[2] - Z0 );
      // Si le point est sur le plan (prod2 quasi nul) : on ne peut pas conclure...
      if (prod1*prod2 < 0 && std::fabs(prod2)>std::fabs(prod1)*Objet_U::precision_geom) return 0;
    }
  return 1;
}


// Description:
//    Renvoie 1 si les sommets specifies par le parametre "pos"
//    sont les sommets de l'element "element" de la zone associee a
//    l'element geometrique.
// Precondition:
// Parametre: IntVect& pos
//    Signification: les numeros des sommets a comparer
//                   avec ceux de l'elements "element"
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int element
//    Signification: le numero de l'element de la zone
//                   dont on veut comparer les sommets
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les sommets passes en parametre
//                   sont ceux de l'element specifie, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Tetraedre::contient(const ArrOfInt& som, int element ) const
{
  const Zone& zone=ma_zone.valeur();
  if((zone.sommet_elem(element,0)==som[0])&&
      (zone.sommet_elem(element,1)==som[1])&&
      (zone.sommet_elem(element,1)==som[2])&&
      (zone.sommet_elem(element,1)==som[3]))
    return 1;
  else
    return 0;
}

// Description:
//    Calcule les volumes des elements de la zone associee.
// Precondition:
// Parametre: DoubleVect& volumes
//    Signification: le vecteur contenant les valeurs  des
//                   des volumes des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Tetraedre::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();

  double x0,y0,z0;
  double x1,y1,z1;
  double x2,y2,z2;
  double x3,y3,z3;
  int s[4];

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      for (int i=0; i<4; i++)
        s[i] = zone.sommet_elem(num_poly,i);

      x0 = dom.coord(s[0],0);
      y0 = dom.coord(s[0],1);
      z0 = dom.coord(s[0],2);

      x1 = dom.coord(s[1],0);
      y1 = dom.coord(s[1],1);
      z1 = dom.coord(s[1],2);

      x2 = dom.coord(s[2],0);
      y2 = dom.coord(s[2],1);
      z2 = dom.coord(s[2],2);

      x3 = dom.coord(s[3],0);
      y3 = dom.coord(s[3],1);
      z3 = dom.coord(s[3],2);

      volumes[num_poly] = std::fabs((x1-x0)*((y2-y0)*(z3-z0)-(y3-y0)*(z2-z0))-
                                    (x2-x0)*((y1-y0)*(z3-z0)-(y3-y0)*(z1-z0))+
                                    (x3-x0)*((y1-y0)*(z2-z0)-(y2-y0)*(z1-z0)))/6;
    }
}


// Description:
//    Calcule les normales aux faces des elements de la zone associee.
// Precondition:
// Parametre: IntTab& face_sommets
//    Signification: les numeros des sommets des faces
//                   dans la liste des sommets de la zone associee
// Parametre: DoubleTab& face_normales
//    Signification : les normales aux faces des elements
//                     de la zone associee
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Tetraedre::calculer_normales(const IntTab& Face_sommets ,
                                  DoubleTab& face_normales) const
{
  const Zone& zone_geom = ma_zone.valeur();
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  int nbfaces = Face_sommets.dimension(0);
  double x1,y1,z1,x2,y2,z2;
  int n0,n1,n2;
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

      face_normales(numface,0) = (y1*z2 - y2*z1)/2;
      face_normales(numface,1) = (-x1*z2 + x2*z1)/2;
      face_normales(numface,2) = (x1*y2 - x2*y1)/2;
    }
}

// Description: voir ElemGeomBase::get_tab_faces_sommets_locaux
int Tetraedre::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  // un tetraedre a quatre faces de trois sommets
  faces_som_local.resize(4,3);
  for (int i=0; i<4; i++)
    for (int j=0; j<3; j++)
      faces_som_local(i,j) = faces_sommets_tetra[i][j];
  return 1;
}

void Tetraedre::get_tab_aretes_sommets_locaux(IntTab& tab) const
{
  // un tetraedre a six aretes de deux sommets
  tab.resize(6, 2);
  int count = 0;
  // Une arete entre chaque couple de sommet du tetra: n * (n-1) / 2 aretes avec n=4
  for (int i = 0; i < 3; i++)
    {
      for (int j = i + 1; j < 4; j++)
        {
          tab(count, 0) = i;
          tab(count, 1) = j;
          count++;
        }
    }
  assert(count == 6);
}


// Calcul de la coordonnee baryrentrique dans un tetra correspondant a une coordonnee
// carthesienne "point". Attention, si "point" est en dehors du tetra, une ou plusieurs
// coordonnees barycentriques seront negatives !
// polys est le tableau des tetraedres (indices de sommets),
// coords est le tableau des coordonnees des sommets,
// le_poly est le numero du tetraetre
// point la coordonnees du point a transformer
// On stocke le resultat dans coord_bary (poids des trois premiers sommets,
//  le quatrieme etant implicitement 1 moins la somme des trois autres)
// Si epsilon est non nul, la valeur de retour est l'incertitude sur les coordonnees
//  barycentriques, pour une incertitude epsilon sur les coordonnees carthesiennes.
//  (calcul en norme Linfini, c'est a dire le max des erreurs sur chaque composantes)
double Tetraedre::coord_bary(const IntTab& polys,
                             const DoubleTab& coords,
                             const Vecteur3& point,
                             int le_poly,
                             Vecteur3& coord_bary,
                             double epsilon)
{
  Matrice33 m;
  Vecteur3 origine;
  matrice_base_tetraedre(polys, coords, le_poly, m, origine);
  Matrice33 inverse_m;
  Matrice33::inverse(m, inverse_m);
  Vecteur3 v(point-origine);
  Matrice33::produit(inverse_m, v, coord_bary);

  double resu;
  if (epsilon > 0.)
    {
      // Une erreur epsilon sur la coordonnee "point" entraine une erreur sur coord_bary:
      double norm = inverse_m.norme_Linfini();
      resu = norm * epsilon;
    }
  else
    {
      resu = 0.;
    }
  return resu;
}
