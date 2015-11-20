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
// File:        Postraitement.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/133
//
//////////////////////////////////////////////////////////////////////////////

#include <Postraitement.h>
#include <EFichier.h>
#include <Zone_VF.h>
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


Implemente_instanciable_sans_constructeur_ni_destructeur(Postraitement,"Postraitement",Postraitement_base);

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
      if ((champs_demande_) || (stat_demande_) || (stat_demande_definition_champs_))   postraiter_champs();
      if (tableaux_demande_) postraiter_tableaux();
    }
  else
    {
      if (sondes_demande_)   traiter_sondes();
      if ((champs_demande_) || (stat_demande_) || (stat_demande_definition_champs_))   traiter_champs();
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
          LIST_CURSEUR(Champ_Generique) curseur2 = champs_post_complet_ ;
          while(curseur2)
            {
              if (sub_type(Champ_Gen_de_Champs_Gen,curseur2.valeur().valeur()))
                {
                  Champ_Gen_de_Champs_Gen& champ_post = ref_cast_non_const(Champ_Gen_de_Champs_Gen,curseur2.valeur().valeur());
                  champ_post.fixer_serie(tstat_deb_,tstat_fin_);
                }
              ++curseur2;
            }
        }
    }
}

void Postraitement::mettre_a_jour(double temps)
{
  //Mise a jour des operateurs statistiques portes par les Champ_Generique_Statistiques
  LIST_CURSEUR(Champ_Generique) curseur2 = champs_post_complet_ ;
  while(curseur2)
    {
      curseur2.valeur()->mettre_a_jour(temps);
      ++curseur2;
    }

  if ( inf_ou_egal(tstat_deb_,temps) &&  inf_ou_egal(temps,tstat_fin_) )
    tstat_dernier_calcul_ =  temps;// Il y a eu mise a jour effective des integrales
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
  sprintf(s,"%.10f", op->get_time());
  nom_fichier+=Nom(s);
}

