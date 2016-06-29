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
// File:        Tetraedriser_homogene.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Tetraedriser_homogene.h>

Implemente_instanciable(Tetraedriser_homogene,"Tetraedriser_homogene",Triangulation_base);

Sortie& Tetraedriser_homogene::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Tetraedriser_homogene::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Traitement des faces
//
static void decoupe(Zone& zone, Faces& faces, IntTab& new_soms_old_elems)
{
  const Domaine& dom=zone.domaine();
  const DoubleTab& coord=dom.coord_sommets();
  IntTab& sommets=faces.les_sommets();
  int nb_faces=sommets.dimension(0);
  assert(sommets.dimension(1)==4);
  IntTab nouveaux(8*nb_faces, 3);
  faces.voisins().resize(8*nb_faces, 2);
  faces.voisins()=-1;
  for(int i=0; i<nb_faces; i++)
    {
      int i1 = sommets(i,0);
      int i3 = sommets(i,1);
      int i7 = sommets(i,2);
      int i9 = sommets(i,3);

      double x=0.5*(coord(i1,0)+coord(i3,0));
      double y=0.5*(coord(i1,1)+coord(i3,1));
      double z=0.5*(coord(i1,2)+coord(i3,2));
      int i2 = zone.chercher_sommets( x, y, z);
      assert(i2>=0);

      x=0.5*(coord(i3,0)+coord(i9,0));
      y=0.5*(coord(i3,1)+coord(i9,1));
      z=0.5*(coord(i3,2)+coord(i9,2));
      int i4 = zone.chercher_sommets(x, y, z);
      assert(i4>=0);

      x=0.5*(coord(i1,0)+coord(i7,0));
      y=0.5*(coord(i1,1)+coord(i7,1));
      z=0.5*(coord(i1,2)+coord(i7,2));
      int i5 = zone.chercher_sommets(x, y, z) ;
      assert(i5>=0);

      x=0.5*(coord(i7,0)+coord(i9,0));
      y=0.5*(coord(i7,1)+coord(i9,1));
      z=0.5*(coord(i7,2)+coord(i9,2));
      int i8 = zone.chercher_sommets(x, y, z) ;
      assert(i8>=0);

      //Nouveau : on prend le barycentre de la face
      //x=0.5*(coord(i3,0)+coord(i7,0));
      //y=0.5*(coord(i3,1)+coord(i7,1));
      //z=0.5*(coord(i3,2)+coord(i7,2));
      x=0.25*(coord(i1,0)+coord(i3,0)+coord(i7,0)+coord(i9,0));
      y=0.25*(coord(i1,1)+coord(i3,1)+coord(i7,1)+coord(i9,1));
      z=0.25*(coord(i1,2)+coord(i3,2)+coord(i7,2)+coord(i9,2));
      int i6 = zone.chercher_sommets(x, y, z) ;
      if (i6<0)
        {
          Cerr<<"New Node i6 not found : pos="<<x<<" "<<y<<" "<<z<<finl;
          Cerr<<"  -> i6="<<i6<<finl;
        }
      assert(i6>=0);

      nouveaux(i,0) = i1;
      nouveaux(i,1) = i2;
      nouveaux(i,2) = i6;

      nouveaux(nb_faces+i,0) = i1;
      nouveaux(nb_faces+i,1) = i6;
      nouveaux(nb_faces+i,2) = i5;

      nouveaux(2*nb_faces+i,0) = i7;
      nouveaux(2*nb_faces+i,1) = i6;
      nouveaux(2*nb_faces+i,2) = i5;

      nouveaux(3*nb_faces+i,0) = i7;
      nouveaux(3*nb_faces+i,1) = i6;
      nouveaux(3*nb_faces+i,2) = i8;

      nouveaux(4*nb_faces+i,0) = i9;
      nouveaux(4*nb_faces+i,1) = i6;
      nouveaux(4*nb_faces+i,2) = i8;

      nouveaux(5*nb_faces+i,0) = i9;
      nouveaux(5*nb_faces+i,1) = i6;
      nouveaux(5*nb_faces+i,2) = i4;

      nouveaux(6*nb_faces+i,0) = i3;
      nouveaux(6*nb_faces+i,1) = i6;
      nouveaux(6*nb_faces+i,2) = i4;

      nouveaux(7*nb_faces+i,0) = i3;
      nouveaux(7*nb_faces+i,1) = i6;
      nouveaux(7*nb_faces+i,2) = i2;
    }
  sommets.ref(nouveaux);
}

