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
#include <Discretisation_base.h>
#include <Domaine_Cl_dis_base.h>
#include <Loi_Fermeture_base.h>
#include <Domaine_dis_base.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Postraitement.h>
#include <stat_counters.h>
#include <Debog.h>

Implemente_base_sans_destructeur(Probleme_base,"Probleme_base",Probleme_U);

// XD pb_gen_base objet_u pb_gen_base -1 Basic class for problems.

// XD Pb_base pb_gen_base Pb_base -3 Resolution of equations on a domain. A problem is defined by creating an object and assigning the problem type that the user wishes to resolve. To enter values for the problem objects created, the Lire (Read) interpretor is used with a data block.
// XD  attr milieu milieu_base milieu 1 The medium associated with the problem.
// XD  attr constituant constituant constituant 1 Constituent.
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
//  XD ref loi1 loi_fermeture_base
//  XD ref loi2 loi_fermeture_base
//  XD ref loi3 loi_fermeture_base
//  XD ref loi4 loi_fermeture_base
//  XD ref loi5 loi_fermeture_base

// XD problem_read_generic Pb_base problem_read_generic -1 The probleme_read_generic differs rom the rest of the TRUST code : The problem does not state the number of equations that are enclosed in the problem. As the list of equations to be solved in the generic read problem is declared in the data file and not pre-defined in the structure of the problem, each equation has to be distinctively associated with the problem with the Associate keyword.
// XD   ref eqn1 eqn_base
// XD   ref eqn2 eqn_base
// XD   ref eqn3 eqn_base
// XD   ref eqn4 eqn_base
// XD   ref eqn5 eqn_base
// XD   ref eqn6 eqn_base
// XD   ref eqn7 eqn_base
// XD   ref eqn8 eqn_base
// XD   ref eqn9 eqn_base
// XD   ref eqn10 eqn_base

// XD constituant milieu_base constituant -1 Constituent.
// XD attr coefficient_diffusion field_base coefficient_diffusion 1 Constituent diffusion coefficient value (m2.s-1). If a multi-constituent problem is being processed, the diffusivite will be a vectorial and each components will be the diffusion of the constituent.

// Variables globales pour initialiser est_le_premier_postraitement_pour_nom_fic
// et est_le_dernier_postraitement_pour_nom_fic en une seule passe.
LIST(Nom) glob_noms_fichiers;
LIST(OBS_PTR(Postraitement)) glob_derniers_posts;

Sortie& Probleme_base::printOn(Sortie& os) const
{
  for (int i = 0; i < nombre_d_equations(); i++)
    os << equation(i).que_suis_je() << " " << equation(i) << finl;
  os << les_postraitements_;
  os << le_domaine_dis_.valeur();
  return os;
}

/*! @brief Lecture d'un probleme dans un flot d'entree, et ouverture du flot de sauvegarde.
 *
 *     Format:
 *      {
 *      nom_milieu bloc de lecture d'un milieu
 *      nom_equation bloc de lecture d'une equation
 *      Postraitement bloc de lecture postraitement
 *      reprise | sauvegarde | sauvegarde_simple
 *      formatte | binaire
 *      nom_de_fichier
 *      }
 *  L'option sauvegarde_simple permet de sauver le probleme dans le fichier choisi
 *  en ecrasant a chaque fois les sauvegardes precedentes : cela permet d'economiser de la place disque.
 *
 * @param (Entree& is) flot d'entree
 * @return (Entree&) le flot d'entre modifie
 * @throws pas d'accolade ouvrante en debut de format
 * @throws mot clef "Postraitement" n'est pas la
 * @throws format de sauvegarde doit etre "binaire" ou "formatte"
 * @throws pas d'accolade fermante en fin de jeu de donnee
 */
Entree& Probleme_base::readOn(Entree& is)
{
  Cerr << "Reading of the problem " << le_nom() << finl;
  Motcle accolade_ouverte("{"), motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected { to start to read the problem" << finl;
      Process::exit();
    }

  /* 1 : solved_equations + milieu : NEW SYNTAX */
  lire_solved_equations(is);

  if (!milieu_via_associer_ && !is_pb_med() && !is_pb_rayo())
    typer_lire_milieu(is);
  else assert((int)le_milieu_.size() == 0);

  /* 2 : On lit les equations */
  lire_equations(is, motlu); //"motlu" contient le premier mot apres la lecture des equations

  /* 3 : Les postraitements */
  // Si le postraitement comprend le mot, on en lit un autre...
  while (les_postraitements_.lire_postraitements(is, motlu, *this))
    {
      is >> motlu;
    }

  /* 4 : On complete ... */
  completer();
  Cerr << "Step verification of data being read in progress..." << finl;

  /* 5 : On verifie ... */
  verifier();
  Cerr << "The read data are coherent" << finl;

  /* 6 : gestion sauvegarde/reprise ... */
  save_restart_.lire_sauvegarde_reprise(is,motlu);

  return is ;
}

