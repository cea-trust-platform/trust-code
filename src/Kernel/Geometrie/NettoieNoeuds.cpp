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
// File:        NettoieNoeuds.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <NettoieNoeuds.h>
#include <Domaine.h>
#include <Scatter.h>

Implemente_instanciable(NettoieNoeuds,"NettoiePasNoeuds",Interprete_geometrique_base);

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
Sortie& NettoieNoeuds::printOn(Sortie& os) const
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
Entree& NettoieNoeuds::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete NettoieNoeuds
//    Structure du jeu de donnee (en dimension 2) :
//    NettoieNoeuds dom alpha
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception: l'objet a mailler n'est pas du type Domaine
// Effets de bord:
// Postcondition:
int NettoieNoeuds::NettoiePasNoeuds=0;
Entree& NettoieNoeuds::interpreter_(Entree& is)
{

  associer_domaine(is);

  if (domaine().les_sommets().size()!=0)
    {
      Cerr << "The keyword NettoiePasNoeuds must be placed before the ";
      Cerr << "reading of the domain to avoid the cleaning of the double";
      Cerr << "nodes of the domain." << finl;
      exit();
    }
  NettoiePasNoeuds=1;
  Cerr << "NettoiePasNoeuds=" << NettoiePasNoeuds << " for the domain " << domaine().le_nom() << finl;

  return is;
}

void NettoieNoeuds::nettoie(Domaine& dom)
{
  if (NettoiePasNoeuds==1)
    return;

  // Autorise la modification de structure des tableaux sommets et elements:
  Scatter::uninit_sequential_domain(dom);

  DoubleTab& coord_sommets=dom.les_sommets();
  Zone& zone=dom.zone(0);
  IntTab& les_elems=zone.les_elems();
  int nb_som_init=coord_sommets.dimension(0);
  int nb_elem=les_elems.dimension(0);
  int nb_som_elem=les_elems.dimension(1);
  ArrOfInt renum_som_old2new(nb_som_init);

  int compteur=0;

  {
    //int i;
    for (int i=0; i<nb_som_init ; i++)
      renum_som_old2new(i)=-1;

    for (int i=0; i<nb_elem ; i++)
      for (int j=0; j< nb_som_elem; j++)
        {
          // GF dans le cas ou on a des polyedres
          if (les_elems(i,j)==-1) break;
          int& tmp=renum_som_old2new(les_elems(i,j));
          assert (tmp < compteur);
          if(tmp==-1)
            tmp=compteur++;
        }
    // ajout GF sinon un sommet double sur un bord devient -1
    int nb_bords=zone.nb_front_Cl();
    for (int ii=0; ii<nb_bords; ii++)
      {
        const Frontiere& front= zone.frontiere(ii);
        const Faces& faces=front.faces();
        const IntTab& faces_sommets=faces.les_sommets();
        int nb_faces=faces_sommets.dimension(0);
        int nb_som_face=faces_sommets.dimension(1);
        for(int i=0; i<nb_faces; i++)
          for(int j=0; j<nb_som_face; j++)
            {
              // dans le cas ou l'on a des polygones
              if (faces_sommets(i,j)==-1) break;
              int& tmp=renum_som_old2new(faces_sommets(i,j));
              assert (tmp < compteur);
              if(tmp==-1)
                tmp=compteur++;
            }
      }
  }
  int nb_som_new=compteur;
  Cerr << "Elimination of " << nb_som_init - nb_som_new << " unnecessary nodes in the domain " << dom.le_nom() << finl;
  if(nb_som_new!=nb_som_init)
    {
      ArrOfInt renum_som_new2old(nb_som_new);
      {
        int j;
        for (int i=0; i<nb_som_init ; i++)
          if((j=renum_som_old2new(i))!=-1)
            renum_som_new2old(j)=i;
      }
      {
        int i, j;
        DoubleTab new_coord(nb_som_new, dimension);

        for (i=0; i<nb_som_new ; i++)
          for (j=0; j<dimension ; j++)
            {
              new_coord(i,j)=coord_sommets(renum_som_new2old(i),j);
            }
        coord_sommets=new_coord;

        for (i=0; i<nb_elem ; i++)
          for (j=0; j<nb_som_elem ; j++)
            {
              int num_som=les_elems(i,j);
              if (num_som!=-1)
                les_elems(i,j)=renum_som_old2new(num_som);
              else
                les_elems(i,j)=-1;
            }
      }

      {
        // On recupere les bords :
        LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
        while(curseur)
          {
            // while(curseur)
            Frontiere& front=curseur.valeur();
            Faces& faces=front.faces();
            IntTab& faces_sommets=faces.les_sommets();
            IntTab old_faces_sommets(faces.les_sommets());
            int nb_faces=faces_sommets.dimension(0);
            int nb_som_face=faces_sommets.dimension(1);
            for(int i=0; i<nb_faces; i++)
              for(int j=0; j<nb_som_face; j++)
                {
                  int som=old_faces_sommets(i,j);
                  if (som!=-1)
                    faces_sommets(i,j)=
                      renum_som_old2new(som);
                  else
                    faces_sommets(i,j)=som;
                }
            ++curseur;
          }
      }
      {
        // Les Faces Internes :
        LIST_CURSEUR(Faces_Interne) curseur(zone.faces_int());;;
        while(curseur)
          {
            // while(curseur)
            Frontiere& front=curseur.valeur();
            Faces& faces=front.faces();
            IntTab& faces_sommets=faces.les_sommets();
            IntTab old_faces_sommets(faces.les_sommets());
            int nb_faces=faces_sommets.dimension(0);
            int nb_som_face=faces_sommets.dimension(1);
            for(int i=0; i<nb_faces; i++)
              for(int j=0; j<nb_som_face; j++)
                faces_sommets(i,j)=
                  renum_som_old2new(old_faces_sommets(i,j));
            ++curseur;
          }
      }
      {
        // Les Raccords
        LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());;;
        while(curseur)
          {
            Frontiere& front=curseur->valeur();
            Faces& faces=front.faces();
            IntTab& faces_sommets=faces.les_sommets();
            IntTab old_faces_sommets(faces.les_sommets());
            int nb_faces=faces_sommets.dimension(0);
            int nb_som_face=faces_sommets.dimension(1);
            for(int i=0; i<nb_faces; i++)
              for(int j=0; j<nb_som_face; j++)
                faces_sommets(i,j)=
                  renum_som_old2new(old_faces_sommets(i,j));
            ++curseur;
          }
      }
    }
  Scatter::init_sequential_domain(dom);
}

