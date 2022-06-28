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
#include <Postraitement_lata.h>
#include <PE_Groups.h>
#include <EcrFicPartageBin.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Zone_VF.h>
#include <Format_Post_Lata_V1.h>
#include <Param.h>

class Comm_Group;

// Quelques variables internes qu'on ne veut pas publier...
class Postraitement_lata_private_data
{
public:
  REF(Champ_base) le_champ;
  DoubleTab       valeurs;
  DoubleTab       centres_de_gravites;
  IntVect         les_polys;
};

Implemente_instanciable_sans_constructeur_ni_destructeur(Postraitement_lata,"Postraitement_lata",Postraitement_base);


// ****************************************************************************

// Description:
//  Renvoie l'extension conventionnelle des fichiers lata : ".lata"
const Nom& Postraitement_lata::extension_lata()
{
  static const Nom extension_(".lata");
  return extension_;
}

// Description:
//  Renvoie le nom d'un fichier sans le path :
//  on enleve les caracteres avant le dernier /
const char * Postraitement_lata::remove_path(const char * filename)
{
  int i = (int)strlen(filename);
  while (i > 0 && filename[i-1] != '/')
    i--;
  return filename + i;
}


void Postraitement_lata::ecrire_entete(const Nom& basename,
                                       const Nom& discretisation,
                                       Format format,
                                       const Comm_Group * comm_group)
{
  Format_Post_Lata::Options_Para option;
  if (comm_group)
    option = Format_Post_Lata::SINGLE_FILE;
  else
    option = Format_Post_Lata::MULTIPLE_FILES;

  Format_Post_Lata_V1::ecrire_entete_lata_V1(basename,
                                             option,
                                             1 /* est le premier post */);
}

void Postraitement_lata::ecrire_zone(const Nom& basename,
                                     const Zone& la_zone,
                                     Format format,
                                     const Comm_Group * comm_group,
                                     const Zone_dis_base * la_zone_dis)
{
  Format_Post_Lata::Options_Para option;
  if (comm_group)
    option = Format_Post_Lata::SINGLE_FILE;
  else
    option = Format_Post_Lata::MULTIPLE_FILES;
  Format_Post_Lata::Format fmt;
  if (format == ASCII)
    fmt = Format_Post_Lata::ASCII;
  else
    fmt = Format_Post_Lata::BINAIRE;

  Format_Post_Lata_V1::ecrire_domaine_lata_V1(la_zone.domaine().le_nom(),
                                              la_zone.type_elem().valeur().que_suis_je(),
                                              Objet_U::dimension,
                                              la_zone.domaine().les_sommets(),
                                              la_zone.les_elems(),
                                              Format_Post_Lata::INITIALIZED,
                                              basename,
                                              fmt,
                                              option);

  if (la_zone_dis)
    {
      Format_Post_Lata_V1::ecrire_faces_lata_V1(basename,
                                                fmt,
                                                option,
                                                la_zone.domaine().le_nom(),
                                                la_zone_dis->face_sommets(),
                                                ref_cast(Zone_VF, *la_zone_dis).elem_faces(),
                                                la_zone.domaine().les_sommets().dimension(0));
    }
}

void Postraitement_lata::ecrire_temps(const Nom& basename,
                                      const double temps,
                                      Format format,
                                      const Comm_Group * comm_group)
{
  Format_Post_Lata::Options_Para option;
  if (comm_group)
    option = Format_Post_Lata::SINGLE_FILE;
  else
    option = Format_Post_Lata::MULTIPLE_FILES;

  Format_Post_Lata::Status status = Format_Post_Lata::INITIALIZED;

  double t = temps;
  Format_Post_Lata::ecrire_temps_lata(temps,
                                      t,
                                      basename,
                                      status,
                                      option);
}

void Postraitement_lata::ecrire_champ(const Nom&         basename,
                                      const Nom&         nom_du_champ,
                                      Type_Champ         type,
                                      char               type_char,
                                      const Nom&         nom_domaine,
                                      const Nom&         nom_probleme,
                                      double             temps,
                                      Localisation       localisation,
                                      const DoubleTab&   valeurs,
                                      Format             format,
                                      const Comm_Group * comm_group)
{
  Format_Post_Lata::Options_Para option;
  if (comm_group)
    option = Format_Post_Lata::SINGLE_FILE;
  else
    option = Format_Post_Lata::MULTIPLE_FILES;
  Format_Post_Lata::Format fmt;
  if (format == ASCII)
    fmt = Format_Post_Lata::ASCII;
  else
    fmt = Format_Post_Lata::BINAIRE;

  Nom loc;
  switch (localisation)
    {
    case SOMMETS:
      loc = "SOM";
      break;
    case ELEMENTS:
      loc = "ELEM";
      break;
    case FACES:
      loc = "FACES";
      break;
    }

  Format_Post_Lata_V1::ecrire_champ_lata_V1(nom_du_champ,
                                            nom_domaine,
                                            loc,
                                            valeurs,
                                            temps,
                                            basename,
                                            fmt,
                                            option);
}