void Probleme_base::typer_lire_milieu(Entree& is)
{
  // NOTA BENE :
  // Normalement on a un milieu par probleme, sauf si le problem contient une equation de concentration
  // Dans ce cas, on a un milieu supplementaire : constituant (faut pas demander pourquoi 2 milieu ... car je sais pas !). a voir si on peut faire mieux ...
  int nb_milieu = 1;

  // On cherche si c'est un pb avec concentration => avec constituant
  const std::string conc = "Concentration", scal_pass = "Scalaires_Passifs", nom_pb = que_suis_je().getString();
  if (nom_pb.find(conc) != std::string::npos) nb_milieu = 2; // pb contient concentration !

  le_milieu_.resize(nb_milieu);

  for (int i = 0; i < nb_milieu; i++)
    {
      le_milieu_[i].typer_lire_simple(is, "Typing the medium ..."); // On commence par la lecture du milieu
      associer_milieu_base(le_milieu_[i].valeur()); // On l'associe a chaque equations (methode virtuelle pour chaque pb ...)
    }

  // Milieu(x) lu(s) ... Lets go ! On discretise les equations
  discretiser_equations();

  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < nombre_d_equations(); i++) equation(i).associer_milieu_equation();

  const bool is_constituant = nb_milieu == 1 ? false : true, is_scal_pass = (nom_pb.find(scal_pass) != std::string::npos) ? true : false;
  const int ns_ou_cond_eq = 0;
  int conc_eq = 1; // pas const !

  // On discretise le/les milieu/x ... Et l'eq de concentration !
  if (is_constituant)
    {
      assert (nombre_d_equations() > 1);
      if (nombre_d_equations() == 2)
        for (int i = 0; i < nombre_d_equations(); i++) equation(i).milieu().discretiser((*this), la_discretisation_.valeur());
      else if (nombre_d_equations() == 3)
        {
          // On a 2 Cas :
          // - Pb_Thermohydraulique_Concentration (NS, Thermique, Conc)
          // - Pb_Hydraulique_Concentration_Scalaires_Passifs (NS, Conc + Equations_Scalaires_Passifs (la lise) !)
          conc_eq = is_scal_pass ? 1 /* conc_eq */ : 2;
          equation(ns_ou_cond_eq).milieu().discretiser((*this), la_discretisation_.valeur()); // NS
          equation(conc_eq).milieu().discretiser((*this), la_discretisation_.valeur()); // Conc
        }
      else
        {
          // Cas rare : Pb_Thermohydraulique_Concentration_Scalaires_Passifs
          // Ici on a NS, Thermique, Conc + Equations_Scalaires_Passifs (la lise) !
          assert (nombre_d_equations() == 4);
          conc_eq = 2;
          equation(ns_ou_cond_eq).milieu().discretiser((*this), la_discretisation_.valeur()); // NS
          equation(conc_eq).milieu().discretiser((*this), la_discretisation_.valeur()); // Conc
        }
    }
  else /* On discretise le milieu de l'eq 1 et c'est tout :-) :-) */
    equation(ns_ou_cond_eq).milieu().discretiser((*this), la_discretisation_.valeur());
}

/*! @brief Lecture des equations du probleme.
 *
 */
Entree& Probleme_base::lire_equations(Entree& is, Motcle& mot)
{
  const int nb_eq = nombre_d_equations();
  is >> mot;
  if (nb_eq == 0) return is;
  Cerr << "Reading of the equations" << finl;
  bool already_read;
  if (mot == "correlations") lire_correlations(is), already_read = false;
  else already_read = true;

  for(int i = 0; i < nb_eq; i++, already_read = false)
    {
      if (!already_read) is >> mot;
      is >> getset_equation_by_name(mot);
    }
  read_optional_equations(is, mot);
  return is;
}

/*! @brief Associe le probleme a toutes ses equations.
 *
 */
void Probleme_base::associer()
{
  save_restart_.assoscier_pb_base(*this);

  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).associer_pb_base(*this);
}

void Probleme_base::warn_old_syntax()
{
  if (!is_pb_rayo())
    {
      Cerr << "YOU ARE USING AN OLD SYNTAX IN YOUR DATA FILE AND THIS IS NO MORE SUPPORTED !" << finl;
      Cerr << "STARTING FROM TRUST-v1.9.3 : THE MEDIUM SHOULD BE READ INSIDE THE PROBLEM AND NOT VIA ASSOSCIATION ... " << finl;
      Cerr << "HAVE A LOOK TO ANY TRUST TEST CASE TO SEE HOW IT SHOULD BE DONE ($TRUST_ROOT/tests/) ... " << finl;
      Cerr << "OR RUN -convert_data OPTION OF YOUR APPLICATION SCRIPT, FOR TRUST FOR EXAMPLE:" << finl;
      Cerr << "   trust -convert_data " << Objet_U::nom_du_cas() << ".data" << finl;
      Process::exit();
    }
}

