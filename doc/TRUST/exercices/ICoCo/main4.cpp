#include <Problem.h>
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"

#include "TrioDEC.hxx"
#include <set>
#include <time.h>
#include <ICoCoTrioField.h>
#include <fstream>
#include <string.h>

using namespace MEDCoupling;
using namespace std;
using namespace ICoCo;

//utility methods for synchronizing
//data from the the two trio instance
typedef enum {sync_and,sync_or} synctype;
void synchronize_bool(bool& stop, synctype s)
{
  int my_stop;
  int my_stop_temp = stop?1:0;
        
  if (s==sync_and)
    MPI_Allreduce(&my_stop_temp,&my_stop,1,MPI_INTEGER,MPI_MIN,MPI_COMM_WORLD);
  else if (s==sync_or)
    MPI_Allreduce(&my_stop_temp,&my_stop,1,MPI_INTEGER,MPI_MAX,MPI_COMM_WORLD);
  else
     throw;
  stop =(my_stop==1);
}

void synchronize_dt(double& dt)
{
  double dttemp=dt;
  MPI_Allreduce(&dttemp,&dt,1,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
}

void modifie_n_TrioDEC(TrioDEC& dec)
{
   dec.setNature(IntensiveMaximum);
}
int main(int argc,char **argv) {

  MPI_Init(&argc,&argv);

  {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  
  if (size <2) {  cout << "pb: must run on  2 procs or +!"<<endl; exit(1); }
  CommInterface comm;
  set<int> dom1_ids;
  set<int> dom2_ids;
  dom1_ids.insert(0);
  dom1_ids.insert(1);
  dom2_ids.insert(2);
  dom2_ids.insert(3);

  MPIProcessorGroup dom2_group(comm,dom2_ids);
  MPIProcessorGroup dom1_group(comm,dom1_ids);

  Problem* T;
  std::string data_file;
  const MPI_Comm* mpicomm=0; 
  if (dom2_group.containsMyRank()) 
    {
      if (!freopen("docond_VEF_3D_dom2.out","w",stdout)) abort();
      if (!freopen("docond_VEF_3D_dom2.err","w",stderr)) abort();
      data_file="docond_VEF_3D_dom2";
      mpicomm=dom2_group.getComm();
    }
  else if (dom1_group.containsMyRank()) 
    {
      if (!freopen("docond_VEF_3D_dom1.out","w",stdout)) abort();
      if (!freopen("docond_VEF_3D_dom1.err","w",stderr)) abort();
      data_file="docond_VEF_3D_dom1";
      mpicomm=dom1_group.getComm();
    }
  else 
    throw 0;
  T = getProblem();
  T->setDataFile(data_file);
  T->setMPIComm((void*)mpicomm);
  T->initialize();

  vector<string> outputnames= T->getOutputFieldsNames();
  for (unsigned int ii=0;ii<outputnames.size();ii++)
    cout<<data_file<<"  Field Output " <<outputnames[ii]<<endl;
  cout<<endl;

  vector<string> inputnames= T->getInputFieldsNames();
  for (unsigned int ii=0;ii<inputnames.size();ii++)
    cout<<data_file<<"  Field Input  " <<inputnames[ii]<<endl;
  cout<<endl;

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true; // Is the time interval successfully solved ?

  synchronize_bool(stop,sync_or);

  double* save_old_field=0;

  clock_t clock0= clock ();
  int compti=0;

  bool init=true; // first time step ??

  // loop on time steps
  while (!stop) {
    compti++;
    clock_t clocki= clock ();
    cout << compti << " CLOCK " << (clocki-clock0)*1.e-6 << endl; 

    ok=false; // Is the time interval successfully solved ?

    // Loop on the time interval tries
    while (!ok && !stop) {
      // Compute the first time step length
      double dt=T->computeTimeStep(stop);
      synchronize_dt(dt);
      synchronize_bool (stop, sync_or);
      if (stop) // Impossible to solve the next time step, the Problem
        // has given up
        break;

      if (rank==0) cout << " chosen dt" << dt <<endl;

      // Prepare the next time step
      T->initTimeStep(dt);
      clock_t clock_avt_resolution= clock ();

      // Solve the next time step
      ok=T->solveTimeStep();

      clock_t clock_ap_resolution= clock ();
      cout << compti << " TEMPS DE RESOLUTION DU PB (s) :  " << (clock_ap_resolution-clock_avt_resolution)*1.e-6 << endl; 

      init=false;

      synchronize_bool(ok,sync_and);
      if (!ok) // The resolution failed, try with a new time interval.
        T->abortTimeStep();

      else // The resolution was successful, validate and go to the
        // next time step.
        T->validateTimeStep();
    }

    // Stop the resolution if the Problem is stationnary
    bool stat=T->isStationary();
    synchronize_bool(stat, sync_and);
    if (stat)
      stop=true;
  }
  
  T->terminate();
  delete T;
  if (save_old_field!=0) 
    delete [] save_old_field;

  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
