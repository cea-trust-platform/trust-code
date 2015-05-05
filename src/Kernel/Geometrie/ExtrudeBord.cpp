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
// File:        ExtrudeBord.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <ExtrudeBord.h>
#include <EFichier.h>
#include <TroisDto2D.h>
#include <Transformer.h>
#include <SFichier.h>
#include <Extruder.h>
#include <Extruder_en3.h>
#include <Scatter.h>
#include <Param.h>
#include <IntVects.h>

Implemente_instanciable_sans_constructeur(ExtrudeBord,"ExtrudeBord",Interprete_geometrique_base);

ExtrudeBord::ExtrudeBord()
{
  hexa_old=0;
  Trois_Tetra=0;
}

Sortie& ExtrudeBord::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& ExtrudeBord::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& ExtrudeBord::interpreter_(Entree& is)
{
  Nom nom_dom_volumique;
  Nom nom_front,nom_dom_surfacique;
  DoubleVect vect_dir(3);
  int nbpas;

  if(dimension!=3)
    {
      Cerr << "Interpreter "<<que_suis_je()<<" can be used only in 3 dimensions." <<finl;
      exit();
    }
  Param param(que_suis_je());
  param.ajouter("domaine_init",&nom_dom_volumique,Param::REQUIRED);
  param.ajouter_arr_size_predefinie("direction",&vect_dir,Param::REQUIRED);
  param.ajouter("nb_tranches",&nbpas,Param::REQUIRED);
  param.ajouter("domaine_final",&nom_dom_surfacique,Param::REQUIRED);
  param.ajouter("nom_bord",&nom_front,Param::REQUIRED);
  param.ajouter_flag("hexa_old",&hexa_old);
  param.ajouter_flag("Trois_Tetra",&Trois_Tetra);
  param.lire_avec_accolades_depuis(is);

  associer_domaine(nom_dom_volumique);

  const Domaine& dom=domaine();
  const Zone& zone=dom.zone(0);

  if (zone.nb_som_elem()==8 && (hexa_old == 1))
    {
      extruder_hexa_old(nom_front, nom_dom_surfacique, vect_dir, nbpas);
    }
  else if (zone.nb_som_elem()==4 || zone.nb_som_elem()==8)
    {
      extruder_bord(nom_front, nom_dom_surfacique, vect_dir, nbpas);
    }


  Domaine& dom_surfacique=ref_cast(Domaine, objet(nom_dom_surfacique));
  Scatter::init_sequential_domain(dom_surfacique);
  return is;
}

