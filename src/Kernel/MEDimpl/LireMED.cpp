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
// File:        LireMED.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/53
//
//////////////////////////////////////////////////////////////////////////////

#include <LireMED.h>
#include <Domaine.h>
#include <med++.h>
#include <NettoieNoeuds.h>
#include <sys/stat.h>
#include <SFichier.h>
#include <IntList.h>
#include <Vect_IntTab.h>
#include <Vect_ArrOfInt.h>
#include <Polyedre.h>
#include <Scatter.h>
#include <Synonyme_info.h>
#include <RegroupeBord.h>
#include <Char_ptr.h>

// fonction utile pour creer un Nom d'une taille donnee
void dimensionne_char_ptr_taille(Char_ptr& nom ,int taille_d_un_mot,int nb)
{
  nom.allocate(taille_d_un_mot*nb);
}


Implemente_instanciable(LireMED,"Lire_MED",Interprete_geometrique_base);
Add_synonym(LireMED,"Read_med");
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
Sortie& LireMED::printOn(Sortie& os) const
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
Entree& LireMED::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

#ifndef MED_
Entree& LireMED::interpreter_(Entree& is)
{
  med_non_installe();
  return is;
}
void LireMED::lire_geom( Nom& nom_fic,Domaine& dom,const Nom& nom_dom,const Nom& nom_dom_trio,int isvef, int isfamilyshort)
{
  med_non_installe();
}
#else
Entree& LireMED::interpreter_(Entree& is)
{
  Cerr << "Syntax: Lire_MED [ vef ] [ family_names_from_group_names | short_family_names ] domaine_name mesh_name filename.med" << finl;
  int isvefforce=0;
  int isfamilyshort = 0;
  Nom nom_dom_trio,nom_dom, nom_fic;
  is >> nom_dom_trio ;
  if (Motcle(nom_dom_trio)=="vef")
    {
      isvefforce=1;
      is>>nom_dom_trio;
    }
  if (Motcle(nom_dom_trio)=="family_names_from_group_names")
    {
      isfamilyshort = 2;
      is>>nom_dom_trio;
    }
  else if (Motcle(nom_dom_trio)=="short_family_names")
    {
      isfamilyshort = 1;
      is>>nom_dom_trio;
    }
  //is >> nom_dom;
  lire_nom_med(nom_dom,is);
  Cerr << "domaine_name:" << nom_dom_trio << finl;
  Cerr << "mesh_name:" << nom_dom << finl;
  is >> nom_fic;
  Cerr << "MED file read: " << nom_fic << finl;
  // on retire _0000 si il existe et on cree le bon fichier
  Nom nom_fic2(nom_fic);
  nom_fic2.prefix(".med");
  if (nom_fic2==nom_fic)
    {
      Cerr<<"Error, the MED file should have .med as extension."<<finl;
      Cerr<<"See the syntax of LireMED keyword." << finl;
      exit();
    }
  Nom nom_fic3(nom_fic2);
  if (nom_fic3.prefix("_0000") != nom_fic2 )
    {
      nom_fic3+=".med";
      nom_fic=nom_fic3.nom_me(me());
    }
  associer_domaine(nom_dom_trio);
  Domaine& dom=domaine();
  lire_geom(nom_fic,dom,nom_dom,nom_dom_trio,isvefforce,isfamilyshort);
  return is;
}

void convert_med_int_to_inttab(ArrOfInt& tab,med_int* tabmed)
{
  if (sizeof(med_int)!=sizeof(int))
    {
      int taille=tab.size_array();
      //    cerr<<sizeof(tab)<<" "<<sizeof(int)<<" "<<sizeof(tab)/sizeof(int)<<" "<<taille<<endl;
      Cerr<<"medint* copy"<<finl;

      for (int i=0; i<taille; i++)
        tab[i]=tabmed[i];
      // on peut liberer le tableau maintenant
      delete [] tabmed;
    }
}
med_int* alloue_med_int_from_inttab(const ArrOfInt& tab)
{
  med_int* tabmed;
  if (sizeof(med_int)==sizeof(int))
    tabmed=(med_int *)tab.addr();
  else
    {

      int taille=tab.size_array();
      Cerr<<"medint* creation"<<finl;
      tabmed=new med_int[taille];
    }
  return tabmed;
}

//int medecrgeom();


extern "C" int MEDimport(char*,char*);

void test_version(Nom& nom)
{
  // on regarde si le fichier est d'une version differente, si oui
  // on cree un fichier med2.2 et on change le nom du fichier
  med_int fid,majeur,mineur,release;
  fid = MEDfileOpen(nom,MED_ACC_RDONLY);
  if (fid<0)
    {
      Cerr<<"Problem when trying to open the file "<<nom<<endl;
      Process::exit();
    }
#ifdef MED30
  MEDfileNumVersionRd(fid,&majeur,&mineur,&release);
  MEDfileClose(fid);
  if (majeur == 3)
    {
      return ;
    }
#else
  MEDversionLire(fid,&majeur,&mineur,&release);
  MEDfileClose(fid);
  if (majeur == 2 && mineur >= 2)
    {
      return ;
    }
  if (majeur>2)
    {
      Cerr<<"Conversion of med file version "<<majeur <<" to med file version 2 not possible"<<finl;
      Process::exit();
    }
#endif

  // On serialise pour eviter que le fichier soit cree plusieurs fois en //

  Nom nom2bis("Conv_");
  // ajout prefixe Conv_
  Nom nom2 ;

  char* nomtmp = new char[strlen(nom)+1];
  int compteur=strlen(nom);

  strcpy(nomtmp,nom);

  const char* ptr=nomtmp+compteur;
  int pas_de_slach=0;

  while((*ptr!='/')&&(compteur>0))  // recherche du dernier slach dans le nom du fichier
    {
      ptr--;
      compteur--;
      if (*ptr=='/')
        {
          pas_de_slach=1;
        }
    }

  if (pas_de_slach==0) // Cas ou le fichier med est dans le repertoire courant
    {
      nom2=nom2bis;
      nom2+=nom;
    }
  else // Cas ou le fichier med est defini par un chemin
    {
      Nom cible(ptr);
      cible.suffix("/");  // nom du fichier
      nom2bis+= cible;   // nom du fichier + prefix Conv_

      Nom nomentier(nom);
      nomentier.prefix(cible);  // nom du chemin pour trouver fichier

      nomentier+=nom2bis;  // nom du chemin + prefixe Conv . nom fichier
      nom2=nomentier;
    }

  delete [] nomtmp;

  for  (int p=0; p<Process::nproc(); p++)
    {
      if (p==Process::me())
        {
          //  if(Process::nproc()>1)
          // nom2=nom2.nom_me(Process::me());

          //Cerr<<"ICI"<<nom2<<finl;
          int a_creer;
          ifstream test(nom2);
          if (test)
            {
              //Cerr<<"le fichier "<<nom2<<" existe deja , je ne fais rien "<<finl;//exit();
              a_creer=0;
              struct stat org,newf;
              stat(nom,&org);
              stat(nom2,&newf);
              if (org.st_mtime>newf.st_mtime)
                a_creer=1;

            }
          else
            a_creer=1;
          if (a_creer)
            {
              Cerr<<"Creation of the file "<<nom2<<" to newer format."<<finl;
              MEDimport(Char_ptr(nom),Char_ptr(nom2));
            }
          else
            Cerr<<"The file "<<nom2<<" is up to date."<<finl;
        }
      Process::barrier();
    }
  nom=nom2;
  return;

}