//
// Pour definir un nouveau sommet, il faut regarder si il n'existe pas deja
// On renvoie le numero "global" du sommet considere
//
// ATTENTION: ne considere qu'une seule zone pour l'instant...
//
int creer_sommet (
  Domaine& dom,
  Zone& zone,
  DoubleTab& new_soms,
  IntTab& elem_traite,
  IntTab& new_soms_old_elems,
  //                int i1, int i2,
  int NbSom, IntTab& sommets,
  int& compteur,
  int oldnbsom, int& nbnewsoms )
{
  const DoubleTab& coord_sommets=dom.coord_sommets();
  //int nb_zones = dom.nb_zones();
  int _out;

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
  /*
    for (int zo=0; zo<nb_zones-1; zo++) {
    trouve = dom.zone(zo).chercher_elements(x,y,z);
    if(trouve!=-1) break;
    }
  */
  trouve = zone.chercher_elements(x,y,z);
  if (trouve<0)
    {
      for (int ii=0 ; ii<NbSom ; ii++)
        {
          Cerr<<"i"<<ii<<"="<<sommets(ii)<<" coord="<<coord_sommets(sommets(ii),0)<<" "<<coord_sommets(sommets(ii),1)<<" "<<coord_sommets(sommets(ii),2)<<endl;
        }
      Cerr<<"x="<<x<<" y="<<y<<" z="<<z<<endl;
      Cerr<<"-----"<<endl;
      assert(trouve>=0);
    }

  if (elem_traite(trouve))
    {
      _out = -1;
      int j = 0;
      double epsilon = Objet_U::precision_geom;
      do
        {
          if ((dabs(coord_sommets(new_soms_old_elems(trouve, j), 0)-x)<epsilon) &&
              (dabs(coord_sommets(new_soms_old_elems(trouve, j), 1)-y)<epsilon)&&
              (dabs(coord_sommets(new_soms_old_elems(trouve, j), 2)-z)<epsilon))
            /*if ((coord_sommets(new_soms_old_elems(trouve, j), 0) == x) &&
              (coord_sommets(new_soms_old_elems(trouve, j), 1) == y) &&
              (coord_sommets(new_soms_old_elems(trouve, j), 2) == z)   ) */
            {
              _out = new_soms_old_elems(trouve, j);
            }
          j++;
        }
      while ((_out == -1) && (j<19));
      if (_out<0)
        {
          Cerr << "Error, not found !" << finl;
          Cerr << "found="<<trouve<<" x="<<x<<" y="<<y<<" z="<<z<<endl;
          for (int ii=0 ; ii<NbSom ; ii++)
            {
              Cerr<<"i"<<ii<<"="<<sommets(ii)<<" coord="<<coord_sommets(sommets(ii),0)<<" "<<coord_sommets(sommets(ii),1)<<" "<<coord_sommets(sommets(ii),2)<<endl;
            }
          Process::exit();
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
    }

  if(_out<0)
    {
      Cerr << "Problem at the creation of node" << finl;
      Process::exit();
    }
  return _out;
}

//
//
//
void Tetraedriser_homogene::trianguler(Zone& zone) const
{
  if(zone.type_elem()->que_suis_je() == "Hexaedre"
      ||zone.type_elem()->que_suis_je() == "Hexaedre_VEF" )
    {
      IntTab& les_elems = zone.les_elems();
      int oldsz = les_elems.dimension(0);
      IntTab elem_traite(oldsz);
      int oldnbsom = zone.nb_som();
      IntTab new_elems(40*oldsz, 4);
      // pour chaque cube, liste des nouveaux sommets qu'il contient :
      IntTab new_soms_old_elems(oldsz, 19);
      IntTab sommets(8);
      Domaine& dom = zone.domaine();
      int compteur = 0;
      int nbnewsoms = 0;
      int i;
      // Construction de l'Octree sur la grille "VDF" de base
      zone.construit_octree();

      //On dimensionne une premiere fois le tableau des sommets avec la dimension maximun
      //puis on redimensionnera seulement a la fin par la dimension exacte

      DoubleTab& sommets_dom = dom.les_sommets();
      //19 pour les nouveaux sommets et 8 pour les anciens sommets=27
      int dim_som_max=27*oldsz;
      int dim_som_old=sommets_dom.dimension(0);
      sommets_dom.resize(dim_som_max,3);


      IntTab indice(19);
      DoubleTab new_soms(19,3);

      for(i=0; i< oldsz; i++)
        {
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
          //Cerr<<" --creer_sommet "<<i0<<" "<<i7<<endl;
          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i2;
          sommets(3)=i3;
          sommets(4)=i4;
          sommets(5)=i5;
          sommets(6)=i6;
          sommets(7)=i7;
          indice(0)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 8, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i7, compteur, oldnbsom, nbnewsoms);

          //centres des faces
          //Cerr<<" --creer_sommet "<<i0<<" "<<i3<<endl;
          indice(1)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i3, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i0<<" "<<i6<<endl;
          sommets(0)=i0;
          sommets(1)=i2;
          sommets(2)=i4;
          sommets(3)=i6;
          indice(2)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i6, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i0<<" "<<i5<<endl;
          sommets(0)=i0;
          sommets(1)=i1;
          sommets(2)=i4;
          sommets(3)=i5;
          indice(3)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i5, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i4<<" "<<i7<<endl;
          sommets(0)=i4;
          sommets(1)=i5;
          sommets(2)=i6;
          sommets(3)=i7;
          indice(4)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i4, i7, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i1<<" "<<i7<<endl;
          sommets(0)=i1;
          sommets(1)=i3;
          sommets(2)=i5;
          sommets(3)=i7;
          indice(5)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i1, i7, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i2<<" "<<i7<<endl;
          sommets(0)=i2;
          sommets(1)=i3;
          sommets(2)=i6;
          sommets(3)=i7;
          indice(6)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 4, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i2, i7, compteur, oldnbsom, nbnewsoms);

          //centres des aretes
          //Cerr<<" --creer_sommet "<<i0<<" "<<i1<<endl;
          sommets(0)=i0;
          sommets(1)=i1;
          indice(7)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i1, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i0<<" "<<i2<<endl;
          sommets(0)=i0;
          sommets(1)=i2;
          indice(8)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i2, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i0<<" "<<i4<<endl;
          sommets(0)=i0;
          sommets(1)=i4;
          indice(9)  =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i0, i4, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i1<<" "<<i3<<endl;
          sommets(0)=i1;
          sommets(1)=i3;
          indice(10) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i1, i3, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i1<<" "<<i5<<endl;
          sommets(0)=i1;
          sommets(1)=i5;
          indice(11) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i1, i5, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i2<<" "<<i3<<endl;
          sommets(0)=i2;
          sommets(1)=i3;
          indice(12) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i2, i3, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i2<<" "<<i6<<endl;
          sommets(0)=i2;
          sommets(1)=i6;
          indice(13) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i2, i6, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i3<<" "<<i7<<endl;
          sommets(0)=i3;
          sommets(1)=i7;
          indice(14) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i3, i7, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i4<<" "<<i5<<endl;
          sommets(0)=i4;
          sommets(1)=i5;
          indice(15) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i4, i5, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i4<<" "<<i6<<endl;
          sommets(0)=i4;
          sommets(1)=i6;
          indice(16) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i4, i6, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i5<<" "<<i7<<endl;
          sommets(0)=i5;
          sommets(1)=i7;
          indice(17) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i5, i7, compteur, oldnbsom, nbnewsoms);
          //Cerr<<" --creer_sommet "<<i6<<" "<<i7<<endl;
          sommets(0)=i6;
          sommets(1)=i7;
          indice(18) =
            creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, 2, sommets, compteur, oldnbsom, nbnewsoms);
          //creer_sommet(dom, zone, new_soms, elem_traite, new_soms_old_elems, i6, i7, compteur, oldnbsom, nbnewsoms);

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
          //new_soms_old_elems(i, t) = indice(t);

          for(int j=0; j<compteur; j++)
            for(int k=0; k<3; k++)
              sommets_dom(dim_som_old+j,k)=new_soms(j,k) ;
          dim_som_old += compteur;


          // L'element en cours a ete "traite" en entier
          elem_traite(i) = 1;

          //0 0-1 0-2 Bas 0-4 Arr Gauche Milieu ** 1
          new_elems(i, 0) = les_elems(i,0);
          new_elems(i, 1) = indice(7);
          new_elems(i, 2) = indice(3);
          new_elems(i, 3) = indice(1);
          //0 0-1 0-2 Bas 0-4 Arr Gauche Milieu ** 2
          new_elems(i+oldsz, 0) = les_elems(i,0);
          new_elems(i+oldsz, 1) = indice(9);
          new_elems(i+oldsz, 2) = indice(3);
          new_elems(i+oldsz, 3) = indice(2);
          mettre_a_jour_sous_zone(zone,i,i+oldsz,1);
          //0 0-1 0-2 Bas 0-4 Arr Gauche Milieu ** 3
          new_elems(i+2*oldsz, 0) = les_elems(i,0);
          new_elems(i+2*oldsz, 1) = indice(8);
          new_elems(i+2*oldsz, 2) = indice(1);
          new_elems(i+2*oldsz, 3) = indice(2);
          mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);
          //0 0-1 0-2 Bas 0-4 Arr Gauche Milieu ** 4
          new_elems(i+3*oldsz, 0) = indice(3);
          new_elems(i+3*oldsz, 1) = indice(0);
          new_elems(i+3*oldsz, 2) = indice(1);
          new_elems(i+3*oldsz, 3) = indice(2);
          mettre_a_jour_sous_zone(zone,i,i+3*oldsz,1);
          //0 0-1 0-2 Bas 0-4 Arr Gauche Milieu ** 5
          new_elems(i+4*oldsz, 0) = les_elems(i,0);
          new_elems(i+4*oldsz, 1) = indice(3);
          new_elems(i+4*oldsz, 2) = indice(1);
          new_elems(i+4*oldsz, 3) = indice(2);
          mettre_a_jour_sous_zone(zone,i,i+4*oldsz,1);

          //0-1 1 Bas 1-3 Arr 1-5 Milieu Droit ** 1
          new_elems(i+5*oldsz, 0) = les_elems(i,1);
          new_elems(i+5*oldsz, 1) = indice(7);
          new_elems(i+5*oldsz, 2) = indice(3);
          new_elems(i+5*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+5*oldsz,1);
          //0-1 1 Bas 1-3 Arr 1-5 Milieu Droit ** 2
          new_elems(i+6*oldsz, 0) = les_elems(i,1);
          new_elems(i+6*oldsz, 1) = indice(11);
          new_elems(i+6*oldsz, 2) = indice(3);
          new_elems(i+6*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+6*oldsz,1);
          //0-1 1 Bas 1-3 Arr 1-5 Milieu Droit ** 3
          new_elems(i+7*oldsz, 0) = les_elems(i,1);
          new_elems(i+7*oldsz, 1) = indice(1);
          new_elems(i+7*oldsz, 2) = indice(10);
          new_elems(i+7*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+7*oldsz,1);
          //0-1 1 Bas 1-3 Arr 1-5 Milieu Droit ** 4
          new_elems(i+8*oldsz, 0) = indice(1);
          new_elems(i+8*oldsz, 1) = indice(3);
          new_elems(i+8*oldsz, 2) = indice(0);
          new_elems(i+8*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+8*oldsz,1);
          //0-1 1 Bas 1-3 Arr 1-5 Milieu Droit ** 5
          new_elems(i+9*oldsz, 0) = les_elems(i,1);
          new_elems(i+9*oldsz, 1) = indice(1);
          new_elems(i+9*oldsz, 2) = indice(3);
          new_elems(i+9*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+9*oldsz,1);

          //0-2 Bas 2 2-3 Gauche Milieu 2-6 Avant ** 1
          new_elems(i+10*oldsz, 0) = les_elems(i,2);
          new_elems(i+10*oldsz, 1) = indice(1);
          new_elems(i+10*oldsz, 2) = indice(6);
          new_elems(i+10*oldsz, 3) = indice(12);
          mettre_a_jour_sous_zone(zone,i,i+10*oldsz,1);
          //0-2 Bas 2 2-3 Gauche Milieu 2-6 Avant ** 2
          new_elems(i+11*oldsz, 0) = les_elems(i,2);
          new_elems(i+11*oldsz, 1) = indice(2);
          new_elems(i+11*oldsz, 2) = indice(8);
          new_elems(i+11*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+11*oldsz,1);
          //0-2 Bas 2 2-3 Gauche Milieu 2-6 Avant ** 3
          new_elems(i+12*oldsz, 0) = les_elems(i,2);
          new_elems(i+12*oldsz, 1) = indice(2);
          new_elems(i+12*oldsz, 2) = indice(6);
          new_elems(i+12*oldsz, 3) = indice(13);
          mettre_a_jour_sous_zone(zone,i,i+12*oldsz,1);
          //0-2 Bas 2 2-3 Gauche Milieu 2-6 Avant ** 4
          new_elems(i+13*oldsz, 0) = indice(2);
          new_elems(i+13*oldsz, 1) = indice(0);
          new_elems(i+13*oldsz, 2) = indice(6);
          new_elems(i+13*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+13*oldsz,1);
          //0-2 Bas 2 2-3 Gauche Milieu 2-6 Avant ** 5
          new_elems(i+14*oldsz, 0) = les_elems(i,2);
          new_elems(i+14*oldsz, 1) = indice(2);
          new_elems(i+14*oldsz, 2) = indice(6);
          new_elems(i+14*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+14*oldsz,1);

          //Bas 1-3 2-3 3 Milieu Droit Avant 3-7 ** 1
          new_elems(i+15*oldsz, 0) = les_elems(i,3);
          new_elems(i+15*oldsz, 1) = indice(1);
          new_elems(i+15*oldsz, 2) = indice(10);
          new_elems(i+15*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+15*oldsz,1);
          //Bas 1-3 2-3 3 Milieu Droit Avant 3-7 ** 2
          new_elems(i+16*oldsz, 0) = les_elems(i,3);
          new_elems(i+16*oldsz, 1) = indice(14);
          new_elems(i+16*oldsz, 2) = indice(6);
          new_elems(i+16*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+16*oldsz,1);
          //Bas 1-3 2-3 3 Milieu Droit Avant 3-7 ** 3
          new_elems(i+17*oldsz, 0) = les_elems(i,3);
          new_elems(i+17*oldsz, 1) = indice(12);
          new_elems(i+17*oldsz, 2) = indice(6);
          new_elems(i+17*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+17*oldsz,1);
          //Bas 1-3 2-3 3 Milieu Droit Avant 3-7 ** 4
          new_elems(i+18*oldsz, 0) = indice(0);
          new_elems(i+18*oldsz, 1) = indice(5);
          new_elems(i+18*oldsz, 2) = indice(6);
          new_elems(i+18*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+18*oldsz,1);
          //Bas 1-3 2-3 3 Milieu Droit Avant 3-7 ** 5
          new_elems(i+19*oldsz, 0) = les_elems(i,3);
          new_elems(i+19*oldsz, 1) = indice(5);
          new_elems(i+19*oldsz, 2) = indice(6);
          new_elems(i+19*oldsz, 3) = indice(1);
          mettre_a_jour_sous_zone(zone,i,i+19*oldsz,1);

          //0-4 Arr Gauche Milieu 4 4-5 4-6 Haut ** 1
          new_elems(i+20*oldsz, 0) = les_elems(i,4);
          new_elems(i+20*oldsz, 1) = indice(9);
          new_elems(i+20*oldsz, 2) = indice(3);
          new_elems(i+20*oldsz, 3) = indice(2);
          mettre_a_jour_sous_zone(zone,i,i+20*oldsz,1);
          //0-4 Arr Gauche Milieu 4 4-5 4-6 Haut ** 2
          new_elems(i+21*oldsz, 0) = les_elems(i,4);
          new_elems(i+21*oldsz, 1) = indice(4);
          new_elems(i+21*oldsz, 2) = indice(3);
          new_elems(i+21*oldsz, 3) = indice(15);
          mettre_a_jour_sous_zone(zone,i,i+21*oldsz,1);
          //0-4 Arr Gauche Milieu 4 4-5 4-6 Haut ** 3
          new_elems(i+22*oldsz, 0) = les_elems(i,4);
          new_elems(i+22*oldsz, 1) = indice(4);
          new_elems(i+22*oldsz, 2) = indice(2);
          new_elems(i+22*oldsz, 3) = indice(16);
          mettre_a_jour_sous_zone(zone,i,i+22*oldsz,1);
          //0-4 Arr Gauche Milieu 4 4-5 4-6 Haut ** 4
          new_elems(i+23*oldsz, 0) = indice(0);
          new_elems(i+23*oldsz, 1) = indice(4);
          new_elems(i+23*oldsz, 2) = indice(2);
          new_elems(i+23*oldsz, 3) = indice(3);
          mettre_a_jour_sous_zone(zone,i,i+23*oldsz,1);
          //0-4 Arr Gauche Milieu 4 4-5 4-6 Haut ** 5
          new_elems(i+24*oldsz, 0) = les_elems(i,4);
          new_elems(i+24*oldsz, 1) = indice(4);
          new_elems(i+24*oldsz, 2) = indice(2);
          new_elems(i+24*oldsz, 3) = indice(3);
          mettre_a_jour_sous_zone(zone,i,i+24*oldsz,1);

          //Arr 1-5 Milieu Droit 4-5 5 Haut 5-7 ** 1
          new_elems(i+25*oldsz, 0) = les_elems(i,5);
          new_elems(i+25*oldsz, 1) = indice(3);
          new_elems(i+25*oldsz, 2) = indice(11);
          new_elems(i+25*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+25*oldsz,1);
          //Arr 1-5 Milieu Droit 4-5 5 Haut 5-7 ** 2
          new_elems(i+26*oldsz, 0) = les_elems(i,5);
          new_elems(i+26*oldsz, 1) = indice(3);
          new_elems(i+26*oldsz, 2) = indice(15);
          new_elems(i+26*oldsz, 3) = indice(4);
          mettre_a_jour_sous_zone(zone,i,i+26*oldsz,1);
          //Arr 1-5 Milieu Droit 4-5 5 Haut 5-7 ** 3
          new_elems(i+27*oldsz, 0) = les_elems(i,5);
          new_elems(i+27*oldsz, 1) = indice(5);
          new_elems(i+27*oldsz, 2) = indice(17);
          new_elems(i+27*oldsz, 3) = indice(4);
          mettre_a_jour_sous_zone(zone,i,i+27*oldsz,1);
          //Arr 1-5 Milieu Droit 4-5 5 Haut 5-7 ** 4
          new_elems(i+28*oldsz, 0) = indice(0);
          new_elems(i+28*oldsz, 1) = indice(3);
          new_elems(i+28*oldsz, 2) = indice(5);
          new_elems(i+28*oldsz, 3) = indice(4);
          mettre_a_jour_sous_zone(zone,i,i+28*oldsz,1);
          //Arr 1-5 Milieu Droit 4-5 5 Haut 5-7 ** 5
          new_elems(i+29*oldsz, 0) = les_elems(i,5);
          new_elems(i+29*oldsz, 1) = indice(5);
          new_elems(i+29*oldsz, 2) = indice(3);
          new_elems(i+29*oldsz, 3) = indice(4);
          mettre_a_jour_sous_zone(zone,i,i+29*oldsz,1);

          //Gauche Milieu 2-6 Avant 4-6 Haut 6 6-7 ** 1
          new_elems(i+30*oldsz, 0) = les_elems(i,6);
          new_elems(i+30*oldsz, 1) = indice(2);
          new_elems(i+30*oldsz, 2) = indice(4);
          new_elems(i+30*oldsz, 3) = indice(16);
          mettre_a_jour_sous_zone(zone,i,i+30*oldsz,1);
          //Gauche Milieu 2-6 Avant 4-6 Haut 6 6-7 ** 2
          new_elems(i+31*oldsz, 0) = les_elems(i,6);
          new_elems(i+31*oldsz, 1) = indice(6);
          new_elems(i+31*oldsz, 2) = indice(4);
          new_elems(i+31*oldsz, 3) = indice(18);
          mettre_a_jour_sous_zone(zone,i,i+31*oldsz,1);
          //Gauche Milieu 2-6 Avant 4-6 Haut 6 6-7 ** 3
          new_elems(i+32*oldsz, 0) = les_elems(i,6);
          new_elems(i+32*oldsz, 1) = indice(6);
          new_elems(i+32*oldsz, 2) = indice(2);
          new_elems(i+32*oldsz, 3) = indice(13);
          mettre_a_jour_sous_zone(zone,i,i+32*oldsz,1);
          //Gauche Milieu 2-6 Avant 4-6 Haut 6 6-7 ** 4
          new_elems(i+33*oldsz, 0) = indice(0);
          new_elems(i+33*oldsz, 1) = indice(6);
          new_elems(i+33*oldsz, 2) = indice(2);
          new_elems(i+33*oldsz, 3) = indice(4);
          mettre_a_jour_sous_zone(zone,i,i+33*oldsz,1);
          //Gauche Milieu 2-6 Avant 4-6 Haut 6 6-7 ** 5
          new_elems(i+34*oldsz, 0) = les_elems(i,6);
          new_elems(i+34*oldsz, 1) = indice(6);
          new_elems(i+34*oldsz, 2) = indice(2);
          new_elems(i+34*oldsz, 3) = indice(4);
          mettre_a_jour_sous_zone(zone,i,i+34*oldsz,1);

          //Milieu Droit Avant 3-7 Haut 5-7 6-7 7 ** 1
          new_elems(i+35*oldsz, 0) = les_elems(i,7);
          new_elems(i+35*oldsz, 1) = indice(4);
          new_elems(i+35*oldsz, 2) = indice(5);
          new_elems(i+35*oldsz, 3) = indice(17);
          mettre_a_jour_sous_zone(zone,i,i+35*oldsz,1);
          //Milieu Droit Avant 3-7 Haut 5-7 6-7 7 ** 2
          new_elems(i+36*oldsz, 0) = les_elems(i,7);
          new_elems(i+36*oldsz, 1) = indice(6);
          new_elems(i+36*oldsz, 2) = indice(5);
          new_elems(i+36*oldsz, 3) = indice(14);
          mettre_a_jour_sous_zone(zone,i,i+36*oldsz,1);
          //Milieu Droit Avant 3-7 Haut 5-7 6-7 7 ** 3
          new_elems(i+37*oldsz, 0) = les_elems(i,7);
          new_elems(i+37*oldsz, 1) = indice(4);
          new_elems(i+37*oldsz, 2) = indice(6);
          new_elems(i+37*oldsz, 3) = indice(18);
          mettre_a_jour_sous_zone(zone,i,i+37*oldsz,1);
          //Milieu Droit Avant 3-7 Haut 5-7 6-7 7 ** 4
          new_elems(i+38*oldsz, 0) = indice(0);
          new_elems(i+38*oldsz, 1) = indice(4);
          new_elems(i+38*oldsz, 2) = indice(6);
          new_elems(i+38*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+38*oldsz,1);
          //Milieu Droit Avant 3-7 Haut 5-7 6-7 7 ** 5
          new_elems(i+39*oldsz, 0) = les_elems(i,7);
          new_elems(i+39*oldsz, 1) = indice(4);
          new_elems(i+39*oldsz, 2) = indice(6);
          new_elems(i+39*oldsz, 3) = indice(5);
          mettre_a_jour_sous_zone(zone,i,i+39*oldsz,1);

        }
      sommets_dom.resize(dim_som_old,3);
      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      Cerr << "We have split the cubes..." << endl;

      zone.invalide_octree();
      zone.typer("Tetraedre");

      {
        Cerr << "Splitting of the boundaries" << finl;
        LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
        while(curseur)
          {
            Faces& les_faces=curseur->faces();
            les_faces.typer(triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems);
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
            les_faces.typer(triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems);
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
            les_faces.typer(triangle_3D);
            decoupe(zone, les_faces, new_soms_old_elems);
            ++curseur;
          }
      }
      Cerr<<"END of Tetraedriser_homogene..."<<finl;
      Cerr<<"  1 NbElem="<<zone.les_elems().dimension(0)<<"  NbNod="<<zone.nb_som()<<finl;
    }
  else
    {
      Cerr << "We do not yet know how to Tetraedriser_homogene the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
    }
}


