/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Process.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/59
//
//////////////////////////////////////////////////////////////////////////////

#include <Process.h>
#include <Statistiques.h>
#include <verif_cast.h>
#include <Comm_Group.h>
#include <PE_Groups.h>
#include <communications.h>
#include <Sortie_Nulle.h>
#include <Journal.h>
#include <SFichier.h>
#include <Synonyme_info.h>
#include <petsc_for_kernel.h>
#include <comm_incl.h>
#include <TriouError.h>
#include <Comm_Group_MPI.h>
#include <unistd.h> // sleep() pour certaines machines
#include <stat_counters.h>

// Chacun des fichiers Cerr, Cout et Journal(i)
// peut etre redirige vers l'un des quatre fichiers suivants:
// Instance de la Sortie nulle (equivalent de /dev/null)
static Sortie_Nulle  journal_zero_;
// Instance de la Sortie pointant vers cerr
static Sortie        std_err_(cerr);
// Instance de la Sortie pointant vers cout
static Sortie        std_out_(cout);
// Instance du fichier Journal
static SFichier      journal_file_;
static int        journal_file_open_;
static Nom           journal_file_name_;
// Niveau maximal des messages ecrits. La valeur initiale determine
// si les messages ecrits avant l'initialisation du journal sont ecrits
// ou pas.
static int        verbose_level_ = 0;

// Drapeau indiquant si les sorties cerr et cout doivent
// etre redirigees vers le fichier journal
static int        cerr_to_journal_ = 0;
extern Stat_Counter_Id mpi_allreduce_counter_;
int Process::exception_sur_exit=0;

// Description: renvoie 1 si on est sur le processeur maitre du groupe courant
//  (c'est a dire me() == 0), 0 sinon. Voir Comm_Group::rank()
int Process::je_suis_maitre()
{
  const int r = PE_Groups::current_group().rank();
  return r == 0;
}

// Description:
//     Constructeur par defaut
Process::Process()
{
}

// Description: renvoie le nombre de processeurs dans le groupe courant
//  Voir Comm_Group::nproc() et PE_Groups::current_group()
int Process::nproc()
{
  const int n = PE_Groups::current_group().nproc();
  return n;
}

// Description: renvoie mon rang dans le groupe de communication courant.
//  Voir Comm_Group::rank() et PE_Groups::current_group()
int Process::me()
{
  const int r = PE_Groups::current_group().rank();
  return r;
}

// Description:
//   Synchronise tous les processeurs du groupe courant
//   (attend que tous les processeurs soient arrives a la barriere)
//   Instruction a executer sur tous les processeurs du groupe.
void Process::barrier()
{
  PE_Groups::current_group().barrier(0);
}

// Description:
//    Destructeur
//    Ne fait rien
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Process::~Process()
{
}

// Description:
//  Calcule le max de x sur tous les processeurs du groupe courant.
//  Remarques :
//   Cette methode doit etre appelee sur tous les processeurs du groupe.
//   La valeur renvoyee est identique sur tous les processeurs.
double Process::mp_max(double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MAX);
  return y;
}

// Description:
//  Calcule le min de x sur tous les processeurs du groupe courant.
//  Voir aussi mp_max()
double Process::mp_min(double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MIN);
  return y;
}

// Description:
//  Calcule la somme de x sur tous les processeurs du groupe courant.
//  Voir aussi mp_max()
double Process::mp_sum(double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_SUM);
  return y;
}

// Description:
//  Calcule la somme de x sur tous les processeurs du groupe courant.
//  Voir aussi mp_max()
int Process::mp_sum(int x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_SUM);
  return y;
}

