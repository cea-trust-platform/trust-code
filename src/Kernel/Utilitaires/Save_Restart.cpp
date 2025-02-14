/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <EcritureLectureSpecial.h>
#include <Entree_Fichier_base.h>
#include <Entree_Fichier_base.h>
#include <Discretisation_base.h>
#include <EcrFicCollecteBin.h>
#include <LecFicDiffuseBin.h>
#include <communications.h>
#include <FichierHDFPar.h>
#include <Probleme_base.h>
#include <Sortie_Nulle.h>
#include <Save_Restart.h>
#include <TRUST_2_PDI.h>
#include <Ecrire_YAML.h>
#include <sys/stat.h>
#include <Avanc.h>

#define CHECK_ALLOCATE 0
#ifdef CHECK_ALLOCATE
#include <unistd.h> // Pour acces a int close(int fd); avec PGI
#include <fcntl.h>
#include <errno.h>
#endif

// Retourne la version du format de sauvegarde
// 151 pour dire que c'est la version initiee a la version 1.5.1 de TRUST
inline int version_format_sauvegarde() { return 184; }

// Version using PDI library
inline int version_format_PDI() { return 196; }

/*! @brief Initialisation de file_size, bad_allocate, nb_pb_total, num_pb
 *
 */
long int Save_Restart::File_size_=0;        // file_size est l'espace disque en octet necessaire pour ecrire les fichiers XYZ
int Save_Restart::Bad_allocate_=1;        // bad_allocate est un int qui permet de savoir si l'allocation a deja eut lieu
int Save_Restart::Nb_pb_total_=0;        // nb_pb_total est le nombre total de probleme
int Save_Restart::Num_pb_=1;                // num_pb est le numero du probleme courant

void Save_Restart::assoscier_pb_base(const Probleme_base& pb)
{
  pb_base_ = pb;
}

void Save_Restart::allocation() const
{
  if(pb_base_->schema_temps().file_allocation() && EcritureLectureSpecial::Active)        // Permet de tester l'allocation d'espace disque
    {
      if (Bad_allocate_==1)                                        // Si l'allocation n'a pas eut lieu
        if (Process::je_suis_maitre())                                // Qu'avec le proc maitre
          {
            if (Num_pb_==1)                                                // Si le probleme est le premier
              if (!allocate_file_size(File_size_))                        // je tente une allocation d'espace disque de taille 2*file_size
                Bad_allocate_=0;                                        // Si cela echoue, j'indique au code que l'allocation a deja eut lieu et n'a pas fonctionner
              else
                Num_pb_=Nb_pb_total_;                                        // Si OK, je modifie num_pb pour que les autres pb ne tentent pas d'allocation
            else
              Num_pb_-=1;                                                // Si le probleme n'est pas le premier, je decremente le numero de probleme
          }
      const int canal = 2007;
      if (Process::je_suis_maitre())                                // le processeur maitre envoi bad_allocate a tout le monde
        for (int p=1; p<Process::nproc(); p++)
          envoyer(Bad_allocate_,p,canal);
      else
        recevoir(Bad_allocate_,0,canal);

      if (Bad_allocate_==0)                                        // Si l'allocation a echoue
        {
          sauver_xyz(1);
          if (Num_pb_==Nb_pb_total_)                                        // Si le numero de probleme correspond au nombre total de probleme
            {
              if (Process::je_suis_maitre())
                {
                  Cerr << finl;                                                // j'arrete le code de facon claire
                  Cerr << "***Error*** " << error_ << finl;                // et je sort l'erreur du code
                  Cerr << "A xyz backup was made because you do not have enough disk space" << finl;
                  Cerr << "to continue the current calculation. Free up disk space and" << finl;
                  Cerr << "restart the calculation thanks to the backup just made." << finl;
                  Cerr << finl;
                }
              Process::barrier();
              Process::exit();
            }
          Num_pb_+=1;                                                // j'incremente le numero de probleme
        }
    }
}

