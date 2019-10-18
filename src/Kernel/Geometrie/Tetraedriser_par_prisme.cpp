/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Tetraedriser_par_prisme.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Tetraedriser_par_prisme.h>
#include <Domaine.h>

Implemente_instanciable(Tetraedriser_par_prisme,"Tetraedriser_par_prisme",Triangulation_base);


// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Tetraedriser_par_prisme::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Tetraedriser_par_prisme::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction hors classe
//    Decoupe toutes les faces d'un objet Faces
//    dont les faces ont 4 sommets
//    en 2 faces a 3 sommets.
// Precondition: les faces doivent avoir 4 sommets
// Parametre: Faces& faces
//    Signification: l'ensemble des faces a decouper
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
static void decoupe(Zone& zone, Faces& faces)
{

  const Domaine& dom=zone.domaine();
  const DoubleTab& coord=dom.coord_sommets();


  IntTab& sommets=faces.les_sommets();
  int nb_faces=sommets.dimension(0);
  assert(sommets.dimension(1)==4);
  IntTab nouveaux(2*nb_faces, 3);
  faces.voisins().resize(2*nb_faces, 2);
  faces.voisins()=-1;
  for(int i=0; i<nb_faces; i++)
    {
      int i1 = sommets(i,0);
      int i2 = sommets(i,1);
      int i3 = sommets(i,2);
      int i4 = sommets(i,3);

      // C'est une face a X constant
      if((coord(i1,0)==coord(i2,0)) && (coord(i1,0)==coord(i3,0)))
        {
          nouveaux(i,0) = i1;
          nouveaux(i,1) = i2;
          nouveaux(i,2) = i3;
          nouveaux(nb_faces+i,0) = i2;
          nouveaux(nb_faces+i,1) = i3;
          nouveaux(nb_faces+i,2) = i4;
        }

      // C'est une face a Z constant
      if((coord(i1,2)==coord(i2,2)) && (coord(i1,2)==coord(i3,2)))
        {
          nouveaux(i,0) = i1;
          nouveaux(i,1) = i2;
          nouveaux(i,2) = i3;
          nouveaux(nb_faces+i,0) = i2;
          nouveaux(nb_faces+i,1) = i3;
          nouveaux(nb_faces+i,2) = i4;
        }
      // C'est une face a Y constant
      if((coord(i1,1)==coord(i2,1)) && (coord(i1,1)==coord(i3,1)))
        {
          nouveaux(i,0) = i1;
          nouveaux(i,1) = i2;
          nouveaux(i,2) = i4;
          nouveaux(nb_faces+i,0) = i1;
          nouveaux(nb_faces+i,1) = i3;
          nouveaux(nb_faces+i,2) = i4;
        }

    }
  sommets.ref(nouveaux);
}


// Description:
//    Tetraedrise tous les elements d'une zone : transforme
//    les elements goemetriques de la zone en tetraedres IDENTIQUES
//    par la methode des prismes.
//    Pour l'instant on ne sait tetraedriser que des Hexaedre.
//    (on les coupe en 2).
//    Les elements sont tetraedrises et tous les bords
//    sont types en Triangle_3D.
// Precondition:
// Parametre: Zone& zone
//    Signification: la zone dont on veut tetraedriser les elements
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception: on ne sait pas Tetraedriser par prisme les elements
//            geometriques de ce type
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Tetraedriser_par_prisme::trianguler(Zone& zone) const
{
  if(zone.type_elem()->que_suis_je() == "Hexaedre")
    {
      zone.typer("Tetraedre");
      IntTab& les_elems=zone.les_elems();
      int oldsz=les_elems.dimension(0);
      IntTab new_elems(6*oldsz, 4);
      for(int i=0; i< oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);
          int i3=les_elems(i,3);
          int i4=les_elems(i,4);
          int i5=les_elems(i,5);
          int i6=les_elems(i,6);
          int i7=les_elems(i,7);
          {
            new_elems(i,0)=i0;
            new_elems(i,1)=i1;
            new_elems(i,2)=i2;
            new_elems(i,3)=i5;

            new_elems(i+oldsz,0)=i0;
            new_elems(i+oldsz,1)=i2;
            new_elems(i+oldsz,2)=i4;
            new_elems(i+oldsz,3)=i5;
            mettre_a_jour_sous_zone(zone,i,i+oldsz,1);

            new_elems(i+2*oldsz,0)=i1;
            new_elems(i+2*oldsz,1)=i2;
            new_elems(i+2*oldsz,2)=i3;
            new_elems(i+2*oldsz,3)=i5;
            mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);

            new_elems(i+3*oldsz,0)=i2;
            new_elems(i+3*oldsz,1)=i3;
            new_elems(i+3*oldsz,2)=i5;
            new_elems(i+3*oldsz,3)=i7;
            mettre_a_jour_sous_zone(zone,i,i+3*oldsz,1);

            new_elems(i+4*oldsz,0)=i2;
            new_elems(i+4*oldsz,1)=i4;
            new_elems(i+4*oldsz,2)=i5;
            new_elems(i+4*oldsz,3)=i6;
            mettre_a_jour_sous_zone(zone,i,i+4*oldsz,1);

            new_elems(i+5*oldsz,0)=i2;
            new_elems(i+5*oldsz,1)=i5;
            new_elems(i+5*oldsz,2)=i6;
            new_elems(i+5*oldsz,3)=i7;
            mettre_a_jour_sous_zone(zone,i,i+5*oldsz,1);
          }
        }
      les_elems.ref(new_elems);
    }
  else
    {
      Cerr << "We do not yet know how to Tetraedriser_par_prisme the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
    }
  {
    LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
    while(curseur)
      {
        Faces& les_faces=curseur->faces();
        les_faces.typer(Faces::triangle_3D);
        //        decoupe(les_faces);
        decoupe(zone,les_faces);
        ++curseur;
      }
  }

  {
    // Les Raccords
    LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());;;
    while(curseur)
      {
        Faces& les_faces=curseur.valeur()->faces();
        les_faces.typer(Faces::triangle_3D);
        //        decoupe(les_faces);
        decoupe(zone,les_faces);
        ++curseur;
      }
  }
  {
    // Les Faces internes
    LIST_CURSEUR(Faces_Interne) curseur(zone.faces_int());;;
    while(curseur)
      {
        Faces& les_faces=curseur->faces();
        les_faces.typer(Faces::triangle_3D);
        //        decoupe(les_faces);
        decoupe(zone,les_faces);
        ++curseur;
      }
  }

}

