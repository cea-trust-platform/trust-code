#include <ICoCoProblem.h>
#include <CommInterface.hxx>
#include <ProcessorGroup.hxx>
#include <MPIProcessorGroup.hxx>
#include <Statistiques.h>

#include <TrioDEC.hxx>
#include <set>
#include <time.h>
#include <ICoCoTrioField.h>
#include <fstream>
#include <string.h>
#include <Objet_U.h>

using namespace MEDCoupling;
using namespace std;
using namespace ICoCo;

//utility methods for synchronizing
//data from the the two trio instance
typedef enum {
    sync_and, sync_or
} synctype;
void synchronize_bool(bool& stop, synctype s) {
    int my_stop;
    int my_stop_temp = stop ? 1 : 0;

    if (s == sync_and)
        MPI_Allreduce(&my_stop_temp, &my_stop, 1, MPI_INTEGER, MPI_MIN,
                      MPI_COMM_WORLD);
    else if (s == sync_or)
        MPI_Allreduce(&my_stop_temp, &my_stop, 1, MPI_INTEGER, MPI_MAX,
                      MPI_COMM_WORLD);
    else
        throw;
    stop = (my_stop == 1);
}

void synchronize_dt(double& dt) {
    double dttemp = dt;
    MPI_Allreduce(&dttemp, &dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
}

void modifie_n_TrioDEC(TrioDEC& dec) {
    dec.setNature(IntensiveMaximum);
}
int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    {
        int rank, size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        if (size != 2) {
            cout << "process rank=" << rank << " pb: must run on 2 procs or +!" << endl;
            exit(1);
        }
        CommInterface comm;
        set<int> dom1_ids;
        set<int> dom2_ids;

        //dom1_ids.insert(0);
        //dom1_ids.insert(1);
        //dom2_ids.insert(2);
        //dom2_ids.insert(3);

        dom1_ids.insert(0);
        dom2_ids.insert(1);

        MPIProcessorGroup dom1_group(comm, dom1_ids);
        MPIProcessorGroup dom2_group(comm, dom2_ids);

        Problem* T;
        std::string data_file;
        const MPI_Comm* mpicomm = 0;
        if (dom1_group.containsMyRank()) {
            if (!freopen("docond_VEF_dom1.out", "w", stdout))
                abort();
            if (!freopen("docond_VEF_dom1.err", "w", stderr))
                abort();
            data_file = "docond_VEF_dom1";
            mpicomm = dom1_group.getComm();
        } else if (dom2_group.containsMyRank()) {
            if (!freopen("docond_VEF_dom2.out", "w", stdout))
                abort();
            if (!freopen("docond_VEF_dom2.err", "w", stderr))
                abort();
            data_file = "docond_VEF_dom2";
            mpicomm = dom2_group.getComm();
        } else
            throw 0;
        T = getProblem();
        T->setDataFile(data_file);
        T->setMPIComm((void*) mpicomm);
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

        synchronize_bool(stop, sync_or);

        TrioDEC dec1(dom2_ids, dom1_ids);
        TrioDEC dec2(dom2_ids, dom1_ids);

        TrioField field1;
        TrioField field2;

        auto clock0 = Statistiques::get_time_now();
        int compti = 0;

        bool init = true; // first time step ??

        // loop on time steps
        while (!stop) {
            compti++;
            auto clocki = Statistiques::get_time_now();
            cout << compti << " CLOCK " << (clocki - clock0) << endl;

            ok = false; // Is the time interval successfully solved ?

            // Loop on the time interval tries
            while (!ok && !stop) {
                // Compute the first time step length
                double dt = T->computeTimeStep(stop);
                synchronize_dt(dt);
                synchronize_bool(stop, sync_or);
                if (stop) // Impossible to solve the next time step, the Problem
                    // has given up
                    break;

                if (rank == 0)
                    cout << " chosen dt " << dt << endl;

                // Prepare the next time step
                T->initTimeStep(dt);
                {
                    // On renseigne le maillage des champs a recevoir:
                    if (dom1_group.containsMyRank()) {
                        T->getInputFieldTemplate("FLUX_SURFACIQUE_IN_DOM1",	field1);
                        T->getOutputField("TEMPERATURE_OUT_DOM1", field2);
                    } else {
                        T->getOutputField("FLUX_SURFACIQUE_OUT_DOM2", field1);
                        T->getInputFieldTemplate("TEMPERATURE_IN_DOM2", field2);
                    }

                    if (init) {
                        dec1.setMethod(field1._type == 0 ? "P0" : "P1");
                        dec2.setMethod(field2._type == 0 ? "P0" : "P1");
                    }
                    // We attached the fields to the pipes
		    const double& precision = Objet_U::precision_geom;
                    dec1.attachLocalField(&field1);
                    dec1.setNature(IntensiveMaximum);
		    dec1.setPrecision(precision);
                    dec2.attachLocalField(&field2);
                    dec2.setNature(IntensiveMaximum);
		    dec2.setPrecision(precision);
                    if (init) {
                        dec1.synchronize();
                        dec2.synchronize();
                    }

                    // Debug field
                    cout << "DEBUG before the exchange the data: " << endl;
                    cout << "field1._mesh_dim " << field1._mesh_dim << endl;
                    cout << "field1._space_dim " << field1._space_dim << endl;
                    cout << "field1._nb_elems " << field1._nb_elems << endl;
                    cout << "field1._nb_field_components " << field1._nb_field_components << endl;
                    for (int i = 0; i < field1._nb_elems; i++)
                        cout << "field1._field[" << i << "]="<< field1._field[i] << " W/m2" << endl;

                    cout << "field2._mesh_dim " << field2._mesh_dim << endl;
                    cout << "field2._space_dim " << field2._space_dim << endl;
                    cout << "field2._nb_elems " << field2._nb_elems << endl;
                    cout << "field2._nb_field_components " << field2._nb_field_components << endl;
                    for (int i = 0; i < field2._nb_elems; i++)
                        cout << "field2._field[" << i << "]=" << field2._field[i] << " K" << endl;
                    // End debug field

                    // send - received
                    if (dom1_group.containsMyRank()) {
                        dec1.recvData();
                        dec2.sendData();
                    } else {
                        dec1.sendData();
                        dec2.recvData();
                    }

                    // On specifie les champs au probleme de TRUST (les CLs sont alors a jour)
                    // Il faut que la CL existe...
                    if (dom1_group.containsMyRank()) {
                        // On inverse le signe du flux et on divise par la surface
                        double sum = 0;
                        for (int i = 0; i < field1._nb_elems; i++) {
                            field1._field[i] *= -1;
                            sum += field1._field[i];
                        }
                        cout << "Impose F1 total=" << sum << " W" << endl;
                        for (int i = 0; i < field1._nb_elems; i++)
                            cout << "Impose FS1[" << i << "]=" << field1._field[i] << " W/m2" << endl;
                        T->setInputField("FLUX_SURFACIQUE_IN_DOM1", field1);
                    } else {
                        for (int i = 0; i < field2._nb_elems; i++)
                            cout << "Impose T2[" << i << "]=" << field2._field[i] << " K" << endl;
                        T->setInputField("TEMPERATURE_IN_DOM2", field2);
                    }
                }        // destruction of fields and associated tables

                auto clock_avt_resolution = Statistiques::get_time_now();
                // Solve the next time step
                ok = T->solveTimeStep();

                auto clock_ap_resolution = Statistiques::get_time_now();
                cout << compti << " TEMPS DE RESOLUTION DU PB (s) :  " << (clock_ap_resolution - clock_avt_resolution) << endl;

                init = false;

                synchronize_bool(ok, sync_and);
                if (!ok) // The resolution failed, try with a new time interval.
                    T->abortTimeStep();

                else
                    // The resolution was successful, validate and go to the next time step.
                    T->validateTimeStep();
            }

            // Stop the resolution if the Problem is stationnary
            bool stat = T->isStationary();
            synchronize_bool(stat, sync_and);
            if (stat)
                stop = true;
        }
        T->terminate();
        delete T;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