/*! @brief surcharge Objet_U::associer_(Objet_U& ob) Associe differents objets au probleme en controlant
 *
 *      le type de l'objet a associer a l'execution.
 *      On peut ainsi associer: un schema en temps, un domaine de calcul ou
 *      un milieu physique.
 *      Utilise les routine de la classe Type_Info (Utilitaires)
 *
 * @param (Objet_U& ob) l'objet a associer
 * @return (int) 1 si association reussie 0 sinon 2 si le milieu est deja associe a un autre probleme
 * @throws Objet_U de type inconnu (non prevu)
 */
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
      warn_old_syntax();
      milieu_via_associer_ = true;
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

/*! @brief Complete les equations associees au probleme.
 *
 * Remplissage des references, deleguee aux equations.
 *
 */
void Probleme_base::completer()
{
  // Cerr << "Probleme_base::completer()" << finl;
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).completer();

  for (auto& itr : liste_loi_fermeture_)
    {
      Loi_Fermeture_base& loi = itr.valeur();
      loi.completer();
    }

  les_postraitements_.completer();

  for (auto &corr : correlations_)
    corr.second->completer();
}

/*! @brief Verifie que l'objet est complet, coherent, .
 *
 * .. NON DEVELOPPE RENVOIE TOUJOURS 1
 *
 * @return (int) 1 si l'objet est correct
 */
int Probleme_base::verifier()
{
  return 1;
}

/*! @brief Associe un domaine au probleme.
 *
 * Prend un_domaine comme support.
 *      apelle Domaine_dis::associer_dom(const Domaine& )
 *
 * @param (Domaine& un_domaine) le domaine
 */
void Probleme_base::associer_domaine(const Domaine& un_domaine)
{
  le_domaine_ = un_domaine;
}

void Probleme_base::discretiser_equations()
{
  Cerr << "Discretization of the equations of problem " << que_suis_je() << " ..." <<  finl;
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      equation(i).associer_domaine_dis(domaine_dis());
      equation(i).discretiser();
    }
}

/*! @brief Affecte une discretisation au probleme Discretise le Domaine associe au probleme avec la discretisation
 *
 *      Associe le premier Domaine aux equations du probleme
 *      Discretise les equations associees au probleme
 *
 * @param (Discretisation_base& discretisation) une discretisation pour le probleme
 */
void Probleme_base::discretiser(Discretisation_base& une_discretisation)
{
  associer();
  la_discretisation_ = une_discretisation;
  Cerr << "Discretization of the domain associated with the problem " << le_nom() << finl;

  if (!le_domaine_.non_nul())
    Process::exit("ERROR: Discretize - You're trying to discretize a problem without having associated a Domain to it!!! Fix your dataset.");

  // Initialisation du tableau renum_som_perio
  le_domaine_->init_renum_perio();

  une_discretisation.associer_domaine(le_domaine_.valeur());
  le_domaine_dis_ = une_discretisation.discretiser();
  // Can not do this before, since the Domaine_dis is not typed yet:
  le_domaine_dis_->associer_domaine(le_domaine_);

  if (milieu_via_associer_)
    {
      discretiser_equations();
      Noms milieux_deja_discretises;
      for (int i = 0; i < nombre_d_equations(); i++)
        {
          equation(i).associer_milieu_equation(); // remontee de l'inconnue vers le milieu
          const Nom& le_milieu = equation(i).milieu().que_suis_je();
          if (!milieux_deja_discretises.contient_(le_milieu))
            {
              equation(i).milieu().discretiser((*this), une_discretisation);
              milieux_deja_discretises.add(le_milieu);
            }
        }
    }

  for (auto& itr : liste_loi_fermeture_)
    {
      Loi_Fermeture_base& loi = itr.valeur();
      loi.discretiser(une_discretisation);
    }
}

/*! @brief Flag le premier et le dernier postraitement pour chaque fichier Et initialise les postraitements
 *
 */
void Probleme_base::init_postraitements()
{
  for (auto& itr : les_postraitements_) // Pour chaque postraitement
    {
      OWN_PTR(Postraitement_base) &der_post = itr;

      // S'il est de type Postraitement, initialiser premier/dernier _pour_nom_fich
      if (sub_type(Postraitement, der_post.valeur()))
        {

          Postraitement& post = ref_cast(Postraitement, der_post.valeur());

          Nom nom_fichier = Sortie_Fichier_base::root;
          nom_fichier+=post.nom_fich();
          int rg = glob_noms_fichiers.rang(nom_fichier);
          if (rg == -1)   // C'est la premiere fois qu'on rencontre ce nom
            {
              glob_noms_fichiers.add(nom_fichier);
              glob_derniers_posts.add(post);
              post.est_le_premier_postraitement_pour_nom_fich() = 1;
            }
          else   // On a deja vu ce nom
            {
              post.est_le_premier_postraitement_pour_nom_fich() = 0;
              Postraitement& autre_post = glob_derniers_posts[rg];
              autre_post.est_le_dernier_postraitement_pour_nom_fich() = 0;
              glob_derniers_posts[rg] = post;

              // On verifie au passage que les intervalles de postraitements
              // sont bien les memes pour tout ce qui ecrit dans le meme
              // fichier  .
              if (post.champs_demande() && autre_post.dt_post_ch() != post.dt_post_ch())
                {
                  Cerr << "Error, the values of dt_post (" << autre_post.dt_post_ch() << " and " << post.dt_post_ch() << ") of two postprocessing blocks writing in the same file" << nom_fichier
                       << " are different!" << finl;
                  Cerr << "Specify the same dt_post, or use two different files for postprocessing." << finl;
                  exit();
                }
            }
          post.est_le_dernier_postraitement_pour_nom_fich() = 1;
        }
    }
  les_postraitements_.init();
}

