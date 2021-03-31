#include <ICoCoProblem.h>
#include <CommInterface.hxx>
#include <ProcessorGroup.hxx>
#include <MPIProcessorGroup.hxx>

#include <TrioDEC.hxx>
#include <set>
#include <time.h>
#include <ICoCoTrioField.h>
#include <fstream>
#include <string.h>

using namespace MEDCoupling;
using namespace std;
using namespace ICoCo;

// main program
int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    {
        std::string data_file;
        data_file = "Vahl_Davis_hexa_ICoCo_exchange"; // name of the datafile to read with ICoCo
        // Coupling information output redirection to datafile.out and datafile.err
        if (!freopen("Vahl_Davis_hexa_ICoCo_exchange.out", "w", stdout))
            abort();
        if (!freopen("Vahl_Davis_hexa_ICoCo_exchange.err", "w", stderr))
            abort();

        Problem* T;
        T = getProblem();
        T->setDataFile(data_file);
        T->initialize();

        vector<string> outputnames = T->getOutputFieldsNames();
        vector<string> inputnames = T->getInputFieldsNames();
        for (unsigned int ii = 0; ii < outputnames.size(); ii++)
            cout << data_file << "  Field Output " << outputnames[ii] << endl;
        cout << endl;

        for (unsigned int ii = 0; ii < inputnames.size(); ii++)
            cout << data_file << "  Field Input  " << inputnames[ii] << endl;
        cout << endl;

        bool stop = false; // Does the Problem want to stop ?
        bool ok = true; // Is the time interval successfully solved ?

        TrioField Temperature_field;

        clock_t clock0 = clock();
        int compti = 0;

        // loop on time steps
        while (!stop) {
            compti++;
            clock_t clocki = clock();
            cout << compti << " CLOCK " << (clocki - clock0) * 1.e-6 << endl;

            ok = false; // Is the time interval successfully solved ?

            // Loop on the time interval tries
            while (!ok && !stop) {
                // Compute the time step
                double dt = T->computeTimeStep(stop);
                if (stop) // Impossible to solve the next time step, the Problem has given up
                    break;
                // Prepare the next time step
                T->initTimeStep(dt);

                {
                    // get the support (mesh) of field (input field in the datafile)
                    T->getInputFieldTemplate("TEMPERATURE_IN_DOM", Temperature_field);
                    // filling the field
                    Temperature_field.set_standalone();
                    int nbvals = Temperature_field.nb_values() * Temperature_field._nb_field_components;
                    for(int i = 0; i < nbvals; i++)
                        Temperature_field._field[i] = 10;
                    // send the field's values to datafile 
                    T->setInputField("TEMPERATURE_IN_DOM", Temperature_field);
                }

                // Solve the next time step and print the resolution duration
                clock_t clock_before_resolution = clock();
                ok = T->solveTimeStep();
                clock_t clock_after_resolution = clock();
                cout << compti << " TEMPS DE RESOLUTION DU PB (s) :  " << (clock_after_resolution - clock_before_resolution) * 1.e-6 << endl;

                if (!ok) // The resolution failed, try with a new time interval.
                    T->abortTimeStep();
                else // The resolution was successful, validate and go to the next time step.
                    T->validateTimeStep();
            }

            // Stop the resolution if the Problem is stationnary
            bool stationary = T->isStationary();
            if (stationary)
                stop = true;
        }
        T->terminate();
        delete T;
    }
    return 0;
}
