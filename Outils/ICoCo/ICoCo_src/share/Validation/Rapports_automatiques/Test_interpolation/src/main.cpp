#include <string>
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"
#include "DEC.hxx"
#include "TrioDEC.hxx"
#include <set>
#include <time.h>
#include "ICoCoTrioField.h"
#include <iostream>
#include <assert.h>
#include <math.h>
#include <stdio.h>

using namespace std;
using namespace MEDCoupling;
using namespace ICoCo;

//utility methods for synchronizing
//data from the the two trio instance

void affiche( const TrioField&   field,const char* name)
{
  cout <<name<<" "<<field.getName()<<endl<<flush;
  for (int ele=0;ele<field._nb_elems;ele++)
    cout <<name<< " "<<ele <<": "<<field._field[ele]<<endl<<flush;;
  
}

void remplit_coord(double* coords)
{
  double angle,epaisseur;
  angle=0*45*(asin(1)/90);
  epaisseur=1e-0;
  coords[0*3+0]=0.;
  coords[0*3+1]=0.;
  coords[0*3+2]=0.;
  
  coords[1*3+0]=cos(angle);
  coords[1*3+1]=0.;
  coords[1*3+2]=sin(angle);
  
    
  coords[2*3+0]=-sin(angle);
  coords[2*3+1]=0.;
  coords[2*3+2]=cos(angle);
  
  for (int d=0;d<3;d++)
    coords[3*3+d]=coords[1*3+d]+ coords[2*3+d];
  
  for (int i=4;i<8;i++)
    {
      for (int d=0;d<3;d++)
	coords[i*3+d]=coords[(i-4)*3+d];
      coords[i*3+1]+=epaisseur;
    }

}

void init_quad(TrioField& champ_quad,int is_master)
{
  
  champ_quad.setName("champ_quad");
  champ_quad._space_dim=3;
  champ_quad._mesh_dim=2;
  champ_quad._nodes_per_elem=4;
  champ_quad._itnumber=0;
  champ_quad._time1=0;
  champ_quad._time2=1;
  champ_quad._nb_field_components=1;

  if (is_master)
    {
      champ_quad._nbnodes=8;
      champ_quad._nb_elems=2;
      
      champ_quad._coords=new double[champ_quad._nbnodes*champ_quad._space_dim];
      //memcpy(afield._coords,sommets.addr(),champ_quad._nbnodes*champ_quad._space_dim*sizeof(double));
      
      remplit_coord(champ_quad._coords);
  
  
      champ_quad._connectivity=new int[champ_quad._nb_elems*champ_quad._nodes_per_elem];
      champ_quad._connectivity[0*champ_quad._nodes_per_elem+0]=0;
      champ_quad._connectivity[0*champ_quad._nodes_per_elem+1]=1;
      champ_quad._connectivity[0*champ_quad._nodes_per_elem+2]=2;
      champ_quad._connectivity[0*champ_quad._nodes_per_elem+3]=3;
      champ_quad._connectivity[1*champ_quad._nodes_per_elem+0]=4;
      champ_quad._connectivity[1*champ_quad._nodes_per_elem+1]=5;
      champ_quad._connectivity[1*champ_quad._nodes_per_elem+2]=6;
      champ_quad._connectivity[1*champ_quad._nodes_per_elem+3]=7;
      
    }
  else
    {
      champ_quad._nbnodes=0;
      champ_quad._nb_elems=0;
      champ_quad._coords=new double[champ_quad._nbnodes*champ_quad._space_dim];
    
    }
  champ_quad._has_field_ownership=false;
  champ_quad._field=0;
  //champ_quad._field=new double[champ_quad._nb_elems];
  //  assert(champ_quad._nb_field_components==1);
}
void init_triangle(TrioField& champ_triangle,int is_master)
{
   
  champ_triangle.setName("champ_triangle");
  champ_triangle._space_dim=3;
  champ_triangle._mesh_dim=2;
  champ_triangle._nodes_per_elem=3;
  champ_triangle._itnumber=0;
  champ_triangle._time1=0;
  champ_triangle._time2=1;
  champ_triangle._nb_field_components=1;

  if (is_master)
    {
      champ_triangle._nb_elems=4;
      champ_triangle._nbnodes=8;
    
      champ_triangle._coords=new double[champ_triangle._nbnodes*champ_triangle._space_dim];
      //memcpy(afield._coords,sommets.addr(),champ_triangle._nbnodes*champ_triangle._space_dim*sizeof(double));
      remplit_coord(champ_triangle._coords);
      
      champ_triangle._connectivity=new int[champ_triangle._nb_elems*champ_triangle._nodes_per_elem];
      champ_triangle._connectivity[0*champ_triangle._nodes_per_elem+0]=0;
      champ_triangle._connectivity[0*champ_triangle._nodes_per_elem+1]=1;
      champ_triangle._connectivity[0*champ_triangle._nodes_per_elem+2]=2;
      champ_triangle._connectivity[1*champ_triangle._nodes_per_elem+0]=1;
      champ_triangle._connectivity[1*champ_triangle._nodes_per_elem+1]=2;
      champ_triangle._connectivity[1*champ_triangle._nodes_per_elem+2]=3;
      
      champ_triangle._connectivity[2*champ_triangle._nodes_per_elem+0]=4;
      champ_triangle._connectivity[2*champ_triangle._nodes_per_elem+1]=5;
      champ_triangle._connectivity[2*champ_triangle._nodes_per_elem+2]=7;
      champ_triangle._connectivity[3*champ_triangle._nodes_per_elem+0]=4;
      champ_triangle._connectivity[3*champ_triangle._nodes_per_elem+1]=6;
      champ_triangle._connectivity[3*champ_triangle._nodes_per_elem+2]=7;
    }
  else
    {
      champ_triangle._nb_elems=0;
      champ_triangle._nbnodes=0;
      champ_triangle._coords=new double[champ_triangle._nbnodes*champ_triangle._space_dim];
    
    }
  champ_triangle._has_field_ownership=false;
  // champ_triangle._field=new double[champ_triangle._nb_elems];
  champ_triangle._field=0;
  
}