void ExtrudeBord::extruder_bord(Nom& nom_front, Nom& nom_dom_surfacique, DoubleVect& vect_dir, int nbpas)
{
  const Domaine& dom=domaine();

  const Zone& zone=dom.zone(0);
  const Bord& front=zone.bord(nom_front);

  Domaine& dom_surfacique=ref_cast(Domaine, objet(nom_dom_surfacique));

  // Extract from the volume domain dom the boundary front and fill the dom_surfacique domain:
  TroisDto2D tD2dD;
  int& coupe=tD2dD.coupe();
  coupe=0;
  tD2dD.extraire_2D(dom,dom_surfacique,front,nom_front,0);

  double xa = tD2dD.getXa();
  double ya = tD2dD.getYa();
  double za = tD2dD.getZa();
  double Ix = tD2dD.getIx();
  double Iy = tD2dD.getIy();
  double Iz = tD2dD.getIz();
  double Jx = tD2dD.getJx();
  double Jy = tD2dD.getJy();
  double Jz = tD2dD.getJz();
  double Kx = tD2dD.getKx();
  double Ky = tD2dD.getKy();
  double Kz = tD2dD.getKz();


  dimension=2;

  if (Trois_Tetra==1)  // Evolution de la methode Extrude bord pour l'adapter a 3tetra
    {
      Extruder_en3 extr3;
      extr3.setNbTranches(nbpas);
      extr3.setDirection(vect_dir(0)*Ix+vect_dir(1)*Iy+vect_dir(2)*Iz,
                         vect_dir(0)*Jx+vect_dir(1)*Jy+vect_dir(2)*Jz,
                         vect_dir(0)*Kx+vect_dir(1)*Ky+vect_dir(2)*Kz);

      int nb_som=dom_surfacique.coord_sommets().dimension(0);
      IntVect num(nb_som);
      dom_surfacique.ajouter(dom_surfacique.coord_sommets(), num);
      extr3.extruder(dom_surfacique,num);

      Zone& zone2 = dom_surfacique.zone(0);
      if ((zone2.type_elem()->que_suis_je())== "Rectangle")
        {
          Cerr << "It is not possible to apply ExtrudeBord with 3Tetra option to : "   <<  zone2.type_elem()->que_suis_je() << " mesh cells" << finl;
          exit();
        }
    }
  else
    {
      Extruder extr;
      extr.setNbTranches(nbpas);
      extr.setDirection(vect_dir(0)*Ix+vect_dir(1)*Iy+vect_dir(2)*Iz,
                        vect_dir(0)*Jx+vect_dir(1)*Jy+vect_dir(2)*Jz,
                        vect_dir(0)*Kx+vect_dir(1)*Ky+vect_dir(2)*Kz);
      extr.extruder(dom_surfacique);
    }

  // on remet le nouveau domaine 3D dans le repere du domaine 3D initial
  dimension = 3;

  Noms les_fcts(3);
  les_fcts[0] = Nom(xa,"%.16e")+Nom("+x*(")+Nom(Ix,"%.16e")+Nom(")+y*(")+Nom(Jx,"%.16e")+Nom(")+z*(")+Nom(Kx,"%.16e")+Nom(")");
  les_fcts[1] = Nom(ya,"%.16e")+Nom("+x*(")+Nom(Iy,"%.16e")+Nom(")+y*(")+Nom(Jy,"%.16e")+Nom(")+z*(")+Nom(Ky,"%.16e")+Nom(")");
  les_fcts[2] = Nom(za,"%.16e")+Nom("+x*(")+Nom(Iz,"%.16e")+Nom(")+y*(")+Nom(Jz,"%.16e")+Nom(")+z*(")+Nom(Kz,"%.16e")+Nom(")");

  Transformer transf;
  transf.transformer(dom_surfacique,les_fcts);
  // Pour que l'on ait plus de problemes de precision ensuite quand on manipule
  // le domaine extrude, on impose les coordonnes de la frontiere nom_front du domaine
  // initial sur les coordonnees de la frontiere devant du domaine extrude
  // Il faut egalement recalculer proprement pour la frontiere derriere
  const Faces& faces=zone.frontiere(zone.rang_frontiere(nom_front)).faces();
  int nb_faces=faces.nb_faces();
  int nb_som_faces=faces.nb_som_faces();

  const Zone& zone2=dom_surfacique.zone(0);
  const Faces& faces2=zone2.frontiere(zone2.rang_frontiere("devant")).faces();
  const Faces& faces3=zone2.frontiere(zone2.rang_frontiere("derriere")).faces();
  for (int j=0; j<nb_faces; j++)
    for (int k=0; k<nb_som_faces; k++)
      {
        int som = faces.sommet(j,k);
        int som2 = faces2.sommet(j,k);
        int som3 = faces3.sommet(j,k);
        for (int i=0; i<3; i++)
          {
            dom_surfacique.coord(som2,i) = dom.coord(som,i);
            dom_surfacique.coord(som3,i) = dom.coord(som,i) + vect_dir(i);
          }
      }
  Cerr << "ExtrudeBord does not create any more PERIO boundary." << finl;
  Cerr << "Use RegroupeBord if you want to create periodic boundary condition..." << finl;
}