int Postraitement_lata::finir_lata(const Nom& basename,
                                   const Comm_Group * comm_group)
{
  Format_Post_Lata::Options_Para option;
  if (comm_group)
    option = Format_Post_Lata::SINGLE_FILE;
  else
    option = Format_Post_Lata::MULTIPLE_FILES;

  Format_Post_Lata_V1::finir_lata_V1(basename,
                                     option,
                                     1);
  return 1;
}


// Description:
//  Constructeur par defaut: initialisation des membres avec des valeurs
//  par defaut.
Postraitement_lata::Postraitement_lata()
{
  // Les donnees privees de la classe:
  private_data_ = new Postraitement_lata_private_data;

  // Quelques valeurs par defaut
  lata_basename_ = Objet_U::nom_du_cas();
  format_ = BINAIRE;
  fichiers_multiples_ = 0;
  entete_ok_ = 0;
  skip_header_ = 0;
  dt_post_ = 0.;
  temps_dernier_post_ = -1E40;
  verbose_stream_ = 0;
}

// Description: Destroy me ... muahahahaha
Postraitement_lata::~Postraitement_lata()
{
  delete private_data_;
}

// Description: Appel a Postraitement_base::readOn
Entree& Postraitement_lata::readOn(Entree& is)
{
  Postraitement_base::readOn(is);
  return is;
}

Sortie& Postraitement_lata::printOn(Sortie& os) const
{
  return os;
}

void Postraitement_lata::set_param(Param& param)
{
  //Nom msg;
  //msg = "Post-processing period for ";
  //msg += que_suis_je();
  //msg += " must be greater or equal to 0.";
  //param.ajouter_condition("value_of_dt_post_ge_0",msg);
  param.ajouter("nom_fichier",&lata_basename_);
  param.ajouter("fichiers_multiples",&fichiers_multiples_);
  param.ajouter("skip_header",&skip_header_);
  param.ajouter_non_std("format",(this));
  param.ajouter_non_std("champs|fields",(this));
  param.ajouter_non_std("print",(this));
  param.ajouter_non_std("dt_post",(this));
}

int Postraitement_lata::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="format")
    {
      lire_format(is);
      return 1;
    }
  else if (mot=="champs|fields")
    {
      lire_champs(is);
      return 1;
    }
  else if (mot=="print")
    {
      if (Process::je_suis_maitre()) verbose_stream_ = &Cerr;
      return 1;
    }
  else if (mot=="dt_post")
    {
      Nom expression;
      is >> expression;
      Cerr << expression;
      fdt_post.setNbVar(1);
      fdt_post.setString(expression);
      fdt_post.addVar("t");
      fdt_post.parseString();
      dt_post_ = fdt_post.eval();
      return 1;
    }
  else
    return Postraitement_base::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description: lecture du format d'enregistrement (ascii ou binaire)
void Postraitement_lata::lire_format(Entree& is)
{
  Motcle motcle;
  is >> motcle;
  if (motcle == "ascii")
    format_ = ASCII;
  else if (motcle == "binaire")
    format_ = BINAIRE;
  else
    {
      Cerr << "Postraitement_lata::lire_format" << finl;
      Cerr << " We expected \"ascii\" or \"binaire\" after format." << finl;
      Cerr << " We found " << motcle << finl;
      exit();
    }
}