int medliregeom(Nom& nom_fic,const Nom& nom_dom,const Nom& nom_dom_trio,int& dimension, DoubleTab& sommets, Nom& type_elem, Elem_geom& ele,IntTab& les_elems, Noms& v_type_face,VECT(IntTab)& v_all_faces_bord, VECT(ArrOfInt)& v_familles,Noms& noms_bords,ArrOfInt& Indice_bords,int isvef, int isfamilyshort)
{
  // IntTab les_elems;
  int ret=0;
  test_version(nom_fic);

  int fid=MEDfileOpen(nom_fic,MED_ACC_RDONLY);
  if (fid<0)
    {
      Cerr<<"Problem when trying to open the file "<<nom_fic<<endl;
      Process::exit();
    }
#ifdef MED30
  int nmaillage=MEDnMesh(fid);
#else
  int nmaillage=MEDnMaa(fid);
#endif

  if (nmaillage<1)
    {
      Cerr<<"We have "<<nmaillage<<" meshes instead of 1 mesh."<<finl;
      return -1;
    }
  Char_ptr maa;
  dimensionne_char_ptr_taille(maa,MED_NAME_SIZE);
  int monmaillage=-1;
#ifdef MED30
  Char_ptr description,dtunit,axisname,axisunit;
  dimensionne_char_ptr_taille(description,MED_COMMENT_SIZE);
  dimensionne_char_ptr_taille(dtunit,MED_SNAME_SIZE);
  dimensionne_char_ptr_taille(axisname,MED_SNAME_SIZE,3);
  dimensionne_char_ptr_taille(axisunit,MED_SNAME_SIZE,3);
  med_sorting_type sortingtype;
  med_mesh_type meshtype;
  med_int spacedim,meshdim,nstep;
  med_axis_type axistype;

#endif
  for (int i=0; i<nmaillage; i++)
    {
      med_int dim;
#ifdef MED30
      ret=MEDmeshInfo(fid,i+1,maa,&dim,&meshdim,&meshtype,description,dtunit,&sortingtype,&nstep,&axistype,axisname,axisunit);
      spacedim=MEDmeshnAxis(fid,i+1);
      if (spacedim<0) spacedim=meshdim;
#else
      ret = MEDmaaInfo(fid,i+1,maa,&dim);
#endif
      Nom Nmaa(maa);
      if (nom_dom==Nmaa)
        {
          monmaillage=i+1;
          break;
        }
      else
        Cerr<<"A mesh named "<<maa<<" is found in the file " << nom_fic << finl;
    }
  if (monmaillage==-1)
    {
      Cerr<<"But the mesh_name "<<nom_dom<<" is not found into the file " << nom_fic << "." << finl;
      Cerr<<"Check the MED file or change the "<<nom_dom<<" name by "<<maa<<" name of Lire_MED interpreter in your .data file." << finl;
      Process::exit();
      return -1;
    }
  med_geometry_type type_geo=MED_POINT1;
  med_connectivity_mode type_conn=MED_NODAL;
  //Cerr<<"type_geo"<<type_geo<<finl;
#ifdef MED30
  dimension=meshdim;
  if (spacedim>-1)
    dimension=spacedim;
  med_bool chgt,transfo;
  int nsommet=MEDmeshnEntity(fid,maa,MED_NO_DT,MED_NO_IT, MED_NODE,type_geo,MED_COORDINATE, type_conn,&chgt,&transfo);
#else
  med_int med_dimension=dimension;
  ret = MEDmaaInfo(fid,monmaillage,maa,&med_dimension);
  int med_space_dimension=MEDdimEspaceLire(fid,maa);
  if (med_space_dimension>0)
    med_dimension=med_space_dimension;
  dimension=med_dimension;
  int nsommet=MEDnEntMaa(fid,maa,MED_COOR,MED_NODE,type_geo,type_conn);
#endif
  //Cerr<<"type_geo "<<type_geo<<" conn "<<type_conn<<finl;
  sommets.resize(nsommet,dimension);

  if (nsommet==0) return 0;


  //   Nom nomcoo="x       y      ";
  //   Nom unicco="m       m      ";
  //   if (dimension>2)
  //     {
  //       nomcoo+="x       ";
  //       unicco+="m       ";
  //     }
  double* sommets2=(double *)sommets.addr();
  med_axis_type rep;
#ifdef MED30
  rep=axistype;
  ret=MEDmeshNodeCoordinateRd(fid,maa,MED_NO_DT,MED_NO_IT,MED_FULL_INTERLACE,sommets2);
#else
  {
    Char_ptr nomcoo;
    dimensionne_char_ptr_taille(nomcoo,MED_SNAME_SIZE,3);
    Char_ptr unicco;
    dimensionne_char_ptr_taille(unicco,MED_SNAME_SIZE,3);
    //    ArrOfInt nufano(nsommet);
    med_int* nufano=new med_int[nsommet];
    med_booleen inonoe,inunoe;
    med_int *numnoe;


    numnoe = new med_int[nsommet] ;
    Char_ptr nomnoe;
    dimensionne_char_ptr_taille(nomnoe,MED_SNAME_SIZE,nsommet);
    ret=MEDnoeudsLire(fid,maa,dimension,sommets2,MED_FULL_INTERLACE,&rep,nomcoo,unicco,nomnoe,&inonoe,numnoe,&inunoe,nufano,nsommet);
    if (inunoe||inonoe)
      {
        Cerr<<"Problem in medliregeom. Contact TRUST support."<<finl;
        //return -1;
      }

    delete [] numnoe;
    delete [] nufano;
  }
#endif
  med_int* num_famille_elem;
  // lecture du nbre et du type elt
  const int med_nbr_type_geom=17;//MED_NBR_GEOMETRIE_MAILLE+2;
  const med_geometry_type all_cell_type1[med_nbr_type_geom]= { MED_POINT1,MED_SEG2,MED_SEG3,MED_TRIA3,MED_QUAD4,MED_TRIA6,MED_POLYGON,MED_POLYHEDRON,MED_QUAD8,MED_TETRA4,MED_PYRA5,MED_PENTA6,MED_HEXA8,MED_TETRA10,MED_PYRA13,MED_PENTA15,MED_HEXA20};
  int jelem=-1;
  {
    int nm1=0,nm;
    //  med_geometry_type type_geo;
    //    for (i=1;i<med_nbr_type_geom;i++)
    for (int i=med_nbr_type_geom-1; i>0; i--)
      if (nm1==0)
        {
          nm=0;
          //        Cerr<<nm << " "<<all_cell_type1[i]<<finl;
#ifdef MED30
          med_bool cght,transfo2;
          nm=MEDmeshnEntity(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,all_cell_type1[i], MED_CONNECTIVITY,MED_NODAL,&cght,&transfo2);
#else
          nm=MEDnEntMaa(fid,nom_dom,MED_CONN,MED_CELL,all_cell_type1[i],MED_NODAL);
#endif

          if (nm!=0)
            {
              if (nm1==0)
                {
                  nm1=nm;
                  jelem=i;
                  type_geo=all_cell_type1[i];
                  Cerr<<nm1<<" elements of kind "<<(int)type_geo<<" has been found." << finl;
                }
              else
                {
                  Cerr<<"Elements of kind "<<(int)type_geo<< " has already been readen" << finl;
                  Cerr<<"TRUST does not support different element types for the mesh."<<finl;
                  Cerr<<"The new elements of kind "<<(int)all_cell_type1[i]<<" are not read." << finl;
                  //return -1;
                }
            }

        }
    if (nm1<=0)
      {
        Cerr<<"No elements found."<<finl;
        return -1;
      }
    switch(type_geo)
      {
      case MED_QUAD4:
        type_elem="Rectangle";
        if (isvef==1) type_elem="Quadrangle";
        break;

      case MED_HEXA8:
        type_elem="Hexaedre";
        if (isvef==1) type_elem="Hexaedre_vef";
        break;
      case MED_TRIA3:
        type_elem="Triangle";
        break;
      case MED_TETRA4:
        type_elem="Tetraedre";
        break;
      case MED_PENTA6:
        type_elem="Prisme";
        break;
      case MED_POLYHEDRON:
        type_elem="Polyedre";
        break;
      case MED_POLYGON:
        type_elem="Polygone";
        break;
      case MED_SEG2:
        type_elem="Segment";
        break;
      default:
        Cerr<<"type_geo " << (int)type_geo <<" is not a supported element."<<finl;
        return -1;
        break;
      }

    if (rep==MED_CYLINDRICAL)
      {
        if (type_geo==MED_QUAD4)
          {
            type_elem="Rectangle_2D_axi";
            if (!Objet_U::bidim_axi)
              {
                Cerr<<"Warning, we will use bidim_axi keyword."<<finl;
                Objet_U::bidim_axi=1;
              }
          }
        else
          {
            Process::exit();
          }
      }
    if (rep==MED_SPHERICAL)
      {
        type_elem+="_axi";
        if (! Objet_U::axi)
          {
            Cerr<<"Warning, we will use now Axi keyword."<<finl;
            Objet_U::axi=1;
          }
      }
    Cerr<<"Element TRUST kind: "<<type_elem<<finl;
    ele.typer(type_elem);

    int is_poly=0;
    if ((type_geo==MED_POLYHEDRON)||(type_geo==MED_POLYGON))
      is_poly=1;
    int nelem=nm1;
    num_famille_elem=new med_int[nelem];
    if (is_poly==0)
      {
        int nbsom=ele.nb_som();
        //    Cerr<<"nbre de sommets par elts "<<ele.nb_som()<<finl;



        //Cerr<<"nelem bis"<<nelem<<finl;
        les_elems.resize(nelem,nbsom);
        IntTab elems_prov;
        IntTab& les_elems2=les_elems;
        //ArrOfInt num_famille_elem(nelem);
        med_int* med_les_elems2=alloue_med_int_from_inttab(les_elems2);
#ifdef MED30
        double dtb;
        med_int numdt,numit;
        MEDmeshComputationStepInfo(fid,nom_dom,1,&numdt,&numit,&dtb);
        ret=MEDmeshElementConnectivityRd(fid,nom_dom,numdt,numit,MED_CELL,type_geo,MED_NODAL,MED_FULL_INTERLACE,med_les_elems2);
        int ret2=MEDmeshEntityFamilyNumberRd(fid,nom_dom,numdt,numit,MED_CELL,type_geo,num_famille_elem);
        if (ret2<0)
          {
            //Process::exit();
            for (int ii=0; ii<nelem; ii++)
              num_famille_elem[ii] = 0;

          }
#else
        Char_ptr nomse2 ;
        dimensionne_char_ptr_taille(nomse2,MED_SNAME_SIZE,nelem);
        med_int *numse2;
        numse2 = new med_int[nelem];
        med_booleen inoele1,inuele1;

        ret=MEDelementsLire(fid,nom_dom,dimension,med_les_elems2,MED_FULL_INTERLACE,nomse2,&inoele1,numse2,&inuele1,num_famille_elem,nelem,MED_CELL,type_geo,MED_NODAL);
        delete [] numse2;
#endif
        convert_med_int_to_inttab(les_elems2,med_les_elems2);


        if (ret<0)
          {
            Cerr<<"Problem when reading the elements into the file "<<nom_fic<<finl;
            Process::exit();
          }


        //    Cerr<<"elem "<<ret<<finl;
      }
    else
      {

        // on a des polyedres...

        //avant d'oublier on lit les familles
#ifdef MED30
        if (MEDmeshEntityFamilyNumberRd(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,type_geo,num_famille_elem)<0)
#else
        if (MEDfamLire(fid,nom_dom,num_famille_elem,nelem,MED_CELL,type_geo) < 0)
#endif
          for (int ii=0; ii<nelem; ii++)
            num_famille_elem[ii] = 0;

        int  NumberOfPolyedre = nm1;


        //med_int ConnectivitySize;
        //med_err err1 = MEDpolygoneInfo(fid,nom_dom,MED_CELL,MED_NODAL, &ConnectivitySize);
        med_int FacesIndexSize, NumberOfNodes;
#ifdef MED30
        med_bool cght,transfo2;
        NumberOfNodes=NumberOfPolyedre;
        NumberOfPolyedre=MEDmeshnEntity(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,MED_POLYHEDRON, MED_INDEX_FACE,MED_NODAL,&cght,&transfo2)-1; // -1 a cause du +1 ensuite !!!
        med_err err1=0;

        FacesIndexSize=MEDmeshnEntity(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,MED_POLYHEDRON, MED_INDEX_NODE,MED_NODAL,&cght,&transfo2);
#else
        med_int NumberOfFaces;
        med_err err1 = MEDpolyedreInfo(fid,nom_dom,MED_NODAL,&FacesIndexSize, &NumberOfNodes);
        NumberOfFaces = FacesIndexSize-1;
#endif
        if (err1 != 0)
          {
            Cerr<<"Error MEDpolygoneInfo"<<finl;
            Process::exit();
          }
        //Cerr<<NumberOfPolyedre<< " "<<NumberOfNodes<<" "<<FacesIndexSize<<finl;
        ArrOfInt Nodes(NumberOfNodes),FacesIndex(FacesIndexSize),PolyhedronIndex(NumberOfPolyedre+1);
        {
          med_int* med_Nodes=alloue_med_int_from_inttab(Nodes);
          med_int* med_FacesIndex=alloue_med_int_from_inttab(FacesIndex);
          med_int* med_PolyhedronIndex=alloue_med_int_from_inttab(PolyhedronIndex);
#ifdef MED30
          med_err err4 =MEDmeshPolyhedronRd(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,MED_NODAL,med_PolyhedronIndex,med_FacesIndex,med_Nodes);
#else
          med_err err4 = MEDpolyedreConnLire(fid,nom_dom,
                                             med_PolyhedronIndex,
                                             NumberOfPolyedre+1,
                                             med_FacesIndex,
                                             NumberOfFaces+1,
                                             med_Nodes,
                                             MED_NODAL);

#endif
          if (err4 != 0)
            {
              Cerr<<": MEDpolyedreConnLire returns "<<(int)err4;
              Process::exit();
            }
          convert_med_int_to_inttab(Nodes,med_Nodes);
          convert_med_int_to_inttab(FacesIndex,med_FacesIndex);
          convert_med_int_to_inttab(PolyhedronIndex,med_PolyhedronIndex);
          //Cerr<<Nodes<< " "<<FacesIndex<<" "<<PolyhedronIndex<<finl;
        }
        Nodes-=1;
        FacesIndex-=1;
        PolyhedronIndex-=1;


        ref_cast(Polyedre,ele.valeur()).affecte_connectivite_numero_global( Nodes,FacesIndex, PolyhedronIndex, les_elems);
        // on remet +1 car apres on le retire....
        les_elems+=1;
      }
  }
  assert(jelem!=-1);
  // lecture des faces
  // lecture du nbre et du type face
  // const med_geometry_type all_cell_type1[med_nbr_type_geom]={  MED_POINT1,MED_SEG2,MED_SEG3,MED_TRIA3,MED_QUAD4,MED_TRIA6,MED_QUAD8,MED_TETRA4,MED_PYRA5,MED_PENTA6,MED_HEXA8,MED_TETRA10,MED_PYRA13,MED_PENTA15,MED_HEXA20};

  //ele.typer(type_elem);
  int nb_type_face=ele.nb_type_face();
  v_familles.dimensionner(nb_type_face);
  v_all_faces_bord.dimensionner(nb_type_face);
  v_type_face.dimensionner(nb_type_face);
  int debut_cherche=jelem;
  for (int tp=0; tp<nb_type_face; tp++)
    {

      ArrOfInt& familles=v_familles[tp];
      IntTab& all_faces_bord=v_all_faces_bord[tp];
      Nom& type_face=v_type_face[tp];

      int nm1=0,nm;
      //  med_geometry_type type_geo;
#ifdef MED30
      med_entity_type type_ent=MED_DESCENDING_FACE;
      if (dimension==2) type_ent=MED_DESCENDING_EDGE;
      if (jelem==0)
        {
          type_ent=MED_NODE;
        }
#else
      med_entity_type type_ent=MED_FACE;
      if (dimension==2) type_ent=MED_ARETE;
#endif
      for (int mm=0; mm<2; mm++)
        {
          if (nm1!=0) break;

          if (mm==1) type_ent=MED_CELL;

          for (int i=debut_cherche-1 ; i>=0; i--)
            {
              if (nm1!=0) break;

              nm=0;
#ifdef MED30
              med_bool cght,transfo3;
              nm=MEDmeshnEntity(fid,nom_dom,MED_NO_DT,MED_NO_IT,type_ent,all_cell_type1[i], MED_CONNECTIVITY,MED_NODAL,&cght,&transfo3);
#else
              nm=MEDnEntMaa(fid,nom_dom,MED_CONN,type_ent,all_cell_type1[i],MED_NODAL);
#endif
              if (nm!=0)
                {
                  if (nm1==0)
                    {
                      nm1=nm;
                      type_geo=all_cell_type1[i];
                      Cerr<<"We find "<<nm1<<" faces of kind "<<(int)type_geo<<finl;
                      debut_cherche=i;
                    }
                  else
                    {
                      Cerr<<"We have already faces of kind "<<(int)type_geo<<finl;
                      Cerr<<"TRUST does not support several kind of faces."<<finl;
                      return -1;
                    }
                }

            }
        }
      if (nm1<=0)
        {
          Cerr<<"No faces found."<<finl;
        }
      else
        {
          switch(type_geo)
            {
            case MED_POINT1:
              type_face="POINT_1D";
              break;
            case MED_SEG2:
              type_face="SEGMENT_2D";
              break;
            case MED_TRIA3:
              type_face="TRIANGLE_3D";
              break;

            case MED_QUAD4:
              type_face="QUADRANGLE_3D";
              break;
            case MED_POLYGON:
              type_face="polygone_3D";
              break;
            default:
              Cerr<<"type_geo " << (int)type_geo <<" is not supported by TRUST."<<finl;
              return -1;
              break;
            }


          if (rep==MED_CYLINDRICAL)
            {
              if (type_geo==MED_SEG2)
                {
                  type_face="QUADRILATERE_2D_AXI";
                  assert(Objet_U::bidim_axi==1);
                }
              else
                {
                  Process::exit();
                }
            }
          if (rep==MED_SPHERICAL)
            {
              type_face+="_axi";
              assert(Objet_U::axi==1);
            }
          //    Cerr<<"type facem ici"<<type_face<<"a"<<finl;
          Faces face;
          face.typer(type_face);
          int nface=nm1;
          familles.resize_array(nface);
          if (type_face!="polygone_3D")
            {

              int nbsom=face.nb_som_faces();
              //Cerr<<"nbre de sommets par elts "<<nbsom<<finl;

              Char_ptr nomse2;
              med_int *numse2;

              //Cerr<<"nface bis"<<nface<<finl;
              all_faces_bord.resize(nface,nbsom);
              IntTab faces_prov;
              IntTab& all_faces_bord2=all_faces_bord;
              //IntTab& all_faces_bord2=all_faces_bord;
              dimensionne_char_ptr_taille(nomse2,MED_SNAME_SIZE,nface);
              numse2 = new med_int[nface];


              {
                med_int* med_all_faces_bord2=alloue_med_int_from_inttab(all_faces_bord2);
                med_int* med_familles=alloue_med_int_from_inttab(familles);
#ifdef MED30
                med_bool withelementname,withelementnumber, withfamnumber;
                med_int* elementnumber=numse2;
                char* elementname=nomse2;
                ret=MEDmeshElementRd(fid,nom_dom,MED_NO_DT,MED_NO_IT,type_ent,type_geo,MED_NODAL,MED_FULL_INTERLACE,med_all_faces_bord2,&withelementname,elementname,&withelementnumber,elementnumber,&withfamnumber,med_familles);
#else
                med_booleen inoele1,inuele1;
                ret=MEDelementsLire(fid,nom_dom,dimension,med_all_faces_bord2,MED_FULL_INTERLACE,nomse2,&inoele1,numse2,&inuele1,med_familles,nface,type_ent,type_geo,MED_NODAL);
                if (inoele1||inuele1)
                  {
                    // Cerr<<"bizarre "<<finl;
                    //return -1;
                  }

#endif
                delete [] numse2;
                convert_med_int_to_inttab(all_faces_bord2,med_all_faces_bord2);
                convert_med_int_to_inttab(familles,med_familles);
              }
              if (ret<0)
                {
                  Cerr<<"Problem when reading the faces "<<ret<<finl;
                  Process::exit();
                }

              //    Cerr<<"elem "<<ret<<finl;
            }
          else
            {
              // familles
              med_int* med_familles=alloue_med_int_from_inttab(familles);
#ifdef MED30
              if (MEDmeshEntityFamilyNumberRd(fid,nom_dom,MED_NO_DT,MED_NO_IT,type_ent,type_geo,med_familles)<0)
#else
              if (MEDfamLire(fid,nom_dom,med_familles,nface,type_ent,type_geo) < 0)
#endif
                for (int ii=0; ii<nface; ii++)
                  med_familles[ii] = 0;

              convert_med_int_to_inttab(familles,med_familles);
              med_int size_conn;
#ifdef MED30
              med_bool cght,transfo3;
              size_conn=nface;
              nface=MEDmeshnEntity(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,MED_POLYGON,MED_INDEX_NODE,MED_NODAL,&cght,&transfo3)-1;
#else
              int ret=MEDpolygoneInfo(fid,nom_dom,type_ent,MED_NODAL,&size_conn);
              if (ret != 0)
                {
                  Cerr<<"Error MEDpolygoneInfo"<<finl;
                  Process::exit();
                }
#endif
              int taille_index=nface+1;
              ArrOfInt connectivite(size_conn),index(taille_index);
              med_int* med_conn=alloue_med_int_from_inttab(connectivite);
              med_int* med_index=alloue_med_int_from_inttab(index);
#ifdef MED30
              ret=MEDmeshPolygonRd(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,MED_NODAL,med_index,med_conn);
#else
              ret=MEDpolygoneConnLire(fid,nom_dom,med_index,taille_index,med_conn,type_ent,MED_NODAL);
#endif
              convert_med_int_to_inttab(connectivite,med_conn);
              convert_med_int_to_inttab(index,med_index);
              int max_som_face=0;
              for (int i=0; i<nface; i++)
                {
                  int nb_som_face=index(i+1)-index(i);
                  if  (nb_som_face>max_som_face) max_som_face=nb_som_face;
                }
              all_faces_bord.resize(nface,max_som_face);
              all_faces_bord=0;

              for (int i=0; i<nface; i++)
                {
                  int nb_som_face=index(i+1)-index(i);
                  for (int j=0; j<nb_som_face; j++)
                    all_faces_bord(i,j)=connectivite(index(i)+j-1);

                }
              if (ret != 0)
                {
                  Cerr<<"Error MEDpolygoneConnlire"<<finl;
                  Process::exit();
                }
            }


        }
    }

  // lecture des familles c.a.d leur nom = les cl
  {
    int nfam;
    if ((nfam = MEDnFamily(fid,nom_dom)) < 0)
      {
        Cerr<<"Problem during the read of the families."<<finl;
        return -1;
      }
    Cerr<<"Number of families: "<<nfam<<finl;
    noms_bords.dimensionner(nfam);
    Indice_bords.resize_array(nfam);
    int non_affecte=-1000;
    med_int ngro;
    LIST(Nom) list_group;
    IntTab corres_grp_fam;
    for (int passage=0; passage<2; passage++)
      {
        if (passage==1)
          {
            // on connait maintenant le nombre de groupe
            corres_grp_fam.resize(list_group.size(),nfam);
            corres_grp_fam=non_affecte;
          }
        // on passe 2 fois
        // la premiere fois on trouve le nombre de groupes totales
        // la deuxieme fois on stocke la correspondance groupe face
        for (int i=0; i<nfam; i++)
          {
            if ((ngro = MEDnFamily(fid,nom_dom))<0)
              ret = -1;
            //Cerr<<"ngro "<<ret<<finl;

            /* nombre d'attributs */
            Char_ptr nomfam;
            med_int numfam;
            Char_ptr gro;
#ifdef MED30
            dimensionne_char_ptr_taille(nomfam,MED_NAME_SIZE);
            ngro=MEDnFamilyGroup(fid,nom_dom,i+1);
            dimensionne_char_ptr_taille(gro,MED_LNAME_SIZE,ngro);

            if (ret==0)
              {
                ret=MEDfamilyInfo(fid,nom_dom,i+1,nomfam,&numfam,gro);
              }
#else
            med_int natt;
            if ((natt = MEDnFam(fid,nom_dom)) < 0)
              ret = -1;
            Char_ptr attdes;
            med_int *attval,*attide;
            dimensionne_char_ptr_taille(nomfam,MED_NAME_SIZE);

            if (ret == 0)
              {
                attide = new med_int[natt];
                attval = new med_int[natt];
                dimensionne_char_ptr_taille(attdes,MED_COMMENT_SIZE,natt);
                dimensionne_char_ptr_taille(gro,MED_LNAME_SIZE,ngro);
                ret = MEDfamInfo(fid,nom_dom,i+1,nomfam,&numfam,attide,attval,attdes,
                                 &natt,gro,&ngro);
                delete [] attide;
                delete [] attval;
              }
#endif
            if (ret==0)
              {
                // on raccourcit nomfam
                {
                  int max=nomfam.longueur()-2;
                  while (nomfam[max]==' ') max--;
                  nomfam[max+1]='\0';
                }
                Nom nom_famille(nomfam);
                {
                  for (int ngr=0; ngr<ngro; ngr++)
                    {
                      // on recupere le ngr ieme nom de groupe
                      Char_ptr toto(gro.getChar()+MED_LNAME_SIZE*ngr);
                      {
                        // on coupe les noms apres le premier blanc
                        for (int c=0; c<toto.longueur(); c++)
                          if (toto[c]==' ')
                            {
                              toto[c]='\0';
                              break;
                            }
                      }
                      if (passage==0)
                        {
                          list_group.add(toto.getChar());
                        }
                      else
                        {

                          int grp=list_group.rang(toto.getChar());
                          corres_grp_fam(grp,i)=numfam;
                        }
                    }
                }
                if (passage==0)
                  {

                    // Cerr<<ret<<finl;
                    Cerr<< " Family of name "<<nomfam<<" , number "<<(int)numfam<<finl;
                    Indice_bords[i]=numfam;
                    if (isfamilyshort==2)
                      {
                        if (ngro>0)
                          {
                            // Cerr<<list_group(list_group.size()-1)<<finl;
                            noms_bords[i]=list_group(list_group.size()-1);

                          }
                        else
                          {
                            noms_bords[i]=nomfam;
                            Cerr<<noms_bords[i]<<" is not seen as a boundary "<< (int)numfam<<finl;
                            if (numfam<0)
                              Indice_bords[i]=-numfam;
                            Indice_bords[i]+=10000;
                          }
                      }

                    //Cerr<<"attdes " <<attdes<<finl;
                    //Cerr<<"gro "<<gro<<finl;
                    //Cerr<<natt<<" "<<attval[0]<<attide[0]<<finl;
                    // BEG PHF
                    else if ((isfamilyshort==1) && nom_famille.debute_par("FAM_"))
                      {
                        // pour certaines machines 64 bits mandriva 2006
                        int dummy=numfam;
                        Nom nnumfam(dummy);
                        int lnumfam = 5+nnumfam.longueur();
                        Nom nomfamcut(nom_famille.substr_old(lnumfam, nom_famille.longueur()-lnumfam));
                        noms_bords[i] = nomfamcut;
                        cerr << "### noms_bord = " << nomfamcut << " numfam = " << numfam << endl;
                      }
                    else
                      {
                        noms_bords[i]=nomfam;
                      }
                    // END PHF

                    // on regarde si le numero ne porte pas que sur des elts
                    int in_bord=0;
                    for (int tp=0; tp<nb_type_face; tp++)
                      for (int b=0; b<v_familles[tp].size_array(); b++)
                        if (v_familles[tp][b]==numfam)
                          {
                            in_bord=1;
                            break;
                          }
                    int  in_elem=0;
                    if (in_bord==0)
                      {
                        // on n'a pas la famille sur les bords
                        // soit c'est un bord vide soit c'est une famille d'element
                        for (int el=0; el<les_elems.dimension(0); el++)
                          if (num_famille_elem[el]==numfam)
                            {
                              in_elem=1;
                              break;
                            }
                        if (in_elem==1)
                          {
                            assert(in_bord==0);
                            // on met un numero de famille >10000 pour que la famille ne soit pas convertie en bord
                            Cerr<<noms_bords[i]<<" is not seen as a boundary "<< (int)numfam<<finl;
                            if (numfam<0)
                              Indice_bords[i]=-numfam;
                            Indice_bords[i]+=10000;
                          }
                        else
                          {
                            Cerr<< noms_bords[i]<<" is seen to be empty "<< (int)numfam<<finl;
                            if (numfam>0)  Indice_bords[i]+=10000;
                          }
                      }
                  }




              }

            //ret=MEDfamLire(fid,nom_dom,noms_bords[i],-(i+1),NULL,NULL,NULL,0,NULL,0);
            //Cerr<<"famille "<<i<<" nom "<<noms_bords[i]<<ret<<finl;
          }
      }
    if ((list_group.size())&& (Process::je_suis_maitre())&&(nom_dom_trio!=Nom()))
      {
        // If several lire_med following and in parallel, so possibility that a field reads the ssz.geo of another mesh => Error
        // That is why ssz.geo and ssz_par.geo are renamed
        // SFichier jdd("ssz.geo");
        // SFichier jdd_par("ssz_par.geo");
        SFichier jdd(nom_dom_trio + "_ssz.geo");
        SFichier jdd_old("ssz.geo"); // je garde celui ci pour les anciens jdds
        SFichier jdd_par(nom_dom_trio + "_ssz_par.geo");
        SFichier jdd_par_old("ssz_par.geo");

        Cerr<<"grp"<<list_group<<finl;
        int nb_elem=les_elems.dimension(0);

        for (int grp=0; grp<list_group.size(); grp++)
          {
            // on passe de IntList a ArrOfInt pour optimiser le temps
            ArrOfInt prov(nb_elem);
            int has_one=0;
            for (int i=0; i<nfam; i++)
              {
                int numfam=corres_grp_fam(grp,i);
                if (numfam!=non_affecte)
                  for (int el=0; el<nb_elem; el++)
                    if (num_famille_elem[el]==numfam)
                      {
                        prov[el]=1;
                        has_one++;
                      }
              }
            const Nom& toto=list_group(grp);
            if (has_one>0)
              {
                Nom file_ssz(nom_dom_trio);
                file_ssz+="_";
                file_ssz+=toto;
                file_ssz+=Nom(".file");
                jdd<<"export Sous_Zone "<<toto<<finl;;
                jdd<<"Associer "<<toto <<" "<<nom_dom_trio<<finl;
                jdd<<"Lire "<<toto <<" { "<<finl<<"fichier "<<file_ssz<<" \n }"<<finl;
                jdd_old<<"export Sous_Zone "<<toto<<finl;;
                jdd_old<<"Associer "<<toto <<" "<<nom_dom_trio<<finl;
                jdd_old<<"Lire "<<toto <<" { "<<finl<<"fichier "<<file_ssz<<" \n }"<<finl;
                jdd_par<<"export Sous_Zone "<<toto<<finl;;
                jdd_par<<"Associer "<<toto <<" "<<nom_dom_trio<<finl;
                jdd_par<<"Lire "<<toto <<" { "<<finl<<"fichier "<<toto<<".ssz  \n }"<<finl;
                jdd_par_old<<"export Sous_Zone "<<toto<<finl;;
                jdd_par_old<<"Associer "<<toto <<" "<<nom_dom_trio<<finl;
                jdd_par_old<<"Lire "<<toto <<" { "<<finl<<"fichier "<<toto<<".ssz  \n }"<<finl;
                SFichier f_ssz(file_ssz);
                f_ssz<<has_one<<finl;
                for (int el=0; el<nb_elem; el++)
                  if (prov[el]==1)
                    f_ssz<<el<<" ";
                f_ssz<<finl;
              }
          }
      }


  }
  delete [] num_famille_elem;
  ret = MEDfileClose(fid);
  return ret;
}


