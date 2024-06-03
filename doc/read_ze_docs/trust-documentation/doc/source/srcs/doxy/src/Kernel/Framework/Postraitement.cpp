/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Postraitement.h>
#include <EFichier.h>
#include <Domaine_VF.h>
#include <Champ_Generique_Interpolation.h>
#include <Champ_Generique_refChamp.h>
#include <Champ_Generique_Correlation.h>
#include <Champ_Generique_Statistiques.h>
#include <Entree_complete.h>
#include <EcritureLectureSpecial.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Operateur_base.h>
#include <Operateur.h>
#include <Param.h>
#include <LecFicDiffuse_JDD.h>
#include <Create_domain_from_sub_domain.h>
#include <Sous_Domaine.h>
#include <EChaine.h>
#include <Interprete_bloc.h>



Implemente_instanciable_sans_constructeur_ni_destructeur(Postraitement,"Postraitement|Post_processing",Postraitement_base);

Postraitement::~Postraitement()
{
  //Cerr<<"remise a zero des noms des sondes" <<finl;
  // quand on detruit un postraitement on detruit la liste des noms des sondes pour pouvoir refaire initialize
  Postraitement::noms_fichiers_sondes_=LIST(Nom)();
}

void Postraitement::associer_nom_et_pb_base(const Nom& nom,
                                            const Probleme_base& pb)
{
  Postraitement_base::associer_nom_et_pb_base(nom, pb);
}

// Si forcer != 0, on postraite quel que soit le temps courant,
// sinon on postraite si l'intervalle entre les postraitements demandes
// est ecoule.
void Postraitement::postraiter(int forcer)
{
  if (forcer)
    {
      if (sondes_demande_)   postraiter_sondes();
      if (besoin_postraiter_champs())   postraiter_champs();
      if (tableaux_demande_) postraiter_tableaux();
    }
  else
    {
      if (sondes_demande_)   traiter_sondes();
      if (besoin_postraiter_champs())   traiter_champs();
      if (tableaux_demande_) traiter_tableaux();
    }
  dernier_temps=mon_probleme->schema_temps().temps_courant();

  // Cas des statistiques en serie, il faut traiter APRES le postraitement
  if (lserie_)
    {
      if (sup_ou_egal(dernier_temps-tstat_deb_,dt_integr_serie_))
        {
          tstat_deb_ = dernier_temps;
          tstat_dernier_calcul_ = tstat_deb_;
          tstat_fin_ = tstat_deb_ + dt_integr_serie_;

          for (auto &itr : champs_post_complet_)
            {
              if (sub_type(Champ_Gen_de_Champs_Gen, itr.valeur()))
                {
                  Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen, itr.valeur());
                  champ_post.fixer_serie(tstat_deb_, tstat_fin_);
                }
            }
        }
    }
}

void Postraitement::mettre_a_jour(double temps)
{
  //Mise a jour des operateurs statistiques portes par les Champ_Generique_Statistiques
  for (auto &itr : champs_post_complet_) itr->mettre_a_jour(temps);

  if ( inf_ou_egal(tstat_deb_,temps) &&  inf_ou_egal(temps,tstat_fin_) )
    tstat_dernier_calcul_ =  temps;// Il y a eu mise a jour effective des integrales
  if ( sup_ou_egal(temps, tstat_fin_))  // si reprise du calcul apres la fin des stats
    {
      double dt=probleme().schema_temps().pas_de_temps(); // si dt cst !!!
      if (sup_strict(dt,0)) // pour eviter une division par 0
        tstat_dernier_calcul_ =  floor(tstat_fin_/dt)*dt;
    }
}

Motcles Postraitement::formats_supportes=Motcles(0);
LIST(Nom) Postraitement::noms_fichiers_sondes_=LIST(Nom)();


inline void nom_fichier(const Postraitement& post, const Champ_Generique_Statistiques& op, const Domaine& dom, Nom& nom_fichier)
{
  nom_fichier+=".";
  const Entity& loc = op->get_localisation();
  Nom loc_post;

  //contenu de la methode get_nom_localisation()
  if (loc==ELEMENT)
    loc_post = "ELEM";
  else if (loc==NODE)
    loc_post = "SOM";
  else if (loc==FACE)
    loc_post = "FACES";
  else
    {
      Cerr<<"This location is not valid for the postprocessing"<<finl;
      Process::exit();
    }

  nom_fichier+=op->get_nom_post()+"."+loc_post+"."+dom.le_nom()+"."+post.probleme().le_nom()+".";
  char s[100];
  snprintf(s,100,"%.10f", op->get_time());
  nom_fichier+=Nom(s);
}

inline void nom_fichier(const Postraitement& post, const Nom& nom_post, const Nom& loc, const double temps_ch, const Domaine& dom, Nom& nom_fichier)
{
  nom_fichier+=".";
  nom_fichier+=nom_post+"."+loc+"."+dom.le_nom()+"."+post.probleme().le_nom()+".";
  char s[100];
  snprintf(s,100,"%.10f",temps_ch);
  nom_fichier+=Nom(s);
}

inline void nom_fichier(const Domaine& dom, Nom& nom_fichier)
{
  nom_fichier+=".";
  nom_fichier+=dom.le_nom();
}
inline int composante(const Nom& le_nom_, const Champ_base& mon_champ)
{
  Motcle motlu(le_nom_);
  const Nom& le_nom_ch = mon_champ.le_nom();
  if (motlu == Motcle(le_nom_ch))
    return -1;
  else
    {
      const Noms& noms_comp = mon_champ.noms_compo();
      int n = noms_comp.size();
      Motcles les_noms_comp(n);
      for (int i=0; i<n; i++)
        les_noms_comp[i] = noms_comp[i];
      int ncomp = les_noms_comp.search(motlu);
      if (ncomp == -1)
        {
          Cerr << "TRUST error, the Champ_Generique name : " << le_nom_ << finl
               << " is neither the field name nor"
               << " one of its components\n";
          Cerr << le_nom_ch << " have for components : " << finl;
          for (int ii=0; ii<n; ii++)
            Cerr << noms_comp[ii] << " ";
          Cerr << finl;
          Process::exit();
        }
      return ncomp;
    }

}

Nom Postraitement::get_nom_localisation(const Entity& loc)
{
  Nom loc_post;

  if (loc==ELEMENT)
    loc_post = "ELEM";
  else if (loc==NODE)
    loc_post = "SOM";
  else if (loc==FACE)
    loc_post = "FACES";
  else
    {
      Cerr<<"This location is not valid for the postprocessing"<<finl;
      exit();
    }

  return loc_post;
}

int Postraitement::champ_fonc(Motcle& nom_champ, REF(Champ_base)& mon_champ, REF(Operateur_Statistique_tps_base)& operateur_statistique) const
{

  if (comprend_champ_post(nom_champ))
    {
      const REF(Champ_Generique_base)& champ = get_champ_post(nom_champ);
      if (sub_type(Champ_Generique_Statistiques_base,champ.valeur()))
        {
          const Champ_Generique_Statistiques_base& champ_stat = ref_cast(Champ_Generique_Statistiques_base,champ.valeur());
          mon_champ = champ_stat.integrale();
          operateur_statistique = champ_stat.Operateur_Statistique();
          return 1;
        }
    }
  return 0;
}

/*! @brief Imprime le type de l'objet sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Postraitement::printOn(Sortie& s ) const
{
  s << que_suis_je() << finl;
  return s << finl;
}


/*! @brief Lit les directives de postraitement sur un flot d'entree.
 *
 * Format:
 *     Postraitement
 *     {
 *       Sondes bloc de lecture des sondes |
 *       Champs bloc de lecture des champs a postraiter |
 *       Statistiques bloc de lecture objet statistique |
 *       Fichier
 *     }
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouverte attendue
 * @throws lecture des champs: mot clef "dt_post" attendu
 * @throws lecture du bloc statistique: mot clef "dt_post" attendu
 * @throws mot clef inconnu
 */
Entree& Postraitement::readOn(Entree& s)
{
  Cerr << "Reading guidelines of postprocessing" << finl;
  assert(mon_probleme.non_nul());

  const Nom& le_nom_du_post = le_nom();
  Nom vide;
  if ((le_nom_du_post != "neant") && (le_nom_du_post != vide))
    {
      nom_fich_ += "_";
      nom_fich_ += le_nom_du_post;
    }

  Probleme_base& le_pb = mon_probleme.valeur();
  le_domaine = le_pb.domaine();

  Postraitement_base::readOn(s);

  if (Motcle(format) == "MEDFILE")
    {
      Cerr << "The postprocessing format 'medfile' is not supported since TRUST v1.9.2! Switch to 'med'." << finl;
      Process::exit();
    }

  if (Motcle(format) == "LATA_V1")
    {
      Cerr << "The postprocessing format 'lata_v1' is not supported since TRUST v1.9.1! Switch to 'lata' (version 2)." << finl;
      Process::exit();
    }

  bool is_supported = false, is_single_lata = false;
  std::vector<Motcle> supported = { "LATA", "SINGLE_LATA", "LATA_V2", "MED", "MED_MAJOR", "LML", "XYZ", "MESHTV" };

  for (auto &itr : supported)
    if (Motcle(format) == itr)
      {
        is_supported = true;
        break;
      }

  if (!is_supported)
    {
      Cerr << "The post-processing format " << Motcle(format) << " is not recognized! The recognized formats are : " << finl;
      for (auto &itr : supported) Cerr << "   - " << itr << finl;
      Process::exit();
    }

  if (Motcle(format) == "MED") format = "med";

  if (Motcle(format) == "MED_MAJOR") format = "med_major";

  if (Motcle(format) == "LATA_V2") format = "lata";

  if (Motcle(format) == "SINGLE_LATA")
    {
      is_single_lata = true;
      format = "lata";
    }

  nom_fich_ += ".";
  nom_fich_ += format;

  // Les sondes sont completees (en effet, si les sondes ont des champs statistiques, on n'a besoin d'avoir
  // lu le bloc statistiques ET le bloc sondes)
  les_sondes_.completer();

  //On type l objet Format_Post
  Nom type_format = "Format_Post_";
  type_format += format;
  format_post.typer_direct(type_format);

  if (is_single_lata)
    format_post->set_single_lata_option(is_single_lata);

  Nom base_name(nom_fich_);
  base_name.prefix(format);
  base_name.prefix(".");
  //format_post->initialize_by_default(base_name);
  format_post->initialize(base_name, binaire, option_para);

  //Le test de verification a ete simplifie entre la v1.5.1 et la v1.5.2
  //On simplifie donc la methode test_coherence
  format_post->test_coherence(champs_demande_, stat_demande_, dt_post_ch_, dt_post_stat_);

  return s;
}