int Probleme_base::expression_predefini(const Motcle& motlu, Nom& expression)
{
  expression = "";
  return 0;
}

/*! @brief Ecriture du probleme sur fichier en vue d'une reprise.
 *
 * Ecrit le nom du probleme et sauvegarde les equations.
 *
 * @param (Sortie& os) flot de sortie pour sauvegarde
 * @return (int) renvoie toujours 1
 */
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
  bytes += les_postraitements_.sauvegarder(os);
  assert(bytes % 4 == 0); // To detect a sauvegarder() method which returns 1 instead of the number of bytes saved.
  return bytes;
}

/*! @brief Lecture d'un flot d'entree (fichier) pour reprise apres une sauvegarde avec Probleme_base::sauvegarder(Sortie& os).
 *
 * @param (Entree& is) le flot d'entree sur lequel on lit la reprise
 * @return (int) renvoie toujours 1
 */
int Probleme_base::reprendre(Entree& is)
{
  statistiques().begin_count(temporary_counter_);
  Debog::set_nom_pb_actuel(le_nom());
  schema_temps().reprendre(is);
  Cerr << "Resuming the problem " << le_nom() << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).reprendre(is);
  les_postraitements_.reprendre(is);
  statistiques().end_count(temporary_counter_);
  Cerr << "End of resuming the problem " << le_nom() << " after " << statistiques().last_time(temporary_counter_) << " s" << finl;
  return 1;
}

/*! @brief Demande au schema en temps s'il faut faire une impression
 *
 * @return (int) 1 il faut faire une impression, 0 il ne faut pas.
 */
int Probleme_base::limpr() const
{
  return schema_temps().limpr();
}

/*! @brief Demande au schema en temps s'il faut faire une sauvegarde
 *
 * @return (int) 1 il faut faire une sauvegarde, 0 il ne faut pas.
 */
int Probleme_base::lsauv() const
{
  return schema_temps().lsauv();
}

/*! @brief Imprime les equations associees au probleme si le schema en temps associe indique que c'est necessaire.
 *
 * @param (Sortie& os) le flot de sortie
 */
void Probleme_base::imprimer(Sortie& os) const
{
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).imprimer(os);
}

/*! @brief Associe un schema en temps au probleme.
 *
 * Associe ensuite le schema en temps a toutes
 *     les equations du probleme.
 *
 * @param (Schema_Temps_base& un_schema_en_temps) le schema en temps a associer
 */
void Probleme_base::associer_sch_tps_base(const Schema_Temps_base& un_schema_en_temps)
{
  if (le_schema_en_temps_.non_nul())
    {
      Cerr << finl;
      Cerr<<"Error: Problem "<<le_nom()<<" was already associated with the scheme "<< le_schema_en_temps_->le_nom()<<" and we try to associate it with "<<un_schema_en_temps.le_nom() << "." <<finl;
      exit();
    }
  le_schema_en_temps_=un_schema_en_temps;
  le_schema_en_temps_->associer_pb(*this);
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).associer_sch_tps_base(un_schema_en_temps);
}

/*! @brief Renvoie le schema en temps associe au probleme.
 *
 * (si il est non nul) (version const)
 *
 * @return (Schema_Temps_base&) le schema en temps associe au probleme
 * @throws le schema en temps n'est pas associe au probleme, la reference est nulle
 */
const Schema_Temps_base& Probleme_base::schema_temps() const
{
  if(!le_schema_en_temps_.non_nul())
    {
      Cerr << le_nom() << " has not been associated to a time scheme !" << finl;
      exit();
    }
  return le_schema_en_temps_.valeur();
}


/*! @brief Renvoie le schema en temps associe au probleme.
 *
 * (si il est non nul)
 *
 * @return (Schema_Temps_base&) le schema en temps associe au probleme
 * @throws le schema en temps n'est pas associe au probleme, la reference est nulle
 */
Schema_Temps_base& Probleme_base::schema_temps()
{
  if(!le_schema_en_temps_.non_nul())
    {
      Cerr << le_nom() << " has not been associated to a time scheme !" << finl;
      exit();
    }
  return le_schema_en_temps_.valeur();
}