int verifier_modifier_type_elem(Nom& type_elem,const IntTab& les_elems,const DoubleTab& sommets)
{

  if ((type_elem=="Rectangle")||(type_elem=="Hexaedre"))
    {
      int dimension=sommets.dimension(1);
      int nb_som_elem=les_elems.dimension(1);
      int ok=1;
      ArrOfDouble pos(2);
      for (int elem=0; ((elem<les_elems.dimension(0))&&(ok)); elem++)
        {
          // pour chaque elt on verifie si il est bien a angle droit
          for (int dir=0; ((dir<dimension)&&(ok)); dir++)
            {
              // on compte le nombre de pos de sommmets dans chaque direction
              int n=0;
              for (int s=0; ((s<nb_som_elem)&&(ok)); s++)
                {

                  double npos=sommets(les_elems(elem,s),dir);
                  int trouve=0;
                  for (int i=0; i<n; i++) if (est_egal(npos,pos(i))) trouve=1;

                  if (trouve==0)
                    {
                      if (n==2)
                        {
                          Cerr<<"There is at least the element "<<elem<<" wich seems to not possess a straight angle"<<finl;
                          ok=0;
                        }

                      else
                        {
                          pos(n)=npos;
                          n++;
                        }
                    }
                }
            }
        }
      //ok=0;
      if (ok==0)
        {
          // on change type_elem
          if (type_elem=="Rectangle")
            type_elem="Quadrangle";
          else if  (type_elem=="Hexaedre")
            type_elem="Hexaedre_vef";

        }
      return 0;
    }
  return 0;
}


