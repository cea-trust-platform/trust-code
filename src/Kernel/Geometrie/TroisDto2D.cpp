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
// File:        TroisDto2D.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <TroisDto2D.h>
#include <Interprete_bloc.h>
#include <Scatter.h>

Implemente_instanciable_sans_constructeur(TroisDto2D,"Extract_2D_from_3D",Interprete_geometrique_base);

TroisDto2D::TroisDto2D()
{
  coupe_=1; // Algorithme de coupe utilise par Xprepro
}

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
Sortie& TroisDto2D::printOn(Sortie& os) const
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
Entree& TroisDto2D::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete Extract_2D_from_3D
//    Structure du jeu de donnee (en dimension 3) :
//    Extract_2D_from_3D dom3D bord3D dom2D
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
Entree& TroisDto2D::interpreter_(Entree& is)
{
  if(Objet_U::dimension==2)
    {
      Cerr << finl;
      Cerr << "You can not use " << que_suis_je() << " on a case already 2D" << finl;
      exit();
    }
  dimension=3;
  int test_axi=0;
  if (que_suis_je()=="Extract_2Daxi_from_3D")
    {
      test_axi=1;
      Cerr<<"We generate a 2D axi domain"<<finl;
    }
  Nom nom_bord, nom_dom2D;
  associer_domaine(is);
  is >> nom_bord >> nom_dom2D;
  Cerr << "Extraction of boundary " << nom_bord
       << " of domain " << domaine().le_nom()
       << " --> " << nom_dom2D << finl;
  {
    // Si l'objet nom_dom2D n'existe pas encore, on
    // ajoute un nouvel objet a l'interprete bloc courant
    Interprete_bloc& interp = Interprete_bloc::interprete_courant();
    if (interp.objet_global_existant(nom_dom2D))
      {
        if (sub_type(Domaine, interp.objet_global(nom_dom2D)))
          {
            Cerr << "Domain " << nom_dom2D
                 << " already exists, writing to this object." << finl;
          }
        else
          {
            Cerr << "Error : object " << nom_dom2D
                 << " already exists and is not a domain." << finl;
            exit();
          }
      }
    else
      {
        Cerr << "Declaring a new domain with name " << nom_dom2D << finl;
        DerObjU ob;
        ob.typer("Domaine");
        interp.ajouter(nom_dom2D, ob);
      }
  }
  Domaine& dom2D = ref_cast(Domaine, objet(nom_dom2D));
  const Domaine& dom3D = domaine();
  const Zone& zone3D=dom3D.zone(0);
  const Bord& bord3D=zone3D.bord(nom_bord);
  Scatter::uninit_sequential_domain(dom2D);
  extraire_2D(dom3D, dom2D, bord3D,nom_bord, test_axi);
  Scatter::init_sequential_domain(dom2D);
  return is;

}

