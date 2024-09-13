/****************************************************************************
* Copyright (c) 2024, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Process.h>
#include <Statistiques.h>
#include <Comm_Group.h>
#include <PE_Groups.h>
#include <communications.h>
#include <Sortie_Nulle.h>
#include <Journal.h>
#include <SFichier.h>
#include <Synonyme_info.h>
#include <petsc_for_kernel.h>
#include <comm_incl.h>
#include <TRUST_Error.h>
#include <Comm_Group_MPI.h>
#include <unistd.h> // sleep() pour certaines machines
#include <SChaine.h>
#include <FichierHDFPar.h>
#include <EChaineJDD.h>
#include <DeviceMemory.h>

// Chacun des fichiers Cerr, Cout et Journal(i)
// peut etre redirige vers l'un des quatre fichiers suivants:
// Instance de la Sortie nulle (equivalent de /dev/null)
static Sortie_Nulle  journal_zero_;
// Instance de la Sortie pointant vers cerr
static Sortie        std_err_(cerr);
// Instance de la Sortie pointant vers cout
static Sortie        std_out_(cout);
// Instances du fichier Journal
// (if the journal is shared between all the procs, journal_file_ is not used
//  and the processors' output is redirected to journal_shared_stream_ before being written in a HDF5 file)
static SFichier     journal_file_;
static SChaine      journal_shared_stream_;
static int 			journal_shared_;

static int        journal_file_open_;
static Nom           journal_file_name_;
// Niveau maximal des messages ecrits. La valeur initiale determine
// si les messages ecrits avant l'initialisation du journal sont ecrits
// ou pas.
static int        verbose_level_ = 0;
static int        disable_stop_ = 0;

// Drapeau indiquant si les sorties cerr et cout doivent
// etre redirigees vers le fichier journal
static int        cerr_to_journal_ = 0;
extern Stat_Counter_Id mpi_allreduce_counter_;
int Process::exception_sur_exit=0;
int Process::multiple_files=5120; // Valeur modifiable avec la variable d'environnement TRUST_MultipleFiles
bool Process::force_single_file(const int ranks, const Nom& filename)
{
  char* theValue = getenv("TRUST_MultipleFiles");
  if (theValue != nullptr) multiple_files=atoi(theValue);
  if (ranks>multiple_files)
    {
      if (Process::je_suis_maitre())   // Attention, necessaire, car appel possible tres tot dans main.cpp alors que Cerr par defini completement sur les processes
        {
          Cerr << "======================================================================================================" << finl;
          Cerr << "Warning! Single file option is forced for " << filename << " above " << multiple_files << " MPI ranks." << finl;
          Cerr << "for I/O performance reasons on cluster and inodes number limitation." << finl;
          Cerr << "If you want to keep multiple files, add at the beginning of your data file to outpass the limitation:" << finl;
          Cerr << "MultipleFiles " << ranks << finl;
          Cerr << "=====================================================================================================" << finl;
        }
      return true;
    }
  else
    return false;
}

/*! @brief renvoie 1 si on est sur le processeur maitre du groupe courant (c'est a dire me() == 0), 0 sinon.
 *
 * Voir Comm_Group::rank()
 *
 */
int Process::je_suis_maitre()
{
  const int r = PE_Groups::current_group().rank();
  return r == 0;
}

/*! @brief renvoie le nombre de processeurs dans le groupe courant Voir Comm_Group::nproc() et PE_Groups::current_group()
 *
 */
int Process::nproc()
{
  const int n = PE_Groups::current_group().nproc();
  return n;
}

bool Process::is_parallel()
{
  return Process::nproc() > 1;
}

bool Process::is_sequential()
{
  return Process::nproc() == 1;
}

/*! @brief renvoie mon rang dans le groupe de communication courant.
 *
 * Voir Comm_Group::rank() et PE_Groups::current_group()
 *
 */
int Process::me()
{
  const int r = PE_Groups::current_group().rank();
  return r;
}

/*! @brief Synchronise tous les processeurs du groupe courant (attend que tous les processeurs soient arrives a la barriere)
 *
 *    Instruction a executer sur tous les processeurs du groupe.
 *
 */