void renum_conn(IntTab& les_elems2,Nom& type_elem,int sens);


void recuperer_info_des_joints(Noms& noms_des_joints, const Nom& nom_fic, const Nom& nom_dom, VECT(ArrOfInt)& corres_joint,  ArrOfInt& tab_pe_voisin)
{
  Cerr<<"reading of the joint informations "<<finl;
  int njoint=-1;
  int fid=MEDfileOpen(nom_fic,MED_ACC_RDONLY);
#ifdef MED30
#define MEDnJoint MEDnSubdomainJoint
#endif
  njoint=MEDnJoint(fid,nom_dom);


  corres_joint.dimensionner(njoint);
  noms_des_joints.dimensionner(njoint);
  tab_pe_voisin.resize_array(njoint);


  for (int j=0; j<njoint; j++)
    {
      med_int num_dom;

      Char_ptr maa_dist;
      dimensionne_char_ptr_taille(maa_dist,MED_NAME_SIZE);
      Char_ptr name_of_joint(maa_dist);
      Char_ptr desc;
#ifdef MED30
      dimensionne_char_ptr_taille(desc,MED_COMMENT_SIZE);
      med_int nstep;
      med_int nocstpn;
      MEDsubdomainJointInfo(fid, nom_dom, j+1, name_of_joint, desc,
                            &num_dom, maa_dist, &nstep,&nocstpn);
#else
      dimensionne_char_ptr_taille(desc,MED_COMMENT_SIZE);
      MEDjointInfo(fid, nom_dom, j+1, name_of_joint, desc,
                   &num_dom, maa_dist);
#endif
      Cerr<<" ici "<<name_of_joint<<" "<<(int)num_dom<< " "<<desc<<" "<<maa_dist<<finl;
      tab_pe_voisin[j]=num_dom;
      noms_des_joints[j]=name_of_joint;
      // Lecture de la correspondance sommet/sommet
      med_entity_type typ_ent_local;
      med_geometry_type typ_geo_local;
      med_entity_type typ_ent_distant;
      med_geometry_type typ_geo_distant;

      //       med_int corres=1; // faut il faire une boucle
      //       nb=MEDjointTypeCorres (fid, nom_dom, name_of_joint, corres,
      //                             &typ_ent_local,   &typ_geo_local,
      //                             &typ_ent_distant, &typ_geo_distant);
      //       Cerr<<MED_NODE<<endl;
      //       Cerr<< nb<<" LA " <<typ_ent_local<<" "<<typ_geo_local;
      //       Cerr<<" LA " <<typ_ent_distant<<" "<<typ_geo_distant<<finl;


      typ_geo_local=MED_NONE;
      typ_geo_distant=typ_geo_local;
      typ_ent_local=MED_NODE;
      typ_ent_distant=typ_ent_local;
#ifdef MED30
      med_int nc;
      MEDsubdomainCorrespondenceSize(fid, nom_dom, name_of_joint,MED_NO_DT,MED_NO_IT,typ_ent_local,typ_geo_local,typ_ent_distant, typ_geo_distant,&nc);
#else
      med_int nc= MEDjointnCorres(fid, nom_dom, name_of_joint,typ_ent_local,typ_geo_local,typ_ent_distant, typ_geo_distant);
#endif
      Cerr<<(int)nc <<" connecting vertices " <<finl;
      // lecture de la correspondance
      ArrOfInt& corres_joint_j =corres_joint[j];
      corres_joint_j.resize_array(nc);
      if (nc > 0)
        {
          med_int* cortab;

          cortab=new med_int[nc*2];
#ifdef MED30
          med_int ret =MEDsubdomainCorrespondenceRd(fid,nom_dom,name_of_joint,MED_NO_DT,MED_NO_IT, typ_ent_local,typ_geo_local,typ_ent_distant,typ_geo_distant,cortab) ;
#else
          med_int ret =MEDjointLire(fid,nom_dom,name_of_joint,cortab,nc*2,
                                    typ_ent_local,typ_geo_local,typ_ent_distant,typ_geo_distant) ;
#endif
          if (ret<0)
            {
              Cerr<<"Error when reading the corresponding informations on the vertices"<<endl;
              Process::exit();
            }
          for (int s=0; s<nc; s++)
            {
              corres_joint_j[s]=cortab[2*s]-1; // -1 pour la numerotation C
            }
          // Cerr<<" sommets du joint "<<corres_joint_j<<finl;
          delete [] cortab;
        }
      else
        {
          Cerr<<" not vertex contained in the joint will leads to trouble" <<finl;
          Process::exit();
        }
    }
  MEDfileClose(fid);
  Cerr<<"End of the reading of the joint informations "<<finl;
}

