/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <Tetraedriser_homogene_fin.h>
#include <Domaine.h>

Implemente_instanciable(Tetraedriser_homogene_fin,"Tetraedriser_homogene_fin",Triangulation_base);

Sortie& Tetraedriser_homogene_fin::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Tetraedriser_homogene_fin::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Traitement des faces
//
void Tetraedriser_homogene_fin::decoupe(Zone& zone, Faces& faces, IntTab& new_soms_old_elems,IntTab& fait_sommet, int nface, IntTab& fait_sommet_arete, int narete) const
{
  IntTab& sommets=faces.les_sommets();
  int nb_faces=sommets.dimension(0);
  assert(sommets.dimension(1)==4);
  IntTab nouveaux(8*nb_faces, 3);
  faces.voisins().resize(8*nb_faces, 2);
  faces.voisins()=-1;
  IntVect indice(5);


  for(int i=0; i<nb_faces; i++)
    {
      indice(0) = sommets(i,0);
      indice(1) = sommets(i,1);
      indice(2) = sommets(i,2);
      indice(3) = sommets(i,3);

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


      int i0=indice(0);
      int i1=indice(1);
      int i2=indice(2);
      int i3=indice(3);
      int i4=-1;

      for (int ii=0 ; ii<nface ; ii++)
        {
          if ( fait_sommet(ii,0)==i0 && fait_sommet(ii,1)==i1 && fait_sommet(ii,2)==i2 )
            {
              i4=fait_sommet(ii,3);
              break;
            }
        }

      if(i4<0)
        {
          Cerr << " FALSE " << finl;
          exit();
        }
      assert(i4>=0);

      int jj=0;

      for (int ii=0 ; ii<narete ; ii++)
        {
          if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i1)
            {
              nouveaux(jj*nb_faces+i,0) = i0;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;

              nouveaux(jj*nb_faces+i,0) = i1;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;
            }

          if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i2)
            {
              nouveaux(jj*nb_faces+i,0) = i0;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;

              nouveaux(jj*nb_faces+i,0) = i2;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;
            }

          if ( fait_sommet_arete(ii,0)==i0 && fait_sommet_arete(ii,1)==i3)
            {
              nouveaux(jj*nb_faces+i,0) = i0;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;

              nouveaux(jj*nb_faces+i,0) = i3;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;
            }

          if ( fait_sommet_arete(ii,0)==i1 && fait_sommet_arete(ii,1)==i2)
            {
              nouveaux(jj*nb_faces+i,0) = i1;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;

              nouveaux(jj*nb_faces+i,0) = i2;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;
            }

          if ( fait_sommet_arete(ii,0)==i1 && fait_sommet_arete(ii,1)==i3)
            {
              nouveaux(jj*nb_faces+i,0) = i1;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;

              nouveaux(jj*nb_faces+i,0) = i3;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;
            }

          if ( fait_sommet_arete(ii,0)==i2 && fait_sommet_arete(ii,1)==i3)
            {
              nouveaux(jj*nb_faces+i,0) = i2;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;

              nouveaux(jj*nb_faces+i,0) = i3;
              nouveaux(jj*nb_faces+i,1) = fait_sommet_arete(ii,2);
              nouveaux(jj*nb_faces+i,2) = i4;
              jj++;
            }
        }

      //                if(jj!=8)
      //                {
      //                Cerr << " il manque des nouvelles faces " << finl;
      //                exit();
      //                }
      assert(jj==8);


    }
  sommets.ref(nouveaux);
}