void Process::barrier()
{
  PE_Groups::current_group().barrier(0);
}

/*! @brief Calcule le max de x sur tous les processeurs du groupe courant.
 *
 * Remarques :
 *    Cette methode doit etre appelee sur tous les processeurs du groupe.
 *    La valeur renvoyee est identique sur tous les processeurs.
 *
 */
double Process::mp_max(double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MAX);
  return y;
}

int Process::mp_max(int x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MAX);
  return y;
}


/*! @brief Calcule le min de x sur tous les processeurs du groupe courant.
 *
 * @sa mp_max()
 *
 */
double Process::mp_min(double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MIN);
  return y;
}

int Process::mp_min(int x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MIN);
  return y;
}

/*! @brief Calcule la somme de x sur tous les processeurs du groupe courant.
 *
 * @sa mp_max()
 *
 */
double Process::mp_sum(double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_SUM);
  return y;
}

/*! @brief Calcule la somme de x sur tous les processeurs du groupe courant.
 *
 * !!! Note that the sum of many int might result in a long !!!
 *
 * @sa mp_max()
 */
trustIdType Process::mp_sum(trustIdType x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  trustIdType y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_SUM);
  return y;
}

int check_int_overflow(trustIdType v)
{
  if (v >= std::numeric_limits<int>::max())
    Process::exit("Value too big - above 32b and can not be converted to int!!");
  return static_cast<int>(v);
}


/*! @brief Calcule le 'et' logique de b sur tous les processeurs du groupe courant.
 *
 */
bool Process::mp_and(bool b)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int x = b ? 1 : 0;
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MIN);
  return y == 1;
}

/*! @brief Routine de sortie de Trio-U sur une erreur Sauvegarde la memoire et le hierarchie dans les fichiers "memoire.
 *
 * dump" et "hierarchie.dump"
 *
 */
void Process::exit(int i)
{
  Nom message="=========================================\nTRUST has caused an error and will stop.\nUnexpected error during TRUST calculation.";
  std::string jddLine = "\nError triggered at line " + std::to_string(EChaineJDD::file_cur_line_) + " in " + Objet_U::nom_du_cas().getString() + ".data";
  message+=jddLine;
  exit(message,i);
}
void Process::exit(const Nom& message ,int i)
{
  if (exception_sur_exit == 2)
    {
      ::exit(-1); // ND 11/01/23 utilisation d'un second ::exit(-1) dans TRUST car si pas droits d'ecriture appel recursif a Process::exit()
    }

  if(je_suis_maitre())
    {
      Cerr << message << finl;
      Cerr.flush();
      // Utile pour XData et la creation de syno.py
      if (getenv("TRUST_USE_XDATA")!=nullptr)
        {
          SFichier hier("hierarchie.dump");
          hier << "\n             KEYWORDS\n";
          Type_info::hierarchie(hier);
          hier << "\n             SYNONYMS\n";
          Synonyme_info::hierarchie(hier);
        }
      if (!get_disable_stop() && Process::je_suis_maitre())
        {
          Nom nomfic( Objet_U::nom_du_cas() );
          nomfic += ".stop";
          {
            SFichier ficstop( nomfic );
            ficstop <<message<<finl;
          }
        }
    }
  Journal() << message << finl;
  if( journal_shared_ && journal_file_open_)
    end_journal(verbose_level_);


  if (exception_sur_exit)
    {
      // Lancement d'une exception (utilise par Execute_parallel)
      throw TRUST_Error("Error in trust ",Process::me());
    }
  else
    {
      int abort=0;
#ifdef MPI_
#if INT_is_64_ == 1
#define MPI_ENTIER MPI_LONG
#else
#define MPI_ENTIER MPI_INT
#endif
      if (Process::is_parallel())
        {
          const MPI_Comm& mpi_comm=ref_cast(Comm_Group_MPI,PE_Groups::groupe_TRUST()).get_mpi_comm();
          int tag = 666;
          int buffer[1]= {1};
          MPI_Request request;

          // Envoi non bloquant vers me()+1
          int to_pe = (me()==nproc()-1?0:me()+1);
          MPI_Isend(buffer, 1, MPI_ENTIER, to_pe, tag, mpi_comm, &request);

          // Reception non bloquante depuis me()-1
          int from_pe = (me()==0?nproc()-1:me()-1);
          MPI_Irecv(buffer, 1, MPI_ENTIER, from_pe, tag, mpi_comm, &request);

          // Attente
          sleep(1);

          // Test si me() a recu de me()-1
          True_int ok;
          MPI_Status status;
          MPI_Test(&request,&ok,&status);
          if (!ok)
            abort=1;
        }
#endif
      if (abort)
        {
          if (!je_suis_maitre())
            {
              std_err_ << "!!! TRUST process number " << Process::me() << " exited unexpectedly ! See error message at the end of the file " << journal_file_name_ << " -> Aborting calculation..." << finl;
            }
          PE_Groups::groupe_TRUST().abort();
        }
      else
        {
#ifdef MPI_
          MPI_Finalize();
#endif
          PE_Groups::finalize();
        }
    }
  Kokkos::finalize();
  // On force exit();
  if (i==0) i=-1;
  ::exit(i); //Seul ::exit utilise dans le code jusqu'a 01/23. second ajoute car appel recursif a Process::exit si droits ecriture dossier etude manquants
}