// Description:
//    Routine de sortie de Trio-U sur une erreur
//    Sauvegarde la memoire et le hierarchie dans les fichiers "memoire.dump" et "hierarchie.dump"
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Process::exit(int i)
{
  Nom message="=========================================\nTRUST has caused an error and will stop.\nUnexpected error during TRUST calculation.";
  exit(message,i);
}
void Process::exit(const Nom& message ,int i)
{
  if(je_suis_maitre())
    {
      Cerr << message << finl;
      Cerr.flush();
      SFichier hier("hierarchie.dump");
      hier << "\n             KEYWORDS\n";
      Type_info::hierarchie(hier);
      hier << "\n             SYNONYMS\n";
      Synonyme_info::hierarchie(hier);
      Nom nomfic( Objet_U::nom_du_cas() );
      nomfic += ".stop";
      {
        SFichier ficstop( nomfic );
        ficstop <<message<<finl;
      }

    }
  Journal() << message << finl;
  if (exception_sur_exit)
    {
      // Lancement d'une exception (utilise par Execute_parallel)
      throw TriouError("Error in trust ",Process::me());
    }
  else
    {
      int abort=0;
#ifdef MPI_
      if (Process::nproc()>1)
        {
          const MPI_Comm& mpi_comm=ref_cast(Comm_Group_MPI,PE_Groups::groupe_TRUST()).get_mpi_comm();
          int tag = 666;
          int buffer[1]= {1};
          MPI_Request request;

          // Envoi non bloquant vers me()+1
          int to_pe = (me()==nproc()-1?0:me()+1);
          MPI_Isend(buffer, 1, MPI_INT, to_pe, tag, mpi_comm, &request);

          // Reception non bloquante depuis me()-1
          int from_pe = (me()==0?nproc()-1:me()-1);
          MPI_Irecv(buffer, 1, MPI_INT, from_pe, tag, mpi_comm, &request);

          // Attente
          sleep(1);

          // Test si me() a recu de me()-1
          int ok;
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
  // On force exit();
  if (i==0) i=-1;
  ::exit(i); //Seul ::exit utilise dans le code
}

// Description:
//    Routine de sortie de Trio-U sur une erreur abort()
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description:
//  Renvoie un objet statique de type Sortie qui sert de journal d'evenements.
//  Si message_level <= verbose_level_, on ecrit le message, sinon
//  on l'envoie sur une Sortie_Nulle.
//  Si le fichier journal est ouvert, on ecrit dans le fichier, sinon dans stderr.
Sortie& Process::Journal(int message_level)
{
  if (message_level <= verbose_level_ && verbose_level_ > 0)
    {
      if (journal_file_open_)
        return journal_file_;
      else
        return std_err_;
    }
  return journal_zero_;
}

// Renvoie la ram occupee par un processeur
double Process::ram_processeur()
{
#ifdef __PETSCKSP_H
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
      if (all_process) Journal() << int(memoire/Mo) << " MBytes of RAM taken by the processor " << Process::me() << finl;
      if (Process::nproc()>100)
        {
          memoire=Process::nproc()*memoire;
          Cout << int(memoire/Mo) << " MBytes of RAM taken by the calculation (estimation)." << finl;
        }
      else
        {
          memoire=Process::mp_sum(memoire);
          Cout << int(memoire/Mo) << " MBytes of RAM taken by the calculation." << finl;
        }
    }
}

// Description: Initialisation du journal
// Parametre: verbose_level
// Signification: les messages de niveau <= verbose_level seront affiches,
//   les autres seront mis a la poubelle.
// Parametre: file_name
// Signification: si pointeur nul, tout le monde ecrit dans cerr, sinon
//   c'est le nom du fichier (doit etre different sur chaque processeur)
// Parametre: append
// Signification: indique si on ouvre le fichier en mode append ou pas.
void init_journal_file(int verbose_level, const char * file_name, int append)
{
  end_journal(verbose_level);
  if (verbose_level > 0)
    {
      if (file_name)
        {
          IOS_OPEN_MODE mode = ios::out;
          if (append)
            mode = ios::app;
          if (!journal_file_.ouvrir(file_name, mode))
            {
              cerr << "Fatal error in init_journal_file: cannot open journal file" << endl;
              Process::exit();
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
  journal_file_open_ = 0;
  journal_file_.close();
}

// Description: Renvoie l'objet Sortie sur lequel seront redirigees les
//  objets ecrits dans Cerr. Cela peut etre std_err_ ou journal_file_
Sortie& get_Cerr()
{
  if (journal_file_open_ && cerr_to_journal_)
    return journal_file_;
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
        return journal_file_;
      else
        return journal_zero_;
    }
}

// Description: Si on est sur le maitre, on renvoie cout ou le fichier journal
//  sinon journal_zero_.
//  Voir aussi cerr_to_journal_
Sortie& get_Cout()
{
  if (Process::je_suis_maitre())
    {
      if (journal_file_open_ && cerr_to_journal_)
        return journal_file_;
      else
        return std_out_;
    }
  else
    {
      return journal_zero_;
    }
}

// Description: change la destination de Cerr et Cout
//  Si flag=0, c'est stderr et stdout, sinon, si le fichier
//  journal est ouvert, c'est le journal, sinon c'est
//  Sortie_Nulle
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


