/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        EcrMED.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/38
//
//////////////////////////////////////////////////////////////////////////////

#include <EcrMED.h>
#include <Domaine.h>
#include <med++.h>
#include <Zone_VF.h>
#include <Champ_Inc_base.h>
#include <Vect_IntTab.h>
#include <LireMED.h>
#include <Polyedre.h>
#include <Polygone.h>
#include <Char_ptr.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDFileMesh.hxx>
#include <MEDCouplingFieldDouble.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
using namespace MEDCoupling;
#endif

#define POURSATURNE

Implemente_instanciable_sans_constructeur(EcrMED,"Ecrire_MED",Interprete);

EcrMED::EcrMED() : major_mode(false)
{
#ifdef MEDCOUPLING_
  use_medcoupling_ = true;
#else
  use_medcoupling_ = false;
#endif
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
Sortie& EcrMED::printOn(Sortie& os) const
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
Entree& EcrMED::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& EcrMED::interpreter(Entree& is)
{
  Cerr<<"syntax : EcrMED [ append ] nom_dom nom_fic "<<finl;
  int mode=-1;
  Nom nom_dom, nom_fic;
  is >> nom_dom ;
  Motcle app("append");
  if (app==nom_dom)
    {
      mode=0;
      is >> nom_dom;
      Cerr<<" Adding "<<nom_dom<<finl;
    }
  is >> nom_fic;
  if(! sub_type(Domaine, objet(nom_dom)))
    {
      Cerr << nom_dom << " type is " << objet(nom_dom).que_suis_je() << finl;
      Cerr << "Only Domaine type objects can be meshed" << finl;
      exit();
    }
  const Domaine& dom=ref_cast(Domaine, objet(nom_dom));
  ecrire_domaine(nom_fic,dom,nom_dom,mode);
  return is;
}

#ifndef MED_
void EcrMED::ecrire_champ(const Nom& type,const Nom& nom_fic,const Nom& nom_dom,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,int compteur)
{
  med_non_installe();
}
void EcrMED::ecrire_domaine(const Nom& nom_fic,const Domaine& dom,const Nom& nom_dom,int mode)
{
  med_non_installe();
}
#else

med_idt trustMEDfileOpen(const char* const filename, const med_access_mode accessmode, bool major_mode)
{
  if (major_mode)
    return MEDfileVersionOpen(filename, accessmode, MED_NUM_MAJEUR,0,0);
  return MEDfileOpen(filename, accessmode );
}

med_int* convert_int_med_int(const ArrOfInt& tab)
{
  med_int* tabmed;
  if (sizeof(med_int)==sizeof(int))
    tabmed=(med_int *)tab.addr();
  else
    {

      int taille=tab.size_array();
      //  Cerr<<sizeof(tab)<<" "<<sizeof(int)<<" "<<sizeof(tab)/sizeof(int)<<" "<<taille<<finl;
      Cerr<<"creation of medint*"<<finl;
      tabmed=new med_int[taille];
      for (int i=0; i<taille; i++)
        tabmed[i]=tab[i];
      //     exit();
    }
  return tabmed;
}
#ifdef MEDCOUPLING_
// renvoie le type medcoupling a partir du type trio : http://docs.salome-platform.org/6/gui/MED/MEDLoader_8cxx.html
INTERP_KERNEL::NormalizedCellType type_geo_trio_to_type_medcoupling(const Nom& type_elem_, int& mesh_dimension)
{
  Motcle type_elem;
  type_elem=type_elem_;
  type_elem.prefix("_AXI");
  if (type_elem!=Motcle(type_elem_))
    {
      if (type_elem == "QUADRILATERE_2D")
        type_elem = "SEGMENT_2D";
      if (type_elem == "RECTANGLE_2D")
        {
          type_elem = "RECTANGLE";
        }
    }
  mesh_dimension = -1;
  INTERP_KERNEL::NormalizedCellType type_cell;
  if ((type_elem=="RECTANGLE") || (type_elem=="QUADRANGLE") || (type_elem=="QUADRANGLE_3D"))
    {
      type_cell = INTERP_KERNEL::NORM_QUAD4;
      mesh_dimension = 2;
    }
  else if  ((type_elem=="HEXAEDRE") || (type_elem=="HEXAEDRE_VEF"))
    {
      type_cell = INTERP_KERNEL::NORM_HEXA8;
      mesh_dimension = 3;
    }
  else if  ((type_elem=="TRIANGLE") || (type_elem=="TRIANGLE_3D"))
    {
      type_cell = INTERP_KERNEL::NORM_TRI3;
      mesh_dimension = 2;
    }
  else if  (type_elem=="TETRAEDRE")
    {
      type_cell = INTERP_KERNEL::NORM_TETRA4;
      mesh_dimension = 3;
    }
  else if ((type_elem=="SEGMENT") || (type_elem=="SEGMENT_2D"))
    {
      type_cell = INTERP_KERNEL::NORM_SEG2;
      mesh_dimension = 1;
    }
  else if (type_elem=="PRISME")
    {
      type_cell = INTERP_KERNEL::NORM_PENTA6;
      mesh_dimension = 3;
    }
  else if (type_elem=="POLYEDRE")
    {
      type_cell = INTERP_KERNEL::NORM_POLYHED;
      mesh_dimension = 3;
    }
  else if ((type_elem=="POLYGONE") || (type_elem=="POLYGONE_3D"))
    {
      type_cell = INTERP_KERNEL::NORM_POLYGON;
      mesh_dimension = 2;
    }
  else if(type_elem=="PRISME_HEXAG")
    {
      type_cell = INTERP_KERNEL::NORM_HEXGP12;
      mesh_dimension = 3;
    }
  else if(type_elem=="POINT_1D")
    {
      type_cell = INTERP_KERNEL::NORM_POINT1;
      mesh_dimension = 0;
    }
  else
    {
      Cerr<<type_elem<< " no available cell." <<finl;
      Process::exit();
      return INTERP_KERNEL::NORM_POINT1;
    }
  assert(mesh_dimension>=0);
  return type_cell;
}
#endif

// renvoit le type med a partir du type trio
med_geometry_type type_geo_trio_to_type_med(const Nom& type_elem_,med_axis_type& rep)
{
  rep=MED_CARTESIAN;
  Motcle type_elem;
  type_elem=type_elem_;
  type_elem.prefix("_AXI");
  if (type_elem!=Motcle(type_elem_))
    {
      rep=MED_SPHERICAL;
      Cerr<<"#"<<type_elem<<"#"<<Motcle(type_elem_)<<"#"<<(type_elem!=Motcle(type_elem_))<<finl;
      if (type_elem=="QUADRILATERE_2D")
        type_elem="SEGMENT_2D";
      if (type_elem=="RECTANGLE_2D")
        {
          type_elem="RECTANGLE";
          rep=MED_CYLINDRICAL;
        }
    }
  med_geometry_type type_elem_med;
  if ((type_elem=="RECTANGLE") || (type_elem=="QUADRANGLE"))
    type_elem_med=MED_QUAD4;
  else if  ((type_elem=="HEXAEDRE")|| (type_elem=="HEXAEDRE_VEF"))
    type_elem_med=MED_HEXA8;
  else if  (type_elem=="TRIANGLE")
    type_elem_med=MED_TRIA3;
  else if  (type_elem=="TETRAEDRE")
    type_elem_med=MED_TETRA4;
  else if ((type_elem=="SEGMENT_2D") || (type_elem=="SEGMENT"))
    type_elem_med=MED_SEG2;
  else if (type_elem=="TRIANGLE_3D")
    type_elem_med=MED_TRIA3;
  else if (type_elem=="QUADRANGLE_3D")
    type_elem_med=MED_QUAD4;
  else if (type_elem=="PRISME")
    type_elem_med=MED_PENTA6;
  else if (type_elem=="POLYEDRE")
    type_elem_med=MED_POLYHEDRON;
  else if (type_elem=="POLYGONE")
    type_elem_med=MED_POLYGON;
  else if (type_elem=="POLYGONE_3D")
    type_elem_med=MED_POLYGON;
  else if(type_elem=="SEGMENT")
    type_elem_med=MED_SEG2;
#ifdef MED30
  else if(type_elem=="PRISME_HEXAG")
    type_elem_med=MED_OCTA12;
#endif
  else if(type_elem=="POINT_1D")
    type_elem_med=MED_POINT1;
  else
    {
      Cerr<<type_elem<< " no available code" <<finl;
      Process::exit();
      type_elem_med=MED_QUAD4;
    }
  return type_elem_med;
}
med_geometry_type type_geo_trio_to_type_med(const Nom& type_elem)
{
  med_axis_type rep;
  return type_geo_trio_to_type_med(type_elem,rep);
}

// ecriture des faces
int medecrirefaces(IntTab& all_faces_bord,const Nom& type_face,int fid,const Nom& nom_dom,int dimension,const ArrOfInt& familles,med_access_mode mode=MED_ACC_RDEXT)
{
  int ret=0;
  int nface=familles.size_array();
  if (nface)
    {
      med_geometry_type type_elem_med;

      type_elem_med=type_geo_trio_to_type_med(type_face);
      med_entity_type type_ent;

#ifdef POURSATURNE
      type_ent=MED_CELL;
#else
      type_ent=MED_FACE;
      if (dimension==2) type_ent=MED_ARETE;
#endif
      // on passe par des tableaux med quand int != med_int
      if (type_elem_med!=MED_POLYGON)
        {
          med_int *med_all_faces_bord=convert_int_med_int(all_faces_bord);
          med_int *med_familles=convert_int_med_int(familles);
#ifdef MED30
          double dtb=0;
          ret=MEDmeshElementWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,dtb,type_ent,type_elem_med,MED_NODAL,MED_FULL_INTERLACE,nface,med_all_faces_bord,MED_FALSE,NULL,MED_FALSE,NULL,MED_TRUE,med_familles);
#else
          ret=MEDelementsEcr(fid,nom_dom,dimension,med_all_faces_bord,MED_FULL_INTERLACE,NULL,MED_FAUX,NULL,MED_FAUX,med_familles,nface,type_ent,type_elem_med,MED_NODAL);
#endif
          if (sizeof(med_int)!=sizeof(int))
            {
              delete []  med_all_faces_bord;
              delete [] med_familles;
            }

          // ATTENTION MED_ACC_CREAT au lieu de MED_ACC_RDEXT
          if (ret<0)
            {
              Cerr<<"Problem when writing faces in "<<nom_dom<<finl;
              Process::exit();
            }
        }
      else
        {
          ArrOfInt index(nface+1);
          index(0)=1;
          int nb_som_max=all_faces_bord.dimension(1);
          for (int f=0; f<nface; f++)
            {
              int nb_som=nb_som_max-1;
              while (all_faces_bord(f,nb_som)==0) nb_som--;
              index(f+1)=index(f)+nb_som+1;
            }

          ArrOfInt conn(index(nface)-1);
          int c=0;
          for (int f=0; f<nface; f++)
            {
              for (int s=0; s<nb_som_max; s++)
                {
                  int s2=all_faces_bord(f,s);
                  if (s2==0) break;
                  conn(c++)=s2;
                }
            }
          assert(c==(index(nface)-1));
          med_int *med_index=convert_int_med_int(index);
          med_int *med_conn=convert_int_med_int(conn);
          med_int *med_familles=convert_int_med_int(familles);
          med_int med_size_index=index.size_array();
          // ret=MEDelementsEcr(fid,nom_dom,dimension,med_all_faces_bord,MED_FULL_INTERLACE,NULL,MED_FAUX,NULL,MED_FAUX,med_familles,nface,type_ent,type_elem_med,MED_NODAL,mode);
          //Cerr<<" ecriture "<<index<<" conn "<<conn<<" "<<all_faces_bord<<finl;
#ifdef MED30
          ret=MEDmeshPolygonWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_NODAL,med_size_index,med_index,med_conn);
#else
          ret=MEDpolygoneConnEcr(fid,nom_dom,med_index,med_size_index,med_conn,type_ent,MED_NODAL);
#endif
          // ATTENTION MED_ACC_CREAT au lieu de MED_ACC_RDEXT
          if (ret<0)
            {
              Cerr<<"Problem when writing faces in "<<nom_dom<<finl;
              Process::exit();
              // on ecrit les familles avant d'oublier....
            }
#ifdef MED30
          if ((ret = MEDmeshEntityFamilyNumberWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,type_ent,type_elem_med,nface,med_familles)) < 0)
#else
          if ((ret = MEDfamEcr(fid,nom_dom,med_familles,nface,type_ent,type_elem_med)) < 0)
#endif
            Process::exit();
          if (sizeof(med_int)!=sizeof(int))
            {
              delete [] med_index;
              delete [] med_conn;
              delete [] med_familles;
            }
        }
    }
  return ret;
}