/*! @brief Verifie que la place necessaire existe sur le disque dur.
 *
 * @param l'espace disque requis
 * @return (int) retourne 1 si l'espace disque est suffisant, 0 sinon
 */
int Save_Restart::allocate_file_size(long int& size) const
{
#ifndef MICROSOFT
#ifndef __APPLE__
#ifndef RS6000
#ifdef CHECK_ALLOCATE
  Nom Fichier_File_size(Objet_U::nom_du_cas());
  Fichier_File_size+="_File_size";
  const char *file = Fichier_File_size;                        // Fichier d'allocation
  //  if (size<1048576)                                        // Si size est trop petit on le fixe a 1 Mo
  //     size=1048576;
  off_t taille = off_t(size+size);                        // Convertion de la taille du fichier 2*size

  int fichier = open(file, O_WRONLY | O_CREAT, 0666);        // Ouverture du fichier File_size
  if (fichier == -1)                                        // Erreur d'ouverture
    {
      error_="Open of ";
      error_+=file;
      error_+=" : ";
      error_+=strerror(errno);                                // Erreur sur l'ouverture
      close(fichier);                                        // fermeture du fichier
      remove(file);                                        // Destruction du fichier File_size
      return 0;                                                // Echec d'allocation car fichier pas ouvert
    }

  if (posix_fallocate(fichier, 0, taille) != 0)                // Erreur d'allocation de l'espace disque
    {
      error_="Allocation of ";
      error_+=file;
      error_+=" : ";
      error_+=strerror(errno);                                // Erreur sur l'allocation
      close(fichier);                                        // fermeture du fichier
      remove(file);                                        // Destruction du fichier File_size
      return 0;                                                // Echec d'allocation car pas assez de place
    }
  close(fichier);                                        // fermeture du fichier
  remove(file);                                                // Destruction du fichier File_size
#endif
#endif
#endif
#endif
  return 1;
}

void Save_Restart::preparer_calcul()
{
#ifndef RS6000
  if (pb_base_->schema_temps().file_allocation() && EcritureLectureSpecial::Active)
    {
      Nom nom_fich_xyz(".xyz");
      sauver_xyz(0);
      if (Process::je_suis_maitre())
        {
          ifstream fichier(nom_fich_xyz); // Calcul de l'espace disque pris par le fichier XYZ du probleme courant
          fichier.seekg(0, std::ios_base::end);
          File_size_ += fichier.tellg(); // Incremente l'espace disque deja necessaire
          fichier.close();
          remove(nom_fich_xyz);
        }
      Nb_pb_total_ += 1; // Permet de connaitre le nombre de probleme total a la fin du preparer_calcul
    }
#endif
}

void Save_Restart::setTinitFromLastTime(double last_time)
{
  // Set the time to restart the calculation
  pb_base_->schema_temps().set_temps_courant() = last_time;
  // Initialize tinit and current time according last_time
  if (pb_base_->schema_temps().temps_init() > -DMAXFLOAT)
    {
      Cerr << "tinit was defined in .data file to " << pb_base_->schema_temps().temps_init() << ". The value is fixed to " << last_time << " accroding to resume_last_time_option" << finl;
    }
  pb_base_->schema_temps().set_temps_init() = last_time;
  pb_base_->schema_temps().set_temps_precedent() = last_time;
  Cerr << "==================================================================================================" << finl;
  Cerr << "In the backup file, we find the last time: " << last_time << " and read the fields." << finl;
}

void Save_Restart::checkVersion(Nom nomfic)
{
  if (Process::mp_min(restart_version_) != Process::mp_max(restart_version_))
    {
      Cerr << "The version of the format backup/resumption is not the same in the resumption files " << nomfic << finl;
      Process::exit();
    }
  if (restart_version_ > version_format_sauvegarde())
    {
      Cerr << "The format " << restart_version_ << " of the resumption file " << nomfic << " is posterior" << finl;
      Cerr << "to the format " << version_format_sauvegarde() << " recognized by this version of TRUST." << finl;
      Cerr << "Please use a more recent version." << finl;
      Process::exit();
    }

  // Ecriture du format de reprise
  Cerr << "The version of the resumption format of file " << nomfic << " is " << restart_version_ << finl;
}