int Tetraedriser_homogene_fin::creer_sommet (
  Domaine& dom,
  Zone& zone,
  DoubleTab& new_soms,
  IntTab& elem_traite,
  IntTab& new_soms_old_elems,
  int NbSom, IntTab& sommets,
  int& compteur,
  int oldnbsom, int& nbnewsoms,IntTab& fait_sommet, int& nface,IntTab& fait_sommet_arete, int& narete ) const
{

  const DoubleTab& coord_sommets=dom.coord_sommets();
  //int nb_zones = dom.nb_zones();
  int _out;


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

    }

  if(NbSom==2) // On verifie que le sommet rattache a une arete n'a pas ete cree ulterieurement
    {
      int permut,som;

      do
        {
          permut=0;
          for(int j=1; j<2; j++)
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

    }

  double x=0,y=0,z=0;
  //x = 0.5*(coord_sommets(i1,0)+coord_sommets(i2,0));
  //y = 0.5*(coord_sommets(i1,1)+coord_sommets(i2,1));
  //z = 0.5*(coord_sommets(i1,2)+coord_sommets(i2,2));
  for (int ii=0 ; ii<NbSom ; ii++)
    {
      x += coord_sommets(sommets(ii),0);
      y += coord_sommets(sommets(ii),1);
      z += coord_sommets(sommets(ii),2);
    }
  x /= NbSom;
  y /= NbSom;
  z /= NbSom;

  int trouve = -1;

  trouve = zone.chercher_elements(x,y,z);

  if (trouve<0)
    {
      for (int ii=0 ; ii<NbSom ; ii++)
        {
          Cerr<<"i"<<ii<<"="<<sommets(ii)<<" coord="<<coord_sommets(sommets(ii),0)<<" "<<coord_sommets(sommets(ii),1)<<" "<<coord_sommets(sommets(ii),2)<<finl;
        }
      Cerr<<"x="<<x<<" y="<<y<<" z="<<z<<finl;
      Cerr<<"-----"<<finl;
      assert(trouve>=0);
    }

  if (elem_traite(trouve))
    {
      _out = -1;
      int j = 0;
      double epsilon = Objet_U::precision_geom;
      do
        {

          if ((std::fabs(coord_sommets(new_soms_old_elems(trouve, j), 0)-x)<epsilon) &&
              (std::fabs(coord_sommets(new_soms_old_elems(trouve, j), 1)-y)<epsilon)&&
              (std::fabs(coord_sommets(new_soms_old_elems(trouve, j), 2)-z)<epsilon))

            {
              _out = new_soms_old_elems(trouve, j);
            }
          j++;
        }
      while ((_out == -1) && (j<19));
      if (_out<0)
        {
          Cerr << "Error, not found !" << finl;
          Cerr << "found="<<trouve<<" x="<<x<<" y="<<y<<" z="<<z<<finl;
          for (int ii=0 ; ii<NbSom ; ii++)
            {
              Cerr<<"i"<<ii<<"="<<sommets(ii)<<" coord="<<coord_sommets(sommets(ii),0)<<" "<<coord_sommets(sommets(ii),1)<<" "<<coord_sommets(sommets(ii),2)<<finl;
            }
          exit();
        }
    }
  else
    {

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

      if(NbSom==2)
        {
          fait_sommet_arete(narete,  0)=sommets[0];
          fait_sommet_arete(narete,  1)=sommets[1];
          fait_sommet_arete(narete,  2)=_out;
          //                 Cerr << sommets(0) << " " << sommets(1) << " " << _out << finl;
          narete++;
        }
    }

  if(_out<0)
    {
      Cerr << "Problem at the creation of node" << finl;
      exit();
    }
  return _out;

}


