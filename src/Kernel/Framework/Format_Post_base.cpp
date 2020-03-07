/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Format_Post_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////
#include <Format_Post_base.h>
#include <Zone_VF.h>
#include <Param.h>

Implemente_base(Format_Post_base,"Format_Post_base",Objet_U);

// Description: erreur => exit
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

int Format_Post_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
  exit();
  return -1;
}

// Description:
//  Initialise le fichier avec des parametres appropries pour son
//  format (exemple: format ascii, efface le fichier existant, un
//  fichier unique pour tous les processeurs etc...)
//  Methode a surcharger dans les classes derivees.
// Valeur de retour: 1 si l'operation a reussi, 0 sinon.
int Format_Post_base::initialize_by_default(const Nom& file_basename)
{
  Cerr << "Format_Post_base::initialize_by_default(" << file_basename
       << ")\n method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::initialize(const Nom& file_basename, const int& format, const Nom& option_para)
{
  Cerr << "Format_Post_base::initialize(" << file_basename
       << ")\n method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::modify_file_basename(const Nom file_basename, const int a_faire, const double tinit)
{
  return 0;
}

int Format_Post_base::ecrire_entete(double temps_courant,int reprise,const int& est_le_premier_post)
{
  Cerr << "Format_Post_base::ecrire_entete method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::finir(int& est_le_dernier_post)
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

int Format_Post_base::preparer_post(const Nom& id_du_domaine,const int& est_le_premier_post,
                                    const int& reprise,
                                    const double& t_init)
{

  Cerr << "Format_Post_base::preparer_post(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;

}

int Format_Post_base::test_coherence(const int& champs, const int& stat, const double& dt_ch, const double dt_stat)
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

// Description:
//  Ecriture d'un maillage. L'ecriture du domaine a lieu soit au debut,
//  avant le premier appel a ecrire_temps, soit plus tard (maillage dynamique),
//  mais ce n'est pas forcement supporte par le postraitement.
//
// Parametre: id_domaine
// Signification: le nom affecte au domaine dans le fichier lata.
// Parametre: type_elem
// Signification: le type de l'element geometrique (un type supporte par
//  la classe derivee, en general on comprend au moins "TETRAEDRE", "HEXAEDRE",
//  "TRIANGLE" et "RECTANGLE")
// Parametre: dimension
// Signification: la dimension du domaine (nombre de coordonnees des sommets)
//  On peut avoir un domaine de dimension 3 et des elements de type triangle
//  (postraitement d'une interface ou du bord d'un domaine volumique).
// Parametre: sommets
// Signification:
//  Coordonnees des sommets. S'il n'est pas vide, il faut que dimension(1)==dimension
// Parametre: elements
// Signification:
//  Indices des sommets de chaque element. dimension(1) doit correspondre au type
//  de l'element (3 pour un triangle, 4 pour un rectangle ou un tetraedre, etc...)
//
// Precondition:
//  Le fichier doit avoir ete initialise.
// Valeur de retour: 1 si operation reussie, 0 sinon (par exemple,
//  preconditions non remplies ou fonctionnalite non supportee par le
//  format).

int Format_Post_base::ecrire_domaine(const Domaine& domaine,const int& est_le_premier_post)
{
  Cerr << "Format_Post_base::ecrire_domaine(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

int Format_Post_base::ecrire_domaine_dis(const Domaine& domaine,const REF(Zone_dis_base)& zone_dis_base,const int& est_le_premier_post)
{
  return ecrire_domaine(domaine, est_le_premier_post);
}

// Description:
//  Commence l'ecriture d'un pas de temps. La classe derivee doit accepter
//  de recevoir plusieurs appels consecutifs a cette methode avec le meme temps.
// Precondition:  Fichier initialise.
// Valeur de retour: 1 si operation reussie, 0 sinon.

int Format_Post_base::ecrire_temps(const double temps)
{
  Cerr << "Format_Post_base::ecrire_temps(const double temps)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

// Description:
//  Ecriture d'un champ dans le fichier de postraitement.
// Parametre: id_du_champ
// Signification:
//  identifiant du champ (permet d'identifier un champ unique si on donne
//  en plus un numero de pas de temps)
// Parametre: id_du_domaine
// Signification:
//  identifiant du domaine sur lequel le champ est defini
//  Ce domaine doit avoir ete ecrit avant par "ecrire_domaine".
// Parametre: localisation
// Signification:
//  localisation des valeurs du champ (SOMMETS, ELEMENTS, ou tout autre id. de
//  tableau deja ecrit)
//  (tout n'est pas obligatoirement supporte par tous les postraitements)
// Parametre: data
// Signification:
//  tableau de valeurs a postraiter. Le nombre de lignes du tableau
//  doit etre egal au nombre de lignes du tableau "localisation"
//  (nombre de sommets, d'elements ou de faces ou autre).
// Valeur de retour: 1 si operation reussie, 0 sinon (par exemple,
//  preconditions non remplies ou fonctionnalite non supportee par le
//  format).

int Format_Post_base::ecrire_champ(const Domaine& domaine,const Noms& unite_,const Noms& noms_compo,
                                   int ncomp, double temps_,double temps_courant,
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
                                    int ncomp, double temps_,double temps_courant,
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

// Description:
//  Ecriture d'un tableau d'entiers dans le fichier de postraitement
//  Voir aussi ecrire_champ
// Parametre: reference
// Signification: Nom d'un autre tableau deja ecrit. data[i] est un
//  indice dans ce tableau (exemple: on peut ecrire le tableau
//  FACES_VOISINS, localisation=FACES, reference=ELEMENTS
//  car le tableau est indexe par un numero de face et contient
//  des indices d'elements)
//  L'ecriture d'un domaine entraine automatiquement l'existence
//  d'une tableau SOMMETS et d'un tableau ELEMENTS
// Parametre: reference_size
// Signification: la taille (locale) du tableau cite en reference
//  (dimension(0) du tableau). Cette dimension est utilisee pour
//  renumeroter le contenu du tableau data afin de creer une numerotation
//  globale lorsque tous les processeurs ecrivent dans un fichier unique.
int Format_Post_base::ecrire_item_int(const Nom&   id_item,
                                      const Nom&   id_du_domaine,
                                      const Nom& id_zone,
                                      const Nom&   localisation,
                                      const Nom&   reference,
                                      const IntVect& data,
                                      const int reference_size)
{
  Cerr << "Format_Post_base::ecrire_champ_int(...)\n"
       << " method not coded for " << que_suis_je() << finl;
  return 0;
}

//Methode visant a postraiter les valeurs d un champ discret a un instant donne
//Le tableau de valeurs (data) a postraiter est redimensionne si necessaire
//Puis appel des methodes de l interface generique pour postraiter
/////////////////Exemple d utilisation//////////////////////////////////////
//Rq : Dans cet exemple on considere les valeurs d un champ scalaire aux elements
//  Format_Post format_post_debug;
//  Nom type_format = "Format_Post_Meshtv";
//  format_post_debug.typer_direct(type_format);
//  Nom nom_fichier("postraitement_debug");
//  Nom id_champ_post="temperature_elem_dom";
//  Nature_du_champ nature = scalaire;
//  Motcle loc="ELEM";
//  const Domaine& dom = ...
//  const double& temps_courant = ...
//  const Champ_base& ch_temp = mon_probleme->get_champ("temperature");
//  const Zone_dis_base& zone_dis = ch_temp.zone_dis_base();
//  const DoubleTab& temp = ch_temp.valeurs();
//  double temps_champ = ch_temp.temps();
//  Noms compo=ch_temp.noms_compo();
//  Noms unites = ch_temp.unites();

//  format_post_debug->postraiter_debug_valeurs_un_instant(nom_fichier,dom,zone_dis,temps_champ,temps_courant,id_champ_post,nature,compo,unites,loc,temp);

/////////////////Exemple d utilisation//////////////////////////////////////

void Format_Post_base::postraiter_debug_valeurs_un_instant(const Nom& nom_fich,
                                                           const Domaine& dom,
                                                           const Zone_dis_base& zone_dis,
                                                           double temps_champ, double temps_courant,
                                                           const Nom& id_champ_post,const Nature_du_champ& nature_champ,
                                                           const Noms& noms_compos,const Noms& unites,const Motcle& loc_post,
                                                           const DoubleTab& data,
                                                           int ncomp,
                                                           int is_axi,
                                                           int champ,int stat,double dt_ch,double dt_stat,int reprise,
                                                           int est_le_premier_post, int est_le_dernier_post,
                                                           double t_init,double temps_post)
{

  const Nom& id_domaine = dom.le_nom();
  const int nb_compo = noms_compos.size();
  DoubleTab valeurs_tmp;
  int dim0 = data.dimension(0);
  int nb_dim_val = 1;

  if (ncomp==-1)
    {
      if (data.nb_dim()>1)
        {
          nb_dim_val = data.dimension(1);
          valeurs_tmp.resize(dim0,nb_dim_val);
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_dim_val; j++)
              valeurs_tmp(i,j) = data(i,j);
        }
      else
        {
          valeurs_tmp.resize(dim0,nb_dim_val);
          for (int i=0; i<dim0; i++)
            valeurs_tmp(i,0) = data(i);
        }
    }
  else
    {
      valeurs_tmp.resize(dim0);
      if (data.nb_dim()>1)
        {
          int nb_comp = data.dimension(1);
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_comp; j++)
              if (j==ncomp)
                valeurs_tmp(i) = data(i,j);
        }
      else
        valeurs_tmp = data;
    }

  initialize_by_default(nom_fich);
  test_coherence(champ,stat,dt_ch,dt_stat);
  ecrire_entete(temps_courant,reprise,est_le_premier_post);
  preparer_post(id_domaine,est_le_premier_post,reprise,t_init);
  completer_post(dom,is_axi,nature_champ,nb_compo,noms_compos,loc_post,id_champ_post);
  ecrire_domaine(dom,est_le_premier_post);

  if (loc_post=="FACES")
    {
      const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis);
      const IntTab& faces_sommets = zone_vf.face_sommets();
      const int nb_sommets = dom.nb_som();
      const int nb_faces = faces_sommets.dimension(0);
      ecrire_item_int("FACES", // Identifiant
                      dom.le_nom(),
                      dom.zone(0).le_nom(),
                      "FACES", // localisation
                      "SOMMETS", // reference
                      faces_sommets,
                      nb_sommets);

      const IntTab& elem_faces = zone_vf.elem_faces();
      ecrire_item_int("ELEM_FACES", // Identifiant
                      dom.le_nom(),
                      dom.zone(0).le_nom(),
                      "ELEMENTS", // localisation
                      "FACES", // reference
                      elem_faces,
                      nb_faces);

    }

  ecrire_temps(temps_courant);
  init_ecriture(temps_courant,temps_post,est_le_premier_post,dom);
  Nom nature("scalar");
  if (nature_champ==vectoriel) nature="vector";
  ecrire_champ(dom,unites,noms_compos,ncomp,temps_champ,temps_courant,id_champ_post,id_domaine,loc_post,nature,valeurs_tmp);
  finir_ecriture(temps_courant);
  finir(est_le_dernier_post);

}


//Actuellement on commente
//Pourquoi pas de declaration de la methode (fonction) static

/*
// Description: Methode outil pour retirer les doublons dans un tableau.
static void array_trier_retirer_doublons(ArrOfInt & array)
{
const int size_ = array.size_array();
if (size_ == 0)
return;
// Tri dans l'ordre croissant
array.ordonne_array();
// Retire les doublons
int new_size_ = 1;
int last_value = array[0];
for (int i = 1; i < size_; i++) {
if (array[i] != last_value) {
array[new_size_] = last_value = array[i];
new_size_++;
}
}
array.resize_array(new_size_);
}
*/


/*
// Description:
//  Ecriture des bords du domaine dans le fichier de postraitement
// Parametre: id_du_domaine
// Signification: le nom du domaine dont on ecrit les bords
// Parametre: type_faces
// Signification: le type des faces du bord (le nom d'un type derive de Elem_Geom_base)
// Parametre: sommets
// Signification: Les coordonnees des sommets du domaine.
// Parametre: faces_sommets
// Signification: pour chaque face de bord, numeros de ses sommets dans le domaine
// Parametre: faces_num_bord
// Signification: pour chaque face, numero du bord auquel elle appartient
//  (le premier bord porte le numero zero)
// Parametre: bords_nom
// Signification: le nom de chaque bord.
int Format_Post_base::ecrire_bords(const Nom    & id_du_domaine,
const Motcle & type_faces,
const DoubleTab & sommets,
const IntTab & faces_sommets,
const IntTab & faces_num_bord,
const Noms   & bords_nom)
{
// Le comportement par defaut de "ecrire_bords" consiste a
// ecrire une nouvelle geometrie appelee "id_du_domaine_boundary"

int i, j;

// Creation de la liste des indices de sommets utilises dans les faces de bord:
ArrOfInt sommets_bord;
sommets_bord = faces_sommets;
array_trier_retirer_doublons(sommets_bord);

// Creation d'un tableau de renumerotation:
//   renum[indice global] = indice dans le tableau des sommets de bord
const int nb_sommets = sommets.dimension(0);
const int nb_sommets_bord = sommets_bord.size_array();
ArrOfInt renum(nb_sommets);
renum = -1;
for (i = 0; i < nb_sommets_bord; i++)
renum[sommets_bord[i]] = i;

// Renumerotation du tableau faces_sommets
IntTab faces_sommets_renum(faces_sommets);
{
ArrOfInt & faces_sommets_array = faces_sommets_renum;
const int faces_sommets_size_array = faces_sommets_array.size_array();
for (i = 0; i < faces_sommets_size_array; i++) {
int & isom = faces_sommets_array[i];
isom = renum[isom];
}
}

// Construction du tableau des sommets
const int dim = sommets.dimension(1);
DoubleTab coord_som(nb_sommets_bord, dim);
for (i = 0; i < nb_sommets_bord; i++) {
const int i_old = sommets_bord[i];
for (j = 0; j < dim; j++)
coord_som(i,j) = sommets(i_old,j);
}

// Ecriture de la geometrie:
Nom id = id_du_domaine + "_boundary";
ecrire_domaine(id,
type_faces,
dim,
coord_som,
faces_sommets_renum);

// Ecriture des noms des bords
Nom id_noms = id + "_names";
ecrire_noms(id_noms,
id,
"",
bords_nom);

// Ecriture des numeros de bord
Nom id_tableau = id + "_numbers";


//La methode s appelle maintenant ecrire_item_int()
ecrire_champ_int(id_tableau,
id,
"elem",
id_noms,
faces_num_bord,
bords_nom.size());

return 1;
}

// Description:
//  Ecriture d'un tableau de chaines de caracteres.
//  Voir aussi ecrire_champ
int Format_Post_base::ecrire_noms(const Nom  & id_de_la_liste,
const Nom  & id_du_domaine,
const Nom  & localisation,
const Noms & liste_noms)
{
Cerr << "Format_Post_base::ecrire_noms(...)\n"
<< " method not coded for " << que_suis_je() << finl;
return 0;
}
*/