void affecte_nom_med(Char_ptr& nom_med,const Nom& mot);
// ecrit la geom appele par EcrMED::ecrire_domaine
int medecrgeom(const Nom& nom_fic,const Nom& nom_dom,int dimension,const DoubleTab& sommets,const Nom& type_elem,const Elem_geom& ele,const IntTab& les_elems,const Noms& type_face,VECT(IntTab)& all_faces_bord,const VECT(ArrOfInt)& familles,Noms& noms_bords,const Nom& nom_zone,int mode, bool major_mode)
{
  // creation d'un mot de longueur MED_NAME_SIZE
  Char_ptr med_taille_nom;
  dimensionne_char_ptr_taille(med_taille_nom,MED_NAME_SIZE);
  //Cerr<<"HERE medecrgeom "<<nom_fic<<" "<<nom_dom<<" "<<mode<<finl;
  int ret=0;
  int fid ;
  // alors ca ou l'autre ????
  // mode =0 on ajoute. mode=-1 on reecrit
  if (mode==0)
    {
      fid= trustMEDfileOpen(nom_fic,MED_ACC_RDEXT, major_mode);
      if (fid<0)
        fid= trustMEDfileOpen(nom_fic,MED_ACC_CREAT, major_mode);
    }
  else
    fid= trustMEDfileOpen(nom_fic,MED_ACC_CREAT, major_mode);

  if (fid < 0)
    {
      Cerr<<"Problem when opening "<<nom_fic<<finl;
      Process::exit();
    }


  Nom nomcoo="x               y               ";
  Nom unicco="m               m               ";
  if (dimension>2)
    {
      nomcoo+="z               ";
      unicco+="m               ";
    }
  med_geometry_type type_elem_med;
  med_axis_type rep;
  type_elem_med=type_geo_trio_to_type_med(type_elem,rep);
#ifdef MED30
  Char_ptr desc;
  desc.allocate(MED_COMMENT_SIZE);
  strcpy(desc,"no description");
  Char_ptr dtunit;
  dtunit.allocate(MED_SNAME_SIZE);
  strcpy(dtunit,"s");
  ret=MEDmeshCr(fid,nom_dom,dimension,dimension,MED_UNSTRUCTURED_MESH,desc,dtunit,MED_SORT_DTIT,rep,nomcoo,unicco );

#else
  ret = MEDmaaCr(fid,nom_dom,dimension);
#endif
  double* sommets2=(double *)sommets.addr();

  int nsommet=sommets.dimension(0);
  if (nsommet==0) return 1;

  // Ecriture des sommets

  {
    //ArrOfInt nufano(nsommet);
    med_int* nufano;
    nufano=new med_int[nsommet];
    for (int i=0; i<nsommet; i++)
      nufano[i]=0;
#ifdef MED30
    double dtb=0;
    ret=MEDmeshNodeWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,dtb,MED_FULL_INTERLACE,nsommet,sommets2,MED_FALSE,NULL,MED_FALSE,NULL,MED_TRUE,nufano);
#else
    ret=MEDnoeudsEcr(fid,nom_dom,dimension,sommets2,MED_FULL_INTERLACE,rep,nomcoo,unicco,NULL,MED_FAUX,NULL,MED_FAUX,nufano,nsommet);
#endif
    if (ret<0)
      {
        Cerr<<"Problem when writing vertex in "<<nom_fic<<finl;
        Process::exit();
      }

    delete []  nufano;
  }
  int famglob=-1000; // ou -1000 ?
  // ecriture des elements internes...
  {
    int nelem=les_elems.dimension(0);
    //ArrOfInt nufano(nelem);
    med_int* nufano;
    nufano=new med_int[nelem];
    for (int i=0; i<nelem; i++)
      nufano[i]=famglob;

    // on passe par des tableaux med quand int != med_int

    med_int* med_les_elems=convert_int_med_int(les_elems);

    int is_poly=0;
    if ((type_elem_med==MED_POLYHEDRON)||(type_elem_med==MED_POLYGON))
      is_poly=1;
    if (is_poly==0)
#ifdef MED30
      {
        double dtb=0;
        ret=MEDmeshElementWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,dtb,MED_CELL,type_elem_med,MED_NODAL,MED_FULL_INTERLACE,nelem,med_les_elems,MED_FALSE,NULL,MED_FALSE,NULL,MED_TRUE,nufano);
      }