inline void nom_fichier(const Postraitement& post, const Nom& nom_post, const Nom& loc, const double& temps_ch, const Domaine& dom, Nom& nom_fichier)
{
  nom_fichier+=".";
  nom_fichier+=nom_post+"."+loc+"."+dom.le_nom()+"."+post.probleme().le_nom()+".";
  char s[100];
  sprintf(s,"%.10f",temps_ch);
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

// Description:
//    Imprime le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Postraitement::printOn(Sortie& s ) const
{
  s << que_suis_je() << finl;
  return s << finl;
}


// Description:
//    Lit les directives de postraitement sur un flot d'entree.
//    Format:
//    Postraitement
//    {
//      Sondes bloc de lecture des sondes |
//      Champs bloc de lecture des champs a postraiter |
//      Statistiques bloc de lecture objet statistique |
//      Fichier
//    }
// Precondition:
// Parametre: Entree& s
//    Signification:  un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouverte attendue
// Exception: lecture des champs: mot clef "dt_post" attendu
// Exception: lecture du bloc statistique: mot clef "dt_post" attendu
// Exception: mot clef inconnu
// Effets de bord:
// Postcondition:
Entree& Postraitement::readOn(Entree& s )
{
  Cerr << "Reading guidelines of postprocessing" << finl;
  assert(mon_probleme.non_nul());

  const Nom& le_nom_du_post =le_nom();
  Nom vide;
  if ((le_nom_du_post!="neant")&&(le_nom_du_post!=vide))
    {
      nom_fich_+="_";
      nom_fich_+=le_nom_du_post;
    }

  Probleme_base& le_pb = mon_probleme.valeur();
  le_domaine=le_pb.domaine();

  ////////////////////////////////////////
  // Creation d'alias pour certains champs
  ////////////////////////////////////////
  Noms liste_noms;
  const Probleme_base& Pb = probleme();
  Pb.get_noms_champs_postraitables(liste_noms);
  // On ajoute temperature_physique aux champs definis
  if ( liste_noms.rang("temperature")!=-1 && !comprend_champ_post("temperature_physique") )
    {
      Nom in("{ temperature_physique ");
      int isP1B=0;
      Motcle disc(probleme().discretisation().que_suis_je());
      if ((disc=="VEF")||(disc=="VEFPreP1b")) isP1B=1;
      if (isP1B==1)
        in+="Tparoi_VEF { source";
      in+="  refChamp { Pb_champ ";
      in+=probleme().le_nom();
      in+=" temperature }";
      if (isP1B==1)
        in+=" } ";
      in+=" }";
      Cerr<<" Building of temperature_physique  "<<in<<finl;
      EChaine IN(in);
      lire_champs_operateurs(IN);
    }

  Postraitement_base::readOn(s);

  if (Motcle(format)=="MED") format="med";
  if ((Motcle(format)!="LML")
      && (Motcle(format)!="MESHTV")
      && (Motcle(format)!="MED")
      && (Motcle(format)!="LATA")
      && (Motcle(format)!="LATA_V1")
      && (Motcle(format)!="LATA_V2"))
    {
      Cerr<<"The wanted postprocessing format " << format << " is not recognized"<<finl;
      Cerr<<"The recognized formats are lml, meshtv, med, lata, lata_V1 and lata_V2"<<finl;
      exit();
    }
  // Changement pour la 1.6.4, le format LATA par defaut est LATA_V2
  if (Motcle(format)=="LATA") format="lata_V2";
  if (Motcle(format)=="LATA_V1") format="lata";

  nom_fich_+=".";
  nom_fich_+=format;

  // Les sondes sont completees (en effet, si les sondes
  // ont des champs statistiques, on n'a besoin d'avoir
  // lu le bloc statistiques ET le bloc sondes)

  les_sondes_.completer();

  //On type l objet Format_Post

  Nom type_format = "Format_Post_";
  type_format += format;
  if (Motcle(format)=="LATA")
    type_format +="_V1";

  format_post.typer_direct(type_format);

  Nom base_name(nom_fich_);
  base_name.prefix(format);
  base_name.prefix(".");
  //format_post->initialize_by_default(base_name);
  format_post->initialize(base_name,binaire,option_para);

  //Le test de verification a ete simplifie entre la v1.5.1 et la v1.5.2
  //On simplifie donc la methode test_coherence
  format_post->test_coherence(champs_demande_,stat_demande_,dt_post_ch_,dt_post_stat_);

  return s;
}

void Postraitement::set_param(Param& param)
{
  param.ajouter("Fichier",&nom_fich_);
  param.ajouter("Format",&format);
  param.ajouter("Parallele",&option_para);
  param.ajouter_non_std("Sondes",(this));
  param.ajouter_non_std("Champs",(this));
  param.ajouter_non_std("Statistiques",(this));
  param.ajouter_non_std("Domaine",(this));
  param.ajouter_non_std("Sondes_Int",(this));
  param.ajouter_non_std("Tableaux_Int",(this));
  param.ajouter_non_std("Statistiques_en_serie",(this));
  param.ajouter_non_std("Definition_champs",(this));
}

int Postraitement::lire_motcle_non_standard(const Motcle& mot, Entree& s)
{
  Motcle motlu;
  if (mot=="Sondes")
    {
      Cerr << "Reading of probes" << finl;
      les_sondes_.associer_post(*this);
      s >> les_sondes_;
      sondes_demande_ = 1;
      return 1;
    }
  else if (mot=="Champs")
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
      if ((champs_demande_) || (stat_demande_))
        {
          Cerr<<"The domain must be specified before reading Champs and Statistiques blocks "<<finl;
          exit();
        }

      Nom nom_du_domaine;
      s >> nom_du_domaine;
      le_domaine=ref_cast(Domaine,Interprete::objet(nom_du_domaine));
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
  else
    {
      Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}

// Description:
//    Constructeur par defaut.
//    Les frequences de postraitement prennent la valeur
//    par defaut 1e6. Et aucun postraitement n'est demande.
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
Postraitement::Postraitement()
{
  est_le_premier_postraitement_pour_nom_fich_=-1;
  est_le_dernier_postraitement_pour_nom_fich_=-1;
  temps_=-1;
  dt_post_ch_=dt_post_stat_=dt_post_tab=dt_integr_serie_=1.e6;
  nb_pas_dt_post_ = (int)(pow(2.0,(double)((sizeof(int)*8)-1))-1);
  sondes_demande_ = champs_demande_ = stat_demande_ = stat_demande_definition_champs_ =tableaux_demande_=0;
  binaire=-1;
  tstat_deb_=tstat_fin_=tstat_dernier_calcul_=-1;
  nb_champs_stat_=0;
  lserie_=0;
  option_para="SIMPLE";
  nom_fich_ = nom_du_cas();
  format="lml";
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


                  LIST_CURSEUR(Champ_Generique) curseur = champs_post_complet_;
                  while(curseur)
                    {
                      if (sub_type(Champ_Gen_de_Champs_Gen,curseur.valeur().valeur()))
                        {
                          const Champ_Gen_de_Champs_Gen& champ_post = ref_cast(Champ_Gen_de_Champs_Gen,curseur.valeur().valeur());
                          champ_post.lire_bidon(is);
                        }
                      ++curseur;
                    }
                }
              else // tinit=>temps_derniere_mise_a_jour_stats : on fait la reprise
                {
                  champs_post_complet_.reprendre(is);

                  // On modifie l'attribut tstat_deb_ et l'attribut t_debut_ des champs
                  // pour tenir compte de la reprise
                  tstat_deb_ = tstat_deb_sauv;

                  LIST_CURSEUR(Champ_Generique) curseur = champs_post_complet_;

                  while(curseur)
                    {
                      if (sub_type(Champ_Gen_de_Champs_Gen,curseur.valeur().valeur()))
                        {
                          Champ_Gen_de_Champs_Gen& champ_post = ref_cast_non_const(Champ_Gen_de_Champs_Gen,curseur.valeur().valeur());
                          champ_post.fixer_tstat_deb(tstat_deb_,temps_derniere_mise_a_jour_stats);
                        }
                      ++curseur;
                    }
                }
            }
          else  // lecture pour sauter le bloc
            {
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

// Description:
//    Lit le nom des champs a postraiter sur un flot d'entree.
//    Format:
//    {
//       ???
//    }
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
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
              else if (motlu=="flux_bords")
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
                              creer_champ_post_moreqn("operateur","flux_bords",i,j,compo,s);
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
  Motcle elem("elem");
  Motcle som("som");
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

  probleme().verifie_tdeb_tfin("statistics block");

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

// Description:
// On recherche les champs statistiques dans les sources du champ courant
int Postraitement::cherche_stat_dans_les_sources(const Champ_Gen_de_Champs_Gen& ch, Motcle nom)
{
  if (sub_type(Champ_Generique_Statistiques_base,ch))
    {
      // Activer pour lancer la sauvegarde et la reprise des statistiques
      stat_demande_definition_champs_ = 1;
      const Champ_Generique_Statistiques_base& champ_stat = ref_cast(Champ_Generique_Statistiques_base,ch);
      tstat_deb_ = champ_stat.tstat_deb();
      tstat_fin_ = champ_stat.tstat_fin();
      probleme().verifie_tdeb_tfin(nom);
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

// Description:
//Lit les champs a postraiter sur un flot d'entree.
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
      s>>champ;
      complete_champ(champ,motlu);
      s>>motlu;
    }

  return 1;
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

// Description:
//    Initialise le postraitement.
//    Cree le fichier associe au postraitement, ecrit
//    des infos sur TrioU. Ecrit le probleme sur
//    le fichier.
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
void Postraitement::init()
{
  assert(est_le_premier_postraitement_pour_nom_fich()!=-1);
  assert(est_le_dernier_postraitement_pour_nom_fich()!=-1);
  Schema_Temps_base& sch = mon_probleme->schema_temps();
  double temps_courant = sch.temps_courant();
  double tinit = sch.temps_init();
  Postraitement::formats_supportes=Motcles(4);
  assert(formats_supportes.size()==4);
  if(formats_supportes[0]!="lml")
    {
      formats_supportes[0]="lml";
      formats_supportes[1]="lata";
      formats_supportes[2]="meshtv";
      formats_supportes[3]="med";
    }

  const Domaine& dom=le_domaine.valeur();
  const Nom& nom_du_domaine = dom.le_nom();
  int reprise = mon_probleme->reprise_effectuee();
  Nom name=nom_fich().prefix(format);
  name.prefix(".");
  format_post->modify_file_basename(name,reprise && est_le_premier_postraitement_pour_nom_fich_,tinit);
  format_post->ecrire_entete(temps_courant,reprise,est_le_premier_postraitement_pour_nom_fich_);
  format_post->preparer_post(nom_du_domaine,est_le_premier_postraitement_pour_nom_fich_,reprise,tinit);

  ////////////////////////////////////////////////////////////////////////

  // S'il existe un champ a postraiter aux faces, on stocke ici une ref a la zone dis base du champ
  // PQ : 13/06/13 : mis en attribut de la classe pour gerer les champs FACES en maillage deformable
  //REF(Zone_dis_base) zone_dis_pour_faces;

  {
    LIST_CURSEUR(Nom) curseur1 = noms_champs_a_post_;
    Nom le_nom_champ_post;

    while(curseur1)
      {
        const Nom& nom_post = curseur1.valeur();
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

            if (Motcle(loc_post) == "FACES")
              {
                if (Motcle(format).debute_par("lata")==0)
                  {
                    Cerr<<"The field "<<nom[0]<<" can not be postprocessed to the faces in the format "<<format<<finl;
                    Cerr<<"The postprocessing to the faces is allowed only in the format lata"<<finl;
                    exit();
                  }
                else
                  zone_dis_pour_faces = champ->get_ref_zone_dis_base();
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

        ++curseur1;
      }
  }


  ////////////////////////////////////////////////////////////////////
  // If domain is not time dependant, we write it in the ::init() method
  // else we write it at each postraiter_champs() call
  if(!dom.deformable())
    {
      format_post->ecrire_domaine(dom,est_le_premier_postraitement_pour_nom_fich_);

      if (zone_dis_pour_faces.non_nul())
        {
          const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis_pour_faces.valeur());
          const IntTab& faces_sommets = zone_vf.face_sommets();
          const int nb_sommets = dom.nb_som();
          const int nb_faces = faces_sommets.dimension(0);
          format_post->ecrire_item_int("FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.zone(0).le_nom(),
                                       "FACES", /* localisation */
                                       "SOMMETS", /* reference */
                                       faces_sommets,
                                       nb_sommets);

          const IntTab& elem_faces = zone_vf.elem_faces();
          format_post->ecrire_item_int("ELEM_FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.zone(0).le_nom(),
                                       "ELEMENTS", /* localisation */
                                       "FACES", /* reference */
                                       elem_faces,
                                       nb_faces);

        }
    }

  dernier_temps=mon_probleme->schema_temps().temps_init();
  format_post->modify_file_basename(name,0,tinit);
}


// Description:
//    Finalise le postraitement
//    Ferme le fichier associe.
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
void Postraitement::finir()
{
  // Fermeture du fichier
  if (est_le_dernier_postraitement_pour_nom_fich_)
    {
      format_post->finir(est_le_dernier_postraitement_pour_nom_fich_);
      Nom name=nom_fich().prefix(format);
      name.prefix(".");
      format_post->modify_file_basename(name,mon_probleme->reprise_effectuee(),-1);
    }
}

// Description:
//    Effectue le postraitement des Champs de facon imperative.
//    Mets a jour les champs crees.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Postraitement::postraiter_champs()
{
  double temps_courant = mon_probleme->schema_temps().temps_courant();
  const Domaine& dom=le_domaine.valeur();

  if (temps_ < temps_courant)
    {
      if (est_le_premier_postraitement_pour_nom_fich_)
        format_post->ecrire_temps(temps_courant);
    }
  // We write the time dependant domain here
  if (dom.deformable())
    {
      format_post->ecrire_domaine(dom,est_le_premier_postraitement_pour_nom_fich_);

      if (zone_dis_pour_faces.non_nul())
        {
          const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis_pour_faces.valeur());
          const IntTab& faces_sommets = zone_vf.face_sommets();
          const int nb_sommets = dom.nb_som();
          const int nb_faces = faces_sommets.dimension(0);
          format_post->ecrire_item_int("FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.zone(0).le_nom(),
                                       "FACES", /* localisation */
                                       "SOMMETS", /* reference */
                                       faces_sommets,
                                       nb_sommets);

          const IntTab& elem_faces = zone_vf.elem_faces();
          format_post->ecrire_item_int("ELEM_FACES", /* Identifiant */
                                       dom.le_nom(),
                                       dom.zone(0).le_nom(),
                                       "ELEMENTS", /* localisation */
                                       "FACES", /* reference */
                                       elem_faces,
                                       nb_faces);

        }
    }

  format_post->init_ecriture(temps_courant,temps_,est_le_premier_postraitement_pour_nom_fich_,dom);

  if (temps_ < temps_courant)
    temps_=temps_courant;

  LIST_CURSEUR(Nom) curseur1 = noms_champs_a_post_;

  while(curseur1)
    {
      const Champ_Generique_base& champ = get_champ_post(curseur1.valeur());

      //Etape de calcul
      //Le champ cree est rendu dans champ_ecriture

      Champ espace_stockage;
      const Champ_base& champ_ecriture = champ.get_champ(espace_stockage);
      const DoubleTab& valeurs_post = champ_ecriture.valeurs();

      //Etape de recuperation des informations specifiques au champ a postraiter

      Entity loc = champ.get_localisation();
      const Nom localisation = get_nom_localisation(loc);
      const Noms nom_post = champ.get_property("nom");
      const Noms& unites = champ.get_property("unites");
      const Noms& noms_compo = champ.get_property("composantes");
      const double& temps_champ = champ.get_time();
      int tenseur = champ.get_info_type_post();

      //Etape pour savoir si on doit postraiter un champ ou une de ses composantes

      int ncomp = Champ_Generique_base::composante(curseur1.valeur(),nom_post[0],noms_compo,champ.get_property("synonyms"));

      //La distinction du type de postraitement (tableau ou tenseur) est fait dans la methode postraiter par la valeur de tenseur
      Nom nature("scalar");
      if (champ_ecriture.nature_du_champ()==vectoriel) nature="vector";
      postraiter(dom,unites,noms_compo,ncomp,temps_champ,temps_courant,curseur1.valeur(),localisation,nature,valeurs_post,tenseur);
      ++curseur1;
    }

  format_post->finir_ecriture(temps_courant);
  return 1;
}

int Postraitement::postraiter_tableaux()
{
  double temps_courant = mon_probleme->schema_temps().temps_courant();
  const Domaine& dom=le_domaine.valeur();

  //Methode ecrire_item_int codee uniquement pour les formats lml et lata
  //Sans doute pas testee par les cas de non regression
  //Signature a revoir (supprimer id_zone)

  //Si codage de la methode pour meshtv, ajouter init_ecriture() et finir_ecriture()

  if (temps_ < temps_courant)
    {
      temps_=temps_courant;
      if (est_le_premier_postraitement_pour_nom_fich_)
        format_post->ecrire_temps(temps_courant);
    }

  LIST_CURSEUR(REF(IntVect)) curseur1 = tableaux_a_postraiter_;
  LIST_CURSEUR(Nom) curseur2 = noms_tableaux_;

  while(curseur1)
    {
      const Nom& id_item = curseur2.valeur();
      const Nom& id_du_domaine = dom.le_nom();
      const Nom& id_zone = dom.zone(0).le_nom();
      const Nom localisation="";
      const Nom reference="";
      const IntVect& val =  curseur1->valeur();
      const int ref_size =0;

      format_post->ecrire_item_int(id_item,id_du_domaine,id_zone,localisation,reference,val,ref_size);

      ++curseur1;
      ++curseur2;
    }


  return 1;
}

// Description:
//    Effectue le postraitement des Champs si cela est necessaire.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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


// Description:
//    Effectue le postraitement lie au sondes de facon imperative.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Postraitement::postraiter_sondes()
{
  les_sondes_.postraiter();
  les_sondes_int_.postraiter(mon_probleme->schema_temps().temps_courant());
  return 1;
}


// Description:
//    Mets a jour (en temps) le sondes.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Postraitement::traiter_sondes()
{
  double temps=mon_probleme->schema_temps().temps_courant();
  double tinit=temps-mon_probleme->schema_temps().temps_calcul();
  les_sondes_.mettre_a_jour(temps,tinit);
  return 1;
}

int Postraitement::postraiter(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int& ncomp,
                              const double& temps_champ,double& temps_courant,
                              Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs,int tenseur)

{

  if (!tenseur)
    postraiter_tableau(dom,unites,noms_compo,ncomp,temps_champ,temps_courant,nom_post,localisation,nature,valeurs);
  else
    postraiter_tenseur(dom,unites,noms_compo,ncomp,temps_champ,temps_courant,nom_post,localisation,nature,valeurs);

  return 1;
}

int Postraitement::postraiter_tableau(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int& ncomp,
                                      const double& temps_champ,double& temps_courant,
                                      Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs)
{
  const Nom& id_du_domaine = dom.le_nom();
  const Nom& id_champ_ecrit = nom_post;

  DoubleTab valeurs_tmp;
  int dim0 = valeurs.dimension(0);
  int nb_dim_val = 1;

  if (ncomp==-1)
    {
      if (valeurs.nb_dim()>1)
        {
          nb_dim_val = valeurs.dimension(1);
          valeurs_tmp.resize(dim0,nb_dim_val);
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_dim_val; j++)
              valeurs_tmp(i,j) = valeurs(i,j);
        }
      else
        {
          valeurs_tmp.resize(dim0,nb_dim_val);
          for (int i=0; i<dim0; i++)
            valeurs_tmp(i,0) = valeurs(i);
        }
    }
  else
    {
      valeurs_tmp.resize(dim0);
      if (valeurs.nb_dim()>1)
        {
          int nb_comp = valeurs.dimension(1);
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_comp; j++)
              if (j==ncomp)
                valeurs_tmp(i) = valeurs(i,j);
        }
      else
        valeurs_tmp = valeurs;
    }

  const DoubleTab& val_post_ecrit = valeurs_tmp;
  format_post->ecrire_champ(dom,unites,noms_compo,ncomp,temps_champ,temps_courant,id_champ_ecrit,id_du_domaine,localisation,nature,val_post_ecrit);
  return 1;
}

