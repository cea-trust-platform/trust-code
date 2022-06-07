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

#include <Triangle.h>
#include <Domaine.h>

// Description des faces du triangle de reference:
//  3 faces de deux sommets.
//  La face i est la face opposee au sommet i
// (voir get_tab_faces_sommets_locaux)
static int faces_sommets_triangle[3][2] =
{
  { 1, 2 },
  { 2, 0 },
  { 0, 1 }
};


Implemente_instanciable(Triangle,"Triangle",Elem_geom_base);


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
Sortie& Triangle::printOn(Sortie& s ) const
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
Entree& Triangle::readOn(Entree& s )
{
  return s;
}


// Description:
//    Renvoie le nom LML d'un triangle = "PRISM6".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "PRISM6"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Triangle::nom_lml() const
{
  static Nom nom="PRISM6";
  if (dimension==3) nom="TRIANGLE_3D";
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
int Triangle::contient(const ArrOfDouble& pos, int ielem) const
{
  assert(pos.size_array()==2);
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  assert(ielem<zone.nb_elem_tot());
  int som0 = zone.sommet_elem(ielem,0);
  int som1 = zone.sommet_elem(ielem,1);
  int som2 = zone.sommet_elem(ielem,2);
  assert((som0>=0) && (som0<dom.nb_som_tot()));
  assert((som1>=0) && (som1<dom.nb_som_tot()));
  assert((som2>=0) && (som2<dom.nb_som_tot()));
  double prod,p0,p1,p2;

  // On regarde tout d'abord si le point cherche n'est pas un des
  // sommets du triangle
  // GF on retire le test pour etre coherent avec tetraedre contient et pour eviter des erreurs dans Champ_implementation_P1::form_function, ou il n'y a pas ce test
  /*
    if( (est_egal(dom.coord(som0,0),pos(0)) && est_egal(dom.coord(som0,1),pos(1)))
    || (est_egal(dom.coord(som1,0),pos(0)) && est_egal(dom.coord(som1,1),pos(1)))
    || (est_egal(dom.coord(som2,0),pos(0)) && est_egal(dom.coord(som2,1),pos(1))) )
    return 1;

  */
  // Attention les sommets sont ranges de facon quelconque.
  // Il faut donc determiner le sens (trigo ou anti trigo) pour la numerotation :
  // Calcul de prod = 01 vectoriel 02 selon z
  // prod > 0 : sens trigo
  // prod < 0 : sens anti trigo
  prod = (dom.coord(som1,0)-dom.coord(som0,0))*(dom.coord(som2,1)-dom.coord(som0,1))
         - (dom.coord(som1,1)-dom.coord(som0,1))*(dom.coord(som2,0)-dom.coord(som0,0));
  double signe;
  if (prod >= 0)
    signe = 1;
  else
    signe = -1;
  // Calcul de p0 = 0M vectoriel 1M selon z
  p0 = (pos[0]-dom.coord(som0,0))*(pos[1]-dom.coord(som1,1))
       - (pos[1]-dom.coord(som0,1))*(pos[0]-dom.coord(som1,0));
  p0 *= signe;
  // Calcul de p1 = 1M vectoriel 2M selon z
  p1 = (pos[0]-dom.coord(som1,0))*(pos[1]-dom.coord(som2,1))
       - (pos[1]-dom.coord(som1,1))*(pos[0]-dom.coord(som2,0));
  p1 *= signe;
  // Calcul de p2 = 2M vectoriel 0M selon z
  p2 = (pos[0]-dom.coord(som2,0))*(pos[1]-dom.coord(som0,1))
       - (pos[1]-dom.coord(som2,1))*(pos[0]-dom.coord(som0,0));
  p2 *= signe;
  double epsilon=std::fabs(prod)*Objet_U::precision_geom;
  if ((p0>-epsilon) && (p1>-epsilon) && (p2>-epsilon))
    return 1;
  else
    return 0;
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
int Triangle::contient(const ArrOfInt& som, int element ) const
{
  const Zone& zone=ma_zone.valeur();
  if((zone.sommet_elem(element,0)==som[0])&&
      (zone.sommet_elem(element,1)==som[1])&&
      (zone.sommet_elem(element,2)==som[2]))
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
void Triangle::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const DoubleTab& coord = zone.domaine().coord_sommets();
  DoubleTab pos(3,dimension);
  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      for (int i=0; i<3; i++)
        {
          int Si = zone.sommet_elem(num_poly,i);
          for (int j=0; j<dimension; j++)
            pos(i,j) = coord(Si,j);
        }
      volumes[num_poly] = aire_triangle(pos);
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
void Triangle::calculer_normales(const IntTab& Face_sommets ,
                                 DoubleTab& face_normales) const
{
  const Zone& zone_geom = ma_zone.valeur();
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  int nbfaces = Face_sommets.dimension(0);
  double x1,y1;
  int n0,n1;
  for (int numface=0; numface<nbfaces; numface++)
    {
      n0 = Face_sommets(numface,0);
      n1 = Face_sommets(numface,1);
      x1 = les_coords(n0,0)-les_coords(n1,0);
      y1 = les_coords(n0,1)-les_coords(n1,1);
      face_normales(numface,0) = -y1;
      face_normales(numface,1) = x1;
    }
}

// Description: voir ElemGeomBase::get_tab_faces_sommets_locaux
int Triangle::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(3,2);
  for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
      faces_som_local(i,j) = faces_sommets_triangle[i][j];
  return 1;
}
