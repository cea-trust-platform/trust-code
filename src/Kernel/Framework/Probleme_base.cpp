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
#include <Discretisation_base.h>
#include <Loi_Fermeture_base.h>
#include <EcrFicCollecteBin.h>
#include <LecFicDiffuseBin.h>
#include <communications.h>
#include <Probleme_base.h>
#include <Postraitement.h>
#include <stat_counters.h>
#include <FichierHDFPar.h>
#include <Milieu_base.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>
#include <sys/stat.h>
#include <Equation.h>
#include <Debog.h>
#include <Avanc.h>

#define CHECK_ALLOCATE 0
#ifdef CHECK_ALLOCATE
#include <unistd.h> // Pour acces a int close(int fd); avec PGI
#include <fcntl.h>
#include <errno.h>
#endif

Implemente_base_sans_destructeur(Probleme_base,"Probleme_base",Probleme_U);

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

// Variables globales pour initialiser est_le_premier_postraitement_pour_nom_fic
// et est_le_dernier_postraitement_pour_nom_fic en une seule passe.
LIST(Nom) glob_noms_fichiers;
LIST(REF(Postraitement)) glob_derniers_posts;

// Retourne la version du format de sauvegarde
// 151 pour dire que c'est la version initiee a la version 1.5.1 de TRUST
inline int version_format_sauvegarde() { return 184; }

Probleme_base::~Probleme_base()
{
  glob_noms_fichiers.vide();
  glob_derniers_posts.vide();
}

/*! @brief Surcharge Objet_U::printOn(Sortie&) Ecriture d'un probleme sur un flot de sortie.
 *
 *     !! Attention n'est pas symetrique de la lecture !!
 *     On ecrit les equations, le postraitement et le domaine
 *     discretise.
 *
 * @param (Sortie& os) flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
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

  /* 1 : milieu : NEW SYNTAX */
  if (!milieu_via_associer_ && !is_pb_med() && !is_pb_FT() && !is_pb_rayo())
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
  lire_sauvegarde_reprise(is,motlu);

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
      is >> le_milieu_[i]; // On commence par la lecture du milieu
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
Entree& Probleme_base::lire_equations(Entree& is, Motcle& dernier_mot)
{
  Nom un_nom;
  int nb_eq = nombre_d_equations();
  Cerr << "Reading of the equations" << finl;

  for (int i = 0; i < nb_eq; i++)
    {
      is >> un_nom;
      is >> getset_equation_by_name(un_nom);
    }
  is >> dernier_mot;
  return is;
}

/*! @brief Associe le probleme a toutes ses equations.
 *
 */
void Probleme_base::associer()
{
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).associer_pb_base(*this);
}

void Probleme_base::warn_old_syntax()
{
  if (!is_pb_FT() && !is_pb_rayo())
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
  une_discretisation.discretiser(le_domaine_dis_);
  // Can not do this before, since the Domaine_dis is not typed yet:
  le_domaine_dis_->associer_domaine(le_domaine_);

  if (milieu_via_associer_ || is_pb_FT())
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
      DERIV(Postraitement_base) &der_post = itr;

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
      Cerr<<"Error: Problem "<<le_nom()<<" was already associated with the scheme "<< le_schema_en_temps_.valeur().le_nom()<<" and we try to associate it with "<<un_schema_en_temps.le_nom() << "." <<finl;
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
 * @return (Domaine_dis&) un domaine discretise
 */
const Domaine_dis& Probleme_base::domaine_dis() const
{
  return le_domaine_dis_.valeur();
}

/*! @brief Renvoie le domaine discretise associe au probleme.
 *
 * @return (Domaine_dis&) un domaine discretise
 */
Domaine_dis& Probleme_base::domaine_dis()
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
  Motcle Type(type);
  Motcle Type_eqn;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Type_eqn = equation(i).que_suis_je();
      if(Type_eqn==Type)
        return equation(i);
    }
  Cerr << que_suis_je() << " does not contain any equation/medium of type: " << type << finl;
  Cerr << "Here is the list of possible equations for a " << que_suis_je() << " problem: " << finl;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Cerr << "\t- " << equation(i).que_suis_je() << finl;
    }
  exit();
  // Pour les compilos;
  return equation(0);
}