#else
      ret=MEDelementsEcr(fid,nom_dom,dimension,med_les_elems,MED_FULL_INTERLACE,NULL,MED_FAUX,NULL,MED_FAUX,nufano,nelem,MED_CELL,type_elem_med,MED_NODAL);
#endif
    else
      {
        ArrOfInt Nodes_glob;
        ArrOfInt FacesIndex;
        ArrOfInt PolyhedronIndex;

        if (type_elem_med==MED_POLYHEDRON)
          {
            const Polyedre& Poly=ref_cast(Polyedre,ele.valeur());
            Poly.remplir_Nodes_glob(Nodes_glob,les_elems);
            FacesIndex=(Poly.getFacesIndex());
            PolyhedronIndex=(Poly.getPolyhedronIndex());
          }
        else
          {
            const Polygone& Poly=ref_cast(Polygone,ele.valeur());
            //Poly.remplir_Nodes_glob(Nodes_glob,les_elems);
            FacesIndex=(Poly.getFacesIndex());
            PolyhedronIndex=(Poly.getPolygonIndex());
          }

        //        pas necessaire on a rempli avec elem qui a le +1 Nodes_glob+=1;
        FacesIndex+=1;
        PolyhedronIndex+=1;
        med_int* med_Nodes=convert_int_med_int(Nodes_glob);
        med_int* med_FacesIndex=convert_int_med_int(FacesIndex);
        med_int* med_PolyhedronIndex=convert_int_med_int(PolyhedronIndex);
        int size_index=PolyhedronIndex.size_array();
        int size_f=FacesIndex.size_array();
#ifdef MED30
        if (type_elem_med==MED_POLYHEDRON)
          {
            ret=MEDmeshPolyhedronWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,0.,MED_CELL,MED_NODAL,size_index,med_PolyhedronIndex,size_f,med_FacesIndex,med_Nodes);
          }
        else
          {
            ret=MEDmeshPolygonWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_UNDEF_DT,MED_CELL,MED_NODAL,size_index,med_PolyhedronIndex,med_FacesIndex);
          }
#else
        ret=MEDpolyedreConnEcr(fid,nom_dom,
                               med_PolyhedronIndex,
                               size_index,
                               med_FacesIndex,
                               size_f,
                               med_Nodes,
                               MED_NODAL);
        if (type_geo!=MED_POLYGON)
          abort();
#endif
        if (ret<0)
          {
            Cerr<<" Problem MEDpolyedreConnEcr"<<finl;
            Process::exit();
          }
        if (sizeof(med_int)!=sizeof(int))
          {
            delete [] med_Nodes;
            delete [] med_FacesIndex;
            delete [] med_PolyhedronIndex;
          }
        // on ecrit les familles avant d'oublier....
#ifdef MED30
        if ((ret = MEDmeshEntityFamilyNumberWr(fid,nom_dom,MED_NO_DT,MED_NO_IT,MED_CELL,type_elem_med,nelem,nufano)) < 0)
#else
        if ((ret = MEDfamEcr(fid,nom_dom,nufano,nelem,MED_CELL,type_elem_med)) < 0)
