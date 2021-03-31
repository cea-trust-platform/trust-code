#include <ICoCoProblem.h>
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"

#include "TrioDEC.hxx"
#include <set>
#include <time.h>

#undef MM 
#ifdef MM
#include <ICoCoMEDDoubleField.hxx>
#include <ICoCoProblem.h>
#else
#include <ICoCoTrioField.h>
#endif

#include <fstream>
#include <string.h>

using namespace MEDCoupling;
using namespace std;
using namespace ICoCo;

// Utility methods for synchronizing data
// from the the two trio instance

typedef enum {sync_and,sync_or} synctype;

void synchronize_bool(bool& stop, synctype s)
{
  int my_stop=-1;
  int my_stop_temp=stop?1:0;
  if (s==sync_and)
    MPI_Allreduce(&my_stop_temp,&my_stop,1,MPI_INTEGER,MPI_MIN,MPI_COMM_WORLD);
  else if (s==sync_or)
    MPI_Allreduce(&my_stop_temp,&my_stop,1,MPI_INTEGER,MPI_MAX,MPI_COMM_WORLD);
  stop=(my_stop==1);
}

void synchronize_dt(double& dt)
{
  double dttemp=dt;
  MPI_Allreduce(&dttemp,&dt,1,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
}

void modifie_n_TrioDEC(TrioDEC& dec)
{
  dec.setNature(IntensiveMaximum); // Mot cle MEDCoupling pour le calcul par integration du debit par exemple
}



int main(int argc,char **argv) {

  // Initialisation of MPI and creation of processor groups

  MPI_Init(&argc,&argv);

  // Begin
  {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int nproc_dom=1; // nb of procs specified in the dom.data file // + ICoCo couplage
    if (size !=nproc_dom) {
      cout << "pb: must run on "<<nproc_dom <<" processors !"<<endl;
      exit(1);
    }

    CommInterface comm;
    set<int> dom_ids;
    int n=0;
    for (n=0;n<nproc_dom;n++)
      dom_ids.insert(n);

    MPIProcessorGroup dom_group(comm,dom_ids);

    // Initialisation of Trio_U
    std::string data_file;
    const MPI_Comm* mpicomm=0;
    if (dom_group.containsMyRank()) {
      data_file="test"; // name of the first dom.data file // + ICoCo couplage
      // Redirection des sorties couplage dans dom.out et dom.err
      if (!freopen("test.out","w",stdout)) abort();         // + ICoCo couplage
      if (!freopen("test.err","w",stderr)) abort();         // + ICoCo couplage
      mpicomm=dom_group.getComm();
    }
    else
      throw 0;

    // Instanciation of Trio_U
    Problem* T;
    T=getProblem(); // new ProblemTrio;

    T->setDataFile(data_file);
    T->setMPIComm((void*)mpicomm);
    T->initialize();

    cout<<endl;
    cout << "######################" << endl;
    cout << data_file << " successfully initialised" << endl;
    cout << "Listing Output Field Names:" << endl;
    vector<string> outputnames=T->getOutputFieldsNames();
    for (unsigned int ii=0;ii<outputnames.size();ii++)
      cout<<data_file<<"  Field Output " <<outputnames[ii]<<endl;
    cout << "Listing Input Field Names:" << endl;
    vector<string> inputnames=T->getInputFieldsNames();
    for (unsigned int ii=0;ii<inputnames.size();ii++)
      cout<<data_file<<"  Field Input  " <<inputnames[ii]<<endl;
    cout << "######################" << endl;

    // Field transferred
#ifdef MM
    MEDDoubleField field_power;
#else
    TrioField field_power;
#endif

    bool stop=false; // Does the Problem want to stop ?
    bool ok=true;    // Is the time interval successfully solved ?

    synchronize_bool(stop,sync_or);

    // Boucle sur les pas de temps
    while (!stop) {

      ok=false; // Time interval not yet successfully solved

      // Loop on the time interval tries
      while (!ok && !stop) {

        // Compute the time step length
        // and check if the Problem wants to stop
        double dt=T->computeTimeStep(stop);
        synchronize_dt(dt);
        synchronize_bool(stop, sync_or);
        if (stop) // Impossible to solve the next time step, the Problem has given up
          break;

        // Prepare the next time step
        T->initTimeStep(dt);

        // Perform field exchange
        {
          ////////////////////////////////////////////////////
          // On renseigne le maillage des champs a recevoir.//
          ////////////////////////////////////////////////////
          cout<<endl;
          cout << "######################" << endl;
          cout << "Getting Output Fields" << endl;

          cout << "Getting Input Field Templates" << endl;
          if (dom_group.containsMyRank()) {
#ifdef MM
            T->getInputMEDDoubleFieldTemplate("POWERDENSITY_CORE",field_power);     // cf. "Boundary_Conditions" block in dom.data // + ICoCo couplage
#else
            T->getInputFieldTemplate("POWERDENSITY_CORE",field_power);     // cf. "Boundary_Conditions" block in dom.data // + ICoCo couplage
#endif
          }

          ////////////////////////////////////////////////////////////////////////
          // On receptionne les champs en appliquant ou non une sous-relaxation.//
          ////////////////////////////////////////////////////////////////////////
          cout << "Setting Input Fields" << endl;
          if (dom_group.containsMyRank() ) {
#ifdef MM
            //int nbcase=field_power.getMCField()->getArray()->getNumberOfTuples();
            //for(int i=0;i<nbcase;i++) {
            //    field_power.getMCField()->getArray()->setIJ(i,0,25.);
            //}
            field_power.getMCField()->getArray()->fillWithValue(25.);
            T->setInputMEDDoubleField("POWERDENSITY_CORE",field_power);           // cf. "Boundary_Conditions" block in dom.data // + ICoCo couplage
#else
            field_power.set_standalone();
            int nbcase=field_power.nb_values()*field_power._nb_field_components;
            for(int i=0;i<nbcase;i++) {
                field_power._field[i]=25.;
            }
            T->setInputField("POWERDENSITY_CORE",field_power);           // cf. "Boundary_Conditions" block in dom.data // + ICoCo couplage
#endif
          }
          cout << "######################" << endl;

        } // End perform field exchange

        // Solve the next time step
        ok=T->solveTimeStep();
        synchronize_bool(ok,sync_and);
        if (!ok) // The resolution failed, try with a new time interval.
          T->abortTimeStep();
        else // The resolution was successful, validate and go to the next time step.
          T->validateTimeStep();

      } // End loop on the time interval tries

      // Stop the resolution if the Problem is stationnary
      bool stat=T->isStationary();
      synchronize_bool(stat, sync_and);
      if (stat)
        stop=true;

    } // Fin boucle sur les pas de temps

    T->terminate();
    delete T;

  } // End

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
} // end main