/*! @brief (B.
 *
 * Math): Methode virtuelle ajoutee pour les problemes ayant plusieurs equations
 *   de meme type (Probleme_FT_Disc_gen). Dans ce cas, le nom de l'equation
 *   n'est pas son type...
 *
 */
const Equation_base& Probleme_base::get_equation_by_name(const Nom& un_nom) const
{
  return equation(un_nom);
}

/*! @brief (B.
 *
 * Math): Methode virtuelle ajoutee pour les problemes ayant plusieurs equations
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
  Motcle Type(type);
  Motcle Type_eqn;
  for(int i=0; i<nombre_d_equations(); i++)
    {
      Type_eqn = equation(i).que_suis_je();
      if(Type_eqn==Type)
        return equation(i);
    }
  Cerr << que_suis_je() << " does not contain any equation/medium of type: " << type << finl;
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
  domaine_dis()->creer_champ(motlu, *this);
  milieu().creer_champ(motlu);
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    equation(i).creer_champ(motlu);

  for (auto& itr : liste_loi_fermeture_)
    {
      Loi_Fermeture_base& loi=itr.valeur();
      loi.creer_champ(motlu);
    }
}

bool Probleme_base::has_champ(const Motcle& un_nom) const
{
  Champ_base const * champ = nullptr ;
  try
    {
      champ = &domaine_dis()->get_champ(un_nom);
      if (champ) return true ;
    }
  catch (Champs_compris_erreur&)
    {
    }
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    {
      try
        {
          champ = &equation(i).get_champ(un_nom);
          if (champ) return true ;
        }
      catch (Champs_compris_erreur&)
        {
        }
      try
        {
          champ = &equation(i).milieu().get_champ(un_nom);
          if (champ) return true ;
        }
      catch (Champs_compris_erreur&)
        {
        }
    }

  for (const auto& itr : liste_loi_fermeture_)
    {
      const Loi_Fermeture_base& loi=itr.valeur();
      try
        {
          champ = &loi.get_champ(un_nom);
          if (champ) return true ;
        }
      catch(Champs_compris_erreur&)
        {
        }
    }
  return false;
}

const Champ_base& Probleme_base::get_champ(const Motcle& un_nom) const
{
  try
    {
      return domaine_dis()->get_champ(un_nom);
    }
  catch (Champs_compris_erreur&)
    {
    }
  int nb_eq = nombre_d_equations();
  for (int i=0; i<nb_eq; i++)
    {
      try
        {
          return equation(i).get_champ(un_nom);
        }
      catch (Champs_compris_erreur&)
        {
        }
      try
        {
          return equation(i).milieu().get_champ(un_nom);
        }
      catch (Champs_compris_erreur&)
        {
        }
    }

  for (const auto& itr : liste_loi_fermeture_)
    {
      const Loi_Fermeture_base& loi=itr.valeur();
      try
        {
          return loi.get_champ(un_nom);
        }
      catch(Champs_compris_erreur&)
        {
        }
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
  domaine_dis()->get_noms_champs_postraitables(noms, opt);
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

const Champ_Generique_base& Probleme_base::get_champ_post(const Motcle& un_nom) const
{
  REF(Champ_Generique_base) ref_champ;

  for (const auto &itr : postraitements())
    {
      if (sub_type(Postraitement, itr.valeur()))
        {
          const Postraitement& post = ref_cast(Postraitement, itr.valeur());
          try
            {
              return post.get_champ_post(un_nom);
            }
          catch (Champs_compris_erreur&) { }
        }
    }
  Cerr<<" "<<finl;
  Cerr<<"The field named "<<un_nom<<" do not correspond to a field understood by the problem."<<finl;
  Cerr<<"Check the name of the field indicated into the postprocessing block of the data file " << finl;
  Cerr<<"or in the list of post-processed fields above (in the block 'Reading of fields to be postprocessed')."<<finl;
  Process::exit();

  //Pour compilation
  return ref_champ;
}

int Probleme_base::a_pour_IntVect(const Motcle&, REF(IntVect)& ) const
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
  equation(0).domaine_dis()->modifier_pour_Cl(equation(0).domaine_Cl_dis().les_conditions_limites());
  milieu().initialiser(temps);
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).preparer_calcul();
  milieu().preparer_calcul();
  for (int i = 0; i < nombre_d_equations(); i++) /* on peut maintenant remplir les champs conserves */
    equation(i).mettre_a_jour_champs_conserves(temps);

  if (schema_temps().file_allocation() && EcritureLectureSpecial::Active)
    file_size_xyz();

  for (auto& itr : liste_loi_fermeture_)
    {
      Loi_Fermeture_base& loi = itr.valeur();
      loi.preparer_calcul();
    }
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

