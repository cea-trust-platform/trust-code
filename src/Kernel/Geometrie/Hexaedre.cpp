/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Hexaedre.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

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

Implemente_instanciable(Hexaedre,"Hexaedre",Elem_geom_base);


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
Sortie& Hexaedre::printOn(Sortie& s ) const
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
Entree& Hexaedre::readOn(Entree& s )
{
  return s;
}

// Description:
//    Reordonne les sommets de l'hexaedre
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
void Hexaedre::reordonner()
{
  if (reordonner_elem()==-1)
    {
      Cerr << "This mesh is not composed of regular hexahedra\n";
      Cerr << "This seems to be VEF hexahedra (Hexaedre_VEF)\n";
      Cerr << "Check your mesh." << finl;
      exit();
    }
}

// Description:
//    Reordonne les sommets de l'hexaedre
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
int Hexaedre::reordonner_elem()
{
  Zone& zone = ma_zone.valeur();
  const Domaine& dom = zone.domaine();
  const DoubleTab& dom_coord = dom.les_sommets();
  IntTab& elem = zone.les_elems();
  ArrOfInt S(8);
  ArrOfInt NS(8);
  double coord[8][3];
  double xmin[3];
  int i, j;
  const int nb_elem = zone.nb_elem();
  const int delta[3] = {1, 2, 4};
  int changed_count = 0;

  for (int num_poly = 0; num_poly < nb_elem; num_poly++)
    {

      xmin[0] = xmin[1] = xmin[2] = 1e40;
      for(i=0; i<8; i++)
        {
          int s = elem(num_poly,i);
          S[i] = s;
          NS[i] = -1;
          for(j=0; j<3; j++)
            {
              double x = dom_coord(s, j);
              coord[i][j] = x;
              if (x < xmin[j])
                xmin[j] = x;
            }
        }

      // Pour chaque sommet, chercher rang dans l'element
      // en fonction de ses coordonnees
      for (i=0; i<8; i++)
        {
          int num_sommet = 0;
          for (j=0; j<3; j++)
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
      for(i=0; i<8; i++)
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
    Cerr << "Hexaedre::reordonner : " << changed_count << " elements reversed" << finl;
  return 0;
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
const Nom& Hexaedre::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}


// Description:
//    Renvoie 1 si l'element "elemen" de la zone associee a
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
int Hexaedre::contient(const ArrOfDouble& pos, int element ) const
{
  assert(pos.size_array()==3);
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  int som0 = zone.sommet_elem(element,0);
  int som7 = zone.sommet_elem(element,7);
  if (    inf_ou_egal(dom.coord(som0,0),pos[0]) && inf_ou_egal(pos[0],dom.coord(som7,0))
          && inf_ou_egal(dom.coord(som0,1),pos[1]) && inf_ou_egal(pos[1],dom.coord(som7,1))
          && inf_ou_egal(dom.coord(som0,2),pos[2]) && inf_ou_egal(pos[2],dom.coord(som7,2)) )
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
int Hexaedre::contient(const ArrOfInt& som, int element ) const
{
  const Zone& zone=ma_zone.valeur();
  if((zone.sommet_elem(element,0)==som[0])&&
      (zone.sommet_elem(element,1)==som[1])&&
      (zone.sommet_elem(element,2)==som[2])&&
      (zone.sommet_elem(element,3)==som[3])&&
      (zone.sommet_elem(element,4)==som[4])&&
      (zone.sommet_elem(element,5)==som[5])&&
      (zone.sommet_elem(element,6)==som[6])&&
      (zone.sommet_elem(element,7)==som[7]))
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
void Hexaedre::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  double dx,dy,dz;
  int S1,S2,S3,S4;

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = zone.sommet_elem(num_poly,0);
      S2 = zone.sommet_elem(num_poly,1);
      S3 = zone.sommet_elem(num_poly,2);
      S4 = zone.sommet_elem(num_poly,4);
      dx = dom.coord(S2,0) - dom.coord(S1,0);
      dy = dom.coord(S3,1) - dom.coord(S1,1);
      dz = dom.coord(S4,2) - dom.coord(S1,2);
      volumes[num_poly]= dx*dy*dz;
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
void Hexaedre::calculer_normales(const IntTab& Face_sommets ,DoubleTab& face_normales) const
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

      face_normales(numface,0) = (y1*z2 - y2*z1);
      face_normales(numface,1) = (-x1*z2 + x2*z1);
      face_normales(numface,2) = (x1*y2 - x2*y1);
    }
}


// Description: voir ElemGeomBase::get_tab_faces_sommets_locaux
int Hexaedre::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(6,4);
  for (int i=0; i<6; i++)
    for (int j=0; j<4; j++)
      faces_som_local(i,j) = faces_sommets_hexa[i][j];
  return 1;
}

// Description:
//    Renvoie le numero du j-ieme sommet de la i-ieme face de
//    l'element.
// Precondition:
// Parametre: int i
//    Signification: un numero de face
//    Valeurs par defaut:
//    Contraintes: i < 6
//    Acces:
// Parametre: int j
//    Signification: un numero de sommet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero du j-ieme sommet de la i-ieme face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Hexaedre::face_sommet(int i, int j) const
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