void Save_Restart::prepare_PDI_restart(int resume_last_time)
{
  TRUST_2_PDI::PDI_restart_ = 1;
  TRUST_2_PDI pdi_interface;

  int last_iteration = -1;
  double tinit = -1.;
  // Restart from the last time
  if (resume_last_time)
    {
      // Look for the last time saved in checkpoint file to init current computation
      pdi_interface.prepareRestart(last_iteration, tinit, 1 /*resume_last_time */);

      // set last time found in checkpoint file to tinit if tinit not set
      setTinitFromLastTime(tinit);
    }
  else // resume from the requested time
    {
      // looking for tinit in backup file
      tinit = pb_base_->schema_temps().temps_init();
      pdi_interface.prepareRestart(last_iteration, tinit, 0 /* reprise */);
    }

  // Check format of checkpoint file
  if(Process::node_master())
    {
      pdi_interface.read("version", &restart_version_);

      if(restart_version_ < version_format_PDI() )
        {
          Cerr << "----------------------------------------------------------------------------------------------" << finl;
          Cerr << "The resumption with PDI format is only available with TRUST versions 1.9.6 and higher " << finl;
          Cerr << "----------------------------------------------------------------------------------------------" << finl;
          Process::exit();
        }
    }
}

void Save_Restart::sauver_xyz(int verbose) const
{
  statistiques().begin_count(sauvegarde_counter_);
  Nom nom_fich_xyz("");
  if (verbose)
    {
      nom_fich_xyz += Objet_U::nom_du_cas();
      nom_fich_xyz += "_";
      nom_fich_xyz += pb_base_->le_nom();
      nom_fich_xyz += ".xyz";
      Cerr << "Creation of " << nom_fich_xyz << " (" << EcritureLectureSpecial::get_Output() << ") for resumption of a calculation with a different number of processors." << finl;
    }
  else
    {
      nom_fich_xyz = ".xyz";
    }
  // Creation du fichier XYZ du probleme courant
  ficsauv_.typer(EcritureLectureSpecial::get_Output());
  ficsauv_->ouvrir(nom_fich_xyz);
  // Nouveau pour le xyz depuis la 155: on note en en-tete le format de sauvegarde
  if (Process::je_suis_maitre())
    ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;

  EcritureLectureSpecial::mode_ecr = 1;
  int bytes = pb_base_->sauvegarder(ficsauv_.valeur());
  EcritureLectureSpecial::mode_ecr = -1;

  if (Process::je_suis_maitre())
    ficsauv_.valeur() << Nom("fin");
  (ficsauv_.valeur()).flush();
  (ficsauv_.valeur()).syncfile();
  ficsauv_.detach();
  Cout << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to write xyz file." << finl;
  statistiques().end_count(sauvegarde_counter_, bytes);
}

