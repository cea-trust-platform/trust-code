/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Prisme.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Prisme.h>
#include <Domaine.h>
//      5
//     /|
//    3----4
//    | |  |
//    | 2  |
//    |/   |
//    0----1

static int faces_sommets_prisme[5][4] =
{
  { 0, 1, 3, 4 },
  { 0, 2, 3, 5 },
  { 1, 2, 4, 5 },
  { 0, 1, 2, -1 },
  { 3, 4, 5, -1 }
};


Implemente_instanciable(Prisme,"Prisme",Elem_geom_base);


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
Sortie& Prisme::printOn(Sortie& s ) const
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
Entree& Prisme::readOn(Entree& s )
{
  return s;
}


// Description:
//    Reordonne les sommets du Prisme.
//    NE FAIT RIEN: A CODER
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Prisme::reordonner()
{
  Cerr << "Prisme::reordonner must be coded " << finl;
  // a coder. remise en conformite de la numerotation des noeuds.
}


// Description:
//    Renvoie le nom LML d'un prisme = "PRISM6".
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
const Nom& Prisme::nom_lml() const
{
  static Nom nom="PRISM6";
  return nom;
}


// Description:
//    NE FAIT RIEN: A CODER, renvoie toujours 0.
//    Renvoie 1 si l'element "element" de la zone associee a
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
// Parametre: int element
//    Signification: le numero de l'element de la zone
//                   dans lequel on cherche le point.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le point de coordonnees specifiees
//                   appartient a l'element "element"
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Prisme::contient(const ArrOfDouble& pos, int ielem ) const
{
  // a coder :
  // est-ce que le prisme de numero element contient le point de
  // coordonnees pos ?
  // adaptation de tetraedre contient
  assert(pos.size_array()==3);
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  double prod1,prod2,xn,yn,zn;
  int som0, som1, som2, som3,som4,som5;

  // On regarde tout d'abord si le pintr cherche n'est pas un des
  // sommets du triangle
  som0 = zone.sommet_elem(ielem,0);
  som1 = zone.sommet_elem(ielem,1);
  som2 = zone.sommet_elem(ielem,2);
  som3 = zone.sommet_elem(ielem,3);
  som4 = zone.sommet_elem(ielem,4);
  som5 = zone.sommet_elem(ielem,5);
  if( ( est_egal(dom.coord(som0,0),pos(0)) && est_egal(dom.coord(som0,1),pos(1)) && est_egal(dom.coord(som0,2),pos(2)) )
      || (est_egal(dom.coord(som1,0),pos(0)) && est_egal(dom.coord(som1,1),pos(1)) && est_egal(dom.coord(som1,2),pos(2)))
      || (est_egal(dom.coord(som2,0),pos(0)) && est_egal(dom.coord(som2,1),pos(1)) && est_egal(dom.coord(som2,2),pos(2)))
      || (est_egal(dom.coord(som3,0),pos(0)) && est_egal(dom.coord(som3,1),pos(1)) && est_egal(dom.coord(som3,2),pos(2)))
      || (est_egal(dom.coord(som4,0),pos(0)) && est_egal(dom.coord(som4,1),pos(1)) && est_egal(dom.coord(som4,2),pos(2)))
      || (est_egal(dom.coord(som5,0),pos(0)) && est_egal(dom.coord(som5,1),pos(1)) && est_egal(dom.coord(som5,2),pos(2)))
    )
    return 1;

  for (int j=0; j<5; j++)
    {
      switch(j)
        {
        case 0 :
          som0 = zone.sommet_elem(ielem,0);
          som1 = zone.sommet_elem(ielem,1);
          som2 = zone.sommet_elem(ielem,3);
          som3 = zone.sommet_elem(ielem,2);
          break;
        case 1 :
          som0 = zone.sommet_elem(ielem,0);
          som1 = zone.sommet_elem(ielem,2);
          som2 = zone.sommet_elem(ielem,3);
          som3 = zone.sommet_elem(ielem,4);
          break;
        case 2 :
          som0 = zone.sommet_elem(ielem,1);
          som1 = zone.sommet_elem(ielem,2);
          som2 = zone.sommet_elem(ielem,4);
          som3 = zone.sommet_elem(ielem,0);
          break;
        case 3 :
          som0 = zone.sommet_elem(ielem,0);
          som1 = zone.sommet_elem(ielem,1);
          som2 = zone.sommet_elem(ielem,2);
          som3 = zone.sommet_elem(ielem,3);
          break;
        case 4 :
          som0 = zone.sommet_elem(ielem,3);
          som1 = zone.sommet_elem(ielem,4);
          som2 = zone.sommet_elem(ielem,5);
          som3 = zone.sommet_elem(ielem,0);
          break;
        }

      // Algorithme : le sommet 3 et le point M doivent pour j=0 a 3 du meme cote
      // que le plan formes par les points som0,som1,som2.
      // calcul de la normale au plan som0,som1,som2 :
      xn = (dom.coord(som1,1)-dom.coord(som0,1))*(dom.coord(som2,2)-dom.coord(som0,2))
           - (dom.coord(som1,2)-dom.coord(som0,2))*(dom.coord(som2,1)-dom.coord(som0,1));
      yn = (dom.coord(som1,2)-dom.coord(som0,2))*(dom.coord(som2,0)-dom.coord(som0,0))
           - (dom.coord(som1,0)-dom.coord(som0,0))*(dom.coord(som2,2)-dom.coord(som0,2));
      zn = (dom.coord(som1,0)-dom.coord(som0,0))*(dom.coord(som2,1)-dom.coord(som0,1))
           - (dom.coord(som1,1)-dom.coord(som0,1))*(dom.coord(som2,0)-dom.coord(som0,0));
      prod1 = xn * ( dom.coord(som3,0) - dom.coord(som0,0) )
              + yn * ( dom.coord(som3,1) - dom.coord(som0,1) )
              + zn * ( dom.coord(som3,2) - dom.coord(som0,2) );
      prod2 = xn * ( pos[0] - dom.coord(som0,0) )
              + yn * ( pos[1] - dom.coord(som0,1) )
              + zn * ( pos[2] - dom.coord(som0,2) );
      // Si le point est sur le plan (prod2 quasi nul) : on ne peut pas conclure...
      if (prod1*prod2 < 0 && dabs(prod2)>dabs(prod1)*Objet_U::precision_geom) return 0;
    }
  return 1;
}


