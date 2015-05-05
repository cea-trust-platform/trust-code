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
// File:        Trianguler_fin.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Trianguler_fin.h>
#include <Domaine.h>

Implemente_instanciable(Trianguler_fin,"Trianguler_fin",Triangulation_base);


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
Sortie& Trianguler_fin::printOn(Sortie& os) const
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
Entree& Trianguler_fin::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Triangule tous les element d'une zone: transforme
//    les elements goemetriques de la zone en triangles.
//    Pour l'instant on ne sait trianguler que des Rectangles
//    (on les coupe en 8).
// Precondition:
// Parametre: Zone& zone
//    Signification: la zone dont on veut trianguler les elements
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Trianguler_fin::trianguler(Zone& zone) const
{
  Domaine& dom=zone.domaine();
  const DoubleTab& xs=dom.coord_sommets();
  IntTab& les_elems=zone.les_elems();
  int oldsz=les_elems.dimension(0);
  DoubleTab& sommets=dom.les_sommets();
  int nbs=sommets.dimension(0);

  {
    DoubleTab sommets_ajoutes(oldsz, dimension);
    zone.type_elem()->calculer_centres_gravite(sommets_ajoutes);
    sommets.resize(4*nbs, dimension);
    for(int i=0; i<oldsz; i++)
      for(int j=0; j<dimension; j++)
        sommets(nbs+i,j)=sommets_ajoutes(i,j);
  }

  if ((zone.type_elem()->que_suis_je() == "Rectangle") || (zone.type_elem()->que_suis_je() == "Quadrangle"))
    {
      zone.typer("Triangle");
      IntTab new_elems(8*oldsz,3);
      IntTab fait_sommet_arete(4*nbs,3);
      fait_sommet_arete=-1;
      int j=0;

      for(int i=0; i< oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);
          int i3=les_elems(i,3);
          int i01=-1;
          int i02=-1;
          int i13=-1;
          int i23=-1;

          for (int ii=0 ; ii<j ; ii++)
            {
              if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i1 ) i01=nbs+oldsz+ii ;
              if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i2 ) i02=nbs+oldsz+ii ;
              if ( fait_sommet_arete(ii,0)==i1 && fait_sommet_arete(ii,1)==i3 ) i13=nbs+oldsz+ii ;
              if ( fait_sommet_arete(ii,0)==i2 && fait_sommet_arete(ii,1)==i3 ) i23=nbs+oldsz+ii ;
            }

          if(i01==-1)
            {
              fait_sommet_arete(j,0)=i0;
              fait_sommet_arete(j,1)=i1;
              i01=nbs+oldsz+j;
              sommets(i01,0)=0.5*(xs(i0,0)+xs(i1,0));
              sommets(i01,1)=0.5*(xs(i0,1)+xs(i1,1));
              j++;
            }

          if(i02==-1)
            {
              fait_sommet_arete(j,0)=i0;
              fait_sommet_arete(j,1)=i2;
              i02=nbs+oldsz+j;
              sommets(i02,0)=0.5*(xs(i0,0)+xs(i2,0));
              sommets(i02,1)=0.5*(xs(i0,1)+xs(i2,1));
              j++;
            }

          if(i13==-1)
            {
              fait_sommet_arete(j,0)=i1;
              fait_sommet_arete(j,1)=i3;
              i13=nbs+oldsz+j;
              sommets(i13,0)=0.5*(xs(i1,0)+xs(i3,0));
              sommets(i13,1)=0.5*(xs(i1,1)+xs(i3,1));
              j++;
            }

          if(i23==-1)
            {
              fait_sommet_arete(j,0)=i2;
              fait_sommet_arete(j,1)=i3;
              i23=nbs+oldsz+j;
              sommets(i23,0)=0.5*(xs(i2,0)+xs(i3,0));
              sommets(i23,1)=0.5*(xs(i2,1)+xs(i3,1));
              j++;
            }

          new_elems(i,0)=    i+nbs;
          new_elems(i,1)=    i0;
          new_elems(i,2)=    i01;

          new_elems(i+oldsz,0)=  i+nbs;
          new_elems(i+oldsz,1)=  i01;
          new_elems(i+oldsz,2)=  i1;
          mettre_a_jour_sous_zone(zone,i,i+oldsz,1);

          new_elems(i+2*oldsz,0)=  i+nbs;
          new_elems(i+2*oldsz,1)=  i1;
          new_elems(i+2*oldsz,2)=  i13;
          mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);

          new_elems(i+3*oldsz,0)=  i+nbs;
          new_elems(i+3*oldsz,1)=  i13;
          new_elems(i+3*oldsz,2)=  i3;
          mettre_a_jour_sous_zone(zone,i,i+3*oldsz,1);

          new_elems(i+4*oldsz,0)=  i+nbs;
          new_elems(i+4*oldsz,1)=  i3;
          new_elems(i+4*oldsz,2)=  i23;
          mettre_a_jour_sous_zone(zone,i,i+4*oldsz,1);

          new_elems(i+5*oldsz,0)=  i+nbs;
          new_elems(i+5*oldsz,1)=  i23;
          new_elems(i+5*oldsz,2)=  i2;
          mettre_a_jour_sous_zone(zone,i,i+5*oldsz,1);

          new_elems(i+6*oldsz,0)=  i+nbs;
          new_elems(i+6*oldsz,1)=  i2;
          new_elems(i+6*oldsz,2)=  i02;
          mettre_a_jour_sous_zone(zone,i,i+6*oldsz,1);

          new_elems(i+7*oldsz,0)=  i+nbs;
          new_elems(i+7*oldsz,1)=  i02;
          new_elems(i+7*oldsz,2)=  i0;
          mettre_a_jour_sous_zone(zone,i,i+7*oldsz,1);

        }

      if(nbs+oldsz+j>4*nbs)
        {
          Cerr << " Review the sizing of sommets() in Trianguler_fin " << finl;
          exit();
        }
      sommets.resize(nbs+oldsz+j,dimension);
      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      Cerr << "We have split the rectangles..." << endl;
      zone.invalide_octree();
      zone.typer("Triangle");
      Cerr << "  Reconstruction of the Octree" << finl;
      zone.construit_octree();
      Cerr << "  Octree rebuilt" << finl;

      {
        Cerr << "Splitting of the boundaries" << finl;
        LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
        while(curseur)
          {
            Faces& les_faces=curseur->faces();
            les_faces.typer(segment_2D);
            IntTab& sommetsfaces=les_faces.les_sommets();
            int nb_faces=sommetsfaces.dimension(0);
            IntTab nouveaux(2*nb_faces,2);
            les_faces.voisins().resize(2*nb_faces,2);
            les_faces.voisins()=-1;

            for(int i=0; i<nb_faces; i++)
              {
                int i0=sommetsfaces(i,0);
                int i1=sommetsfaces(i,1);
                int i01=-1;
                for (int ii=0 ; ii<nbs+oldsz+j ; ii++)
                  if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i1 ) i01=nbs+oldsz+ii ;
                assert(i01>=0);
                nouveaux(2*i  ,0)=i0;
                nouveaux(2*i  ,1)=i01;
                nouveaux(2*i+1,0)=i01;
                nouveaux(2*i+1,1)=i1;
              }
            sommetsfaces.ref(nouveaux);
            ++curseur;
          }
      }
      {
        Cerr << "Splitting of the connectors" << finl;
        LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());;;
        while(curseur)
          {
            Faces& les_faces=curseur.valeur()->faces();
            les_faces.typer(segment_2D);
            IntTab& sommetsfaces=les_faces.les_sommets();
            int nb_faces=sommetsfaces.dimension(0);
            IntTab nouveaux(2*nb_faces,2);
            les_faces.voisins().resize(2*nb_faces,2);
            les_faces.voisins()=-1;

            for(int i=0; i<nb_faces; i++)
              {
                int i0=sommetsfaces(i,0);
                int i1=sommetsfaces(i,1);
                int i01=-1;
                for (int ii=0 ; ii<nbs+oldsz+j ; ii++)
                  if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i1 ) i01=nbs+oldsz+ii ;
                assert(i01>=0);
                nouveaux(2*i  ,0)=i0;
                nouveaux(2*i  ,1)=i01;
                nouveaux(2*i+1,0)=i01;
                nouveaux(2*i+1,1)=i1;
              }
            sommetsfaces.ref(nouveaux);
            ++curseur;
          }
      }
      {
        Cerr << "Splitting of the internal faces" << finl;
        LIST_CURSEUR(Faces_Interne) curseur(zone.faces_int());;;
        while(curseur)
          {
            Faces& les_faces=curseur->faces();
            les_faces.typer(segment_2D);
            IntTab& sommetsfaces=les_faces.les_sommets();
            int nb_faces=sommetsfaces.dimension(0);
            IntTab nouveaux(2*nb_faces,2);
            les_faces.voisins().resize(2*nb_faces,2);
            les_faces.voisins()=-1;

            for(int i=0; i<nb_faces; i++)
              {
                int i0=sommetsfaces(i,0);
                int i1=sommetsfaces(i,1);
                int i01=-1;
                for (int ii=0 ; ii<nbs+oldsz+j ; ii++)
                  if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i1 ) i01=nbs+oldsz+ii ;
                assert(i01>=0);
                nouveaux(2*i  ,0)=i0;
                nouveaux(2*i  ,1)=i01;
                nouveaux(2*i+1,0)=i01;
                nouveaux(2*i+1,1)=i1;
              }
            sommetsfaces.ref(nouveaux);
            ++curseur;
          }
      }
    }
  else
    {
      Cerr << "We do not yet know how to Trianguler_fin the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
      exit();
    }
}