void Postraitement::set_param(Param& param)
{
// XD postraitement postraitement_base postraitement -1 An object of post-processing (without name).
//  attr interfaces champs_posts interfaces 1 Keyword to read all the caracteristics of the interfaces. Different kind of interfaces exist as well as different interface intitialisations.
  param.ajouter("Fichier",&nom_fich_); // XD_ADD_P chaine Name of file.
  param.ajouter("Format",&format); // XD_ADD_P chaine(into=["lml","lata","single_lata","lata_v2","med","med_major"]) This optional parameter specifies the format of the output file. The basename used for the output file is the basename of the data file. For the fmt parameter, choices are lml or lata. A short description of each format can be found below. The default value is lml. single_lata is not compatible with 64 bits integer version.
  param.ajouter_non_std("Domaine",(this)); // XD_ADD_P chaine This optional parameter specifies the domain on which the data should be interpolated before it is written in the output file. The default is to write the data on the domain of the current problem (no interpolation).
  param.ajouter_non_std("Sous_domaine|Sous_zone",(this)); // XD_ADD_P chaine This optional parameter specifies the sub_domaine on which the data should be interpolated before it is written in the output file. It is only available for sequential computation.
  param.ajouter("Parallele",&option_para); // XD_ADD_P chaine(into=["simple","multiple","mpi-io"]) Select simple (single file, sequential write), multiple (several files, parallel write), or mpi-io (single file, parallel write) for LATA format
  param.ajouter_non_std("Definition_champs",(this));// XD_ADD_P definition_champs  Keyword to create new or more complex field for advanced postprocessing.
  param.ajouter_non_std("Definition_champs_fichier|Definition_champs_file",(this));// XD_ADD_P Definition_champs_fichier  Definition_champs read from file.
  param.ajouter_non_std("Sondes|Probes",(this)); // XD_ADD_P sondes Probe.
  param.ajouter_non_std("Sondes_mobiles|Mobile_probes",(this)); // XD_ADD_P sondes Mobile probes useful for ALE, their positions will be updated in the mesh.
  param.ajouter_non_std("Sondes_fichier|Probes_file",(this)); // XD_ADD_P sondes_fichier Probe read in a file.
  param.ajouter("DeprecatedKeepDuplicatedProbes",&DeprecatedKeepDuplicatedProbes); // XD_ADD_P entier Flag to not remove duplicated probes in .son files (1: keep duplicate probes, 0: remove duplicate probes)
  param.ajouter_non_std("champs|fields",(this)); // XD_ADD_P champs_posts Field\'s write mode.
  param.ajouter_non_std("Statistiques",(this));  // XD_ADD_P stats_posts Statistics between two points fixed : start of integration time and end of integration time.
  param.ajouter_non_std("Sondes_Int",(this));
  param.ajouter_non_std("Tableaux_Int",(this));
  param.ajouter_non_std("Statistiques_en_serie",(this));// XD_ADD_P stats_serie_posts Statistics between two points not fixed : on period of integration.
}

int Postraitement::lire_motcle_non_standard(const Motcle& mot, Entree& s)
{
  ////////////////////////////////////////
  // Creation d'alias pour certains champs
  ////////////////////////////////////////
  {
    Noms liste_noms;
    const Probleme_base& Pb = probleme();
    Pb.get_noms_champs_postraitables(liste_noms);
    // On ajoute temperature_physique aux champs definis
    if (liste_noms.rang("temperature") != -1 && !comprend_champ_post("temperature_physique"))
      {
        Motcle disc(probleme().discretisation().que_suis_je());
        if ((disc == "VEF") || (disc == "VEFPreP1b"))
          {
            Nom in("{ temperature_physique ");
            in += "Tparoi_VEF { source  refChamp { Pb_champ ";
            in += probleme().le_nom();
            in += " temperature } } }";
            Cerr << " Building of temperature_physique  " << in << finl;
            EChaine IN(in);
            lire_champs_operateurs(IN);
          }
      }
  }

  Motcle motlu;
  if (mot=="Sondes|Probes")
    {
      Cerr << "Reading of probes" << finl;
      les_sondes_.associer_post(*this);
      s >> les_sondes_;
      sondes_demande_ = 1;
      return 1;
    }
  if (mot=="Sondes_mobiles|Mobile_probes")
    {
      Cerr << "Reading of mobile probes" << finl;
      les_sondes_.associer_post(*this);
      s >>  les_sondes_;
      les_sondes_.set_update_positions(true);
      sondes_demande_ = 1;
      return 1;
    }

  if (mot=="Sondes_fichier|Probes_file")
    {
      Nom file;
      Param param2("probes_files");
      // XD sondes_fichier objet_lecture nul 1 not_set
      param2.ajouter("fichier|file",&file,Param::REQUIRED); // XD_ADD_P chaine name of file
      param2.lire_avec_accolades_depuis(s);

      Cerr << "Reading of probes" << finl;
      les_sondes_.associer_post(*this);
      les_sondes_.lire_fichier(file);
      sondes_demande_ = 1;
      return 1;
    }
  else if (mot=="champs|fields")
    {
      Cerr << "Reading of fields to be postprocessed" << finl;
      Noms liste_noms;
      Option opt=DESCRIPTION;
      mon_probleme->get_noms_champs_postraitables(liste_noms,opt);
      s >> motlu;
      if (motlu == "binaire")
        {
          binaire=1;
          s >> motlu;
        }
      else if (motlu == "formatte")
        {
          binaire=0;
          s >> motlu;
        }

      if (motlu == "dt_post")
        {
          Nom expression;
          s >> expression;
          fdt_post.setNbVar(1);
          fdt_post.setString(expression);
          fdt_post.addVar("t");
          fdt_post.parseString();
          dt_post_ch_ = fdt_post.eval();
        }
      else if (motlu == "nb_pas_dt_post")
        s >> nb_pas_dt_post_;
      else
        {
          Cerr << "Error while reading the input data for postprocessing :" << finl;
          Cerr << "We expected the keyword dt_post or nb_pas_dt_post" << finl;
          exit();
        }
      //La methode lire_champs_a_postraiter() va generer auatomatiquement un Champ_Generique
      //en fonction des indications du jeu de donnees (ancienne formulation)

      lire_champs_a_postraiter(s);
      champs_demande_ = 1;
      return 1;
    }
  else if (mot=="Statistiques")
    {
      Cerr << "Reading of the statistics block" << finl;
      s >> motlu;

      if (motlu == "dt_post")
        {
          Nom expression;
          s >> expression;
          fdt_post.setNbVar(1);
          fdt_post.setString(expression);
          fdt_post.addVar("t");
          fdt_post.parseString();
          dt_post_stat_ = fdt_post.eval();
        }
      else if (motlu == "nb_pas_dt_post")
        s >> nb_pas_dt_post_;
      else
        {
          Cerr << "Error while reading the input data for statistics postprocessing :" << finl;
          Cerr << "We expected the keyword dt_post or nb_pas_dt_post" << finl;
          exit();
        }
      //La methode lire_champs_stat_a_postraiter() va generer auatomatiquement un Champ_Generique
      //en fonction des indications du jeu de donnees (ancienne formulation)
      lire_champs_stat_a_postraiter(s);

      //Activer pour lancer la sauvegarde et la reprise des statistiques
      stat_demande_ = 1;
      return 1;
    }
  else if (mot=="Domaine")
    {
      if (champs_demande_ || stat_demande_ || sondes_demande_)
        {
          Cerr<<"The domain must be specified before reading Champs, Probes and Statistiques blocks at the beginning of the post-processing block."<<finl;
          exit();
        }

      Nom nom_du_domaine;
      s >> nom_du_domaine;
      le_domaine=ref_cast(Domaine,Interprete::objet(nom_du_domaine));
      return 1;
    }
  else if (mot=="Sous_domaine" || mot == "Sous_zone")
    {
      // Sanity check
      if (champs_demande_ || stat_demande_ || sondes_demande_)
        {
          Cerr<<"The domain must be specified before reading Champs, Probes and Statistiques blocks at the beginning of the post-processing block."<<finl;
          exit();
        }

      // Recuperation du sous-domaine
      Nom nom_du_sous_domaine;
      s >> nom_du_sous_domaine;
      Sous_Domaine le_sous_domaine;
      if (!Interprete_bloc::objet_global_existant(nom_du_sous_domaine))
        {
          Cerr << "Unknown sous_domaine : " << nom_du_sous_domaine << finl;
          Process::exit();
        }
      le_sous_domaine=ref_cast(Sous_Domaine,Interprete_bloc::objet_global(nom_du_sous_domaine));

      // Declaration du domaine
      Nom nom_du_dom(le_sous_domaine.domaine().le_nom());
      Nom nom_du_nouveau_dom = nom_du_dom + Nom("_") + nom_du_sous_domaine;

      Nom in("domaine ");
      in += nom_du_nouveau_dom;

      EChaine IN(in);
      Interprete_bloc::interprete_courant().interpreter_bloc(IN, Interprete_bloc::BLOC_EOF, 0);

      // Definition du domaine a partir du sous-domaine
      in = "Create_domain_from_sub_domain { domaine_final ";
      in += nom_du_nouveau_dom;
      in += " par_sous_zone ";
      in += nom_du_sous_domaine;
      in += " domaine_init ";
      in += nom_du_dom;
      in += " } ";

      EChaine IN2(in);
      Interprete_bloc::interprete_courant().interpreter_bloc(IN2, Interprete_bloc::BLOC_EOF, 0);
      le_domaine=ref_cast(Domaine,Interprete_bloc::objet_global(nom_du_nouveau_dom));

      return 1;
    }
  else if (mot=="Sondes_Int")
    {
      Cerr << "Reading of probes related to integers arrays" << finl;
      les_sondes_int_.associer_post(*this);
      s >> les_sondes_int_;
      les_sondes_int_.ouvrir_fichiers();
      sondes_demande_ = 1;
      return 1;
    }
  else if (mot=="Tableaux_Int")
    {
      Cerr << "Reading of integers arrays to be postprocessed "<< finl;
      s >> motlu;
      if (motlu != "dt_post")
        {
          Cerr << "Error while reading the statistics block:" << finl;
          Cerr << "We expected the keyword dt_post " << finl;
          exit();
        }
      s >> dt_post_tab;
      lire_tableaux_a_postraiter(s);
      tableaux_demande_ = 1;
      return 1;
    }
  else if (mot=="Statistiques_en_serie")
    {
      Cerr << "Reading of the serial statistics block" << finl;
      s >> motlu;
      if (motlu != "dt_integr")
        {
          Cerr << "Error while reading the statistics block:" << finl;
          Cerr << "We expected the keyword dt_integr " << finl;
          exit();
        }

      s >> dt_integr_serie_;
      dt_post_stat_ = dt_post_ch_;
      lire_champs_stat_a_postraiter(s);
      stat_demande_ = 1;
      lserie_=1;
      return 1;
    }
  else if (mot=="Definition_champs")
    {
      //La methode lire_champs_operateurs() permet la lecture d un champ a postraiter avec
      //la nouvelle formulation dans le jeu de donnees
      lire_champs_operateurs(s);
      return 1;
    }
  else if (mot=="Definition_champs_fichier|Definition_champs_file")
    {
      //bool depuisFichier = false;
      Nom file;
      Param param("definition_champ_files");
      // XD Definition_champs_fichier objet_lecture nul 1 Keyword to read definition_champs from a file
      param.ajouter("fichier|file",&file,Param::REQUIRED); // XD_ADD_P chaine name of file containing the definition of advanced fields
      param.lire_avec_accolades_depuis(s);

      Cerr << "Reading of Definition_champs from file " << file << finl;

      lire_fichier(file);

      return 1;
    }



  return -1;
}

