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
// File:        Lire_Ideas.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Lire_Ideas.h>
#include <Domaine.h>
#include <EFichier.h>
#include <Scatter.h>

Implemente_instanciable(Lire_Ideas,"Lire_Ideas",Interprete_geometrique_base);

// printOn et readOn

Sortie& Lire_Ideas::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Lire_Ideas::readOn(Entree& is )
{
  return is;
}

Entree& Lire_Ideas::interpreter_(Entree& is)
{
  Nom nom_fic;
  associer_domaine(is);
  is >> nom_fic;
  Domaine& dom=domaine();
  //
  int NDIM=dimension;
  int NNOEUDS=0;
  int NELEM=0;
  int NTETRA=0;
  int NFACB=0;
  int NSEG=0;
  int NGROUPE=0;
  int CAS=0;
  //
  Cerr << "First pass to determine the sizes of arrays" << finl;
  //
  char  nom_fic2[81];
  for (int ii=0; ii<80; ii++)
    nom_fic2[ii]='\0';
  strcpy(nom_fic2,nom_fic);
  F77NAME(LIREPREM) (nom_fic2,
                     &NDIM,
                     &NNOEUDS,
                     &NELEM,
                     &NTETRA,
                     &NFACB,
                     &NSEG,
                     &NGROUPE,
                     &CAS);
  //
  Cerr << "Number of nodes             : " << NNOEUDS << finl;
  Cerr << "Number of elements          : " << NELEM << finl;
  Cerr << "Number of tetrahedra        : " << NTETRA << finl;
  Cerr << "Number of boundary faces    : " << NFACB << finl;
  Cerr << "Number of boundary segments : " << NSEG << finl;
  Cerr << "Number of groups            : " << NGROUPE << finl;
  //
  ArrOfInt NGELEM(NGROUPE);
  ArrOfInt WORK(max(NTETRA,NFACB*4));
  //
  //Passage F77 a C++: les indices entre les deux langages sont inverses
  // (methode de stockage differente)
  //
  IntTab TETRA(4,NTETRA);
  IntTab FACB(3,NFACB);
  IntTab GROUP(NFACB,NGROUPE);
  //
  ArrOfDouble X(NNOEUDS);
  ArrOfDouble Y(NNOEUDS);
  ArrOfDouble Z(NNOEUDS);
  Cerr << "call of LIREIDEAS " << finl;
  //
  F77NAME(LIREIDEAS)(nom_fic2,
                     &NDIM,
                     X.addr(),
                     Y.addr(),
                     Z.addr(),
                     TETRA.addr(),
                     FACB.addr(),
                     GROUP.addr(),
                     &NNOEUDS,
                     &NELEM,
                     &NTETRA,
                     &NFACB,
                     &NGROUPE,
                     NGELEM.addr(),
                     WORK.addr(),
                     &CAS);
  //
  Cerr << " Reading completed " << finl;
  //
  DoubleTab& coord=dom.les_sommets();
  Zone une_zone;
  Zone& la_zone=dom.add(une_zone);
  //
  la_zone.nommer(dom.le_nom());
  la_zone.associer_domaine(dom);
  la_zone.type_elem().typer("Tetraedre");
  la_zone.type_elem().associer_zone(la_zone);

  //
  // On commence par transferer les coordonnees
  //
  coord.resize(NNOEUDS, dimension);
  {
    for(int i=0; i<NNOEUDS; i++)
      {
        coord(i,0) = (X[i]);
        coord(i,1) = (Y[i]);
        coord(i,2) = (Z[i]);
      }
  }
  //
  Cerr << " Completing the assignment of coordinates of nodes" << finl;
  //
  // On continue par les elements (connectivite element -> noeuds)
  //
  IntTab& les_elems=la_zone.les_elems();
  les_elems.resize(NTETRA, dimension+1);
  for(int i=0; i<NTETRA; i++)
    {
      for(int j=0; j<dimension+1; j++)
        {
          //
          // ATTENTION : Inversion des indices entre le tableau Fortran et C++
          //
          les_elems(i,j)= TETRA(j,i)-1 ;
        }
    }
  //
  Cerr << " Completing the assignment of elements" << finl;
  //
  //
  // Maintenant, on traite les faces de bords (tableau FACB, de dim (NFACB,3))
  // qui se trouvent dans les differents groupes de faces de bords. Chaque
  // groupe correspond a un bord bien defini du domaine (paroi, entree, sortie,
  // symetrie, ...) et chaque groupe porte le nom de ce type de bord (GRNAME)
  //
  // En resume :
  //    GRNAME (NGROUPE)         : Nom des differents bords
  //    GROUP  (NNOEUDS,NGROUPE) : Groupe de faces de bord (Numero de la face)
  //    NGELEM (NGROUPE)         : Nombre de faces de bords pour le groupe NGROUPE
  //    FACB   (NFACB,3)         : Connectivite des faces de bords (les 3 noeuds du triangle)
  //
  //
  //
  // On ouvre le fichier contenant les noms des groupes
  //
  Nom nom1="nom.groupe";
  EFichier fic(nom1);
  fic.set_check_types(1); // Remplace UFichier
  Cerr << "Reading of the file : " << nom1 << finl;
  // On definit les bords comme etant des faces de bords de la zone
  // On fait Zone --> les_bords
  //
  Bords& les_bords=la_zone.faces_bord();
  //
  // Ici, on associe les bords a la zone (on se refere a la_zone)
  // On fait les_bords --> Zone
  //
  les_bords.associer_zone(la_zone);
  //
  Nom nom_bord="Bord";
  //
  // On affecte les bords :
  //
  for(int k=0; k<NGROUPE; k++)
    {
      fic >> nom_bord;
      Bord& nouveau=les_bords.add(Bord());
      //
      nouveau.nommer(nom_bord);
      nouveau.faces().typer(triangle_3D);
      //
      nouveau.faces().dimensionner(NGELEM[k]);
      //
      int num;
      for (int j=0; j<NGELEM[k]; j++)
        {
          //
          // ATTENTION : Inversion des indices entre le tableau Fortran et C++
          //
          //        GF renumerotation dans lire_ideas.f maintenant...
          //
          //num=GROUP(j,k)-1-NTETRA;
          num=GROUP(j,k)-1;
          nouveau.faces().sommet(j,0)= FACB(0,num)-1;
          nouveau.faces().sommet(j,1)= FACB(1,num)-1;
          nouveau.faces().sommet(j,2)= FACB(2,num)-1;
        }
      nouveau.associer_zone(la_zone);
    }
  //
  Cerr << " Completing the assignment of the boundary faces" << finl;
  //
  Cerr << " Exit of Lire_Ideas " << finl;
  //
  la_zone.fixer_premieres_faces_frontiere();

  Scatter::init_sequential_domain(dom);
  return is;
}