// Description:
// regarde si on n'a pas des noeuds doubles
void NettoieNoeuds::verifie_noeuds(const Domaine& dom)
{
  Cerr<<finl<<"Beginning of the search of identical nodes (can be long, algorithm in n^2)..."<<finl;
  const DoubleTab& coord_sommets=dom.les_sommets();
  int ns=coord_sommets.dimension(0);
  //  int nbsomelem=coord_sommets.dimension(1);
  //  const IntTab& les_elems= dom.zone(0).les_elems();
  int err=0;
  // la version avec chercher element plus rapide a priori
  // n log(n) a des chances de rater des sommets doubles
  // on revien ta un algo en n*n/2
  /*  for (int sommet=0;sommet<ns;sommet++)
      {

      double x,y,z=0;
      x=coord_sommets(sommet,0);
      y=coord_sommets(sommet,1);
      if (dimension>2)
      z=coord_sommets(sommet,0);

      int elem2=dom.zone(0).chercher_elements(x,y,z);
      if (elem2==-1)
      {
      Cerr<<"unable to find node "<<finl;
      exit();
      }
      int som;
      for ( som=0;som<nbsomelem;som++)
      {
      int sommet2=les_elems(elem2,som);
      int ok=1;
      for (int dir=0;dir<Objet_U::dimension;dir++)
      ok=ok&&(est_egal(coord_sommets(sommet,dir),coord_sommets(sommet2,dir)));
      if (ok)
      {
      if (sommet!=sommet2)
      {
      Cerr<<" the nodes "<<sommet<<" and "<<sommet2<<" seem the same"<<finl;
      err=1;
      }
      }
      }
      }
  */
  int tmp,pourcent=0;
  for (int sommet=0; sommet<ns; sommet++)
    {
      tmp =(sommet*100)/ns;
      if(tmp>=pourcent)
        {
          pourcent = tmp;
          Cerr << pourcent << "% of nodes have been verified\r " << flush;
        }
      for ( int sommet2=sommet+1; sommet2<ns; sommet2++)
        {
          int ok=1;
          for (int dir=0; dir<Objet_U::dimension; dir++)
            ok=ok&&(est_egal(coord_sommets(sommet,dir),coord_sommets(sommet2,dir)));
          if (ok)
            {
              if (sommet!=sommet2)
                {
                  Cerr<< finl << "The nodes "<<sommet<<" and "<<sommet2<<" are the same !"<<finl;
                  err=1;
                }
            }
        }
    }
  Cerr<<"End of the search of identical nodes."<<finl;
  if (err)
    exit();
}
