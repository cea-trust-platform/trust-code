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
//////////////////////////////////////////////////////////////////////////////
//
// File:        DecoupeBord.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <DecoupeBord.h>
#include <EFichier.h>
#include <Vect_ArrOfInt.h>
#include <Param.h>
#include <SFichier.h>
#include <Parser_U.h>

Implemente_instanciable(DecoupeBord,"DecoupeBord_pour_rayonnement|DecoupeBord",Interprete_geometrique_base);

Sortie& DecoupeBord::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& DecoupeBord::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

void create_listb_from_domaine2(const Domaine& dom1, const Domaine& dom2,const Noms& nomdec)
{
  const DoubleTab& xs1=dom1.coord_sommets();
  const Zone& zone1=dom1.zone(0);
  const DoubleTab& xs2=dom2.coord_sommets();
  const Zone& zone2=dom2.zone(0);

  int nbfr=zone1.nb_front_Cl();

  SFichier listb(dom1.le_nom()+".boundary_list");

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere& fr1=zone1.frontiere(l);
      const Nom& nomfr1=fr1.le_nom();
      int l2=zone2.rang_frontiere(nomfr1);

      if(l2<0)
        {
          Cerr << " The domains " << dom1.le_nom() << " and " << dom2.le_nom() << " do not have the same names of boundaries " << finl;
          Cerr<<nomfr1<<" not found in "<<dom1.le_nom() <<finl;
          Cerr << " Make sure they were generated in a similar way (almost to the mesh size) " << finl;
          Process::exit();
        }
      const Frontiere& fr2=zone2.frontiere(l2);

      if (nomdec.search(nomfr1)!=-1)
        {
          int nbfaces1=fr1.nb_faces();
          const IntTab& sommets_face1 = (ref_cast(Frontiere,fr1)).les_sommets_des_faces();

          int nbfaces2=fr2.nb_faces();
          const IntTab& sommets_face2 = (ref_cast(Frontiere,fr2)).les_sommets_des_faces();

          VECT(ArrOfInt) faces_associees(nbfaces2);

          ArrOfDouble xg1(Objet_U::dimension); // centre de gravite face de fr1
          ArrOfDouble xg2(Objet_U::dimension); // centre de gravite face de fr2

          int face1,face2,i,j;
          int nb_som_face=sommets_face1.dimension(1);


          for (face1=0; face1<nbfaces1; face1++) // Generation de Liste_faces_associees
            {
              xg1=0.;

              for (i=0; i<nb_som_face; i++)
                for (j=0; j<Objet_U::dimension; j++)
                  xg1(j)+=xs1(sommets_face1(face1,i),j);  // centre de gravite de la face1 (non divise par nb_som_face)

              // recherche de la face de dom2 correspondante

              double dist_min=1.e6;
              int           face_min=-1;

              for (face2=0; face2<nbfaces2; face2++)
                {
                  xg2=0.;

                  for (i=0; i<nb_som_face; i++)
                    for (j=0; j<Objet_U::dimension; j++)
                      xg2(j)+=xs2(sommets_face2(face2,i),j);  //centre de gravite de la face2 (non divise par nb_som_face)

                  double dist=0.;

                  for (j=0; j<Objet_U::dimension; j++) dist+=(xg2(j)-xg1(j))*(xg2(j)-xg1(j));

                  if(dist<dist_min)
                    {
                      dist_min=dist ;
                      face_min=face2;
                    }
                }

              faces_associees[face_min].set_smart_resize(1);
              faces_associees[face_min].append_array(face1);
            }

          for (face2=0; face2<nbfaces2; face2++) // Ecriture des fichiers de sortie
            {
              Nom nomfic(nomfr1);
              nomfic+="%";
              Nom n;
              n=face2;
              nomfic+=n;
              SFichier s(nomfic);
              s<<faces_associees[face2]<<finl;
              listb<<nomfic<<finl;
              s.close();
            }

        }
    }

  listb.close();
}

