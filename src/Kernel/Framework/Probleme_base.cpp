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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Probleme_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/110
//
//////////////////////////////////////////////////////////////////////////////

#include <Probleme_base.h>
#include <Equation.h>
#include <Periodique.h>
#include <Schema_Euler_Implicite.h>
#include <Milieu_base.h>
#include <EcrFicCollecteBin.h>
#include <LecFicDiffuseBin.h>
#include <EcritureLectureSpecial.h>
#include <Postraitement.h>
#include <Discretisation_base.h>
#include <ICoCoExceptions.h>
#include <stat_counters.h>
#include <Debog.h>
#include <List_Ref_Postraitement.h>
#include <Champ_Gen_de_Champs_Gen.h>
#include <communications.h>
#include <Avanc.h>
#include <Deriv_Entree_Fichier_base.h>
#include <sys/stat.h>
#include <Loi_Fermeture_base.h>
#include <FichierHDFPar.h>

#define CHECK_ALLOCATE 0
#ifdef CHECK_ALLOCATE
#include <unistd.h> // Pour acces a int close(int fd); avec PGI
#include <fcntl.h>
#include <errno.h>
#endif

using ICoCo::WrongArgument;
using ICoCo::WrongContext;

Implemente_base_sans_constructeur_ni_destructeur(Probleme_base,"Probleme_base",Probleme_U);


// XD Pb_base pb_gen_base Pb_base -3 Resolution of equations on a domain. A problem is defined by creating an object and assigning the problem type that the user wishes to resolve. To enter values for the problem objects created, the Lire (Read) interpretor is used with a data block.
// XD  attr postraitement|Post_processing corps_postraitement postraitement 1 One post-processing (without name).
// XD  attr postraitements|Post_processings postraitements postraitements 1 List of Postraitement objects (with name).
// XD  attr liste_de_postraitements liste_post_ok liste_de_postraitements 1 This
// XD  attr liste_postraitements liste_post liste_postraitements 1 This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation or the lata files will be lost.
// XD  attr sauvegarde format_file sauvegarde 1 Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified for each problem. In this case, you must save to different files and correctly specify these files when resuming the calculation.
// XD  attr sauvegarde_simple format_file sauvegarde_simple 1 The same keyword than Sauvegarde except, the last time step only is saved.
// XD  attr reprise format_file reprise 1 Keyword to resume a calculation based on the name_file file (see the class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the previous calculation. With this file, it is possible to resume a parallel calculation on P processors, whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in error.
//  XD  attr resume_last_time format_file resume_last_time 1 Keyword to resume a calculation based on the name_file file, resume the calculation at the last time found in the file (tinit is set to last time of saved files).
//  XD ref domaine domaine
//  XD ref scheme schema_temps_base
//  XD ref milieu milieu_base
//  XD ref loi1 loi_fermeture_base
//  XD ref loi2 loi_fermeture_base
//  XD ref loi3 loi_fermeture_base
//  XD ref loi4 loi_fermeture_base
//  XD ref loi5 loi_fermeture_base

// Variables globales pour initialiser est_le_premier_postraitement_pour_nom_fic
// et est_le_dernier_postraitement_pour_nom_fic en une seule passe.
LIST(Nom) glob_noms_fichiers;
LIST(REF(Postraitement)) glob_derniers_posts;

Probleme_base::~Probleme_base()
{
  glob_noms_fichiers.vide();
  glob_derniers_posts.vide();
}
//////////////////////////////////////////////////
//                                              //
// Implementation de l'interface de Probleme_U  //
//                                              //
//////////////////////////////////////////////////


void Probleme_base::initialize()
{
  if (initialized)
    throw WrongContext(le_nom().getChar(),"initialize","initialize should not have been called");

  Debog::set_nom_pb_actuel(le_nom());
  preparer_calcul();
  domaine().initialiser(0,domaine_dis(),*this); // Pour le cas de geometries variables (ex. ALE)
  // on initialise le schema en temps avant le postraitement
  // ainsi les sources qui dependent du pas de temps fonctionnent
  schema_temps().initialize();
  init_postraitements();

  initialized=true;
}

double Probleme_base::presentTime() const
{
  if (!initialized)
    throw WrongContext(le_nom().getChar(),"presentTime","initialize should have been called");
  if (terminated)
    throw WrongContext(le_nom().getChar(),"presentTime","terminate should not have been called");

  Debog::set_nom_pb_actuel(le_nom());
  return schema_temps().temps_courant();
}

bool Probleme_base::initTimeStep(double dt)
{
  if (!initialized)
    throw WrongContext(le_nom().getChar(),"initTimeStep","initialize should have been called");
  if (terminated)
    throw WrongContext(le_nom().getChar(),"initTimeStep","terminate should not have been called");
  if (dt_defined)
    throw WrongContext(le_nom().getChar(),"initTimeStep","initTimeStep should not have been called");
  if (dt<=0)
    throw WrongArgument(le_nom().getChar(),"initTimeStep","dt","dt should be a positive real number");

  Debog::set_nom_pb_actuel(le_nom());

  bool ok=schema_temps().initTimeStep(dt);
  domaine().set_dt(dt);
  milieu().initTimeStep(dt);
  for(int i=0; i<nombre_d_equations(); i++)
    ok = ok && equation(i).initTimeStep(dt);

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps=schema_temps().temps_defaut();
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).zone_Cl_dis()->calculer_coeffs_echange(tps);

  dt_validated=false;
  dt_defined=true;
  return ok;
}

bool Probleme_base::solveTimeStep()
{
  if (!dt_defined)
    throw WrongContext(le_nom().getChar(),"solveTimeStep","initTimeStep should have been called");

  Debog::set_nom_pb_actuel(le_nom());

  bool ok=Probleme_U::solveTimeStep();

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps=schema_temps().temps_defaut();
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).zone_Cl_dis()->calculer_coeffs_echange(tps);

  return ok;
}

bool Probleme_base::iterateTimeStep(bool& converged)
{
  if (!dt_defined)
    throw WrongContext(le_nom().getChar(),"iterateTimeStep","initTimeStep should have been called");

  Debog::set_nom_pb_actuel(le_nom());
  bool ok=schema_temps().iterateTimeStep(converged);

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps=schema_temps().temps_defaut();
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).zone_Cl_dis()->calculer_coeffs_echange(tps);

  return ok;
}

bool Probleme_base::updateGivenFields()
{
  Debog::set_nom_pb_actuel(le_nom());
  bool ok=true;
  for(int i=0; i<nombre_d_equations(); i++)
    ok = ok && equation(i).updateGivenFields();
  return ok;
}

void Probleme_base::validateTimeStep()
{
  if (!dt_defined)
    throw WrongContext(le_nom().getChar(),"validateTimeStep","initTimeStep should have been called");

  Debog::set_nom_pb_actuel(le_nom());
  schema_temps().validateTimeStep();
  allocation();

  dt_defined=false;
  dt_validated=true;
}

void Probleme_base::terminate()
{
  if (!initialized)
    throw WrongContext(le_nom().getChar(),"terminate","initialize should have been called");
  if (terminated)
    throw WrongContext(le_nom().getChar(),"terminate","terminate should not have been called");
  if (dt_defined)
    throw WrongContext(le_nom().getChar(),"terminate","time step should be validated or aborted first");

  Debog::set_nom_pb_actuel(le_nom());
  finir();
  schema_temps().terminate();

  terminated=true;
}

