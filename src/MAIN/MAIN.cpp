/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <MAIN.h>
#include <sys/types.h>
#include <stdlib.h>
#include <mon_main.h>
#include <EFichier.h>
#ifdef linux
#include <fenv.h>
#endif
#include <errno.h>
#ifdef VTRACE
#include <vt_user.h>
#endif
#ifdef MICROSOFT
static const char directory_separator = '\\';
#else
static const char directory_separator = '/';
#endif

extern void desalloue_pwd();
void usage()
{
  Cerr << "usage:\n";
  Cerr << "TRUST_EXECUTABLE [CASE[.data]] [options]\n";
  Cerr << " CASE is the basename of the trust data file (must have .data extension)\n";
  Cerr << "   If no CASE given, the current directory name is used\n";
  Cerr << " -help_trust => print options\n";
  Cerr << " -mpi => run in parallel with MPI (must run with mpirun)\n";
  Cerr << " -check_enabled=0|1  => enables or disables runtime checking of parallel messages\n";
  Cerr << " -debugscript=SCRIPT => execute \"SCRIPT n\" after parallel initialisation, n=processor rank\n";
  Cerr << " -petsc=0            => disable call to PetscInitialize\n";
  Cerr << " -journal=0..9       => select journal level (0=disable, 9=maximum verbosity)\n";
  Cerr << " -journal_master     => only master processor writes a journal (not compatible with journal_shared)\n";
  Cerr << " -journal_shared     => each processor writes in a single log file (not compatible with journal_master)\n";
  Cerr << " -disable_ieee       => Disable the detection of NaNs. The detection can also be de-activated with env variable TRUST_DISABLE_FP_EXCEPT set to non zero.\n";
  Cerr << " -no_verify          => Disable the call to verifie function (from Type_Verifie) to catch outdated keywords while reading data file.\n";
  Cerr << " -disable_stop       => Disable the writing of the .stop file.\n";
  Cerr << finl;
  Process::exit();
}

#ifdef NDEBUG
#define DEFAULT_CHECK_ENABLED 0
#else
#define DEFAULT_CHECK_ENABLED 1
#endif