void create_listb_from_xyz(const Domaine& dom1,const Noms& nomdec,const Noms& expr)
{
  int n1=nomdec.size();
  int n2=expr.size();
  if(n1!=n2)
    {
      Cerr<<"The numbers of boundaries in 'bords_a_decouper' and 'condition_geometrique' have to be the same"<<finl;
      Process::exit();
    }

  const DoubleTab& xs1=dom1.coord_sommets();
  const Zone& zone1=dom1.zone(0);
  Parser_U parser;
  parser.setNbVar(Objet_U::dimension);
  parser.addVar("x");
  parser.addVar("y");
  if (Objet_U::dimension==3)
    parser.addVar("z");


  int nbfr=zone1.nb_front_Cl();

  SFichier listb(dom1.le_nom()+".boundary_list");

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere& fr1=zone1.frontiere(l);
      const Nom& nomfr1=fr1.le_nom();

      int inc=nomdec.search(nomfr1);
      if (inc!=-1)
        {
          Nom expr2(expr[inc]);
          parser.setString(expr2);
          parser.parseString();

          int nbfaces1=fr1.nb_faces();
          const IntTab& sommets_face1 = (ref_cast(Frontiere,fr1)).les_sommets_des_faces();



          VECT(ArrOfInt) faces_associees(1000); // dimensionnement en dur qui devrait suffire

          ArrOfDouble xg1(Objet_U::dimension); // centre de gravite face de fr1

          int face1,i,j;
          int nb_som_face=sommets_face1.dimension(1);
          int nb=0;


          for (face1=0; face1<nbfaces1; face1++) // Generation de Liste_faces_associees
            {
              xg1=0.;

              for (i=0; i<nb_som_face; i++)
                for (j=0; j<Objet_U::dimension; j++)
                  xg1(j)+=xs1(sommets_face1(face1,i),j);  // centre de gravite de la face1 (non divise par nb_som_face)
              xg1/=nb_som_face;
              parser.setVar("x",xg1(0));

              parser.setVar("y",xg1(1));
              if (Objet_U::dimension==3)
                parser.setVar("z",xg1(2));
              double res=parser.eval();
              int face_min=(int)(res+0.5);
              nb=max(nb,face_min);
              //faces_associees.dimensionner_force(nb+1);
              faces_associees[face_min].set_smart_resize(1);
              faces_associees[face_min].append_array(face1);
              //Cerr << face1 << " associee a " << face_min << finl;
            }

          for (int face2=0; face2<nb+1; face2++) // Ecriture des fichiers de sortie
            {
              Nom nomfic(nomfr1);
              nomfic+="%";
              Nom n;
              n=face2;
              nomfic+=n;
              SFichier s(nomfic);
              s<<faces_associees[face2]<<finl;
              listb<<nomfic<<finl;
              s.close();
            }
        }
    }

  listb.close();
}

void create_listb_naif(const Domaine& dom1,const Noms& nomdec,const ArrOfInt& nbdec)
{
  int n1=nomdec.size();
  int n2=nbdec.size_array();
  if(n1!=n2)
    {
      Cerr << "The numbers of boundaries in 'bords_a_decouper' and 'nb_parts_naif' are not the same : check your data file"<<finl;
      Process::exit();
    }

  SFichier listb(dom1.le_nom()+".boundary_list");
  const Zone& zone=dom1.zone(0);
  int nbfr=zone.nb_front_Cl();
  for (int l=0; l<nbfr; l++)
    {
      const Frontiere& fr=zone.frontiere(l);
      int nbfaces=fr.nb_faces();
      int compt=0;
      const Nom& nomfr=fr.le_nom();
      int inc=nomdec.search(nomfr);
      if (inc!=-1)
        {
          int nb=nbdec[inc];
          nb=min(nb,nbfaces);
          if (nbfaces!=0)
            {
              int nc=nbfaces/nb;

              //Cerr<<nbfaces<<finl;
              if ((nb*nc)<nbfaces) nc++;
              //Cerr<<"nc "<<nc<<finl;
              Cerr<<nomfr<<" splitting in "<<nb<<" parts, average number of faces "<<nbfaces/(1.*nb)<<finl;
              //  Cerr<<"nombre moyen de faces  "<<nc<<" bord decoupe "<<nomfr<<" en "<<nbfaces/nc<<" parties"<<finl;
              for (int i=0; i<nb; i++)
                {
                  if (i==nb-1) nc=nbfaces-compt;
                  IntVect p(nc);
                  for (int j=0; j<nc; j++) p[j]=compt++;
                  Nom nomfic(nomfr);
                  nomfic+="%";
                  Nom n;
                  n=i;
                  nomfic+=n;
                  SFichier s(nomfic);
                  s<<p<<finl;
                  listb<<nomfic<<finl;
                  s.close();
                }
            }

        }
    }
  listb.close();
}

