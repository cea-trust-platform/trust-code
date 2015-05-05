/*****************************************************************************
 *
 * Copyright (c) 2011 - 2013, CEA
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CEA, nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <LataDB.h>
#include <LataFilter.h>
#include <LmlReader.h>
#include <stdlib.h>
#include <Octree_Double.h>
#include <string.h>
//#include <strings.h>

using namespace std;


//#define connect_max 18


double       gepsilon =1e-05;
double       gepsilon_champs =1e-05;
double       gerr = 0.0;
double       gemax = 0;         /* Erreur maximum rencontree */
DoubleTab xp;
double xmax = 0.0;
double ymax = 0.0;
double zmax = 0.0;

int max_delta=0;

int Ecart(double x, double y, double ymax,double gepsilon=1e-5)
{
  int flag = 1;
  gerr = 0.0;
  if(ymax != 0.0) gerr = fabs((x-y)/ymax);
  /* Erreur relative : en %, multiplier par 100; */
  /* Si le max est inferieur a 1.e-12 on ne compare pas */
  /* GF et si maintenant on compare */
  double seuil=1e-12;
  if (ymax<=seuil)
    {
      if( (fabs(x)>seuil)||(fabs(y)>seuil))
        {
          gerr=1;
        }
      else gerr=0;
    }
  if(gerr <= gepsilon) flag = 0;
  if(fabs(gerr) > gemax) gemax = gerr;
  return (flag);
}

void open(Nom& filename, LataFilter& filter, LataOptions& opt, LataDB& lata_db)
{
  try
    {
      LataOptions::extract_path_basename(filename, opt.path_prefix, opt.basename);
      opt.dual_mesh = true;
      opt.faces_mesh = false;
      // on ne veut jamais creer le domaine IJK
      opt.regularize = 2;
      opt.regularize_tolerance = 1e-7;
      read_any_format_options(filename, opt);
      // Read the source file to the lata database
      read_any_format(filename, opt.path_prefix, lata_db);
      filter.initialize(opt, lata_db);
    }
  catch (LataDBError err)
    {
      cerr<<" error while opening "<<filename<<" "<<err.describe()<<endl;
      exit(-1);
    }


}


void usage(const char* arg0)
{
  cerr<<"usage "<<arg0<<" file1 file2 [--dernier] [--seuil val] [--max_delta]"<<endl;
}


const Domain& get_domain(LataFilter& filter,Domain_Id& id, Nom& filename)
{
  try
    {
      return filter.get_geometry(id);
    }
  catch (LataDBError err)
    {
      cerr<<" error while loading "<<id.name_ <<" in file "<< filename <<" "<<err.describe()<<endl;
      exit(-1);

    }
  catch(LataError err)
    {
      cerr<<" error while opening "<<id.name_<< " in file "<<filename <<endl;
      exit(-1);
    }
  return filter.get_geometry(id);
}