void TroisDto2D::extraire_2D(const Domaine& dom3D, Domaine& dom2D, const Bord& bord3D, const Nom& nom_bord, int test_axi)
{
  const DoubleTab& coord_sommets3D=dom3D.coord_sommets();
  DoubleTab& coord_sommets2D=dom2D.les_sommets();
  const Zone& zone3D=dom3D.zone(0);
  const Faces& faces3D=bord3D.faces();
  const IntTab& les_faces3D=faces3D.les_sommets();
  int nb_som3D=coord_sommets3D.dimension(0);
  Cerr << "number of 3D nodes :" << nb_som3D << finl;
  int nb_faces3D=les_faces3D.dimension(0);
  Cerr << "number of 3D faces of " << nom_bord << " :" << nb_faces3D << finl;
  int nb_som_fac3D=les_faces3D.dimension(1);
  Cerr << "number of nodes per faces in 3D :" << nb_som_fac3D << finl;

  // Construction de renum_som3D2D
  ArrOfInt renum_som3D2D(nb_som3D);
  for (int i=0; i<nb_som3D ; i++)
    renum_som3D2D(i)=-1;
  int compteur=0;
  for (int i=0; i<nb_faces3D; i++)
    for (int j=0; j<nb_som_fac3D; j++)
      {
        int& tmp=renum_som3D2D(les_faces3D(i,j));
        if(tmp==-1)
          tmp=compteur++;
      }

  int nb_som2D=compteur;
  // Construction de renum_som2D3D
  ArrOfInt renum_som2D3D(nb_som2D);

  for (int i=0; i<nb_som3D ; i++)
    {
      int j;
      if((j=renum_som3D2D(i))!=-1)
        renum_som2D3D(j)=i;
    }
  coord_sommets2D.resize(nb_som2D,2);
  if (coupe_==2) coord_sommets2D.resize(nb_som2D,3);
  Zone tmpz;
  Zone& zone2D=dom2D.add(tmpz);
  Nom nom_zone("surface");
  nom_zone+=dom2D.le_nom();
  zone2D.nommer("surface");
  if(zone3D.type_elem()->que_suis_je()=="Hexaedre")
    if (test_axi)
      zone2D.typer("Rectangle_2D_axi");
    else
      zone2D.typer("Rectangle");
  else if(zone3D.type_elem()->que_suis_je()=="Tetraedre")
    {
      zone2D.typer("Triangle");
    }
  else if(zone3D.type_elem()->que_suis_je()=="Hexaedre_VEF")
    {
      zone2D.typer("Quadrangle");
    }
  if (coupe_!=1)
    {
      // Algorithme general
      // La frontiere doit etre plane (non verifie), parallele a un axe ou pas
      // Attention: les coordonnees du domaine 2D sont dans le nouveau repere
      double xb,yb,zb;
      double xc,yc,zc;
      double ux,uy,uz;
      double norm;
      double scalI,scalJ;

      xa=0.;
      ya=0.;
      za=0.;
      xb=0.;
      yb=0.;
      zb=0.;
      xc=0.;
      yc=0.;
      zc=0;
      ux=0.;
      uy=0.;
      uz=0.;
      norm=0.;
      scalI=0.;
      scalJ=0.;

      // on determine un repere orthonorme (A;IJK) sur le bord a extraire
      xa = coord_sommets3D(les_faces3D(0,0),0);
      ya = coord_sommets3D(les_faces3D(0,0),1);
      za = coord_sommets3D(les_faces3D(0,0),2);

      xb = coord_sommets3D(les_faces3D(0,1),0);
      yb = coord_sommets3D(les_faces3D(0,1),1);
      zb = coord_sommets3D(les_faces3D(0,1),2);

      xc = coord_sommets3D(les_faces3D(0,2),0);
      yc = coord_sommets3D(les_faces3D(0,2),1);
      zc = coord_sommets3D(les_faces3D(0,2),2);

      // calcul de I = AB/norme(AB)
      Ix=xb-xa;
      Iy=yb-ya;
      Iz=zb-za;
      norm = sqrt(Ix*Ix+Iy*Iy+Iz*Iz);
      Ix/=norm;
      Iy/=norm;
      Iz/=norm;

      // calcul de K = AB^AC/norme(AB^AC)
      ux=xc-xa;
      uy=yc-ya;
      uz=zc-za;
      norm = sqrt(ux*ux+uy*uy+uz*uz);
      ux/=norm;
      uy/=norm;
      uz/=norm;
      Kx = Iy*uz-uy*Iz;
      Ky = Iz*ux-Ix*uz;
      Kz = Ix*uy-Iy*ux;
      norm = sqrt(Kx*Kx+Ky*Ky+Kz*Kz);
      Kx/=norm;
      Ky/=norm;
      Kz/=norm;

      //  calcul de J = K^I
      Jx = Ky*Iz-Iy*Kz;
      Jy = Kz*Ix-Iz*Kx;
      Jz = Kx*Iy-Ky*Ix;

      // produit scalaire OA.I et OA.J
      scalI = xa*Ix+ya*Iy+za*Iz;
      scalJ = xa*Jx+ya*Jy+za*Jz;

      // on calcule les coordonnees des points du bord dans le nouveau repere 2D (A;I;J)
      // pour cela changement de repere du repere 3D vers le repere (A;I;J)
      for(int i=0; i<nb_som2D; i++)
        {
          double x = coord_sommets3D(renum_som2D3D(i),0);
          double y = coord_sommets3D(renum_som2D3D(i),1);
          double z = coord_sommets3D(renum_som2D3D(i),2);
          if (coupe_==0)
            {
              coord_sommets2D(i,0) = -scalI + x*Ix + y*Iy + z*Iz;
              coord_sommets2D(i,1) = -scalJ + x*Jx + y*Jy + z*Jz;
            }
          else
            {
              assert(coupe_==2);
// on ne ramene pas en 2D
              coord_sommets2D(i,0)=x;
              coord_sommets2D(i,1)=y;
              coord_sommets2D(i,2)=z;
            }
          //        Cout << "coord 1 = " << coord_sommets2D(i,0) << finl;
          //        Cout << "coord 2 = " << coord_sommets2D(i,1) << finl;
          //        Cout << "coord 3 = " << -xa*Kx-ya*Ky-za*Kz+x*Kx + y*Ky + z*Kz << finl;
        }
    }
  else
    {
      // Algorithme de coupe utilise par Xprepro en VDF
      // Attention: limite a une frontiere plane parallele a un axe
      // Les coordoonnees du domaine 2D sont conservees
      int test=0;
      double precision=DMAXFLOAT;
      for (test=2; test>-1; test--)
        {
          double x1,x2,x3;
          x1=coord_sommets3D(les_faces3D(0,0),test);
          x2=coord_sommets3D(les_faces3D(0,1),test);
          x3=coord_sommets3D(les_faces3D(0,2),test);
          double tmp=dabs(x1-x2);
          precision=(tmp<precision && tmp>0?tmp:precision);
          tmp=dabs(x3-x2);
          precision=(tmp<precision && tmp>0?tmp:precision);
          if (est_egal(x1,x2)&&est_egal(x2,x3)) break;
        }
      if (test==-1)
        {
          Cerr << "Error into TroisDto2D.cpp..." << finl;
          Cerr << "May be the " << dom2D.le_nom() << " boundary domain is not plane enough" << finl;
          Cerr << "Try to use: PrecisionGeom " << 100*precision << " in your data file" << finl;
          Cerr << "Or contact TRUST support." << finl;
          exit();
        }
      IntVect dir(2);
      int kk=0;
      for (int jj=0; jj<3; jj++)
        {
          if (test!=jj)
            {
              dir[kk]=jj;
              kk++;
            }
        }
      for(int i=0; i<nb_som2D; i++)
        for(int j=0; j<2; j++)
          coord_sommets2D(i,j)=coord_sommets3D(renum_som2D3D(i),dir[j]);
    }

  zone2D.associer_domaine(dom2D);

  IntTab& les_elems2D=zone2D.les_elems();
  les_elems2D=les_faces3D;
  for (int i=0; i< nb_faces3D; i++)
    for (int j=0; j< nb_som_fac3D; j++)
      {
        les_elems2D(i,j)=renum_som3D2D(les_elems2D(i,j));
      }

  dimension=2;
  {
    // On recupere les bords :

    CONST_LIST_CURSEUR(Bord) curseur(zone3D.faces_bord());
    while(curseur)
      {
        const Frontiere& front=curseur.valeur();
        const Faces& faces3D_front=front.faces();
        if ( (front.le_nom()!=nom_bord))
          {
            Bord tmp;
            Bord& bord2D=zone2D.faces_bord().add(tmp);
            bord2D.nommer(front.le_nom());
            bord2D.associer_zone(zone2D);
            if (test_axi)
              bord2D.typer_faces("QUADRILATERE_2D_AXI");
            else
              bord2D.typer_faces("segment_2D");
            // creer les faces de bord 2D ici!
            int compteur2=0;
            const IntTab& faces_sommets=faces3D_front.les_sommets();
            int nb_faces=faces_sommets.dimension(0);
            IntTab& aretes=bord2D.les_sommets_des_faces();
            IntTab& faces_voisins=bord2D.faces().voisins();

            aretes.resize(nb_faces, 2);
            // Boucle sur les faces du domaine 2D
            int doublons=0;
            for(int face=0; face < nb_faces; face++)
              {
                int ok=0;
                int tmpbis;
                for(int i=0; i<nb_som_fac3D; i++)
                  {
                    tmpbis=renum_som3D2D(faces_sommets(face,i));
                    if (tmpbis!=-1)
                      {
                        ok++;
                        if(ok==1)
                          aretes(compteur2,0)=tmpbis;
                        if(ok==2)
                          {
                            aretes(compteur2,1)=tmpbis;
                            compteur2++;
                          }
                        if(ok==3)
                          {
                            // Si 3 sommets de la face appartienne a la frontiere
                            // il y'a un probleme, on ajoute les 3 aretes et on cherchera
                            // les doublons qu'il faudra eliminer...
                            doublons=1;
                            aretes(compteur2,0)=aretes(compteur2-1,1);
                            aretes(compteur2,1)=tmpbis;
                            compteur2++;
                            aretes(compteur2,0)=aretes(compteur2-1,1);
                            aretes(compteur2,1)=aretes(compteur2-2,0);
                            compteur2++;
                          }
                      }
                  }
              }
            if (doublons)
              {
                // Recherche des doublons (algo en n^2)
                for (int i=0; i<compteur2; i++)
                  {
                    int& S10=aretes(i,0);
                    int& S11=aretes(i,1);
                    for (int j=i+1; j<compteur2; j++)
                      {
                        int& S20=aretes(j,0);
                        int& S21=aretes(j,1);
                        if ( (S10==S20 && S11==S21) || (S10==S21 && S11==S20) )
                          {
                            S10=-1;
                            S11=-1;
                            S20=-1;
                            S21=-1;
                          }
                      }
                  }
                // On supprime les doublons
                for (int i=0; i<compteur2; i++)
                  if (aretes(i,0)==-1)
                    {
                      for (int j=i; j<compteur2-1; j++)
                        {
                          aretes(j,0)=aretes(j+1,0);
                          aretes(j,1)=aretes(j+1,1);
                        }
                      compteur2--;
                    }
              }
            aretes.resize(compteur2, 2);
            faces_voisins.resize(compteur2,2);
            faces_voisins=-1;
            if (compteur2==0)
              {
                Cerr<<"boundary to eliminate " << front.le_nom()<<finl;
                zone2D.faces_bord().suppr(bord2D);
              }
            else
              {
                Cerr<<"boundary maintained " << front.le_nom()<<finl;
              }
          }
        ++curseur;
      }
  }

  {
    // On recupere les raccords :

    CONST_LIST_CURSEUR(Raccord) curseur(zone3D.faces_raccord());;;
    while(curseur)
      {
        // while(curseur)
        const Frontiere& front=curseur->valeur();
        const Faces& faces3Dfront=front.faces();
        if ( (front.le_nom()!=nom_bord))
          {
            Raccord tmpbis;
            Raccord& bord2D=zone2D.faces_raccord().add(tmpbis);
            bord2D.typer(curseur->valeur().le_type());
            bord2D->nommer(front.le_nom());
            bord2D->associer_zone(zone2D);
            if (test_axi)
              bord2D->typer_faces("QUADRILATERE_2D_AXI");
            else
              bord2D->typer_faces("segment_2D");
            // creer les faces de bord 2D ici!
            int compteur2=0;
            const IntTab& faces_sommets=faces3Dfront.les_sommets();
            int nb_faces=faces_sommets.dimension(0);
            IntTab& aretes=bord2D->les_sommets_des_faces();
            IntTab& faces_voisins=bord2D->faces().voisins();

            aretes.resize(nb_faces, 2);
            for(int face=0; face < nb_faces; face++)
              {
                int ok=0;
                int tmp;
                for(int i=0; i<nb_som_fac3D; i++)
                  {
                    tmp=renum_som3D2D(faces_sommets(face,i));
                    if (tmp!=-1)
                      {
                        ok++;
                        if(ok==1)
                          aretes(compteur2,0)=tmp;
                        else if(ok==2)
                          {
                            aretes(compteur2,1)=tmp;
                            compteur2++;
                            i=nb_som_fac3D;
                          }
                      }
                  }
              }
            aretes.resize(compteur2, 2);
            faces_voisins.resize(compteur2,2);
            faces_voisins=-1;
            if (compteur2==0)
              {
                Cerr<<"connector to eliminate " << front.le_nom()<<finl;
                zone2D.faces_raccord().suppr(bord2D);
              }
            else
              {
                Cerr<<"connector maintained " << front.le_nom()<<finl;
              }
          }
        ++curseur;
      }
  }

  if(zone3D.type_elem()->que_suis_je()!="Hexaedre_VEF")
    if (coupe_!=2)
      dom2D.reordonner();
  dimension=3;
}