void LireMED::lire_geom( Nom& nom_fic,Domaine& dom,const Nom& nom_dom,const Nom& nom_dom_trio,int isvef, int isfamilyshort)
{

  VECT(ArrOfInt) sommets_joints;
  ArrOfInt tab_pe_voisin;

  //dom.nommer(nom_dom);
  Nom type_elem;
  Noms type_face;
  Noms noms_bords;
  DoubleTab sommets2;
  VECT(ArrOfInt) familles;
  ArrOfInt Indices_bord;
  VECT(IntTab) all_faces_bord;
  IntTab les_elems2;
  int dim=dimension;
  Cerr<<"Try to read the domain "<<nom_dom<<" into the file "<<nom_fic<< " in order to affect to domain "<< nom_dom_trio << "..." <<finl;
  Elem_geom type_ele;
  int ret=medliregeom(nom_fic,nom_dom,nom_dom_trio,dim,sommets2,type_elem,type_ele,les_elems2,type_face,all_faces_bord,familles,noms_bords,Indices_bord,isvef,isfamilyshort);
  assert(ret==0);
  /*
    if (dim!=dimension)
    {
    Cerr<<"Error when reading the geometry dim!=dimension"<<finl;
    //exit();
    }
  */
  // Cerr<<"domaine de nom "<<nom_dom_trio<<" dimension "<<dim<<finl;


  // pour verif
  if (dimension==0)
    {
      Cerr << "Dimension is not defined. Check your data file." << finl;
      Process::exit();
    }


  Cerr << "nom_fic = " << nom_fic<< finl
       << "nom_dom ="<<nom_dom<<finl
       << "dimension = " << dimension<< finl
       /*
         << "sommets = " << sommets2<< finl
         << "type_elem = " << type_elem<< finl
         << "les_elems = " << les_elems2<< finl
         << "type_face = " << type_face<< finl
         << "all_faces_bord = " << all_faces_bord<< finl
         << "familles = " << familles << finl
       */
       << "noms_bords= " << noms_bords<< finl;
  /*
    SFichier es(nom_fic+".es2");
    es<< "nom_fic = " << nom_fic<< finl
    <<"nom_dom ="<<nom_dom<<finl
    << "dimension = " << dimension<< finl
    << "sommets = " << sommets2<< finl
    << "type_elem = " << type_elem<< finl
    << "les_elems = " << les_elems2<< finl
    << "type_face = " << type_face<< finl
    << "all_faces_bord = " << all_faces_bord<< finl
    << "familles = " << familles << finl
    << "noms_bords= " << noms_bords<< finl;
  */
  renum_conn(les_elems2,type_elem,-1);
  for (int j=0; j<type_face.size(); j++)
    renum_conn(all_faces_bord[j],type_face[j],-1);
  if (ret!=0)
    {
      Cerr<<"Problem when reading the informtions"<<finl;
      //exit();
    }

  Scatter::uninit_sequential_domain(dom);

  DoubleTab& sommets=dom.les_sommets();
  // affectation des sommets
  if (dim!=dimension)
    {
      Cerr<<"One tries to read a meshing written in "<<dim<<"D in "<<dimension<<"D "<<finl;
      // determination de la direction inutile
      int nbsom=sommets2.dimension(0);
      int dirinut=-1;
      for (int dir=0; dir<dim; dir++)
        {
          int trouve=1;
          double val1=sommets2(0,dir);
          for (int i=0; i<nbsom; i++)
            if (val1!=sommets2(i,dir))
              {
                trouve=0;
                Cerr<<val1 << " "<<sommets2(i,dir)<<finl;
                break;
              }
          if (trouve==1)
            {
              dirinut=dir;
              break;
            }
        }
      if (dirinut==-1)
        {
          Cerr<<"No useless direction "<<finl;
          exit();
        }
      Cerr<<"useless direction "<<dirinut<<finl<<finl;

      sommets.resize(nbsom,dimension);
      int d=0;
      for (int dir=0; dir<dim; dir++)
        if (dir!=dirinut)
          {
            for (int i=0; i<nbsom; i++)
              sommets(i,d)=sommets2(i,dir);
            d++;
          }


    }
  else
    sommets=sommets2;

  //Cerr<<"ici "<<sommets.dimension(0)<<finl;
  if (dom.nb_zones()==0)
    {
      Zone zonebidon;
      //Cerr<<"dom.nb_zone" << dom.nb_zones()<<finl;
      dom.add(zonebidon);
    }
  //
  Zone& zone=dom.zone(0);
  zone.nommer("PAS_NOMME");
  // typage des elts de  la zone et remplissage des elts
  // Avant de typer on regarde si il ne faut pas transormer les hexa en Hexa_vef
  int err=verifier_modifier_type_elem(type_elem,les_elems2,sommets);
  if (err!=0) exit();

  zone.type_elem()=type_ele;
  // si on a modifier_type_elem
  if (type_ele.valeur().que_suis_je()!=type_elem)
    zone.typer(type_elem);
  zone.type_elem().associer_zone(zone);
  zone.associer_domaine(dom);
  IntTab& les_elems=zone.les_elems();
  // on revient a une numerotation c
  //les_elems2-=1;
  les_elems=les_elems2;
  int nbord=noms_bords.size();

  // on revient a une numerotation c
  //all_faces_bord-=1;
  // on remet dans le meme sens ne sert a rien mais permet de debugger
  // EcrMed LireMed
  ArrOfInt cor2(Indices_bord);
  {
    ArrOfInt cor(Indices_bord);

    for (int jj=0; jj<nbord; jj++) cor2[jj]=jj;
    for (int kk=0; kk<nbord; kk++)
      {
        int imin=cor[kk];
        int pos=kk;
        for (int ll=kk+1; ll<nbord; ll++)
          {
            if (cor[ll]<imin)
              {
                pos=ll;
                imin=cor[ll];
              }
          }
        cor[pos]=cor[kk];
        cor[kk]=imin;
        // Cerr<<"pos "<<pos <<" kk "<< kk<<" cor2 "<<cor2<<finl;
        int pp=cor2[kk];
        cor2[kk]=cor2[pos];
        cor2[pos]=pp;
        //Cerr<<"Apres pos "<<pos <<" kk "<< kk<<" cor2 "<<cor2<<finl;
      }
    //Cerr<<"cor2"<<cor2<<finl<<"cor "<<cor<<"Indices"<<Indices_bord<<finl;
  }
  ArrOfInt nb_t(nbord);
  Bords& faces_bord=zone.faces_bord();
  faces_bord.vide();
  Raccords& faces_raccord=zone.faces_raccord();
  faces_raccord.vide();
  Joints& faces_joint=zone.faces_joint();
  faces_joint.vide();

  Noms noms_des_joints;
  recuperer_info_des_joints(noms_des_joints,nom_fic,nom_dom,sommets_joints,tab_pe_voisin);

  //  Cerr<<" FACES BORD VIDE ?"<< faces_bord<<finl;
  //    for (int ib2=0;ib2<nbord;ib2++)
  for (int typef=0; typef<type_face.size(); typef++)
    if (type_face[typef]!=Nom())
      for (int ib2=nbord-1; ib2>-1; ib2--)
        {
          IntTab& all_faces_bord_=all_faces_bord[typef];
          int nfacebord=all_faces_bord_.dimension(0);
          int ib=cor2[ib2];
          int indice_bord=Indices_bord[ib];
          if (indice_bord>=10000) continue;
          if (noms_bords[ib]=="elems") continue;
          Bord bordprov_;
          Raccord raccprov;
          Joint jointprov;
          int israccord=0;
          int isjoint=0;
          if (noms_bords[ib].debute_par("type_raccord_"))
            {
              israccord=1;
              noms_bords[ib].suffix("type_raccord_");
              raccprov.typer("Raccord_local_homogene");
            }

          int numero_joint=noms_des_joints.search(noms_bords[ib]);
          if (numero_joint>-1)
            {
              Cerr<<noms_bords[ib]<<" is considered as a joint "<<finl;
              isjoint=1;
            }
          //modif_nom_bord(noms_bords[ib]);
          // on recupere la frontiere  .... que ce soit un Bord,Raccord,ou Joint
          Frontiere& bordprov=(isjoint?jointprov:(israccord?ref_cast(Frontiere,raccprov.valeur()):ref_cast(Frontiere,bordprov_)));

          bordprov.nommer(noms_bords[ib]);
          bordprov.typer_faces(type_face[typef]);
          int nb=0;
          int nsomfa=all_faces_bord_.dimension(1);
          IntTab sommprov(nfacebord,nsomfa);
          for (int j=0; j<nfacebord; j++)
            {
              if (familles[typef](j)==indice_bord)
                {

                  for (int k=0; k<nsomfa; k++)
                    sommprov(nb,k)=all_faces_bord_(j,k);
                  nb++;
                }
            }
          Faces& facesi=bordprov.faces();
          IntTab& sommetsfaces=facesi.les_sommets();
          sommetsfaces.resize(nb,nsomfa);
          for (int jj=0; jj<nb; jj++)
            for (int k=0; k<nsomfa; k++)
              sommetsfaces(jj,k)=sommprov(jj,k);
          IntTab& facesv=facesi.voisins();
          facesv.resize(nb,2);
          facesv=-1;

          //if ((nb!=0)&&(indice_bord!=0))
          int vide_0D_a_ecrire=0;
          if (typef==type_face.size()-1)
            {
              if ((nb_t[ib]==0)&&(nb==0))
                {
                  bordprov.typer_faces("vide_0D");
                  vide_0D_a_ecrire=1;
                }
            }
          else if (nb>nb_t[ib]) nb_t[ib]=nb;
          // ne marche pas ajoute la famille elem
          if ((indice_bord!=0)&&((nb>0)||(vide_0D_a_ecrire)))
            {
              if (isjoint)
                {
                  int PE_voisin=tab_pe_voisin[numero_joint];
                  int epaisseur=1;
                  ArrOfInt& sommets_joint=sommets_joints[numero_joint];
                  jointprov.affecte_PEvoisin(PE_voisin);
                  jointprov.affecte_epaisseur(epaisseur);
                  ArrOfInt& sommets_du_joint=jointprov.set_joint_item(Joint::SOMMET).set_items_communs();
                  sommets_du_joint=sommets_joint;
                  faces_joint.add(jointprov);
                }
              else if (israccord)
                faces_raccord.add(raccprov);
              else
                faces_bord.add(bordprov_);
            }
        }
  for (int j=0; j<nbord; j++)
    {
      if (Indices_bord(j)==-1000)
        {
          // Cerr<<" j "<<j <<noms_bords[j]<<finl;
          Nom nom_zone=noms_bords[j];
          if (nom_zone.debute_par("cpy_"))
            nom_zone.suffix("cpy_");
          Cerr<<" the zone is named "<<nom_zone<<finl;
          zone.nommer(nom_zone);
        }
    }
  faces_bord.associer_zone(zone);
  faces_raccord.associer_zone(zone);
  faces_joint.associer_zone(zone);
  zone.type_elem().associer_zone(zone);
  zone.fixer_premieres_faces_frontiere();
  int nbfr=zone.nb_front_Cl();
  int fr;
  for ( fr=0; fr<nbfr; fr++)
    {
      //
      zone.frontiere(fr).faces().associer_zone(zone);
      if ( zone.frontiere(fr).faces().type_face()!=vide_0D)
        zone.frontiere(fr).faces().reordonner();
    }
  //  GF au moins en polyedre il faut  reordonner
  //  il faut certainement le faire tout le temps
  //  non c'est trop long
  if (sub_type(Polyedre,zone.type_elem().valeur())|| (type_elem=="Rectangle")|| (type_elem=="Hexaedre"))
    dom.reordonner();


  if (nproc()==1)
    NettoieNoeuds::nettoie(dom);

  // +1 car med numero de 1 a n
  // medecrgeom(nom_fic,nom_dom,dimension,sommets,type_elem,les_elems2,type_face,all_faces_bord,familles,noms_bords);
  Scatter::init_sequential_domain(dom);
  if (isfamilyshort==2)
    {
      // il est possible que l'on est cree plusieurs bord de meme nom
      RegroupeBord r;
      r.rassemble_bords(dom);
    }
  Cerr<<"Reading of the domain ended"<<finl;
  // return is;
}