#endif
          Process::exit();

      }
    if (sizeof(med_int)!=sizeof(int))
      delete [] med_les_elems;

    if (ret<0)
      {
        Cerr<<"Problem when writing elements in "<<nom_fic<<finl;
        Process::exit();
      }
    delete []  nufano;
  }
  //ecriture des faces...
  for (int j=0; j<type_face.size(); j++)
    medecrirefaces( all_faces_bord[j],type_face[j], fid, nom_dom, Objet_U::dimension, familles[j]);
  // ecriture des familles c.a.d leur nom = les cl
  {
    int natt=1;
    med_int *attval,*attide;
    attide = new med_int[natt];
    attval = new med_int[natt];
    attide[0]=1;
    // ajout de la famille 0
    Nom nomfz(med_taille_nom);
    Nom nom_zone_bis;
    nom_zone_bis="";
    if (noms_bords.search(nom_zone)!=-1)
      {
        Cerr<<"The zone name is also a boundary name"<<finl;
        nom_zone_bis="cpy_";
      }
    nom_zone_bis+=nom_zone;
    //strcpy(nomfz,nom_zone_bis);
    nomfz="elems";
#ifdef MED30
#define MEDfamCr(fid,nom_dom,nomf,num,a,b,c,d,group,ngroup)  MEDfamilyCr(fid,nom_dom,nomf,num,ngroup,group);
#endif
    ret=MEDfamCr(fid,nom_dom,nomfz,0,NULL,NULL,NULL,0,NULL,0);
    if (ret<0)
      {
        Cerr<<"Problem when writing nom_zone family in "<<nom_fic<<finl;
        Process::exit();
      }
    if (nom_zone_bis=="elems") nom_zone_bis+="_cpy";
    nomfz=nom_zone_bis;
    //strcpy(nomfz,"elems");
    ret=MEDfamCr(fid,nom_dom,nomfz,famglob,NULL,NULL,NULL,0,NULL,0);
    if (ret<0)
      {
        Cerr<<"Problem when writing the elements family in "<<nom_fic<<finl;
        Process::exit();
      }
    Char_ptr attdes;
    dimensionne_char_ptr_taille(attdes,MED_COMMENT_SIZE,natt);
    int ii;
    for (ii=0; ii<MED_COMMENT_SIZE*natt; ii++) attdes[ii]=' ';
    attdes[ii]='\0';

    for (int i=0; i<noms_bords.size(); i++)
      {
        attval[0]=i+1;
#ifdef MED30
        Char_ptr groupname;
        dimensionne_char_ptr_taille(groupname,MED_LNAME_SIZE);
        affecte_nom_med(groupname,noms_bords[i]);
        ret=MEDfamilyCr(fid,nom_dom,noms_bords[i],-(i+1),1,groupname);

#else
        ret=MEDfamCr(fid,nom_dom,noms_bords[i],-(i+1),attide,attval,attdes,natt,NULL,0);
#endif
        if (ret<0)
          {
            Cerr<<"Problem when writing the family " <<i+1<<" in "<<nom_fic<<finl;
            Cerr<<nom_dom<<" "<<noms_bords[i]<<" "<<(int)attide[0]<<" "<<(int)attval[0]<<" "<<attdes<<" natt "<<natt<<finl;
            Process::exit();
          }
      }
    delete [] attide;
    delete [] attval;
  }
#ifdef MED_ESSAI
  //un petit essai pour rire
  // provoque memory_fault!!!
  // peut etre pb MEDTAILLE non utilise partout... a reprendre partout
  // a chaque appel MED  .... courage
  if (0)
    {
      //_MEDmodeErreurVerrouiller();
      med_idt root;
      Nom chemin="/TRUST";
      // creation du repertoire TRUST
      if ((root = _MEDdatagroupOuvrir(fid,chemin)) < 0)
        if ((root = _MEDdatagroupCreer(fid,chemin)) < 0)
          exit();
      Nom data(Objet_U::nom_du_cas());
      if ((ret = _MEDattrStringEcrire(root,"CAS",strlen(data),data,MED_ACC_CREAT)) < 0)
        exit();
      //    if ((ret = _MEDattFloatEcrire(root,"DOUBLE",strlen(data),data,MED_ACC_CREAT)) < 0)
      // exit();
    }
#endif
  ret = MEDfileClose(fid);
  if (ret<0)
    {
      Cerr<<"Problem when closing  "<<nom_fic<<finl;
      Process::exit();
    }
  return ret;
}


// passage de la connectivite trio a MED si sens=1
// de MED a trio si sens=-1
void renum_conn(IntTab& les_elems2,Nom& type_elem,int sens)
{
  int nele=les_elems2.dimension(0);
  // cas face_bord vide
  if (nele==0) return;
  med_geometry_type type_elem_med;
  type_elem_med=type_geo_trio_to_type_med(type_elem);
  IntTab les_elemsn(les_elems2);
  ArrOfInt filter;
  switch (type_elem_med)
    {
    case MED_POINT1  :
      {
        filter.resize_array(1) ;
        filter[0] = 0 ;
        break ;
      }
    case MED_SEG2    :
      {
        filter.resize_array(2) ;
        filter[0] = 0 ;
        filter[1] = 1 ;

        break ;
      }
    case MED_SEG3    :
      {
        break ;
      }
    case MED_TRIA3   :
      {
        filter.resize_array(3) ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        break ;
      }
    case MED_QUAD4   :
      {
        filter.resize_array(4) ;
        //         filter[0] = 0 ;
        //         filter[1] = 1 ;
        //         filter[2] = 3 ;
        //           filter[3] = 2 ;


        filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 3 ;
        filter[3] = 1 ;

        break ;
      }
    case MED_TRIA6   :
      {
        break ;
      }
    case MED_QUAD8   :
      {
        break ;
      }
    case MED_TETRA4  :
      {
        filter.resize_array(4) ;
        //exit();
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;  // 3td element in med are 4th in vtk (array begin at 0 !)
        filter[3] = 3 ;  // 4th element in med are 3rd in vtk (array begin at 0 !)
        break ;
      }
    case MED_PYRA5   :
      {
        filter.resize_array(5) ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
        filter[4] = 4 ;
        break ;
      }
    case MED_PENTA6  :
      {
        filter.resize_array(6) ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        filter[3] = 3 ;
        filter[4] = 4 ;
        filter[5] = 5 ;
        break ;
      }
    case MED_HEXA8   :
      {
        filter.resize_array(8) ;


        filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 3 ;
        filter[3] = 1 ;
        filter[4] = 4 ;
        filter[5] = 6 ;
        filter[6] = 7 ;
        filter[7] = 5 ;
        break ;
      }
    case MED_TETRA10 :
      {
        break ;
      }
    case MED_PYRA13  :
      {
        break ;
      }
    case MED_PENTA15 :
      {
        break ;
      }
    case MED_HEXA20  :
      {
        break ;
      }
#ifdef MED30
    case MED_OCTA12 :
#endif
    case MED_POLYGON:
    case MED_POLYHEDRON:
      {
        int nb_som_max=les_elems2.dimension(1);
        filter.resize_array(nb_som_max);
        for (int i=0; i<nb_som_max; i++) filter[i]=i;
        break ;
      }
    default:
      {
        Cerr<<"case not scheduled"<<finl;
        Process::exit();
      }
    }

  int ns=filter.size_array();
  if (ns==0)
    {
      Cerr<<"Problem for filtering operation "<<finl;
      Process::exit();
    }
  if (filter.size_array()!=les_elems2.dimension(1))
    {
#ifndef POURSATURNE
      Cerr<<"Problem for filtering operation "<<finl;
      exit();
#endif
    }
  if (sens==1)
    {
      for (int el=0; el<nele; el++)
        for (int n=0; n<ns; n++)
          les_elems2(el,n)=les_elemsn(el,filter[n])+1;

    }
  else if (sens==-1)
    {
      for (int el=0; el<nele; el++)
        for (int n=0; n<ns; n++)
          les_elems2(el,filter[n])=les_elemsn(el,n)-1;
    }
  else
    {
      Cerr<<"error in renum_conn : meaning "<<sens<<"not valid"<<finl;
      Process::exit();
    }
}





// permet de boucler sur les bords,raccords,joints
const Frontiere& mes_faces_fr(const Zone& zone,int i)
{
  //int nb_faces_joint=zone.nb_faces_joint();
  int nb_std=zone.nb_front_Cl();
  if (i<nb_std)
    return zone.frontiere(i);
  else return zone.joint(i-nb_std);
}