/*! @brief Routine de sortie de Trio-U sur une erreur abort()
 *
 */
void Process::abort()
{
#ifdef NDEBUG
  // En optimise on sort proprement.
  exit();
#else
  // En debug, on sort brutal pour avoir des infos avec le debugger ?
  ::abort(); //Seul ::abort() utilise dans le code
#endif
}

/*! @brief Renvoie un objet statique de type Sortie qui sert de journal d'evenements.
 *
 * Si message_level <= verbose_level_, on ecrit le message, sinon
 *   on l'envoie sur une Sortie_Nulle.
 *   Si le fichier journal est ouvert, on ecrit dans le fichier, sinon dans stderr.
 *
 */
Sortie& Process::Journal(int message_level)
{
  if (message_level <= verbose_level_ && verbose_level_ > 0)
    {
      if (journal_file_open_)
        {
          if(journal_shared_)
            return journal_shared_stream_;
          else
            return journal_file_;
        }
      else
        return std_err_;
    }
  return journal_zero_;
}

// Renvoie la ram occupee par un processeur
double Process::ram_processeur()
{
#ifdef PETSCKSP_H
  PetscLogDouble memoire;
  PetscMemoryGetCurrentUsage(&memoire);
  return memoire;
#else
  return 0;
#endif
}

void Process::imprimer_ram_totale(int all_process)
{
  double memoire=ram_processeur();
  if (memoire)
    {
      int Mo=1024*1024;
      if (all_process) Journal() << (int)(memoire/Mo) << " MBytes of RAM taken by the processor " << Process::me() << finl;
      /*      if (Process::nproc()>100)
              {
                memoire=Process::nproc()*memoire;
                Cout << (int)(memoire/Mo) << " MBytes of RAM taken by the calculation (estimation)." << finl;
              }
            else */
      {
        //int max_memoire=Process::mp_max(memoire); // int au lieu de double provoque un segfault comment detecter ?
        double max_memoire=Process::mp_max(memoire);
        double total_memoire=Process::mp_sum(memoire);
        Cout << (int)(total_memoire/Mo) << " MBytes of RAM taken by the calculation (max on a rank: "<<(int)(max_memoire/Mo)<<" MB)." << finl;
#ifdef _OPENMP_TARGET
        int Go = 1024 * Mo;
        double allocated = mp_max((double)DeviceMemory::allocatedBytesOnDevice());
        size_t total = DeviceMemory::deviceMemGetInfo(1);
        Cout << 0.1*(int)(10*allocated/Go) << " GBytes of maximal RAM allocated on a GPU (" <<  int(100 * allocated / total) << "%)" << finl;
#endif
      }
    }
}