void create_listb_geom(const Domaine& dom1,const Noms& nomdec,const ArrOfInt& nbdec)
{
  if((Objet_U::dimension)!=3)
    {
      Cerr<<"nb_parts_geom has been defined for dimension 3 only"<<finl;;
      Process::exit();
    }

  int n1=nomdec.size();
  int n2=nbdec.size_array();
  if((2*n1)!=n2)
    {
      Cerr << "The 'nb_parts_geom' has to be twice the  number of boundaries in 'bords_a_decouper' : check your data file"<<finl;
      Process::exit();
    }

  const DoubleTab& xs=dom1.coord_sommets();
  const Zone& zone=dom1.zone(0);
  int nbfr=zone.nb_front_Cl();

  Parser_U parser;
  parser.setNbVar(3);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  Noms expr_tot(n1);

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere& fr=zone.frontiere(l);
      int nbfaces=fr.nb_faces();
      const Nom& nomfr=fr.le_nom();
      int inc=nomdec.search(nomfr);
      if (inc!=-1)
        {
          int nb1=nbdec[2*inc];
          int nb2=nbdec[2*inc+1];

          if (nbfaces!=0)
            {

              //int nbfaces=fr.nb_faces();
              const IntTab& sommets_face = (ref_cast(Frontiere,fr)).les_sommets_des_faces();
              int nb_som_face=dom1.les_sommets().dimension(1);

              ArrOfDouble coord_min(3);
              coord_min=1e6;
              ArrOfDouble coord_max(3);
              coord_max=-1e6;

              for (int face=0; face<nbfaces; face++)
                {
                  for (int i=0; i<nb_som_face; i++)
                    for (int j=0; j<3; j++)
                      {
                        coord_min(j) = dmin(coord_min(j),xs(sommets_face(face,i),j));
                        coord_max(j) = dmax(coord_max(j),xs(sommets_face(face,i),j));
                      }
                }

              Nom expr="0.+";

              if(coord_min(0)==coord_max(0)) // plan x=cste
                {
                  double dy=(coord_max(1)-coord_min(1))/nb1;
                  double dz=(coord_max(2)-coord_min(2))/nb2;

                  for (int j=1; j<nb1; j++)
                    {
                      double pas=j*dy;
                      expr+="(y>";
                      expr+=Nom(pas);
                      expr+=")+";
                    }
                  expr+=Nom(nb1);
                  expr+="*(";
                  for (int k=1; k<nb2; k++)
                    {
                      double pas=k*dz;
                      expr+="(z>";
                      expr+=Nom(pas);
                      expr+=")+";
                    }
                }
              else if (coord_min(1)==coord_max(1)) // plan y=cste
                {
                  double dx=(coord_max(0)-coord_min(0))/nb1;
                  double dz=(coord_max(2)-coord_min(2))/nb2;

                  for (int i=1; i<nb1; i++)
                    {
                      double pas=i*dx;
                      expr+="(x>";
                      expr+=Nom(pas);
                      expr+=")+";
                    }
                  expr+=Nom(nb1);
                  expr+="*(";
                  for (int k=1; k<nb2; k++)
                    {
                      double pas=k*dz;
                      expr+="(z>";
                      expr+=Nom(pas);
                      expr+=")+";
                    }
                }
              else if (coord_min(2)==coord_max(2)) // plan z=cste
                {
                  double dx=(coord_max(0)-coord_min(0))/nb1;
                  double dy=(coord_max(1)-coord_min(1))/nb2;

                  for (int i=1; i<nb1; i++)
                    {
                      double pas=i*dx;
                      expr+="(x>";
                      expr+=Nom(pas);
                      expr+=")+";
                    }
                  expr+=Nom(nb1);
                  expr+="*(";
                  for (int j=1; j<nb2; j++)
                    {
                      double pas=j*dy;
                      expr+="(y>";
                      expr+=Nom(pas);
                      expr+=")+";
                    }
                }
              else
                {
                  Cerr <<"the boundary "<<nomfr<<" is not plane : nb_parts_geom can't be used"<<finl;
                  Process::exit();
                }

              expr+="0.)";
              Cerr << "fontiere : "<<nomfr<<" -  expression : " <<expr<<finl;
              parser.setString(expr);
              parser.parseString();
              expr_tot[inc]=expr;

            }//if (nbfaces!=0)
        }//if (inc!=-1)
    }//for (int l=0;l<nbfr;l++)

  Cerr <<"create_listb_from_xyz ....  "<<finl;
  create_listb_from_xyz(dom1,nomdec,expr_tot);
  Cerr <<" ... ok "<<finl;
}

