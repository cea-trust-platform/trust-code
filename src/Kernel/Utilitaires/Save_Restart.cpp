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

#include <EcritureLectureSpecial.h>
#include <Entree_Fichier_base.h>
#include <Entree_Fichier_base.h>
#include <Discretisation_base.h>
#include <EcrFicCollecteBin.h>
#include <LecFicDiffuseBin.h>
#include <communications.h>
#include <FichierHDFPar.h>
#include <Probleme_base.h>
#include <Save_Restart.h>
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

/*! @brief Ecrit le probleme dans un fichier *.calcul_xyz et calcule la place disque prise par ce fichier
 *
 * @return (int) retourne toujours 1
 */
int Save_Restart::file_size_xyz() const
{
#ifndef RS6000
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
#endif
  return 1;
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
  // Desormais les fichiers de sauvegarde sont nommes par defaut nomducas_nomdupb.sauv
  restart_file_name_ += "_";
  restart_file_name_ += pb_base_->le_nom();
  restart_file_name_ += ".sauv";
  Motcle accolade_fermee("}");
  int resume_last_time = 0;
  while (1)
    {
      /////////////////////////////////////////////
      // Lecture des options de reprise d'un calcul
      /////////////////////////////////////////////
      if ((motlu == "reprise") || (motlu == "resume_last_time"))
        {
          resume_last_time = (motlu == "resume_last_time" ? 1 : 0);
          // remise a defaut a zero pour pouvoir faire une reprise std apres une reprise xyz
          EcritureLectureSpecial::mode_lec = 0;
          Motcle format_rep;
          is >> format_rep;
          if ((format_rep != "formatte") && (format_rep != "binaire") && (format_rep != "xyz") && (format_rep != "single_hdf"))
            {
              Cerr << "Restarting calculation... : keyword " << format_rep << " not understood. Waiting for:" << finl << motlu << " formatte|binaire|xyz|single_hdf Filename" << finl;
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
            format_rep = "single_hdf";
          // Open the file:
          OWN_PTR(Entree_Fichier_base) fic;
#ifdef MPI_
          Entree_Brute input_data;
          FichierHDFPar fic_hdf; //FichierHDF fic_hdf;
#endif

          if (format_rep == "formatte")
            fic.typer("LecFicDistribue");
          else if (format_rep == "binaire")
            fic.typer("LecFicDistribueBin");
          else if (format_rep == "xyz")
            {
              EcritureLectureSpecial::mode_lec = 1;
              fic.typer(EcritureLectureSpecial::Input);
            }

          if (format_rep == "single_hdf")
            {
#ifdef MPI_
              LecFicDiffuse test;
              if (!test.ouvrir(nomfic))
                {
                  Cerr << "Error! " << nomfic << " file not found ! " << finl;
                  Process::exit();
                }
              fic_hdf.open(nomfic, true);
              fic_hdf.read_dataset("/sauv", Process::me(),input_data);
#endif
            }
          else
            {
              fic->ouvrir(nomfic);
              if (fic->fail())
                {
                  Cerr << "Error during the opening of the restart file : " << nomfic << finl;
                  Process::exit();
                }
            }

          // Restart from the last time
          if (resume_last_time)
            {
              // Resume_last_time is supported with xyz format
              // if (format_rep == "xyz")
              //   {
              //     Cerr << "Resume_last_time is not supported with xyz format yet." << finl;
              //     exit();
              //   }
              // Look for the last time and set it to tinit if tinit not set
              double last_time = -1.;
              if (format_rep == "single_hdf")
                {
#ifdef MPI_
                  last_time = get_last_time(input_data);
#endif
                }
              else
                last_time = get_last_time(fic);
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
              Cerr << "In the " << nomfic << " file, we find the last time: " << last_time << " and read the fields." << finl;
              if (format_rep != "single_hdf")
                {
                  fic->close();
                  fic->ouvrir(nomfic);
                }
              else
                {
#ifdef MPI_
                  fic_hdf.read_dataset("/sauv", Process::me(), input_data);
#endif
                }
            }
          // Lecture de la version du format de sauvegarde si c'est une reprise classique
          // Depuis la 1.5.1, on marque le format de sauvegarde en tete des fichiers de sauvegarde
          // afin de pouvoir faire evoluer plus facilement ce format dans le futur
          // En outre avec la 1.5.1, les faces etant numerotees differemment, il est faux
          // de faire une reprise d'un fichier de sauvegarde anterieur et c'est donc un moyen
          // de prevenir les utilisateurs: il leur faudra faire une reprise xyz pour poursuivre
          // avec la 1.5.1 un calcul lance avec une version anterieure
          // Depuis la 1.5.5, Il y a pas une version de format pour le xyz
          if (format_rep != "single_hdf")
            fic.valeur() >> motlu;
          else
            {
#ifdef MPI_
              input_data >> motlu;
#endif
            }

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
              // Lecture du format de Lsauvegarde
              if (format_rep != "single_hdf")
                fic.valeur() >> restart_version_;
              else
                {
#ifdef MPI_
                  input_data >> restart_version_;
#endif
                }
              if (mp_min(restart_version_) != mp_max(restart_version_))
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
            }
          // Ecriture du format de reprise
          Cerr << "The version of the resumption format of file " << nomfic << " is " << restart_version_ << finl;
          if (format_rep != "single_hdf")
            pb_base_->reprendre(fic.valeur());
          else
            {
#ifdef MPI_
              pb_base_->reprendre(input_data);
              fic_hdf.close();
#endif
            }
          restart_done_ = true;
          restart_in_progress_ = true;
        }
      ////////////////////////////////////////////////
      // Lecture des options de sauvegarde d'un calcul
      ////////////////////////////////////////////////
      else if (motlu == "sauvegarde" || motlu == "sauvegarde_simple")
        {
          // restart_file=1: le fichier est ecrasee a chaque sauvegarde (et ne donc contient qu'un seul instant)
          if (motlu == "sauvegarde_simple")
            simple_restart_ = true;
          is >> restart_format_;
          if ((Motcle(restart_format_) != "binaire") && (Motcle(restart_format_) != "formatte") && (Motcle(restart_format_) != "xyz") && (Motcle(restart_format_) != "single_hdf"))
            {
              restart_file_name_ = restart_format_;
              restart_format_ = "binaire";
            }
          else
            is >> restart_file_name_;
        }
      else if (motlu == accolade_fermee)
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
    restart_format_ = "single_hdf";

  if ((Motcle(restart_format_) != "binaire") && (Motcle(restart_format_) != "formatte") && (Motcle(restart_format_) != "xyz") && (Motcle(restart_format_) != "single_hdf"))
    {
      Cerr << "Error of backup format" << finl;
      Cerr << "We expected formatte, binaire, xyz, or single_hdf." << finl;
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
  // Si le fichier de sauvegarde n'a pas ete ouvert alors on cree le fichier de sauvegarde:
  if (!ficsauv_.non_nul() && !osauv_hdf_)
    {
      if (Motcle(restart_format_) == "formatte")
        {
          ficsauv_.typer("EcrFicCollecte");
          ficsauv_->ouvrir(restart_file_name_);
          ficsauv_->setf(ios::scientific);
        }
      else if (Motcle(restart_format_) == "binaire")
        {
          ficsauv_.typer("EcrFicCollecteBin");
          ficsauv_->ouvrir(restart_file_name_);
        }
      else if (Motcle(restart_format_) == "xyz")
        {
          ficsauv_.typer(EcritureLectureSpecial::get_Output());
          ficsauv_->ouvrir(restart_file_name_);
        }
      else if (Motcle(restart_format_) == "single_hdf")
        osauv_hdf_ = new Sortie_Brute;
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
      else if ((Motcle(restart_format_) == "single_hdf"))
        *osauv_hdf_ << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
      else
        ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
    }

  // On realise l'ecriture de la sauvegarde
  int bytes;
  EcritureLectureSpecial::mode_ecr = (Motcle(restart_format_) == "xyz");
  if (Motcle(restart_format_) != "single_hdf")
    bytes = pb_base_->sauvegarder(ficsauv_.valeur());
  else
    bytes = pb_base_->sauvegarder(*osauv_hdf_);

  EcritureLectureSpecial::mode_ecr = -1;

  // Si c'est une sauvegarde simple, on referme immediatement et proprement le fichier
  if (simple_restart_)
    {
      if (Motcle(restart_format_) == "xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
          (ficsauv_.valeur()).syncfile();
        }
      else if (Motcle(restart_format_) == "single_hdf")
        {
          *osauv_hdf_ << Nom("fin");
          FichierHDFPar fic_hdf;
          fic_hdf.create(restart_file_name_);
          fic_hdf.create_and_fill_dataset_MW("/sauv", *osauv_hdf_);
          fic_hdf.close();
          delete osauv_hdf_;
          osauv_hdf_ = 0;
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
  if (!simple_restart_ && (ficsauv_.non_nul() || osauv_hdf_))
    {
      if (Motcle(restart_format_) == "xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
          (ficsauv_.valeur()).syncfile();
        }
      else if (Motcle(restart_format_) == "single_hdf")
        {
          *osauv_hdf_ << Nom("fin");
          FichierHDFPar fic_hdf;
          fic_hdf.create(restart_file_name_);
          fic_hdf.create_and_fill_dataset_MW("/sauv", *osauv_hdf_);
          fic_hdf.close();
          delete osauv_hdf_;
          osauv_hdf_ = 0;
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