// Adds the input fields names this pb understands to noms
void Probleme_base::getInputFieldsNames(Noms& noms) const
{
  int n=input_fields.size();
  Noms nouveaux_noms(n);
  for (int i=0; i<n; i++)
    nouveaux_noms[i]=input_fields[i]->le_nom();
  // GF pour contourner un bug dans add() si vecteur null
  if (n)
    noms.add(nouveaux_noms);
}

// Adds the input fields names this pb understands to noms
void Probleme_base::getOutputFieldsNames(Noms& noms) const
{
  for (int i=0; i<postraitements().size(); i++)
    {
      if (sub_type(Postraitement,postraitements()(i).valeur()))
        {
          // LIST(Nom) suite;
          const Liste_Champ_Generique& liste_champ= ref_cast(Postraitement,postraitements()(i).valeur()).champs_post_complet();
          for (int ii=0; ii<liste_champ.size(); ii++)
            noms.add(liste_champ(ii).valeur().get_nom_post() );

        }
    }
}


// Description:
//  Si force=1, effectue le postraitement sans tenir compte des
//    frequences de postraitement.
//    Le postraitement est mis a jour et les traitements eventuels sur
//    les sondes, champs et statistiques sont effectues.
//  Si force=0, tient compte des frequences d'impression demandees.
int Probleme_base::postraiter(int force)
{
  statistiques().begin_count(postraitement_counter_);
  Schema_Temps_base& sch = schema_temps();
  Debog::set_nom_pb_actuel(le_nom());
  if (sch.nb_pas_dt()!=0)
    imprimer(Cout);
  if (force)
    {
      //Les sources postraitables (Terme_Source_Acceleration) ne sont pas mis a jour
      //pour le temps final et ne font pas partie des champs_crees_ du postraitement
      //qui eux sont mis a jour par les_postraitements.mettre_a_jour.
      //On les met donc a jour ici

      const int nb_pas_dt_max = sch.nb_pas_dt_max();
      int& indice_nb_pas_dt = sch.set_indice_nb_pas_dt_max_atteint();
      int& indice_tps_final = sch.set_indice_tps_final_atteint();
      const double t_init = sch.temps_init();
      const double t_max = sch.temps_max();

      //Test pour eviter de repeter le postraitement a l instant initial
      //Cela evite un plantage dans le cas d un postraitement au format meshtv

      if (!((indice_nb_pas_dt) && (nb_pas_dt_max==0))
          && (!((indice_tps_final) && (est_egal(t_init,t_max)))))
        {
          for (int i=0; i<nombre_d_equations(); i++)
            equation(i).sources().mettre_a_jour(schema_temps().temps_courant());

          les_postraitements.mettre_a_jour(schema_temps().temps_courant());
          les_postraitements.postraiter();
          if (nb_pas_dt_max==0) indice_nb_pas_dt=1;
          if (est_egal(t_init,t_max)) indice_tps_final = 1;
        }
    }
  else
    {
      les_postraitements.traiter_postraitement();
    }
  statistiques().end_count(postraitement_counter_);
  return 1;
}

// Description:
//
double Probleme_base::computeTimeStep(bool& stop) const
{
  if (!initialized)
    throw WrongContext(le_nom().getChar(),"computeTimeStep","initialize should have been called");
  if (terminated)
    throw WrongContext(le_nom().getChar(),"computeTimeStep","terminate should not have been called");

  Debog::set_nom_pb_actuel(le_nom());
  double dt=schema_temps().computeTimeStep(stop);
  return dt;
}


bool Probleme_base::isStationary() const
{
  if (!dt_validated)
    throw WrongContext(le_nom().getChar(),"isStationary","validateTimeStep should have been called");
  if (terminated)
    throw WrongContext(le_nom().getChar(),"isStationary","terminate should not have been called");

  return schema_temps().isStationary();
}

void Probleme_base::setStationary(bool flag)
{
  schema_temps().set_stationnaires_atteints(flag);
}

void Probleme_base::abortTimeStep()
{
  if (!dt_defined)
    throw WrongContext(le_nom().getChar(),"abortTimeStep","initTimeStep should have been called");

  schema_temps().abortTimeStep();
  milieu().abortTimeStep();
  dt_defined=false;
}

REF(Field_base) Probleme_base::findInputField(const Nom& name) const
{
  // WEC : there should be a better way to scan the list
  for (int i=0; i<input_fields.size(); i++)
    {
      if (input_fields[i]->le_nom()==name)
        return input_fields[i];
    }
  REF(Field_base) ch;
  return ch;
}

REF(Champ_Generique_base) Probleme_base::findOutputField(const Nom& name) const
{
  REF(Champ_Generique_base) ch;
  if (comprend_champ_post(name))
    {
      ch=get_champ_post(name);
    }
  return ch;
}
///////////////////////////////////////////////////////////
//                                                       //
// Fin de l'implementation de l'interface de Probleme_U  //
//                                                       //
///////////////////////////////////////////////////////////

// B.Math. 21/09/2004: quelques initialisations, ca fait pas de mal...
Probleme_base::Probleme_base() :
  //ficsauv(0),
  osauv_hdf_(0),
  reprise_effectuee_(0),
  reprise_version_(155),
  restart_file(0),
  initialized(false),
  terminated(false),
  dt_defined(false),
  dt_validated(false),
  coupled_(0)
{
  tstat_deb_=tstat_fin_=-1;
}

// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Ecriture d'un probleme sur un flot de sortie.
//    !! Attention n'est pas symetrique de la lecture !!
//    On ecrit les equations, le postraitement et le domaine
//    discretise.
// Precondition:
// Parametre: Sortie& os
//    Signification: flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Probleme_base::printOn(Sortie& os) const
{
  for(int i=0; i<nombre_d_equations(); i++)
    os << equation(i).que_suis_je() << " " << equation(i) << finl;
  os << les_postraitements;
  os << le_domaine_dis;
  return os;
}

// Retourne la version du format de sauvegarde
// 151 pour dire que c'est la version initiee a la version 1.5.1 de TRUST
inline int version_format_sauvegarde()
{
  return 184;
}