/////////////////////////////////////////////
// Lecture des options de reprise d'un calcul
/////////////////////////////////////////////
void Save_Restart::lire_reprise(Entree& is, Motcle& motlu)
{
  int resume_last_time = (motlu == "resume_last_time" ? 1 : 0);
  // remise a defaut a zero pour pouvoir faire une reprise std apres une reprise xyz
  EcritureLectureSpecial::mode_lec = 0;
  Motcle format_rep;
  is >> format_rep;
  if ((format_rep != "formatte") && (format_rep != "binaire") && (format_rep != "xyz") && (format_rep != "single_hdf") && (format_rep != "pdi"))
    {
      Cerr << "Restarting calculation... : keyword " << format_rep << " not understood. Waiting for:" << finl << motlu << " formatte|binaire|xyz|single_hdf|pdi Filename" << finl;
      Process::exit();
    }

  // XXX Elie Saikali : for polymac => only .sauv files are possible
  if (pb_base_->discretisation().is_polymac_p0() && format_rep != "binaire")
    {
      Cerr << "Error in Probleme_base::" << __func__ << " !! " << finl;
      Cerr << "Only the binary format is currently supported to resume a simulation with the discretization " << pb_base_->discretisation().que_suis_je() << " ! " << finl;
      Cerr << "Please update your data file and use a .sauv file !" << finl;
      Process::exit();
    }

  // Read the filename:
  Nom nomfic;
  is >> nomfic;
  // Force reprise hdf au dela d'un certain nombre de rangs MPI:
  if (format_rep != "xyz" && Process::force_single_file(Process::nproc(), nomfic))
    format_rep = "pdi";

  if(format_rep == "pdi")
    {
      Ecrire_YAML yaml_file;
      yaml_file.add_pb_base(pb_base_, nomfic);
      std::string yaml_fname = "restart_" + pb_base_->le_nom().getString() + ".yml";
      yaml_file.write_restart_file(yaml_fname);
      TRUST_2_PDI::init(yaml_fname);

      // Prepare restart
      prepare_PDI_restart(resume_last_time);

      Entree useless;
      pb_base_->reprendre(useless);

      TRUST_2_PDI::finalize();
    }
  else if(format_rep == "single_hdf")
    {
      // !! DEPRECATED HDF5 FILE !!
      Cerr << "WARNING::you are using a deprecated backup file format. Please switch to PDI." << finl;
      LecFicDiffuse test;
      if (!test.ouvrir(nomfic))
        {
          Cerr << "Error! " << nomfic << " file not found ! " << finl;
          Process::exit();
        }
      FichierHDFPar fic_hdf;
      Entree_Brute input_data;
      fic_hdf.open(nomfic, true);
      fic_hdf.read_dataset("/sauv", Process::me(),input_data);

      if(resume_last_time)
        {
          double last_time = -1;
          last_time = get_last_time(input_data);
          setTinitFromLastTime(last_time);
          fic_hdf.read_dataset("/sauv", Process::me(), input_data);
        }

      input_data >> motlu;
      if (motlu=="format_sauvegarde:")
        {
          input_data >> restart_version_;
          checkVersion(nomfic);
        }
      else
        {
          Cerr<<"This .sauv file is too old and the format is not supported anymore."<<finl;
          Process::exit();
        }
      fic_hdf.close();

      // Restart computation from checkpoint file
      pb_base_->reprendre(input_data);
    }
  else
    {
      OWN_PTR(Entree_Fichier_base) fic;
      if (format_rep == "formatte")
        fic.typer("LecFicDistribue");
      else if (format_rep == "binaire")
        fic.typer("LecFicDistribueBin");
      else if (format_rep == "xyz")
        {
          EcritureLectureSpecial::mode_lec = 1;
          fic.typer(EcritureLectureSpecial::Input);
        }
      fic->ouvrir(nomfic);
      if (fic->fail())
        {
          Cerr << "Error during the opening of the restart file : " << nomfic << finl;
          Process::exit();
        }

      // Restart from the last time
      if (resume_last_time)
        {
          // Look for the last time and set it to tinit if tinit not set
          double last_time = -1.;
          last_time = get_last_time(fic);
          setTinitFromLastTime(last_time);

          fic->close();
          fic->ouvrir(nomfic);
        }

      // Lecture de la version du format de sauvegarde si c'est une reprise classique
      // Depuis la 1.5.1, on marque le format de sauvegarde en tete des fichiers de sauvegarde
      // afin de pouvoir faire evoluer plus facilement ce format dans le futur
      // En outre avec la 1.5.1, les faces etant numerotees differemment, il est faux
      // de faire une reprise d'un fichier de sauvegarde anterieur et c'est donc un moyen
      // de prevenir les utilisateurs: il leur faudra faire une reprise xyz pour poursuivre
      // avec la 1.5.1 un calcul lance avec une version anterieure
      // Depuis la 1.5.5, Il y a pas une version de format pour le xyz
      fic.valeur() >> motlu;
      if (motlu != "FORMAT_SAUVEGARDE:")
        {
          if (format_rep == "xyz")
            {
              // We close and re-open the file:
              fic->close();
              fic->ouvrir(nomfic);
              restart_version_ = 151;
            }
          else
            {
              Cerr << "-------------------------------------------------------------------------------------" << finl;
              Cerr << "The resumption file " << nomfic << " can not be read by this version of TRUST" << finl;
              Cerr << "which is a later version than 1.5. Indeed, the numbering of the faces have changed" << finl;
              Cerr << "and it would produce an erroneous resumption. If you want to use this version," << finl;
              Cerr << "you must do a resumption of the file .xyz saved during the previous calculation" << finl;
              Cerr << "because this file is independent of the numbering of the faces." << finl;
              Cerr << "The next backup will be made in a format compatible with the new" << finl;
              Cerr << "numbering of the faces and you can then redo classical resumptions." << finl;
              Cerr << "-------------------------------------------------------------------------------------" << finl;
              Process::exit();
            }
        }
      else
        {
          fic.valeur() >> restart_version_;
          checkVersion(nomfic);
        }

      // Restart computation from checkpoint file
      pb_base_->reprendre(fic.valeur());
    }

  restart_done_ = true;
  restart_in_progress_ = true;
}