Entree& DecoupeBord::interpreter_(Entree& is)
{
  Nom nom_dom1,nom_dom2;
  Nom nom_fichier_sortie("");
  LIST(Nom) list_nom_dec;
  Noms nomdec;
  ArrOfInt nb_parts,nb_parts_geom;
  Param param(que_suis_je());
  int binaire=0;
  Noms expr;
  param.ajouter("domaine",&nom_dom1,Param::REQUIRED);
  param.ajouter("domaine_grossier",&nom_dom2);
  param.ajouter("nb_parts_naif",&nb_parts);
  param.ajouter("nb_parts_geom",&nb_parts_geom);
  param.ajouter("condition_geometrique",&expr);
  param.ajouter("bords_a_decouper",&nomdec,Param::REQUIRED);
  param.ajouter("nom_fichier_sortie",&nom_fichier_sortie);
  param.ajouter("binaire",&binaire);
  param.lire_avec_accolades_depuis(is);
  // on fait une copie de dom1 pour le modifier
  associer_domaine(nom_dom1);
  const Domaine& dom_1=domaine();
  if (nom_fichier_sortie=="")
    {
      nom_fichier_sortie=dom_1.le_nom()+".newgeom";
    }
  Domaine dom1;
  dom1=dom_1;
  if (nom_dom2!=Nom())
    {
      if (!sub_type(Domaine, objet(nom_dom2)))
        {
          Cerr << nom_dom2 << " is not an object of type Domaine " << finl;
          Cerr << " Please correct your data set" << finl;
          exit();
        }

      const Domaine& dom_2=ref_cast(Domaine, objet(nom_dom2));
      create_listb_from_domaine2(dom1,dom_2,nomdec);
    }
  else
    {
      //if (nb_parts.size_array()==0)
      //  {
      //    Cerr<<" You must specify domaine_grossier or cut into n"<<finl;
      //    exit();
      //  }
      if (expr.size()!=0)
        {
          create_listb_from_xyz(dom1,nomdec,expr);
        }
      else if (nb_parts.size_array()!=0)
        {
          create_listb_naif(dom1,nomdec,nb_parts);
        }
      else if (nb_parts_geom.size_array()!=0)
        {
          create_listb_geom(dom1,nomdec,nb_parts_geom);
        }
      else
        {
          Cerr<<"in DecoupeBord_pour_rayonnement : You must specify domaine_grossier or nb_parts_naif or nb_part_geom or condition_geometrique"<<finl;
          exit();
        }

    }
  Nom nom_file(dom1.le_nom()+".boundary_list");
  Decouper(dom1,nom_file);

  Cout <<"Writing of the new geometry in the file " << nom_fichier_sortie <<finl;
  SFichier fic;
  fic.set_bin(binaire);
  fic.ouvrir(nom_fichier_sortie);
  fic <<dom1 <<finl;

  return is;
}


