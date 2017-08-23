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
// File:        Decouper_Bord_coincident.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Decouper_Bord_coincident.h>
#include <EFichier.h>
#include <ArrOfBit.h>
#include <Scatter.h>

Implemente_instanciable(Decouper_Bord_coincident,"Decouper_Bord_coincident",Interprete_geometrique_base);

Sortie& Decouper_Bord_coincident::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Decouper_Bord_coincident::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Decouper_Bord_coincident::interpreter_(Entree& is)
{
  if(dimension==3)
    {
      Cerr << "We do not know how to split in 3D" << finl;
      exit();
    }
  associer_domaine(is);
  is >> nom_bord;

  for(int i=0; i<domaine().nb_zones() ; i++)

    {
      Decouper_Bord_coincident_(domaine().zone(i));
    }
  Cerr << "Decouper_Bord_coincident... OK" << finl;
  return is;
}


void Decouper_Bord_coincident::Decouper_Bord_coincident_(Zone& zone)
{
  if  (zone.type_elem()->que_suis_je() == "Triangle")
    //           ||(zone.type_elem()->que_suis_je() == "Tetraedre"))
    {
      Nom fichier="connectivity_failed_";
      fichier+=nom_bord;
      EFichier fic(fichier);

      if(!fic.good())
        {
          Cerr << "The file " << fichier << " was not found !!! " << finl;
          exit();
        }

      Domaine& dom = zone.domaine();
      const DoubleTab& xs = dom.coord_sommets();
      IntTab& les_elems = zone.les_elems();
      int oldsz = les_elems.dimension(0);
      int oldnbsom = zone.nb_som();

      IntTab new_elems(3*oldsz, dimension+1); // tableau sur-dimensionne

      // Construction de l'Octree sur la grille VEF de base
      zone.construit_octree();

      //On dimensionne une premiere fois le tableau des sommets
      //puis on redimensionnera seulement a la fin par la dimension exacte
      Scatter::uninit_sequential_domain(dom);

      DoubleTab& sommets_dom = dom.les_sommets();
      //int dim_som_old=sommets_dom.dimension(0); // dim_som_old = oldnbsom
      sommets_dom.resize(2*oldnbsom,dimension);

      // On initialise new_elem sur la base du maillage non redecoupe :

      for(int elem=0; elem<oldsz; elem++)
        {
          for(int k=0; k<dimension+1; k++)
            new_elems(elem, k) = les_elems(elem,k);
          mettre_a_jour_sous_zone(zone,elem,oldsz,0);
        }


      int face_oldsz, face_newsz, fac, indice;

      fic >> face_oldsz;
      fic >> face_newsz;

      IntTab new_faces_racc(face_newsz, dimension);
      //                IntTab new_faces_int(3*face_oldsz, dimension); // tableau sur-dimensionne

      int j,elem;
      int j0=0,j1=0,j2=0;
      int som1=0,som2=0;


      // compteurs

      int indice_new_som = oldnbsom;
      int i=0;
      //                int ii=0;

      fic >> indice ;

      while (indice!=-1)
        {
          if (!fic.good())
            {
              Cerr<<"Pb with "<<fichier <<" in Decouper_Bord_coincident::interpreter_"<<finl;
              Cerr<<" eof ???"<<finl;
              exit();
            }

          fic >> fac >> elem >> j ;

          if(j==0)
            {
              j0=0;
              j1=1;
              j2=2;
            }
          else if(j==1)
            {
              j0=1;
              j1=0;
              j2=2;
            }
          else if(j==2)
            {
              j0=2;
              j1=0;
              j2=1;
            }
          else
            {
              Cerr << " problem with the index j " << finl;
              exit();
            }

          if(indice==1) // face de bord a ne pas decouper
            {
              new_faces_racc(fac,0) = les_elems(elem,j1);
              new_faces_racc(fac,1) = les_elems(elem,j2);
            }
          else if(indice==2) // face de bord a decouper
            {
              som1=les_elems(elem,j1);
              som2=les_elems(elem,j2);

              sommets_dom(indice_new_som,0) = 0.5*(xs(som1,0)+xs(som2,0));
              sommets_dom(indice_new_som,1) = 0.5*(xs(som1,1)+xs(som2,1));

              //////////////////////////////////////////////////

              new_elems(elem, 0) = les_elems(elem,j0);
              new_elems(elem, 1) = les_elems(elem,j1);
              new_elems(elem, 2) = indice_new_som;

              new_faces_racc(fac,0) = les_elems(elem,j1);
              new_faces_racc(fac,1) = indice_new_som;

              //////////////////////////////////////////////////

              new_elems(i+oldsz, 0) = les_elems(elem,j0);
              new_elems(i+oldsz, 1) = les_elems(elem,j2);
              new_elems(i+oldsz, 2) = indice_new_som;

              new_faces_racc(i+face_oldsz,0) = les_elems(elem,2);
              new_faces_racc(i+face_oldsz,1) = indice_new_som;

              indice_new_som++;
              i++;

              //////////////////////////////////////////////////

              //                    new_faces_int(ii,0) = les_elems(elem,j0);
              //                    new_faces_int(ii,1) = indice_new_som;
              //
              //                    ii++;
            }
          else
            {
              Cerr << "problem with the partition of the element to realize" << finl;
              Cerr << "We do not know how to split in " << indice << finl;
              exit();
            }

          fic >> indice ;

        }//while


      Cerr << " Number of boundary face (generated) " << i+face_oldsz << finl;
      Cerr << " Number of boundary face (planned)  " << face_newsz << finl;

      if( (i+face_oldsz) != face_newsz)
        {
          Cerr << " Concern in the reconstruction " << finl;
          exit();
        }


      sommets_dom.resize(indice_new_som,dimension);
      new_elems.resize(i+oldsz,dimension+1);
      les_elems.ref(new_elems);
      //                new_faces_int.resize(ii,dimension);

      // Reconstruction de l'octree
      Cerr << "Splitting performs ..." << finl;
      zone.invalide_octree();
      if(dimension==2)
        zone.typer("Triangle");
      else
        zone.typer("Tetraedre");
      Cerr << "  Reconstruction of the Octree" << finl;
      zone.construit_octree();
      Cerr << "  Octree rebuilt" << finl;

      {
        // Les Raccords
        Cerr << "Splitting of connections" << finl;
        LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());;;
        while(curseur)
          {
            if(curseur.valeur()->le_nom()==nom_bord)
              {
                Faces& les_faces=curseur.valeur()->faces();
                if(dimension==2)
                  les_faces.typer(segment_2D);
                else
                  les_faces.typer(triangle_3D);

                IntTab& sommets=les_faces.les_sommets();
                assert(sommets.dimension(1)==dimension);
                sommets.ref(new_faces_racc);
                les_faces.voisins().resize(face_newsz, 2);
                les_faces.voisins()=-1;
              }
            ++curseur;
          }
      }
      Scatter::init_sequential_domain(dom);
      /*
        {
        // Les Faces internes
        Cerr << "Splitting of internal faces" << finl;
        LIST_CURSEUR(Faces_Interne) curseur(zone.faces_int());;;
        while(curseur)
        {
        Faces& les_faces=curseur->faces();
        if(dimension==2)
        les_faces.typer(segment_2D);
        else
        les_faces.typer(triangle_3D);

        IntTab& sommets=les_faces.les_sommets();
        int nb_faces=sommets.dimension(0);
        assert(sommets.dimension(1)==dimension);

        IntTab new_faces(nb_faces+ii, dimension);

        for(int j=0; j<nb_faces; j++)
        for(int k=0; k<dimension; k++)
        new_faces(j,k)=sommets(j,k);

        for(int j=0; j<ii; j++)
        for(int k=0; k<dimension; k++)
        new_faces(j+nb_faces,k)=new_faces_int(j,k);

        sommets.ref(new_faces);
        les_faces.voisins().resize(nb_faces+ii, 2);
        les_faces.voisins()=-1;
        }
        }
      */
      Cerr<<"END of Decouper_Bord_coincident..."<<finl;
      Cerr<<"  1 NbElem="<<zone.les_elems().dimension(0)<<"  NbSom="<<zone.nb_som()<<finl;
    }
  else
    {
      Cerr << "We do not yet know how to Decouper_Bord_coincident the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
    }

}