void ExtrudeBord::extruder_hexa_old(Nom& nom_front, Nom& nom_dom_surfacique, DoubleVect& vect_dir, int nbpas)
{

  const Domaine& dom=domaine();

  const Zone& zone=dom.zone(0);
  int nbfr=zone.nb_front_Cl();


  Domaine& dom_surfacique=ref_cast(Domaine, objet(nom_dom_surfacique));

  Zone& zone2=dom_surfacique.add(*(new Zone()));
  zone2.associer_domaine(dom_surfacique);
  zone2.typer("Hexaedre");
  zone2.nommer(nom_dom_surfacique);
  DoubleTab& sommets2=dom_surfacique.les_sommets();
  IntTab& les_elems2=zone2.les_elems();
  int i;

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere& fr=zone.frontiere(l);
      const Nom& nomfr=fr.le_nom();

      if(nomfr==nom_front)
        {
          int nbfaces=fr.nb_faces();
          const IntTab& som=fr.les_sommets_des_faces();
          IntTab newsom(4*nbfaces);
          IntTab compt(4*nbfaces);
          newsom=-100;
          compt=0;

          int nbsombord,trouve;
          nbsombord = 0;

          // ******** Table de correspondance des sommets de bords : dom_volumique - dom_surfacique

          for (i=0; i<nbfaces; i++)
            for (int j=0; j<4; j++)
              {
                trouve = 0;
                int sommet=som(i,j);

                for (int k=0; k<nbsombord+1; k++)
                  {
                    if(sommet==newsom(k))
                      {
                        k=nbsombord+1;
                        trouve = 1;
                      }
                  }

                if (trouve==0)
                  {
                    newsom[nbsombord]=sommet;
                    nbsombord++;
                  }
              }

          newsom.resize(nbsombord);
          compt.resize(nbsombord);
          sommets2.resize((nbpas+1)*nbsombord,3);

          // ******** Creation des sommets deduits par translation

          for (int j=0; j<nbpas+1; j++)
            for (i=0; i<nbsombord; i++)
              {
                sommets2(i+j*nbsombord,0)=dom.coord(newsom(i),0)+j*vect_dir(0)/nbpas;
                sommets2(i+j*nbsombord,1)=dom.coord(newsom(i),1)+j*vect_dir(1)/nbpas;
                sommets2(i+j*nbsombord,2)=dom.coord(newsom(i),2)+j*vect_dir(2)/nbpas;
              }


          // ******** Definition des tableaux de correspondance ELEM<->SOMMETS

          les_elems2.resize(nbpas*nbfaces,8);

          int  sommet0,sommet1,sommet2,sommet3;
          int  newsom0=-1,newsom1=-1,newsom2=-1,newsom3=-1;

          for (i=0; i<nbfaces; i++)
            {
              sommet0=som(i,0);
              sommet1=som(i,1);
              sommet2=som(i,2);
              sommet3=som(i,3);


              for(int j=0; j<nbsombord; j++)
                {
                  if(newsom[j]==sommet0) newsom0=j;
                  if(newsom[j]==sommet1) newsom1=j;
                  if(newsom[j]==sommet2) newsom2=j;
                  if(newsom[j]==sommet3) newsom3=j;
                }

              for(int k=0; k<nbpas; k++)
                {
                  les_elems2(i+k*nbfaces,0)=newsom0+k*nbsombord;
                  les_elems2(i+k*nbfaces,1)=newsom1+k*nbsombord;
                  les_elems2(i+k*nbfaces,2)=newsom2+k*nbsombord;
                  les_elems2(i+k*nbfaces,3)=newsom3+k*nbsombord;
                  les_elems2(i+k*nbfaces,4)=newsom0+(k+1)*nbsombord;
                  les_elems2(i+k*nbfaces,5)=newsom1+(k+1)*nbsombord;
                  les_elems2(i+k*nbfaces,6)=newsom2+(k+1)*nbsombord;
                  les_elems2(i+k*nbfaces,7)=newsom3+(k+1)*nbsombord;
                }
            }



          // ******** Definition des frontieres periodiques


          Bord& bordperio=zone2.faces_bord().add(Bord());
          bordperio.nommer("PERIO");
          bordperio.typer_faces("QUADRANGLE_3D");
          bordperio.dimensionner(0);
          IntTab faces_perio(2*nbfaces,4);

          for (i=0; i<nbfaces; i++)
            {
              sommet0=som(i,0);
              sommet1=som(i,1);
              sommet2=som(i,2);
              sommet3=som(i,3);


              for(int j=0; j<nbsombord; j++)
                {
                  if(newsom[j]==sommet0) newsom0=j;
                  if(newsom[j]==sommet1) newsom1=j;
                  if(newsom[j]==sommet2) newsom2=j;
                  if(newsom[j]==sommet3) newsom3=j;
                }

              faces_perio(i,0)=newsom0;
              faces_perio(i,1)=newsom1;
              faces_perio(i,2)=newsom2;
              faces_perio(i,3)=newsom3;

              faces_perio(i+nbfaces,0)=newsom0+nbpas*nbsombord;
              faces_perio(i+nbfaces,1)=newsom1+nbpas*nbsombord;
              faces_perio(i+nbfaces,2)=newsom2+nbpas*nbsombord;
              faces_perio(i+nbfaces,3)=newsom3+nbpas*nbsombord;
            }


          bordperio.ajouter_faces(faces_perio);




          // ******** Definition des frontieres parois


          Bord& bordparoi=zone2.faces_bord().add(Bord());
          bordparoi.nommer("PAROI");
          bordparoi.typer_faces("QUADRANGLE_3D");
          bordparoi.dimensionner(0);


          int nbsombordparoi=0;

          // ***** recherche des sommets appartenant au contour de la face de bord *****

          for (i=0; i<nbfaces; i++)
            for (int j=0; j<4; j++)
              for (int k=0; k<nbsombord; k++)
                if(som(i,j)==newsom(k))  compt(k)++; // nb de faces se rapportant a chacun des sommets

          for (int k=0; k<nbsombord; k++)
            if(compt(k)<3) nbsombordparoi++; // nb de sommets a extruder pour constituer les faces de parois

          // PQ : 05/05/04 : ATTENTION : des sommets internes peuvant etre connectes qu'a
          //                        3 faces (cf. maillage en "O" d'un cylindre)
          //                        -> on se place donc dans le cas ou les sommets de bords ne sont connectes qu'a
          //                        2 elements (excluant de fait les bords types VDF en marche d'escalier).


          // **** Extension des aretes pour former les faces de la frontiere paroi

          IntTab faces_paroi(nbsombordparoi*nbpas,4);
          int ip=0;

          for (i=0; i<nbfaces; i++)
            {
              sommet0=som(i,0);
              sommet1=som(i,1);
              sommet2=som(i,2);
              sommet3=som(i,3);

              int compt0=4,compt1=4,compt2=4,compt3=4;

              for(int j=0; j<nbsombord; j++)
                {
                  if(newsom[j]==sommet0)
                    {
                      newsom0=j;
                      compt0=compt(j);
                    }
                  if(newsom[j]==sommet1)
                    {
                      newsom1=j;
                      compt1=compt(j);
                    }
                  if(newsom[j]==sommet2)
                    {
                      newsom2=j;
                      compt2=compt(j);
                    }
                  if(newsom[j]==sommet3)
                    {
                      newsom3=j;
                      compt3=compt(j);
                    }
                }


              if( (compt0<3) && (compt1<3) ) // arete[0-1]
                {
                  for(int k=0; k<nbpas; k++)
                    {
                      faces_paroi(ip+k,0)=newsom0+k*nbsombord;
                      faces_paroi(ip+k,1)=newsom0+(k+1)*nbsombord;
                      faces_paroi(ip+k,2)=newsom1+k*nbsombord;
                      faces_paroi(ip+k,3)=newsom1+(k+1)*nbsombord;
                    }
                  ip+=nbpas;
                }


              if( (compt1<3) && (compt3<3) ) // arete[1-3]
                {
                  for(int k=0; k<nbpas; k++)
                    {
                      faces_paroi(ip+k,0)=newsom1+k*nbsombord;
                      faces_paroi(ip+k,1)=newsom1+(k+1)*nbsombord;
                      faces_paroi(ip+k,2)=newsom3+k*nbsombord;
                      faces_paroi(ip+k,3)=newsom3+(k+1)*nbsombord;
                    }
                  ip+=nbpas;
                }


              if( (compt3<3) && (compt2<3) ) // arete[3-2]
                {
                  for(int k=0; k<nbpas; k++)
                    {
                      faces_paroi(ip+k,0)=newsom3+k*nbsombord;
                      faces_paroi(ip+k,1)=newsom3+(k+1)*nbsombord;
                      faces_paroi(ip+k,2)=newsom2+k*nbsombord;
                      faces_paroi(ip+k,3)=newsom2+(k+1)*nbsombord;
                    }
                  ip+=nbpas;
                }


              if( (compt2<3) && (compt0<3) ) // arete[2-0]
                {
                  for(int k=0; k<nbpas; k++)
                    {
                      faces_paroi(ip+k,0)=newsom2+k*nbsombord;
                      faces_paroi(ip+k,1)=newsom2+(k+1)*nbsombord;
                      faces_paroi(ip+k,2)=newsom0+k*nbsombord;
                      faces_paroi(ip+k,3)=newsom0+(k+1)*nbsombord;
                    }
                  ip+=nbpas;
                }
            }

          bordparoi.ajouter_faces(faces_paroi);

        }//fin de la condition sur la frontiere a extruder
    }//fin de parcours de toutes les frontieres
  Nom mfile=nom_dom_surfacique;
  mfile+=".geom";

  {
    SFichier file(mfile);
    file<<dom_surfacique;
    file.close();
  }
  {
    EFichier file(mfile);
    file>>dom_surfacique;
    file.close();
  }
}