int main_TRUST(int argc, char** argv,mon_main*& main_process,int force_mpi)
{
#ifdef VTRACE
  //VT_USER_END("Initialization");
#endif
  // Attention: on n'a pas le droit d'utiliser les communications paralleles au debut.
  // Cerr et Cout sont OK (variables statiques dans Journal_log_files.cpp)
  // Le journal peut etre utilise mais ecrit dans Sortie_Nulle jusqu'a ce qu'on ouvre
  // les fichiers (voir Journal_log_files.cpp)
  //
  // Voir <PARALLEL_OK>

  // *************** Process command-line arguments ********************
  int with_mpi = force_mpi;
  int check_enabled = DEFAULT_CHECK_ENABLED;
  int with_petsc = -1;       // -1 => use petsc if compiled
  int nproc = -1;
  int verbose_level = 1;
  int journal_master = 0;
  int journal_shared = 0;
  int helptrust = 0;
  int ieee = 1;              // 1 => use of feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
  //int ieee = 0;              // PolyMAC ?
  bool apply_verification = true;
  int disable_stop = 0;
  Nom data_file;
  data_file = "";
  Nom exec_script;
  exec_script = "";
  Nom arguments_info;
  arguments_info = "Executable: ";
  arguments_info += argv[0];
  arguments_info += "\n";
  for (int i = 1; i < argc; i++)
    {
      // int error = 0;
      // Le -help est reserve par Petsc
      if (strcmp(argv[i], "-help_trust") == 0)
        {
          helptrust = 1;
        }
      else if (strcmp(argv[i], "-disable_ieee") == 0)
        {
          ieee = 0;
          arguments_info += "-disable_ieee => disable of feenableexcept\n";
        }
      else if (strcmp(argv[i], "-mpi") == 0)
        {
          with_mpi = 1;
          arguments_info += "-mpi => parallel computation with mpi\n";
        }
      else if (strcmp(argv[i], "-no_verify") == 0)
        {
          apply_verification = false;
          arguments_info += "-no_verify => Disable the call to verifie function (from Type_Verifie) to catch outdated keywords while reading data file.\n";
        }
      else if (strcmp(argv[i], "-check_enabled=1") == 0)
        {
          check_enabled = 1;
          arguments_info += "-check_enabled=1 => force enable parallel message checking\n";
        }
      else if (strcmp(argv[i], "-check_enabled=0") == 0)
        {
          check_enabled = 0;
          arguments_info += "-check_enabled=0 => force disable parallel message checking\n";
        }
      else if (strncmp(argv[i], "-debugscript=", 13) == 0)
        {
          exec_script = argv[i]+13;
          arguments_info += "-debugscript => execute script ";
          arguments_info += exec_script;
          arguments_info += "\n";
        }
      else if (strcmp(argv[i], "-petsc=0") == 0)
        {
          with_petsc = 0;
          arguments_info += "-petsc=0 => disable call to PetscInitialize\n";
        }
      else if (strncmp(argv[i], "-journal=", 9) == 0)
        {
          int level = atoi(argv[i]+9);
          if (level < 0 || level > 9)
            {
              Cerr << "Bad journal level : " << argv[i] << finl;
              //        error = 1;
            }
          else
            {
              verbose_level = level;
              arguments_info += "-journal=";
              arguments_info += Nom(level);
              arguments_info += "\n";
            }
        }
      else if (strcmp(argv[i], "-journal_master") == 0)
        {
          journal_master = 1;
          arguments_info += "-journal_master => Only the master processor will write a journal";
          if(journal_shared)
            {
              journal_shared = 0;
              arguments_info += " (journal_shared is thus ignored)";
            }
          arguments_info += "\n";
        }
      else if (strcmp(argv[i], "-journal_shared") == 0)
        {
          journal_shared = 1;
          arguments_info += "-journal_shared => all the processors are going to write in a unique journal file";
          if(journal_master)
            {
              journal_master = 0;
              arguments_info += " (journal_master is thus ignored)";
            }
          arguments_info += "\n";
        }
      else if (strcmp(argv[i], "-disable_stop") == 0)
        {
          disable_stop = 1;
          arguments_info += "-disable_stop => Disable the writing of the .stop file.\n";
        }
      else if (i == 1)
        {
          // Les deux derniers tests doivent rester a la fin, inserer les arguments supplementaires avant.
          data_file = argv[1];
          arguments_info += "Data file name = ";
          arguments_info += data_file;
          arguments_info += "\n";
        }
      else if (i == 2)
        {
          errno = 0;
          int nprocs = strtol(argv[2], (char **)NULL, 10);
          if (!errno && nprocs)
            {
              nproc = nprocs;
              if (nproc > 1)
                {
                  with_mpi = 1;
                  arguments_info += "Running in parallel with ";
                  arguments_info += Nom(nproc);
                  arguments_info += " processors\n";
                }
              else if (nproc == 1)
                {
                  arguments_info += "Sequential calculation\n";
                }
              else
                {
                  Cerr << "Bad number of processors in old syntax TRUST case N" << finl;
                  //error = 1;
                }
            }
          else
            {
              // Mise en commentaire car dans certains scripts verifie on teste: trust $jdd -ksp_view
              /*
                      Nom fichier=argv[i];
                      fichier.prefix(".data");
                      fichier+=".data";
                      EFichier fic(fichier);
                      if(fic.fail())
                        {
                          Cerr << "Trying to open file " << fichier << finl;
                          Cerr << "File " << fichier << " doesnt exist !" << finl;
                          Process::exit();
                        }
                      else
                        {
                          data_file = fichier.prefix(".data");
                        }
              */
              //error = 1;
            }
        }
      else
        {
          //error = 1;
        }
      //if (i==1) Cerr << "Arguments lus: " << argv[0];
      //Cerr << " " << argv[i];
      /* PL: Je desactive car MPICH rajoute des arguments derriere le nombre de processeurs:
         ... TRUST_mpich_opt jdd 2 -p4pg .../PINNNN -p4wd pathname
         if (error) {
         Cerr << "TRUST error processing command line, argument " << i << " : \n "
         << argv[i] << finl;
         usage();
         } */
    }

  {

    if ( ieee == 1 )
      {
        char* theValue = getenv("TRUST_DISABLE_FP_EXCEPT");
        if (theValue == NULL || atoi(theValue) == 0)
          {
            arguments_info += "feenableexcept enabled.\n";
#ifdef linux
            // Detect all NaNs (don't add FE_UNDERFLOW cause exp(-x) with x big throws an exception)
            // Test pre 1.7.0, on active en optimise:
#ifndef _COMPILE_AVEC_CLANG
#ifndef _COMPILE_AVEC_FCC // Crashes bizarres 
            feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
#endif
#endif
          }
      }

    // ************** Initialisation de Petsc, du parallele (si Petsc) **********
    // .. et demarrage du journal
    // (tout ce qu'on veut faire en commun avec l'interface python doit etre
    //  mis dans mon_main)
    main_process=new  mon_main(verbose_level, journal_master, journal_shared, apply_verification, disable_stop);

    main_process->init_parallel(argc, argv, with_mpi, check_enabled, with_petsc);

    // *************************  <PARALLEL_OK> *****************************
    // A partir d'ici on a le droit d'utiliser les communications entre processeurs,
    // me() etc...
    const int master = Process::je_suis_maitre();

    // Modif B.Mathieu 28/09/2004 :
    //  Pour debugger trust, on donne parametre le nom
    //  d'une commande systeme a executer lorsqu'on arrive a cet endroit.
    //  Le plus pratique, c'est d'utiliser un shell script qui ouvre
    //  un xterm et lance gdb. On peut aussi faire un tail -f du fichier log,
    //  etc...
    //  La commande est executee avec comme parametre le numero du PE.
    if(exec_script != "")
      {
        exec_script += " ";
        exec_script += Nom(Process::me());
        Cerr << "[" << Process::me() << "] Debug mode => execute command : " << exec_script << finl;
        std::cerr << system(exec_script) << std::endl;
      }

    if (master)
      {
        if ( helptrust == 1 ) usage();

        // On affiche le resultat de la ligne de commande ici pour ne pas remplir stderr
        // avec tous les processeurs...
        Cerr << arguments_info;

        if (nproc != -1 && Process::nproc() != nproc)
          {
            Cerr << "Error: nproc on command line does not match mpirun parameter.\n"
                 << " Use same number or -mpi parameter instead." << finl;
            usage();
          }
      }

    cerr.setf(ios::scientific);
    cerr.precision(12);
    cout.setf(ios::scientific);
    cout.precision(12);

    // ****************** Nom du cas ***********************
    if (data_file == "")
      {
        // TRUST sans argument => nom du cas = nom du repertoire courant
        Nom pwd(::pwd());
        if (master)
          Cerr << "No command line argument. Data file name from directory name:\n "
               << pwd << finl;
        const int l = pwd.longueur() - 1; // Attention, longueur()=strlen+1
        int i = l - 1;
        while(i > 0 && pwd[i] != directory_separator)
          {
            i--;
          }
        if (i == l - 1)
          {
            if (master)
              {
                Cerr << "Error : pwd() ends with directory separator " << (int)directory_separator << finl;
                Process::exit();
              }
          }
        data_file = pwd.substr_old(i + 2, l - i - 1); // Attention, voir Nom::substr()
      }
    if (master)
      {
        Cerr << "Data file : " << data_file << finl;
      }
    // Si le nom du cas finit par .data, on l'enleve.
    if (data_file.finit_par(".data"))
      {
        if (master)
          {
            Cerr << " (removing .data extension)" << finl;
          }
        data_file.prefix(".data");
      }



    if (master)
      {
        Cerr<<"Localisation etude: " << ::pwd() << finl;
        Cerr<<"Nom du cas " << data_file << finl;
        Cerr<<" code : "<< argv[0] << finl;
        Cerr<<" version : 1.9.0 " << finl;
      }

    main_process->dowork(data_file);

    if (master)
      {
        Cerr << "Arret des processes." << finl;
      }
  }

  //  pour detruire les derniers octets
  desalloue_pwd();
  return (0);
}