// Description: lecture d'une liste de champs a postraiter
//  On attend ceci :
//    champs sommets|elements|faces  (le mot cle champs a deja ete lu)
//    {
//       nom_du_champ
//     [ nom_du_champ ]
//     [ ... ]
//    }
//  Les champs lus sont ajoutes a la liste correspondante (sommets, elements, ...),
//  les doublons sont retires.
//  Le nom du champ doit etre compris par le probleme (pb.comprend_champ(...))
//  Attention: les champs postraites aux faces doivent etre discretises aux faces
//    (pas d'interpolation, on postraite le champ tel quel).
void Postraitement_lata::lire_champs(Entree& is)
{
  Motcle motcle;
  is >> motcle;
  Localisation loc = SOMMETS;
  if (motcle == "sommets")
    loc = SOMMETS;
  else if (motcle == "elements")
    loc = ELEMENTS;
  else if (motcle == "faces")
    loc = FACES;
  else
    {
      Cerr << "Error in Postraitement_lata::lire_champs :" << finl;
      Cerr << " We read : " << motcle << finl;
      Cerr << " We expected \"sommets\", \"elements\" or \"faces\" after \"champs\"" << finl;
      exit();
    }
  Motcles& liste =
    (loc == SOMMETS) ? liste_champs_aux_sommets
    : (loc == ELEMENTS) ? liste_champs_aux_elements
    : liste_champs_aux_faces;

  is >> motcle;
  if (motcle != "{")
    {
      Cerr << "Error in Postraitement_lata::lire_champs :" << finl;
      Cerr << " We read : " << motcle << finl;
      Cerr << " We expected { after sommets/elements/faces." << finl;
      exit();
    }
  Probleme_base& pb = mon_probleme.valeur();
  REF(Champ_base) refch_base;

  while (1)
    {
      is >> motcle;
      if (motcle == "}")
        break;
      Noms liste_noms;
      pb.get_noms_champs_postraitables(liste_noms);
      pb.creer_champ(motcle);
      int ok=0;
      for (int i=0; i<liste_noms.size(); i++)
        if (Motcle(liste_noms[i])==motcle) ok=1;

      if (!ok)
        {
          Cerr << "Error in Postraitement_lata::lire_champs :\n";
          Cerr << " The field " << motcle;
          Cerr << " is not understood by the problem " << pb.le_nom() << finl;
          exit();
        }

      // Si un champ aux faces est demande, il faut que le champ d'origine
      // soit aux faces (on ne sait pas interpoler le champ)
      if (loc == FACES)
        {
          refch_base = pb.get_champ(motcle);

          const int nb_valeurs =
            refch_base.valeur().valeurs().dimension(0);
          const int nb_faces =
            pb.domaine_dis().zone_dis(0).valeur().face_sommets().dimension(0);
          if (nb_valeurs != nb_faces)
            {
              Cerr << "Error in Postraitement_lata::lire_champs :\n";
              Cerr << " The field " << motcle;
              Cerr << " can not be postprocessed to the faces.\n";
              Cerr << " (Fields allowed: only fields discretized to the faces)" << finl;
              exit();
            }
        }
      if (!liste.contient_(motcle))
        liste.add(motcle);
    }
}

// Description:
//  Evident...
// Parametre: loc
// Description: valeurs autorisees: SOMMETS, ou ELEMENTS ou FACES
// Parametre: liste
// Description: une liste de champs compris par le probleme.
void Postraitement_lata::postraiter_liste_champs(Localisation loc, const Motcles& liste)
{
  if (verbose_stream_)
    {
      *verbose_stream_ << "Postraitement_lata::postraiter_liste_champs ";
      switch(loc)
        {
        case SOMMETS:
          *verbose_stream_ << "sommets" << finl;
          break;
        case ELEMENTS:
          *verbose_stream_ << "elements" << finl;
          break;
        case FACES:
          *verbose_stream_ << "faces" << finl;
          break;
        default:
          Cerr << "Localization " << (int)loc << " not recognized in Postraitement_lata::postraiter_liste_champs" << finl;
          exit();
        }
    }

  const Comm_Group * comm_group = 0;
  if (fichiers_multiples_ == 0)
    comm_group = & PE_Groups::groupe_TRUST();

  const int nb_champs = liste.size();
  if (nb_champs == 0)
    return;

  // Postraitement des champs :
  const Probleme_base& pb          = mon_probleme.valeur();
  const Zone&           la_zone     = pb.domaine().zone(0);
  const Zone_dis_base&  zone_dis    = pb.domaine_dis().zone_dis(0).valeur();
  const int          nb_elem     = la_zone.nb_elem();
  const int          nb_sommets  = la_zone.nb_som();
  DoubleTab&            valeurs             = private_data_->valeurs;
  DoubleTab&            centres_de_gravites = private_data_->centres_de_gravites;

  IntVect&              les_polys           = private_data_->les_polys;
  REF(Champ_base) &     le_champ            = private_data_->le_champ;

  if (loc == ELEMENTS)
    {
      ////centres_de_gravites.resize(nb_elem, Objet_U::dimension);
      // Ouille... faut calculer ca ???
      ////la_zone.calculer_centres_gravite(centres_de_gravites);
      const Zone_VF& zvf = ref_cast(Zone_VF,zone_dis);
      centres_de_gravites = zvf.xp();

      les_polys.resize(nb_elem);
      int i;
      for (i = 0; i < nb_elem; i++)
        les_polys(i) = i;
    }

  for (int i = 0; i < nb_champs; i++)
    {
      const Motcle nom_du_champ = liste[i];
      if (verbose_stream_)
        *verbose_stream_ << "Postraitement du champ " << nom_du_champ << finl;


      le_champ = pb.get_champ(nom_du_champ);

      {
        if (loc == FACES)
          valeurs.resize(0);
        else if (loc == SOMMETS )
          {
            int dimension0 = (loc==SOMMETS) ? nb_sommets : nb_elem;
            const int dimension1 = le_champ.valeur().nb_comp();
            valeurs.resize(dimension0, dimension1);
          }
        else
          {
            int dimension0 = (loc==SOMMETS) ? nb_sommets : nb_elem;
            if (le_champ.valeur().nb_comp() == 1)
              valeurs.resize(dimension0);
            else
              {
                const int dimension1 = le_champ.valeur().nb_comp();
                valeurs.resize(dimension0, dimension1);
              }
          }
      }

      DoubleTab * valeurs_a_ecrire = 0;

      switch (loc)
        {
        case SOMMETS:
          // Interpolation du champ aux sommets
          le_champ.valeur().valeur_aux_sommets(pb.domaine(), valeurs);
          valeurs_a_ecrire = &valeurs;
          break;
        case ELEMENTS:
          // Interpolation du champ aux elements
          le_champ.valeur().valeur_aux_elems(centres_de_gravites, les_polys, valeurs);
          valeurs_a_ecrire = &valeurs;
          break;
        case FACES:
          // Pour un champ aux faces, aucune interpolation, on ecrit le tableau tel quel.
          valeurs_a_ecrire = &le_champ.valeur().valeurs();
          break;
        default:
          exit();
        }

      ecrire_champ(lata_basename_,
                   nom_du_champ, // nom du champ
                   CHAMP,  // type
                   'I',    // champ instationnaire
                   pb.domaine().le_nom(),
                   pb.le_nom(),
                   temps_,
                   loc,
                   *valeurs_a_ecrire,
                   format_,
                   comm_group);
    }
  // On libere la memoire des tableaux temporaires
  // (mais on garde les objets)
  //valeurs.resize(0);
  valeurs.reset();
  centres_de_gravites.resize(0);
  les_polys.resize(0);
}