// Description:
//    Lecture d'un probleme dans un flot d'entree, et ouverture du
//    flot de sauvegarde.
//    Format:
//     {
//     nom_equation bloc de lecture d'une equation
//     Postraitement bloc de lecture postraitement
//     reprise | sauvegarde | sauvegarde_simple
//     formatte | binaire
//     nom_de_fichier
//     }
// L'option sauvegarde_simple permet de sauver le probleme dans le fichier choisi
// en ecrasant a chaque fois les sauvegardes precedentes : cela permet d'economiser de la place disque.
// Precondition:
// Parametre: Entree& is
//    Signification: flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entre modifie
//    Contraintes:
// Exception: pas d'accolade ouvrante en debut de format
// Exception: mot clef "Postraitement" n'est pas la
// Exception: format de sauvegarde doit etre "binaire" ou "formatte"
// Exception: pas d'accolade fermante en fin de jeu de donnee
// Effets de bord:
// Postcondition: Le pb est associe au postraitement.
Entree& Probleme_base::readOn(Entree& is)
{
  Cerr << "Reading of the problem " << le_nom() << finl;
  format_sauv="binaire";
  nom_fich=Objet_U::nom_du_cas();
  // Desormais les fichiers de sauvegarde sont nommes par defaut nomducas_nomdupb.sauv
  nom_fich+="_";
  nom_fich+=le_nom();
  nom_fich+= ".sauv";
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;

  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected { to start to read the problem" << finl;
      exit();
    }

  lire_equations(is, motlu); //"motlu" contient le premier mot apres la lecture des equations

  // Si le postraitement comprend le mot, on en lit un autre...
  while (les_postraitements.lire_postraitements(is, motlu, *this))
    {
      is >> motlu;
    }

  completer();
  Cerr << "Step verification of data being read in progress..." << finl;
  verifier();
  Cerr << "The read data are coherent" << finl;
  int resume_last_time = 0;
  while(1)
    {
      /////////////////////////////////////////////
      // Lecture des options de reprise d'un calcul
      /////////////////////////////////////////////
      if ((motlu == "reprise") || (motlu == "resume_last_time"))
        {
          resume_last_time = (motlu == "resume_last_time" ? 1 : 0 );
          // remise a defaut a zero pour pouvoir faire une reprise std apres une reprise xyz
          EcritureLectureSpecial::mode_lec=0;
          Motcle format_rep;
          is >> format_rep;
          if ((format_rep != "formatte") && (format_rep != "binaire")
              && (format_rep!="xyz") && (format_rep!="single_hdf") )
            {
              Cerr << "Restarting calculation... : keyword " << format_rep << " not understood. Waiting for:" << finl
                   << motlu << " formatte|binaire|xyz|single_hdf Filename" << finl;
              exit();
            }
          // Read the filename:
          Nom nomfic;
          is >> nomfic;
          // Force reprise hdf au dela d'un certain nombre de rangs MPI:
          if (format_rep!="xyz" && Process::force_single_file(Process::nproc(), nomfic)) format_rep = "single_hdf";
          // Open the file:
          DERIV(Entree_Fichier_base) fic;
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
              EcritureLectureSpecial::mode_lec=1;
              fic.typer(EcritureLectureSpecial::Input);
            }

          if( format_rep == "single_hdf")
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
              if(fic->fail())
                {
                  Cerr<<"Error during the opening of the restart file : " <<nomfic<<finl;
                  exit();
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
              if(format_rep == "single_hdf")
                {
#ifdef MPI_
                  last_time = get_last_time(input_data);
#endif
                }
              else
                last_time = get_last_time(fic);
              // Set the time to restart the calculation
              schema_temps().set_temps_courant()=last_time;
              // Initialize tinit and current time according last_time
              if (schema_temps().temps_init()>-DMAXFLOAT)
                {
                  Cerr<<"tinit was defined in .data file to "<< schema_temps().temps_init()<< ". The value is fixed to "<<last_time<<" accroding to resume_last_time_option" <<finl;
                }
              schema_temps().set_temps_init()=last_time;
              schema_temps().set_temps_precedent()=last_time;
              Cerr << "==================================================================================================" << finl;
              Cerr << "In the " << nomfic << " file, we find the last time: " << last_time << " and read the fields." << finl;
              if( format_rep != "single_hdf")
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
          if(format_rep != "single_hdf")
            fic.valeur() >> motlu;
          else
            {
#ifdef MPI_
              input_data >> motlu;
#endif
            }

          if (motlu!="FORMAT_SAUVEGARDE:")
            {
              if (format_rep == "xyz")
                {
                  // We close and re-open the file:
                  fic->close();
                  fic->ouvrir(nomfic);
                  reprise_version_ = 151;
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
                  exit();
                }
            }
          else
            {
              // Lecture du format de Lsauvegarde
              if(format_rep != "single_hdf")
                fic.valeur() >> reprise_version_;
              else
                {
#ifdef MPI_
                  input_data >> reprise_version_;
#endif
                }
              if (mp_min(reprise_version_)!=mp_max(reprise_version_))
                {
                  Cerr << "The version of the format backup/resumption is not the same in the resumption files " << nomfic << finl;
                  exit();
                }
              if (reprise_version_>version_format_sauvegarde())
                {
                  Cerr << "The format " << reprise_version_ << " of the resumption file " << nomfic << " is posterior" << finl;
                  Cerr << "to the format "<<version_format_sauvegarde()<<" recognized by this version of TRUST." << finl;
                  Cerr << "Please use a more recent version." << finl;
                  exit();
                }
            }
          // Ecriture du format de reprise
          Cerr << "The version of the resumption format of file " << nomfic << " is " << reprise_version_ << finl;
          if(format_rep != "single_hdf")
            reprendre(fic.valeur());
          else
            {
#ifdef MPI_
              reprendre(input_data);
              fic_hdf.close();
#endif
            }
          reprise_effectuee_=1;
        }
      ////////////////////////////////////////////////
      // Lecture des options de sauvegarde d'un calcul
      ////////////////////////////////////////////////
      else if (motlu == "sauvegarde" || motlu == "sauvegarde_simple")
        {
          // restart_file=1: le fichier est ecrasee a chaque sauvegarde (et ne donc contient qu'un seul instant)
          if (motlu == "sauvegarde_simple") restart_file=1;
          is >> format_sauv;
          if( (Motcle(format_sauv) != "binaire")
              && (Motcle(format_sauv) != "formatte")
              && (Motcle(format_sauv) != "xyz")
              && (Motcle(format_sauv) != "single_hdf") )
            {
              nom_fich=format_sauv;
              format_sauv="binaire";
            }
          else
            is >> nom_fich;
        }
      else if (motlu == accolade_fermee)
        break;
      else
        {
          Cerr << "Error in Probleme_base::lire_donnees" << finl;
          Cerr << "We expected } instead of " << motlu << " to mark the end of the data set" << finl;
          exit();
        }
      is >> motlu;
    }
  ficsauv_.detach();
  // Force sauvegarde hdf au dela d'un certain nombre de rangs MPI:
  if (format_sauv!="xyz" && Process::force_single_file(Process::nproc(), nom_fich))
    format_sauv = "single_hdf";

  if ( (Motcle(format_sauv) != "binaire")
       && (Motcle(format_sauv) != "formatte")
       && (Motcle(format_sauv) != "xyz")
       && (Motcle(format_sauv) != "single_hdf") )
    {
      Cerr << "Error of backup format" << finl;
      Cerr << "We expected formatte, binaire, xyz, or single_hdf." << finl;
      exit();
    }
  if (schema_temps().temps_init()<=-DMAXFLOAT)
    {
      schema_temps().set_temps_init()=0;
      schema_temps().set_temps_courant()=0;
    }

  if (reprise_effectuee())
    {
      // on teste si dt_ev existe sinon on met reprise a 2
      // on recrera l'entete dans dt_ev sinon l'entete est fausse en reprise de pb_couple
      Nom fichier(nom_du_cas());
      fichier+=".dt_ev";
      struct stat f;
      if (stat(fichier,&f))
        reprise_effectuee()=2;
    }

  return is ;
}