//void medinfo1champ(const Nom& nom_fic, Nom& nomchamp,int& numero,int& nbcomp,int& ndt,med_entity_type& type_ent, med_geometry_type& type_geo,int& size,const Nom& nom_dom,int verifie_type,ArrOfDouble& temps_sauv);

med_geometry_type type_geo_trio_to_type_med(const Nom& a);
void medinfochamp_existe(const Nom& nom_fic,Noms& nomschamp,const Domaine& dom,ArrOfDouble& temps_sauv)
{
  int fid=MEDfileOpen(nom_fic,MED_ACC_RDONLY);
  if (fid<0)
    {
      Cerr <<"Problem when opening "<<nom_fic<<finl;
      Process::exit();
    }
  int ret=0;
  // on regarde si le champ existe
  int nbch=MEDnField(fid);
  if (nbch<0)
    {
      Cerr<<"Error when reading fileds number"<<finl;
      Process::exit();
    }
  //  int trouve=0;
  Char_ptr nomcha;
  dimensionne_char_ptr_taille(nomcha,MED_NAME_SIZE);

  Char_ptr comp, unit;
  nomschamp.dimensionner(1);
  int nbch_reel=0;
  for (int ch=0; ch<nbch; ch++)
    {
      //printf("\nChamp numero : %d \n",i+1);
      int ncomp=0;
      /* combien de composantes */
      if (ret == 0)
        if ((ncomp = MEDfieldnComponent(fid,ch+1)) < 0)
          ret = -1;
      //printf("%d\n",ret);


      /* allocation memoire de comp et unit*/
      if (ret == 0)
        {
          dimensionne_char_ptr_taille(comp,MED_SNAME_SIZE,ncomp);
          dimensionne_char_ptr_taille(unit,MED_SNAME_SIZE,ncomp);

          /* infos sur les champs */
#ifdef MED30
          Char_ptr meshname;
          dimensionne_char_ptr_taille(meshname,MED_NAME_SIZE);
          Char_ptr dtunit;
          dimensionne_char_ptr_taille(dtunit,MED_SNAME_SIZE);
          med_bool localmesh;
          med_field_type fieldtype;
          med_int nbofcstp;
          ret=MEDfieldInfo(fid,ch+1,nomcha,meshname,&localmesh,&fieldtype,comp,unit,dtunit,&nbofcstp);
          if (dom.le_nom()==(const char*)meshname)
#else
          med_type_champ typcha;
          ret = MEDchampInfo(fid,ch+1,nomcha,&typcha,comp,unit,ncomp);
#endif
            // il faut verifier si nom_dom est correct pour le champ
            {
              med_geometry_type type_geo=type_geo_trio_to_type_med(dom.zone(0).type_elem()->que_suis_je());
              int numero_ch,nb_dt,nbcomp,size;
              med_entity_type type_ent;
              ArrOfDouble temps_sauv2;
              medinfo1champ(nom_fic,nomcha,numero_ch,nbcomp,nb_dt,type_ent,type_geo,size,dom.le_nom(),0,temps_sauv2);

              if (nb_dt!=0)
                {
                  temps_sauv=temps_sauv2;
                  nomschamp.add(nomcha.getChar());
                  nbch_reel++;
                  type_ent=MED_CELL;
                  medinfo1champ(nom_fic,nomcha,numero_ch,nbcomp,nb_dt,type_ent,type_geo,size,dom.le_nom(),1,temps_sauv2);
                  if (nb_dt!=0)
                    {
                      Cerr<<"the field "<<nomcha<<" is discretized at the elements"<<finl;
                      nomschamp.add(nomcha.getChar());
                      nbch_reel++;
                      nomschamp[nbch_reel]+="_elem";
                      Cerr<<"temps_sauv"<<temps_sauv2;
                    }
                  type_ent=MED_NODE;
                  medinfo1champ(nom_fic,nomcha,numero_ch,nbcomp,nb_dt,type_ent,type_geo,size,dom.le_nom(),1,temps_sauv2);
                  if (nb_dt!=0)
                    {
                      Cerr<<"the field "<<nomcha<<" is discretized at the vertices"<<finl;
                      nomschamp.add(nomcha.getChar());
                      nbch_reel++;
                      nomschamp[nbch_reel]+="_som";
                      Cerr<<"temps_sauv"<<temps_sauv2;
                    }
                }

            }

          //Cerr<<"Nom du champ "<<nomcha<<" de type "<<typcha<<finl;
          //Cerr<<"Nom des composantes "<<comp<<finl;
          //Cerr<<"Unites des composantes "<<unit<<finl
        }
    }
  MEDfileClose(fid);
}
Nom medinfo1champ(const Nom& nom_fic, const char* nomchamp_utilisateur,int& numero,int& nbcomp,int& ndt,med_entity_type& type_ent, med_geometry_type& type_geo,int& size,const Nom& nom_dom,int verifie_type,ArrOfDouble& temps_sauv)
{
  Nom nomchamp(nomchamp_utilisateur);
  int fid=MEDfileOpen(nom_fic,MED_ACC_RDONLY);
  if (fid<0)
    {
      Cerr <<"Problem when opening "<<nom_fic<<finl;
      Process::exit();
    }

  int ret=0;
  // on regarde si le champ existe
  int nbch=MEDnField(fid);
  if (nbch<0)
    {
      Cerr<<"Error when reading the fields number"<<finl;
      Process::exit();
    }
  int trouve=0;
  Char_ptr nomcha;
  dimensionne_char_ptr_taille(nomcha,MED_NAME_SIZE);

  Char_ptr comp, unit;
  //  nomschamp.dimensionner(nbch);
  Nom noms_champ_file("");
  for (int ch=0; ch<nbch; ch++)
    {
      //printf("\nChamp numero : %d \n",i+1);
      int ncomp=0;
      /* combien de composantes */
      if (ret == 0)
        if ((ncomp = MEDfieldnComponent(fid,ch+1)) < 0)
          ret = -1;
      //printf("%d\n",ret);


      /* allocation memoire de comp et unit*/
      if (ret == 0)
        {
          dimensionne_char_ptr_taille(comp,MED_SNAME_SIZE,ncomp);
          dimensionne_char_ptr_taille(unit,MED_SNAME_SIZE,ncomp);

          /* infos sur les champs */
#ifdef MED30
          med_field_type typcha;
          Char_ptr meshname;
          dimensionne_char_ptr_taille(meshname,MED_NAME_SIZE);
          Char_ptr dtunit;
          dimensionne_char_ptr_taille(dtunit,MED_SNAME_SIZE);
          med_bool localmesh;
          //        med_field_type fieldtype;
          med_int nbofcstp;
          ret=MEDfieldInfo(fid,ch+1,nomcha,meshname,&localmesh,&typcha,comp,unit,dtunit,&nbofcstp);
          // Cerr << nomcha<<" "<<nom_dom<<" "<<meshname <<finl;
          if (nom_dom!=(const char*)meshname)
            {
              nomcha[0]='#';
            }

          ndt=nbofcstp;
#else
          med_type_champ typcha;
          ret = MEDchampInfo(fid,ch+1,nomcha,&typcha,comp,unit,ncomp);
#endif
          Nom Nnomcha(nomcha);
          //nomschamp[ch]=nomcha;
          //Cerr<<"Nom du champ "<<nomcha<<" de type "<<typcha<<finl;
          //Cerr<<"Nom des composantes "<<comp<<finl;
          //Cerr<<"Unites des composantes "<<unit<<finl;
          // Cerr << Motcle(nomchamp) << " " << Nnomcha << finl;
          noms_champ_file+=Nnomcha+" ";

          Motcle mot2(nomchamp);
          mot2+="_";
          mot2+=nom_dom;
          Motcle mot3(nomchamp);
          if (verifie_type==1)
            {
              if (type_ent==MED_CELL)
                mot3+="_elem_";
              else
                mot3+="_som_";
              mot3+=nom_dom;
            }


          /*

          noms_champ_file+=mot2+" ";
          */
          if ((trouve==0)&&((Motcle(nomchamp)==Nnomcha) || (mot2==Nnomcha)||(mot3==Nnomcha)  ))
            {
              trouve=1;
              nomchamp=Nnomcha;
              numero=ch;
              nbcomp=ncomp;
              //  Cerr<<"On a deja ce champ"<<finl;
              if (typcha!=MED_FLOAT64)
                {
                  Cerr<<"field of type different from MED_FLOAT64 not coded" <<finl;
                  Process::exit();
                }
              break;
            }
        }
    }
  if (trouve==0)
    {
      Cerr<<nomchamp << " not found on "<<nom_dom<<finl;
      Cerr<< "Fields in file : "<<  noms_champ_file<<finl;
      Process::exit();
    }
#ifdef MED30
  if (verifie_type==0)
    {
      type_ent=MED_CELL;
      med_int numdt,numo;
      double dtb;
      MEDfieldComputingStepInfo(fid,nomcha,1,&numdt,&numo,&dtb);

      if (MEDfieldnValue(fid,nomchamp,numdt,numo,type_ent,type_geo)<=0)
        {
          type_ent=MED_NODE;
          // type_geo=MED_NONE;
          if (MEDfieldnValue(fid,nomchamp,numdt,numo,type_ent,type_geo)<=0)
            abort();
        }
    }
#else
  if (verifie_type==0)
    type_ent=MED_CELL;

  ndt=MEDnPasdetemps(fid,nomchamp,type_ent,type_geo);
#endif
  // on fait une boucle pour le cas ou temperature aux som pour un dom
  // et aux elems pour l'autre
  int fini=0;
  med_int numo;
  int numdtsa=-1,marq=0;
  while (fini==0)
    {
      if ((ndt==0)&&(verifie_type==0))
        {
#ifndef MED30
          type_ent=MED_NODE;
          ndt=MEDnPasdetemps(fid,nomchamp,type_ent,type_geo);
#endif
          Cerr<<"No time found for "<<nomchamp<<" on type_ent " << (int)type_ent<<finl;
        }
      temps_sauv.resize_array(ndt);
      med_int numdt;
      double dt;
      Char_ptr dt_unit;
      dimensionne_char_ptr_taille(dt_unit,MED_SNAME_SIZE);
      strcpy(dt_unit," C      ");
      Char_ptr nom_dom_med;
      dimensionne_char_ptr_taille(nom_dom_med,MED_NAME_SIZE);
      //strcpy(nom_dom_med,nom_dom);
      // on regarde si tous les pas de temps sont sur le bon support
      int ndt2=0;
      for (int i=0; i<ndt; i++)
        {
#ifdef MED30
          med_int meshnumdt,meshnumit;
          MEDfieldComputingStepMeshInfo(fid,nomchamp,i+1,&numdt,&numo,&dt,&meshnumdt,&meshnumit);
          size=MEDfieldnValue(fid,nomchamp,numdt,numo,type_ent,type_geo);
          // Cerr<<"ici "<<size<<finl;
          if (size)
#else
          med_int ngauss;
          MEDpasdetempsInfo(fid,nomchamp,type_ent,type_geo,
                            i+1, nom_dom_med, &ngauss, &numdt,
                            dt_unit, &dt, &numo);

          if (nom_dom==(const char*)nom_dom_med)
#endif
            {
              temps_sauv[ndt2]=dt;
              ndt2++;
              numdtsa=numdt;
            }
        }
      //Cerr<<"ici "<<numdt;
      fini=1;
      if (ndt!=ndt2)
        {
          Cerr<<" Only "<<ndt2<<" times available for "<<nomchamp <<" on "<<nom_dom<<" on MED type "<<(int)type_ent<<finl;
          ndt=ndt2;
          if (verifie_type==0)
            if (ndt==0)
              {
                fini=0;
                ndt=0;
                if (marq==1) fini=1;
                else marq=1;

              }
        }
    }
  if (ndt>0)
    {
#ifdef MED30
      size=MEDfieldnValue(fid,nomchamp,numdtsa,numo,type_ent,type_geo);
#else
      size=MEDnVal(fid,nomchamp,type_ent,type_geo,numdtsa,numo,nom_dom,MED_COMPACT);
#endif
      if (size<0)
        {
          Cerr<<"Problem when reading the size for the field "<<nomchamp<<finl;
          Process::exit();
        }
    }
  temps_sauv.resize_array(ndt);
  MEDfileClose(fid);
  return nomchamp;
}

