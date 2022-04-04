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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Segment.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Segment.h>
#include <Domaine.h>

Implemente_instanciable(Segment,"Segment",Elem_geom_base);

static int faces_sommets_segment[2][1] =
{ { 0 } ,{ 1 }  };


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
Sortie& Segment::printOn(Sortie& s ) const
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
Entree& Segment::readOn(Entree& s )
{
  return s;
}

// Description:
//    Reordonne les sommets du segment.
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
void Segment::reordonner()
{
  Zone& zone=ma_zone.valeur();
  IntTab& elem=zone.les_elems();
  const Domaine& dom=zone.domaine();
  ArrOfInt S(2);
  int i;
  const int nb_elem=zone.nb_elem();
  for (int num_poly=0; num_poly<nb_elem; num_poly++)
    {
      for(i=0; i<2; i++)
        S[i] = elem(num_poly,i);

      if(sup_strict(dom.coord(S[0], 0),dom.coord(S[1], 0)))
        {
          int tmp=S[0];
          S[0]=S[1];
          S[1]=tmp;
        }

      for(i=0; i<2; i++)
        elem(num_poly, i)=S[i];
    }
}


// Description:
//    Renvoie le nom LML d'un triangle = "VOXEL8".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "VOXEL8"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Segment::nom_lml() const
{
  static Nom nom="SEGMENT";
  if (dimension==2)
    nom="QUADRANGLE_3D";
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
int Segment::contient(const ArrOfDouble& pos, int element ) const
{
  assert(pos.size_array()==dimension);

  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  const IntTab& elem=zone.les_elems();
  // Test whether OM = a.O1 with O and 1 the extreme points of the seg and M the point to be tested
  double autre_a = 0;
  for (int d=0; d<dimension; d++)
    {
      double O1 = dom.coord(elem(element,1), d) - dom.coord(elem(element,0), d);
      double OM = pos[d] - dom.coord(elem(element,0), d);
      if (!est_egal(O1,0))
        {
          double a = OM/O1;
          // M is outside O1?
          if (a<-Objet_U::precision_geom || a>1+Objet_U::precision_geom) return 0;
          // a is not the same as for another dimension, <=> not aligned
          if (autre_a>0 && !est_egal(a, autre_a)) return 0;
          autre_a = a;
        }
      else if (!est_egal(OM,0)) return 0; // M is not along O1
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
int Segment::contient(const ArrOfInt& pos, int element ) const
{
  assert(pos.size_array()==1);
  const Zone& zone=ma_zone.valeur();
  if((zone.sommet_elem(element,0)==pos[0])&&
      (zone.sommet_elem(element,1)==pos[1]))
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
void Segment::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  double dx;
  int S1,S2;

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = zone.sommet_elem(num_poly,0);
      S2 = zone.sommet_elem(num_poly,1);
      dx=0;
      for (int i=0; i<dimension; i++)
        {
          double d = dom.coord(S2,i) - dom.coord(S1,i);
          dx+=d*d;
        }

      volumes[num_poly]= sqrt(dx);
    }
}
// Description: voir ElemGeomBase::get_tab_faces_sommets_locaux
int Segment::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(2,1);
  for (int j=0; j<2; j++)
    faces_som_local(j,0) = faces_sommets_segment[j][0];
  return 1;
}