/*! @brief Constructeur par defaut.
 *
 * Les frequences de postraitement prennent la valeur
 *     par defaut 1e6. Et aucun postraitement n'est demande.
 *
 */
Postraitement::Postraitement():
  est_le_premier_postraitement_pour_nom_fich_(-1), est_le_dernier_postraitement_pour_nom_fich_(-1),
  dt_post_ch_ (1.e6),
  dt_post_stat_(1.e6),
  dt_post_tab(1.e6),
  nb_pas_dt_post_((int)(pow(2.0,(double)((sizeof(True_int)*8)-1))-1)),
  nb_champs_stat_(0),
  tstat_deb_(-1), tstat_fin_(-1), tstat_dernier_calcul_(-1),
  lserie_(0),
  dt_integr_serie_(1.e6),
  sondes_demande_(0), champs_demande_(0), stat_demande_(0), stat_demande_definition_champs_(0),
  binaire(-1), tableaux_demande_(0),
  nom_fich_(nom_du_cas()),
  format("lml"),
  option_para("SIMPLE"),
  temps_(-1.), dernier_temps(-1.)
{
}

int Postraitement::sauvegarder(Sortie& os) const
{
  int bytes = 0;
  if ((stat_demande_) || (stat_demande_definition_champs_))
    {
      const Schema_Temps_base& sch = mon_probleme->schema_temps();
      double temps_courant = sch.temps_courant();
      // le test est necessaire pour avoir une ecriture lecture symetrique
      if (temps_courant>tstat_deb_)
        {
          // en mode ecriture special seul le maitre ecrit l'entete
          int a_faire,special;
          EcritureLectureSpecial::is_ecriture_special(special,a_faire);

          if (a_faire)
            {
              //On veut retrouver le nom precedent pour relecture des statistiques (format xyz)
              Nom mon_ident("Operateurs_Statistique_tps");
              mon_ident += probleme().domaine().le_nom();
              double temps = probleme().schema_temps().temps_courant();
              mon_ident += Nom(temps,"%e");
              os << mon_ident << finl;
              //On veut retrouver le nom precedent pour relecture des statistiques (format xyz)
              os << "Operateurs_Statistique_tps" << finl;
              os << nb_champs_stat_ << finl;
              os << tstat_deb_ << finl;
              os << tstat_dernier_calcul_ << finl ;
            }

          bytes += champs_post_complet_.sauvegarder(os);

          if (a_faire) os.flush();
        }
    }
  return bytes;

  /////////////////////////////////////////////////////////////////////
}

int Postraitement::reprendre(Entree& is)
{

  //Nouvelle formulation
  ////////////////////////////////////////////////////////////////////
  Cerr << "Postraitement::reprendre() " << finl;
  if ((stat_demande_) || (stat_demande_definition_champs_))
    {
      Schema_Temps_base& sch = mon_probleme->schema_temps();
      double temps_courant = sch.temps_courant();
      double tinit = probleme().schema_temps().temps_courant();
      if (temps_courant>tstat_deb_)
        {

          if (nb_champs_stat_!=0)
            {
              Nom bidon;
              is >> bidon;
              if (bidon=="fin")
                {
                  // Ce test evite un beau segmentation fault a la lecture
                  // du deuxieme bidon lors d'une sauvegarde/reprise au format binaire
                  Cerr << "End of the resumption file reached...." << finl;
                  Cerr << "This file contains no statistics." << finl;
                  Cerr << "The tinit time of resumption (= " << tinit << " ) must be less" << finl;
                  Cerr << "than the beginning time, t_deb (= " << tstat_deb_ << " ), of statistics calculation." << finl;
                  exit();
                }
              is >> bidon;
              int n;
              is >> n;
              double tstat_deb_sauv,temps_derniere_mise_a_jour_stats;
              is >> tstat_deb_sauv;
              is >> temps_derniere_mise_a_jour_stats;

              // Plusieurs cas possibles:
              if (inf_strict(tinit,temps_derniere_mise_a_jour_stats,1.e-5))
                {
                  Cerr << "The initial time " << tinit << " of the resumed calculation is less" << finl;
                  Cerr << "than the time of the last update of statistics " << temps_derniere_mise_a_jour_stats << finl;
                  Cerr << "We do not know how to treat this case." << finl;
                  exit();
                }
              else if ((!est_egal(tstat_deb_sauv,tstat_deb_)) && (!lserie_))
                {
                  // t_deb est modifie : on refait une statistique sans reprendre dans certains cas
                  if (inf_strict(tstat_deb_,tinit,1.e-5))
                    {
                      Cerr << "You have changed t_deb (old= " << tstat_deb_sauv << " new= " << tstat_deb_ << " ) to make a new statistical computing without resumption" << finl;
                      Cerr << "but t_deb (= " << tstat_deb_ << " ) is less than tinit (= "<< tinit << " ) : this is not possible." << finl;
                      exit();
                    }
                  Cerr << "We do not resume the statistics and thus the statistics calculation" << finl;
                  Cerr << "will restart at t_deb=" << tstat_deb_ << finl;

                  for (auto &itr : champs_post_complet_)
                    {
                      if (sub_type(Champ_Gen_de_Champs_Gen, itr.valeur()))
                        {
                          const Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen, itr.valeur());
                          champ_post.lire_bidon(is);
                        }
                    }
                }
              else // tinit=>temps_derniere_mise_a_jour_stats : on fait la reprise
                {
                  champs_post_complet_.reprendre(is);

                  // On modifie l'attribut tstat_deb_ et l'attribut t_debut_ des champs
                  // pour tenir compte de la reprise
                  tstat_deb_ = tstat_deb_sauv;

                  for (auto &itr : champs_post_complet_)
                    {
                      if (sub_type(Champ_Gen_de_Champs_Gen, itr.valeur()))
                        {
                          Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen, itr.valeur());
                          champ_post.fixer_tstat_deb(tstat_deb_, temps_derniere_mise_a_jour_stats);
                        }
                    }
                }
            }
          else  // lecture pour sauter le bloc
            {
              Motcle tmp;
              is >> tmp >> tmp;
              int n;
              is >> n;
              double tstat_deb_sauv,temps_derniere_mise_a_jour_stats;
              is >> tstat_deb_sauv;
              is >> temps_derniere_mise_a_jour_stats;
              Nom bidon;
              double dbidon;
              DoubleTab tab_bidon;
              for (int i=0; i<n; i++)
                {
                  is >> bidon >> bidon;
                  is >> dbidon;
                  tab_bidon.jump(is);
                }
            }
        }
    }
  return 1;


  ////////////////////////////////////////////////////////////////////
}

void Postraitement::completer()
{
  les_sondes_.init_bords();
}

void Postraitement::completer_sondes()
{
  les_sondes_.completer();
}

/*! @brief Lit le nom des champs a postraiter sur un flot d'entree.
 *
 * Format:
 *     {
 *        ???
 *     }
 *
 * @param (Entree& s) un flot d'entree
 * @return (int) renvoie toujours 1
 * @throws accolade ouvrante attendue
 */