// Description:
//    Lecture des equations du probleme.
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
// Postcondition: les equations ont ete lues
Entree& Probleme_base::lire_equations(Entree& is, Motcle& dernier_mot)
{
  Nom un_nom;
  int nb_eq= nombre_d_equations();
  Cerr << "Reading of the equations" << finl;

  for(int i=0; i<nb_eq; i++)
    {
      is >> un_nom ;
      is >> getset_equation_by_name(un_nom);
    }
  is >> dernier_mot;
  return is;
}

// Description:
//    Associe le probleme a toutes ses equations.
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
// Effets de bord: modification des objets equations associees, ces
//                 objets contiennent alors une reference sur le probleme.
// Postcondition: les equations ont ete associees au probleme
void Probleme_base::associer()
{
  int nb_eqn=nombre_d_equations();
  for(int i=0; i<nb_eqn; i++)
    equation(i).associer_pb_base(*this);
}

// Description:
//     surcharge Objet_U::associer_(Objet_U& ob)
//     Associe differents objets au probleme en controlant
//     le type de l'objet a associer a l'execution.
//     On peut ainsi associer: un schema en temps, un domaine de calcul ou
//     un milieu physique.
//     Utilise les routine de la classe Type_Info (Utilitaires)
// Precondition:
// Parametre: Objet_U& ob
//    Signification: l'objet a associer
//    Valeurs par defaut:
//    Contraintes: le type de l'objet peut etre Schema_Temps_base
//                                              Domaine
//                                              Milieu_base
//    Acces: entree
// Retour: int
//    Signification: 1 si association reussie 0 sinon
//                     2 si le milieu est deja associe a un autre probleme
//    Contraintes:
// Exception: Objet_U de type inconnu (non prevu)
// Effets de bord:
// Postcondition:
int Probleme_base::associer_(Objet_U& ob)
{
  // Schema_Temps_base Domaine Milieu_base
  if( sub_type(Schema_Temps_base, ob))
    {
      associer_sch_tps_base(ref_cast(Schema_Temps_base, ob));
      return 1;
    }
  if( sub_type(Domaine, ob))
    {
      associer_domaine(ref_cast(Domaine, ob));
      return 1;
    }
  if( sub_type(Milieu_base, ob))
    {
      if (!ref_cast(Milieu_base, ob).est_deja_associe())
        return 2;
      associer_milieu_base(ref_cast(Milieu_base, ob));
      return 1;
    }
  if (sub_type(Loi_Fermeture_base,ob))
    {
      Loi_Fermeture_base& loi=ref_cast(Loi_Fermeture_base,ob);
      liste_loi_fermeture_.add(loi);
      loi.associer_pb_base(*this);

      return 1;
    }
  return 0;
}

// Description:
//    Complete les equations associees au probleme.
//    Remplissage des references, deleguee aux equations.
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
// Effets de bord: modification des objets equations associees
// Postcondition:
void Probleme_base::completer()
{
  // Cerr << "Probleme_base::completer()" << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).completer();

  LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.completer();
      ++curseur;
    }

  les_postraitements.completer();
}

// Description:
//     Verifie que l'objet est complet, coherent, ...
//     NON DEVELOPPE RENVOIE TOUJOURS 1
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si l'objet est correct
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Probleme_base::verifier()
{
  return 1;
}

// Description:
//     Associe un domaine au probleme.
//     Prend un_domaine comme support.
//     apelle Domaine_dis::associer_dom(const Domaine& )
// Precondition:
// Parametre: Domaine& un_domaine
//    Signification: le domaine
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: les equations sont associees si elles ne l'etaient pas
void Probleme_base::associer_domaine(const Domaine& un_domaine)
{
  le_domaine_dis.associer_domaine(un_domaine);

}

// Description:
//     Affecte une discretisation au probleme
//     Discretise le Domaine associe au probleme avec la discretisation
//     Associe la premiere zone du Domaine aux equations du probleme
//     Discretise les equations associees au probleme
//     NOTE: TRUST V1 une seule Zone_dis pas Domaine_dis est traitee
// Precondition:
// Parametre: Discretisation_base& discretisation
//    Signification: une discretisation pour le probleme
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: les equations associees au probleme sont discretisees
void Probleme_base::discretiser(const Discretisation_base& une_discretisation)
{
  associer();
  la_discretisation=une_discretisation;
  Cerr << "Discretization of the domain associated with the problem " << le_nom() << finl;
  une_discretisation.discretiser(le_domaine_dis);
  Cerr << "Discretization of equations" << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      equation(i).associer_zone_dis(domaine_dis().zone_dis(0));
      equation(i).discretiser();
      //remontee de l'inconnue vers le milieu
      equation(i).associer_milieu_equation();
    }
  // Discretisation du milieu:
  //   ATTENTION (BM): il faudra faire quelque chose ici car si on associe deux
  //   milieux au probleme (fluide_incompressible + constituant), seul
  //   le premier est discretise.
  //   Cette facon de faire n'est pas propre !
  //   Solution probable: gros nettoyage et on met le milieu dans le probleme
  Noms milieux_deja_discretises;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      const Nom& le_milieu = equation(i).milieu().que_suis_je();
      if (!milieux_deja_discretises.contient_(le_milieu))
        {
          equation(i).milieu().discretiser((*this),une_discretisation);
          milieux_deja_discretises.add(le_milieu);
        }
    }

  LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.discretiser(une_discretisation);
      ++curseur;
    }

}

// Description :
// Flag le premier et le dernier postraitement pour chaque fichier
// Et initialise les postraitements
void Probleme_base::init_postraitements()
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = les_postraitements;
  while(curseur)   // Pour chaque postraitement
    {

      DERIV(Postraitement_base)& der_post = curseur.valeur();

      // S'il est de type Postraitement, initialiser premier/dernier _pour_nom_fich

      if (sub_type(Postraitement,der_post.valeur()))
        {

          Postraitement& post=ref_cast(Postraitement,der_post.valeur());

          Nom nom_fichier=post.nom_fich();
          int rg=glob_noms_fichiers.rang(nom_fichier);
          if (rg==-1)   // C'est la premiere fois qu'on rencontre ce nom
            {
              glob_noms_fichiers.add(nom_fichier);
              glob_derniers_posts.add(post);
              post.est_le_premier_postraitement_pour_nom_fich()=1;
            }
          else   // On a deja vu ce nom
            {
              post.est_le_premier_postraitement_pour_nom_fich()=0;
              Postraitement& autre_post=glob_derniers_posts[rg];
              autre_post.est_le_dernier_postraitement_pour_nom_fich()=0;
              glob_derniers_posts[rg]=post;

              // On verifie au passage que les intervalles de postraitements
              // sont bien les memes pour tout ce qui ecrit dans le meme
              // fichier  .
              if (post.champs_demande() && autre_post.dt_post_ch()!=post.dt_post_ch())
                {
                  Cerr << "Error, the values of dt_post (" << autre_post.dt_post_ch()
                       << " and " << post.dt_post_ch()
                       << ") of two postprocessing blocks writing in the same file"
                       << post.nom_fich() << " are different!" << finl;
                  Cerr << "Specify the same dt_post, or use two different files for postprocessing." << finl;
                  exit();
                }
            }
          post.est_le_dernier_postraitement_pour_nom_fich()=1;
        }
      ++curseur;
    }
  les_postraitements.init();
}