void Tetraedriser_homogene_fin::trianguler(Zone& zone) const
{
  if  ((zone.type_elem()->que_suis_je() == "Hexaedre")
       ||(zone.type_elem()->que_suis_je() == "Hexaedre_VEF"))
    {
      IntTab& les_elems = zone.les_elems();
      int oldsz = les_elems.dimension(0);
      int nbs = zone.nb_som();
      IntTab elem_traite(oldsz);
      int oldnbsom = zone.nb_som();
      IntTab new_elems(48*oldsz, 4);
      // pour chaque cube, liste des nouveaux sommets qu'il contient :
      IntTab new_soms_old_elems(oldsz,19);
      IntTab sommets(8);
      Domaine& dom = zone.domaine();
      int compteur = 0;
      int nbnewsoms = 0;
      int nface = 0;
      int narete = 0;
      int i;

      Cerr << " NB ELEM : " << oldsz << " NB NODE : " << nbs << finl;
      IntTab fait_sommet(3*nbs,4); // PQ : 04-03 : dimensionnement de fait_sommet a 3*nbs (a revoir si insuffisant)
      IntTab fait_sommet_arete(3*nbs,3); // PQ : 02-05 : dimensionnement de fait_sommet_arete a 3*nbs (a revoir si insuffisant)
      fait_sommet=-1;
      fait_sommet_arete=-1;


      // Construction de l'Octree sur la grille "VDF" de base
      zone.construit_octree();

      //On dimensionne une premiere fois le tableau des sommets avec la dimension maximun
      //puis on redimensionnera seulement a la fin par la dimension exacte

      DoubleTab& sommets_dom = dom.les_sommets();
      //int dim_som_max=19*oldsz*40;
      //19 pour les nouveaux sommets et 8 pour les anciens sommets = 27
      int dim_som_max=27*oldsz;
      int dim_som_old=sommets_dom.dimension(0);
      sommets_dom.resize(dim_som_max,3);

      IntTab indice(19);
      DoubleTab new_soms(19,3);

      for(i=0; i< oldsz; i++)
        {
          if (nface>=3*nbs)
            {
              Cerr << " The sizing of the array fait_sommet in Tetraedriser_homogene_fin::Trianguler is inappropriate " << finl;
              exit();
            }
          if (narete>=3*nbs)
            {
              Cerr << " The sizing of the array fait_sommet_arete in Tetraedriser_homogene_fin::Trianguler is inappropriate " << finl;
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
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 8,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des faces bas (0-1-3-2)
          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i3;
          sommets(3)=i2;
          indice(1)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des faces haut (4-5-7-6)
          sommets(0)=i4;
          sommets(1)=i5;
          sommets(2)=i7;
          sommets(3)=i6;
          indice(4)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des faces avant (0-1-5-4)
          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i5;
          sommets(3)=i4;
          indice(3)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des faces arriere (2-3-7-6)
          sommets(0)=i2;
          sommets(1)=i3;
          sommets(2)=i7;
          sommets(3)=i6;
          indice(6)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des faces gauche (0-2-6-4)
          sommets(0)=i0;
          sommets(1)=i2;
          sommets(2)=i6;
          sommets(3)=i4;
          indice(2)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des faces droite (1-3-7-5)
          sommets(0)=i1;
          sommets(1)=i3;
          sommets(2)=i7;
          sommets(3)=i5;
          indice(5)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (0-1)
          sommets(0)=i0;
          sommets(1)=i1;
          indice(7)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (1-3)
          sommets(0)=i1;
          sommets(1)=i3;
          indice(8)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (3-2)
          sommets(0)=i3;
          sommets(1)=i2;
          indice(9)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (2-0)
          sommets(0)=i2;
          sommets(1)=i0;
          indice(10)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (1-5)
          sommets(0)=i1;
          sommets(1)=i5;
          indice(11)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (3-7)
          sommets(0)=i3;
          sommets(1)=i7;
          indice(12)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (2-6)
          sommets(0)=i2;
          sommets(1)=i6;
          indice(13)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (0-4)
          sommets(0)=i0;
          sommets(1)=i4;
          indice(14)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (4-5)
          sommets(0)=i4;
          sommets(1)=i5;
          indice(15)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (5-7)
          sommets(0)=i5;
          sommets(1)=i7;
          indice(16)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (7-6)
          sommets(0)=i7;
          sommets(1)=i6;
          indice(17)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          //centres des aretes (6-4)
          sommets(0)=i6;
          sommets(1)=i4;
          indice(18)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2,
                         sommets, compteur, oldnbsom, nbnewsoms, fait_sommet, nface, fait_sommet_arete, narete);

          // Liste des nouveaux sommets pour cet ancien cube
          for (int t=0; t<19; t++)
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
          new_elems(i, 1) = indice(7);
          new_elems(i, 2) = indice(1);
          new_elems(i, 3) = indice(0);

          new_elems(i+oldsz, 0) = indice(7);
          new_elems(i+oldsz, 1) = les_elems(i,1);
          new_elems(i+oldsz, 2) = indice(1);
          new_elems(i+oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+oldsz,1);

          new_elems(i+2*oldsz, 0) = les_elems(i,1);
          new_elems(i+2*oldsz, 1) = indice(8);
          new_elems(i+2*oldsz, 2) = indice(1);
          new_elems(i+2*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);

          new_elems(i+3*oldsz, 0) = indice(8);
          new_elems(i+3*oldsz, 1) = les_elems(i,3);
          new_elems(i+3*oldsz, 2) = indice(1);
          new_elems(i+3*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+3*oldsz,1);

          new_elems(i+4*oldsz, 0) = les_elems(i,3);
          new_elems(i+4*oldsz, 1) = indice(9);
          new_elems(i+4*oldsz, 2) = indice(1);
          new_elems(i+4*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+4*oldsz,1);

          new_elems(i+5*oldsz, 0) = indice(9);
          new_elems(i+5*oldsz, 1) = les_elems(i,2);
          new_elems(i+5*oldsz, 2) = indice(1);
          new_elems(i+5*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+5*oldsz,1);

          new_elems(i+6*oldsz, 0) = les_elems(i,2);
          new_elems(i+6*oldsz, 1) = indice(10);
          new_elems(i+6*oldsz, 2) = indice(1);
          new_elems(i+6*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+6*oldsz,1);

          new_elems(i+7*oldsz, 0) = indice(10);
          new_elems(i+7*oldsz, 1) = les_elems(i,0);
          new_elems(i+7*oldsz, 2) = indice(1);
          new_elems(i+7*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+7*oldsz,1);


          // pyramide haut (base : 4-5-7-6)

          new_elems(i+8*oldsz, 0) = les_elems(i,4);
          new_elems(i+8*oldsz, 1) = indice(15);
          new_elems(i+8*oldsz, 2) = indice(4);
          new_elems(i+8*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+8*oldsz,1);

          new_elems(i+9*oldsz, 0) = indice(15);
          new_elems(i+9*oldsz, 1) = les_elems(i,5);
          new_elems(i+9*oldsz, 2) = indice(4);
          new_elems(i+9*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+9*oldsz,1);

          new_elems(i+10*oldsz, 0) = les_elems(i,5);
          new_elems(i+10*oldsz, 1) = indice(16);
          new_elems(i+10*oldsz, 2) = indice(4);
          new_elems(i+10*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+10*oldsz,1);

          new_elems(i+11*oldsz, 0) = indice(16);
          new_elems(i+11*oldsz, 1) = les_elems(i,7);
          new_elems(i+11*oldsz, 2) = indice(4);
          new_elems(i+11*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+11*oldsz,1);

          new_elems(i+12*oldsz, 0) = les_elems(i,7);
          new_elems(i+12*oldsz, 1) = indice(17);
          new_elems(i+12*oldsz, 2) = indice(4);
          new_elems(i+12*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+12*oldsz,1);

          new_elems(i+13*oldsz, 0) = indice(17);
          new_elems(i+13*oldsz, 1) = les_elems(i,6);
          new_elems(i+13*oldsz, 2) = indice(4);
          new_elems(i+13*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+13*oldsz,1);

          new_elems(i+14*oldsz, 0) = les_elems(i,6);
          new_elems(i+14*oldsz, 1) = indice(18);
          new_elems(i+14*oldsz, 2) = indice(4);
          new_elems(i+14*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+14*oldsz,1);

          new_elems(i+15*oldsz, 0) = indice(18);
          new_elems(i+15*oldsz, 1) = les_elems(i,4);
          new_elems(i+15*oldsz, 2) = indice(4);
          new_elems(i+15*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+15*oldsz,1);

          // pyramide avant (base : 0-1-5-4)

          new_elems(i+16*oldsz, 0) = les_elems(i,0);
          new_elems(i+16*oldsz, 1) = indice(7);
          new_elems(i+16*oldsz, 2) = indice(3);
          new_elems(i+16*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+16*oldsz,1);

          new_elems(i+17*oldsz, 0) = indice(7);
          new_elems(i+17*oldsz, 1) = les_elems(i,1);
          new_elems(i+17*oldsz, 2) = indice(3);
          new_elems(i+17*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+17*oldsz,1);

          new_elems(i+18*oldsz, 0) = les_elems(i,1);
          new_elems(i+18*oldsz, 1) = indice(11);
          new_elems(i+18*oldsz, 2) = indice(3);
          new_elems(i+18*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+18*oldsz,1);

          new_elems(i+19*oldsz, 0) = indice(11);
          new_elems(i+19*oldsz, 1) = les_elems(i,5);
          new_elems(i+19*oldsz, 2) = indice(3);
          new_elems(i+19*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+19*oldsz,1);

          new_elems(i+20*oldsz, 0) = les_elems(i,5);
          new_elems(i+20*oldsz, 1) = indice(15);
          new_elems(i+20*oldsz, 2) = indice(3);
          new_elems(i+20*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+20*oldsz,1);

          new_elems(i+21*oldsz, 0) = indice(15);
          new_elems(i+21*oldsz, 1) = les_elems(i,4);
          new_elems(i+21*oldsz, 2) = indice(3);
          new_elems(i+21*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+21*oldsz,1);

          new_elems(i+22*oldsz, 0) = les_elems(i,4);
          new_elems(i+22*oldsz, 1) = indice(14);
          new_elems(i+22*oldsz, 2) = indice(3);
          new_elems(i+22*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+22*oldsz,1);

          new_elems(i+23*oldsz, 0) = indice(14);
          new_elems(i+23*oldsz, 1) = les_elems(i,0);
          new_elems(i+23*oldsz, 2) = indice(3);
          new_elems(i+23*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+23*oldsz,1);

          // pyramide arriere (base : 2-3-7-6)

          new_elems(i+24*oldsz, 0) = les_elems(i,2);
          new_elems(i+24*oldsz, 1) = indice(9);
          new_elems(i+24*oldsz, 2) = indice(6);
          new_elems(i+24*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+24*oldsz,1);

          new_elems(i+25*oldsz, 0) = indice(9);
          new_elems(i+25*oldsz, 1) = les_elems(i,3);
          new_elems(i+25*oldsz, 2) = indice(6);
          new_elems(i+25*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+25*oldsz,1);

          new_elems(i+26*oldsz, 0) = les_elems(i,3);
          new_elems(i+26*oldsz, 1) = indice(12);
          new_elems(i+26*oldsz, 2) = indice(6);
          new_elems(i+26*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+26*oldsz,1);

          new_elems(i+27*oldsz, 0) = indice(12);
          new_elems(i+27*oldsz, 1) = les_elems(i,7);
          new_elems(i+27*oldsz, 2) = indice(6);
          new_elems(i+27*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+27*oldsz,1);

          new_elems(i+28*oldsz, 0) = les_elems(i,7);
          new_elems(i+28*oldsz, 1) = indice(17);
          new_elems(i+28*oldsz, 2) = indice(6);
          new_elems(i+28*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+28*oldsz,1);

          new_elems(i+29*oldsz, 0) = indice(17);
          new_elems(i+29*oldsz, 1) = les_elems(i,6);
          new_elems(i+29*oldsz, 2) = indice(6);
          new_elems(i+29*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+29*oldsz,1);

          new_elems(i+30*oldsz, 0) = les_elems(i,6);
          new_elems(i+30*oldsz, 1) = indice(13);
          new_elems(i+30*oldsz, 2) = indice(6);
          new_elems(i+30*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+30*oldsz,1);

          new_elems(i+31*oldsz, 0) = indice(13);
          new_elems(i+31*oldsz, 1) = les_elems(i,2);
          new_elems(i+31*oldsz, 2) = indice(6);
          new_elems(i+31*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+31*oldsz,1);

          // pyramide gauche (base : 0-2-6-4)

          new_elems(i+32*oldsz, 0) = les_elems(i,0);
          new_elems(i+32*oldsz, 1) = indice(10);
          new_elems(i+32*oldsz, 2) = indice(2);
          new_elems(i+32*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+32*oldsz,1);

          new_elems(i+33*oldsz, 0) = indice(10);
          new_elems(i+33*oldsz, 1) = les_elems(i,2);
          new_elems(i+33*oldsz, 2) = indice(2);
          new_elems(i+33*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+33*oldsz,1);

          new_elems(i+34*oldsz, 0) = les_elems(i,2);
          new_elems(i+34*oldsz, 1) = indice(13);
          new_elems(i+34*oldsz, 2) = indice(2);
          new_elems(i+34*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+34*oldsz,1);

          new_elems(i+35*oldsz, 0) = indice(13);
          new_elems(i+35*oldsz, 1) = les_elems(i,6);
          new_elems(i+35*oldsz, 2) = indice(2);
          new_elems(i+35*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+35*oldsz,1);

          new_elems(i+36*oldsz, 0) = les_elems(i,6);
          new_elems(i+36*oldsz, 1) = indice(18);
          new_elems(i+36*oldsz, 2) = indice(2);
          new_elems(i+36*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+36*oldsz,1);

          new_elems(i+37*oldsz, 0) = indice(18);
          new_elems(i+37*oldsz, 1) = les_elems(i,4);
          new_elems(i+37*oldsz, 2) = indice(2);
          new_elems(i+37*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+37*oldsz,1);

          new_elems(i+38*oldsz, 0) = les_elems(i,4);
          new_elems(i+38*oldsz, 1) = indice(14);
          new_elems(i+38*oldsz, 2) = indice(2);
          new_elems(i+38*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+38*oldsz,1);

          new_elems(i+39*oldsz, 0) = indice(14);
          new_elems(i+39*oldsz, 1) = les_elems(i,0);
          new_elems(i+39*oldsz, 2) = indice(2);
          new_elems(i+39*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+39*oldsz,1);

          // pyramide droite (base : 1-3-7-5)

          new_elems(i+40*oldsz, 0) = les_elems(i,1);
          new_elems(i+40*oldsz, 1) = indice(8);
          new_elems(i+40*oldsz, 2) = indice(5);
          new_elems(i+40*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+40*oldsz,1);

          new_elems(i+41*oldsz, 0) = indice(8);
          new_elems(i+41*oldsz, 1) = les_elems(i,3);
          new_elems(i+41*oldsz, 2) = indice(5);
          new_elems(i+41*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+41*oldsz,1);

          new_elems(i+42*oldsz, 0) = les_elems(i,3);
          new_elems(i+42*oldsz, 1) = indice(12);
          new_elems(i+42*oldsz, 2) = indice(5);
          new_elems(i+42*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+42*oldsz,1);

          new_elems(i+43*oldsz, 0) = indice(12);
          new_elems(i+43*oldsz, 1) = les_elems(i,7);
          new_elems(i+43*oldsz, 2) = indice(5);
          new_elems(i+43*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+43*oldsz,1);

          new_elems(i+44*oldsz, 0) = les_elems(i,7);
          new_elems(i+44*oldsz, 1) = indice(16);
          new_elems(i+44*oldsz, 2) = indice(5);
          new_elems(i+44*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+44*oldsz,1);

          new_elems(i+45*oldsz, 0) = indice(16);
          new_elems(i+45*oldsz, 1) = les_elems(i,5);
          new_elems(i+45*oldsz, 2) = indice(5);
          new_elems(i+45*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+45*oldsz,1);

          new_elems(i+46*oldsz, 0) = les_elems(i,5);
          new_elems(i+46*oldsz, 1) = indice(11);
          new_elems(i+46*oldsz, 2) = indice(5);
          new_elems(i+46*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+46*oldsz,1);

          new_elems(i+47*oldsz, 0) = indice(11);
          new_elems(i+47*oldsz, 1) = les_elems(i,1);
          new_elems(i+47*oldsz, 2) = indice(5);
          new_elems(i+47*oldsz, 3) = indice(0);
          mettre_a_jour_sous_zone(zone,i,i+47*oldsz,1);

        }
      sommets_dom.resize(dim_som_old,3);
      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      Cerr << "We have split the cubes..." << finl;
      zone.invalide_octree();
      zone.typer("Tetraedre");
      Cerr << "  Reconstruction of the Octree" << finl;
      zone.construit_octree();
      Cerr << "  Octree rebuilt" << finl;

      {
        Cerr << "Splitting of the boundaries" << finl;
        LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
        while(curseur)
          {
            Faces& les_faces=curseur->faces();
            les_faces.typer(Faces::triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems, fait_sommet, nface, fait_sommet_arete, narete);
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
            decoupe(zone, les_faces, new_soms_old_elems, fait_sommet, nface, fait_sommet_arete, narete);
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
            decoupe(zone, les_faces, new_soms_old_elems, fait_sommet, nface, fait_sommet_arete, narete);
            ++curseur;
          }
      }
      Cerr<<"END of Tetraedriser_homogene_fin..."<<finl;
      Cerr<<"  1 NbElem="<<zone.les_elems().dimension(0)<<"  NbNod="<<zone.nb_som()<<finl;
    }
  else
    {
      Cerr << "We do not yet know how to Tetraedriser_homogene_fin the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
    }
}