int Postraitement::lire_champs_a_postraiter(Entree& s)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  Motcle motlu2;

  s >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected { to start to read the fields of postprocessing with the simplified syntax" << finl;
      exit();
    }
  s >> motlu;

  Noms liste_noms;
  mon_probleme->get_noms_champs_postraitables(liste_noms);
  //Cerr<<"liste_noms="<<liste_noms<<finl;
  while (motlu != accolade_fermee)
    {
      // GF pour tester le creer_champ
      for (int i=0; i<liste_noms.size()*0; i++)
        {
          Motcle toto(liste_noms[i]);
          mon_probleme->creer_champ(toto);
          Cerr<<" get "<<toto<<" ";
          Cerr<<mon_probleme->get_champ(toto)<<finl;
        }

      Nom expression;
      int is_champ_predefini = probleme().expression_predefini(motlu,expression);
      if ((is_champ_predefini) && (!comprend_champ_post(motlu)))
        {
          Champ_Generique champ;
          Entree_complete s_complete(expression,s);
          s_complete>>champ;
          complete_champ(champ,motlu);
        }

      s>>motlu2;
      if ((motlu2=="elem") || (motlu2=="som") || (motlu2=="faces"))
        {
          //Prise en compte des pb_med
          if (mon_probleme.valeur().que_suis_je()!="Pb_MED")
            creer_champ_post(motlu,motlu2,s);
          else
            creer_champ_post_med(motlu,motlu2,s);

          s>>motlu;
        }
      else
        {

          //On teste la distinction entre un Champ_Generique a creer par macro (sans som specifie)
          //et l ajout d un Champ_Generique deja cree (existant dans champs_post_complet_)
          //On va tester si le motlu correpond au nom d un champ porte par le probleme
          //Si c est le cas on cree un Champ_Generique par macro sinon on va recuperer le Champ_Generique dans la liste complete
          //et on l ajoute dans la liste des champs a postraiter

          Motcles mots_compare(liste_noms.size());
          for (int i=0; i<liste_noms.size(); i++)
            mots_compare[i] = liste_noms[i];

          if (mots_compare.contient_(motlu))
            creer_champ_post(motlu,"som",s);
          else
            {
              if (motlu=="pas_de_temps")
                {
                  int nb_eq = probleme().nombre_d_equations();
                  for (int i=0; i<nb_eq; i++)
                    {
                      int nb_morceaux = probleme().equation(i).nombre_d_operateurs();
                      for (int j=0; j<nb_morceaux; j++)
                        creer_champ_post_moreqn("operateur","stabilite",i,j,-1,s);
                    }
                }
              else if (motlu=="flux_bords" || motlu=="flux_surfacique_bords")
                {
                  int nb_eq = probleme().nombre_d_equations();
                  for (int i=0; i<nb_eq; i++)
                    {
                      int nb_morceaux = probleme().equation(i).nombre_d_operateurs_tot();
                      const Champ_Inc& ch_inco = probleme().equation(i).inconnue();
                      int nb_compo = ch_inco->nb_comp();

                      int compo;
                      for (int j=0; j<nb_morceaux; j++)
                        {
                          if (j==3)
                            nb_compo = 1;

                          for (int comp=0; comp<nb_compo; comp++)
                            {
                              if (nb_compo==1)
                                compo=-1;
                              else
                                compo=comp;
                              creer_champ_post_moreqn("operateur",motlu,i,j,compo,s);
                            }
                        }
                    }
                }
              else
                {
                  const Champ_Generique_base& champ_op = probleme().get_champ_post(motlu);
                  const Noms nom = champ_op.get_property("nom");
                  if (noms_champs_a_post_.contient(nom[0]))
                    {
                      Cerr << "Postraitement::lire_champs_a_postraiter : duplicate field " << nom[0] << finl;
                      exit();
                    }
                  else
                    noms_champs_a_post_.add(nom[0]);
                }
            }

          motlu=motlu2;
        }
    }

  return 1;
}

int Postraitement::lire_champs_stat_a_postraiter(Entree& s)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu,motlu2,motlu3,motlu4,indic_corr;
  //Motcle elem("elem");
  //Motcle som("som");
  indic_corr="non_correlation";

  s >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected { to start to read the data to create the statistical fields" << finl;
      exit();
    }

  Noms liste_noms;
  mon_probleme->get_noms_champs_postraitables(liste_noms);
  //Cerr<<"liste_noms="<<liste_noms<<finl;

  s >> motlu;

  if (motlu == Motcle("t_deb"))
    s >> tstat_deb_;
  else
    {
      Cerr << "Error while reading the statistics in the postprocessing" << finl;
      Cerr << "We expected the keyword t_deb instead of " << motlu << finl;
      exit();
    }
  s >> motlu;
  if (motlu == Motcle("t_fin"))
    s >> tstat_fin_;
  else
    {
      Cerr << "Error while reading the statistics in the postprocessing" << finl;
      Cerr << "We expected the keyword t_fin instead of " << motlu << finl;
      exit();
    }

  s >> motlu;

  while (motlu != accolade_fermee)
    {
      s>>motlu2;
      s>>motlu3;

      if (motlu!="correlation")
        {
          if ((motlu3=="elem") || (motlu3=="som") || (motlu3=="faces"))
            {
              creer_champ_post_stat(motlu2,motlu3,motlu,indic_corr,tstat_deb_,tstat_fin_,s);
              s>>motlu;
            }
          else
            {
              creer_champ_post_stat(motlu2,"som",motlu,indic_corr,tstat_deb_,tstat_fin_,s);
              motlu=motlu3;
            }
        }
      else
        {
          s>>motlu4;
          if ((motlu4=="elem") || (motlu4=="som") || (motlu4=="faces"))
            {
              creer_champ_post_stat(motlu2,motlu4,motlu,motlu3,tstat_deb_,tstat_fin_,s);
              s>>motlu;
            }
          else
            {
              creer_champ_post_stat(motlu2,"som",motlu,motlu3,tstat_deb_,tstat_fin_,s);
              motlu=motlu4;
            }
        }
      nb_champs_stat_ += 1;
    }

  return 1;
}

/*! @brief On recherche les champs statistiques dans les sources du champ courant
 *
 */
int Postraitement::cherche_stat_dans_les_sources(const Champ_Gen_de_Champs_Gen& ch, Motcle nom)
{
  if (sub_type(Champ_Generique_Statistiques_base,ch))
    {
      // Activer pour lancer la sauvegarde et la reprise des statistiques
      stat_demande_definition_champs_ = 1;
      const Champ_Generique_Statistiques_base& champ_stat = ref_cast(Champ_Generique_Statistiques_base,ch);
      if (tstat_deb_>-1 && champ_stat.tstat_deb()!=tstat_deb_)
        {
          Cerr << "Error, the advanced field " << nom << " has a t_deb= " << champ_stat.tstat_deb() << finl;
          Cerr << "Whereas another advanced field has t_deb= " << tstat_deb_ << finl;
          Cerr << "You can't use different t_deb in a same post-processing block for the moment." << finl;
          Cerr << "Tip: add a new post-processing block for your advanced fields with different t_deb value." << finl;
          Process::exit();
        }
      tstat_deb_ = champ_stat.tstat_deb();
      tstat_fin_ = champ_stat.tstat_fin();
    }
  else
    for (int i=0; i<ch.get_nb_sources(); i++)
      {
        const Champ_Generique_base& source = ch.get_source(i);
        if (sub_type(Champ_Gen_de_Champs_Gen,source))
          {
            const Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen,source);
            cherche_stat_dans_les_sources(champ_post,nom);
          }
      }
  return 1;
}

/*! @brief Lit les champs a postraiter sur un flot d'entree.
 *
 */
int Postraitement::lire_champs_operateurs(Entree& s)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  Champ_Generique champ;

  s>>motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected { to start to read the postprocessing fields" << finl;
      exit();
    }
  s>>motlu;

  Nom expression;
  int is_champ_predefini = probleme().expression_predefini(motlu,expression);

  if (is_champ_predefini)
    {
      Cerr<<"The name "<<motlu<<" of a post-processing field of the problem "<<probleme().le_nom()<<finl;
      Cerr<<"is identical to the name of one of this problem pre-defined post-processing fields."<<finl;
      Cerr<<"Please change the name of this post-processing field."<<finl;
      exit();
    }
  while (motlu != accolade_fermee)
    {
      Cerr <<"Reading definition of field "<<motlu<<finl;
      s>>champ;
      complete_champ(champ,motlu);
      s>>motlu;
    }

  return 1;
}

void Postraitement::lire_fichier(const Nom& nom_fichier)
{
  LecFicDiffuse_JDD f(nom_fichier);
  Motcle motlu;
  Champ_Generique champ;

  if (!f.good())
    {
      Cerr << "Cannot open the file " << nom_fichier << finl;
      Process::exit();
    }

  f >> motlu;
  while (!f.eof())
    {
      Cerr <<"Reading definition of field "<<motlu<<finl;
      f >> champ;
      complete_champ(champ,motlu);
      f >> motlu;
    }
  f.close();
}

void Postraitement::complete_champ(Champ_Generique& champ,const Motcle& motlu)
{

  if (sub_type(Champ_Gen_de_Champs_Gen,champ.valeur()))
    {
      const Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen,champ.valeur());
      cherche_stat_dans_les_sources(champ_post,motlu);
      if (sub_type(Champ_Generique_Statistiques_base,champ_post))
        nb_champs_stat_ += 1;
    }
  champ->nommer(motlu);
  //On teste le nom du champ et de ses sources si elles ont ete specifiees par l utilisateur
  //Methode suivante a reviser ou a ne pas utiliser
  verifie_nom_et_sources(champ);

  Champ_Generique& champ_a_completer = champs_post_complet_.add_if_not(champ);
  champ_a_completer->completer(*this);
}

int Postraitement::lire_tableaux_a_postraiter(Entree& s)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  Nom nom_tab;

  s >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected { to start to read the postprocessing data" << finl;
      exit();
    }

  s >> nom_tab;
  motlu = nom_tab;
  while (motlu != accolade_fermee)
    {
      // Recherche du tableau a postraiter
      REF(IntVect) ch_tab;
      Noms liste_noms;
      mon_probleme->get_noms_champs_postraitables(liste_noms);

      ////if (mon_probleme->comprend_mot(motlu))
      //if (liste_noms.contient(motlu))
      for (int i=0; i<liste_noms.size(); i++)
        if (liste_noms[i]==motlu)
          {
            if (mon_probleme->a_pour_IntVect(motlu,ch_tab))
              {
                noms_tableaux_.add(nom_tab);
                tableaux_a_postraiter_.add(ch_tab);;
              }
            else
              {
                Cerr << "The problem do not have the array " << motlu << finl;
                exit();
              }
          }

        else if (i==liste_noms.size()-1)
          {
            Cerr << "The problem does not understand the word " << motlu << finl;
            exit();
          }
      s >> nom_tab;
      motlu = nom_tab;
    }
  return 1;
}

// E Saikali : on ajoute cette liste qui est utile pour un probleme couple
// dans le cas ou on ecrit sur le meme domaine
static Noms liste_dom_ecrit;

/*! @brief Initialise le postraitement.
 *
 * Cree le fichier associe au postraitement, ecrit
 *     des infos sur TrioU. Ecrit le probleme sur
 *     le fichier.
 *
 */