int Probleme_base::expression_predefini(const Motcle& motlu, Nom& expression)
{
  expression = "";
  return 0;
}

// Description:
//    Ecriture sur fichier en vue d'une reprise (sauvegarde)
// Precondition: le flot de sauvegarde est ouvert (membre ficsauv)
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: modifie le flot reference par Probleme_base::ficsauv
// Postcondition: la methode ne modifie pas l'objet
void Probleme_base::sauver() const
{
  statistiques().begin_count(sauvegarde_counter_);

  // Si le fichier de sauvegarde n'a pas ete ouvert alors on cree le fichier de sauvegarde:
  if ( !ficsauv_.non_nul() && !osauv_hdf_ )
    {
      if (Motcle(format_sauv) == "formatte")
        {
          ficsauv_.typer("EcrFicCollecte");
          ficsauv_->ouvrir(nom_fich);
          ficsauv_->setf(ios::scientific);
        }
      else if (Motcle(format_sauv) == "binaire")
        {
          ficsauv_.typer("EcrFicCollecteBin");
          ficsauv_->ouvrir(nom_fich);
        }
      else if (Motcle(format_sauv) == "xyz")
        {
          ficsauv_.typer(EcritureLectureSpecial::get_Output());
          ficsauv_->ouvrir(nom_fich);
        }
      else if (Motcle(format_sauv) == "single_hdf")
        osauv_hdf_ = new Sortie_Brute;
      else
        {
          Cerr << "Error in Probleme_base::sauver() " << finl;
          Cerr << "The format for the backup file must be either binary or formatted" << finl;
          Cerr << "But it is :" << format_sauv << finl;
          exit();
        }
      // Si c'est la premiere sauvegarde, on note en en-tete le format de sauvegarde
      if (Motcle(format_sauv) == "xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
        }
      else if((Motcle(format_sauv) == "single_hdf"))
        *osauv_hdf_ << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
      else
        ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;
    }

  // On realise l'ecriture de la sauvegarde
  int bytes;
  EcritureLectureSpecial::mode_ecr=(Motcle(format_sauv)=="xyz");
  if(Motcle(format_sauv) != "single_hdf")
    bytes = sauvegarder(ficsauv_.valeur());
  else
    bytes = sauvegarder(*osauv_hdf_);

  EcritureLectureSpecial::mode_ecr=-1;

  // Si c'est une sauvegarde simple, on referme immediatement et proprement le fichier
  if (restart_file==1)
    {
      if (Motcle(format_sauv)=="xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
          (ficsauv_.valeur()).syncfile();
        }
      else if(Motcle(format_sauv)=="single_hdf")
        {
          *osauv_hdf_ << Nom("fin");
          FichierHDFPar fic_hdf;
          fic_hdf.create(nom_fich);
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
  Debog::set_nom_pb_actuel(le_nom());
  statistiques().end_count(sauvegarde_counter_, bytes);
  Cout << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to write save file." << finl;
}

// Description:
//    Finit le postraitement et sauve le probleme dans un fichier.
//    Fermeture du fichier associe au postraitement.(Postraitement::finir())
// Precondition: le flot de sauvegarde est ouvert
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
// Postcondition: le postraitement est fini.
void Probleme_base::finir()
{
  Debog::set_nom_pb_actuel(le_nom());

  les_postraitements.finir();
  if (schema_temps().temps_sauv() > 0.0)
    sauver();

  // On ferme proprement le fichier de sauvegarde
  // Si c'est une sauvegarde_simple, le fin a ete mis a chaque appel a ::sauver()
  if (restart_file!=1 && (ficsauv_.non_nul() || osauv_hdf_) )
    {
      if (Motcle(format_sauv)=="xyz")
        {
          if (Process::je_suis_maitre())
            ficsauv_.valeur() << Nom("fin");
          (ficsauv_.valeur()).flush();
          (ficsauv_.valeur()).syncfile();
        }
      else if(Motcle(format_sauv) == "single_hdf")
        {
          *osauv_hdf_ << Nom("fin");
          FichierHDFPar fic_hdf;
          fic_hdf.create(nom_fich);
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
  if (Motcle(format_sauv)!="xyz" && (EcritureLectureSpecial::Active))
    sauver_xyz(1);
}

// Description:
//    Ecriture du probleme sur fichier en vue d'une reprise.
//    Ecrit le nom du probleme et sauvegarde les equations.
// Precondition:
// Parametre: Sortie& os
//    Signification: flot de sortie pour sauvegarde
//    Valeurs par defaut:
//    Contraintes: le flot doit etre ouvert
//    Acces: sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Probleme_base::sauvegarder(Sortie& os) const
{
  Debog::set_nom_pb_actuel(le_nom());
  schema_temps().sauvegarder(os);
  Cerr << "Backup of problem " << le_nom() << finl;
  int bytes=0;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      bytes += equation(i).sauvegarder(os);
      assert(bytes % 4 == 0);
    }
  bytes += les_postraitements.sauvegarder(os);
  assert(bytes % 4 == 0); // To detect a sauvegarder() method which returns 1 instead of the number of bytes saved.
  return bytes;
}

// Description:
//    Lecture d'un flot d'entree (fichier) pour reprise apres
//    une sauvegarde avec Probleme_base::sauvegarder(Sortie& os).
// Precondition: Le flot doit etre ouvert
// Parametre: Entree& is
//    Signification: le flot d'entree sur lequel on lit la reprise
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: les equations du problemes sont remises dans l'etat de la sauvegarde
int Probleme_base::reprendre(Entree& is)
{
  statistiques().begin_count(temporary_counter_);
  Debog::set_nom_pb_actuel(le_nom());
  schema_temps().reprendre(is);
  Cerr << "Resuming the problem " << le_nom() << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).reprendre(is);
  les_postraitements.reprendre(is);
  statistiques().end_count(temporary_counter_);
  Cerr << "End of resuming the problem " << le_nom() << " after " << statistiques().last_time(temporary_counter_) << " s" << finl;
  return 1;
}

// Description:
//    Demande au schema en temps s'il faut faire une impression
// Precondition: le schema en temps doit etre associe au probleme
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 il faut faire une impression, 0 il ne faut pas.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Probleme_base::limpr() const
{
  return schema_temps().limpr();
}

// Description:
//    Demande au schema en temps s'il faut faire une sauvegarde
// Precondition: le schema en temps doit etre associe au probleme
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 il faut faire une sauvegarde, 0 il ne faut pas.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Probleme_base::lsauv() const
{
  return schema_temps().lsauv();
}

// Description:
//     Imprime les equations associees au probleme si le schema en
//     temps associe indique que c'est necessaire.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Probleme_base::imprimer(Sortie& os) const
{
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).imprimer(os);
}

// Description:
//    Associe un schema en temps au probleme.
//    Associe ensuite le schema en temps a toutes
//    les equations du probleme.
// Precondition:
// Parametre: Schema_Temps_base& un_schema_en_temps
//    Signification: le schema en temps a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: les equations associees au probleme sont modifiees
// Postcondition:
void Probleme_base::associer_sch_tps_base(const Schema_Temps_base& un_schema_en_temps)
{
  if (le_schema_en_temps.non_nul())
    {
      Cerr << finl;
      Cerr<<"Error: Problem "<<le_nom()<<" was already associated with the scheme "<< le_schema_en_temps.valeur().le_nom()<<" and we try to associate it with "<<un_schema_en_temps.le_nom() << "." <<finl;
      exit();
    }
  le_schema_en_temps=un_schema_en_temps;
  le_schema_en_temps->associer_pb(*this);
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).associer_sch_tps_base(un_schema_en_temps);
}