////////////////////////////////////////////////
// Lecture des options de sauvegarde d'un calcul
////////////////////////////////////////////////
void Save_Restart::lire_sauvegarde(Entree& is, Motcle& motlu)
{
  // restart_file=1: le fichier est ecrasee a chaque sauvegarde (et ne donc contient qu'un seul instant)
  if (motlu == "sauvegarde_simple")
    simple_restart_ = true;
  is >> restart_format_;
  if ((Motcle(restart_format_) != "binaire") && (Motcle(restart_format_) != "formatte") &&
      (Motcle(restart_format_) != "xyz") && (Motcle(restart_format_) != "single_hdf") &&
      (Motcle(restart_format_) != "pdi") )
    {
      restart_file_name_ = restart_format_;
      restart_format_ = "binaire";
    }
  else
    {
      Nom nom;
      is >> nom;
      motlu = nom;
      if(motlu==Motcle("{"))
        {
          if( (Motcle(restart_format_) != "pdi") )
            {
              Cerr << "[Save_restart] lire_sauvegarde : Wrong syntax..." << finl;
              Process::exit();
            }
          Motcles compris(3);
          compris[0]="}";
          compris[1]="checkpoint_fname";
          compris[2]="yaml_fname";
          int ind = -1;
          while (ind!=0)
            {
              is >> motlu;
              ind = compris.rang(motlu);
              if (ind==1)
                is >> restart_file_name_;
              else if (ind==2)
                {
                  Cerr << "[Save_Restart] lire_sauvegarde :: You have provided your own yaml file to initialize PDI ! " << finl;
                  is >> yaml_fname_;

                  // Check to see if the file exists
                  LecFicDiffuse test;
                  if (!test.ouvrir(yaml_fname_))
                    {
                      Cerr << "[Save_Restart] lire_sauvegarde :: Error! The provided file " << yaml_fname_ << " does not exist " << finl;
                      Process::exit();
                    }
                }
              else if (ind==-1)
                {
                  Cerr << "[Save_Restart] lire_sauvegarde :: " << motlu << " is not understood. Keywords are:" << finl;
                  Cerr << compris << finl;
                  Process::exit();
                }
            }
        }
      else
        restart_file_name_ = nom;
    }
}

