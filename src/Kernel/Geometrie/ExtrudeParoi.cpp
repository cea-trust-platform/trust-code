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
//////////////////////////////////////////////////////////////////////////////
//
// File:        ExtrudeParoi.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <Faces_builder.h>
#include <ExtrudeParoi.h>
#include <Array_tools.h>
#include <TRUSTList.h>
#include <TRUSTTab.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(ExtrudeParoi,"ExtrudeParoi",Interprete_geometrique_base);

ExtrudeParoi::ExtrudeParoi()
{
  projection_normale_bord=1;
  nb_couche=1;
  type=0;
  epaisseur.resize_array(1);
  epaisseur[0]=0.5;
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
Sortie& ExtrudeParoi::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


void calcul_normal2(const ArrOfDouble& pt0, const ArrOfDouble& pt1, const ArrOfDouble& pt2, ArrOfDouble& normal)
{
  normal[0]=(pt1[1]-pt0[1])*(pt2[2]-pt0[2])-((pt2[1]-pt0[1])*(pt1[2]-pt0[2]));
  normal[1]=(pt1[2]-pt0[2])*(pt2[0]-pt0[0])-((pt2[2]-pt0[2])*(pt1[0]-pt0[0]));
  normal[2]=(pt1[0]-pt0[0])*(pt2[1]-pt0[1])-((pt2[0]-pt0[0])*(pt1[1]-pt0[1]));
}

void calcul_normal_norme2(const ArrOfDouble& pt0, const ArrOfDouble& pt1, const ArrOfDouble& pt2, ArrOfDouble& normal)
{
  calcul_normal2(pt0, pt1,  pt2, normal);
  normal/=norme_array(normal);
}


void calcul_tab_norme(DoubleTab& tab)
{
  for(int i=0; i<tab.dimension(0); i++)
    {
      double norm = sqrt(tab(i,0)*tab(i,0)+tab(i,1)*tab(i,1)+tab(i,2)*tab(i,2));
      norm += DMINFLOAT;
      tab(i,0)/=norm;
      tab(i,1)/=norm;
      tab(i,2)/=norm;
    }
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
Entree& ExtrudeParoi::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


// Description:
//    Fonction principale de l'interprete ExtrudeParoi
//    Creation d'une couche de prismes en paroi
//    (prismes tetraedrises en 3)
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
Entree& ExtrudeParoi::interpreter_(Entree& is)
{
  Nom nom_dom;
  Param param(que_suis_je());
  param.ajouter("domaine",&nom_dom,Param::REQUIRED);
  param.ajouter("nom_bord",&nom_front,Param::REQUIRED);
  param.ajouter("epaisseur",&epaisseur);
  param.ajouter_flag("critere_absolu",&type);
  param.ajouter("projection_normale_bord",&projection_normale_bord);
  param.lire_avec_accolades_depuis(is);
  epaisseur.array_trier_retirer_doublons();
  nb_couche=epaisseur.size_array();
  associer_domaine(nom_dom);
  Scatter::uninit_sequential_domain(domaine());
  extrude(domaine());
  Scatter::init_sequential_domain(domaine());
  return is;
}

// Description:
// Precondition:
// Parametre: Zone& zone
//    Signification: la zone dont on veut raffiner les elements
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void ExtrudeParoi::extrude(Domaine& dom)
{
  Zone& zone = dom.zone(0);

  if  (zone.type_elem()->que_suis_je() == "Tetraedre")
    {
      Cerr << "ExtrudeParoi ..... " << finl;
    }
  else
    {
      Cerr << "The " << zone.type_elem()->que_suis_je() <<"s are not treated with ExtrudeParoi"<<finl;
      exit();
    }

  IntTab& les_elems = zone.les_elems();
  int oldsz = les_elems.dimension(0);
  int nbs = zone.nb_som();
  IntTab elem_traite(oldsz);
  int oldnbsom = zone.nb_som();

  Faces lesfaces;
  //zone.creer_faces(les_faces);
  {
    // bloc a factoriser avec Zone_VF.cpp :
    Type_Face type_face = zone.type_elem().type_face(0);
    lesfaces.typer(type_face);
    lesfaces.associer_zone(zone);

    Static_Int_Lists connectivite_som_elem;
    const int     nb_sommets_tot = zone.domaine().nb_som_tot();
    const IntTab&    elements       = zone.les_elems();

    construire_connectivite_som_elem(nb_sommets_tot,
                                     elements,
                                     connectivite_som_elem,
                                     1 /* include virtual elements */);

    Faces_builder faces_builder;
    IntTab elem_faces; // Tableau dont on aura pas besoin
    faces_builder.creer_faces_reeles(zone,
                                     connectivite_som_elem,
                                     lesfaces,
                                     elem_faces);
  }
  ArrOfDouble ep_abs(epaisseur);
  IntTab& faces_voisins = lesfaces.voisins();
  DoubleTab& coord=dom.les_sommets();

  IntList  List_som;
  IntTab som_arete;

  DoubleTab  normale_som(nbs,3);
  normale_som=0.;
  DoubleVect dmin_som(nbs);
  dmin_som=1e6;

  DoubleTab new_soms;                // les nouveaux sommets
  IntTab new_elems;                 // les nouveaux elements


  for (int l=0; l<zone.nb_front_Cl(); l++)
    {
      const Frontiere& fr=zone.frontiere(l);
      const Nom& nomfr=fr.le_nom();

      if(nomfr==nom_front)
        {
          int nbfaces=fr.nb_faces();
          int ndeb = fr.num_premiere_face();
          const IntTab& sommet=fr.les_sommets_des_faces();


          new_elems.resize(oldsz+nbfaces*3*nb_couche,4);
          new_elems.inject_array(les_elems);
          som_arete.resize(3*nbfaces,2);


          ArrOfInt som_elem(4); // sommet rattache a l'element frontiere
          ArrOfInt som_face(3); // sommet de la face frontiere

          ArrOfDouble pt0(3),pt1(3),pt2(3),pt_ext(3);
          ArrOfDouble normale(3),vect_int(3);


          // calcul de la normale portee par chaque sommet de la frontiere
          ////////////////////////////////////////////////////////////////

          int compt=0;

          for (int i=0; i<nbfaces; i++)
            {
              int elem=faces_voisins(i+ndeb,0);
              if (elem==-1) elem=faces_voisins(i+ndeb,1);

              for(int j=0; j<4; j++)   som_elem[j]=les_elems(elem,j);
              for(int j=0; j<3; j++)
                {
                  som_face[j]=sommet(i,j);
                  List_som.add_if_not(som_face[j]) ;
                }

              //rangt par ordre croissant des sommets

              int stot=som_face[0]+som_face[1]+som_face[2];

              int som0=min_array(som_face);
              int som2=max_array(som_face);
              int som1=stot-som0-som2;


              som_arete(compt,0)=som0;
              som_arete(compt,1)=som1;
              compt++;

              som_arete(compt,0)=som0;
              som_arete(compt,1)=som2;
              compt++;

              som_arete(compt,0)=som1;
              som_arete(compt,1)=som2;
              compt++;

              int som_ext=0;

              for(int j=0; j<4; j++)
                {
                  int ok=0;
                  for(int k=0; k<3; k++)
                    if(som_elem[j]==som_face[k]) ok=1;
                  if(ok==0)
                    {
                      som_ext=som_elem[j] ;
                      break;
                    }
                }

              for(int j=0; j<3; j++)
                {
                  pt0[j]=coord(som_face[0],j);
                  pt1[j]=coord(som_face[1],j);
                  pt2[j]=coord(som_face[2],j);
                  vect_int[j]=coord(som_ext,j)-pt0[j];
                }

              calcul_normal_norme2(pt0,pt1,pt2,normale);
              double dist_paroi = dotproduct_array(normale,vect_int);
              if (dist_paroi<0.) normale *=-1.;

              for(int k=0; k<3; k++)
                {
                  int som=som_face[k];
                  dmin_som(som) = std::min(dmin_som(som),std::fabs(dist_paroi));
                  for(int j=0; j<3; j++) normale_som(som,j) += normale[j];
                }
            }

        }//if(nomfr==nom_front)
    }//for (l<zone.nb_front_Cl()

  calcul_tab_norme(normale_som);

  Cerr << "minimum distance node-wall : "<< dmin_som.mp_min_vect()<< finl;
  if(type==1) Cerr << "thickness of the layer "<< ep_abs[nb_couche-1] << finl;

  if(type==1 && (dmin_som.mp_min_vect()<=ep_abs[nb_couche-1]))
    {
      Cerr << "Error !! The thickness of the layer is greater than the minimum distance node-wall" << finl;
      exit();
    }

  // Recherche des aretes de nom_front partagee avec d'autres frontieres
  //////////////////////////////////////////////////////////////////////

  tri_lexicographique_tableau(som_arete);
  IntTab som_arete_bord(som_arete.dimension(0),3);

  int nba=som_arete.dimension(0);
  int compt=0;

  for(int i=1; i<nba; i++)
    {
      if( (som_arete(i,0)==som_arete(i-1,0)) && (som_arete(i,1)==som_arete(i-1,1)) ) i++;
      else
        {
          som_arete_bord(compt,0)=som_arete(i-1,0);
          som_arete_bord(compt,1)=som_arete(i-1,1);
          compt++;

          if(i==nba)
            {
              som_arete_bord(compt,0)=som_arete(i,0);
              som_arete_bord(compt,1)=som_arete(i,1);
              compt++;
            }
        }
    }

  int nba_bord=compt;

  som_arete_bord.resize(nba_bord,3);

  ArrOfInt corresp_bord(nba_bord);
  corresp_bord=-1;

  for (int num_front=0; num_front<zone.nb_front_Cl(); num_front++)
    {
      const Frontiere& fr=zone.frontiere(num_front);
      const Nom& nomfr=fr.le_nom();

      if(nomfr!=nom_front)
        {
          int nbfaces=fr.nb_faces();
          const IntTab& sommet=fr.les_sommets_des_faces();

          ArrOfInt som_face(3); // sommets de la face frontiere

          for (int i=0; i<nbfaces; i++)
            {
              for(int j=0; j<3; j++) som_face[j]=sommet(i,j);

              int stot=som_face[0]+som_face[1]+som_face[2];

              int som0=min_array(som_face);
              int som2=max_array(som_face);
              int som1=stot-som0-som2;

              for(int j=0; j<nba_bord; j++)
                {
                  if((som_arete_bord(j,0)==som0) && (som_arete_bord(j,1)==som1))
                    {
                      som_arete_bord(j,2)=som2;
                      corresp_bord[j]=num_front;
                      break;
                    }
                  if((som_arete_bord(j,0)==som0) && (som_arete_bord(j,1)==som2))
                    {
                      som_arete_bord(j,2)=som1;
                      corresp_bord[j]=num_front;
                      break;
                    }
                  if((som_arete_bord(j,0)==som1) && (som_arete_bord(j,1)==som2))
                    {
                      som_arete_bord(j,2)=som0;
                      corresp_bord[j]=num_front;
                      break;
                    }
                }
            }
        }
    }

  if(min_array(corresp_bord)==-1)
    {
      Cerr << "Problem with the boundary edges !!" << finl;
      exit();
    }


  // projection de la normale de bord dans le plan de la frontiere voisine de bord
  ////////////////////////////////////////////////////////////////////////////////

  if(projection_normale_bord)
    {
      ArrOfDouble pt1(3),pt2(3),pt3(3);
      ArrOfDouble n1(3),n2(3),n_proj(3);

      for(int i=0; i<nba_bord; i++)
        {
          int s1 = som_arete_bord(i,0);
          int s2 = som_arete_bord(i,1);
          int s3 = som_arete_bord(i,2);

          for(int j=0; j<3; j++)
            {
              n1[j]=normale_som(s1,j) ;
              n2[j]=normale_som(s2,j) ;

              pt1[j]=coord(s1,j);
              pt2[j]=coord(s2,j);
              pt3[j]=coord(s3,j);
            }

          calcul_normal_norme2(pt1,pt2,pt3,n_proj);
          double ori = dotproduct_array(n1,n_proj);
          if (ori<0.) n_proj *=-1.;

          double psc1=dotproduct_array(n1,n_proj);
          double psc2=dotproduct_array(n2,n_proj);

          for(int j=0; j<3; j++)
            {
              normale_som(s1,j) -= psc1*n_proj[j];
              normale_som(s2,j) -= psc2*n_proj[j];
            }
        }
    }

  // creation des sommets de la couche prismatique
  ////////////////////////////////////////////////

  int nb_som = List_som.size();
  int newnbsom = oldnbsom+nb_couche*nb_som;

  new_soms.resize(newnbsom,3);
  new_soms.inject_array(coord);

  for (int i=0; i<nb_som; i++)
    {
      int som=List_som[i];

      if(type==0) // traduction de l'epaisseur relative en epaisseur absolue
        {
          for(int j=0; j<nb_couche; j++) ep_abs[j]=epaisseur[j]*dmin_som(som);
        }

      for(int j=0; j<3; j++)
        {
          new_soms(oldnbsom+i,j) = new_soms(som,j); // creation des nouveaux sommets rattaches a la paroi

          if(nb_couche>1)                               // creation des nouveaux sommets rattaches aux couches successives
            for(int k=0; k<nb_couche-1; k++)
              new_soms(oldnbsom+(k+1)*nb_som+i,j) = new_soms(som,j)+ep_abs[k]*normale_som(som,j);
          new_soms(som,j) += ep_abs[nb_couche-1]*normale_som(som,j); // translation des anciens sommets
        }
    }

  // creation des elements de la couche prismatique
  /////////////////////////////////////////////////

  IntTab som_front;

  for (int l=0; l<zone.nb_front_Cl(); l++)
    {
      const Frontiere& fr=zone.frontiere(l);
      const Nom& nomfr=fr.le_nom();

      if(nomfr==nom_front)
        {
          int nbfaces=fr.nb_faces();
          const IntTab& sommet=fr.les_sommets_des_faces();

          som_front.resize(nbfaces,3);

          int cpt=0;

          for (int i=0; i<nbfaces; i++)
            {
              int s1=sommet(i,0);
              int s2=sommet(i,1);
              int s3=sommet(i,2);

              int stot=s1+s2+s3;

              int i1=std::min(std::min(s1,s2),s3);
              int i3=std::max(std::max(s1,s2),s3);
              int i2=stot-i1-i3;

              int i4=oldnbsom+List_som.rang(i1);
              int i5=oldnbsom+List_som.rang(i2);
              int i6=oldnbsom+List_som.rang(i3);

              som_front(i,0)=i4;
              som_front(i,1)=i5;
              som_front(i,2)=i6;

              for(int k=0; k<nb_couche; k++)
                {
                  // sommets sup de la couche

                  int ii1=i4+(k+1)*nb_som;
                  int ii2=i5+(k+1)*nb_som;
                  int ii3=i6+(k+1)*nb_som;

                  if(k==(nb_couche-1))
                    {
                      ii1=i1;
                      ii2=i2;
                      ii3=i3;
                    }

                  // sommets inf de la couche

                  int ii4=i4+k*nb_som;
                  int ii5=i5+k*nb_som;
                  int ii6=i6+k*nb_som;


                  // connectivite sommets-element inspiree de Extruder_en3

                  new_elems(oldsz+cpt,0) = ii1;
                  new_elems(oldsz+cpt,1) = ii2;
                  new_elems(oldsz+cpt,2) = ii3;
                  new_elems(oldsz+cpt,3) = ii6;
                  cpt++;

                  new_elems(oldsz+cpt,0) = ii1;
                  new_elems(oldsz+cpt,1) = ii2;
                  new_elems(oldsz+cpt,2) = ii5;
                  new_elems(oldsz+cpt,3) = ii6;
                  cpt++;

                  new_elems(oldsz+cpt,0) = ii1;
                  new_elems(oldsz+cpt,1) = ii4;
                  new_elems(oldsz+cpt,2) = ii5;
                  new_elems(oldsz+cpt,3) = ii6;
                  cpt++;

                  mettre_a_jour_sous_zone(zone,i,oldsz,0);
                }
            }
        }//if(nomfr==nom_front)
    }//for (l<zone.nb_front_Cl()


  coord.reset();
  dom.ajouter(new_soms);
  les_elems.ref(new_elems);

  // Reconstruction de l'octree
  zone.invalide_octree();
  zone.typer("Tetraedre");

  Cerr << "  Reconstruction of the Octree" << finl;
  zone.construit_octree();
  Cerr << "  Octree rebuilt" << finl;


  {
    Cerr << "Reconstruction of the boundaries" << finl;
    LIST_CURSEUR(Bord) curseur(zone.faces_bord());;;
    int num_front=0;
    while(curseur)
      {
        Faces& lesfacesbord=curseur->faces();
        lesfacesbord.typer(Faces::triangle_3D);
        IntTab& sommets=lesfacesbord.les_sommets();
        if(curseur->le_nom()==nom_front)
          {
            sommets.ref(som_front);
          }
        else
          {
            IntTab som_front2(sommets);

            int nbf =lesfacesbord.nb_faces();
            int nbf2=0;

            for (int i=0; i<nba_bord; i++)
              if(corresp_bord[i]==num_front) nbf2++;

            som_front2.resize(nbf+2*nb_couche*nbf2,3);

            int compt2=nbf;

            for (int i=0; i<nba_bord; i++)
              if(corresp_bord[i]==num_front)
                {
                  int s1=som_arete_bord(i,0);
                  int s2=som_arete_bord(i,1);

                  int s1_new=oldnbsom+List_som.rang(s1);
                  int s2_new=oldnbsom+List_som.rang(s2);

                  for(int k=0; k<nb_couche; k++)
                    {
                      // sommets sup de la couche

                      int s1_sup=s1_new+(k+1)*nb_som;
                      int s2_sup=s2_new+(k+1)*nb_som;

                      if(k==(nb_couche-1))
                        {
                          s1_sup=s1;
                          s2_sup=s2;
                        }

                      // sommets inf de la couche

                      int s1_inf=s1_new+k*nb_som;
                      int s2_inf=s2_new+k*nb_som;


                      som_front2(compt2,0)=s1_sup;
                      som_front2(compt2,1)=s2_sup;
                      som_front2(compt2,2)=s2_inf;
                      compt2++;

                      som_front2(compt2,0)=s1_sup;
                      som_front2(compt2,1)=s1_inf;
                      som_front2(compt2,2)=s2_inf;
                      compt2++;
                    }
                }

            sommets.ref(som_front2);
          }
        num_front++;
        ++curseur;
      }
  }

}

