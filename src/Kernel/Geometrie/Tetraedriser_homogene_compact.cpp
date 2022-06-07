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

#include <Tetraedriser_homogene_compact.h>

Implemente_instanciable(Tetraedriser_homogene_compact,"Tetraedriser_homogene_compact",Triangulation_base);

Sortie& Tetraedriser_homogene_compact::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Tetraedriser_homogene_compact::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Traitement des faces
//
void Tetraedriser_homogene_compact::decoupe(Zone& zone, Faces& faces, IntTab& new_soms_old_elems,IntTab& fait_sommet, int nface) const
{
  IntTab& sommets=faces.les_sommets();
  int nb_faces=sommets.dimension(0);
  assert(sommets.dimension(1)==4);
  IntTab nouveaux(4*nb_faces, 3);
  faces.voisins().resize(4*nb_faces, 2);
  faces.voisins()=-1;
  IntVect indice(5);

  for(int i=0; i<nb_faces; i++)
    {
      indice(0) = sommets(i,0);
      indice(1) = sommets(i,1);
      indice(2) = sommets(i,2);
      indice(3) = sommets(i,3);

      int i0=indice(0);
      int i1=indice(1);
      int i2=indice(2);
      int i3=indice(3);
      int i4=-1;

      // on remet un peu d'ordre dans les indices

      int permut,ind;

      do
        {
          permut=0;
          for(int j=1; j<4; j++)
            {
              if( indice[j]<indice[j-1] )
                {
                  ind=indice[j-1];
                  indice[j-1]=indice[j];
                  indice[j]=ind;
                  permut++;
                }
            }
        }
      while(permut);


      for (int ii=0 ; ii<nface ; ii++)
        {
          if ( fait_sommet(ii,0)==indice[0] && fait_sommet(ii,1)==indice[1] && fait_sommet(ii,2)==indice[2] )
            {
              i4=fait_sommet(ii,3);
              break;
            }
        }

      if(indice(4)<0)
        {
          //                Cerr << indice(0) << " " << indice(1) << " " << indice(2)  << finl;
          Cerr << " FALSE " << finl;
          exit();
        }
      assert(indice(4)>=0);

      nouveaux(i,0) = i0;
      nouveaux(i,1) = i1;
      nouveaux(i,2) = i4;

      nouveaux(nb_faces+i,0) = i1;
      nouveaux(nb_faces+i,1) = i3;
      nouveaux(nb_faces+i,2) = i4;

      nouveaux(2*nb_faces+i,0) = i3;
      nouveaux(2*nb_faces+i,1) = i2;
      nouveaux(2*nb_faces+i,2) = i4;

      nouveaux(3*nb_faces+i,0) = i2;
      nouveaux(3*nb_faces+i,1) = i0;
      nouveaux(3*nb_faces+i,2) = i4;

    }
  sommets.ref(nouveaux);
}

//
// ATTENTION: ne considere qu'une seule zone pour l'instant...
//
int Tetraedriser_homogene_compact::creer_sommet (
  Domaine& dom,
  Zone& zone,
  DoubleTab& new_soms,
  IntTab& elem_traite,
  IntTab& new_soms_old_elems,
  int NbSom, IntTab& sommets,
  int& compteur,
  int oldnbsom, int& nbnewsoms,IntTab& fait_sommet, int& nface ) const
{

  int _out=-1;
  if(NbSom==4) // On verifie que le sommet rattache a une face n'a pas ete cree ulterieurement
    {
      int permut,som;

      do
        {
          permut=0;
          for(int j=1; j<4; j++)
            {
              if( sommets[j]<sommets[j-1] )
                {
                  som=sommets[j-1];
                  sommets[j-1]=sommets[j];
                  sommets[j]=som;
                  permut++;
                }
            }
        }
      while(permut);



      for (int i=0 ; i<nface ; i++)
        if (fait_sommet(i,0)==sommets[0] && fait_sommet(i,1)==sommets[1] && fait_sommet(i,2)==sommets[2]) return fait_sommet(i,3);
    }

  const DoubleTab& coord_sommets=dom.coord_sommets();

  double x=0,y=0,z=0;

  for (int ii=0 ; ii<NbSom ; ii++)
    {
      x += coord_sommets(sommets(ii),0);
      y += coord_sommets(sommets(ii),1);
      z += coord_sommets(sommets(ii),2);
    }
  x /= NbSom;
  y /= NbSom;
  z /= NbSom;



  _out = oldnbsom + nbnewsoms;
  new_soms(compteur, 0) = x;
  new_soms(compteur, 1) = y;
  new_soms(compteur, 2) = z;
  nbnewsoms++;
  compteur++;

  if(NbSom==4)
    {
      fait_sommet(nface,  0)=sommets[0];
      fait_sommet(nface,  1)=sommets[1];
      fait_sommet(nface,  2)=sommets[2];
      fait_sommet(nface,  3)=_out;
      //                 Cerr << sommets(0) << " " << sommets(1) << " " << sommets(2) << " " << _out << finl;
      nface++;
    }

  return _out;
}