/*! @brief Renvoie le domaine associe au probleme.
 *
 * (version const)
 *
 * @return (Domaine&) un domaine
 */
const Domaine& Probleme_base::domaine() const
{
  return le_domaine_.valeur();
}

/*! @brief Renvoie le domaine associe au probleme.
 *
 * @return (Domaine&) un domaine
 */
Domaine& Probleme_base::domaine()
{
  return le_domaine_.valeur();
}

/*! @brief Renvoie le domaine discretise associe au probleme.
 *
 * (version const)
 *
 * @return (Domaine_dis_base&) un domaine discretise
 */
const Domaine_dis_base& Probleme_base::domaine_dis() const
{
  return le_domaine_dis_.valeur();
}

/*! @brief Renvoie le domaine discretise associe au probleme.
 *
 * @return (Domaine_dis_base&) un domaine discretise
 */
Domaine_dis_base& Probleme_base::domaine_dis()
{
  return le_domaine_dis_.valeur();
}

/*! @brief Associe un milieu physique aux equations du probleme.
 *
 * Choix du milieu physique.
 *
 * @param (Milieu_base& mil) le milieu a associer (Solide, Fluide Incompressible ...)
 */
void Probleme_base::associer_milieu_base(const Milieu_base& mil)
{
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).associer_milieu_base(mil);
}

/*! @brief Renvoie le milieu physique associe au probleme.
 *
 * (version const)
 *     On renvoie le milieu associe a la premiere equation.
 *
 * @return (Milieu_base&) un milieu physique
 */
const Milieu_base& Probleme_base::milieu() const
{
  return equation(0).milieu();
}

/*! @brief Renvoie le milieu physique associe au probleme.
 *
 * On renvoie le milieu associe a la premiere equation.
 *
 * @return (Milieu_base&) un milieu physique
 */
Milieu_base& Probleme_base::milieu()
{
  return equation(0).milieu();
}

/*! @brief Renvoie l'equation dont le nom est specifie.
 *
 * On indexe les equations avec leur nom associe.
 *     On cherche dans toutes les equations du probleme celle
 *     qui porte le nom specifie.
 *     (version const)
 *
 * @param (Nom& type) le nom de l'equation a renvoyer
 * @return (Equation_base&) une equation
 * @throws pas d'equation du nom specifie
 */
const Equation_base& Probleme_base::equation(const Nom& type) const
{
  Motcle Type(type), Type_eqn;
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      Type_eqn = equation(i).que_suis_je();
      if (Type_eqn == Type)
        return equation(i);

      // on teste si synonyme ...
      const Synonyme_info *syn_info = Synonyme_info::synonyme_info_from_name(type);
      if (syn_info != 0)
        if (Motcle(syn_info->org_name_()) == Type_eqn)
          return equation(i);
    }
  Cerr << que_suis_je() << " does not contain any equation/medium of type: " << type << finl;
  Cerr << "Here is the list of possible equations for a " << que_suis_je() << " problem: " << finl;
  for (int i = 0; i < nombre_d_equations(); i++)
    Cerr << "\t- " << equation(i).que_suis_je() << finl;
  Process::exit();
  // Pour les compilos;
  return equation(0);
}

/*! @brief (B. Math): Methode virtuelle ajoutee pour les problemes ayant plusieurs equations
 *   de meme type (Probleme_FT_Disc_gen). Dans ce cas, le nom de l'equation
 *   n'est pas son type...
 *
 */
const Equation_base& Probleme_base::get_equation_by_name(const Nom& un_nom) const
{
  return equation(un_nom);
}

/*! @brief (B. Math): Methode virtuelle ajoutee pour les problemes ayant plusieurs equations
 *   de meme type (Probleme_FT_Disc_gen). Dans ce cas, le nom de l'equation
 *   n'est pas son type...
 *   Version non const. Cette methode est notamment appelee a la lecture du probleme.
 *
 */
Equation_base& Probleme_base::getset_equation_by_name(const Nom& un_nom)
{
  return equation(un_nom);
}

/*! @brief Renvoie l'equation dont le nom est specifie.
 *
 * On indexe les equations avec leur nom associe.
 *     On cherche dans toutes les equations du probleme celle
 *     qui porte le nom specifie.
 *
 * @param (Nom& type) le nom de l'equation a renvoyer
 * @return (Equation_base&) une equation
 * @throws pas d'equation du nom specifie
 */
Equation_base& Probleme_base::equation(const Nom& type)
{
  Motcle Type(type), Type_eqn;
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      Type_eqn = equation(i).que_suis_je();
      if (Type_eqn == Type)
        return equation(i);

      // on teste si synonyme ...
      const Synonyme_info *syn_info = Synonyme_info::synonyme_info_from_name(type);
      if (syn_info != 0)
        if (Motcle(syn_info->org_name_()) == Type_eqn)
          return equation(i);
    }
  Cerr << que_suis_je() << " does not contain any equation/medium of type: " << type << finl;
  Cerr << "Here is the list of possible equations for a " << que_suis_je() << " problem: " << finl;
  for (int i = 0; i < nombre_d_equations(); i++)
    Cerr << "\t- " << equation(i).que_suis_je() << finl;
  Process::exit();
  // Pour les compilos;
  return equation(0);
}

