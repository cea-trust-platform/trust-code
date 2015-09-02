#include <Problem.h>
#include <ICoCoTrioField.h>
#include <dlfcn.h>
#include <iostream>
#include <vector>
#include <fpu_control.h>
#include <mpi.h>
#include <Init_Params.h>

#include <assert.h>
#ifdef _libtrio_
#define _dl__
#endif

using namespace std;
using ICoCo::Problem;
using ICoCo::Init_Params;
using ICoCo::TrioField;
#ifndef _dl__
extern "C"  Problem* getProblem();
#endif
// Verifier la boucle...

int main(int argc, char** argv) {
  MPI_Init(&argc,&argv);
  {
  fpu_control_t cw = _FPU_DEFAULT & ~(_FPU_MASK_IM | _FPU_MASK_ZM | _FPU_MASK_OM);
  _FPU_SETCW(cw);
  Init_Params my_data;
  my_data.data_file="U_in_var_impl_ICoCo.data";
  my_data.problem_name="pb";
  my_data.comm=MPI_COMM_WORLD;
#ifdef _dl__
  cout <<" dlopen "<<endl;
  void* handle_Trio;
  Problem *(*getProblem_Trio)();
  //  handle_Trio=dlopen("../lib/_Trio_UModule_opt.so", RTLD_LAZY | RTLD_LOCAL);
  handle_Trio=dlopen(_libtrio_, RTLD_LAZY | RTLD_LOCAL);
  if (!handle_Trio) {
    cout << dlerror() << endl;
    throw 0;
  }
 // getProblem=(Problem* (*)(void*))dlsym(handle_Trio, "getProblem");
  *(void **)(&getProblem_Trio)=dlsym(handle_Trio, "getProblem");
  if (!getProblem_Trio) {
    cout << dlerror() << endl;
    throw 0;
  }
  
  

  Problem *T=(*getProblem_Trio)();
  dlclose(handle_Trio);
#else
  cout <<" no dlopen "<<endl;
   Problem *T=getProblem();
#endif
   T->setDataFile(my_data.data_file);
  T->initialize();


  vector<string> outputnames= T->getOutputFieldsNames();
  for (unsigned int ii=0;ii<outputnames.size();ii++)
    cout<<my_data.data_file<<"  Field Output " <<outputnames[ii]<<endl;
  cout<<endl;

  bool stop=false;  // Trio wants to stop
  bool ok=true;     // Trio transient runned OK

  while (1) {                     // Loop on timesteps
    
      
      double dt=T->computeTimeStep(stop);
      if (stop)
	break;

      T->initTimeStep(dt);

      // Exemple d'output
      if (1)
      {
	TrioField f;
	T->getOutputField("vitesse_sortie",f);
       	cout <<  f.getName()<<" Output value " << *f._field << endl;
	for (int nb_val=0;nb_val<f._nb_elems;nb_val++)
	  for (int comp=0;comp<f._nb_field_components;comp++)
	    cout<<nb_val<<" "<<comp<<" "<<f._field[nb_val*f._nb_field_components+comp]<<endl;
	cout <<"mesh_dim "<<f._mesh_dim<<endl;
      }
   
      if (1)
      {
	TrioField f;
	T->getOutputField("vitesse_elem_sortie",f);
       	cout <<  f.getName()<<" Output value " << *f._field << endl;
	for (int nb_val=0;nb_val<f._nb_elems;nb_val++)
	  for (int comp=0;comp<f._nb_field_components;comp++)
	    cout<<nb_val<<" "<<comp<<" "<<f._field[nb_val*f._nb_field_components+comp]<<endl;
      }
      if (1)
      {
	TrioField f;
	T->getOutputField("viscosite_dynamique_elem_sortie",f);
       	cout << f.getName()<<" Output value " << *f._field << endl;
	for (int nb_val=0;nb_val<f._nb_elems;nb_val++)
	  for (int comp=0;comp<f._nb_field_components;comp++)
	    cout<<nb_val<<" "<<comp<<" "<<f._field[nb_val*f._nb_field_components+comp]<<endl;
      }
      if (0)
      {
	// champ aux faces
	TrioField f;
	T->getOutputField("VITESSE_NATIF_dom",f);
       	cout <<  f.getName()<<" Output value " << *f._field << endl;
	for (int nb_val=0;nb_val<f._nb_elems;nb_val++)
	  for (int comp=0;comp<f._nb_field_components;comp++)
	    cout<<nb_val<<" "<<comp<<" "<<f._field[nb_val*f._nb_field_components+comp]<<endl;
	
      }
      // Exemple d'input
      if (1)
      {
	vector<string> sv=T->getInputFieldsNames();
	cout << "Input Fields" << " :" << endl;
	for (unsigned i=0;i<sv.size();i++)
	  cout << i << ". " << sv[i] << endl;

	TrioField afield;
	int nb_elems,nb_comp;

	// Get the right geometry & nbcomp for "pression_sortie"
	T->getInputFieldTemplate("pression_sortie",afield);
	// Allocate memory for the values (size=nb_elems*nb_comp)
	afield.set_standalone();
	// Fill the values
	nb_elems=afield._nb_elems;
	nb_comp=afield._nb_field_components;
	assert(nb_comp==1);
	for (int i=0;i<nb_elems;i++)
	  afield._field[i]=0;
	// Give the field to the problem
	T->setInputField("pression_sortie",afield);

	// Get the right geometry & nbcomp for "vitesse_entree"
	T->getInputFieldTemplate("vitesse_entree",afield);
	// Allocate memory for the values (size=nb_elems*nb_comp)
	afield.set_standalone();
	// Fill the values
	nb_elems=afield._nb_elems;
	nb_comp=afield._nb_field_components;
	assert(nb_elems==1); // ch_front_input_uniforme in the data file
	double t=T->presentTime();
	t+=dt;
	double vx=(t>100)?100:t;
	for (int i=0;i<nb_elems;i++)
	  for (int j=0;j<afield._nb_field_components;j++)
	    afield._field[i*nb_comp+j]=(j==0)?vx:0;
	// Give the field to the problem
	T->setInputField("vitesse_entree",afield);
      }


      ok=T->solveTimeStep();

      if (!ok) {
	T->abortTimeStep();
      }
      else {
	T->validateTimeStep();
      }

  }                                   // End loop on timesteps
  
  T->terminate();
  cerr<<"Arret des processes."<<endl;
  delete T;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;

}
