/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <DecoupeBord.h>
#include <EFichier.h>
#include <TRUSTArrays.h>
#include <Param.h>
#include <SFichier.h>
#include <Parser_U.h>
#include <Synonyme_info.h>

Implemente_instanciable_32_64(DecoupeBord_32_64,"DecoupeBord",Interprete_geometrique_base_32_64<_T_>);
Add_synonym(DecoupeBord,"DecoupeBord_pour_rayonnement");

// XD decoupebord_pour_rayonnement interprete decoupebord 1 To subdivide the external boundary of a domain into several parts (may be useful for better accuracy when using radiation model in transparent medium). To specify the boundaries of the fine_domain_name domain to be splitted. These boundaries will be cut according the coarse mesh defined by either the keyword domaine_grossier (each boundary face of the coarse mesh coarse_domain_name will be used to group boundary faces of the fine mesh to define a new boundary), either by the keyword nb_parts_naif (each boundary of the fine mesh is splitted into a partition with nx*ny*nz elements), either by a geometric condition given by a formulae with the keyword condition_geometrique. If used, the coarse_domain_name domain should have the same boundaries name of the fine_domain_name domain. NL2 A mesh file (ASCII format, except if binaire option is specified) named by default newgeom (or specified by the nom_fichier_sortie keyword) will be created and will contain the fine_domain_name domain with the splitted boundaries named boundary_name%I (where I is between from 0 and n-1). Furthermore, several files named boundary_name%I and boundary_name_xv will be created, containing the definition of the subdived boundaries. newgeom will be used to calculate view factors with geom2ansys script whereas only the boundary_name_xv files will be necessary for the radiation calculation. The file listb will contain the list of the boundaries boundary_name%I.

template <typename _SIZE_>
Sortie& DecoupeBord_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& DecoupeBord_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


//// Helper implementation class - only visible here
template <typename _SIZE_>
class Impl_32_64
{
public:
  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using IntVect_t = IVect_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using ArrsOfInt_t = ArrsOfInt_T<_SIZE_>;

  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Frontiere_t = Frontiere_32_64<_SIZE_>;

  static void create_listb_from_domaine2(const Domaine_t& dom1, const Domaine_t& dom_oth,const Noms& nomdec);
  static void create_listb_from_xyz(const Domaine_t& dom, const Noms& nomdec,const Noms& expr);
  static void create_listb_naif(const Domaine_t& dom, const Noms& nomdec,const ArrOfInt& nbdec);
  static void create_listb_geom(const Domaine_t& dom, const Noms& nomdec,const ArrOfInt& nbdec);

  static inline int check_front_sz(const Frontiere_t& fr)
  {
    int_t tmp=fr.nb_faces();
    if (tmp >= std::numeric_limits<int>::max())
      {
        Cerr << "Too many faces in frontiere '" << fr.le_nom() << "' (exceed 32b limit) - the support for this is not yet implemented." << endl;
        Cerr << "Please contact TRUST support!" << endl;
        Process::exit(-1);
      }
    return (int)tmp;
  }
};