// Description:
//  Ecrit l'entete si ca n'a pas ete fait, puis, si le temps est ecoule,
//  post-traite les champs demandes. Le temps actuel est le temps
//  attribue lors du dernier appel a mettre_a_jour du postraitement.
void Postraitement_lata::postraiter(int forcer)
{
  /*
            if (verbose_stream_) {
            *verbose_stream_ << "Postraitement_lata::postraiter nom:" << le_nom();
            *verbose_stream_ << " fichier:" << lata_basename_ << finl;
            } */
  const Comm_Group * comm_group = 0;
  if (fichiers_multiples_ == 0)
    comm_group = & PE_Groups::groupe_TRUST();

  if (! skip_header_)
    {
      if (! entete_ok_)
        {
          // Ecriture de l'entete du fichier
          if (verbose_stream_)
            *verbose_stream_ << " Ecriture de l'entete." << finl;
          ecrire_entete(lata_basename_,
                        mon_probleme.valeur().discretisation().que_suis_je(),
                        format_,
                        comm_group);
          if (verbose_stream_)
            *verbose_stream_ << " Ecriture de la zone." << finl;

          // S'il y a des champs aux faces a postraiter, ecrire les faces
          const Zone_dis_base * zone_dis_baseptr = 0;
          if (liste_champs_aux_faces.size() > 0)
            zone_dis_baseptr = & mon_probleme.valeur().domaine_dis().zone_dis(0).valeur();
          ecrire_zone(lata_basename_,
                      mon_probleme.valeur().domaine().zone(0),
                      format_,
                      comm_group,
                      zone_dis_baseptr);
          entete_ok_ = 1;
        }
    }
  if (! forcer)
    if (! dt_post_ecoule(dt_post_, temps_dernier_post_))
      // L'intervalle de temps entre postraitements n'est pas ecoule
      return;

  if (! skip_header_)
    {
      if (verbose_stream_)
        *verbose_stream_ << " Ecriture du temps : " << temps_ << finl;
      ecrire_temps(lata_basename_,
                   temps_,
                   format_,
                   comm_group);

    }
  postraiter_liste_champs(SOMMETS, liste_champs_aux_sommets);
  postraiter_liste_champs(ELEMENTS, liste_champs_aux_elements);
  if (liste_champs_aux_faces.size() > 0)
    postraiter_liste_champs(FACES, liste_champs_aux_faces);

  temps_dernier_post_ = temps_;
}

// Description: renvoie 1 si l'intervalle de temps demande entre deux
// postraitements est ecoule.
int Postraitement_lata::dt_post_ecoule(double& dt_post,
                                       double temps_dernier_post)
{
  if (dt_post<=temps_ - temps_dernier_post_ || dt_post==0)
    return 1;
  else
    {
      fdt_post.setVar("t",temps_);
      dt_post = fdt_post.eval();
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      double i, j, epsilon = 1.e-8;
      modf(temps_/dt_post + epsilon, &i);
      modf(temps_dernier_post/dt_post + epsilon, &j);
      return ( i>j );
    }
}