// Description:
//    Renvoie le schema en temps associe au probleme. (si il est non nul)
//    (version const)
// Precondition: le schema en temps doit etre associe au probleme
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Schema_Temps_base&
//    Signification: le schema en temps associe au probleme
//    Contraintes: reference constante
// Exception: le schema en temps n'est pas associe au probleme, la reference est nulle
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Schema_Temps_base& Probleme_base::schema_temps() const
{
  if(!le_schema_en_temps.non_nul())
    {
      Cerr << le_nom() << " has not been associated to a time scheme !" << finl;
      exit();
    }
  return le_schema_en_temps.valeur();
}


// Description:
//    Renvoie le schema en temps associe au probleme. (si il est non nul)
// Precondition: le schema en temps doit etre associe au probleme
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Schema_Temps_base&
//    Signification: le schema en temps associe au probleme
//    Contraintes:
// Exception: le schema en temps n'est pas associe au probleme, la reference est nulle
// Effets de bord:
// Postcondition:
Schema_Temps_base& Probleme_base::schema_temps()
{
  if(!le_schema_en_temps.non_nul())
    {
      Cerr << le_nom() << " has not been associated to a time scheme !" << finl;
      exit();
    }
  return le_schema_en_temps.valeur();
}


// Description:
//    Renvoie le domaine associe au probleme.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Domaine&
//    Signification: un domaine
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Domaine& Probleme_base::domaine() const
{
  return le_domaine_dis.domaine();
}

// Description:
//    Renvoie le domaine associe au probleme.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Domaine&
//    Signification: un domaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Domaine& Probleme_base::domaine()
{
  return le_domaine_dis.domaine();
}

// Description:
//    Renvoie le domaine discretise associe au probleme.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Domaine_dis&
//    Signification: un domaine discretise
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Domaine_dis& Probleme_base::domaine_dis() const
{
  return le_domaine_dis;
}

// Description:
//    Renvoie le domaine discretise associe au probleme.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Domaine_dis&
//    Signification: un domaine discretise
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Domaine_dis& Probleme_base::domaine_dis()
{
  return le_domaine_dis;
}

// Description:
//    Associe un milieu physique aux equations du probleme.
//    Choix du milieu physique.
// Precondition:
// Parametre:  Milieu_base& mil
//    Signification: le milieu a associer (Solide, Fluide Incompressible ...)
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: les equations associees au probleme ont un milieu associe
// Postcondition:
void Probleme_base::associer_milieu_base(const Milieu_base& mil)
{
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).associer_milieu_base(mil);
}

// Description:
//    Renvoie le milieu physique associe au probleme.
//    (version const)
//    On renvoie le milieu associe a la premiere equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: un milieu physique
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
const Milieu_base& Probleme_base::milieu() const
{
  return equation(0).milieu();
}

// Description:
//    Renvoie le milieu physique associe au probleme.
//    On renvoie le milieu associe a la premiere equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: un milieu physique
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Probleme_base::milieu()
{
  return equation(0).milieu();
}

// Description:
//    Renvoie l'equation dont le nom est specifie.
//    On indexe les equations avec leur nom associe.
//    On cherche dans toutes les equations du probleme celle
//    qui porte le nom specifie.
//    (version const)
// Precondition: une equation du probleme doit porter le nom specifie
// Parametre: Nom& type
//    Signification: le nom de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour: Equation_base&
//    Signification: une equation
//    Contraintes: reference constante
// Exception: pas d'equation du nom specifie
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Equation_base& Probleme_base::equation(const Nom& type) const
{
  Motcle Type(type);
  Motcle Type_eqn;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Type_eqn = equation(i).que_suis_je();
      if(Type_eqn==Type)
        return equation(i);
    }
  Cerr << que_suis_je() << " does not contain any equation of type: " << type << finl;
  Cerr << "Here is the list of possible equations for a " << que_suis_je() << " problem: " << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Cerr << "\t- " << equation(i).que_suis_je() << finl;
    }
  exit();
  // Pour les compilos;
  return equation(0);
}

// Description (B.Math):
//  Methode virtuelle ajoutee pour les problemes ayant plusieurs equations
//  de meme type (Probleme_FT_Disc_gen). Dans ce cas, le nom de l'equation
//  n'est pas son type...
const Equation_base& Probleme_base::get_equation_by_name(const Nom& un_nom) const
{
  return equation(un_nom);
}

// Description (B.Math):
//  Methode virtuelle ajoutee pour les problemes ayant plusieurs equations
//  de meme type (Probleme_FT_Disc_gen). Dans ce cas, le nom de l'equation
//  n'est pas son type...
//  Version non const. Cette methode est notamment appelee a la lecture du probleme.
Equation_base& Probleme_base::getset_equation_by_name(const Nom& un_nom)
{
  return equation(un_nom);
}

// Description:
//    Renvoie l'equation dont le nom est specifie.
//    On indexe les equations avec leur nom associe.
//    On cherche dans toutes les equations du probleme celle
//    qui porte le nom specifie.
// Precondition: une equation du probleme doit porter le nom specifie
// Parametre: Nom& type
//    Signification: le nom de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour: Equation_base&
//    Signification: une equation
//    Contraintes: reference constante
// Exception: pas d'equation du nom specifie
// Effets de bord:
// Postcondition:
Equation_base& Probleme_base::equation(const Nom& type)
{
  Motcle Type(type);
  Motcle Type_eqn;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Type_eqn = equation(i).que_suis_je();
      if(Type_eqn==Type)
        return equation(i);
    }
  Cerr << que_suis_je() << " does not contain any equation of type: " << type << finl;
  Cerr << "Here is the list of possible equations for a " << que_suis_je() << " problem: " << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Cerr << "\t- " << equation(i).que_suis_je() << finl;
    }
  exit();
  // Pour les compilos;
  return equation(0);
}

void Probleme_base::creer_champ(const Motcle& motlu)
{
  milieu().creer_champ(motlu);
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    equation(i).creer_champ(motlu);

  LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.creer_champ(motlu);
      ++curseur;
    }
}