// a partir d'un domaine extrait le type de face, la connectivite des faces de bords, le nom des bords et cree les familles
void creer_all_faces_bord(const Domaine& dom,Noms& type_face,VECT(IntTab)& all_faces_bord, Noms& noms_bords,VECT(ArrOfInt)& familles)
{
  const Zone& zone=dom.zone(0);
  int nb_type_face=zone.type_elem().nb_type_face();
  type_face.dimensionner(nb_type_face);
  all_faces_bord.dimensionner(nb_type_face);
  familles.dimensionner(nb_type_face);


  int nb_faces_bord=zone.nb_faces_bord()+zone.nb_faces_raccord()+zone.nb_faces_int()+zone.nb_faces_joint();
  int nb_bords=zone.nb_front_Cl()+zone.nb_joints();
  if (nb_bords==0)
    {
      // on n'a pas les bords
      for (int j=0; j<nb_type_face; j++)
        all_faces_bord[j].resize(0,0);
      return;
    }
  // on cherche un bord non vide pour recuperer le type de faces
  int nb_type_trouve=0;
  for (int ii=0; ii<nb_bords; ii++)
    {
      if (mes_faces_fr(zone,ii).faces().nb_faces()!=0)
        {
          Nom type_face_b;
          type_face_b=mes_faces_fr(zone,ii).faces().type(mes_faces_fr(zone,ii).faces().type_face());
          int existe=0;
          for (int j=0; j<nb_type_trouve; j++)
            if (type_face_b==type_face[j])
              existe=1;
          if (!existe)
            {
              type_face[nb_type_trouve]=type_face_b;
              nb_type_trouve++;
            }
        }
    }
  if (nb_type_trouve!=nb_type_face)
    {
      Cerr<<"all boundaries type have not been found" <<finl;
    }
  //Cerr<<"type_face "<<type_face<<finl;

  int nb_som_face_max=zone.type_elem().nb_som_face(0);
  for (int j=0; j<nb_type_face; j++)
    {
      all_faces_bord[j].resize(nb_faces_bord, nb_som_face_max);
#ifdef POURSATURNE
      all_faces_bord[j]=0;
#endif
      familles[j].resize_array(nb_faces_bord);
    }
  noms_bords.dimensionner(nb_bords);

  // enfin on recupere le nom des faces de bord et leur connectivite
  ArrOfInt cpt(nb_type_face);
  ArrOfInt nb_som(nb_type_face);
  for(int i=0; i<nb_bords; i++)
    {
      if (sub_type(Raccord_base,mes_faces_fr(zone,i)))
        {
          noms_bords[i]="type_raccord_";
          noms_bords[i]+=mes_faces_fr(zone,i).le_nom();
        }
      else
        noms_bords[i]=mes_faces_fr(zone,i).le_nom();
      const IntTab& les_sommets_des_faces=mes_faces_fr(zone,i).les_sommets_des_faces();
      int nb_fac=les_sommets_des_faces.dimension(0);
      // on cherche de quel type est le bord
      int ref=0;
      if (nb_fac>0)
        {
          Nom type_face_b=mes_faces_fr(zone,i).faces().type(mes_faces_fr(zone,i).faces().type_face());
          ref=type_face.search(type_face_b);
        }
      for(int j=0; j<nb_fac; j++)
        {
          int nb_som_face=les_sommets_des_faces.dimension(1);
          familles[ref](cpt[ref])=-(i+1);
          for(int k=0; k<nb_som_face; k++)
            all_faces_bord[ref](cpt[ref], k)=les_sommets_des_faces(j, k);
          cpt[ref]++;
          nb_som[ref]=nb_som_face;
        }
    }
  for (int j=0; j<nb_type_face; j++)
    {
      familles[j].resize_array(cpt[j]);
      if (nb_type_face>1)
        {
          IntTab prov=all_faces_bord[j];
          int nb_som_face=nb_som[j];
          int nb_faces=cpt[j];
          IntTab& all_faces_bord_=all_faces_bord[j];
          all_faces_bord_.resize(nb_faces,nb_som_face);
          for (int f=0; f<nb_faces; f++)
            for (int c=0; c<nb_som_face; c++)
              all_faces_bord_(f,c)=prov(f,c);
        }
    }
}