// methode � mettre dans lata2dx, commune compare_lata et lata_to_other
DomainUnstructured convertIJKtoUnstructured(const DomainIJK& ijk)
{
  cerr<<"conversion domaijk ";
  DomainUnstructured dom;
  dom.elt_type_=ijk.elt_type_;
  int nx=1,ny=1,nz=1;


  FloatTab& som=dom.nodes_;
  const int dim = ijk.coord_.size();
  nx=ijk.coord_[0].size_array();
  ny=ijk.coord_[1].size_array();
  nz=1;
  if (dim>2) nz=ijk.coord_[2].size_array();

  som.resize(nx*ny*nz,dim);

  for (int i=0; i<nx; i++)
    for (int j=0; j<ny; j++)
      for (int k=0; k<nz; k++)
        {
          int nn=i*ny*nz+j*nz+k;
          som(nn,0)=ijk.coord_[0][i];
          som(nn,1)=ijk.coord_[1][j];
          if (dim>2)
            som(nn,2)=ijk.coord_[2][k];
        }

  IntTab& elems=dom.elements_;
  int nb_som_elem=4;
  if (dim==3)
    {
      nb_som_elem=8;

      elems.resize((nx-1)*(ny-1)*(nz-1),nb_som_elem);

      for (int i=0; i<nx-1; i++)
        for (int j=0; j<ny-1; j++)
          for (int k=0; k<nz-1; k++)
            {
              int nn=i+j*(nx-1)+k*((nx-1)*(ny-1));
              elems(nn,0)=i*ny*nz+j*nz+k;
              elems(nn,1)=(i+1)*ny*nz+j*nz+k;
              elems(nn,2)=(i)*ny*nz+(j+1)*nz+k;
              elems(nn,3)=(i+1)*ny*nz+(j+1)*nz+k;
              assert(elems.dimension(1)>4);
              for (int ii=0; ii<4; ii++) elems(nn,4+ii)=elems(nn,ii)+1;
            }
    }
  else
    {
      nb_som_elem=4;

      elems.resize((nx-1)*(ny-1),nb_som_elem);

      for (int i=0; i<nx-1; i++)
        for (int j=0; j<ny-1; j++)
          {
            int nn=i+j*(nx-1);
            elems(nn,0)=i*ny+j;
            elems(nn,1)=(i+1)*ny+j;
            elems(nn,2)=(i)*ny+(j+1);
            elems(nn,3)=(i+1)*ny+(j+1);
          }
    }
  cerr<<" ok"<<endl;
  return dom;
}


void print(const Noms& geoms)
{
  for (int i=0; i<geoms.size(); i++) cerr<<geoms[i]<<" ";
  cerr<<endl;
}

Motcle get_long_field_name(const Field_UName& field)
{
  Motcle titi(field.get_field_name());
  titi+="_";
  titi+=field.get_localisation();
  titi+="_";
  titi+=field.get_geometry();
  return titi;
}

void print(const Field_UNames& geoms)
{
  for (int i=0; i<geoms.size(); i++) cerr<<get_long_field_name(geoms[i])<<" ";
  cerr<<endl;
}
int compare_temps(double time, double t)
{
  if(t == 0.0 )
    {
      if(fabs(t - time) <= gepsilon) return 1;
    }
  else if(fabs(t - time)/t <= gepsilon)
    {
      return 1;
    }
  return 0;

}

int compare_noms(const Noms& geoms, const Noms& geoms2,Noms& geoms3)
{
  int status=1;
  int size=geoms.size();
  int size2=geoms2.size();
  if (size!=size2)
    status=0;
  for (int i=0; i<size; i++)
    {
      if (geoms2.rang(geoms[i])==-1)
        {
          cerr<<"field "<<(geoms[i])<<" not found in second file"<<endl;
          status= 0;
        }
      else
        {
          geoms3.add(geoms[i]);
        }
    }
  return status;
}
int compare_noms(const Field_UNames& geoms, const Field_UNames& geoms2, Field_UNames& geoms3 )
{
  int status=1;
  int size=geoms.size();
  int size2=geoms2.size();
  if (size!=size2)
    status=0;
  Motcles test(size2);
  for (int i=0; i<size2; i++)
    test[i]=get_long_field_name(geoms2[i]);
  for (int i=0; i<size; i++)
    {
      if (test.rang(get_long_field_name(geoms[i]))==-1)
        {
          cerr<<"field "<<get_long_field_name(geoms[i])<<" not found in second file"<<endl;
          status= 0;
        }
      else
        {
          geoms3.add(geoms[i]);
        }
    }
  return status;
}