/*! @brief Initialisation de file_size, bad_allocate, nb_pb_total, num_pb
 *
 */
long int Probleme_base::File_size_=0;        // file_size est l'espace disque en octet necessaire pour ecrire les fichiers XYZ
int Probleme_base::Bad_allocate_=1;        // bad_allocate est un int qui permet de savoir si l'allocation a deja eut lieu
int Probleme_base::Nb_pb_total_=0;        // nb_pb_total est le nombre total de probleme
int Probleme_base::Num_pb_=1;                // num_pb est le numero du probleme courant

/*! @brief Verifie que la place necessaire existe sur le disque dur.
 *
 */
void Probleme_base::allocation() const
{
  if(schema_temps().file_allocation() && EcritureLectureSpecial::Active)        // Permet de tester l'allocation d'espace disque
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
              barrier();
              exit();
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
int Probleme_base::file_size_xyz() const
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

void Probleme_base::sauver_xyz(int verbose) const
{
  statistiques().begin_count(sauvegarde_counter_);
  Nom nom_fich_xyz("");
  if (verbose)
    {
      nom_fich_xyz += Objet_U::nom_du_cas();
      nom_fich_xyz += "_";
      nom_fich_xyz += le_nom();
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
  int bytes = sauvegarder(ficsauv_.valeur());
  EcritureLectureSpecial::mode_ecr = -1;

  if (Process::je_suis_maitre())
    ficsauv_.valeur() << Nom("fin");
  (ficsauv_.valeur()).flush();
  (ficsauv_.valeur()).syncfile();
  ficsauv_.detach();
  Cout << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to write xyz file." << finl;
  statistiques().end_count(sauvegarde_counter_, bytes);
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
    {
      les_postraitements_.traiter_postraitement();
    }
  statistiques().end_count(postraitement_counter_);

  //Start specific postraitements for mobile domain (like ALE)
  if(!restart_in_progress_)  //no projection during the iteration of resumption of computation
    {
      double temps = le_schema_en_temps_->temps_courant();
      le_domaine_dis_->domaine().update_after_post(temps);
    }
  restart_in_progress_=0; //reset to false in order to make the following projections
  // end specific postraitements for mobile domain (like ALE)

  return 1;
}

void Probleme_base::lire_sauvegarde_reprise(Entree& is, Motcle& motlu)
{
  // XXX Elie Saikali : for PolyMAC_P0 => No xyz for the moment
  if (la_discretisation_->is_polymac_p0())
    {
      Cerr << "Problem "  << le_nom() << " with the discretization " << la_discretisation_->que_suis_je() <<  " => EcritureLectureSpecial = 0 !" << finl;
      EcritureLectureSpecial::Active = 0;
    }
  restart_format_ = "binaire";
  restart_file_name_ = Objet_U::nom_du_cas();
  // Desormais les fichiers de sauvegarde sont nommes par defaut nomducas_nomdupb.sauv
  restart_file_name_ += "_";
  restart_file_name_ += le_nom();
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
          if (la_discretisation_->is_polymac_p0() && format_rep != "binaire")
            {
              Cerr << "Error in Probleme_base::" << __func__ << " !! " << finl;
              Cerr << "Only the binary format is currently supported to resume a simulation with the discretization " << la_discretisation_->que_suis_je() << " ! " << finl;
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
              if (format_rep == "single_hdf")
                {
#ifdef MPI_
                  last_time = get_last_time(input_data);
#endif
                }
              else
                last_time = get_last_time(fic);
              // Set the time to restart the calculation
              schema_temps().set_temps_courant() = last_time;
              // Initialize tinit and current time according last_time
              if (schema_temps().temps_init() > -DMAXFLOAT)
                {
                  Cerr << "tinit was defined in .data file to " << schema_temps().temps_init() << ". The value is fixed to " << last_time << " accroding to resume_last_time_option" << finl;
                }
              schema_temps().set_temps_init() = last_time;
              schema_temps().set_temps_precedent() = last_time;
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
                  exit();
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
                  exit();
                }
              if (restart_version_ > version_format_sauvegarde())
                {
                  Cerr << "The format " << restart_version_ << " of the resumption file " << nomfic << " is posterior" << finl;
                  Cerr << "to the format " << version_format_sauvegarde() << " recognized by this version of TRUST." << finl;
                  Cerr << "Please use a more recent version." << finl;
                  exit();
                }
            }
          // Ecriture du format de reprise
          Cerr << "The version of the resumption format of file " << nomfic << " is " << restart_version_ << finl;
          if (format_rep != "single_hdf")
            reprendre(fic.valeur());
          else
            {
#ifdef MPI_
              reprendre(input_data);
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
          exit();
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
      exit();
    }
  if (schema_temps().temps_init() <= -DMAXFLOAT)
    {
      schema_temps().set_temps_init() = 0;
      schema_temps().set_temps_courant() = 0;
    }

  if (reprise_effectuee())
    {
      // on teste si dt_ev existe sinon on met reprise a 2
      // on recrera l'entete dans dt_ev sinon l'entete est fausse en reprise de pb_couple
      Nom fichier(nom_du_cas());
      fichier += ".dt_ev";
      struct stat f;
      if (stat(fichier, &f))
        reprise_effectuee() = 2;
    }
}

/*! @brief Ecriture sur fichier en vue d'une reprise (sauvegarde)
 *
 */
void Probleme_base::sauver() const
{
  statistiques().begin_count(sauvegarde_counter_);

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
          exit();
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
    bytes = sauvegarder(ficsauv_.valeur());
  else
    bytes = sauvegarder(*osauv_hdf_);

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
  schema_temps().finir();
  les_postraitements_.finir();
  if (schema_temps().temps_sauv() > 0.0)
    sauver();

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


void Probleme_base::resetTime(double time)
{
  static const std::string param_name = "SORTIE_ROOT_DIRECTORY";
  std::string new_root_dir = (str_params_.count(param_name) == 0) ? "" : getOutputStringValue(param_name);

  resetTimeWithDir_impl(*this, time, new_root_dir);
}

/*! @brief Recherche des champs parametriques, et pour chacun, passage au parametre suivant
 *
 */
int Probleme_base::newCompute()
{
  int compute = 0;
  // Boucle sur les champs des conditions limites:
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      const Equation_base& eq = equation(i);
      const Conds_lim& condsLim = eq.domaine_Cl_dis().les_conditions_limites();
      for (auto const &condLim : condsLim)
        {
          const Cond_lim_base& la_cl_base = condLim.valeur();
          if (sub_type(Champ_front_Parametrique, la_cl_base.champ_front().valeur()))
            {
              const Champ_front_Parametrique& champ_front = ref_cast(Champ_front_Parametrique, la_cl_base.champ_front().valeur());
              compute = champ_front.newCompute();
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
              for (auto const &champ_don: source.valeur().champs_don())
                {
                  if (champ_don->non_nul() && sub_type(Champ_Parametrique, champ_don->valeur()))
                    {
                      const Champ_Parametrique& champ = ref_cast(Champ_Parametrique, champ_don->valeur());
                      compute = champ.newCompute();
                    }
                }
            }
        }
      // Boucle sur les champs du Milieu:
      for (auto const &champ_don: milieu().champs_don())
        {
          if (champ_don->non_nul() && sub_type(Champ_Parametrique, champ_don->valeur()))
            {
              const Champ_Parametrique& champ = ref_cast(Champ_Parametrique, champ_don->valeur());
              compute = champ.newCompute();
            }
        }
    }
  return compute;
}