void Probleme_base::creer_champ(const Motcle& motlu)
{
  domaine_dis().creer_champ(motlu, *this);
  milieu().creer_champ(motlu);

  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).creer_champ(motlu);

  for (auto &itr : liste_loi_fermeture_)
    itr->creer_champ(motlu);
}

bool Probleme_base::has_champ(const Motcle& un_nom, OBS_PTR(Champ_base) &ref_champ) const
{
  if (domaine_dis().has_champ(un_nom, ref_champ))
    return true;

  for (int i = 0; i < nombre_d_equations(); i++)
    {
      if (equation(i).has_champ(un_nom, ref_champ))
        return true;

      if (equation(i).milieu().has_champ(un_nom, ref_champ))
        return true;
    }

  for (const auto &corr : correlations_)
    if (corr.second->has_champ(un_nom, ref_champ))
      return true;

  for (const auto &itr : liste_loi_fermeture_)
    if (itr->has_champ(un_nom, ref_champ))
      return true;

  return false; /* rien trouve */
}

bool Probleme_base::has_champ(const Motcle& un_nom) const
{
  if (domaine_dis().has_champ(un_nom))
    return true;

  for (int i = 0; i < nombre_d_equations(); i++)
    {
      if (equation(i).has_champ(un_nom))
        return true;

      if (equation(i).milieu().has_champ(un_nom))
        return true;
    }

  for (const auto &corr : correlations_)
    if (corr.second->has_champ(un_nom))
      return true;

  for (const auto &itr : liste_loi_fermeture_)
    if (itr->has_champ(un_nom))
      return true;

  return false; /* rien trouve */
}

const Champ_base& Probleme_base::get_champ(const Motcle& un_nom) const
{
  OBS_PTR(Champ_base) ref_champ;

  if (domaine_dis().has_champ(un_nom, ref_champ))
    return ref_champ;

  for (int i = 0; i < nombre_d_equations(); i++)
    {
      if (equation(i).has_champ(un_nom, ref_champ))
        return ref_champ;

      if (equation(i).milieu().has_champ(un_nom, ref_champ))
        return ref_champ;
    }

  for (const auto &corr : correlations_)
    if (corr.second->has_champ(un_nom, ref_champ))
      return ref_champ;

  for (const auto &itr : liste_loi_fermeture_)
    if (itr->has_champ(un_nom, ref_champ))
      return ref_champ;

  Cerr << "The field of name " << un_nom << " do not correspond to a field understood by the problem." << finl;
  Cerr << "It may be a field dedicated only to post-process and defined in the Definition_champs set." << finl;
  Cerr << "1) If you have request the post-processing of " << un_nom << " in the Champs set" << finl;
  Cerr << "please remove the localisation elem or som that you may have specified." << finl;
  Cerr << "2) If you have used " << un_nom << " in Definition_champs, please use 'sources_reference { " << un_nom << " }'" << finl;
  Cerr << "instead of 'source refchamp { pb_champ " << le_nom() << " " << un_nom << " }'" << finl;
  Cerr << "3) Check reference manual." << finl;
  Cerr << "4) Contact TRUST support." << finl;
  Process::exit();

  throw std::runtime_error(std::string("Field ") + un_nom.getString() + std::string(" not found !"));
}

void Probleme_base::get_noms_champs_postraitables(Noms& noms,Option opt) const
{
  domaine_dis().get_noms_champs_postraitables(noms, opt);
  milieu().get_noms_champs_postraitables(noms,opt);
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    equation(i).get_noms_champs_postraitables(noms,opt);

  for (const auto& itr : liste_loi_fermeture_)
    {
      const Loi_Fermeture_base& loi=itr.valeur();
      loi.get_noms_champs_postraitables(noms,opt);
    }
}

int Probleme_base::comprend_champ_post(const Motcle& un_nom) const
{
  if (un_nom == "TEMPERATURE_PHYSIQUE") return 0;

  for (const auto &itr : postraitements())
    {
      const Postraitement& post = ref_cast(Postraitement, itr.valeur());
      if (post.comprend_champ_post(un_nom))
        return 1;
    }
  return 0;
}

bool Probleme_base::has_champ_post(const Motcle& un_nom) const
{
  for (const auto &itr : postraitements())
    if (sub_type(Postraitement, itr.valeur()))
      {
        const Postraitement& post = ref_cast(Postraitement, itr.valeur());
        if (post.has_champ_post(un_nom))
          return true;
      }

  return false; /* rien trouve */
}