// Description:
//    NE FAIT RIEN: A CODER, renvoie toujours 0
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
int Prisme::contient(const ArrOfInt& pos, int element ) const
{
  // a coder :
  exit();
  return 0;
}


// Description:
//    NE FAIT RIEN: A CODER
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
void Prisme::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const IntTab& elem=zone.les_elems();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      int i1=elem(num_poly,0);
      int i2=elem(num_poly,1);
      int i3=elem(num_poly,2);
      int i4=elem(num_poly,3);
      int i5=elem(num_poly,4);
      int i6=elem(num_poly,5);

      // recuperer de MEDMEM_Formulae.hxx
      double a1 = (coord(i2,0)-coord(i3,0))/2.0, a2 = (coord(i2,1)-coord(i3,1))/2.0, a3 = (coord(i2,2)-coord(i3,2))/2.0;
      double b1 = (coord(i5,0)-coord(i6,0))/2.0, b2 = (coord(i5,1)-coord(i6,1))/2.0, b3 = (coord(i5,2)-coord(i6,2))/2.0;
      double c1 = (coord(i4,0)-coord(i1,0))/2.0, c2 = (coord(i4,1)-coord(i1,1))/2.0, c3 = (coord(i4,2)-coord(i1,2))/2.0;
      double d1 = (coord(i5,0)-coord(i2,0))/2.0, d2 = (coord(i5,1)-coord(i2,1))/2.0, d3 = (coord(i5,2)-coord(i2,2))/2.0;
      double e1 = (coord(i6,0)-coord(i3,0))/2.0, e2 = (coord(i6,1)-coord(i3,1))/2.0, e3 = (coord(i6,2)-coord(i3,2))/2.0;
      double f1 = (coord(i1,0)-coord(i3,0))/2.0, f2 = (coord(i1,1)-coord(i3,1))/2.0, f3 = (coord(i1,2)-coord(i3,2))/2.0;
      double h1 = (coord(i4,0)-coord(i6,0))/2.0, h2 = (coord(i4,1)-coord(i6,1))/2.0, h3 = (coord(i4,2)-coord(i6,2))/2.0;

      double A = a1*c2*f3 - a1*c3*f2 - a2*c1*f3 + a2*c3*f1 +
                 a3*c1*f2 - a3*c2*f1;
      double B = b1*c2*h3 - b1*c3*h2 - b2*c1*h3 + b2*c3*h1 +
                 b3*c1*h2 - b3*c2*h1;
      double C = (a1*c2*h3 + b1*c2*f3) - (a1*c3*h2 + b1*c3*f2) -
                 (a2*c1*h3 + b2*c1*f3) + (a2*c3*h1 + b2*c3*f1) +
                 (a3*c1*h2 + b3*c1*f2) - (a3*c2*h1 + b3*c2*f1);
      double D = a1*d2*f3 - a1*d3*f2 - a2*d1*f3 + a2*d3*f1 +
                 a3*d1*f2 - a3*d2*f1;
      double E = b1*d2*h3 - b1*d3*h2 - b2*d1*h3 + b2*d3*h1 +
                 b3*d1*h2 - b3*d2*h1;
      double F = (a1*d2*h3 + b1*d2*f3) - (a1*d3*h2 + b1*d3*f2) -
                 (a2*d1*h3 + b2*d1*f3) + (a2*d3*h1 + b2*d3*f1) +
                 (a3*d1*h2 + b3*d1*f2) - (a3*d2*h1 + b3*d2*f1);
      double G = a1*e2*f3 - a1*e3*f2 - a2*e1*f3 + a2*e3*f1 +
                 a3*e1*f2 - a3*e2*f1;
      double H = b1*e2*h3 - b1*e3*h2 - b2*e1*h3 + b2*e3*h1 +
                 b3*e1*h2 - b3*e2*h1;
      double P = (a1*e2*h3 + b1*e2*f3) - (a1*e3*h2 + b1*e3*f2) -
                 (a2*e1*h3 + b2*e1*f3) + (a2*e3*h1 + b2*e3*f1) +
                 (a3*e1*h2 + b3*e1*f2) - (a3*e2*h1 + b3*e2*f1);
      volumes[num_poly]=dabs(-2.0*(2.0*(A + B + D + E + G + H) + C + F + P)/9.0);

    }

}
int Prisme::nb_type_face() const
{
  return 2;
}
// Description: remplit le tableau faces_som_local(i,j) qui donne pour
//  0 <= i < nb_faces()  et  0 <= j < nb_som_face(i) le numero local du sommet
//  sur l'element.
//  On a  0 <= faces_sommets_locaux(i,j) < nb_som()
// Si toutes les faces de l'element n'ont pas le meme nombre de sommets, le nombre
// de colonnes du tableau est le plus grand nombre de sommets, et les cases inutilisees
// du tableau sont mises a -1
// On renvoie 1 si toutes les faces ont le meme nombre d'elements, 0 sinon.
int Prisme::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(5,4);
  for (int i=0; i<5; i++)
    for (int j=0; j<4; j++)
      faces_som_local(i,j) = faces_sommets_prisme[i][j];
  return 1;


  Cerr << "Elem_geom_base::faces_sommets_locaux : error.\n"
       << " Method not implemented for the object "
       << que_suis_je() << finl;
  exit();
  return 0;
}