template <typename _SIZE_>
void Impl_32_64<_SIZE_>::create_listb_from_domaine2(const Domaine_t& dom1, const Domaine_t& dom_oth,const Noms& nomdec)
{
  const DoubleTab_t& xs1=dom1.coord_sommets();
  const DoubleTab_t& xs2=dom_oth.coord_sommets();

  int nbfr=dom1.nb_front_Cl();

  SFichier listb(dom1.le_nom()+".boundary_list");

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere_t& fr1=dom1.frontiere(l);
      const Nom& nomfr1=fr1.le_nom();
      int l2=dom_oth.rang_frontiere(nomfr1);

      if(l2<0)
        {
          Cerr << " The domains " << dom1.le_nom() << " and " << dom_oth.le_nom() << " do not have the same names of boundaries " << finl;
          Cerr<<nomfr1<<" not found in "<<dom1.le_nom() <<finl;
          Cerr << " Make sure they were generated in a similar way (almost to the mesh size) " << finl;
          Process::exit();
        }
      const Frontiere_t& fr2=dom_oth.frontiere(l2);

      if (nomdec.search(nomfr1)!=-1)
        {
          int_t nbfaces1=fr1.nb_faces();
          const IntTab_t& sommets_face1 = (ref_cast(Frontiere_t,fr1)).les_sommets_des_faces();

          int nbfaces2 = check_front_sz(fr2);
          const IntTab_t& sommets_face2 = (ref_cast(Frontiere_t,fr2)).les_sommets_des_faces();

          ArrsOfInt_t faces_associees(nbfaces2);

          ArrOfDouble xg1(Objet_U::dimension); // centre de gravite face de fr1
          ArrOfDouble xg2(Objet_U::dimension); // centre de gravite face de fr2

          int nb_som_face=(int)sommets_face1.dimension(1);

          for (int_t face1=0; face1<nbfaces1; face1++) // Generation de Liste_faces_associees
            {
              xg1=0.;

              for (int i=0; i<nb_som_face; i++)
                for (int j=0; j<Objet_U::dimension; j++)
                  xg1[j]+=xs1(sommets_face1(face1,i),j);  // centre de gravite de la face1 (non divise par nb_som_face)

              // recherche de la face de dom2 correspondante

              double dist_min=1.e6;
              int face_min=-1;

              for (int face2=0; face2<nbfaces2; face2++)
                {
                  xg2=0.;

                  for (int i=0; i<nb_som_face; i++)
                    for (int j=0; j<Objet_U::dimension; j++)
                      xg2[j]+=xs2(sommets_face2(face2,i),j);  //centre de gravite de la face2 (non divise par nb_som_face)

                  double dist=0.;

                  for (int j=0; j<Objet_U::dimension; j++)
                    dist+=(xg2[j]-xg1[j])*(xg2[j]-xg1[j]);

                  if(dist<dist_min)
                    {
                      dist_min=dist ;
                      face_min=face2;
                    }
                }

              faces_associees[face_min].append_array(face1);
            }

          for (int face2=0; face2<nbfaces2; face2++) // Ecriture des fichiers de sortie
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

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::create_listb_from_xyz(const Domaine_t& dom, const Noms& nomdec,const Noms& expr)
{
  int n1=nomdec.size();
  int n2=expr.size();
  if(n1!=n2)
    {
      Cerr<<"The numbers of boundaries in 'bords_a_decouper' and 'condition_geometrique' have to be the same"<<finl;
      Process::exit();
    }

  const DoubleTab_t& xs1=dom.coord_sommets();
  Parser_U parser;
  parser.setNbVar(Objet_U::dimension);
  parser.addVar("x");
  parser.addVar("y");
  if (Objet_U::dimension==3)
    parser.addVar("z");

  int nbfr=dom.nb_front_Cl();

  SFichier listb(dom.le_nom()+".boundary_list");

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere_t& fr1=dom.frontiere(l);
      const Nom& nomfr1=fr1.le_nom();

      int inc=nomdec.search(nomfr1);
      if (inc!=-1)
        {
          Nom expr2(expr[inc]);
          parser.setString(expr2);
          parser.parseString();

          int nbfaces1 = check_front_sz(fr1);

          const IntTab_t& sommets_face1 = (ref_cast(Frontiere_t,fr1)).les_sommets_des_faces();

          ArrsOfInt_t faces_associees(1000); // dimensionnement en dur qui devrait suffire

          ArrOfDouble xg1(Objet_U::dimension); // centre de gravite face de fr1

          int nb_som_face=(int)sommets_face1.dimension(1);
          int nb=0;

          for (int face1=0; face1<nbfaces1; face1++) // Generation de Liste_faces_associees
            {
              xg1=0.;

              for (int i=0; i<nb_som_face; i++)
                for (int j=0; j<Objet_U::dimension; j++)
                  xg1[j]+=xs1(sommets_face1(face1,i),j);  // centre de gravite de la face1 (non divise par nb_som_face)
              xg1/=nb_som_face;
              parser.setVar("x",xg1[0]);

              parser.setVar("y",xg1[1]);
              if (Objet_U::dimension==3)
                parser.setVar("z",xg1[2]);
              double res=parser.eval();
              int face_min=static_cast<int>(res+0.5);
              nb=std::max(nb,face_min);
              //faces_associees.dimensionner_force(nb+1);

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

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::create_listb_naif(const Domaine_t& dom, const Noms& nomdec,const ArrOfInt& nbdec)
{
  int n1=nomdec.size();
  int n2=nbdec.size_array();
  if(n1!=n2)
    {
      Cerr << "The numbers of boundaries in 'bords_a_decouper' and 'nb_parts_naif' are not the same : check your data file"<<finl;
      Process::exit();
    }

  SFichier listb(dom.le_nom()+".boundary_list");
  int nbfr=dom.nb_front_Cl();
  for (int l=0; l<nbfr; l++)
    {
      const Frontiere_t& fr=dom.frontiere(l);
      int nbfaces = check_front_sz(fr);

      int compt=0;
      const Nom& nomfr=fr.le_nom();
      int inc=nomdec.search(nomfr);
      if (inc!=-1)
        {
          int nb=nbdec[inc];
          nb=std::min(nb,nbfaces);
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

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::create_listb_geom(const Domaine_t& dom, const Noms& nomdec,const ArrOfInt& nbdec)
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

  const DoubleTab_t& xs=dom.coord_sommets();
  int nbfr=dom.nb_front_Cl();

  Parser_U parser;
  parser.setNbVar(3);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  Noms expr_tot(n1);

  for (int l=0; l<nbfr; l++)
    {
      const Frontiere_t& fr=dom.frontiere(l);
      int_t nbfaces=fr.nb_faces();
      const Nom& nomfr=fr.le_nom();
      int inc=nomdec.search(nomfr);
      if (inc!=-1)
        {
          int nb1=nbdec[2*inc];
          int nb2=nbdec[2*inc+1];

          if (nbfaces!=0)
            {
              const IntTab_t& sommets_face = (ref_cast(Frontiere_t,fr)).les_sommets_des_faces();
              int nb_som_face = (int)dom.les_sommets().dimension(1);

              ArrOfDouble coord_min(3);
              coord_min=1e6;
              ArrOfDouble coord_max(3);
              coord_max=-1e6;

              for (int_t face=0; face<nbfaces; face++)
                {
                  for (int i=0; i<nb_som_face; i++)
                    for (int j=0; j<3; j++)
                      {
                        coord_min[j] = std::min(coord_min[j],xs(sommets_face(face,i),j));
                        coord_max[j] = std::max(coord_max[j],xs(sommets_face(face,i),j));
                      }
                }

              Nom expr="0.+";

              if(coord_min[0]==coord_max[0]) // plan x=cste
                {
                  double dy=(coord_max[1]-coord_min[1])/nb1;
                  double dz=(coord_max[2]-coord_min[2])/nb2;

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
              else if (coord_min[1]==coord_max[1]) // plan y=cste
                {
                  double dx=(coord_max[0]-coord_min[0])/nb1;
                  double dz=(coord_max[2]-coord_min[2])/nb2;

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
              else if (coord_min[2]==coord_max[2]) // plan z=cste
                {
                  double dx=(coord_max[0]-coord_min[0])/nb1;
                  double dy=(coord_max[1]-coord_min[1])/nb2;

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
  create_listb_from_xyz(dom, nomdec,expr_tot);
  Cerr <<" ... ok "<<finl;
}

template <typename _SIZE_>
Entree& DecoupeBord_32_64<_SIZE_>::interpreter_(Entree& is)
{
  using Impl_ = Impl_32_64<_SIZE_>;

  Nom nom_dom1,nom_dom2;
  Nom nom_fichier_sortie("");
  //LIST(Nom) list_nom_dec;
  Noms nomdec;
  ArrOfInt nb_parts,nb_parts_geom;
  Param param(this->que_suis_je());
  int binaire=0;
  Noms expr;
  param.ajouter("domaine",&nom_dom1,Param::REQUIRED); // XD_ADD_P ref_domaine not_set
  param.ajouter("domaine_grossier",&nom_dom2); // XD_ADD_P ref_domaine not_set
  param.ajouter("nb_parts_naif",&nb_parts); // XD_ADD_P listentier not_set
  param.ajouter("nb_parts_geom",&nb_parts_geom); // XD_ADD_P listentier not_set
  param.ajouter("condition_geometrique",&expr); // XD_ADD_P listchaine not_set
  param.ajouter("bords_a_decouper",&nomdec,Param::REQUIRED); // XD_ADD_P listchaine not_set
  param.ajouter("nom_fichier_sortie",&nom_fichier_sortie); // XD_ADD_P chaine not_set
  param.ajouter("binaire",&binaire); // XD_ADD_P entier not_set
  param.lire_avec_accolades_depuis(is);
  // on fait une copie de dom1 pour le modifier
  this->associer_domaine(nom_dom1);
  const Domaine_t& dom_1=this->domaine();
  if (nom_fichier_sortie=="")
    {
      nom_fichier_sortie=dom_1.le_nom()+".newgeom";
    }
  Domaine_t dom1;
  dom1=dom_1;
  if (nom_dom2!=Nom())
    {
      if (!sub_type(Domaine_t, this->objet(nom_dom2)))
        {
          Cerr << nom_dom2 << " is not an object of type Domaine " << finl;
          Cerr << " Please correct your data set" << finl;
          Process::exit();
        }

      const Domaine_t& dom_2=ref_cast(Domaine_t, this->objet(nom_dom2));
      Impl_::create_listb_from_domaine2(dom1,dom_2,nomdec);
    }
  else
    {
      if (expr.size()!=0)
        Impl_::create_listb_from_xyz(dom1,nomdec,expr);
      else if (nb_parts.size_array()!=0)
        Impl_::create_listb_naif(dom1,nomdec,nb_parts);
      else if (nb_parts_geom.size_array()!=0)
        Impl_::create_listb_geom(dom1,nomdec,nb_parts_geom);
      else
        {
          Cerr<<"in DecoupeBord_32_64_pour_rayonnement : You must specify domaine_grossier or nb_parts_naif or nb_part_geom or condition_geometrique"<<finl;
          Process::exit();
        }

    }
  Nom nom_file(dom1.le_nom()+".boundary_list");
  decouper(dom1,nom_file);

  Cout <<"Writing of the new geometry in the file " << nom_fichier_sortie <<finl;
  SFichier fic;
  fic.set_bin(binaire);
  fic.ouvrir(nom_fichier_sortie);
  fic <<dom1 <<finl;

  return is;
}

template <typename _SIZE_>
void DecoupeBord_32_64<_SIZE_>::decouper(Domaine_t& dom, const Nom& nom_file)
{
  Cerr << "Splitting to the radiation of the boundaries of the domain " << dom.le_nom() << finl;
  int_t nb_faces_bord_sa=dom. nb_faces_bord();
  int_t nb_faces_raccord_sa=dom. nb_faces_raccord();
  if (this->nproc()!=1)
    {
      Cerr << " The splitting DecoupeBord_32_64 can be done only in sequential " << finl;
      Process::exit();
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

  int nbfr=dom.nb_front_Cl();
  Bords_t& listbord=dom.faces_bord();
  Raccords_t& listrac=dom.faces_raccord();
  Bords_t listbord2;
  Raccords_t listrac2;
  int nbbord=dom.nb_bords();
  IntVect decoup(nbfr);  // BoolVect in fact ...
  Noms nomsdesbordsorg(nbfr);
  // on parcourt toutes les frontieres
  for (int b=0; b<nbfr; b++)
    {
      Frontiere_t& org=dom.frontiere(b);
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
                  decoup(b)=1;
                  Bord_t toto_bord;
                  Raccord_t toto_rac;
                  if (b>=nbbord) toto_rac.typer(org.le_type());
                  Frontiere_t& toto=(b<nbbord?ref_cast(Frontiere_t,toto_bord):ref_cast(Frontiere_t,toto_rac.valeur()));
                  Nom nombord_dec_bis=nomborddec[nf];
                  toto.nommer(nombord_dec_bis);
                  IntVect_t listfaces;
                  // on recupere la liste des faces
                  EFichier lis(nombord_dec_bis);
                  lis>>listfaces;
                  // on genere les faces associes a ce nouveau bord
                  Faces_t& Facesorg=org.faces();
                  Faces_t& newfaces=toto.faces();
                  newfaces.typer(Facesorg.type_face());
                  int_t newnbfaces=listfaces.size();
                  int nbsom=Facesorg.nb_som_faces();
                  newfaces.dimensionner(newnbfaces);
                  const IntTab_t& sommetsorg=Facesorg.les_sommets();
                  IntTab_t& newsommet=newfaces.les_sommets();
                  for (int_t i=0; i<newnbfaces; i++)
                    for (int j=0; j<nbsom; j++)
                      newsommet(i,j)=sommetsorg(listfaces[i],j);
                  DoubleTab_t position;

                  Faces_t::Calculer_centres_gravite(position,newfaces.type_face(),dom.coord_sommets(),newsommet);

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
      listbord.suppr(dom.bord(nomsdesbordsorg[b2]));
  for (int r2=nbbord; r2<nbfr; r2++)
    if (decoup(r2))
      listrac.suppr(dom.raccord(nomsdesbordsorg[r2]));
  listbord.add(listbord2);
  listrac.add(listrac2);

  int_t nb_faces_bord=dom.nb_faces_bord();
  if (nb_faces_bord!=nb_faces_bord_sa)
    {
      Cerr<<"the new number of boundary faces is false :"<<nb_faces_bord;
      Cerr<<" instead of "<<nb_faces_bord_sa<<finl;
      Process::exit();
    }
  int_t nb_faces_raccord=dom.nb_faces_raccord();
  if (nb_faces_raccord!=nb_faces_raccord_sa)
    {
      Cerr<<"the new number of connection faces is false :"<<nb_faces_raccord;
      Cerr<<" instead of "<<nb_faces_raccord_sa<<finl;
      Process::exit();
    }
}


template class DecoupeBord_32_64<int>;
#if INT_is_64_ == 2
template class DecoupeBord_32_64<trustIdType>;
#endif