void Postraitement::init()
{
  assert(est_le_premier_postraitement_pour_nom_fich()!=-1);
  assert(est_le_dernier_postraitement_pour_nom_fich()!=-1);
  Schema_Temps_base& sch = mon_probleme->schema_temps();
  double temps_courant = sch.temps_courant();
  double tinit = sch.temps_init();
  Postraitement::formats_supportes=Motcles(5);
  assert(formats_supportes.size()==5);
  if(formats_supportes[0]!="lml")
    {
      formats_supportes[0]="lml";
      formats_supportes[1]="lata";
      formats_supportes[2]="meshtv";
      formats_supportes[3]="med";
      formats_supportes[4]="xyz";
    }

  const Domaine& dom=le_domaine.valeur();
  const Nom& nom_du_domaine = dom.le_nom();
  Nom name=nom_fich().prefix(format);
  name.prefix(".");
  if (besoin_postraiter_champs())
    {
      int reprise = mon_probleme->reprise_effectuee();
      format_post->modify_file_basename(name, reprise && est_le_premier_postraitement_pour_nom_fich_, tinit);
      format_post->ecrire_entete(temps_courant, reprise, est_le_premier_postraitement_pour_nom_fich_);
      format_post->preparer_post(nom_du_domaine, est_le_premier_postraitement_pour_nom_fich_, reprise, tinit);
    }
  ////////////////////////////////////////////////////////////////////////

  // S'il existe un champ a postraiter aux faces, on stocke ici une ref au domaine dis base du champ
  // PQ : 13/06/13 : mis en attribut de la classe pour gerer les champs FACES en maillage deformable

  {
    Nom le_nom_champ_post;
    for (auto& itr : noms_champs_a_post_)
      {
        const Nom& nom_post = itr;
        const REF(Champ_Generique_base)& champ = get_champ_post(nom_post);

        int indic_correlation=0;
        if ((sub_type(Champ_Gen_de_Champs_Gen,champ.valeur())))
          {
            const Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen,champ.valeur());
            if (sub_type(Champ_Generique_Correlation,champ_post.get_source(0)))
              indic_correlation=1;
          }

        if (!indic_correlation)
          {

            Champ espace_stockage;
            const Champ_base& champ_ecriture = champ->get_champ(espace_stockage);
            Entity loc = champ->get_localisation();
            const Nom loc_post = get_nom_localisation(loc);
            const Noms nom = champ->get_property("nom");
            const Noms composantes = champ->get_property("composantes");

            if (Motcle(loc_post) == "FACES" && Motcle(format).debute_par("lata")==0 && Motcle(format).debute_par("med")==0 && Motcle(format).debute_par("xyz")==0)
              {
                Cerr<<"The field "<<nom[0]<<" can not be postprocessed to the faces in the format "<<format<<finl;
                Cerr<<"The postprocessing to the faces is allowed only in the format lata or med"<<finl;
                exit();
              }
            // PL: Ajout automatique du postraitement aux faces pour PolyMAC_P0P1NC seul, sinon doit etre specifie par FACES
            if (Motcle(loc_post) == "FACES" || champ->get_discretisation().is_polymac_family())
              {
                REF(Domaine_dis_base) ref_domaine_dis = champ->get_ref_domaine_dis_base();
                if (ref_domaine_dis.non_nul())
                  domaine_dis_pour_faces = ref_domaine_dis;
              }
            const Nature_du_champ& nature = champ_ecriture.nature_du_champ();
            if (Motcle(nom_post)== Motcle(nom[0]))
              {
                le_nom_champ_post = nom[0];
                /* Sinon le cas Reprise_grossier_fin ne passe pas, en effet on doit pouvoir postraiter K_Eps
                   if (nature==multi_scalaire)
                   {
                   Cerr<<"The field "<<nom[0]<<" is of multi scalar nature."<<finl;
                   Cerr<<"Only the components of this field may be post-processed."<<finl;
                   exit();
                   } */
              }
            else
              {
                const Noms& noms_comp = composantes;
                int n = noms_comp.size();
                for (int i=0; i<n; i++)
                  if (Motcle(nom_post)==Motcle(noms_comp[i]))
                    le_nom_champ_post = noms_comp[i];
              }


            // il n'existe pas d'interpolation aux faces pour les champs qui ne sont pas naturellement localises aux faces
            // test pour gerer la problematique du post-traitement aux faces non pris en compte
            Nom nomposttmp(le_nom_champ_post) ;
            Nom& locpostjdd=nomposttmp.prefix(nom_du_domaine);
            locpostjdd.prefix("_");
            if (locpostjdd.finit_par("FACES") && Motcle(loc_post) != "FACES")
              {
                Cerr<<"The field "<< le_nom_champ_post <<" can not be postprocessed to the faces because this interpolation is not implemented in the code." <<finl;
                Cerr<<"The postprocessing to the faces is allowed only for the field naturally localized at the faces."<<finl;
                exit();
              }

            const int nb_compo = champ_ecriture.nb_comp();
            format_post->completer_post(dom,axi,nature,nb_compo,composantes,loc_post,le_nom_champ_post);

          }
      }
  }

  const Nom token = nom_du_domaine+"_"+nom_fich_; // format est deja dans nom_fich_ (xxx.lml, lata ou med)
  const bool already_written_domain = liste_dom_ecrit.contient_(token);

  ////////////////////////////////////////////////////////////////////
  // If domain is not time dependant, we write it in the ::init() method
  // else we write it at each postraiter_champs() call
  // PL: On ecrit le domaine que si le postraitement des champs est demande
  if(!dom.deformable() && besoin_postraiter_champs() && !already_written_domain)
    {
      liste_dom_ecrit.add(token); // on ajoute dans la liste !
      format_post->ecrire_domaine_dis(dom,domaine_dis_pour_faces,est_le_premier_postraitement_pour_nom_fich_);
      // domaine_dis_pour_faces non_nul() si on demande un postraitement d'un champ aux faces:
      if (domaine_dis_pour_faces.non_nul() && Motcle(format) != "LML")
        {
          const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis_pour_faces.valeur());
          const IntTab& faces_sommets = domaine_vf.face_sommets();
          const int nb_sommets = dom.nb_som();
          const int nb_faces = faces_sommets.dimension(0);
          format_post->ecrire_item_int("FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.le_nom(),
                                       "FACES", /* localisation */
                                       "SOMMETS", /* reference */
                                       faces_sommets,
                                       nb_sommets);

          const IntTab& elem_faces = domaine_vf.elem_faces();
          format_post->ecrire_item_int("ELEM_FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.le_nom(),
                                       "ELEMENTS", /* localisation */
                                       "FACES", /* reference */
                                       elem_faces,
                                       nb_faces);

        }
    }

  dernier_temps=mon_probleme->schema_temps().temps_init();
  format_post->modify_file_basename(name,0,tinit);
}


/*! @brief Finalise le postraitement Ferme le fichier associe.
 *
 */
void Postraitement::finir()
{
  // Fermeture du fichier si le postraitement des champs etait demande
  if (est_le_dernier_postraitement_pour_nom_fich_ && besoin_postraiter_champs())
    {
      format_post->finir(est_le_dernier_postraitement_pour_nom_fich_);
      Nom name=nom_fich().prefix(format);
      name.prefix(".");
      format_post->modify_file_basename(name,mon_probleme->reprise_effectuee(),-1);
    }
}

/*! @brief Effectue le postraitement des Champs de facon imperative.
 *
 * Mets a jour les champs crees.
 *
 * @return (int) renvoie toujours 1
 */
int Postraitement::postraiter_champs()
{
  double temps_courant = mon_probleme->schema_temps().temps_courant();
  const Domaine& dom=le_domaine.valeur();

  if (temps_ < temps_courant)
    {
      if (est_le_premier_postraitement_pour_nom_fich_)
        format_post->ecrire_temps(temps_courant);
    }
  // We write the time dependant domain here. PL: we write only if fields list is not empty
  if (dom.deformable() && besoin_postraiter_champs())
    {
      format_post->ecrire_domaine_dis(dom,domaine_dis_pour_faces,est_le_premier_postraitement_pour_nom_fich_);

      if (domaine_dis_pour_faces.non_nul())
        {
          const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis_pour_faces.valeur());
          const IntTab& faces_sommets = domaine_vf.face_sommets();
          const int nb_sommets = dom.nb_som();
          const int nb_faces = faces_sommets.dimension(0);
          format_post->ecrire_item_int("FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.le_nom(),
                                       "FACES", /* localisation */
                                       "SOMMETS", /* reference */
                                       faces_sommets,
                                       nb_sommets);

          const IntTab& elem_faces = domaine_vf.elem_faces();
          format_post->ecrire_item_int("ELEM_FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.le_nom(),
                                       "ELEMENTS", /* localisation */
                                       "FACES", /* reference */
                                       elem_faces,
                                       nb_faces);

        }
    }

  write_extra_mesh(); // For FT for example, this will write INTERFACES.

  format_post->init_ecriture(temps_courant,temps_,est_le_premier_postraitement_pour_nom_fich_,dom);

  if (temps_ < temps_courant)
    temps_=temps_courant;

  postprocess_field_values();

  format_post->finir_ecriture(temps_courant);
  return 1;
}