const Champ_base& Probleme_base::get_champ(const Motcle& un_nom) const
{
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    {
      try
        {
          return equation(i).get_champ(un_nom);
        }
      catch (Champs_compris_erreur)
        {
        }
      try
        {
          return equation(i).milieu().get_champ(un_nom);
        }
      catch (Champs_compris_erreur)
        {
        }
    }

  CONST_LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      const Loi_Fermeture_base& loi=curseur.valeur().valeur();
      try
        {
          return loi.get_champ(un_nom);
        }
      catch(Champs_compris_erreur)
        {
        }
      ++curseur;
    }

  if (discretisation().que_suis_je()=="VDF")
    {
      Cerr << "\n" << un_nom << " is not available for VDF discretization" << finl;
      exit();
    }

  Cerr<<"The field of name "<<un_nom<<" do not correspond to a field understood by the problem."<<finl;
  Cerr<<"It may be a field dedicated only to post-process and defined in the Definition_champs set."<<finl;
  Cerr<<"1) If you have request the post-processing of "<<un_nom<<" in the Champs set"<<finl;
  Cerr<<"please remove the localisation elem or som that you may have specified."<<finl;
  Cerr<<"2) If you have used "<<un_nom<<" in Definition_champs, please use 'sources_reference { "<<un_nom<<" }'"<<finl;
  Cerr<<"instead of 'source refchamp { pb_champ "<<le_nom()<<" "<<un_nom<<" }'"<<finl;
  Cerr<<"3) Check reference manual." << finl;
  Cerr<<"4) Contact TRUST support." << finl;
  exit();

  //Pour compilation
  REF(Champ_base) ref_champ;
  return ref_champ;
}

void Probleme_base::get_noms_champs_postraitables(Noms& noms,Option opt) const
{
  milieu().get_noms_champs_postraitables(noms,opt);
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    equation(i).get_noms_champs_postraitables(noms,opt);

  CONST_LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      const Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.get_noms_champs_postraitables(noms,opt);
      ++curseur;
    }

}

int Probleme_base::comprend_champ_post(const Motcle& un_nom) const
{
  if (un_nom=="TEMPERATURE_PHYSIQUE")
    return 0;
  CONST_LIST_CURSEUR(DERIV(Postraitement_base)) curseur_post = postraitements();
  while (curseur_post)
    {
      const Postraitement& post = ref_cast(Postraitement,curseur_post.valeur().valeur());
      if (post.comprend_champ_post(un_nom))
        return 1;
      ++curseur_post;
    }
  return 0;
}

// Description:
// On verifie que le temps de debut et de fin des statistiques est identique sur tous les champsde tous les postraitements
int Probleme_base::verifie_tdeb_tfin(const Motcle& un_nom) const
{
  CONST_LIST_CURSEUR(DERIV(Postraitement_base)) curseur_post = postraitements();
  while (curseur_post)
    {
      const Postraitement& post = ref_cast(Postraitement,curseur_post.valeur().valeur());
      if (tstat_deb_==-1)
        tstat_deb_ = post.tstat_deb();
      else if (!est_egal(tstat_deb_,post.tstat_deb()) && post.tstat_deb()!=-1)
        {
          //Cerr << finl;
          //Cerr << "Error in " << un_nom << finl;
          //Cerr << "Your beginning time of statistics t_deb=" << post.tstat_deb() << " must be equal" << finl;
          //Cerr << "to the previous beginning time already read t_deb=" << tstat_deb_ << finl;
          Cerr << "Beginning times of statistics t_deb are differents but the calculation continues" << finl;
          //exit();
        }
      if (tstat_fin_==-1)
        tstat_fin_ = post.tstat_fin();
      else if (!est_egal(tstat_fin_,post.tstat_fin()) && post.tstat_fin()!=-1)
        {
          //Cerr << finl;
          //Cerr << "Error in " << un_nom << finl;
          //Cerr << "Your ending time of statistics t_fin=" << post.tstat_fin() << " must be equal" << finl;
          //Cerr << "to the previous ending time already read t_fin=" << tstat_fin_ << finl;
          Cerr << "Ending times of statistics t_fin are differents but the calculation continues" << finl;
          //exit();
        }
      ++curseur_post;
    }
  return 1;
}

const Champ_Generique_base& Probleme_base::get_champ_post(const Motcle& un_nom) const
{

  REF(Champ_Generique_base) ref_champ;

  CONST_LIST_CURSEUR(DERIV(Postraitement_base)) curseur_post = postraitements();
  while (curseur_post)
    {
      if (sub_type(Postraitement,curseur_post.valeur().valeur()))
        {
          const Postraitement& post = ref_cast(Postraitement,curseur_post.valeur().valeur());
          try
            {
              return post.get_champ_post(un_nom);
            }
          catch (Champs_compris_erreur) { }
        }
      ++curseur_post;
    }
  Cerr<<" "<<finl;
  Cerr<<"The field named "<<un_nom<<" do not correspond to a field understood by the problem."<<finl;
  Cerr<<"Check the name of the field indicated into the postprocessing block of the data file " << finl;
  Cerr<<"or in the list of post-processed fields above (in the block 'Reading of fields to be postprocessed')."<<finl;
  exit();

  //Pour compilation
  return ref_champ;
}

int Probleme_base::a_pour_IntVect(const Motcle&, REF(IntVect)& ) const
{
  return 0;
}

// Description:
//    Effectue une mise a jour en temps du probleme.
//    Effectue la mise a jour sur toutes les equations du probleme.
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Probleme_base::mettre_a_jour(double temps)
{
  // Update the name of the problem being debugged
  Debog::set_nom_pb_actuel(le_nom());

  // Update the equations:
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).mettre_a_jour(temps);

  // Update the media:
  milieu().mettre_a_jour(temps);

  // Update the conserved fields in the equations (must be done after the media):
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).mettre_a_jour_champs_conserves(temps);

  // Update the post-processing:
  les_postraitements.mettre_a_jour(temps);

  // Update the domain:
  domaine().mettre_a_jour(temps,domaine_dis(),*this);

  LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.mettre_a_jour(temps);
      ++curseur;
    }

}

// Description:
//    Prepare le calcul: initialise les parametres du milieu et
//    prepare le calcul de chacune des equations.
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
// Effets de bord: Le milieu associe est initilialise, les equations sont
//                 preparees au calcul.
// Postcondition:
void Probleme_base::preparer_calcul()
{
  const double temps = schema_temps().temps_courant();
  // Modification du tableau Qdm porte par la zone_dis() dans le cas
  // ou il y a des conditions aux limites periodiques.
  // Rq : Si l'une des equations porte la condition a la limite periodique
  //      alors les autres doivent forcement la porter.
  equation(0).zone_dis()->modifier_pour_Cl(equation(0).zone_Cl_dis().les_conditions_limites());
  milieu().initialiser(temps);
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).preparer_calcul();
  milieu().preparer_calcul();

  if(schema_temps().file_allocation() && EcritureLectureSpecial::Active)
    file_size_xyz();

  LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.preparer_calcul();
      ++curseur;
    }
}


// Description:
//    Calcul la valeur du prochain pas de temps du probleme.
//    On calcule le minimum des pas de temps des equations associees
//    au probleme.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le pas de temps maximum autorise pour ce probleme
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Probleme_base::calculer_pas_de_temps() const
{
  Debog::set_nom_pb_actuel(le_nom());
  double dt=schema_temps().pas_temps_max();
  for(int i=0; i<nombre_d_equations(); i++)
    dt=std::min(dt,equation(i).calculer_pas_de_temps());
  return dt;
}