void construit_corres(const DomainUnstructured& dom,const DomainUnstructured& dom2,ArrOfInt& ielem,ArrOfInt& iseq)
{

  /* On verifie que tous les sommets sont identiques sur chaque grille */
  xmax = 0.0;
  ymax = 0.0;
  zmax = 0.0;
  const FloatTab& coord1= dom.nodes_;
  const FloatTab& coord2= dom2.nodes_;
  int nb_nodes=coord1.dimension(0);
  int nb_nodes2=coord2.dimension(0);
  const IntTab& connect = dom.elements_;
  int nb_maille=connect.dimension(0);
  const IntTab& connect2 = dom2.elements_;
  int nb_maille2=connect2.dimension(0);
  int dimension=coord1.dimension(1);

  for(int j = 0; j < nb_nodes; j++)
    {
      if(fabs(coord1(j,0)) >= xmax) xmax = fabs(coord1(j,0));
      if(fabs(coord1(j,1)) >= ymax) ymax = fabs(coord1(j,1));
      if (dimension>2)
        if(fabs(coord1(j,2)) >= zmax) zmax = fabs(coord1(j,2));
    }
  // cherche_element


  xp.resize(nb_maille,3);
  int connect_max=connect.dimension(1);
  //int nb_som_elem=0;
  //for (int ii=0; ii<connect_max; ii++)
  //  if (connect(0,ii) != -1) nb_som_elem=ii;
  //nb_som_elem++;
  //cerr<<nb_som_elem;	exit(-1);
  ArrOfInt index_sort(nb_maille);
  // calcul du centre de gravite de la grille seq
  for(int k = 0; k < nb_maille; k++)
    {
      index_sort[k]=k;
      int nb_som_elem=0;
      for (int ii=0; ii<connect_max; ii++)
        {
          int som=connect(k,ii);
	  if (som>=0) { nb_som_elem++;
          xp(k,0)+=coord1(som,0);
          xp(k,1)+=coord1(som,1);
          if (dimension>2)
            xp(k,2)+=coord1(som,2);
	  }
        }
      for (int ii=0; ii<3; ii++)
        xp(k,ii)/=nb_som_elem;
      //cerr<<k<<" xp "<<xp(k,0)<<" "<<xp(k,1)<<" "<<xp(k,2)<<endl;
    }
  Octree_Double octree;

  octree.build_elements(coord1,connect,1e-15,0);

  int nb_elem_1=nb_maille;

  ielem.resize_array(nb_maille2);
  iseq.resize_array(nb_nodes2);
  iseq=-1;
  ielem=-1;

  // pour chaque element de 2 on fait une dichotomie

  for (int j=0; j<nb_maille2; j++)
    {
      //cerr<<((double)j)/contenu2.nb_maille[i2]<<endl;
      //equiv[i2].ielem[j]= -1;
      double pos[3];
      for (int ii=0; ii<3; ii++)
        pos[ii]=0;
      int nb_som_elem=0;
      for (int ii=0; ii<connect_max; ii++)
        {
          int som=connect2(j,ii);
	  if (som>=0) { nb_som_elem++;
          pos[0]+=coord2(som,0);
          pos[1]+=coord2(som,1);
          if (dimension>2)
            pos[2]+=coord2(som,2);
	  }
	}
      for (int ii=0; ii<3; ii++)
        pos[ii]/=nb_som_elem;
      // equiv[i2].ielem[j]= -1;
      int corres_l=-1;


      int index;
      int n=octree.search_elements(pos[0],pos[1],pos[2],index);
      for (int kk=0; kk<n; kk++)
        {
          int k=octree.floor_elements()[index+kk];

          if (!((Ecart(xp(k,0), pos[0], xmax))
                ||(Ecart(xp(k,1), pos[1], ymax))
                ||(Ecart(xp(k,2), pos[2], zmax)))
             )
            {

              corres_l=k;
              break;
            }

          // Cerr<<octree.floor_elements()[k]<<finl;
        }

      if ((Ecart(xp(corres_l,0), pos[0], xmax))
          ||(Ecart(xp(corres_l,1), pos[1], ymax))
          ||(Ecart(xp(corres_l,2), pos[2], zmax)))
        {
          if (0)
            {
              cerr<<j<<" pb  "<<corres_l<<endl;
              for(int ii=0; ii<3; ii++)
                {
                  cerr<<"pos "<<xp(corres_l,ii)<<" "<<pos[ii]<<endl;
                }
              exit(-1);
            }
          corres_l=-1;
        }
      // si on a pas trouve par l amethode optimisee on essaye toutes les mailles , mais avant on essaye le suivant du precedent
      if (corres_l==-1)
        {
          //cerr<<"on cherche l'elt a la main"<<endl;
          int k=j;
          if (j>0)
            k=ielem[j-1]+1;

          if (!((Ecart(xp(k,0), pos[0], xmax))
                ||(Ecart(xp(k,1), pos[1], ymax))
                ||(Ecart(xp(k,2), pos[2], zmax))))
            {
              corres_l=k;
            }
        }
      if (corres_l==-1)
        {

          for (int k=0; k<nb_maille2; k++)
            if (!((Ecart(xp(k,0), pos[0], xmax))
                  || (Ecart(xp(k,1), pos[1], ymax))
                  ||(Ecart(xp(k,2), pos[2], zmax))))
              {
                corres_l=k;
                break;
              }
        }
      ielem[j]=corres_l;
      // On a trouve l element sequentiel associe
      // On cherche maintenant les sommets
      if (corres_l!=-1)
        for (int s_par=0; s_par<nb_som_elem; s_par++)
          {
            int som_par=connect2(j,s_par);
	    if (som_par>=0) {
            if ( iseq[som_par]==-1)
              for (int s_seq=0; s_seq<nb_som_elem; s_seq++)
                {
                  int som_seq=connect(corres_l,s_seq);
		  if (som_seq>=0) {
                  if(!Ecart(coord1(som_seq,0), coord2(som_par,0), xmax))
                    if(!Ecart(coord1(som_seq,1), coord2(som_par,1), ymax))
                      if((dimension<3)||(!Ecart(coord1(som_seq,2), coord2(som_par,2), zmax)))
                        {
                          iseq[som_par]=som_seq;
                          break;
                        }
		  }
                }
	    }
          }
    }



//cerr<<"ouf"<<endl;

  for(int j = 0; j < nb_nodes2; j++)
    {
      if (iseq[j] == -1)
        {
	  //cerr<< " sommet "<<j<<" "<<coord2(j,0)<<" "<<  coord2(j,1) ;
	  //if (dimension==3) cerr<<" "<<coord2(j,2);
	  //cerr<< " pas trouve avec la nouvelle methode"<<endl;
          // le tri sur les xp peut etre un peu faux
          // si on a pas trouve par la methode en log n
          // on essaye la methode en n (mais sur pas trop de case)


          // equiv[i2].iseq[j] = -1;
          int loc=-1;
          for(int k = 0; k < nb_nodes; k++)
            if(!Ecart(coord1(k,0), coord2(j,0), xmax))
              if(!Ecart(coord1(k,1), coord2(j,1), ymax))
                if((dimension<3)||(!Ecart(coord1(k,2), coord2(j,2), zmax)))
                  {
                    loc = k;
                    break;
                  }

          iseq[j]=loc;
          if(iseq[j] == -1)
            {
	     cerr<< " sommet "<<j<<" "<<coord2(j,0)<<" "<<  coord2(j,1) ;
	      if (dimension==3) cerr<<" "<<coord2(j,2);
	      cerr<<endl;
              printf(" Pas trouve le sommet %d Topologie : %s \n",
                     j+1, "oups");
              exit(1);
            }
        }
      /*printf("j i equiv[i2].iseq[j] isto= %d %d %d %d\n", j, i, equiv[i2].iseq[j], isto);*/
    }
  //cerr<<"ouf2"<<endl;

  gemax = 0;         /* Erreur maximum rencontree */
  /* On verifie le nombre de mailles sur chaque grille */

  if(nb_maille != nb_maille2)
    {
      printf(" Le nombre de mailles n'est pas le m�me dans le premier fichier que dans le second\n");
      exit(-1);
    }

  /* On verifie qu'on trouve les m�mes elements sur chaque grille */


  for (int j=0; j<nb_maille2; j++)
    {
      if (ielem[j]==-1)
        {
          if (0)
            cerr<<"element "<<j<<" pas trouve avec la nouvelle methode"<<endl;
          // le tri sur les xp peut etre un peu faux
          // si on a pas trouve par la methode en log n
          // on essaye la methode en n (mais sur pas trop de case)

          //equiv[i2].ielem[j]= -1;
          for(int k = 0; k < nb_maille; k++)
            {
              int m=1;
              for (int ii=0; ii<connect_max; ii++)
                {
                  if (connect(k,ii) == -1) break;
                  if (connect(k,ii) !=
                      iseq[connect2(j,ii)] )
                    {
                      m=0;
                      break;
                    }
                }
              if (m==1)
                {
                  ielem[j]=k;

                  break;
                }
            }

          /*printf("j i equiv[i2].ielem[j] isto= %d %d %d %d\n", j, i, equiv[i2].ielem[j], isto);*/
          if (ielem[j]==-1)
            {
              printf("pas trouve l elt %d\n",j+1);
              for (int ii=0; ii<connect_max; ii++)
                printf("%d ", connect2(j,ii));
              printf("\n");
              exit(-1);
            }
        }
    }
  // cerr<<"ouf3"<<endl;

  xp.resize(0,0);
}