void Postraitement::postprocess_field_values()
{
  double temps_courant = mon_probleme->schema_temps().temps_courant();
  const Domaine& dom=le_domaine.valeur();

  for (auto& itr : noms_champs_a_post_)
    {
      const Champ_Generique_base& champ = get_champ_post(itr);

      //Etape de calcul
      //Le champ cree est rendu dans champ_ecriture

      Champ espace_stockage;
      const Champ_base& champ_ecriture = champ.get_champ(espace_stockage);
      DoubleTab val_vec;
      bool isChamp_Face_PolyMAC = (champ_ecriture.que_suis_je().debute_par("Champ_Face_PolyMAC") || champ_ecriture.que_suis_je().debute_par("Champ_Fonc_Face_PolyMAC"));
      if (isChamp_Face_PolyMAC)
        champ_ecriture.valeur_aux_faces(val_vec);
      const DoubleTab& valeurs_post = isChamp_Face_PolyMAC ? val_vec : champ_ecriture.valeurs();
      //Etape de recuperation des informations specifiques au champ a postraiter

      Entity loc = champ.get_localisation();
      const Nom localisation = get_nom_localisation(loc);
      const Noms nom_post = champ.get_property("nom");
      const Noms& unites = champ.get_property("unites");
      const Noms& noms_compo = champ.get_property("composantes");
      int tenseur = champ.get_info_type_post();

      //Etape pour savoir si on doit postraiter un champ ou une de ses composantes

      int ncomp = Champ_Generique_base::composante(itr,nom_post[0],noms_compo,champ.get_property("synonyms"));

      //La distinction du type de postraitement (tableau ou tenseur) est fait dans la methode postraiter par la valeur de tenseur
      Nom nature("scalar");
      if (champ_ecriture.nature_du_champ()==vectoriel) nature="vector";
      postraiter(dom,unites,noms_compo,ncomp,temps_courant,itr,localisation,nature,valeurs_post,tenseur);
    }
}

int Postraitement::postraiter_tableaux()
{
  double temps_courant = mon_probleme->schema_temps().temps_courant();
  const Domaine& dom=le_domaine.valeur();

  //Methode ecrire_item_int codee uniquement pour les formats lml et lata
  //Sans doute pas testee par les cas de non regression
  //Signature a revoir (supprimer id_domaine)

  //Si codage de la methode pour meshtv, ajouter init_ecriture() et finir_ecriture()

  if (temps_ < temps_courant)
    {
      temps_=temps_courant;
      if (est_le_premier_postraitement_pour_nom_fich_)
        format_post->ecrire_temps(temps_courant);
    }

  auto& list1 = tableaux_a_postraiter_.get_stl_list();
  auto& list2 = noms_tableaux_.get_stl_list();

  auto itr2 = list2.begin();

  for (auto& itr1 : list1)
    {
      const Nom& id_item = *itr2;
      const Nom& id_du_domaine = dom.le_nom();
      const Nom& id_domaine = dom.le_nom();
      const Nom localisation="";
      const Nom reference="";
      const IntVect& val = itr1.valeur();
      const int ref_size =0;

      format_post->ecrire_item_int(id_item,id_du_domaine,id_domaine,localisation,reference,val,ref_size);

      ++itr2;
    }


  return 1;
}

/*! @brief Effectue le postraitement des Champs si cela est necessaire.
 *
 * @return (int) renvoie toujours 1
 */
int Postraitement::traiter_champs()
{
  Schema_Temps_base& sch = mon_probleme->schema_temps();
  double temps_courant = sch.temps_courant();
  int nb_pas_dt = sch.nb_pas_dt();
  int ind_pas_dt_post = ind_post(nb_pas_dt);

  if (lpost_champ(temps_courant) || lpost_stat(temps_courant) || ind_pas_dt_post)
    {
      postraiter_champs();
      if (!ind_pas_dt_post)
        {
          fdt_post.setVar("t",temps_courant);
          dt_post_ch_ = fdt_post.eval();
          dt_post_stat_ = dt_post_ch_;
        }
    }

  return 1;
}

int Postraitement::traiter_tableaux()
{
  Schema_Temps_base& sch = probleme().schema_temps();
  double temps_courant = sch.temps_courant();
  if ( lpost_tab(temps_courant) )
    postraiter_tableaux();
  return 1;
}


/*! @brief Effectue le postraitement lie au sondes de facon imperative.
 *
 * @return (int) renvoie toujours 1
 */
int Postraitement::postraiter_sondes()
{
  les_sondes_.postraiter();
  les_sondes_int_.postraiter(mon_probleme->schema_temps().temps_courant());
  return 1;
}


/*! @brief Mets a jour (en temps) le sondes.
 *
 * @return (int) renvoie toujours 1
 */
int Postraitement::traiter_sondes()
{
  double temps=mon_probleme->schema_temps().temps_courant();
  double tinit=temps-mon_probleme->schema_temps().temps_calcul();
  les_sondes_.mettre_a_jour(temps,tinit);
  return 1;
}

int Postraitement::postraiter(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int ncomp,
                              const double temps,
                              Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs,int tenseur)

{

  if (!tenseur)
    postraiter_tableau(dom,unites,noms_compo,ncomp,temps,nom_post,localisation,nature,valeurs);
  else
    postraiter_tenseur(dom,unites,noms_compo,ncomp,temps,nom_post,localisation,nature,valeurs);

  return 1;
}

int Postraitement::postraiter_tableau(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int ncomp,
                                      const double temps,
                                      Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs)
{
  const Nom& id_du_domaine = dom.le_nom();
  const Nom& id_champ_ecrit = nom_post;

  const int dim0 = valeurs.dimension(0);
  const int N = valeurs.line_size();
  DoubleTab valeurs_tmp(dim0, ncomp == -1 ? N : 1);

  for (int i = 0; i < dim0; i++)
    if (ncomp == -1)
      for (int j = 0; j < N; j++)
        valeurs_tmp(i, j) = valeurs(i, j);
    else
      valeurs_tmp(i, 0) = valeurs(i, ncomp);

  const DoubleTab& val_post_ecrit = valeurs_tmp;
  if (Motcle(format)=="XYZ")
    {
      if (localisation == "SOM")
        {
          const DoubleTab& coord = dom.coord_sommets();
          format_post->ecrire_champ2(dom,unites,noms_compo,ncomp,temps,id_champ_ecrit,id_du_domaine,localisation,nature,val_post_ecrit,coord);
        }
      else if (localisation == "ELEM")
        {
          DoubleTab coord;
          dom.calculer_centres_gravite(coord);
          format_post->ecrire_champ2(dom,unites,noms_compo,ncomp,temps,id_champ_ecrit,id_du_domaine,localisation,nature,val_post_ecrit,coord);
        }
      else if (localisation == "FACES")
        {
          const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis_pour_faces.valeur());
          const DoubleTab& coord = domaine_vf.xv();
          format_post->ecrire_champ2(dom,unites,noms_compo,ncomp,temps,id_champ_ecrit,id_du_domaine,localisation,nature,val_post_ecrit,coord);
        }
    }
  else
    format_post->ecrire_champ(dom,unites,noms_compo,ncomp,temps,id_champ_ecrit,id_du_domaine,localisation,nature,val_post_ecrit);
  return 1;
}

int Postraitement::postraiter_tenseur(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int ncomp,
                                      const double temps,
                                      Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs)
{

  int nb_comp = noms_compo.size();
  for (int comp=0; comp<nb_comp; comp++)
    postraiter_tableau(dom,unites,noms_compo,comp,temps,noms_compo[comp],localisation,nature,valeurs);
  return 1;
}

Nom Postraitement::set_expression_champ(const Motcle& motlu1,const Motcle& motlu2,
                                        const Motcle& motlu3,const Motcle& motlu4,
                                        const int trouve)
{
  Nom ajout("");
  if (((motlu2!="natif") && (motlu2!="faces") && (motlu3==""))
      || ((motlu2!="natif") && (motlu3!="")) )
    {

      ajout = "Interpolation { localisation ";
      ajout += motlu2;

      if (motlu3=="Moyenne")
        ajout += " source Moyenne { ";
      else if (motlu3=="Ecart_type")
        ajout += " source Ecart_Type { ";
      else if (motlu3=="Correlation")
        ajout += " source Correlation { ";

      if (trouve==1)
        {
          ajout += " sources_reference { ";
          ajout += motlu1;
          ajout += " } }";
        }
      else
        {
          if (motlu3!="Correlation")
            ajout += " source ";
          else
            ajout += " sources { ";
          ajout += " refChamp { Pb_champ ";
          ajout += probleme().le_nom();
          ajout += " ";
          ajout += motlu1;
          ajout += " } ";

          if (motlu3!="Correlation")
            ajout += " } ";
          else
            {
              ajout += " , ";
              ajout += " refChamp { Pb_champ ";
              ajout += probleme().le_nom();
              ajout += " ";
              ajout += motlu4;
              ajout += " } ";
              ajout += " } } ";
            }

          if ((motlu3=="Moyenne") || (motlu3=="Ecart_type") || (motlu3=="Correlation"))
            ajout += " } ";
        }

    }
  else
    {

      if(!trouve)
        {
          ajout += " refChamp { Pb_champ ";
          ajout += probleme().le_nom();
          ajout += " ";
          ajout += motlu1;
          ajout += " }";
        }
    }

  return ajout;
}

//Creation d un champ generique en fonction des parametres qui sont passes a la methode
//Cette methode a pour objectif de pouvoir utiliser l ancienne syntaxe dans le jeu de donnees
//pour lancer la requete d un champ a postraiter (ex : vitesse elem) mais aussi generer un
//champ generique pour creer une sonde d 'un champ discret du probleme
//
//Les etapes sont les suivantes :
// -creation d une chaine de caracteres pour indiquer le type de champ generique a creer
// -lecture du champ generique
// -association du domaine au champ et creation d un domaine discretise si le domaine n est pas celui du calcul
// -le champ est nomme
// -les sources sont nommees (sauf si creation d un Champ_Generique_refChamp)
//  et les composantes sont fixees (permet de tester la non regression avec format lml)
// -le champ est ajoute a la liste champs_post_complet_
// -l identifiant du champ est ajoute a la liste noms_champs_a_post_
//
//-motlu2 = "natif" : creation d un Champ_Generique_refChamp (utilise pour les sondes)
//       refChamp { Pb_champ "nom_du_pb" "nom_du_champ" }
//  (nom_du_champ=motlu1)

//-sinon creation d un Champ_Generique_Interpolation
//       Interpolation { localisation "loc"
//                        source refChamp { Pb_champ "nom_du_pb" "nom_du_champ" }
//         }
//  (loc=motlu2 nom_du_champ=motlu1)