#endif
void traite_nom_fichier_med(Nom& nom_fic)
{
  Nom nom_fic2(nom_fic);
  nom_fic2.prefix(".med");
  if (nom_fic2==nom_fic)
    {
      Cerr<<"Error : the med field must have a .med extension"<<finl;
      Cerr<<nom_fic2 <<" "<<nom_fic<<finl;
      Process::exit();
    }

  nom_fic2.prefix("_0000");
  nom_fic2+=".med";
  nom_fic=nom_fic2.nom_me(nom_fic.me());
  // on essaye en premier les fichiers _0000.med
  {
    ifstream test(nom_fic);
    if (!test)
      {
        nom_fic=nom_fic2;
      }
  }
  {
    ifstream test(nom_fic);
    if (!test)
      {
        Cerr<<"med file "<<nom_fic<<" not found."<<finl;
        Process::exit();
      }
  }
  //  Cerr<<"File med read: "<<nom_fic<<finl;
}

// Description:  Permet de lire le nom d'un champ particulier
//    exemple "toto toto" ou "toto\ toto"
//    si le premier mot commence par " on concatene les mots jusqu'au dernier. Si on veut 2 espaces on est oblige de mettre "a\ \ a"
void lire_nom_med(Nom& nom_champ, Entree& s)
{

  s>>nom_champ;
  if (nom_champ[0]=='"')
    {
      nom_champ.suffix("\"");
      nom_champ.prefix("\\");
      Nom  suite;
      while(nom_champ[nom_champ.longueur()-2]!='"')
        {
          //s.get(suite,1);
          s >> suite;
          nom_champ+=" ";
          nom_champ+=suite;
          nom_champ.prefix("\\");
          Cerr<<nom_champ<<finl;
          //Cerr<<nom_champ[nom_champ.longueur()-2]<<'"'<<finl;
          //exit();
        }
      // nom_champ="champ vectoriel  1";
      Cerr<<nom_champ<<finl;
      nom_champ.prefix("\"");
      Cerr<<nom_champ<<finl;
    }
}