class Ecarts
{
public:
  ArrOfFloat min,max,min2,max2,delta,val,val2;
  ArrOfInt loc,loc2;
  double t;
  void dimensionne(int n)
  {
    min.resize_array(n);
    max.resize_array(n);
    min2.resize_array(n);
    max2.resize_array(n);
    delta.resize_array(n);
    val.resize_array(n);
    loc.resize_array(n);
    val2.resize_array(n);
    loc2.resize_array(n);
  } ;
};
class EcartField
{
public:
  Field_UName name_;
  LataVector<Ecarts> les_ecarts_;
};

void  compare_fields(const FieldFloat& field,const FieldFloat& field2,Ecarts& ecarts,const ArrOfInt& ielem,const ArrOfInt& isom)
{
  if (field.localisation_!=field2.localisation_)
    exit(-1);
  if ((field.localisation_!=LataField_base::ELEM)
      &&(field.localisation_!=LataField_base::SOM ) )
    {
      cerr <<"compare_fields not coded for this localisation "<<LataField_base::localisation_to_string(field.localisation_)<<endl;
      exit(-1);
    }
  const ArrOfInt& renum=(field.localisation_==LataField_base::ELEM?ielem:isom);
  const FloatTab& tab=field.data_;
  int nv=tab.dimension(0);
  const FloatTab& tab2=field2.data_;
  int nv2=tab2.dimension(0);
  int nc=tab.dimension(1);
  ecarts.dimensionne(nc);
  for (int c=0; c<nc; c++)
    {
      float max=-1e30;
      float min=1e30;
      for (int i=0; i<nv; i++)
        {
          const double& val=tab(i,c);
          if (val>max) max=val;
          if (val<min) min=val;
        }
      ecarts.min[c]=min;
      ecarts.max[c]=max;
    }


  for (int c=0; c<nc; c++)
    {
      float max=-1e30;
      float min=1e30;
      float delta=0;
      for (int i=0; i<nv2; i++)
        {
          int iseq=renum[i];
          const float& val=tab(iseq,c);
          const float& val2=tab2(i,c);
          float dval=fabs(val2-val);
          if (val2>max) max=val2;
          if (val2<min) min=val2;
          if (dval>delta)
            {
              delta=dval;
              ecarts.val[c]=val;
              ecarts.val2[c]=val2;
              ecarts.loc[c]=iseq;
              ecarts.loc2[c]=i;
            }
        }
      ecarts.min2[c]=min;
      ecarts.max2[c]=max;
      ecarts.delta[c]=delta;

      //      cerr<<"ici "<<delta<<" " <<min<<" "<<max<<endl;
    }
}
int test_nom_champ_vitesse(const Nom& nom_champ,const char  Y)
{
  if ((strstr(nom_champ,"vitesse")||strstr(nom_champ,"VITESSE")))
    {
      char titi[2]="A";
      titi[0]=Y;
      
      if (strstr(nom_champ,titi))
        {
          return 1;
        }
    }
  return 0;

}
int main(int argc,char **argv)
{

  LataOptions opt,opt2;
  LataDB  lata_db,lata_db2;
  LataFilter  filter,filter2;
  Nom filename,filename2;


  if (argc<3)
    {
      usage(argv[0]);
      exit(-1);
    }
  int dernier_only=0;
  for (int i=3; i<argc; i++)
    {
      if (strcmp(argv[i],"--dernier")==0)
        {
          dernier_only=1;
        }
      else
        {
          if(strcmp(argv[i],"--seuil")==0)
            {
              i++;
              gepsilon_champs=atof(argv[i]);
            }
	  else
	    {
	      if(strcmp(argv[i],"--max_delta")==0)
		{
		  max_delta=1;
		}
	      else
		{
		  cerr<<argv[i]<<" incompris "<<endl;;
		  usage(argv[0]);
		  exit(-1);
		}
	    }
	}
    }
  filename=argv[1];
  filename2=argv[2];
  open(filename,filter,opt,lata_db);
  open(filename2,filter2,opt2,lata_db2);

  // Nom des domaines

  Noms geoms3;

  int gnerr=-1;
  {
    Noms geoms = filter.get_exportable_geometry_names();
    Noms geoms2 = filter2.get_exportable_geometry_names();
    if (compare_noms(geoms,geoms2,geoms3)==0)
      {
        cerr<<filename << " has " <<geoms.size()<<" geometry : ";
        print(geoms);
        cerr<<filename2 << " has " <<geoms2.size()<<" geometry : ";
        print(geoms2);
        cerr<<geoms3.size()<<" geometries communs  in "<<filename <<" and in "<<filename2 <<" ";
        print(geoms3);
        gnerr+=1000;
      }
  }
  LataVector<EcartField> les_ecarts;
  for (entier i = 0; i < geoms3.size(); i++)
    {
      Domain_Id id(geoms3[i], 1, -1);
      Field_UNames fields = filter.get_exportable_field_unames(geoms3[i]);
      Field_UNames fields2 = filter2.get_exportable_field_unames(geoms3[i]);
      Field_UNames fields3 ;
      if (compare_noms(fields,fields2,fields3)==0)
        {
          cerr<<fields.size()<<" fields on "<< geoms3[i]<<" in "<<filename <<" ";
          print(fields);
          cerr<<fields2.size()<<" fields on "<< geoms3[i]<<" in "<<filename2 <<" ";
          print(fields2);
          cerr<<fields3.size()<<" fields communs on "<< geoms3[i]<<" in "<<filename <<" and in "<<filename2 <<" ";
          print(fields3);
          //exit(-1);
          gnerr+=100;
        }
    }
  int nbtimes=filter.get_nb_timesteps();
  // timestep
  if (dernier_only==0)
    {
      int  nbtimes2=filter2.get_nb_timesteps();
      if (nbtimes!=nbtimes2)
        {
          cerr<<filename <<" has "<< nbtimes-1<<" timesteps "<<endl;
          cerr<<filename2 <<" has "<< nbtimes2-1<<" timesteps "<<endl;
          //   exit(1);
	  // pour comparer les n premiers pas de temps
	  if (nbtimes2<nbtimes) 
	    nbtimes=nbtimes2;
          gnerr+=100000;
        }

      for (int i=1; i<nbtimes; i++)
        {
          double t=filter.get_timestep(i);
          double t2=filter2.get_timestep(i);
          if (compare_temps(t,t2)==0)
            {
              cerr<<"timestep "<<i<<" time in "<<filename << " "<<t <<" in "<<filename2<<" "<<t2<<endl;
//	  exit(1);
              gnerr+=10000;
            }
        }
    }

  for (entier i = 0; i < geoms3.size(); i++)
    {
      Domain_Id id(geoms3[i], 1, -1);

      const Domain& dom = get_domain(filter,id,filename);
      const Domain& dom2 = get_domain(filter2,id,filename2);
      ArrOfInt ielem,isom;
      // on construit les tableux de ocnnectivites elem2 -> elem et som2 -> som
      construit_corres(
        ((dom.get_domain_type()==Domain::UNSTRUCTURED)?dom.cast_DomainUnstructured():convertIJKtoUnstructured(dom.cast_DomainIJK())),
        (dom2.get_domain_type()==Domain::UNSTRUCTURED)?dom2.cast_DomainUnstructured():convertIJKtoUnstructured(dom2.cast_DomainIJK()),
        ielem,isom);

      /*
        cerr<<dom.get_domain_type()<<endl;
        cerr<<"ici "<< geoms3[i]<< " "<<dom.cast_DomainUnstructured().elements_.dimension(1)<<endl;
        cerr<<"ici "<< geoms3[i]<< " "<<dom2.cast_DomainUnstructured().elements_.dimension(1)<<endl;
      */

      Field_UNames fields = filter.get_exportable_field_unames(geoms3[i]);
      Field_UNames fields2 = filter2.get_exportable_field_unames(geoms3[i]);
      Field_UNames fields3;
      compare_noms(fields,fields2,fields3);

      for (entier j = 0; j < fields3.size(); j++)
        {
          EcartField& un_ecart= les_ecarts.add();
          un_ecart.name_=fields3[j];
          un_ecart.les_ecarts_.add();
          //double val_max=0;

          for (int i=1; i<nbtimes; i++)
            {
              int t1,t2;
              if (dernier_only==0)
                {
                  t1=t2=i;
                }
              else
                {
                  t1=nbtimes-1;
                  t2=filter2.get_nb_timesteps()-1;
                  i=t1;
                }
              Field_Id id(fields3[j],t1,-1);
              Field_Id id2(fields3[j],t2,-1);
              try
                {
                  const FieldFloat& field = filter.get_float_field(id);
                  //	  cerr<<field.id_.uname_<<field.data_.dimension(1)<<endl;
                  const FieldFloat& field2 = filter2.get_float_field(id2);

                  Ecarts& ecarts=un_ecart.les_ecarts_.add();
                  ecarts.t= filter.get_timestep(t1);
                  compare_fields(field,field2,ecarts,ielem,isom);

                  filter2.release_field(field2);
                  filter.release_field(field);
                }
              catch (LataError err)
                {
                  cerr<<"error loading field"<<endl;
                  exit(-1);
                }
            }
        }


      filter.release_geometry(dom);
      filter2.release_geometry(dom2);

    }



  // on regarde si "par hasard" on n' a pas un vecteur cache...
  for (int j=0; j<les_ecarts.size(); j++)
    {

      const EcartField& un_ecart=les_ecarts[j];

      Nom nom_champ=get_long_field_name(un_ecart.name_);
      int nbc=un_ecart.les_ecarts_[1].min.size_array();
      if (nbc==1)
        {
          int nbv=1;
	  if (j>0)
          if (test_nom_champ_vitesse(nom_champ,'Y'))
            {
              nbv++;
              if (!test_nom_champ_vitesse(les_ecarts[j-1].name_.get_field_name(),'X'))
                nbv--; // On n' a pas la composante X
	      else
		if (j+1<les_ecarts.size())
		  if (test_nom_champ_vitesse(les_ecarts[j+1].name_.get_field_name(),'Z')) nbv++;
              //cerr<<" on a un vecteur a "<<nbv <<" composantes "<<endl;
              // on reprend le max sur les nbv composantes pour tous les temps
              for (int t=1; t<un_ecart.les_ecarts_.size(); t++)
                {
                  double min=1e30,max=-1e30;
                  for (int i=0; i<nbv; i++)
                    {

                      const Ecarts& ecarts=les_ecarts[j-1+i].les_ecarts_[t];
                      // cerr<<t<<" la "<<ecarts.min[0]<<" "<<ecarts.max[0]<<endl;
                      double p=(ecarts.min[0]);
                      if (p<min) min=p;
                      p=fabs(ecarts.max[0]);
                      if (p>max) max=p;
                    }
                  for (int i=0; i<nbv; i++)
                    {
                      Ecarts& ecarts=les_ecarts[j-1+i].les_ecarts_[t];
                      ecarts.max[0]=max;
                      ecarts.min[0]=min;
                    }
                }
              //exit(-1);
            }

        }
    }

  if (les_ecarts.size())
    {
      const EcartField& un_ecart=les_ecarts[0];
     
      int nbtime=un_ecart.les_ecarts_.size();
      ArrOfFloat max_field(les_ecarts.size());
      for (int t=1; t<nbtime; t++)
        for (int j=0; j<les_ecarts.size(); j++)
          {
            double em=0;
            int gnerrf=-1;
            const EcartField& un_ecart=les_ecarts[j];
	    
            // cerr<<un_ecart.name_<<endl;
	    int nbc=un_ecart.les_ecarts_[1].min.size_array();
            float& max=max_field[j];
            {
              const Ecarts& ecarts=un_ecart.les_ecarts_[t];

              for (int c=0; c<nbc; c++)
                {
		  if (max_delta==0)
		    {
                  double p=fabs(ecarts.min[c]);
                  if (p>max) max=p;
                  p=fabs(ecarts.max[c]);
		  if (p>max) max=p;
		    }
		  else
		    {
		      double p=(ecarts.max[c]-ecarts.min[c]+1e-2);
		      if (p>max) max=p;
		    }
                }
              int c0=-1;
              for (int c=0; c<nbc; c++)
                {
                  // cerr<<t<<" " <<ecarts.t<<" "<<max[c]<<" "<<ecarts.delta[c]<<endl;
                  if (Ecart(ecarts.val[c],ecarts.val2[c],max,gepsilon_champs))
                    if(fabs(gerr) >= em )
                      {
                        em = gerr;
                        c0=c;
                      }
                  //	      cerr<<gerr<< " ici  "<<gemax<<" "<<un_ecart.name_<<" " <<ecarts.t<<endl;
                }
              if (em>0.)
                {
                  int c=c0;
                  Nom type_case(un_ecart.name_.get_localisation());

                  printf("Ecarts pour %s au temps:%15.8e Erreur max:%10.3e %s seq %d=%15.8e %s par %d=%15.8e  gmax %15.8e composante %d\n",
                         (const char *)get_long_field_name(un_ecart.name_), ecarts.t,em,(const char*)type_case, ecarts.loc[c],  ecarts.val[c] , (const char*)type_case, ecarts.loc2[c], ecarts.val2[c],max,c);
                  gnerr++;
                  gnerrf++;
                  em = 0.0;
                }
              else if (gnerrf!=-1)
                {
                  printf("    OK pour %s au temps:%15.8e\n",
                         (const char *)get_long_field_name(un_ecart.name_) , ecarts.t);
                }

            }

          }
    }
  gnerr++;
  printf("Number of differences : %d\n", gnerr);
  printf("Maximal relative error encountered : %10.5e\n", gemax);
  return gnerr;
}
