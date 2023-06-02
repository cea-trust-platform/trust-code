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
#include <Format_Post_base.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_base(Format_Post_base,"Format_Post_base",Objet_U);

/*! @brief erreur => exit
 *
 */
Sortie& Format_Post_base::printOn(Sortie& os) const
{
  Cerr << "Format_Post_base::printOn : error" << finl;
  exit();
  return os;
}

Entree& Format_Post_base::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" post-processing format object"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Format_Post_base::resetTime(double t, const std::string dirname)
{
  if (dirname.empty())
    {
      Cerr << "Format '" << que_suis_je() << " does not support resetTime()!!" << std::endl;
      Process::exit(-1);
      // but LATA does :-)
    }
}

int Format_Post_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return -1;
}

/*! @brief Initialise le fichier avec des parametres appropries pour son format (exemple: format ascii, efface le fichier existant, un
 *
 *   fichier unique pour tous les processeurs etc...)
 *   Methode a surcharger dans les classes derivees.
 *  Valeur de retour: 1 si l'operation a reussi, 0 sinon.
 *
 */
int Format_Post_base::initialize_by_default(const Nom& file_basename)
{
  Cerr << "Format_Post_base::initialize_by_default(" << file_basename
       << ")\n method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::initialize(const Nom& file_basename, const int format, const Nom& option_para)
{
  Cerr << "Format_Post_base::initialize(" << file_basename
       << ")\n method not coded for " << que_suis_je() << finl;
  return 0;
}

/*! @brief Modification of the post processing file name.
 * For save/restart this might also move and rename files around to avoid overriding existing files.
 */
int Format_Post_base::modify_file_basename(const Nom file_basename, bool for_restart, const double tinit)
{
  return 0;
}

int Format_Post_base::ecrire_entete(const double temps_courant,const int reprise,const int est_le_premier_post)
{
  Cerr << "Format_Post_base::ecrire_entete method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::finir(const int est_le_dernier_post)
{
  Cerr << "Format_Post_base::finir method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::init_ecriture(double temps_courant,double temps_post,
                                    int est_le_premier_postraitement_pour_nom_fich_,const Domaine& domaine)
{
  return 1;
}

int Format_Post_base::finir_ecriture(double temps_courant)
{
  return 1;
}



int Format_Post_base::completer_post(const Domaine& dom,const int is_axi,
                                     const Nature_du_champ& nature,const int nb_compo,const Noms& noms_compo,
                                     const Motcle& loc_post,const Nom& le_nom_champ_post)
{

  Cerr << "Format_Post_base::preparer_post_champ(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;

}

int Format_Post_base::preparer_post(const Nom& id_du_domaine,const int est_le_premier_post,
                                    const int reprise,
                                    const double t_init)
{

  Cerr << "Format_Post_base::preparer_post(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;

}

int Format_Post_base::test_coherence(const int champs, const int stat, const double dt_ch, const double dt_stat)
{
  if (stat && champs && !est_egal(dt_stat,dt_ch))
    {
      Cerr << "Error!" << finl;
      Cerr << "You try to write unknown fields and statistic fields with a different time period (dt_post):" << finl;
      Cerr << "-----------------------------" << finl;
      Cerr << "Champs dt_post " << dt_ch << finl;
      Cerr << "Statistiques dt_post " << dt_stat << finl;
      Cerr << "-----------------------------" << finl;
      Cerr << "It is not possible in the same postprocessing block." << finl;
      Cerr << "Try to use different postprocessing blocks, one for the unknown fields" << finl;
      Cerr << "and one for the statistic fields." << finl;
      exit();
    }
  return 1;
}

/*! @brief Ecriture d'un maillage.
 *
 * L'ecriture du domaine a lieu soit au debut, avant le premier appel a ecrire_temps, soit plus tard (maillage dynamique),
 *   mais ce n'est pas forcement supporte par le postraitement.
 *
 *
 * @param (id_domaine) le nom affecte au domaine dans le fichier lata.
 * @param (type_elem) le type de l'element geometrique (un type supporte par la classe derivee, en general on comprend au moins "TETRAEDRE", "HEXAEDRE", "TRIANGLE" et "RECTANGLE")
 * @param (dimension) la dimension du domaine (nombre de coordonnees des sommets) On peut avoir un domaine de dimension 3 et des elements de type triangle (postraitement d'une interface ou du bord d'un domaine volumique).
 * @param (sommets) Coordonnees des sommets. S'il n'est pas vide, il faut que dimension(1)==dimension
 * @param (elements) Indices des sommets de chaque element. dimension(1) doit correspondre au type de l'element (3 pour un triangle, 4 pour un rectangle ou un tetraedre, etc...)
 */

int Format_Post_base::ecrire_domaine(const Domaine& domaine,const int est_le_premier_post)
{
  Cerr << "Format_Post_base::ecrire_domaine(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::ecrire_domaine_dis(const Domaine& domaine,const OBS_PTR(Domaine_dis_base)& domaine_dis_base,const int est_le_premier_post)
{
  return ecrire_domaine(domaine, est_le_premier_post);
}

/*! @brief Commence l'ecriture d'un pas de temps.
 *
 * La classe derivee doit accepter de recevoir plusieurs appels consecutifs a cette methode avec le meme temps.
 *
 */

int Format_Post_base::ecrire_temps(const double temps)
{
  Cerr << "Format_Post_base::ecrire_temps(const double temps)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

/*! @brief Ecriture d'un champ dans le fichier de postraitement.
 *
 * @param (id_du_champ) identifiant du champ (permet d'identifier un champ unique si on donne en plus un numero de pas de temps)
 * @param (id_du_domaine) identifiant du domaine sur lequel le champ est defini Ce domaine doit avoir ete ecrit avant par "ecrire_domaine".
 * @param (localisation) localisation des valeurs du champ (SOMMETS, ELEMENTS, ou tout autre id. de tableau deja ecrit) (tout n'est pas obligatoirement supporte par tous les postraitements)
 * @param (data) tableau de valeurs a postraiter. Le nombre de lignes du tableau doit etre egal au nombre de lignes du tableau "localisation" (nombre de sommets, d'elements ou de faces ou autre). Valeur de retour: 1 si operation reussie, 0 sinon (par exemple, preconditions non remplies ou fonctionnalite non supportee par le format).
 */

int Format_Post_base::ecrire_champ(const Domaine& domaine,const Noms& unite_,const Noms& noms_compo,
                                   int ncomp, double temps_,
                                   const Nom&   id_du_champ,
                                   const Nom&         id_du_domaine,
                                   const Nom&         localisation,
                                   const Nom&   nature,
                                   const DoubleTab&   data)
{
  Cerr << "Format_Post_base::ecrire_champ(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

// Surcharge de la methode precedente pour le format XYZ : donner les coordonees de postraitement en argument
int Format_Post_base::ecrire_champ2(const Domaine& domaine,const Noms& unite_,const Noms& noms_compo,
                                    int ncomp, double temps_,
                                    const Nom&   id_du_champ,
                                    const Nom&         id_du_domaine,
                                    const Nom&         localisation,
                                    const Nom&   nature,
                                    const DoubleTab&   data,
                                    const DoubleTab&   coord)
{
  Cerr << "Format_Post_base::ecrire_champ2(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

/*! @brief Ecriture d'un tableau d'entiers dans le fichier de postraitement
 *
 * @sa ecrire_champ
 *
 * @param (reference) Nom d'un autre tableau deja ecrit. data[i] est un indice dans ce tableau (exemple: on peut ecrire le tableau FACES_VOISINS, localisation=FACES, reference=ELEMENTS car le tableau est indexe par un numero de face et contient des indices d'elements) L'ecriture d'un domaine entraine automatiquement l'existence d'une tableau SOMMETS et d'un tableau ELEMENTS
 * @param (reference_size) la taille (locale) du tableau cite en reference (dimension(0) du tableau). Cette dimension est utilisee pour renumeroter le contenu du tableau data afin de creer une numerotation globale lorsque tous les processeurs ecrivent dans un fichier unique.
 */
int Format_Post_base::ecrire_item_int(const Nom&   id_item,
                                      const Nom&   id_du_domaine,
                                      const Nom& id_domaine,
                                      const Nom&   localisation,
                                      const Nom&   reference,
                                      const IntVect& data,
                                      const int reference_size)
{
  Cerr << "Format_Post_base::ecrire_champ_int(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}