int Postraitement::postraiter_tenseur(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int& ncomp,
                                      const double& temps_champ,double& temps_courant,
                                      Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs)
{

  int nb_comp = noms_compo.size();
  DoubleTab valeurs_comp(valeurs.dimension(0));

  for (int comp=0; comp<nb_comp; comp++)
    {
      Nom nom_post_compo = noms_compo[comp];
      for (int i=0; i<valeurs_comp.dimension(0); i++)
        valeurs_comp(i) = valeurs(i,comp);

      postraiter_tableau(dom,unites,noms_compo,comp,temps_champ,temps_courant,nom_post_compo,localisation,nature,valeurs_comp);

    }
  return 1;
}

Nom Postraitement::set_expression_champ(const Motcle& motlu1,const Motcle& motlu2,
                                        const Motcle& motlu3,const Motcle& motlu4,
                                        const int& trouve)
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
// -les sources son nommees (sauf si creation d un Champ_Generique_refChamp)
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
      champ_post.nommer_sources();

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
                                          motlu4,const double& t_deb,const double& t_fin,Entree& s)
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
      champ_post.nommer_sources();

      //On fixe l attribut compo_ pour le Champ_Generique_Interpolation cree par cette macro
      Champ_Generique_Statistiques_base& champ_stat = ref_cast_non_const(Champ_Generique_Statistiques_base,champ_post.get_source(0));
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
void Postraitement::creer_champ_post_moreqn(const Motcle& type,const Motcle& option,const int& num_eq,const int& num_morceau,const int& compo,Entree& s)
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
  Motcle type_option;
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
  else if (option=="flux_bords")
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
  champ_post.nommer_sources();

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

  CONST_LIST_CURSEUR(Champ_Generique) curseur_liste_champs = champs_post_complet_;

  while (curseur_liste_champs)
    {
      if (curseur_liste_champs.valeur().valeur().comprend_champ_post(identifiant))
        return 1;
      ++curseur_liste_champs;
    }

  return 0;
}

const Champ_Generique_base& Postraitement::get_champ_post(const Motcle& nom) const
{

  REF(Champ_Generique_base) ref_champ;
  CONST_LIST_CURSEUR(Champ_Generique) curseur_liste_champs = champs_post_complet_;

  while (curseur_liste_champs)
    {
      try
        {
          return curseur_liste_champs.valeur().valeur().get_champ_post(nom);
        }
      catch (Champs_compris_erreur)
        {
        }
      ++curseur_liste_champs;
    }

  throw Champs_compris_erreur();

  //Pour compilation
  return ref_champ;

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