void DecoupeBord::Decouper(Domaine& dom, const Nom& nom_file)
{
  Cerr << "Splitting to the radiation of the boundaries of the domain " << dom.le_nom() << finl;
  int nb_faces_bord_sa=dom.zone(0). nb_faces_bord();
  int nb_faces_raccord_sa=dom.zone(0). nb_faces_raccord();
  if (nproc()!=1)
    {
      Cerr << " The splitting DecoupeBord can be done only in sequential " << finl;
      exit();
    }


  // on ecrit la nvelle geom en sequentiel
  int nbtot=0;
  {
    // on regarde le nombre de lignes de nom_file
    EFichier listb2(nom_file);
    Nom toto;
    listb2>>toto;
    while(listb2.good())
      {
        listb2>>toto;
        nbtot++;
      }
  }

  EFichier listb2(nom_file);
  Noms nomborddec(nbtot);
  for (int nf=0; nf<nbtot; nf++) listb2>>nomborddec[nf];

  Zone& zone=dom.zone(0);

  int nbfr=zone.nb_front_Cl();
  Bords& listbord=zone.faces_bord();
  Raccords& listrac=zone.faces_raccord();
  Bords listbord2;
  Raccords listrac2;
  int nbbord=zone.nb_bords();
  IntVect decoup(nbfr);
  Noms nomsdesbordsorg(nbfr);
  // on parcourt toutes les frontieres
  for (int b=0; b<nbfr; b++)
    {
      Frontiere& org=zone.frontiere(b);
      const Nom& nombord=org.le_nom();
      nomsdesbordsorg[b]=nombord;
      decoup(b)=0;
      SFichier bord_xv(dom.le_nom()+"."+nomsdesbordsorg[b]+"_xv");
      bord_xv.setf(ios::scientific);
      bord_xv.precision(12);
      for (int nf=0; nf<nbtot; nf++)
        {
          Nom nombord_dec=nomborddec[nf];
          const char* marq=strchr(nombord_dec,'%');
          if (marq)
            {
              nombord_dec.prefix(marq);
              // on regarde dans les bords decoupes si ils correspondent a la frontiere courante
              if (nombord==nombord_dec)
                {

                  // exit();
                  decoup(b)=1;
                  Bord toto_bord;
                  Raccord toto_rac;
                  if (b>=nbbord) toto_rac.typer(org.le_type());
                  Frontiere& toto=(b<nbbord?ref_cast(Frontiere,toto_bord):ref_cast(Frontiere,toto_rac.valeur()));
                  Nom nombord_dec_bis=nomborddec[nf];
                  toto.nommer(nombord_dec_bis);
                  IntVect listfaces;
                  // on recupere la liste des faces
                  EFichier lis(nombord_dec_bis);
                  lis>>listfaces;
                  // on genere les faces associes a ce nouveau bord
                  Faces& Facesorg=org.faces();
                  Faces& newfaces=toto.faces();
                  newfaces.typer(Facesorg.type_face());
                  int newnbfaces=listfaces.size();
                  int nbsom=Facesorg.nb_som_faces();
                  newfaces.dimensionner(newnbfaces);
                  const IntTab& sommetsorg=Facesorg.les_sommets();
                  IntTab& newsommet=newfaces.les_sommets();
                  for (int i=0; i<newnbfaces; i++)
                    for (int j=0; j<nbsom; j++)
                      newsommet(i,j)=sommetsorg(listfaces[i],j);
                  DoubleTab position;

                  calculer_centres_gravite(position,newfaces.type_face(),dom.coord_sommets(),newsommet);

                  bord_xv<<nombord_dec_bis<<finl;
                  bord_xv<<position<<finl;
                  if (b<nbbord)
                    listbord2.add(toto_bord);
                  else
                    listrac2.add(toto_rac);
                }
            }
        }
      bord_xv.close();
    }
  // on supprime les bords decoupes du domaine
  for (int b2=0; b2<nbbord; b2++)
    if (decoup(b2))
      listbord.suppr(zone.bord(nomsdesbordsorg[b2]));
  for (int r2=nbbord; r2<nbfr; r2++)
    if (decoup(r2))
      listrac.suppr(zone.raccord(nomsdesbordsorg[r2]));
  listbord.add(listbord2);
  listrac.add(listrac2);

  int nb_faces_bord=dom.zone(0).nb_faces_bord();
  if (nb_faces_bord!=nb_faces_bord_sa)
    {
      Cerr<<"the new number of boundary faces is false :"<<nb_faces_bord;
      Cerr<<" instead of "<<nb_faces_bord_sa<<finl;
      exit();
    }
  int nb_faces_raccord=dom.zone(0).nb_faces_raccord();
  if (nb_faces_raccord!=nb_faces_raccord_sa)
    {
      Cerr<<"the new number of connection faces is false :"<<nb_faces_raccord;
      Cerr<<" instead of "<<nb_faces_raccord_sa<<finl;
      exit();
    }


}