void Tetraedriser_homogene_compact::trianguler(Zone& zone) const
{
  if  ((zone.type_elem()->que_suis_je() == "Hexaedre")
       ||(zone.type_elem()->que_suis_je() == "Hexaedre_VEF"))
    {
      IntTab& les_elems = zone.les_elems();
      int oldsz = les_elems.dimension(0);
      int nbs = zone.nb_som();
      IntTab elem_traite(oldsz);
      int oldnbsom = zone.nb_som();
      IntTab new_elems(24*oldsz, 4);
      // pour chaque cube, liste des nouveaux sommets qu'il contient :
      IntTab new_soms_old_elems(oldsz,7);
      IntTab sommets(8);
      Domaine& dom = zone.domaine();
      int compteur = 0;
      int nbnewsoms = 0;
      int nface = 0;
      int i;

      Cerr << " NB ELEM : " << oldsz << " NB NODE : " << nbs << finl;
      IntTab fait_sommet(3*nbs,4); // PQ : 04-03 : dimensionnement de fait_sommet a 3*nbs (a revoir si insuffisant)
      fait_sommet=-1;


      // Construction de l'Octree sur la grille "VDF" de base

      zone.construit_octree();

      //On dimensionne une premiere fois le tableau des sommets avec la dimension maximun
      //puis on redimensionnera seulement a la fin par la dimension exacte

      DoubleTab& sommets_dom = dom.les_sommets();
      //8 pour les nouveaux sommets et 8 pour les anciens sommets =16
      int dim_som_max=16*oldsz;
      int dim_som_old=sommets_dom.dimension(0);
      sommets_dom.resize(dim_som_max,3);

      IntTab indice(8);
      DoubleTab new_soms(7, 3);

      for(i=0; i< oldsz; i++)
        {
          if (nface>=3*nbs)
            {
              Cerr << " The sizing of the array fait_sommet in Tetraedriser_homogene_compact::Trianguler is inappropriate " << finl;
              exit();
            }

          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);
          int i3=les_elems(i,3);
          int i4=les_elems(i,4);
          int i5=les_elems(i,5);
          int i6=les_elems(i,6);
          int i7=les_elems(i,7);

          compteur = 0;

          // Definition des nouveaux sommets : creation des barycentres
          //centre de l'hexaedre

          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i2;
          sommets(3)=i3;
          sommets(4)=i4;
          sommets(5)=i5;
          sommets(6)=i6;
          sommets(7)=i7;
          indice(0)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 8, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          //centres des faces bas (0-1-3-2)
          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i3;
          sommets(3)=i2;
          indice(1)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          //centres des faces haut (4-5-7-6)
          sommets(0)=i4;
          sommets(1)=i5;
          sommets(2)=i7;
          sommets(3)=i6;
          indice(4)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          //centres des faces avant (0-1-5-4)
          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i5;
          sommets(3)=i4;
          indice(3)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          //centres des faces arriere (2-3-7-6)
          sommets(0)=i2;
          sommets(1)=i3;
          sommets(2)=i7;
          sommets(3)=i6;
          indice(6)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          //centres des faces gauche (0-2-6-4)
          sommets(0)=i0;
          sommets(1)=i2;
          sommets(2)=i6;
          sommets(3)=i4;
          indice(2)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          //centres des faces droite (1-3-7-5)
          sommets(0)=i1;
          sommets(1)=i3;
          sommets(2)=i7;
          sommets(3)=i5;
          indice(5)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface);

          // Liste des nouveaux sommets pour cet ancien cube
          for (int t=0; t<7; t++)
            {
              new_soms_old_elems(i, t) = indice(t);
              if(indice(t)<0)
                {
                  Cerr << "Error negative index" << finl;
                  exit();
                }
              for(int s=0; s<t; s++)
                if(indice(s)==indice(t))
                  {
                    Cerr << "Error repeated index" << finl;
                    exit();
                  }
            }


          for(int j=0; j<compteur; j++)
            for(int k=0; k<3; k++)
              sommets_dom(dim_som_old+j,k)=new_soms(j,k) ;
          dim_som_old += compteur;

          // L'element en cours a ete "traite" en entier
          elem_traite(i) = 1;


          // pyramide bas (base : 0-1-3-2)

          new_elems(i, 0) = les_elems(i,0);
          new_elems(i, 1) = les_elems(i,1);
          new_elems(i, 2) = indice(1);
          new_elems(i, 3) = indice(0);

          new_elems(i+oldsz, 0) = les_elems(i,1);
          new_elems(i+oldsz, 1) = les_elems(i,3);
          new_elems(i+oldsz, 2) = indice(1);
          new_elems(i+oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+oldsz,1);

          new_elems(i+2*oldsz, 0) = les_elems(i,3);
          new_elems(i+2*oldsz, 1) = les_elems(i,2);
          new_elems(i+2*oldsz, 2) = indice(1);
          new_elems(i+2*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);

          new_elems(i+3*oldsz, 0) = les_elems(i,2);
          new_elems(i+3*oldsz, 1) = les_elems(i,0);
          new_elems(i+3*oldsz, 2) = indice(1);
          new_elems(i+3*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+3*oldsz,1);


          // pyramide haut (base : 4-5-7-6)

          new_elems(i+4*oldsz, 0) = les_elems(i,4);
          new_elems(i+4*oldsz, 1) = les_elems(i,5);
          new_elems(i+4*oldsz, 2) = indice(4);
          new_elems(i+4*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+4*oldsz,1);

          new_elems(i+5*oldsz, 0) = les_elems(i,5);
          new_elems(i+5*oldsz, 1) = les_elems(i,7);
          new_elems(i+5*oldsz, 2) = indice(4);
          new_elems(i+5*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+5*oldsz,1);

          new_elems(i+6*oldsz, 0) = les_elems(i,7);
          new_elems(i+6*oldsz, 1) = les_elems(i,6);
          new_elems(i+6*oldsz, 2) = indice(4);
          new_elems(i+6*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+6*oldsz,1);

          new_elems(i+7*oldsz, 0) = les_elems(i,6);
          new_elems(i+7*oldsz, 1) = les_elems(i,4);
          new_elems(i+7*oldsz, 2) = indice(4);
          new_elems(i+7*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+7*oldsz,1);


          // pyramide avant (base : 0-1-5-4)

          new_elems(i+8*oldsz, 0) = les_elems(i,0);
          new_elems(i+8*oldsz, 1) = les_elems(i,1);
          new_elems(i+8*oldsz, 2) = indice(3);
          new_elems(i+8*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+8*oldsz,1);

          new_elems(i+9*oldsz, 0) = les_elems(i,1);
          new_elems(i+9*oldsz, 1) = les_elems(i,5);
          new_elems(i+9*oldsz, 2) = indice(3);
          new_elems(i+9*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+9*oldsz,1);

          new_elems(i+10*oldsz, 0) = les_elems(i,5);
          new_elems(i+10*oldsz, 1) = les_elems(i,4);
          new_elems(i+10*oldsz, 2) = indice(3);
          new_elems(i+10*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+10*oldsz,1);

          new_elems(i+11*oldsz, 0) = les_elems(i,4);
          new_elems(i+11*oldsz, 1) = les_elems(i,0);
          new_elems(i+11*oldsz, 2) = indice(3);
          new_elems(i+11*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+11*oldsz,1);


          // pyramide arriere (base : 2-3-7-6)

          new_elems(i+12*oldsz, 0) = les_elems(i,2);
          new_elems(i+12*oldsz, 1) = les_elems(i,3);
          new_elems(i+12*oldsz, 2) = indice(6);
          new_elems(i+12*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+12*oldsz,1);

          new_elems(i+13*oldsz, 0) = les_elems(i,3);
          new_elems(i+13*oldsz, 1) = les_elems(i,7);
          new_elems(i+13*oldsz, 2) = indice(6);
          new_elems(i+13*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+13*oldsz,1);

          new_elems(i+14*oldsz, 0) = les_elems(i,7);
          new_elems(i+14*oldsz, 1) = les_elems(i,6);
          new_elems(i+14*oldsz, 2) = indice(6);
          new_elems(i+14*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+14*oldsz,1);

          new_elems(i+15*oldsz, 0) = les_elems(i,6);
          new_elems(i+15*oldsz, 1) = les_elems(i,2);
          new_elems(i+15*oldsz, 2) = indice(6);
          new_elems(i+15*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+15*oldsz,1);


          // pyramide gauche (base : 0-2-6-4)

          new_elems(i+16*oldsz, 0) = les_elems(i,0);
          new_elems(i+16*oldsz, 1) = les_elems(i,2);
          new_elems(i+16*oldsz, 2) = indice(2);
          new_elems(i+16*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+16*oldsz,1);

          new_elems(i+17*oldsz, 0) = les_elems(i,2);
          new_elems(i+17*oldsz, 1) = les_elems(i,6);
          new_elems(i+17*oldsz, 2) = indice(2);
          new_elems(i+17*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+17*oldsz,1);

          new_elems(i+18*oldsz, 0) = les_elems(i,6);
          new_elems(i+18*oldsz, 1) = les_elems(i,4);
          new_elems(i+18*oldsz, 2) = indice(2);
          new_elems(i+18*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+18*oldsz,1);

          new_elems(i+19*oldsz, 0) = les_elems(i,4);
          new_elems(i+19*oldsz, 1) = les_elems(i,0);
          new_elems(i+19*oldsz, 2) = indice(2);
          new_elems(i+19*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+19*oldsz,1);


          // pyramide droite (base : 1-3-7-5)

          new_elems(i+20*oldsz, 0) = les_elems(i,1);
          new_elems(i+20*oldsz, 1) = les_elems(i,3);
          new_elems(i+20*oldsz, 2) = indice(5);
          new_elems(i+20*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+20*oldsz,1);

          new_elems(i+21*oldsz, 0) = les_elems(i,3);
          new_elems(i+21*oldsz, 1) = les_elems(i,7);
          new_elems(i+21*oldsz, 2) = indice(5);
          new_elems(i+21*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+21*oldsz,1);

          new_elems(i+22*oldsz, 0) = les_elems(i,7);
          new_elems(i+22*oldsz, 1) = les_elems(i,5);
          new_elems(i+22*oldsz, 2) = indice(5);
          new_elems(i+22*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+22*oldsz,1);

          new_elems(i+23*oldsz, 0) = les_elems(i,5);
          new_elems(i+23*oldsz, 1) = les_elems(i,1);
          new_elems(i+23*oldsz, 2) = indice(5);
          new_elems(i+23*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+23*oldsz,1);

        }
      sommets_dom.resize(dim_som_old,3);
      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      Cerr << "We have split the cubes..." << finl;
      zone.invalide_octree();
      zone.typer("Tetraedre");

      {
        Cerr << "Splitting of the boundaries" << finl;
        LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
        while(curseur)
          {
            Faces& les_faces=curseur->faces();
            les_faces.typer(Faces::triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems, fait_sommet, nface);
            ++curseur;
          }
      }


      {
        // Les Raccords
        Cerr << "Splitting of the connectors" << finl;
        LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());;;
        while(curseur)
          {
            Faces& les_faces=curseur.valeur()->faces();
            les_faces.typer(Faces::triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems, fait_sommet, nface);
            ++curseur;
          }
      }

      {
        // Les Faces internes
        Cerr << "Splitting of the internal faces" << finl;
        LIST_CURSEUR(Faces_Interne) curseur(zone.faces_int());;;
        while(curseur)
          {
            Faces& les_faces=curseur->faces();
            les_faces.typer(Faces::triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems, fait_sommet, nface);
            ++curseur;
          }
      }

      Cerr<<"END of Tetraedriser_homogene_compact..."<<finl;
      Cerr<<"  1 NbElem="<<zone.les_elems().dimension(0)<<"  NbNod="<<zone.nb_som()<<finl;
    }
  else
    {
      Cerr << "We do not yet know how to Tetraedriser_homogene_compact the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
    }
}