void Save_Restart::lire_sauvegarde_reprise(Entree& is, Motcle& motlu)
{
  // XXX Elie Saikali : for PolyMAC_P0 => No xyz for the moment
  if (pb_base_->discretisation().is_polymac_p0())
    {
      Cerr << "Problem "  << pb_base_->le_nom() << " with the discretization "
           << pb_base_->discretisation().que_suis_je() <<  " => EcritureLectureSpecial = 0 !" << finl;
      EcritureLectureSpecial::Active = 0;
    }
  restart_format_ = "binaire";
  restart_file_name_ = Objet_U::nom_du_cas();
  restart_file_name_ += "_";
  restart_file_name_ += pb_base_->le_nom();
  restart_file_name_ += ".sauv";

  while (1)
    {
      if ((motlu == "reprise") || (motlu == "resume_last_time"))
        lire_reprise(is, motlu);
      else if (motlu == "sauvegarde" || motlu == "sauvegarde_simple")
        lire_sauvegarde(is, motlu);
      else if (motlu == "}")
        break;
      else
        {
          Cerr << "Error in Probleme_base::lire_donnees" << finl;
          Cerr << "We expected } instead of " << motlu << " to mark the end of the data set" << finl;
          Process::exit();
        }
      is >> motlu;
    }

  ficsauv_.detach();
  // Force sauvegarde hdf au dela d'un certain nombre de rangs MPI:
  if (restart_format_ != "xyz" && Process::force_single_file(Process::nproc(), restart_file_name_))
    restart_format_ = "pdi";

  if ((Motcle(restart_format_) != "binaire") && (Motcle(restart_format_) != "formatte") &&
      (Motcle(restart_format_) != "xyz") && (Motcle(restart_format_) != "single_hdf") &&
      (Motcle(restart_format_) != "pdi"))
    {
      Cerr << "Error of backup format ! We expected formatte, binaire, xyz, or pdi." << finl;
      Process::exit();
    }

  if (pb_base_->schema_temps().temps_init() <= -DMAXFLOAT)
    {
      pb_base_->schema_temps().set_temps_init() = 0;
      pb_base_->schema_temps().set_temps_courant() = 0;
    }

  if (reprise_effectuee())
    {
      // on teste si dt_ev existe sinon on met reprise a 2
      // on recrera l'entete dans dt_ev sinon l'entete est fausse en reprise de pb_couple
      Nom fichier(Objet_U::nom_du_cas());
      fichier += ".dt_ev";
      struct stat f;
      if (stat(fichier, &f))
        reprise_effectuee() = 2;
    }
}

/*! @brief Ecriture sur fichier en vue d'une reprise (sauvegarde)
 *
 */
