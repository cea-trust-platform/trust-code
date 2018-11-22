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

int main(int argc,char **argv) {
  MPI_Init(&argc,&argv);
  {
    Problem* T;
    std::string data_file;
    data_file="Vahl_Davis_hexa_ICoCo"; // name of the first dom.data file // + ICoCo couplage
    // Redirection des sorties couplage dans dom.out et dom.err
    if (!freopen("Vahl_Davis_hexa_ICoCo.out","w",stdout)) abort();         // + ICoCo couplage
    if (!freopen("Vahl_Davis_hexa_ICoCo.err","w",stderr)) abort();         // + ICoCo couplage
    T = getProblem();
    T->setDataFile(data_file);
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

    double* save_old_field=0;

    clock_t clock0= clock ();
    int compti=0;

    bool init=true; // first time step ??

    // loop on time steps
    while (!stop)
      {
        compti++;
        clock_t clocki= clock ();
        cout << compti << " CLOCK " << (clocki-clock0)*1.e-6 << endl; 
        ok=false; // Is the time interval successfully solved ?

        // Loop on the time interval tries
        while (!ok && !stop)
          {
            // Compute the first time step length
            double dt=T->computeTimeStep(stop);
            if (stop) // Impossible to solve the next time step, the Problem has given up
              break;
            // Prepare the next time step
            T->initTimeStep(dt);
            clock_t clock_avt_resolution= clock ();
            // Solve the next time step
            ok=T->solveTimeStep();
            clock_t clock_ap_resolution= clock ();
            cout << compti << " TEMPS DE RESOLUTION DU PB (s) :  " << (clock_ap_resolution-clock_avt_resolution)*1.e-6 << endl; 
            init=false;
            if (!ok) // The resolution failed, try with a new time interval.
              T->abortTimeStep();
            else // The resolution was successful, validate and go to the
              // next time step.
              T->validateTimeStep();
          }
        // Stop the resolution if the Problem is stationnary
        bool stat=T->isStationary();
        if (stat)
          stop=true;
      }
    T->terminate();
    delete T;
    if (save_old_field!=0) 
      delete [] save_old_field;
  }

  return 0;
}