void Postraitement::creer_champ_post(const Motcle& motlu1,const Motcle& motlu2,Entree& s)
{
  Champ_Generique champ;
  Nom ajout;
  Nom nom_champ, nom_champ_a_post;

  //Le postraitement aux faces concerne actuellement les champs dont la discretisation "natif" est aux faces
  //On construit dans ce cas la un Champ_Generique_refChamp

  // on essaye avant dans les champs_posts...
  int trouve=comprend_champ_post(motlu1);

  if ((trouve) && ((motlu2=="natif") || (motlu2=="faces")))
    {
      ajout="";
      noms_champs_a_post_.add_if_not(motlu1);
      return;
    }

  ajout = set_expression_champ(motlu1,motlu2,"","",trouve);

  //A decommenter si choix fait de desactiver les champs discrets correspondants
  //Meme procedure a faire pour creer_champ_post_stat()
  //En fait seul gradient_temperature n est cree que pour le postraitement
  /*
  //////////////////////////////////////////////////////////////////////////////////////////////////
  if ((motlu1=="divergence_U") || (motlu1=="gradient_pression") || (motlu1=="gradient_temperature"))
  {
  Motcle motlu1_corrige;
  if (motlu1=="gradient_pression") motlu1_corrige="pression";
  if (motlu1=="gradient_temperature") motlu1_corrige="temperature";
  if (motlu1=="divergence_U") motlu1_corrige="vitesse";

  if ((motlu1=="gradient_pression") || (motlu1=="gradient_temperature")) {

  ajout = "Interpolation { localisation ";
  ajout +=   motlu2;
  ajout += " source Gradient { source refChamp { Pb_champ ";
  ajout += mon_probleme->le_nom();
  ajout += " ";
  ajout += motlu1_corrige;
  ajout += " } } }";
  }

  if (motlu1=="divergence_U") {
  ajout = "Interpolation { localisation ";
  ajout +=   motlu2;
  ajout += " source Divergence { source refChamp { Pb_champ ";
  ajout += mon_probleme->le_nom();
  ajout += " ";
  ajout += motlu1_corrige;
  ajout += " } } }";
  }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  */

  Entree_complete s_complete(ajout,s);
  s_complete>>champ;

  //if ((le_domaine.valeur().le_nom()!=mon_probleme.valeur().domaine().le_nom()) && ((motlu2!="natif"))) {
  {

    if ((motlu2!="natif")&&(motlu2!="faces"))
      {
        Champ_Generique_Interpolation& champ_interp = ref_cast(Champ_Generique_Interpolation,champ.valeur());
        champ_interp.set_domaine(le_domaine.valeur().le_nom());
        // champ_interp.discretiser_domaine(*this);
      }
  }

  Nom nom_champ_ref;
  //A decommenter si suppression champs discrets correspondants
  /*
    if ((motlu1=="divergence_U") || (motlu1=="gradient_pression") || (motlu1=="gradient_temperature"))
    {
    nom_champ_ref=motlu1;
    }
    else {
  */
  Noms composantes;
  Noms source_compos,source_syno;
  if (trouve==0)
    {
      REF(Champ_base) champ_ref;
      champ_ref = mon_probleme->get_champ(motlu1);
      nom_champ_ref = champ_ref->le_nom();
      source_compos = champ_ref->noms_compo();
      composantes = champ_ref->noms_compo();
      source_syno = champ_ref->get_synonyms();
      if ((source_compos.size()==1) && (source_compos[0]==motlu1))
        {
          nom_champ_ref=motlu1;
        }
      for (int i=0; i<source_syno.size(); i++)
        if (source_syno[i]==motlu1)
          nom_champ_ref=motlu1;
    }
  else
    {
      REF(Champ_Generique_base) champ_ref;
      champ_ref=get_champ_post(motlu1);
      nom_champ_ref = champ_ref->get_nom_post();
      source_compos = champ_ref->get_property("composantes");
      source_syno = champ_ref->get_property("synonyms");
      composantes =source_compos;
    }


  nom_champ = Motcle(nom_champ_ref)+"_"+motlu2+"_"+le_domaine.valeur().le_nom();
  champ->nommer(nom_champ);

  //On nomme la source d un Champ_Generique_Interpolation cree par macro
  if ((motlu2!="natif") && (motlu2!="faces"))
    {

      Champ_Generique_Interpolation& champ_post = ref_cast(Champ_Generique_Interpolation,champ.valeur());
      champ_post.nommer_sources(*this);

      //On fixe l attribut compo_ pour le Champ_Generique_Interpolation cree par cette macro

      //A decommenter si suppression des champs discrets
      /*
        if ((motlu1=="divergence_U") || (motlu1=="gradient_pression") || (motlu1=="gradient_temperature"))
        ////Noms source_compos = champ_ref->noms_compo();
        {
        if ((motlu1=="gradient_pression") || (motlu1=="gradient_temperature"))
        {
        source_compos.dimensionner(dimension);
        source_compos[0]=nom_champ_ref+"X";
        source_compos[1]=nom_champ_ref+"Y";
        if (dimension>2)
        source_compos[2]=nom_champ_ref+"Z";
        }
        }
        else
      */

      const Nom& nom_dom = champ_post.get_ref_domain().le_nom();
      int nb_comp = source_compos.size();
      Noms compo(nb_comp);
      Nom loc;
      if (motlu2=="ELEM")
        loc = "elem";
      else if (motlu2=="SOM")
        loc = "som";

      for (int i=0; i<nb_comp; i++)
        compo[i] = source_compos[i] +"_"+loc+"_"+nom_dom;
      champ_post.fixer_noms_compo(compo);
      {
        Noms les_synonyms(source_syno.size());
        for (int i=0; i<source_syno.size(); i++)
          les_synonyms[i] = source_syno[i] +"_"+loc+"_"+nom_dom;
        champ_post.fixer_noms_synonyms(les_synonyms);
      }
    }

  Champ_Generique& champ_a_completer = champs_post_complet_.add_if_not(champ);
  champ_a_completer->completer(*this);

  if (motlu2!="natif")
    {
      if (motlu2!="faces")
        nom_champ_a_post = motlu1+"_"+motlu2+"_"+le_domaine.valeur().le_nom();
      else
        //Dans le cas d une localisation aux faces on test s il s agit d une composante ou du champ
        {
          int ncomp = Champ_Generique_base::composante(motlu1,nom_champ_ref,composantes,source_syno);
          if (ncomp==-1)
            nom_champ_a_post = motlu1+"_"+motlu2+"_"+le_domaine.valeur().le_nom();
          else
            {
              //On determine le numero de composante
              Nom nume(ncomp);
              nom_champ_a_post = nom_champ_ref+"_"+motlu2+"_"+le_domaine.valeur().le_nom()+nume;
            }
        }
      if (noms_champs_a_post_.contient(nom_champ_a_post))
        {
          Cerr << "Postraitement::lire_champs_a_postraiter : duplicate field " << nom_champ_a_post << finl;
          Cerr<<" existing names "<<noms_champs_a_post_<<finl;
          exit();
        }
      else
        noms_champs_a_post_.add(nom_champ_a_post);
    }
}

//Creation d un champ generique en fonction des parametres qui sont passes a la methode
//Cette methode a pour objectif de pouvoir utiliser l ancienne syntaxe dans le jeu de donnees
//pour lancer la requete de statistiques (ex : Moyenne vitesse elem)

//Les etapes sont les suivantes :
// -creation d une chaine de caracteres pour indiquer le type de champ generique a creer
// -lecture du champ generique
// -association du domaine au champ et creation d un domaine discretise si le domaine n est pas celui du calcul
// -le champ est nomme
// -les composantes sont fixees (permet de tester la non regression avec format lml)
// -le champ est complete
// -le champ est ajoute a la liste champs_post_complet_
// -l identifiant du champ est ajoute a la liste noms_champs_a_post_


void Postraitement::creer_champ_post_stat(const Motcle& motlu1,const Motcle& motlu2,const Motcle& motlu3,const Motcle&
                                          motlu4,const double t_deb,const double t_fin,Entree& s)
{
  Champ_Generique champ;
  Nom ajout;
  Nom nom_champ;

  if (((motlu3=="Moyenne") || (motlu3=="Ecart_type")) || (motlu3=="Correlation"))
    {
      ajout = set_expression_champ(motlu1,motlu2,motlu3,motlu4,0);
      Entree_complete s_complete(ajout,s);
      s_complete>>champ;

      //if (le_domaine.valeur().le_nom()!=mon_probleme.valeur().domaine().le_nom()) {
      Champ_Generique_Interpolation& champ_interp = ref_cast(Champ_Generique_Interpolation,champ.valeur());
      champ_interp.set_domaine(le_domaine.valeur().le_nom());
      // champ_interp.discretiser_domaine(*this);
      //}

      if (motlu3!="Correlation")
        nom_champ = motlu3+"_"+motlu1+"_"+motlu2+"_"+le_domaine.valeur().le_nom();
      else
        nom_champ = motlu3+"_"+motlu1+"_"+motlu4+"_"+motlu2+"_"+le_domaine.valeur().le_nom();

      champ->nommer(nom_champ);

      //On nomme les sources du Champ_Generique_Interpolation cree par macro
      Champ_Generique_Interpolation& champ_post = ref_cast(Champ_Generique_Interpolation,champ.valeur());
      champ_post.nommer_sources(*this);

      //On fixe l attribut compo_ pour le Champ_Generique_Interpolation cree par cette macro
      Champ_Generique_Statistiques_base& champ_stat = ref_cast(Champ_Generique_Statistiques_base,champ_post.set_source(0));
      const Champ_Generique_refChamp& champ_ref = ref_cast(Champ_Generique_refChamp,champ_stat.get_source(0));

      Champ espace_stockage;
      const Champ_base& champ_discret = champ_ref.get_champ(espace_stockage);
      const Noms& source_compos = champ_discret.noms_compo();
      const Nom& nom_dom = champ_post.get_ref_domain().le_nom();
      int nb_comp = source_compos.size();
      Noms compo(nb_comp);
      Nom loc;
      if (motlu2=="ELEM")
        loc = "elem";
      else if (motlu2=="SOM")
        loc = "som";
      else if (motlu2=="FACES")
        loc = "faces";

      for (int i=0; i<nb_comp; i++)
        {
          if (motlu3=="Moyenne")
            {
              compo[i] = "Moyenne_";
              compo[i] += source_compos[i] +"_"+loc+"_"+nom_dom;
            }
          else if (motlu3=="Ecart_Type")
            {
              compo[i] = "Ecart_Type_";
              compo[i] += source_compos[i] +"_"+loc+"_"+nom_dom;
            }
        }

      champ_post.fixer_noms_compo(compo);
      champ_stat.fixer_tdeb_tfin(t_deb,t_fin);
      Champ_Generique& champ_a_completer = champs_post_complet_.add_if_not(champ);
      champ_a_completer->completer(*this);

      //On fixe l attribut compo_ pour le Champ_Generique_Interpolation d une Correlation cree par cette macro
      /////////////////////////////////////////////////////////////
      if (motlu3=="Correlation")
        {
          Champ_Generique_Interpolation& champ_post_corr = ref_cast(Champ_Generique_Interpolation,champ_a_completer.valeur());
          const Champ_Generique_Correlation& champ_corr = ref_cast(Champ_Generique_Correlation,champ_post_corr.get_source(0));

          const Noms& source_compos_corr = champ_corr.integrale()->noms_compo();
          // const Nom& nom_dom = champ_post.get_ref_domain().le_nom();
          int nb_comp_corr = source_compos_corr.size();
          Noms compo_corr(nb_comp_corr);
          Nom loc_corr;
          if (motlu2=="ELEM")
            loc_corr = "elem";
          else if (motlu2=="SOM")
            loc_corr = "som";
          else if (motlu2=="FACES")
            loc_corr = "faces";

          for (int i=0; i<nb_comp_corr; i++)
            {
              compo_corr[i] = source_compos_corr[i] +"_"+loc_corr+"_"+nom_dom;

            }
          champ_post_corr.fixer_noms_compo(compo_corr);

        }

      //////////////////////////////////////////////////////////////


      const Noms nom = champ_a_completer->get_property("nom");
      if (noms_champs_a_post_.contient(nom[0]))
        {
          Cerr << "Postraitement::lire_champs_a_postraiter : duplicate field " << nom[0] << finl;
          exit();
        }
      else
        noms_champs_a_post_.add(nom[0]);

    }

  else
    {
      Cerr<<"Verify the syntax of the wanted statistical fields for the postprocessing"<<finl;
      exit();
    }

}