int Save_Restart::sauver() const
{
  int pdi_format = Motcle(restart_format_) == "pdi";
  if(pdi_format)
    {
      if(!TRUST_2_PDI::PDI_initialized_)
        {
          std::string yaml_fname = yaml_fname_.getString();
          if(yaml_fname == "??")
            {
              Ecrire_YAML yaml_file;
              yaml_fname = "save_" + pb_base_->le_nom().getString() + ".yml";
              yaml_file.add_pb_base(pb_base_, restart_file_name_);
              yaml_file.write_checkpoint_file(yaml_fname);
            }
          TRUST_2_PDI::init(yaml_fname);
        }

      // if we are dealing with a coupled problem, the initialization might have been done twice
      // in which case we don't want to overwrite the file
      if(Process::node_master() && !ficsauv_created_)
        {
          TRUST_2_PDI pdi_interface;

          // if a file with the same name already exists, delete it and create a new one
          int nb_proc = Process::nproc();
          pdi_interface.TRUST_start_sharing("nb_proc", &nb_proc);
          std::string event = "init_" + pb_base_->le_nom().getString();
          pdi_interface.trigger(event);
          pdi_interface.stop_sharing_last_variable();

          // format information
          int version = version_format_PDI();
          int non_const_sr = simple_restart_;
          pdi_interface.write("version", &version);
          pdi_interface.write("simple_sauvegarde", &non_const_sr);

          ficsauv_created_ = true;
        }
    }
  else if (!ficsauv_.non_nul())
    {
      // Si le fichier de sauvegarde n'a pas ete ouvert alors on cree le fichier de sauvegarde:
      if (Motcle(restart_format_) == "formatte")
        {
          ficsauv_.typer("EcrFicCollecte");
          //
          // Even in 64b, a save/restart SAUV file never actually requires 64b indices since all the information
          // saved is per proc. So we might as well save some space (not so much actually, since most of the data
          // saved are double values).
          //
          ficsauv_->set_64b(false);
          ficsauv_->ouvrir(restart_file_name_);
          ficsauv_->setf(ios::scientific);
        }
      else if (Motcle(restart_format_) == "binaire")
        {
          ficsauv_.typer("EcrFicCollecteBin");
          ficsauv_->set_64b(false); // see comment above!
          ficsauv_->ouvrir(restart_file_name_);
        }
      else if (Motcle(restart_format_) == "xyz")
        {
          ficsauv_.typer(EcritureLectureSpecial::get_Output());
          ficsauv_->ouvrir(restart_file_name_);
        }
      else
        {
          Cerr << "Error in Probleme_base::sauver() " << finl;
          Cerr << "The format for the backup file must be either binary or formatted" << finl;
          Cerr << "But it is :" << restart_format_ << finl;
          Process::exit();
        }
      // Si c'est la premiere sauvegarde, on note en en-tete le format de sauvegarde
      if (Motcle(restart_format_) == "xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
        }
      else
        ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
    }

  // On realise l'ecriture de la sauvegarde
  int bytes;
  EcritureLectureSpecial::mode_ecr = (Motcle(restart_format_) == "xyz");
  TRUST_2_PDI::PDI_checkpoint_ = pdi_format;
  if(pdi_format)
    {
      Sortie_Nulle useless;
      bytes = pb_base_->sauvegarder(useless);

      TRUST_2_PDI pdi_interface;
      std::string f_event = "fields_backup_" + pb_base_->le_nom().getString();
      pdi_interface.trigger(f_event);
      if(Process::node_master())
        {
          std::string s_event = "scalars_backup_" + pb_base_->le_nom().getString();
          pdi_interface.trigger(s_event);
        }
      pdi_interface.stop_sharing();
    }
  else
    bytes = pb_base_->sauvegarder(ficsauv_.valeur());
  EcritureLectureSpecial::mode_ecr = -1;
  TRUST_2_PDI::PDI_checkpoint_ = 0;

  // Si c'est une sauvegarde simple, on referme immediatement et proprement le fichier
  if (simple_restart_ && !pdi_format)
    {
      if (Motcle(restart_format_) == "xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
          (ficsauv_.valeur()).syncfile();
        }
      else
        {
          ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
        }
      ficsauv_.detach();
    }
  return bytes;
}

void Save_Restart::finir()
{
  // On ferme proprement le fichier de sauvegarde
  // Si c'est une sauvegarde_simple, le fin a ete mis a chaque appel a ::sauver()
  if(Motcle(restart_format_) == "pdi" && TRUST_2_PDI::PDI_initialized_)
    TRUST_2_PDI::finalize();
  else  if (!simple_restart_ && ficsauv_.non_nul())
    {
      if (Motcle(restart_format_) == "xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
          (ficsauv_.valeur()).syncfile();
        }
      else
        {
          ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
        }

      ficsauv_.detach();
    }
  // Si la sauvegarde est classique et que l'utilisateur n'a pas desactive la sauvegarde finale xyz
  // alors on effectue la sauvegarde finale xyz
  if (Motcle(restart_format_) != "xyz" && (EcritureLectureSpecial::Active))
    sauver_xyz(1);
}
