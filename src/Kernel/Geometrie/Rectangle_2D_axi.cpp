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
// File:        Rectangle_2D_axi.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Rectangle_2D_axi.h>
#include <Domaine.h>
#include <math.h>
Implemente_instanciable(Rectangle_2D_axi,"Rectangle_2D_axi",Rectangle);


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
Sortie& Rectangle_2D_axi::printOn(Sortie& s ) const
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
Entree& Rectangle_2D_axi::readOn(Entree& s )
{
  return s;
}

//Description:
//    Renvoie le nom LML d'un rectangle_2D_axi = "VOXEL8".
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
const Nom& Rectangle_2D_axi::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}

// Description:
//    NE FAIT RIEN
//    A CODER
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
void Rectangle_2D_axi::reordonner()
{
  Rectangle::reordonner();
  // A CODER
}


// Description:
//    Calcule les centres de gravites de tous les elements
//    de la zone associee a l'element goemetrique.
// Precondition:
// Parametre: DoubleTab& xp
//    Signification: le tableau contenant les coordonnees des
//                   centres de gravite
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Rectangle_2D_axi::calculer_centres_gravite(DoubleTab& xp) const
{
  /*
    const IntTab& les_Polys = ma_zone->les_elems();
    const Domaine& le_domaine = ma_zone->domaine();
    int nb_elem = ma_zone->nb_elem_tot();
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
void Rectangle_2D_axi::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();// zone.valeur() permet d'acceder a                                            // la zone qui est en cours de traitement
  const Domaine& dom=zone.domaine();
  double r,r1,r2,dr,dz;
  int S1,S2,S3;
  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = zone.sommet_elem(num_poly,0);
      S2 = zone.sommet_elem(num_poly,1);
      S3 = zone.sommet_elem(num_poly,2);
      r1= dom.coord(S1,0);
      r2= dom.coord(S2,0);
      if (r1< r2) r =r1;
      else r =r2;
      dr =dabs( dom.coord(S2,0) - dom.coord(S1,0));
      dz =dabs( dom.coord(S3,1) - dom.coord(S1,1));
      volumes[num_poly]= 2*M_PI*(r*dr+0.5*dr*dr)*dz;
    }
  /*
    Rectangle::calculer_volumes(volumes);
  */
}