void Probleme_base::lire_postraitement_interfaces(Entree& is)
{
  Cerr<<"The postprocessing of interfaces is only possible for"<<finl;
  Cerr<<"a problem type Pb_Front_Tracking, not a "<<que_suis_je()<<finl;
  exit();

}
void Probleme_base::postraiter_interfaces(const Nom& nomfich, Sortie& s, const Nom& format, double temps)
{
  Cerr<<que_suis_je()<<" must overloaded :postraiter_interfaces"<<finl;
  // exit();
}

// En attendant de mieux poser les equations
int Probleme_base::is_dilatable() const
{
  return (milieu().que_suis_je()=="Fluide_Quasi_Compressible" || milieu().que_suis_je()=="Fluide_Weakly_Compressible");
}

// Description:
//    Initialisation de file_size, bad_allocate, nb_pb_total, num_pb
long int Probleme_base::file_size=0;        // file_size est l'espace disque en octet necessaire pour ecrire les fichiers XYZ
int Probleme_base::bad_allocate=1;        // bad_allocate est un int qui permet de savoir si l'allocation a deja eut lieu
int Probleme_base::nb_pb_total=0;        // nb_pb_total est le nombre total de probleme
int Probleme_base::num_pb=1;                // num_pb est le numero du probleme courant

// Description:
//    Verifie que la place necessaire existe sur le disque dur.
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
// Postcondition: la methode ne modifie pas l'objet
void Probleme_base::allocation() const
{
  if(schema_temps().file_allocation() && EcritureLectureSpecial::Active)        // Permet de tester l'allocation d'espace disque
    {
      if (bad_allocate==1)                                        // Si l'allocation n'a pas eut lieu
        if (Process::je_suis_maitre())                                // Qu'avec le proc maitre
          {
            if (num_pb==1)                                                // Si le probleme est le premier
              if (!allocate_file_size(file_size))                        // je tente une allocation d'espace disque de taille 2*file_size
                bad_allocate=0;                                        // Si cela echoue, j'indique au code que l'allocation a deja eut lieu et n'a pas fonctionner
              else
                num_pb=nb_pb_total;                                        // Si OK, je modifie num_pb pour que les autres pb ne tentent pas d'allocation
            else
              num_pb-=1;                                                // Si le probleme n'est pas le premier, je decremente le numero de probleme
          }
      const int canal = 2007;
      if (Process::je_suis_maitre())                                // le processeur maitre envoi bad_allocate a tout le monde
        for (int p=1; p<Process::nproc(); p++)
          envoyer(bad_allocate,p,canal);
      else
        recevoir(bad_allocate,0,canal);

      if (bad_allocate==0)                                        // Si l'allocation a echoue
        {
          sauver_xyz(1);
          if (num_pb==nb_pb_total)                                        // Si le numero de probleme correspond au nombre total de probleme
            {
              if (Process::je_suis_maitre())
                {
                  Cerr << finl;                                                // j'arrete le code de facon claire
                  Cerr << "***Error*** " << error << finl;                // et je sort l'erreur du code
                  Cerr << "A xyz backup was made because you do not have enough disk space" << finl;
                  Cerr << "to continue the current calculation. Free up disk space and" << finl;
                  Cerr << "restart the calculation thanks to the backup just made." << finl;
                  Cerr << finl;
                }
              barrier();
              exit();
            }
          num_pb+=1;                                                // j'incremente le numero de probleme
        }
    }
}

// Description:
//    Verifie que la place necessaire existe sur le disque dur.
// Precondition:
// Parametre:
//    Signification: l'espace disque requis
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: retourne 1 si l'espace disque est suffisant, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Probleme_base::allocate_file_size(long int& size) const
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
      error="Open of ";
      error+=file;
      error+=" : ";
      error+=strerror(errno);                                // Erreur sur l'ouverture
      close(fichier);                                        // fermeture du fichier
      remove(file);                                        // Destruction du fichier File_size
      return 0;                                                // Echec d'allocation car fichier pas ouvert
    }

  if (posix_fallocate(fichier, 0, taille) != 0)                // Erreur d'allocation de l'espace disque
    {
      error="Allocation of ";
      error+=file;
      error+=" : ";
      error+=strerror(errno);                                // Erreur sur l'allocation
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

// Description:
//    Ecrit le probleme dans un fichier *.calcul_xyz
//    et calcul la place disque prise par ce fichier
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: retourne toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Probleme_base::file_size_xyz() const
{
#ifndef RS6000
  Nom nom_fich_xyz(".xyz");
  sauver_xyz(0);
  if (Process::je_suis_maitre())
    {
      ifstream fichier(nom_fich_xyz);                        // Calcul de l'espace disque pris par le fichier XYZ du probleme courant
      fichier.seekg(0,std::ios_base::end);
      file_size+=fichier.tellg();                                // Incremente l'espace disque deja necessaire
      fichier.close();
      remove(nom_fich_xyz);
    }
  nb_pb_total+=1;                                        // Permet de connaitre le nombre de probleme total a la fin du preparer_calcul
#endif
  return 1;
}

void Probleme_base::addInputField(Field_base& f)
{
  const Nom& name= f.le_nom();
  for (int i=0; i<input_fields.size(); i++)
    {
      if (input_fields[i]->le_nom()==name)
        throw WrongContext(name.getChar(),"addInputField", "Field with this name is already in list");
    }
  input_fields.add(f);
}

double Probleme_base::futureTime() const
{
  if (!dt_defined)
    throw WrongContext(le_nom().getChar(),"futureTime","initTimeStep should have been called");
  int i=schema_temps().nb_valeurs_futures();
  return schema_temps().temps_futur(i);
}

void Probleme_base::sauver_xyz(int verbose) const
{
  statistiques().begin_count(sauvegarde_counter_);
  Nom nom_fich_xyz("");
  if (verbose)
    {
      nom_fich_xyz+=Objet_U::nom_du_cas();
      nom_fich_xyz+="_";
      nom_fich_xyz+=le_nom();
      nom_fich_xyz+=".xyz";
      if (Process::je_suis_maitre())
        Cerr <<"Creation of "<<nom_fich_xyz<< " (" << EcritureLectureSpecial::get_Output() << ") for resumption of a calculation with a different number of processors."<<finl;
    }
  else
    {
      nom_fich_xyz=".xyz";
    }
  // Creation du fichier XYZ du probleme courant
  ficsauv_.typer(EcritureLectureSpecial::get_Output());
  ficsauv_->ouvrir(nom_fich_xyz);
  // Nouveau pour le xyz depuis la 155: on note en en-tete le format de sauvegarde
  if (Process::je_suis_maitre())
    ficsauv_.valeur() << "format_sauvegarde:" << finl << version_format_sauvegarde() << finl;

  EcritureLectureSpecial::mode_ecr=1;
  int bytes = sauvegarder(ficsauv_.valeur());
  EcritureLectureSpecial::mode_ecr=-1;

  if (Process::je_suis_maitre())
    ficsauv_.valeur() << Nom("fin");
  (ficsauv_.valeur()).flush();
  (ficsauv_.valeur()).syncfile();
  ficsauv_.detach();
  Cout << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to write xyz file." << finl;
  statistiques().end_count(sauvegarde_counter_, bytes);
}