const Champ_Generique_base& Probleme_base::get_champ_post(const Motcle& un_nom) const
{
  for (const auto &itr : postraitements())
    if (sub_type(Postraitement, itr.valeur()))
      {
        const Postraitement& post = ref_cast(Postraitement, itr.valeur());
        if (post.has_champ_post(un_nom))
          return post.get_champ_post(un_nom);
      }

  Cerr << "The field named " << un_nom << " do not correspond to a field understood by the problem." << finl;
  Cerr << "Check the name of the field indicated into the postprocessing block of the data file " << finl;
  Cerr << "or in the list of post-processed fields above (in the block 'Reading of fields to be postprocessed')." << finl;
  Process::exit();

  throw std::runtime_error(std::string("Field ") + un_nom.getString() + std::string(" not found !"));
}

int Probleme_base::a_pour_IntVect(const Motcle&, OBS_PTR(IntVect)& ) const
{
  return 0;
}

/*! @brief Effectue une mise a jour en temps du probleme.
 *
 * Effectue la mise a jour sur toutes les equations du probleme.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
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
  les_postraitements_.mettre_a_jour(temps);

  for (auto& itr : liste_loi_fermeture_)
    {
      Loi_Fermeture_base& loi=itr.valeur();
      loi.mettre_a_jour(temps);
    }
  for (auto &corr : correlations_)
    corr.second->mettre_a_jour(temps);
}

/*! @brief Prepare le calcul: initialise les parametres du milieu et prepare le calcul de chacune des equations.
 *
 */
void Probleme_base::preparer_calcul()
{
  const double temps = schema_temps().temps_courant();
  // Modification du tableau Qdm porte par le domaine_dis() dans le cas
  // ou il y a des conditions aux limites periodiques.
  // Rq : Si l'une des equations porte la condition a la limite periodique
  //      alors les autres doivent forcement la porter.
  equation(0).domaine_dis().modifier_pour_Cl(equation(0).domaine_Cl_dis().les_conditions_limites());
  milieu().initialiser(temps);
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).preparer_calcul();
  milieu().preparer_calcul();
  for (int i = 0; i < nombre_d_equations(); i++) /* on peut maintenant remplir les champs conserves */
    equation(i).mettre_a_jour_champs_conserves(temps);

  if (schema_temps().file_allocation() && EcritureLectureSpecial::Active)
    save_restart_.file_size_xyz();

  for (auto& itr : liste_loi_fermeture_)
    {
      Loi_Fermeture_base& loi = itr.valeur();
      loi.preparer_calcul();
    }

  if (correlations_.size() > 0)
    mettre_a_jour(temps);
}


/*! @brief Calcul la valeur du prochain pas de temps du probleme.
 *
 * On calcule le minimum des pas de temps des equations associees
 *     au probleme.
 *
 * @return (double) le pas de temps maximum autorise pour ce probleme
 */
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


/*! @brief Verifie que la place necessaire existe sur le disque dur.
 *
 */
void Probleme_base::allocation() const
{
  if(schema_temps().file_allocation() && EcritureLectureSpecial::Active)        // Permet de tester l'allocation d'espace disque
    save_restart_.allocation();
}

/*! @brief Si force=1, effectue le postraitement sans tenir compte des frequences de postraitement.
 *
 *     Le postraitement est mis a jour et les traitements eventuels sur
 *     les sondes, champs et statistiques sont effectues.
 *   Si force=0, tient compte des frequences d'impression demandees.
 *
 */
int Probleme_base::postraiter(int force)
{
  statistiques().begin_count(postraitement_counter_);
  Schema_Temps_base& sch = schema_temps();
  Debog::set_nom_pb_actuel(le_nom());
  if (sch.nb_pas_dt() != 0)
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

      if (!((indice_nb_pas_dt) && (nb_pas_dt_max == 0)) && (!((indice_tps_final) && (est_egal(t_init, t_max)))))
        {
          for (int i = 0; i < nombre_d_equations(); i++)
            equation(i).sources().mettre_a_jour(schema_temps().temps_courant());

          les_postraitements_.mettre_a_jour(schema_temps().temps_courant());
          les_postraitements_.postraiter();
          if (nb_pas_dt_max == 0)
            indice_nb_pas_dt = 1;
          if (est_egal(t_init, t_max))
            indice_tps_final = 1;
        }
    }
  else
    les_postraitements_.traiter_postraitement();

  statistiques().end_count(postraitement_counter_);

  //Start specific postraitements for mobile domain (like ALE)
  if(!save_restart_.is_restart_in_progress())  //no projection during the iteration of resumption of computation
    {
      double temps = le_schema_en_temps_->temps_courant();
      le_domaine_dis_->domaine().update_after_post(temps);
    }

  save_restart_.set_restart_in_progress(false); //reset to false in order to make the following projections
  // end specific postraitements for mobile domain (like ALE)

  return 1;
}

/*! @brief Ecriture sur fichier en vue d'une reprise (sauvegarde)
 *
 */