//Creation d un champ generique en fonction des parametres qui sont passes a la methode
//Cette methode a pour objectif de creer un Champ_Generique de type Champ_Generique_Morceau_Equation
//Les etapes sont les suivantes :
// -creation d une chaine de caracteres pour indiquer le type de champ generique a creer
// -lecture du champ generique
// -le champ est nomme
// -le champ est ajoute a la liste champs_post_complet_ et completer
// -les sources son nommees
// -l identifiant du champ est ajoute a la liste noms_champs_a_post_
// -type de champ cree :
//        Morceau_Equation { type "type_moreqn" numero "numero_moreqn" option "type_option" [ compo "num_compo" ]
//                                           source Champ_Post_refChamp { Pb_champ "nom_du_pb" "nom_du_champ" } }
//
// type disponible   : operateur
// numero             : 0 (diffusion) 1 (convection) [ 2 (gradient) 3 (divergence) pour le cas des "flux_bords" ]
// option disponible : stabilite flux_bords
//
void Postraitement::creer_champ_post_moreqn(const Motcle& type,const Motcle& option,const int num_eq,const int num_morceau,const int compo,Entree& s)
{
  Champ_Generique champ;
  Nom ajout("");
  Nom nom_champ, nom_champ_a_post;
  const Champ_Inc& ch_inco = mon_probleme->equation(num_eq).inconnue();
  const Motcle& nom_inco = ch_inco->le_nom();
  Nom nume(num_morceau);

  ajout = "Morceau_Equation { type ";
  ajout += type;
  ajout += " numero ";
  ajout += nume;
  ajout += " option ";
  ajout += option;
  if (compo!=-1)
    {
      Nom numc(compo);
      ajout +=" compo ";
      ajout += numc;
    }
  ajout += " source refChamp { Pb_champ ";
  ajout += mon_probleme->le_nom();
  ajout += " ";
  ajout += nom_inco;
  ajout += " } }";

  Entree_complete s_complete(ajout,s);
  s_complete>>champ;

  const Motcle& nom_eq = mon_probleme->equation(num_eq).le_nom();

  Motcle type_morceau;
  //Motcle type_option;
  if (type=="operateur")
    {
      if (num_morceau==0)
        type_morceau = "diffusion";
      else if (num_morceau==1)
        type_morceau = "convection";
      else if (num_morceau==2)
        type_morceau = "gradient";
      else if (num_morceau==3)
        type_morceau = "divergence";
      else
        {
          Cerr<<"Only two numbers of operators are available."<<finl;
          Cerr<<"0 for the diffusive operator and 1 for the convective operator."<<finl;
          exit();
        }
    }
  else
    {
      Cerr<<"Currently the only pieces of equations considered for the postprocessing"<<finl;
      Cerr<<"are of type operator"<<finl;
      exit();
    }

  if (option=="stabilite")
    {
      nom_champ = nom_eq;
      nom_champ += "_";
      nom_champ += type_morceau;
      nom_champ += "_";
      nom_champ += "dt";
    }
  else if (option=="flux_bords" || option=="flux_surfacique_bords")
    {
      int nb_op = mon_probleme->equation(num_eq).nombre_d_operateurs();
      if (num_morceau >= nb_op)
        nom_champ = mon_probleme->equation(num_eq).operateur_fonctionnel(num_morceau-nb_op).l_op_base().fichier();
      else
        nom_champ = mon_probleme->equation(num_eq).operateur(num_morceau).l_op_base().fichier();
      if (compo!=-1)
        {
          Nom nume2(compo);
          nom_champ += nume2;
        }
    }
  champ->nommer(nom_champ);

  Champ_Generique& champ_a_completer = champs_post_complet_.add_if_not(champ);
  champ_a_completer->completer(*this);
  Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen,champ.valeur());
  champ_post.nommer_sources(*this);

  nom_champ_a_post = nom_champ;
  if (noms_champs_a_post_.contient(nom_champ_a_post))
    {
      Cerr << "Postraitement::lire_champs_a_postraiter : duplicate field " << nom_champ_a_post << finl;
      Cerr<<" existing names "<<noms_champs_a_post_<<finl;
      exit();
    }
  else
    noms_champs_a_post_.add(nom_champ_a_post);
}

//Creation d un champ generique en fonction des parametres qui sont passes a la methode
//Cette methode a pour objectif de pouvoir utiliser l ancienne syntaxe dans le jeu de donnees
//pour les champs d un Pb_MED
//
//Les etapes sont les suivantes :
// -typage du champ generique en Champ_Generique_refChamp
// -association de la reference au postraitement (specifique au cas d un Pb_MED)
// -creation du Champ_Fonc_Med
// -initialisation de la reference au Champ_Fonc_MED et initialisation de la localisation
// -le champ est nomme (identifiant+nom_domaine)
// -les composantes sont fixees (permet de tester la non regression avec format lml)
// -le champ generique est complete
// -le champ est ajoute a la liste champs_post_complet_
// -l identifiant du champ est ajoute a la liste noms_champs_a_post_

void Postraitement::creer_champ_post_med(const Motcle& motlu1,const Motcle& motlu2,Entree& s)
{
  creer_champ_post(motlu1,motlu2,s);
  Motcle test_loc(motlu1);
  Motcle loc2("_");
  loc2+=motlu2;
  Motcle test_loc_bis(test_loc);
  test_loc_bis.prefix(loc2);
  if (test_loc_bis!=test_loc)
    {
      Cerr<< "we must change the name "<< motlu1;
      //Cerr<<test_loc_bis<< " "<<test_loc<<finl;
      int ss=champs_post_complet_.size();
      Nom ancien=champs_post_complet_(ss-1).valeur().get_nom_post();
      ancien.suffix(test_loc);
      //Cerr<<ancien<<" ";
      Nom Nouveau(test_loc_bis);
      Nouveau+=ancien;
      Cerr<<" to " <<Nouveau<<finl;
      champs_post_complet_(ss-1).valeur().nommer(Nouveau);
      noms_champs_a_post_(noms_champs_a_post_.size()-1)=Nouveau;
    }
  //int ss=champs_post_complet_.size();
  //Cerr<<champs_post_complet_(ss-1).valeur().get_nom_post()<<finl;
  //ss=noms_champs_a_post_.size();
  //Cerr<<noms_champs_a_post_(ss-1)<<finl; //exit();
}

int Postraitement::comprend_champ_post(const Motcle& identifiant) const
{
  for (const auto &itr : champs_post_complet_)
    if (itr->comprend_champ_post(identifiant))
      return 1;

  return 0;
}

const Champ_Generique_base& Postraitement::get_champ_post(const Motcle& nom) const
{
  for (const auto& itr : champs_post_complet_)
    {
      try
        {
          return itr->get_champ_post(nom);
        }
      catch (Champs_compris_erreur&)
        {
        }
    }
  Cerr<<"Field " <<nom<<" not found."<< finl;
  throw Champs_compris_erreur();
}

//Decider de ce que doit faire exactement cette methode ou la supprimer
void Postraitement::verifie_nom_et_sources(const Champ_Generique& champ)
{
  Noms liste_noms;
  const Probleme_base& Pb = probleme();
  Pb.get_noms_champs_postraitables(liste_noms);

  Motcles mots_compare(liste_noms.size());
  for (int i=0; i<liste_noms.size(); i++)
    mots_compare[i] = liste_noms[i];
  const Noms nom = champ->get_property("nom");
  const Motcle& nom_champ = nom[0];

  if (mots_compare.contient_(nom_champ))
    {
      Cerr<<"The name "<<nom_champ<<" of a post-processing field of the problem "<<Pb.le_nom()<<finl;
      Cerr<<"is identical to the name of one of this problem discrete fields."<<finl;
      Cerr<<"Please change the name of this post-processing field."<<finl;
      exit();
    }

  if (Pb.comprend_champ_post(nom_champ))
    {
      Cerr<<"The name "<<nom_champ<<" of a post-processing field of the problem "<<Pb.le_nom()<<finl;
      Cerr<<"is identical to the name of another post-processing field, to one of his components or one of his sources."<<finl;
      Cerr<<"Please change the name of this post-processing field."<<finl;
      exit();
    }
}