void main2 (int argc,char **argv) {
  int num_cas=0;
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  
  int is_master=0;

  CommInterface comm;
  set<int> emetteur_ids;
  set<int> recepteur_ids;
  emetteur_ids.insert(0);
  if ((size <2)||(size>4)) {  cout << "pb: must run on 2-4 procs !"<<endl; exit(1); }

  recepteur_ids.insert(1);
  if (size >2) 
    recepteur_ids.insert(2);
  if (size >3) 
    emetteur_ids.insert(3);
  if ((rank==0)||(rank==1)) 
    is_master=1;
  
  MPIProcessorGroup recepteur_group(comm,recepteur_ids);
  MPIProcessorGroup emetteur_group(comm,emetteur_ids);


  string cas;
  if (recepteur_group.containsMyRank())
    {
      cas="recepteur";
      if (is_master)
	{
	  if (!freopen("recepteur.out","w",stdout)) abort();
	  if (!freopen("recepteur.err","w",stderr)) abort();
	}
      
    }
  else
    {
      cas="emetteur";
      if (is_master)
	{
	  if (!freopen("emetteur.out","w",stdout)) abort();
	  if (!freopen("emetteur.err","w",stderr)) abort();
	}
    }

  for (int send=0;send<2;send++)
    for (int rec=0;rec<2;rec++)
    {
  TrioDEC dec_emetteur(emetteur_ids, recepteur_ids);
#ifndef med_old
  dec_emetteur.setOrientation(2);
  dec_emetteur.setMaxDistance3DSurfIntersect(1e-3);
#endif
  TrioField champ_emetteur, champ_recepteur;
   
  if (send==0)
    init_quad(champ_emetteur,is_master);
  else
    init_triangle(champ_emetteur,is_master);
  //init_triangle(champ_emetteur);
  if (rec==0)
    init_triangle(champ_recepteur,is_master);
  else
    init_quad(champ_recepteur,is_master);
  
  
    
  if (cas=="emetteur") 
    {
      champ_emetteur._field=new double[champ_emetteur._nb_elems];
      for (int ele=0;ele<champ_emetteur._nb_elems;ele++)
	champ_emetteur._field[ele]=1;
      
      champ_emetteur._has_field_ownership=true;
    }
  
  
  MPI_Barrier(MPI_COMM_WORLD);

  clock_t clock0= clock ();
  int compti=0;

  bool init=true; // first time step ??
  bool stop=false;
  //boucle sur les pas de quads
  while (!stop) {
  
    compti++;
    clock_t clocki= clock ();
    cout << compti << " CLOCK " << (clocki-clock0)*1.e-6 << endl; 
    for (int non_unif=0;non_unif<2;non_unif++)
      {
	// if (champ_recepteur._field)
	//   delete [] champ_recepteur._field;
	champ_recepteur._field=0;
	// champ_recepteur._has_field_ownership=false;
  

  
	if (cas=="emetteur") 
	  {
	    if (non_unif)
	     if (champ_emetteur._nb_elems>0)
	      champ_emetteur._field[0]=40;
	    cout<<"emetteur cas num "<<num_cas<<endl;
	    affiche(champ_emetteur," emetteur ");
	  }
	//bool ok=false; // Is the time interval successfully solved ?
    
	// Loop on the time interval tries
	if (1)
	 {
      

	  if (cas=="emetteur")
	    dec_emetteur.attachLocalField( &champ_emetteur);
	  else
	    dec_emetteur.attachLocalField(&champ_recepteur);
#ifndef med_old
          dec_emetteur.setNature(IntensiveMaximum);
#endif
	  if(init) dec_emetteur.synchronize();
	  init=false;

	  if (cas=="emetteur") {
	    //    affiche(champ_emetteur);
	    dec_emetteur.sendData();
	  }
	  else if (cas=="recepteur") {
	    dec_emetteur.recvData();
	    if (is_master)
	      {
		cout<<"recepteur cas num "<<num_cas<<endl;
		affiche(champ_recepteur," recepteur ");
	      }
	  }
	  else
	    throw 0;
	  MPI_Barrier(MPI_COMM_WORLD);
	}
	stop=true;
	num_cas++;
      }
    // destruction des champs, des DEC, et des tableaux associes

  }
    }   
}

int main(int argc,char **argv) {

  MPI_Init(&argc,&argv);

  main2(argc,argv);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  cerr<<"Arret des processes."<<endl;
  return 0;
}