// ecrit le domaine dom dans le fichier nom_fic
// mode =0 nouveau fichier
// mode = -1 ajout du domaine dans le fichier
void EcrMED::ecrire_domaine(const Nom& nom_fic,const Domaine& dom,const Nom& nom_dom,int mode)
{
  //Cerr<<"Here writing of the domain "<<nom_dom<<" in "<<nom_fic<<" mode "<<mode<<finl;
  const  DoubleTab& sommets=dom.les_sommets();
  const  Zone& zone=dom.zone(0);
  Nom type_elem=zone.type_elem()->que_suis_je();

  const IntTab& les_elems=zone.les_elems();

  Noms type_face;
  VECT(IntTab) all_faces_bord;
  Noms noms_bords;
  VECT(ArrOfInt) familles;
  // remplit le tableau all_faces_bords ainsi que noms_bords et familles
  creer_all_faces_bord( dom, type_face, all_faces_bord,  noms_bords,familles);
  // connectivite Trio a MED
  for (int j=0; j<type_face.size(); j++)
    renum_conn(all_faces_bord[j],type_face[j],1);
  IntTab les_elems2(les_elems);

  // connectivite Trio a MED
  renum_conn(les_elems2,type_elem,1);

  if (dimension==0)
    {
      Cerr << "Dimension is not defined. Check your data file." << finl;
      Process::exit();
    }
  if (0)
    {
      Cerr << "Writing of the domain at the med format"<<finl
           <<"nom_fic = " << nom_fic<< finl
           << "nom_dom ="<<nom_dom<<finl;
      Cerr << "dimension = " << dimension<< finl
           << "noms_bords= " << noms_bords<< finl;
    }
#ifdef MEDCOUPLING_
  if (use_medcoupling_)
    {
      Cerr << "Trying to write MED file with MEDCoupling API. To use the MEDFile API, use EcrMEDfile or MEDFile keyword." << finl;
      Cerr << "Creating a MEDCouplingUMesh object for the domain " << nom_dom << finl;
      // Get MEDCoupling cell type and mesh dimension:
      int mesh_dimension = -1;
      INTERP_KERNEL::NormalizedCellType cell_type = type_geo_trio_to_type_medcoupling(type_elem, mesh_dimension);
      int ncells = les_elems2.dimension(0);
      int nverts = les_elems2.dimension(1);
      MCAuto<MEDCouplingUMesh> mesh(MEDCouplingUMesh::New(nom_dom.getChar(), mesh_dimension));
      // Nodes:
      int nnodes = sommets.dimension(0);
      MCAuto<DataArrayDouble> points(DataArrayDouble::New());
      points->useArray(sommets.addr(), false, MEDCoupling::CPP_DEALLOC, nnodes, dimension);
      points->setInfoOnComponent(0, "x");
      points->setInfoOnComponent(1, "y");
      if (dimension == 3) points->setInfoOnComponent(2, "z");
      mesh->setCoords(points);
      mesh->allocateCells(ncells);
      // Cells
      if (cell_type == INTERP_KERNEL::NORM_POLYHED)
        {
          // Polyedron is special, seepage 10:
          // http://trac.lecad.si/vaje/chrome/site/doc8.3.0/extra/Normalisation_pour_le_couplage_de_codes.pdf
          const Polyedre& Poly = ref_cast(Polyedre, zone.type_elem().valeur());
          ArrOfInt Nodes_glob;
          Poly.remplir_Nodes_glob(Nodes_glob, les_elems2);
          const ArrOfInt& FacesIndex = Poly.getFacesIndex();
          const ArrOfInt& PolyhedronIndex = Poly.getPolyhedronIndex();
          assert(ncells == PolyhedronIndex.size_array() - 1);
          for (int i = 0; i < ncells; i++)
            {
              int size = 0;
              for (int face = PolyhedronIndex(i); face < PolyhedronIndex(i + 1); face++)
                size += FacesIndex(face + 1) - FacesIndex(face) + 1;
              size--; // No -1 at the end of the cell
              ArrOfInt cell_def(size);
              size = 0;
              for (int face = PolyhedronIndex(i); face < PolyhedronIndex(i + 1); face++)
                {
                  for (int node = FacesIndex(face); node < FacesIndex(face + 1); node++)
                    {
                      cell_def[size] = Nodes_glob(node) - 1; // Numerotation Fortran -> C++
                      size++;
                    }
                  if (size < cell_def.size_array())
                    {
                      // Add -1 to mark the end of a face:
                      cell_def[size] = -1;
                      size++;
                    }
                }
              mesh->insertNextCell(cell_type, cell_def.size_array(), cell_def.addr());
            }
        }
      else
        {
          // Other cells:
          les_elems2 -= 1; // Numerotation Fortran -> C++
          for (int i = 0; i < ncells; i++)
            {
              int nvertices = nverts;
              for (int j = 0; j < nverts; j++)
                if (les_elems2(i, j) < 0)
                  nvertices--; // Some cell type has not a constant number of vertices (eg: Polyhedron)
              mesh->insertNextCell(cell_type, nvertices, les_elems2.addr() + i * nverts);
            }
        }
      MCAuto<MEDFileUMesh> file(MEDFileUMesh::New());
      file->setName(mesh->getName()); //name needed to be non empty
      file->setCoords(mesh->getCoords());
      file->setMeshAtLevel(0, mesh, false);
      // Store the mesh
      dom.setUMesh(mesh);

      // Family for the cells:
      int global_family_id = -1000;
      MCAuto<DataArrayInt> famArr(DataArrayInt::New());
      famArr->alloc(ncells);
      famArr->fillWithValue(global_family_id);
      file->setFamilyFieldArr(0, famArr);
      // Name the family and check unicity:
      Nom family_name = noms_bords.search(zone.le_nom()) != -1 ? "cpy_" : "";
      family_name += zone.le_nom();
      file->addFamily(family_name.getString(), global_family_id);

      // Boundary mesh:
      MCAuto<MEDCouplingUMesh> boundary_mesh(MEDCouplingUMesh::New(mesh->getName(), mesh_dimension - 1));
      boundary_mesh->setCoords(mesh->getCoords());
      int nfaces = 0;
      int nb_type_face = familles.size();
      for (int j = 0; j < nb_type_face; j++)
        nfaces += familles(j).size_array();
      boundary_mesh->allocateCells(nfaces);
      for (int j = 0; j < nb_type_face; j++)
        {
          int size = familles(j).size_array();
          if (size)
            {
              // Converting trio to medcoupling boundary cell:
              int boundary_mesh_dimension = -1;
              INTERP_KERNEL::NormalizedCellType type_boundary_cell = type_geo_trio_to_type_medcoupling(type_face[j], boundary_mesh_dimension);
              assert(boundary_mesh_dimension == mesh_dimension - 1);
              nverts = all_faces_bord[j].dimension(1);
              all_faces_bord[j] -= 1; // Numerotation Fortran -> C++
              for (int i = 0; i < size; i++)
                {
                  int nvertices = nverts;
                  for (int k = 0; k < nverts; k++)
                    if (all_faces_bord[j].addr()[i * nverts + k] < 0)
                      nvertices--; // Some face type has not a constant number of vertices (eg: Polygon)
                  boundary_mesh->insertNextCell(type_boundary_cell, nvertices, all_faces_bord[j].addr() + i * nverts);
                }
            }
        }
      MCAuto<DataArrayInt> renum_boundary_cell(boundary_mesh->sortCellsInMEDFileFrmt());
      file->setMeshAtLevel(-1, boundary_mesh, false);

      bool use_group_instead_of_family = false;
      if (use_group_instead_of_family)
        {
          //(ToDo try to hide family notion for MEDCoupling and use group instead)
        }
      else
        {
          // Family (with possible renum of the boundary cells)
          MCAuto<DataArrayInt> family_array(DataArrayInt::New());
          family_array->alloc(nfaces);
          int face = 0;
          for (int j = 0; j < nb_type_face; j++)
            for (int i = 0; i < familles[j].size_array(); i++)
              {
                int family_id = familles[j](i);
                family_array->setIJ(renum_boundary_cell->getIJ(face, 0), 0, family_id);
                face++;
              }
          file->setFamilyFieldArr(-1, family_array);

          // Naming family on boundaries:
          for (int i = 0; i < noms_bords.size(); i++)
            {
              int family_id = -(i + 1);
              file->addFamily(noms_bords[i].getString(), family_id);
              std::vector<std::string> grps(1);
              grps[0] = noms_bords[i].getString();
              file->setGroupsOnFamily(noms_bords[i].getString(), grps);
            }
        }
      // Write:
      int option = (mode == -1 ? 2 : 1); /* 2: reset file. 1: append, 0: overwrite objects */
      Cerr<<"Writing file " << nom_fic<<" (mode=" << mode << ") ..."<<finl;
      file->write(nom_fic.getString(), option);
    }
  else
#endif
    medecrgeom(nom_fic,nom_dom,dimension,sommets,type_elem,zone.type_elem(),les_elems2,type_face,all_faces_bord,familles,noms_bords,zone.le_nom(),mode, major_mode);
//Cerr<<"Writing of the domain is ended"<<finl;

}

