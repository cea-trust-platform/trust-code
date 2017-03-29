#include <Problem.h>
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"

#include "TrioDEC.hxx"
#include <set>


#define MM 
#ifdef MM
#include <ICoCoMEDField.hxx>
#else
#include <ICoCoTrioField.h>
#endif

using namespace MEDCoupling;
using namespace std;
using namespace ICoCo;

// Utility methods for synchronizing data
// from the the two trio instances

int mpi_min(int i) {
  int result;
  MPI_Allreduce(&i,&result,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
  return result;
}
int mpi_max(int i) {
  int result;
  MPI_Allreduce(&i,&result,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  return result;
}
double mpi_min(double i) {
  double result;
  MPI_Allreduce(&i,&result,1,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
  return result;
}
double mpi_max(double i) {
  double result;
  MPI_Allreduce(&i,&result,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);
  return result;
}
bool mpi_and(bool b) {
  int i=b?1:0;
  int result;
  MPI_Allreduce(&i,&result,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
  return (result!=0);
}
bool mpi_or(bool b) {
  int i=b?1:0;
  int result;
  MPI_Allreduce(&i,&result,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  return result;
}



int main(int argc,char **argv) {

  // Initialisation of MPI and creation of processor groups

  MPI_Init(&argc,&argv);

  {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  
  if (size !=4) {  cout << "pb: must run on 4 procs !"<<endl; exit(1); }

  CommInterface comm;
  set<int> boite_ids;
  set<int> canal_ids;
  boite_ids.insert(0);
  boite_ids.insert(1);
  canal_ids.insert(2);
  canal_ids.insert(3);
  MPIProcessorGroup boite_group(comm,boite_ids);
  MPIProcessorGroup canal_group(comm,canal_ids);

  // Instanciation of Trio_U
  Problem *T = getProblem(); // new ProblemTrio;
  
  // Initialisation of Trio_U
  std::string data_file;
  const MPI_Comm* mpicomm=0; 
  if (boite_group.containsMyRank()) {
    data_file="boite";
    mpicomm=boite_group.getComm();
  }
  if (canal_group.containsMyRank()) { 
    data_file="canal";
    mpicomm=canal_group.getComm();
  }
  
  //T->set_data(&params);
  T->setDataFile(data_file);
  T->setMPIComm((void*)mpicomm);
   
  T->initialize();


  vector<string> outputnames= T->getOutputFieldsNames();
  for (unsigned int ii=0;ii<outputnames.size();ii++)
    cout<<data_file<<"  Field Output " <<outputnames[ii]<<endl;
  cout<<endl;
  
  T->getInputFieldsNames();
  vector<string> inputnames= T->getInputFieldsNames();
  for (unsigned int ii=0;ii<inputnames.size();ii++)
    cout<<data_file<<" Field Input " <<inputnames[ii]<<endl;
  
  // Creation of an interpolator from boite procs to canal procs
  // If several fields are to be exchanged, create one DEC for each
#ifndef testvector
  TrioDEC dec(boite_ids, canal_ids);
#else
  vector<TrioDEC> decu(1);
  decu[0]=TrioDEC(boite_ids, canal_ids);
  TrioDEC& dec=decu[0];
#endif
 
#ifdef MM
  MEDCouplingFieldDouble* field_vitesse=0;
#else
#ifndef testvector
  TrioField vitesse;
#else
  
  vector<TrioField> tt(1);
  TrioField& vitesse=tt[0];
#endif
#endif

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true;    // Is the time interval successfully solved ?
  bool init=true;  // Is this the first use of dec ? (only one call to synchronize)

  //boucle sur les pas de temps
  while (!stop) {
    
    ok=false; // Time interval not yet successfully solved
    
    // Loop on the time interval tries
    while (!ok && !stop) {
      
      // Compute the time step length
      // and check if the Problem wants to stop
      double dt=T->computeTimeStep(stop);
      dt=mpi_min(dt);
      stop=mpi_or(stop);
      if (stop)
	break;

      // Prepare the next time step
      T->initTimeStep(dt);
      

      // Perform field exchange
      {			      	
	// Receiver side
	if (canal_group.containsMyRank()) {
	  
	  // Get the mesh on which the field is wanted
	
#ifdef MM
	  cout <<" medcouplindfielddouble"<<endl;
	  field_vitesse= T->getInputMEDFieldTemplate("vitesse_entree");
	  dec.attachLocalField(field_vitesse);
	  
#else
	  T->getInputFieldTemplate("vitesse_entree",vitesse);
	  dec.attachLocalField( &vitesse);
#endif	  
	  // Compute intersections if necessary 
	  if (init) {
	    dec.synchronize();
	    init=false;
	  }

	  // Receive field from boite
	  dec.recvData();
#ifdef MM
	  T->setInputMEDField("vitesse_entree",field_vitesse);
#else
	  T->setInputField("vitesse_entree",vitesse);
#endif
	}
	
	// Sender side
	if (boite_group.containsMyRank()) {
	  
	  // Get the output field
	 
#ifdef MM
	  cout <<" medcouplindfielddouble"<<endl;
	  field_vitesse=T->getOutputMEDField("VITESSE_ELEM_boite_boundaries_perio");
	  //dec.attachLocalField(parafield);
	  dec.attachLocalField(field_vitesse);
#else 
	  T->getOutputField("VITESSE_ELEM_boite_boundaries_perio",vitesse);
	  dec.attachLocalField( &vitesse);
#endif	  
	  // Compute intersections if necessary 
	  if (init) {
	    dec.synchronize();
	    init=false;
	  }
	  
	  // Send field to canal
	  dec.sendData();
	}
      
#ifdef MM
	if (field_vitesse)
	  field_vitesse->decrRef();
	
#endif
      }
      // Solve the next time step
      ok=T->solveTimeStep();
      ok=mpi_and(ok);
      if (!ok) // The resolution failed, try with a new time interval.
	T->abortTimeStep();      
      else // The resolution was successful, validate and go to the next time step.
	T->validateTimeStep();

    }

    // Stop the resolution if the Problem is stationnary
    bool stat=T->isStationary();
    stat=mpi_and(stat);
    if (stat)
      stop=true;
    
  }
  
  T->terminate();
  delete T;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
