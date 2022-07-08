#include <ICoCoProblem.h>
#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"
#include <Statistiques.h>

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
void modifie_TrioDEC(TrioDEC& dec)
{
  //dec.setOrientation(2);
  //dec.setMaxDistance3DSurfIntersect(1e-6);
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
    set<int> EF_ids;
    set<int> entree_vef_ids;
    EF_ids.insert(0);
    for (int i=1;i<size;i++)
      entree_vef_ids.insert(i);

    MPIProcessorGroup entree_vef_group(comm,entree_vef_ids);
    MPIProcessorGroup EF_group(comm,EF_ids);

    Problem* T;
    std::string data_file;
    const MPI_Comm* mpicomm=0;
    if (entree_vef_group.containsMyRank())
      {
        if (!freopen("entree_vef.out","w",stdout)) abort();
        if (!freopen("entree_vef.err","w",stderr)) abort();

        data_file="entree_vef";
        mpicomm=entree_vef_group.getComm();

      }
    else if (EF_group.containsMyRank())
      {
        if (!freopen("EF.out","w",stdout)) abort();
        if (!freopen("EF.err","w",stderr)) abort();

        data_file="EF";
        mpicomm=EF_group.getComm();

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

    T->getInputFieldsNames();
    vector<string> inputnames= T->getInputFieldsNames();
    for (unsigned int ii=0;ii<inputnames.size();ii++)
      cout<<" necessaire " <<inputnames[ii]<<endl;



    bool stop=false; // Does the Problem want to stop ?
    bool ok=true; // Is the time interval successfully solved ?


    synchronize_bool(stop,sync_or);


    TrioDEC dec_vit_in_chaude(entree_vef_ids, EF_ids);

    TrioDEC pression_chaude(EF_ids, entree_vef_ids);


    modifie_TrioDEC(dec_vit_in_chaude);
    /*
  if ( entree_vef_group.containsMyRank()) 
    dec_vit_in_chaude.setMethod("P1");
  else
    dec_vit_in_chaude.setMethod("P0");
     */
    modifie_TrioDEC(pression_chaude);

    TrioField Pression_chaude;

    double* save_old_field=0;

    TrioField vit_chaude;

    auto clock0= Statistiques::get_time_now();
    int compti=0;

    bool init=true; // first time step ??


    //boucle sur les pas de temps
    while (!stop) {

        compti++;
        auto clocki= Statistiques::get_time_now();
        cout << compti << " CLOCK " << (clocki-clock0) << endl;

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

            {
              // OK on all procs


              // On renseigne le maillage des champs � recevoir.


              if ( entree_vef_group.containsMyRank()) {
                  T->getInputFieldTemplate("Pression_chaude",Pression_chaude);
                  T->getOutputField("VITESSE_OUT",vit_chaude);
              }
              else
                {
                  T->getInputFieldTemplate("vitesse_in",vit_chaude);
                  T->getOutputField("Pression_lisse",Pression_chaude);
                  Pression_chaude.setName("Pression_lisse");
                }

              if (init)
                {
                  if (vit_chaude._type==0)
                    dec_vit_in_chaude.setMethod("P0");
                  else
                    dec_vit_in_chaude.setMethod("P1");
                  if (Pression_chaude._type==0)
                    pression_chaude.setMethod("P0");
                  else
                    pression_chaude.setMethod("P1");
                }
              // On attache les champs au bout des tuyaux
              dec_vit_in_chaude.attachLocalField( &vit_chaude);
              pression_chaude.attachLocalField( &Pression_chaude);

              modifie_n_TrioDEC(dec_vit_in_chaude);
              modifie_n_TrioDEC(pression_chaude);
              if(init)
                {
                  dec_vit_in_chaude.synchronize();
                  pression_chaude.synchronize();
                }


              // Envois - receptions
              if (entree_vef_group.containsMyRank()) {
                  dec_vit_in_chaude.sendData();
                  ofstream save_vit("save_vit_vef.field");
                  vit_chaude.save(save_vit);
                  pression_chaude.recvData();
              }
              else {
                  dec_vit_in_chaude.recvData();
                  ofstream save_vit("save_vit_ef.field");
                  vit_chaude.save(save_vit);
                  pression_chaude.sendData();
                  ofstream save_p("save_pression_ef.field");
                  Pression_chaude.save(save_p);
              }





              if (entree_vef_group.containsMyRank() ) {
                  double r=0.05;
                  double p0=Pression_chaude._field[0];
                  int nbcase=Pression_chaude.nb_values()*Pression_chaude._nb_field_components;
                  // on retire la pression dans la case 0 pour avoir toujours P(0)=0
                  // la pression est d�finie a une constante pres
                  // pour t<=0.1 pas de retroaction en pression (P=0)
                  // ensuite P=r*P_ef+(1-r)*P_old relaxation pour faciliter la
                  // convergence du couplage
                  for(int i=0;i<nbcase;i++)
                    if ((save_old_field!=0)&&(Pression_chaude._time1 > 0.10))
                      {

                        Pression_chaude._field[i]-=p0;
                        Pression_chaude._field[i]=r*Pression_chaude._field[i]+(1-r)*save_old_field[i];
                      }
                    else
                      Pression_chaude._field[i]=0;

                  T->setInputField("Pression_chaude",Pression_chaude);
                  if (save_old_field==0) save_old_field=new double[nbcase];
                  memcpy(save_old_field,Pression_chaude._field,nbcase*sizeof(double));
                  ofstream save_p("save_pression_vef.field");
                  Pression_chaude.save(save_p);

              }
              else
                {
                  // couplage en vitesse des le debut et sans relaxataion
                  double vmin=1e38, vmax=-1e38;

                  for(int i=0;i<vit_chaude.nb_values()*vit_chaude._nb_field_components;i++)
                    {
                      double v=vit_chaude._field[i];
                      if (v<vmin) vmin=v;
                      if (v>vmax) vmax=v;
                    }

                  cerr<<" vitesse min"<<vmin << " max "<<vmax<<endl;

                  T->setInputField("vitesse_in",vit_chaude);

                }


            }	// destruction des champs et des tableaux associ�s

            auto clock_avt_resolution= Statistiques::get_time_now();

            // Solve the next time step
            ok=T->solveTimeStep();

            auto clock_ap_resolution= Statistiques::get_time_now();
            cout << compti << " TEMPS DE RESOLUTION DU PB (s) :  " << (clock_ap_resolution-clock_avt_resolution) << endl;

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