// POUR LES CHAMPS
void affecte_nom_med(Char_ptr& nom_med,const Nom& mot)
{
  int cut=0;
  int mm=mot.longueur();
  if (nom_med.longueur()<mm)
    {
      Cerr<<"Following name is cut "<< mot;
      mm=nom_med.longueur()-1;
      cut=1;
    }
  strncpy(nom_med,mot,mm);
  nom_med[mm-1]='\0';
  if (cut) Cerr<<" to "<<nom_med<<finl;

}
// regarde si le champ de nom cha1 existe dans nom_fic
// sinon le cree
int medcreerchamp(const Nom& nom_fic,const Nom& nomcha1_org,const Nom& nom_dom, const Nom& comp2,const Nom& unit2,int nbcomp,med_int& nbofcstp, bool major_mode)
{
  Char_ptr med_taille_nom;
  dimensionne_char_ptr_taille(med_taille_nom,MED_NAME_SIZE);
  Char_ptr nomcha1(med_taille_nom);
  affecte_nom_med(nomcha1,nomcha1_org);
  int fid = trustMEDfileOpen(nom_fic,MED_ACC_RDEXT, major_mode);

  int ret=0;
  // on regarde si le champ existe
  int nbch=MEDnField(fid);
  if (nbch<0)
    {
      Cerr<<"Error when reading the number of fields"<<finl;
      Process::exit();
    }
  int trouve=0;
  Char_ptr nomcha(med_taille_nom);

  Char_ptr comp, unit;
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
          // med_int nbofcstp;
          ret=MEDfieldInfo(fid,ch+1,nomcha,meshname,&localmesh,&fieldtype,comp,unit,dtunit,&nbofcstp);
#else
          med_type_champ typcha;
          ret = MEDchampInfo(fid,ch+1,nomcha,&typcha,comp,unit,ncomp);
#endif
          //printf("Nom du champ : %s de type %d\n",nomcha,typcha);
          //printf("Nom des composantes : %s\n",comp);
          //printf("Unites des composantes : %s \n",unit);
          Nom Nnomcha(nomcha);
          if ((trouve==0)&&(nomcha1==Nnomcha))
            {
              trouve=1;
              assert(ncomp==nbcomp);
#ifdef MED30
              if ((meshname!=nom_dom))

                {
                  Cerr<< nomcha1<<" is alrady on "<<meshname<<". You can't add this field on "<<nom_dom<<finl;
                  Process::exit();
                }
#endif
              //Cerr<<"This field is already in hand"<<finl;
              break;
            }
        }
    }
  if (trouve==0)
    {
      // il n existe pas on le cree
      //Cerr<<"creation of the field "<<nomcha1<<finl;
#ifdef MED30
      Char_ptr dtunit;
      dimensionne_char_ptr_taille(dtunit,MED_SNAME_SIZE);
      strcpy(dtunit,"s");
      ret=MEDfieldCr(fid,nomcha1,MED_FLOAT64,nbcomp,comp2,unit2,dtunit,nom_dom);
      nbofcstp=0;
#else
      ret=MEDchampCr(fid,nomcha1,MED_FLOAT64,comp2,unit2,nbcomp);
#endif
    }
  MEDfileClose(fid);
  return ret;
}

// ecrit les valeurs dans le champ precedemment declare
int medecrchamp(const Nom& nom_fic,const Nom& nom_dom,const Nom& nomcha1,const DoubleTab& val,const Nom& type,const Nom& type_elem,double dt,int compteur,med_int& nboft, bool major_mode)
{
  Char_ptr med_taille_nom;
  dimensionne_char_ptr_taille(med_taille_nom,MED_NAME_SIZE);
  int ret=0;
  int nbele=val.dimension(0);
  if (nbele==0) return 0;
  int fid = trustMEDfileOpen(nom_fic,MED_ACC_RDEXT, major_mode);

  //MED_CELL si CHAMPMAILLE
  //MED_NODE si CHAMPNOEUD
  // comme les bords si CHAMPFACE
  med_entity_type type_ent;
  type_ent=MED_CELL;
  if (type=="CHAMPPOINT")
    {
      type_ent=MED_NODE;
    }
  else if (type=="CHAMPFACE")
    {

#ifdef POURSATURNE
      type_ent=MED_CELL;
#else
      if (Objet_U::dimension==2)
        type_ent=MED_ARETE;
      else
        type_ent=MED_FACE;
#endif
    }
  else if (type!="CHAMPMAILLE")
    {
      Cerr<<type<<"to check"<<finl;
      Process::exit();
    }

  //
#ifdef MED30
  int nn=nboft;
#else
  int nn=MEDnPasdetemps(fid,nomcha1,type_ent,type_geo_trio_to_type_med(type_elem));
#endif
  //Cerr<<nn <<" Pas de temps Champ"<<nomcha1<<finl;
  Char_ptr nom_dom_med(med_taille_nom);
  strcpy(nom_dom_med,nom_dom);

  Char_ptr nom_chp_med(med_taille_nom);
  affecte_nom_med(nom_chp_med,nomcha1);
#ifdef MED30
  ret+=MEDfieldValueWr(fid,nom_chp_med,nn,MED_NO_IT,dt,type_ent,type_geo_trio_to_type_med(type_elem),MED_FULL_INTERLACE, MED_ALL_CONSTITUENT,nbele,(unsigned char*) val.addr());
#else
  ret+=MEDchampEcr(fid,nom_dom_med,nom_chp_med,(unsigned char*) val.addr(),MED_FULL_INTERLACE,nbele,(char*)MED_NOGAUSS,MED_ALL,(char*)MED_NOPFL,MED_NO_PFLMOD,type_ent,type_geo_trio_to_type_med(type_elem),nn,(char*)"s",dt,MED_NONOR);
#endif

  MEDfileClose(fid);
  return ret;
}



// permet d'ecrire le tableau de valeurs val comme un champ dans le
// fichier med de nom nom_fic, avec pour support le domaine de nom nom_dom.
// type: CHAMPPOINT,CHAMPMAILLE,CHAMPFACE
// nom_cha1 le nom du champ
// unite : les unites
// type_elem le type des elems du domaine
// time le temps
// compteur le numero du pas de temps (ne sert pas pour l'instant)
// ToDo: suppress compteur variable (set to 1 and NEVER used !)
void EcrMED::ecrire_champ(const Nom& type,const Nom& nom_fic,const Domaine& dom,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,int compteur)
{
  const Nom& nom_dom = dom.le_nom();
#ifdef MEDCOUPLING_
  // MED Coupling
  if (use_medcoupling_)
    {
      // Create MEDCouplingField
      MEDCoupling::TypeOfField field_type;
      if (type == "CHAMPMAILLE")
        field_type = ON_CELLS;
      else if (type == "CHAMPPOINT")
        field_type = ON_NODES;
      else
        {
          Cerr << "Field type " << type << " is not supported yet." << finl;
          Process::exit();
          return;
        }
      std::string file_name = nom_fic.getString();
      std::string field_name = nom_cha1.getString();
      // Get the previous timestep:
      int timestep = 0;
      std::vector< std::string > field_names = GetAllFieldNames(file_name);
      if ( std::find(field_names.begin(), field_names.end(), field_name) != field_names.end() )
        {
          std::vector< std::pair< std::pair< int, int >, double > > ts = GetAllFieldIterations(file_name, field_name);
          timestep = ts[ts.size()-1].first.first + 1;
        }
      MCAuto<MEDCouplingFieldDouble> field(MEDCouplingFieldDouble::New(field_type, MEDCoupling::ONE_TIME));
      field->setName(field_name);
      field->setTime(time, timestep, -1);
      field->setTimeUnit("s");
      // Try to get directly the mesh from the domain:
      if (dom.getUMesh()!=NULL)
        field->setMesh(dom.getUMesh());
      else
        {
          // Get mesh from the file (less optimal but sometime necessary: eg: call from latatoother::interpreter())
          MCAuto<MEDFileUMesh> file_mesh(MEDFileUMesh::New(file_name));
          field->setMesh(file_mesh->getMeshAtLevel(0));
        }
      // Fill array:
      int size = val.dimension(0);
      int nb_comp = val.nb_dim() == 1 ? 1 : val.dimension(1);
      MCAuto<DataArrayDouble> array(DataArrayDouble::New());
      array->useArray(val.addr(), false, MEDCoupling::CPP_DEALLOC, size, nb_comp);
      array->setInfoOnComponent(0, "x ["+unite[0].getString()+"]");
      if (nb_comp>1)
        {
          array->setInfoOnComponent(1, "y ["+unite[1].getString()+"]");
          if (nb_comp>2)
            array->setInfoOnComponent(2, "z ["+unite[2].getString()+"]");
        }
      field->setArray(array);
      // Write
      MCAuto<MEDFileField1TS> file(MEDFileField1TS::New());
      file->setFieldNoProfileSBT(field);
      file->write(file_name, 0);

    }
  else
#endif
    {
      // Deprecated:
      // MED file
      int ret = 0;
      int nbcomp = 1;
      if (val.nb_dim() > 1)
        nbcomp = val.dimension(1);

      // conversion de la liste de noms unite en un mot
      Nom unite1 = "";
      for (int nn = 0; nn < nbcomp; nn++)
        {
          Nom uni = unite[nn];
          int lutil = uni.longueur() - 1;
          if (lutil > MED_SNAME_SIZE)
            {
              Cerr << uni << " " << uni.longueur() << " names copy has to be revised" << finl;
              //exit();
              uni = "???";
              lutil = uni.longueur() - 1;


            }
          unite1 += uni;
          for (int c = 0; c < (MED_SNAME_SIZE - lutil); c++) unite1 += " ";
        }
      Nom nomcoo = "x               ";
      if (nbcomp > 1)
        nomcoo += "y               ";
      if (nbcomp > 2)
        nomcoo += "z               ";

      // cree le champ si il n'existe pas
      med_int nbofcstp;
      ret = medcreerchamp(nom_fic, nom_cha1, nom_dom, nomcoo, unite1, nbcomp, nbofcstp, major_mode);
      if (ret < 0)
        {
          Cerr << "error field creation" << finl;
          exit();
        }


      // ecrit le champ
      ret += medecrchamp(nom_fic, nom_dom, nom_cha1, val, type, type_elem, time, compteur, nbofcstp, major_mode);
      if (ret < 0)
        {
          Cerr << "Error writing field" << finl;
          exit();
        }
    }
}





