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
// File:        Quadrangle_VEF.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Quadrangle_VEF.h>
#include <Domaine.h>
#include <Triangle.h>

// Definition des sommets :
//  y
//  ^
//  |
//  2-----3
//  |     |
//  |     |
//  |     |
//  0-----1--> x
//
// Definition des faces d'un quadrangle:
//
//  *--3--*
//  |     |
//  0     2
//  |     |
//  *--1--*
static int faces_sommets_quadra[4][2] =
{
  { 0, 2 },
  { 0, 1 },
  { 1, 3 },
  { 2, 3 }
};

Implemente_instanciable(Quadrangle_VEF,"Quadrangle",Elem_geom_base);


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
Sortie& Quadrangle_VEF::printOn(Sortie& s ) const
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
Entree& Quadrangle_VEF::readOn(Entree& s )
{
  return s;
}


// Description:
//    Renvoie le nom LML d'un Quadrangle_VEF = "GOLGOTH24".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "GOLGOTH24"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Quadrangle_VEF::nom_lml() const
{
  // static Nom nom="GOLGOTH24";
  static Nom nom="VOXEL8";
  if (dimension==3) nom="QUADRANGLE_3D";
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
int Quadrangle_VEF::contient(const ArrOfDouble& pos, int element) const
{
  assert(pos.size_array()==2);
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  int som0 = zone.sommet_elem(element,0);
  int som1 = zone.sommet_elem(element,1);
  int som2 = zone.sommet_elem(element,2);
  int som3 = zone.sommet_elem(element,3);
  // On regarde tout d'abord si le point cherche n'est pas un des
  // sommets du quadrangle
  if( (est_egal(dom.coord(som0,0),pos(0)) && est_egal(dom.coord(som0,1),pos(1)))
      || (est_egal(dom.coord(som1,0),pos(0)) && est_egal(dom.coord(som1,1),pos(1)))
      || (est_egal(dom.coord(som2,0),pos(0)) && est_egal(dom.coord(som2,1),pos(1)))
      || (est_egal(dom.coord(som3,0),pos(0)) && est_egal(dom.coord(som3,1),pos(1))) )
    return 1;
  double prod,p0,p1,p2,p3;
  // Calcul de prod = 01 vectoriel 02 selon z
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
  // Calcul de p1 = 1M vectoriel 3M selon z
  p1 = (pos[0]-dom.coord(som1,0))*(pos[1]-dom.coord(som3,1))
       - (pos[1]-dom.coord(som1,1))*(pos[0]-dom.coord(som3,0));
  p1 *= signe;
  // Calcul de p2 = 3M vectoriel 2M selon z
  p2 = (pos[0]-dom.coord(som3,0))*(pos[1]-dom.coord(som2,1))
       - (pos[1]-dom.coord(som3,1))*(pos[0]-dom.coord(som2,0));
  p2 *= signe;
  // Calcul de p3 = 2M vectoriel 0M selon z
  p3 = (pos[0]-dom.coord(som2,0))*(pos[1]-dom.coord(som0,1))
       - (pos[1]-dom.coord(som2,1))*(pos[0]-dom.coord(som0,0));
  p3 *= signe;
  double epsilon=dabs(prod)*Objet_U::precision_geom;
  if ((p0>-epsilon) && (p1>-epsilon) && (p2>-epsilon) && (p3>-epsilon))
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
int Quadrangle_VEF::contient(const ArrOfInt& som, int element ) const
{
  const Zone& zone=ma_zone.valeur();
  if((zone.sommet_elem(element,0)==som[0])&&
      (zone.sommet_elem(element,1)==som[1])&&
      (zone.sommet_elem(element,2)==som[2])&&
      (zone.sommet_elem(element,3)==som[3]))
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
void Quadrangle_VEF::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const DoubleTab& coord = zone.domaine().coord_sommets();
  int S0,S1,S2,S3;
  ArrOfDouble xg(dimension);
  IntTab face_sommet_global;
  face_sommet_global.resize(4,2);
  DoubleTab pos(3,dimension);
  int numface;

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S0 = zone.sommet_elem(num_poly,0);
      S1 = zone.sommet_elem(num_poly,1);
      S2 = zone.sommet_elem(num_poly,2);
      S3 = zone.sommet_elem(num_poly,3);

      // on construit le tableau de travail face_sommet_global
      // les faces seront ordonnee suivant Xmin, Xmax, Ymin, Ymax, Zmin,Zmax
      // toujours en gardant la numerotation de TRUST

      face_sommet_global(0,0) =S0;
      face_sommet_global(0,1) =S2;

      face_sommet_global(1,0) =S0;
      face_sommet_global(1,1) =S1;

      face_sommet_global(2,0) =S1;
      face_sommet_global(2,1) =S3;

      face_sommet_global(3,0) =S3;
      face_sommet_global(3,1) =S2;

      //calcul du centre de gravite de l'element
      for (int j=0; j<dimension; j++)
        pos(0,j)=(coord(S0,j)+coord(S1,j)+coord(S2,j)+coord(S3,j))*0.25;

      double v = 0.;
      // boucle sur les faces de l'element que l'on decoupe en 4 triangles.
      // constitue des deux sommets de la face consideree et du centre de gravite de l'element
      for(numface=0; numface<nb_faces(0); numface++)
        {
          for (int i=0; i<2; i++)
            {
              int Si = face_sommet_global(numface,i);
              for (int j=0; j<dimension; j++)
                pos(i+1,j) = coord(Si,j);
            }
          v += aire_triangle(pos);
        }
      volumes[num_poly] = v;
    }
}


// Description:
//    Reordonne
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Quadrangle_VEF::reordonner()
{
  Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  IntTab& elem=zone.les_elems();

  ArrOfInt S(4);
  ArrOfInt NS(4);
  DoubleTab co(4,2);
  int num_poly;
  const int& nb_elem=zone.nb_elem();
  for (num_poly=0; num_poly<nb_elem; num_poly++)
    {
      for(int i=0; i<4; i++)
        {
          S(i) = elem(num_poly,i);
        }

      // adaptation de Hexaedre_VEF ::reordonne

      const DoubleTab& coord=dom.les_sommets();
      DoubleTab v(3,3);
      for (int i=1; i<4; i++)
        for (int dir=0; dir<2; dir++)
          v(i-1,dir)=coord(S[i],dir)-coord(S[0],dir);
      ArrOfDouble prod_(3);
      int opp=-1;
      for (int op=0; op<3; op++)
        {
          DoubleTab prod_v(3,3);
          for (int i=0; i<3; i++)
            {
              //  prod_v(i,0)=v(op,1)*v(i,2)-v(op,2)*v(i,1);
              //prod_v(i,1)=v(op,2)*v(i,0)-v(op,0)*v(i,2);
              prod_v(i,2)=v(op,0)*v(i,1)-v(op,1)*v(i,0);
            }
          //Cerr<<prod_v<<finl;
          double prod=0;
          int i1=-1,i2=-1;
          if (op==0)
            {
              i1=1;
              i2=2;
            }
          else if (op==1)
            {
              i1=0;
              i2=2;
            }
          else if (op==2)
            {
              i1=0;
              i2=1;
            }
          for (int dir=0; dir<3; dir++)
            prod+=(prod_v(i1,dir)*prod_v(i2,dir));
          prod_[op]=prod;
          if (prod<0)
            {
              if (opp!=-1)
                {
                  op=2;
                  // on a deux produits negatifs on ne fait rien
                }
              opp=op;

            }
        }
      if (opp!=2)
        {
          int i2=2;
          int i1=1;
          int i3=3;
          int i2b=i2;
          if (opp==0) i2b=i1;
          int tmp=elem(num_poly,i2b);
          elem(num_poly,i2b)=elem(num_poly,i3);
          elem(num_poly,i3)=tmp;
          Cerr << "Permutation of local nodes "<<i2b<<" and "<<i3<<" on the element " <<num_poly<<" prod "<<prod_<<finl;
        }

    }
}

// Description: voir ElemGeomBase::get_tab_faces_sommets_locaux
int Quadrangle_VEF::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(4,2);
  for (int i=0; i<4; i++)
    for (int j=0; j<2; j++)
      faces_som_local(i,j) = faces_sommets_quadra[i][j];
  return 1;
}