void Probleme_base::sauver() const
{
  statistiques().begin_count(sauvegarde_counter_);
  int bytes = save_restart_.sauver();
  Debog::set_nom_pb_actuel(le_nom());
  statistiques().end_count(sauvegarde_counter_, bytes);
  Cout << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to write save file." << finl;
}

/*! @brief Finit le postraitement et sauve le probleme dans un fichier.
 *
 * Fermeture du fichier associe au postraitement.(Postraitement::finir())
 *
 */
void Probleme_base::finir()
{
  Debog::set_nom_pb_actuel(le_nom());
  schema_temps().finir(); // Fermeture du .dt_ev
  les_postraitements_.finir(); // Fermeture des fichiers de postraitements
  for (auto os : get_set_out_files())
    if (os->is_open())
      os->close(); // Fermeture des fichiers .out
  // Vide les variables globales suivantes (utile pour resetTime dans un meme repertoire)
  glob_noms_fichiers.vide();
  glob_derniers_posts.vide();

  if (schema_temps().temps_sauv() > 0.0)
    sauver();

  save_restart_.finir();
}

void Probleme_base::resetTime(double time)
{
  static const std::string param_name = "SORTIE_ROOT_DIRECTORY";
  std::string new_root_dir = (str_params_.count(param_name) == 0) ? "" : getOutputStringValue(param_name);

  resetTimeWithDir_impl(*this, time, new_root_dir);
}

/*! @brief Recherche des champs parametriques, et pour chacun, passage au parametre suivant
 *
 */
std::string Probleme_base::newCompute()
{
  std::string dirname="";
  // Boucle sur les champs des conditions limites:
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      const Equation_base& eq = equation(i);
      const Conds_lim& condsLim = eq.domaine_Cl_dis().les_conditions_limites();
      for (auto const &condLim : condsLim)
        {
          const Cond_lim_base& la_cl_base = condLim.valeur();
          if (sub_type(Champ_front_Parametrique, la_cl_base.champ_front()))
            {
              const Champ_front_Parametrique& champ_front = ref_cast(Champ_front_Parametrique, la_cl_base.champ_front());
              dirname = champ_front.newCompute();
            }
        }
    }
  // Boucles sur les champs des sources:
  if(Champ_Parametrique::enabled)
    {
      for (int i = 0; i < nombre_d_equations(); i++)
        {
          const Equation_base& eq = equation(i);
          const Sources& sources = eq.sources();
          for (auto &source: sources)
            {
              for (auto const &champ_don: source->champs_don())
                {
                  if (sub_type(Champ_Parametrique, champ_don.valeur()))
                    {
                      const Champ_Parametrique& champ = ref_cast(Champ_Parametrique, champ_don.valeur());
                      dirname = champ.newCompute();
                    }
                }
            }
        }
      // Boucle sur les champs du Milieu:
      for (auto const &champ_don: milieu().champs_don())
        {
          if (sub_type(Champ_Parametrique, champ_don.valeur()))
            {
              const Champ_Parametrique& champ = ref_cast(Champ_Parametrique, champ_don.valeur());
              dirname = champ.newCompute();
            }
        }
    }
  return dirname;
}

Entree& Probleme_base::read_optional_equations(Entree& is, Motcle& mot)
{
  /* lecture d'equations optionnelles */
  Noms noms_eq, noms_eq_maj; //noms de toutes les equations possibles!
  Type_info::les_sous_types(Nom("Equation_base"), noms_eq);
  for (auto& itr : noms_eq) noms_eq_maj.add(Motcle(itr)); //ha ha ha
  for (is >> mot; noms_eq_maj.rang(mot) >= 0; is >> mot)
    {
      eq_opt_.add(OWN_PTR(Equation_base)()); //une autre equation optionelle
      eq_opt_.dernier().typer(mot); //on lui donne le bon type
      Equation_base& eq = eq_opt_.dernier().valeur();
      //memes associations que pour les autres equations : probleme, milieu, schema en temps
      eq.associer_pb_base(*this);
      eq.associer_milieu_base(milieu());
      eq.associer_sch_tps_base(le_schema_en_temps_);
      eq.associer_domaine_dis(domaine_dis());
      eq.discretiser(); //a faire avant de lire l'equation
      is >> eq; //et c'est parti!
      eq.associer_milieu_equation(); //remontee vers le milieu
    }
  return is;
}

Entree& Probleme_base::lire_correlations(Entree& is)
{
  Motcle mot;
  is >> mot;
  if (mot != "{")
    {
      Cerr << "correlations : { expected instead of " << mot << finl;
      Process::exit();
    }
  for (is >> mot; mot != "}"; is >> mot)
    if (correlations_.count(mot.getString())) Process::exit(que_suis_je() + " : a correlation already exists for " + mot + " !");
    else
      Correlation_base::typer_lire_correlation(correlations_[mot.getString()], *this, mot, is);

  return is;
}