// methode proche de celle standard
// le but ecrire les champs aux faces
// on lui passe en plus le Champ_Inc pour recuperer la zone_dis

// ne marche pas si l'on a pas ecrit des le depart toutes les faces...
void EcrMED::ecrire_champ(const Nom& type,const Nom& nom_fic,const Domaine& dom,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,int compteur,const Champ_Inc_base& le_ch)
{
  if (use_medcoupling_)
    {
      Cerr << "Todo " << finl;
      Process::exit();
      return;
    }
#ifndef MED30
  if (type!="CHAMPFACE")
#endif
    {
      Cerr<<"EcrMED::ecrire_champ(const Nom& type,const Nom& nom_fic,const Nom& nom_dom,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,int compteur,const Champ_Inc_base& le_ch) is assessed only for fields of type Champ_Face"<<finl;
      exit();
    }
#ifndef MED30
  const Zone_VF& la_zone_dis=ref_cast(Zone_VF,le_ch.zone_dis_base());
  DoubleTab valeurs(la_zone_dis.nb_faces(),le_ch.nb_comp());

  // Pour les faces
  Cerr<<" checking that all the sides are written"<<finl;
  med_entity_type type_ent;
  type_ent=MED_CELL;
  if (type=="CHAMPPOINT")
    {
      type_ent=MED_NODE;
    }
  else if (type=="CHAMPFACE")
    {
      if (Objet_U::dimension==2)
        type_ent=MED_ARETE;
      else
        type_ent=MED_FACE;
#ifdef POURSATURNE
      type_ent=MED_CELL;
#endif
    }
  else if (type!="CHAMPMAILLE")
    {
      Cerr<<type<<"to check"<<finl;
      exit();
    }
  int fid = trustMEDfileOpen(nom_fic,MED_ACC_RDONLY, major_mode);
  int nm=MEDnEntMaa(fid,nom_dom,MED_CONN,type_ent,type_geo_trio_to_type_med(type_elem),MED_NODAL);
  MEDfileClose(fid);
  Cerr<<"nm "<<nm<<" " <<valeurs.dimension(0)<<finl;

  if (nm!=valeurs.dimension(0))
    {
      Cerr<<"one rewrites the sides"<<finl;

      Noms type_face_;
      VECT(IntTab) all_faces_bord_;
      Noms noms_bords;
      VECT(ArrOfInt) familles_;
      const Zone_VF& la_zone_dis=ref_cast(Zone_VF,ref_cast(Champ_Inc_base, le_ch).zone_dis_base());
      const Domaine& dom=la_zone_dis.zone().domaine();
      creer_all_faces_bord( dom, type_face_, all_faces_bord_,  noms_bords,familles_);
      assert(type_face_.size()==1);
      Nom& type_face=type_face_[0];
      IntTab& all_faces_bord=all_faces_bord_[0];
      ArrOfInt& familles=familles_[0];
      // on recupere les faces internes

      int premiere_face_int=la_zone_dis.premiere_face_int();
      int nbfaces=la_zone_dis.nb_faces();
      int nbsomfa=all_faces_bord.dimension(1);
      int nbbord=familles.size_array();
      if (0)
        {
          Cerr<<"before "<<all_faces_bord(20,0)<<finl;
          all_faces_bord.resize(nbfaces,nbsomfa);
          Cerr<<"after "<<all_faces_bord(20,0)<<finl;
          familles.resize_array(nbfaces);
          const IntTab& face_sommets= la_zone_dis.face_sommets();
          int nb_som_fa=face_sommets.dimension(1);
          for (int fac=premiere_face_int; fac<nbfaces; fac++)
            {
              for (int som=0; som<nb_som_fa; som++) all_faces_bord(fac,som)=face_sommets(fac,som);
              familles(fac)=-11*0;
            }
        }

      if (nbbord!= premiere_face_int) abort();

      renum_conn(all_faces_bord,type_face,1);
      Cerr<<"here "<<(int)MED_ACC_RDEXT <<" "<<(int)MED_ACC_CREAT<<finl;
      med_access_mode es=MED_ACC_RDEXT;
      // if (1) es=MED_ACC_CREAT;
      //es=MED_ACC_RDEXT;
      int fid = trustMEDfileOpen(nom_fic,es, major_mode);
      Cerr<<" fid "<<fid<<finl;
      //int nm2=MEDnEntMaa(fid,nom_dom,MED_CONN,type_ent,type_geo_trio_to_type_med(type_elem),MED_NODAL);
      int nm2=nm;
      Cerr<<" number "<<nm<<" "<<nm2<<finl;
      //MEDfileClose(fid);
      //exit();
      med_access_mode es2=MED_ACC_RDEXT;
      //if (1) es2=MED_ACC_CREAT;
      Cerr<<"Ok HERE "<<medecrirefaces( all_faces_bord,type_face, fid, nom_dom, Objet_U::dimension, familles,es2)<<" "<<(int)es<<" "<<(int)es2<<finl;

      MEDfileClose(fid);
      // on bloque tout
      // on n'arrive pas a reecrire les faces !!!
      //  abort();
    }

  const   DoubleTab& xv =la_zone_dis.xv();
  le_ch.valeur_aux(xv, valeurs);

  ecrire_champ(type, nom_fic,nom_dom, nom_cha1,valeurs,unite,type_elem, time,compteur);
#endif
}
#endif