/*! @brief Initialisation du journal
 *
 * @param (verbose_level) les messages de niveau <= verbose_level seront affiches, les autres seront mis a la poubelle.
 * @param (file_name) si pointeur nul, tout le monde ecrit dans cerr, sinon c'est le nom du fichier (doit etre different sur chaque processeur)
 * @param (append) indique si on ouvre le fichier en mode append ou pas.
 */
void init_journal_file(int verbose_level, int journal_shared, const char * file_name, int append)
{
  journal_shared_ = journal_shared;
  if( journal_shared_ )
    {
      if(journal_file_open_)
        end_journal(verbose_level);
    }
  else
    end_journal(verbose_level);

  if (verbose_level > 0)
    {
      if (file_name)
        {
          if(!journal_shared_)
            {
              IOS_OPEN_MODE mode = ios::out;
              if (append)
                mode = ios::app;
              if (!journal_file_.ouvrir(file_name, mode))
                {
                  Cerr << "Fatal error in init_journal_file: cannot open journal file" << finl;
                  Process::exit();
                }
            }
          else
            {
              if(append)
                Cerr << "Process.cpp::init_journal_file : append mode is not possible with HDF5!\n"
                     << "If " << file_name << " already exists, it will be overwritten" << finl;
            }
          journal_file_open_ = 1;
          journal_file_name_ = file_name;
        }
    }
  verbose_level_ = verbose_level;
}

void end_journal(int verbose_level)
{
  // Attention: acrobatie pour que ca "plante proprement" si le destructeur
  // ecrit dans le journal !
  if(journal_shared_)
    {
      FichierHDFPar fic_hdf;
      fic_hdf.create(journal_file_name_);
      fic_hdf.create_and_fill_dataset_MW("/log", journal_shared_stream_);
      fic_hdf.close();
    }
  else
    journal_file_.close();
  journal_file_open_ = 0;
}

/*! @brief Renvoie l'objet Sortie sur lequel seront redirigees les objets ecrits dans Cerr.
 *
 * Cela peut etre std_err_ ou journal_file_
 *
 */
Sortie& get_Cerr()
{
  if (journal_file_open_ && cerr_to_journal_)
    {
      if(journal_shared_)
        return journal_shared_stream_;
      else
        return journal_file_;
    }
  else
    {
      // dans le cas ou on a pas initialise les groupes
      // on ne peut pas tester si on est maitre
      if (PE_Groups::get_nb_groups()==0)
        return std_err_;
      // Seul le processeur maitre ecrit sur std_err_, les autres ecrivent sur journal_file_
      if (Process::je_suis_maitre())
        return std_err_;
      else if (verbose_level_)
        {
          if(journal_shared_)
            return journal_shared_stream_;
          else
            return journal_file_;
        }
      else
        return journal_zero_;
    }
}

/*! @brief Si on est sur le maitre, on renvoie cout ou le fichier journal sinon journal_zero_.
 *
 *   @sa cerr_to_journal_
 *
 */
Sortie& get_Cout()
{
  if (Process::je_suis_maitre())
    {
      if (journal_file_open_ && cerr_to_journal_)
        {
          if(journal_shared_)
            return journal_shared_stream_;
          else
            return journal_file_;
        }
      else
        return std_out_;
    }
  else
    {
      return journal_zero_;
    }
}

/*! @brief change la destination de Cerr et Cout Si flag=0, c'est stderr et stdout, sinon, si le fichier
 *
 *   journal est ouvert, c'est le journal, sinon c'est
 *   Sortie_Nulle
 *
 */
void set_Cerr_to_journal(int flag)
{
  cerr_to_journal_ = flag;
}

int get_journal_level()
{
  return verbose_level_;
}

void change_journal_level(int level)
{
  verbose_level_ = level;
}

/*! @brief Returns the disable_stop_ flag (Disable or not the writing of the .
 *
 * stop file)
 *
 */
int get_disable_stop()
{
  return disable_stop_;
}

/*! @brief Affects a new value to disable_stop_ flag (Disable or not the writing of the .
 *
 * stop file)
 *
 */
void change_disable_stop(int new_stop)
{
  disable_stop_ = new_stop;
}
