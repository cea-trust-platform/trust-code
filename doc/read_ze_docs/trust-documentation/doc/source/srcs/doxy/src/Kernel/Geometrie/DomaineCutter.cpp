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

#include <DomaineCutter.h>
#include <ArrOfBit.h>
#include <Domaine.h>
#include <Connectivite_som_elem.h>
#include <SFichierBin.h>
#include <Array_tools.h>
#include <Scatter.h>
#include <TRUSTArrays.h>
#include <Sous_Domaine.h>
#include <Sparskit.h>
#include <Poly_geom_base.h>
#include <Sortie_Brute.h>
#include <TRUSTVect.h>
#include <FichierHDFPar.h>
#include <communications.h>

Implemente_instanciable_sans_constructeur(DomaineCutter,"DomaineCutter",Objet_U);

/*! @brief Appel invalide.
 *
 * exit.
 *
 */
const DomaineCutter& DomaineCutter::operator=(const DomaineCutter& dc)
{
  exit();
  return *this; // pour le compilateur
}

/*! @brief Appel invalide.
 *
 * exit.
 *
 */
DomaineCutter::DomaineCutter(const DomaineCutter& dc): Objet_U(dc)
{
  exit();
}

/*! @brief Appel invalide.
 *
 * exit.
 *
 */
Sortie& DomaineCutter::printOn(Sortie& os) const
{
  Cerr << "Error : DomaineCutter::printOn should not be used." << finl;
  exit();
  return os; // pour le compilateur
}

/*! @brief Appel invalide.
 *
 * exit.
 *
 */
Entree& DomaineCutter::readOn(Entree& s)
{
  Cerr << "Error : DomaineCutter::readOn should not be used." << finl;
  exit();
  return s; // pour le compilateur
}

/*! @brief Creation de la liste des sommets du sous-domaine "partie".
 *
 * C'est l'ensemble des sommets des elements appartenant a ce sous-domaine.
 *  On ne traite que les elements reels.
 *
 * @param (nb_sommets) nombre de sommets du domaine globale
 * @param (les_elems)
 * @param (elem_part) tableau de decoupage (pour chaque element i du domaine global, elem_part[i] est le numero du sous-domaine auquel il est affecte)
 * @param (partie) le numero du sous-domaine a construire
 * @param (liste_sommets) en sortie : liste des sommets du sous-domaine: liste_sommets[i] est l'indice dans le domaine_globale du i-ieme sommet du sous-domaine. Les indices sont classes dans l'ordre croissant.
 * @param (liste_inverse_sommets) en sortie : on lui donne la taille nb_sommets et on l'initialise. liste_inverse_sommet[i] est l'indice du sommet dans le sous-domaine ou -1 si le sommet i n'est pas dans le sous-domaine)
 */
static void construire_liste_sommets_sousdomaine(const int nb_sommets,
                                                 const IntTab& les_elems,
                                                 const ArrOfInt& liste_elements,
                                                 const int i_part,
                                                 const Static_Int_Lists *som_raccord,
                                                 ArrOfInt& liste_sommets,
                                                 ArrOfInt& liste_inverse_sommets)
{
  const int nb_elem_part = liste_elements.size_array();
  const int nb_sommets_par_element = les_elems.dimension(1);

  // Algorithme : on parcourt les elements: pour les elements de la partie,
  // on marque les sommets de l'element par un drapeau.
  // Puis on fait une boucle sur les sommets, et ceux dont le drapeau
  // est mis sont mis dans la liste.

  // D'abord, on compte les sommets de la partie et on
  // remplit drapeau_sommet
  ArrOfBit drapeau_sommet(nb_sommets);

  drapeau_sommet = 0;
  // Nombre de sommets de la partie "part"
  int nb_sommets_part = 0;
  for (int i_elem = 0; i_elem < nb_elem_part; i_elem++)
    {
      const int elem = liste_elements[i_elem];
      for (int j = 0; j < nb_sommets_par_element; j++)
        {
          int sommet = les_elems(elem, j);
          if (sommet>-1)
            {
              int bit = drapeau_sommet.testsetbit(sommet);
              // Si le drapeau n'etait pas mis, cela fait un sommet de plus
              if (! bit)
                nb_sommets_part++;
            }
        }
    }
  //sommets a ajouter a cause de som_raccord
  if (som_raccord)
    for (int s = 0; s < som_raccord->get_nb_lists(); s++)
      for (int i = 0; i < som_raccord->get_list_size(s); i++)
        if ((*som_raccord)(s, i) == i_part && !drapeau_sommet.testsetbit(s)) //le sommet est demande par ce proc
          nb_sommets_part++; //si on ne l'a pas deja, on le rajoute

  // Remplissage de liste_sommets et liste_inverse_sommets
  liste_sommets.resize_array(0); // On oublie les valeurs precedentes
  liste_sommets.resize_array(nb_sommets_part);

  liste_inverse_sommets.resize_array(0); // On oublie les valeurs precedentes
  liste_inverse_sommets.resize_array(nb_sommets,Array_base::NOCOPY_NOINIT);
  liste_inverse_sommets = -1;

  int n = 0;
  for (int i = 0; i < nb_sommets; i++)
    {
      if (drapeau_sommet[i])
        {
          liste_sommets[n] = i;
          liste_inverse_sommets[i] = n;
          n++;
        }
    }
}

// Remplissage du tableau des coordonnees des sommets d'une partie
// a partir des coordonnees des sommets du domaine complet (sommets_glob)
// et de la liste des sommets de la partie (liste_sommets)
// On cree sommets_loc comme suit:
//   sommets_loc.dimension(0) = liste_sommets.size_array()
//   sommets_loc.dimension(1) = sommets_glob.dimension(1)
//
// Parametre:     sommets_glob
// Signification: les coordonnees des sommets du domaine global
// Parametre:     liste_sommets
// Signification: les indices des sommets de sommets_glob a copier
//                dans sommets_loc.
static void remplir_coordsommets_sous_domaine(const DoubleTab& sommets_glob,
                                              const ArrOfInt& liste_sommets,
                                              DoubleTab& sommets_loc)
{
  const int nb_som = liste_sommets.size_array();
  const int dim = sommets_glob.dimension(1);
  sommets_loc.resize(nb_som, dim);

  int i;
  for (i = 0; i < nb_som; i++)
    {
      int indice = liste_sommets[i];
      int j;
      for (j = 0; j < dim; j++)
        sommets_loc(i, j) = sommets_glob(indice, j);
    }
}

// Construction du tableau elems des elements de la partie "part".
// elems(i, j) sera le nouveau numero du sommet j de l'element i
// dans la partie part. On utilise la liste_inverse pour obtenir les
// nouveaux numeros des sommets.
// Les elements du domaine local sont crees dans l'ordre croissant de
// leur indice dans le domaine global.

void construire_elems_sous_domaine(const IntTab&    elems_domaine_globale,
                                   const ArrOfInt& liste_elements,
                                   const ArrOfInt& liste_inverse_sommets,
                                   IntTab&    elems_domaine_locale,
                                   ArrOfInt& liste_inverse_elements)
{
  const int nb_elem_tot                = elems_domaine_globale.dimension_tot(0);
  const int nb_sommets_par_element = elems_domaine_globale.dimension(1);

  liste_inverse_elements.resize(nb_elem_tot,Array_base::NOCOPY_NOINIT);
  liste_inverse_elements = -1;

  // Premier passage: comptage du nombre d'elements de la partie
  const int nb_elem_part = liste_elements.size_array();
  elems_domaine_locale.resize(nb_elem_part, nb_sommets_par_element);

  // Deuxieme passage: remplissage du tableau
  for (int i_elem = 0; i_elem < nb_elem_part; i_elem++)
    {
      int elem = liste_elements[i_elem];

      // Nouveau numero de l'element dans la partie
      liste_inverse_elements[elem] = i_elem;
      // Copie de l'element avec traduction du numero des sommets en
      // numero local dans le sous-domaine
      for (int i = 0; i < nb_sommets_par_element; i++)
        {
          int sommet = elems_domaine_globale(elem, i);
          if (sommet<0)
            elems_domaine_locale(i_elem, i) =sommet;
          else
            {
              int new_num = liste_inverse_sommets[sommet];
              assert(new_num >= 0);
              elems_domaine_locale(i_elem, i) = new_num;
            }
        }
    }
}

/*! @brief Pour une liste de "faces" du domaine globale, compter le nombre de faces incluses dans la partie "part" et les copier dans la structure
 *
 *  faces_partie en remplacant les numeros de sommets par les numeros locaux
 *  dans le sous-domaine.
 *  L'ordre des faces est conserve (si une face apparait avant une autre dans
 *  la liste du domaine complet et si elles sont toutes les deux dans la sous-partie,
 *  alors leur ordre est conserve). C'est important pour le periodique
 *  (hypothese qu'il y a correspondance entre la face i et la face i+n/2 du bord
 *  periodique).
 *  Attention: la condition pour qu'une face soit incluse est "la face appartient
 *   a un element de la partie voisine". La condition "les sommets des faces sont
 *   des sommets de joint" n'est PAS suffisante.
 *
 */
void construire_liste_faces_sous_domaine(const ArrOfInt& elements_voisins,
                                         const IntVect& elem_part,
                                         const int     partie,
                                         const IntTab&    faces_sommets,
                                         const ArrOfInt& liste_inverse_sommets,
                                         IntTab& faces_sommets_partie)
{
  const int nb_faces = faces_sommets.dimension(0);
  const int nb_sommets_par_face = faces_sommets.dimension(1);
  int i;

  assert(elements_voisins.size_array() == nb_faces);
  // La liste des faces du tableau faces_sommets a inclure dans le sous-domaine
  ArrOfInt liste_faces;
  liste_faces.set_smart_resize(1); // Pour faire append_array...

  // Premier passage : on cherche les faces a inclure
  {
    for (i = 0; i < nb_faces; i++)
      {
        int elem = elements_voisins[i];
        const int part = elem_part[elem];
        if (part == partie)
          liste_faces.append_array(i);
      }
  }

  const int nb_faces_part = liste_faces.size_array();
  faces_sommets_partie.resize(nb_faces_part, nb_sommets_par_face);

  // Deuxieme passage : stockage des faces et conversion des numeros
  //  de sommets des faces en numeros locaux dans le sous-domaine.
  for (i = 0; i < nb_faces_part; i++)
    {
      const int i_face = liste_faces[i];
      int j;
      for (j = 0; j < nb_sommets_par_face; j++)
        {
          int sommet = faces_sommets(i_face, j);
          if (sommet>-1)
            {
              int new_num = liste_inverse_sommets[sommet];
              assert(new_num >= 0);
              faces_sommets_partie(i, j) = new_num;
            }
          else
            faces_sommets_partie(i, j) = -1;
        }
    }
}

// Pour les trois fonctions suivantes :
// Remplissage des frontieres de la partie associee a un processeur.
void DomaineCutter::construire_faces_bords_ssdom(const ArrOfInt& liste_inverse_sommets,
                                                 const int partie,
                                                 Domaine& domaine_partie) const
{
  const Domaine& domaine = ref_domaine_.valeur();
  int i_fr = 0;
  ArrOfInt elements_voisins;
  for(const auto& itr: domaine.faces_bord())
    {
      const Frontiere& frontiere = itr;
      Frontiere& front_partie = domaine_partie.faces_bord().add(Bord());
      front_partie.nommer(frontiere.le_nom());
      front_partie.associer_domaine(domaine_partie);
      front_partie.faces().typer(frontiere.faces().type_face());
      voisins_bords_.copy_list_to_array(i_fr, elements_voisins);
      construire_liste_faces_sous_domaine(elements_voisins,
                                          ref_elem_part_.valeur(),
                                          partie,
                                          frontiere.faces().les_sommets(),
                                          liste_inverse_sommets,
                                          front_partie.faces().les_sommets());
      i_fr++;
    }
}

/*! @brief Constructions des raccords de la sous_partie a partir des raccords du domaine global.
 *
 *   Les Raccord_local_homogene sont transformes en Raccord_distant_homogene.
 *
 */
void DomaineCutter::construire_faces_raccords_ssdom(const ArrOfInt& liste_inverse_sommets,
                                                    const int partie,
                                                    Domaine& domaine_partie) const
{
  const Domaine& domaine = ref_domaine_.valeur();
  int i_fr = domaine.nb_bords();
  ArrOfInt elements_voisins;
  for(const auto& itr: domaine.faces_raccord())
    {
      const Raccord& raccord = itr;
      const Frontiere& frontiere = raccord.valeur();
      Raccord& raccord_partie = domaine_partie.faces_raccord().add(Raccord());
      Nom type_raccord = frontiere.que_suis_je();
      if (type_raccord == "Raccord_local_homogene")
        type_raccord = "Raccord_distant_homogene";
      raccord_partie.typer(type_raccord);
      Frontiere& front_partie = raccord_partie.valeur();
      front_partie.nommer(frontiere.le_nom());
      front_partie.associer_domaine(domaine_partie);
      front_partie.faces().typer(frontiere.faces().type_face());
      voisins_bords_.copy_list_to_array(i_fr, elements_voisins);
      construire_liste_faces_sous_domaine(elements_voisins,
                                          ref_elem_part_.valeur(),
                                          partie,
                                          frontiere.faces().les_sommets(),
                                          liste_inverse_sommets,
                                          front_partie.faces().les_sommets());
      i_fr++;
    }
}

void DomaineCutter::construire_frontieres_internes_ssdom(const ArrOfInt& liste_inverse_sommets,
                                                         const int partie,
                                                         Domaine& domaine_partie) const
{
  // Rappel : les bords internes sont des "frontieres" a l'interieur du domaine
  // (par exemple une plaque d'epaisseur nulle dans l'ecoulement)
  const Domaine& domaine = ref_domaine_.valeur();
  int i_fr = domaine.nb_bords()+domaine.nb_raccords();
  ArrOfInt elements_voisins;

  for(const auto& itr: domaine.bords_int())
    {
      const Frontiere& frontiere = itr;
      Frontiere& front_partie =
        domaine_partie.bords_int().add(Bord_Interne());
      front_partie.nommer(frontiere.le_nom());
      front_partie.associer_domaine(domaine_partie);
      front_partie.faces().typer(frontiere.faces().type_face());
      voisins_bords_.copy_list_to_array(i_fr, elements_voisins);
      construire_liste_faces_sous_domaine(elements_voisins,
                                          ref_elem_part_.valeur(),
                                          partie,
                                          frontiere.faces().les_sommets(),
                                          liste_inverse_sommets,
                                          front_partie.faces().les_sommets());
      i_fr++;
    }
}

void DomaineCutter::construire_groupe_faces_ssdom(const ArrOfInt& liste_inverse_sommets,
                                                  const int partie,
                                                  Domaine& domaine_partie) const
{
  const Domaine& domaine = ref_domaine_.valeur();

  Static_Int_Lists voisins;
  const int nb_groupe_faces = domaine.nb_groupes_faces();
  ArrOfInt nb_faces(nb_groupe_faces);
  for (int i = 0; i < nb_groupe_faces; i++)
    nb_faces[i] = domaine.groupe_faces(i).nb_faces();
  voisins.set_list_sizes(nb_faces);
  const int nb_som_face = domaine.frontiere(0).faces().nb_som_faces();
  ArrOfInt une_face(nb_som_face);
  ArrOfInt elements_voisins;
  elements_voisins.set_smart_resize(1);
  const IntVect& elem_part = ref_elem_part_.valeur();

  for (int grp = 0; grp < nb_groupe_faces; grp++)
    {
      const Groupe_Faces& groupe_faces = domaine.groupe_faces(grp);
      Groupe_Faces& groupe_partie =
        domaine_partie.groupes_faces().add(Groupe_Faces());
      groupe_partie.nommer(groupe_faces.le_nom());
      groupe_partie.associer_domaine(domaine_partie);
      groupe_partie.faces().typer(groupe_faces.faces().type_face());
      const IntTab& faces_sommets = groupe_faces.faces().les_sommets();
      IntTab& faces_sommets_partie = groupe_partie.faces().les_sommets();

      ArrOfInt liste_faces;
      liste_faces.set_smart_resize(1); // Pour faire append_array...

      // Premier passage : on cherche les faces a inclure
      const IntTab& faces = groupe_faces.les_sommets_des_faces();
      const int n = nb_faces[grp];
      for (int j = 0; j < n; j++)
        {
          for (int k = 0; k < nb_som_face; k++)
            une_face[k] = faces(j, k);
          find_adjacent_elements(som_elem_, une_face, elements_voisins);

          if (elements_voisins.size_array() == 1 && elem_part[elements_voisins[0]] == partie) liste_faces.append_array(j);

          if (elements_voisins.size_array() == 2)
            if (elem_part[elements_voisins[0]] == partie || elem_part[elements_voisins[1]] == partie)
              liste_faces.append_array(j);
        }

      const int nb_faces_part = liste_faces.size_array();
      faces_sommets_partie.resize(nb_faces_part, nb_som_face);

      // Deuxieme passage : stockage des faces et conversion des numeros
      //  de sommets des faces en numeros locaux dans le sous-domaine.
      for (int i = 0; i < nb_faces_part; i++)
        {
          const int i_face = liste_faces[i];
          for (int j = 0; j < nb_som_face; j++)
            {
              int sommet = faces_sommets(i_face, j);
              if (sommet>-1)
                {
                  int new_num = liste_inverse_sommets[sommet];
                  assert(new_num >= 0);
                  faces_sommets_partie(i, j) = new_num;
                }
              else
                faces_sommets_partie(i, j) = -1;
            }
        }
    }
}

/*! @brief Pour chaque pe mentionne dans le tableau "voisins", si un joint avec ce pe n'existe par encore dans le domaine, ajoute un joint et initialise ce joint avec "epaisseur".
 *
 */
static void ajouter_joints(Domaine& domaine, const ArrOfInt& voisins, const int epaisseur)
{
  Joints& joints = domaine.faces_joint();

  const int n = voisins.size_array();
  for (int i = 0; i < n; i++)
    {
      const int pe = voisins[i];
      int j;
      const int nb_joints = joints.size();
      for (j = 0; j < nb_joints; j++)
        if (joints[j].PEvoisin() == pe)
          break;
      if (j == nb_joints)
        {
          Cerr << " Adding of a new joint : " << pe << finl;
          Joint& joint = joints.add(Joint());
          joint.nommer(Nom("Joint_")+Nom(pe));
          joint.associer_domaine(domaine);
          joint.affecte_epaisseur(epaisseur);
          joint.affecte_PEvoisin(pe);
          // Ces joints n'auront pas de sommets communs. Met le flag
          // d'initialisation a 1.
          joint.set_joint_item(Joint::SOMMET).set_items_communs();
        }
    }
}

/*! @brief A partir d'une liste de sommets de depart (liste_sommets_depart), on parcourt les elements voisins de ces sommets (si epaisseur <= 1),
 *
 *   puis les voisins de ces elements (voisins par un sommet de l'element) si epaisseur <= 2,
 *   puis les voisins des voisins si epaisseur <= 3, etc
 *   Les elements appartenant a la "partie_a_ignorer" ne sont pas parcourus.
 *   Les indices des elements parcourus sont ranges dans liste_elements_trouves.
 *   Cette methode a ete codee pour construire_elements_distants_ssdom()
 *
 */
static void parcourir_epaisseurs_elements(const IntTab& elements,
                                          const Static_Int_Lists& elem_som,
                                          const IntVect& elem_part,
                                          ArrOfInt liste_sommets_depart, /* par valeur car on va la modifier */
                                          const int partie_a_ignorer, /* ne pas parcourir les elems de cette partie */
                                          const int epaisseur,
                                          ArrOfInt& liste_elements_trouves)
{
  const int nb_sommets = elem_som.get_nb_lists();
  const int nb_elements = elements.dimension_tot(0);
  const int nb_som_elem = elements.dimension(1);

  liste_elements_trouves.set_smart_resize(1);
  liste_elements_trouves.resize_array(0);

  ArrOfBit sommets_parcourus(nb_sommets);
  ArrOfBit elements_parcourus(nb_elements);
  ArrOfInt new_liste;
  liste_sommets_depart.set_smart_resize(1);
  new_liste.set_smart_resize(1);
  sommets_parcourus = 0;
  elements_parcourus = 0;

  {
    // Marquage des sommets de depart et construction d'une liste de sommets uniques
    const int sz_liste = liste_sommets_depart.size_array();
    for (int i = 0; i < sz_liste; i++)
      {
        const int sommet = liste_sommets_depart[i];
        if (sommet>-1)
          if (!sommets_parcourus.testsetbit(sommet))
            new_liste.append_array(sommet);
      }
  }

  // Boucle sur les epaisseurs successives d'elements autour des sommets de joint.
  for (int ep = 1; ep <= epaisseur; ep++)
    {
      liste_sommets_depart = new_liste;
      new_liste.resize_array(0);
      const int sz_liste = liste_sommets_depart.size_array();
      for (int i_sommet = 0; i_sommet < sz_liste; i_sommet++)
        {
          const int sommet = liste_sommets_depart[i_sommet];
          // Parcours des elements voisins de ce sommet :
          const int nb_elems_voisins = elem_som.get_list_size(sommet);
          for (int i_elem = 0; i_elem < nb_elems_voisins; i_elem++)
            {
              const int elem = elem_som(sommet, i_elem);
              if (elements_parcourus[elem])
                continue; // Cet element a deja ete visite
              // Numero de domaine de l'element voisin:
              const int part = elem_part[elem];
              if (part == partie_a_ignorer)
                continue; // Ne traiter que les elements des autres parties

              liste_elements_trouves.append_array(elem);
              elements_parcourus.setbit(elem);

              // Ajout a la prochaine liste de sommets a traiter les sommets de cet element:
              for (int i = 0; i < nb_som_elem; i++)
                {
                  const int sommet2 = elements(elem, i);
                  if (sommet2>-1)
                    {
                      if (sommets_parcourus.testsetbit(sommet2) == 0)
                        new_liste.append_array(sommet2);
                    }
                }
            }
        }
    }
}

/*! @brief calcul et remplissage de domaine_partie.
 *
 * joint(i).set_joint_item(Joint::ELEMENT).set_items_distants()
 *    (liste des elements distants).
 *    Eventuellement, de nouveaux joints sont crees.
 *  Historique: methode codee en janvier 2006 par B.Mathieu. Je croyais que c'etait
 *   trop complique de determiner les elements distants au moment du scatter et j'ai
 *   fini par trouver comment faire. Du coup la methode ci-dessous n'est plus utilisee
 *   en temps normal. En cas de probleme, on peut la reactiver: en theorie elle donne
 *   exactement le meme resultat que l'algorithme code dans Scatter::calculer_espace_distant_elements.
 *   Attention, elle ne fait rien de special pour le bords periodiques...
 *
 */
void DomaineCutter::construire_elements_distants_ssdom(const int     partie,
                                                       const ArrOfInt& liste_sommets,
                                                       const ArrOfInt& liste_inverse_elements,
                                                       Domaine& domaine_partie) const
{
  const Domaine& domaine          = ref_domaine_.valeur();
  const IntTab& elements    = domaine.les_elems();
  const IntVect& elem_part = ref_elem_part_.valeur();

  const int nb_som_elem = elements.dimension(1);

  // Premiere etape: chercher les elements virtuels de la sous-partie
  // (elements des autres parties situes dans l'epaisseur de joint en partant
  //  des sommets de joint).
  ArrOfInt elements_virtuels;
  {
    // Construction d'un tableau contenant tous les sommets de tous les joints
    ArrOfInt sommets_joint;
    sommets_joint.set_smart_resize(1);
    const int nb_joints = domaine_partie.nb_joints();
    for (int i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        const ArrOfInt& sommets = domaine_partie.joint(i_joint).joint_item(Joint::SOMMET).items_communs();
        const int n = sommets.size_array();
        for (int i = 0; i < n; i++)
          {
            const int sommet_local = sommets[i];
            const int sommet_global = liste_sommets[sommet_local];
            sommets_joint.append_array(sommet_global);
          }
      }

    parcourir_epaisseurs_elements(elements, som_elem_, elem_part,
                                  sommets_joint, // Sommets de depart
                                  partie, /* ignorer les elements de la partie locale */
                                  epaisseur_joint_,
                                  elements_virtuels);
  }
  const int nb_elements_virtuels = elements_virtuels.size_array();
  // Construire la liste des numeros de domaines voisins et ajouter les joints manquants:
  ArrOfInt parties_voisines;
  {
    parties_voisines.set_smart_resize(1);
    for (int i = 0; i < nb_elements_virtuels; i++)
      {
        const int elem = elements_virtuels[i];
        const int part = elem_part[elem];
        parties_voisines.append_array(part);
      }
    array_trier_retirer_doublons(parties_voisines);
    ajouter_joints(domaine_partie, parties_voisines, epaisseur_joint_);
  }

  // Pour chaque partie voisine, chercher les elements distants:
  const int nb_parties_voisines = parties_voisines.size_array();
  ArrOfInt sommets_depart;
  sommets_depart.set_smart_resize(1);
  for (int i_part = 0; i_part < nb_parties_voisines; i_part++)
    {
      const int partie_voisine = parties_voisines[i_part];
      sommets_depart.resize_array(0);
      // Liste des sommets des elements virtuels de la partie voisine:
      for (int i_elem = 0; i_elem < nb_elements_virtuels; i_elem++)
        {
          const int elem = elements_virtuels[i_elem];
          if (elem_part(elem) == partie_voisine)
            {
              for (int i = 0; i < nb_som_elem; i++)
                {
                  const int som = elements(elem, i);
                  sommets_depart.append_array(som);
                }
            }
        }

      // Recherche des elements voisins de cette liste:
      ArrOfInt elements_distants;
      parcourir_epaisseurs_elements(elements, som_elem_, elem_part,
                                    sommets_depart,
                                    partie_voisine, /* ignorer les elements de la partie voisine */
                                    epaisseur_joint_,
                                    elements_distants);

      // Retirer de la liste les elements qui ne sont pas dans la partie locale:
      {
        int count = 0;
        const int n = elements_distants.size_array();
        for (int i = 0; i < n; i++)
          {
            const int elem = elements_distants[i];
            if (elem_part[elem] == partie && elem < elements.dimension(0) )
              elements_distants[count++] = elem;
          }
        elements_distants.resize_array(count);
      }
      // Traduire les indices des elements en indices locaux sur le sous-domaine.
      {
        const int n = elements_distants.size_array();
        for (int i = 0; i < n; i++)
          {
            const int elem = elements_distants[i];
            const int renum = liste_inverse_elements[elem];
            elements_distants[i] = renum;
          }
      }
      // Trier les elements dans l'ordre croissant des indices
      elements_distants.ordonne_array();
      // Stocker la liste des elements distants dans le joint
      Joint&     joint = domaine_partie.joint_of_pe(partie_voisine);
      joint.set_joint_item(Joint::ELEMENT).set_items_distants() = elements_distants;
    }
}

/*! @brief Creation des joints et construction des listes et des tableaux de sommets joints pour tous les joints de
 *
 *   la partie part. Les sommets du joint avec le "PEvoisin" sont
 *   les sommets de la partie "part" qui sont aussi un sommet d'un
 *   element appartenant au PEvoisin.
 *   Les joints apparaissent dans la liste dans l'ordre croissant des PEs.
 *   Note:  Les faces que l'on va creer ensuite utilisent forcement
 *          des sommets que l'on va trouver ici.
 *   Propriete : Les sommets du joint sont classes dans l'ordre croissant
 *               de leur numero local, donc dans l'ordre croissant de
 *               leur numero global (voir Remplir_Numeros_Sommets)
 *
 */

void DomaineCutter::construire_sommets_joints_ssdom(const ArrOfInt& liste_sommets,
                                                    const ArrOfInt& liste_inverse_sommets,
                                                    const int partie,
                                                    const Static_Int_Lists* som_raccord,
                                                    Domaine& domaine_partie) const
{
  Joints& les_joints = domaine_partie.faces_joint();

  const int parts = nb_parties_;
  const IntVect& elem_part = ref_elem_part_.valeur();
  const Static_Int_Lists& som_elem = som_elem_;

  // Liste de sommets de joint (toutes parties confondues, un sommet peut
  // apparaitre plusieurs fois dans le tableau, une fois par pe voisin au maximum)
  // C'est le numero global du sommet
  ArrsOfInt joints_sommets(parts);
  {
    // Pour append_array :
    for (int i = 0; i < parts; i++)
      joints_sommets[i].set_smart_resize(1);
  }

  // Algorithme : boucle sur les sommets de la partie.
  // Pour chaque sommet du sous-domaine "p", on cherche les PEs auxquels appartiennent
  // les elements voisins du sommet, et, s'il y a un PE voisin different de "p",
  // on ajoute ce sommet aux joints avec ces PEs.

  const int nb_sommets = liste_sommets.size_array();

  int i_sommet;
  // i = indice local du sommet dans le sous-domaine
  for (i_sommet = 0; i_sommet < nb_sommets; i_sommet++)
    {
      // Numero global du sommet
      int sommet = liste_sommets[i_sommet];
      // Boucle sur les elements voisins du sommet i
      int nb_elements_voisins = som_elem.get_list_size(sommet);
      int i;
      for (i = 0; i < nb_elements_voisins; i++)
        {
          int elem = som_elem(sommet, i);
          int PEvoisin = elem_part[elem];
          // Faut-il ajouter ce sommet aux sommets du joint avec le PEvoisin ?
          if (PEvoisin != partie)
            joints_sommets[PEvoisin].append_array(sommet);
        }
      //boucle sur les procs connectes au sommet par un raccord
      if (som_raccord)
        for (i = 0; i < som_raccord->get_list_size(sommet); i++)
          if ((*som_raccord)(sommet, i) != partie) joints_sommets[(*som_raccord)(sommet, i)].append_array(sommet);
    }

  // Creation des joints : dans l'ordre croissant du PE voisin
  int PEvoisin;
  for (PEvoisin = 0; PEvoisin < parts; PEvoisin++)
    {
      ArrOfInt& sommets = joints_sommets[PEvoisin];
      if (sommets.size_array() > 0)
        {
          // Trier les sommets de joint par ordre croissant de l'indice global
          // (donc toutes les listes sur les processeurs voisins seront ordonnees
          //  de la meme facon) et retirer les doublons
          array_trier_retirer_doublons(sommets);
          const int nb_sommets2 = sommets.size_array();

          // On cree un nouveau joint
          Joint& joint = les_joints.add(Joint());
          Nom nom_joint("Joint_");
          Nom nom_numero(PEvoisin);
          nom_joint+=nom_numero;
          joint.nommer(nom_joint);
          joint.affecte_epaisseur(epaisseur_joint_);
          joint.associer_domaine(domaine_partie);
          joint.affecte_PEvoisin(PEvoisin);
          ArrOfInt& sommets_locaux = joint.set_joint_item(Joint::SOMMET).set_items_communs();
          sommets_locaux.resize_array(nb_sommets2);
          // Remplissage du tableau (transformation en indice local de sommet)
          for (int i = 0; i < nb_sommets2; i++)
            {
              const int indice_global = sommets[i];
              const int indice_local = liste_inverse_sommets[indice_global];

              assert(indice_local >= 0);
              sommets_locaux[i] = indice_local;
            }
        }
    }

}

// Recherche des faces de joints (faces adjacentes a deux elements appartenant
// a deux processeurs differents).
// On suppose que les sommets des joints ont ete construits.

void DomaineCutter::construire_faces_joints_ssdom(const int partie,
                                                  const DomaineCutter_Correspondance& correspondance,
                                                  Domaine& domaine_partie) const
{
  const int nb_sommets_ssdom = domaine_partie.nb_som();
  const int nb_elem_ssdom = domaine_partie.nb_elem();
  const ArrOfInt& liste_sommets = correspondance.get_liste_sommets();
  const ArrOfInt& liste_inverse_elements = correspondance.get_liste_inverse_elements();
  const Static_Int_Lists& som_elem = som_elem_;

  // Premiere etape: reperage des "elements de joint" (elements de la "partie"
  //  possedant un sommet de joint), et marquage des sommets de joint
  ArrOfInt liste_elements_joint;
  ArrOfBit drapeaux_sommets_joints(nb_sommets_ssdom);
  {
    // Pour l'instant, on utilise les joints en const...
    const Joints& joints_partie = domaine_partie.faces_joint();

    drapeaux_sommets_joints = 0;
    liste_elements_joint.set_smart_resize(1);

    // Marqueurs: pour chaque element du sous-domaine, est-il deja dans la liste ?
    ArrOfBit drapeaux_elements(nb_elem_ssdom);
    drapeaux_elements = 0;
    // Boucle sur tous les sommets de joint
    const int nb_joints = joints_partie.size();
    int i_joint;
    for (i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        const Joint& joint = joints_partie[i_joint];
        const ArrOfInt& sommets_du_joint = joint.joint_item(Joint::SOMMET).items_communs();
        const int n = sommets_du_joint.size_array();
        int i;
        for (i = 0; i < n; i++)
          {
            const int i_sommet_local = sommets_du_joint[i];
            const int i_sommet_global = liste_sommets[i_sommet_local];
            const int nb_elem_voisins = som_elem.get_list_size(i_sommet_global);
            // Marquage du sommet de joint dans le sous-domaine
            drapeaux_sommets_joints.setbit(i_sommet_local);
            int j;
            for (j = 0; j < nb_elem_voisins; j++)
              {
                const int i_elem_global = som_elem(i_sommet_global, j);
                const int i_elem_local = liste_inverse_elements[i_elem_global];
                // On ajoute l'element a la liste d'elements joint s'il est dans ma partie
                // et s'il n'est pas encore dans la liste.
                if (i_elem_local >= 0)
                  if (! drapeaux_elements.testsetbit(i_elem_local))
                    liste_elements_joint.append_array(i_elem_global);
              }
          }
      }
  }

  // Deuxieme etape: pour chaque element de joint, construction de ses
  // faces et on regarde si la face a un element voisin dans une autre partie.
  // Si oui, c'est une face de joint

  // Tableau des indices des sommets de toutes les faces de joint
  //  (tous joints confondus, indices de sommets dans le sous-domaine)
  IntTab faces_joints;
  // Pour chaque face de joint, numero du sous-domaine voisin par cette face
  ArrOfInt faces_pe_voisins;
  // Pour chaque processeur, nombre de faces de joint avec ce proc.
  // (on initialise a zero)
  ArrOfInt nb_faces_joints(nb_parties_);
  {
    faces_joints.set_smart_resize(1);
    faces_pe_voisins.set_smart_resize(1);

    // Indices des sommets des faces sur l'element de reference
    IntTab faces_element_reference;
    const Elem_geom_base& type_elem = domaine_partie.type_elem().valeur();
    int is_regular = type_elem.get_tab_faces_sommets_locaux(faces_element_reference);
    // On ne sait pas traiter les elements non reguliers
    if (!is_regular)
      {
        // Cerr << "DomaineCutter::faces_joints Error: non-regular elements not supported" << finl;
        ref_cast(Poly_geom_base,type_elem).get_tab_faces_sommets_locaux(faces_element_reference,0);
        //exit();
      }

    int nb_faces_elem       = faces_element_reference.dimension(0);
    const int nb_sommets_par_face = faces_element_reference.dimension(1);
    faces_joints.resize(0, nb_sommets_par_face); // Voir *suite*
    const ArrOfInt& liste_inverse_sommets = correspondance.get_liste_inverse_sommets();
    const IntVect& elem_part = ref_elem_part_.valeur();
    // Sommets des elements du maillage global
    const Domaine& domaine = ref_domaine_.valeur();
    const IntTab& elem_som = domaine.les_elems();

    ArrOfInt une_face(nb_sommets_par_face);
    ArrOfInt elements_voisins;
    elements_voisins.set_smart_resize(1);

    // Boucle sur les elements de joint
    const int nb_elem_joints = liste_elements_joint.size_array();
    int i_elem_joint;
    for (i_elem_joint = 0; i_elem_joint < nb_elem_joints; i_elem_joint++)
      {
        // Indice de l'element dans le domaine global
        const int i_elem_global = liste_elements_joint[i_elem_joint];
        // Boucle sur les faces de l'element
        int i_face;
        if (!is_regular)
          {
            // global ou non ????????
            ref_cast(Poly_geom_base,type_elem).get_tab_faces_sommets_locaux(faces_element_reference,liste_inverse_elements[i_elem_global]);
            nb_faces_elem       = faces_element_reference.dimension(0);
            while ( faces_element_reference(nb_faces_elem-1,0)==-1)
              nb_faces_elem--;
          }
        for (i_face = 0; i_face < nb_faces_elem; i_face++)
          {
            // Construction de la face
            int i;
            int face_ok = 1;
            for (i = 0; i < nb_sommets_par_face; i++)
              {
                const int i_ref = faces_element_reference(i_face, i);
                if (i_ref<0)
                  {
                    une_face[i] = i_ref;
                  }
                else
                  {
                    const int i_som = elem_som(i_elem_global, i_ref);
                    une_face[i] = i_som;
                    // Indice du sommet dans le sous-domaine

                    if (i_som>-1)
                      {
                        const int i_som_local = liste_inverse_sommets[i_som];
                        if(i_som_local>0)
                          {
                            // Le sommet est-il sur un joint ?
                            if (drapeaux_sommets_joints[i_som_local] == 0)
                              face_ok = 0; // Non => cette face n'est pas sur un joint
                          }
                      }
                  }
              }
            // Premier test pour eliminer tout de suite la face
            // si tous ses sommets ne sont pas des sommets de joint.
            if (face_ok)
              {
                // Recherche des elements voisins de cette face
                find_adjacent_elements(som_elem, une_face, elements_voisins);
                // Recherche d'un element voisin qui n'est pas dans ma partie
                const int nb_elem_voisins = elements_voisins.size_array();
                int PEvoisin = -1;
                for (i = 0; i < nb_elem_voisins; i++)
                  {
                    const int i_elem_glob = elements_voisins[i];
                    PEvoisin = elem_part[i_elem_glob];
                    if (PEvoisin != partie)
                      break;
                  }
                if (i < nb_elem_voisins)
                  {
                    // J'ai une face de joint, je l'ajoute au tableau
                    faces_pe_voisins.append_array(PEvoisin);
                    const int n = faces_joints.dimension(0);
                    faces_joints.resize(n+1, nb_sommets_par_face);
                    for (i = 0; i < nb_sommets_par_face; i++)
                      {
                        const int i_som_global = une_face[i];
                        if (i_som_global>-1)
                          {
                            const int i_som_local = liste_inverse_sommets[i_som_global];
                            faces_joints(n, i) = i_som_local;
                          }
                        else
                          faces_joints(n, i) = -1;
                      }
                    nb_faces_joints[PEvoisin]++;
                  }
              }
          }
      }
  }

  // Troisieme etape: ajout des faces dans les joints
  {
    // Le type des faces de joint
    const Domaine& domaine_globale = ref_domaine_.valeur();
    const Type_Face& type_face_joint = domaine_globale.type_elem().type_face();
    // On va modifier les joints:
    Joints& joints_partie = domaine_partie.faces_joint();
    const int nb_joints = joints_partie.size();
    int i_joint;

    const int nb_faces_joints_tot = faces_joints.dimension(0);
    // *suite* : tableau de dimension 2 meme s'il n'y a pas de faces
    const int nb_sommets_par_face = faces_joints.dimension(1);

    for (i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        // Typage des faces et allocation memoire
        Joint& joint = joints_partie[i_joint];
        joint.typer_faces(type_face_joint);
        joint.faces().les_sommets().resize(0, nb_sommets_par_face);
        const int PE_voisin = joint.PEvoisin();
        const int nb_faces = nb_faces_joints[PE_voisin];
        joint.dimensionner(nb_faces);
        // Remplissage du tableau de faces
        IntTab& faces = joint.faces().les_sommets();
        int i;
        int k = 0;
        for (i = 0; i < nb_faces_joints_tot; i++)
          {
            const int face_pe = faces_pe_voisins[i];
            if (face_pe == PE_voisin)
              {
                int j;
                for (j = 0; j < nb_sommets_par_face; j++)
                  faces(k, j) = faces_joints(i,j);
                k++;
              }
          }
      }
  }
}

/*! @brief Constructeur par defaut (structure vide)
 *
 */
DomaineCutter::DomaineCutter()
{
  nb_parties_ = -1;
  epaisseur_joint_ = -1;
}

/*! @brief annule toutes les references et vide les tableaux
 *
 */
void DomaineCutter::reset()
{
  ref_domaine_.reset();
  ref_elem_part_.reset();
  nb_parties_ = -1;
  epaisseur_joint_ = -1;
  som_elem_.reset();
}

void  calculer_listes_elements_sous_domaines(const IntVect& elem_part,
                                             const int nb_parts,
                                             const int nbelem,
                                             Static_Int_Lists& liste_elems_sous_domaines)
{
  ArrOfInt sizes(nb_parts);
  // Premier passage : comptage
  int i;
  for (i = 0; i < nbelem; i++)
    {
      const int part = elem_part[i];
      sizes[part]++;
    }
  liste_elems_sous_domaines.set_list_sizes(sizes);
  // Deuxieme passage : remplissage
  sizes = 0;
  for (i = 0; i < nbelem; i++)
    {
      const int part = elem_part[i];
      const int index = sizes[part]++;
      liste_elems_sous_domaines.set_value(part, index, i);
    }
}

void calculer_elements_voisins_bords(const Domaine& dom,
                                     const Static_Int_Lists& som_elem,
                                     Static_Int_Lists& voisins,const IntVect& elem_part, const int permissif, Noms& bords_a_pb_)
{
  const Domaine& domaine = dom;
  const int nb_front = domaine.nb_front_Cl();
  ArrOfInt nb_faces(nb_front);
  for (int i = 0; i < nb_front; i++)
    nb_faces[i] = domaine.frontiere(i).nb_faces();
  voisins.set_list_sizes(nb_faces);
  const int nb_som_face = domaine.frontiere(0).faces().nb_som_faces();
  ArrOfInt une_face(nb_som_face);
  ArrOfInt elems_voisins;
  elems_voisins.set_smart_resize(1);
  for (int i = 0; i < nb_front; i++)
    {
      int drap=0;
      const IntTab& faces = domaine.frontiere(i).les_sommets_des_faces();
      const int n = nb_faces[i];
      for (int j = 0; j < n; j++)
        {
          for (int k = 0; k < nb_som_face; k++)
            une_face[k] = faces(j, k);
          find_adjacent_elements(som_elem, une_face, elems_voisins);
          const int n_voisins = elems_voisins.size_array();
          if (n_voisins != 1)
            {
              if (drap==0)
                {
                  if (permissif)
                    Cerr << "Message from DomaineCutter.cpp : calculer_elements_voisins_bords\n"
                         << " boundary face "<<  domaine.frontiere(i).le_nom()<<" with " << n_voisins << " neighbors." << finl;
                  else
                    Cerr << "Error in DomaineCutter.cpp : calculer_elements_voisins_bords\n"
                         << " boundary face "<<  domaine.frontiere(i).le_nom()<<" with " << n_voisins << " neighbors." << finl;
                  bords_a_pb_.add( domaine.frontiere(i).le_nom());
                  if (elems_voisins.size_array()==0) Process::exit();
                }
              drap=1;
              if (elem_part[elems_voisins[1]]==1)
                elems_voisins[0]=elems_voisins[1];
              if (permissif==0)
                Process::exit();
            }
          voisins.set_value(i, j, elems_voisins[0]);
        }
    }
}

/*! @brief Prepare les structures de donnees pour la construction des sous-domaines en fonction d'un decoupage fourni dans elem_part.
 *
 * @param (domaine_global) le domaine a decouper (doit avoir un domaine). On prend une ref a ce domaine (il doit donc rester valide).
 * @param (elem_part) pour chaque element, numero du sous-domaine auquel il appartient, avec 0 <= elem_part[i] < nb_parts. Attention, on prend une ref a ce tableau, on ne fait pas une copie local. Le tableau doit continuer a exister jusqu'a ce qu'on a fini d'utiliser DomaineCutter.
 * @param (nb_parts) nombre total de sous-domaines
 * @param (les_faces)
 * @param (epaisseur_joint)
 */
void DomaineCutter::initialiser(const Domaine&   domaine_global,
                                const IntVect& elem_part,
                                const int     nb_parts,
                                const int     epaisseur_joint,
                                const Noms&      liste_bords_periodiques,
                                const int permissif)
{
  assert(nb_parts >= 0);
  assert(elem_part.size_array() == domaine_global.nb_elem_tot());
  assert(max_array(elem_part) < nb_parts);
  if (min_array(elem_part)<0)
    {
      Cerr << "Error in DomaineCutter::initialiser" << finl;
      Cerr << "The built partition has a default." << finl;
      Cerr << "Try to change the partition tool or try changing some partitioning options." << finl;
      Cerr << "Contact TRUST support if you are still unsuccessful with your tries." << finl;
      Process::exit();
    }
  reset();

  ref_domaine_ = domaine_global;
  ref_elem_part_ = elem_part;
  nb_parties_ = nb_parts;
  epaisseur_joint_ = epaisseur_joint;
  liste_bords_periodiques_ = liste_bords_periodiques;

  const IntTab& elems = domaine_global.les_elems();
  const int nb_som = domaine_global.nb_som_tot();
  construire_connectivite_som_elem(nb_som, elems, som_elem_,
                                   1 /* inclure les elems virtuels */);

  Cerr << "Search of neighboring elements of boundary faces" << finl;
  calculer_elements_voisins_bords(domaine_global, som_elem_, voisins_bords_,elem_part,permissif,bords_a_pb_);
  Cerr << "Construction of lists of elements per subdomain" << finl;
  calculer_listes_elements_sous_domaines(elem_part, nb_parts, elems.dimension(0), liste_elems_sous_domaines_);
  //calculer_listes_elements_sous_domaines(elem_part, nb_parts, liste_elems_sous_domaines_);

}

/*! @brief Remplit la structure "correspondance" et le "sous_domaine" pour la partie "part".
 *
 * On cherche les sommets qui appartiennent a la sous-partie,
 *   on calcule les tableaux de renumerotation des sommets et des elements entre numero
 *   global et numero local (structure correspondance), on remplit les structures de
 *   sous_domaine (sommets, elements, frontieres, joints, etc).
 *   Attention: sous_domaine doit etre un objet vierge (ne pas contenir de domaine)
 *
 */
void DomaineCutter::construire_sous_domaine(const int part, DomaineCutter_Correspondance& correspondance, Domaine& sous_domain, const Static_Int_Lists *som_raccord) const
{
  // L'objet doit etre initialise:
  assert(nb_parties_ >= 0);
  // sous_domaine vide
  assert(sous_domain.nb_elem() == 0);
  // Numero de partie valide
  assert(part >= 0 && part < nb_parties_);

  correspondance.partie_ = part;

  const Domaine& domain = ref_domaine_.valeur();

  ArrOfInt elements_sous_partie;
  liste_elems_sous_domaines_.copy_list_to_array(part, elements_sous_partie);

  // Preparation du sous_domaine
  // sous_domaine.reset(); /* reset n'existe pas encore... */
  sous_domain.nommer(domain.le_nom());
  if (sub_type(Poly_geom_base, domain.type_elem().valeur()))
    ref_cast(Poly_geom_base,domain.type_elem().valeur()).build_reduced(sous_domain.type_elem(), elements_sous_partie);
  else
    sous_domain.type_elem() = domain.type_elem();
  sous_domain.type_elem().associer_domaine(sous_domain);

  construire_liste_sommets_sousdomaine(domain.nb_som_tot(), domain.les_elems(), elements_sous_partie, part,
                                       som_raccord, correspondance.liste_sommets_ /* write */,
                                       correspondance.liste_inverse_sommets_ /* write */);

  remplir_coordsommets_sous_domaine(domain.coord_sommets(), correspondance.liste_sommets_, sous_domain.les_sommets() /* write */);
  construire_elems_sous_domaine(domain.les_elems(), elements_sous_partie, correspondance.liste_inverse_sommets_, sous_domain.les_elems() /* write */,
                                correspondance.liste_inverse_elements_ /* write */);
  {
    const ArrOfInt& l_som = correspondance.liste_sommets_;
    const ArrOfInt& l_inv_som = correspondance.liste_inverse_sommets_;
    construire_faces_bords_ssdom(l_inv_som, part, sous_domain);
    construire_faces_raccords_ssdom(l_inv_som, part, sous_domain);
    construire_frontieres_internes_ssdom(l_inv_som, part, sous_domain);
    construire_groupe_faces_ssdom(l_inv_som, part, sous_domain);
    construire_sommets_joints_ssdom(l_som, l_inv_som, part, som_raccord, sous_domain);
  }

  construire_faces_joints_ssdom(part, correspondance, sous_domain);

  //if som_raccord is used (for DecouperMulti), then construire_elements_distants_ssdom()
  //can lead to the creation of empty joints -> it must not be called
  int compute_items_distants = Decouper::print_more_infos && !som_raccord; // To print NbElemDist informations
  if (compute_items_distants)
    {
      // Cet algorithme sequentiel n'est pas utilise en temps normal, sauf si
      // on veut tester l'algorithme parallele dans Scatter.cpp
      // voir  CHECK_ALGO_ESPACE_VIRTUEL dans Scatter.cpp (Benoit Mathieu)
      construire_elements_distants_ssdom(part, correspondance.liste_sommets_, correspondance.liste_inverse_elements_, sous_domain);
    }
  else
    {
      // Initialiser des joints vides (sinon assert en debug car les joints ne sont pas initialises)
      for (int ij = 0; ij < sous_domain.nb_joints(); ij++)
        {
          Joint& joint = sous_domain.joint(ij);
          joint.set_joint_item(Joint::ELEMENT).set_items_distants();
        }
    }
  Scatter::trier_les_joints(sous_domain.faces_joint());
}

/*! @brief Build the name of the ".
 *
 * Domaines" file for a given proc and a domain. If partie == -1 a single filename is returned. For example
 *           DOM.Zones
 *    instead of
 *           DOM_0001.Zones
 *
 */
static void construire_nom_fichier_sous_domaine(const Nom& basename,
                                                const int partie, const int nb_parties_,
                                                const int original_proc,
                                                Nom& fichier)
{
  fichier = basename;

  if (partie > 100000)
    {
      Cerr << "Error while generating filename: nb_parties_ too large" << finl;
      Process::exit();
    }
  char s[30];
  // single file name for all procs (HDF5)
  // the number of parts is still included in the file name
  // (make_PAR.data needs to know how many domaines there are)
  if (partie < 0)
    {
      if (nb_parties_ > 10000)
        snprintf(s, 30, "_p%05d.Zones", (True_int)nb_parties_);
      else
        snprintf(s, 30, "_p%04d.Zones",(True_int) nb_parties_);
    }
  else
    {
      if (nb_parties_ > 10000)
        {
          if(original_proc < 0)
            snprintf(s, 30, "_%05d.Zones", (True_int)partie);
          else
            snprintf(s, 30, "_%05d_%d.Zones", (True_int)partie, (True_int)original_proc);
        }
      else
        {
          if(original_proc < 0)
            snprintf(s, 30, "_%04d.Zones", (True_int)partie);
          else
            snprintf(s, 30, "_%04d_%d.Zones", (True_int)partie, (True_int)original_proc);
        }
    }
  fichier += Nom(s);
}


void DomaineCutter::writeData(const Domaine& sous_domaine, Sortie& os) const
{
  os << sous_domaine;
  // Benoit Mathieu: Scatter a besoin de la liste des bords periodiques pour le
  // calcul des elements distants. Je l'ecris apres le domaine dans le fichier .Zones.
  // Pas genial mais c'est pour depanner en attendant mieux.
  os << liste_bords_periodiques_;
}

/*! @brief Generation de tous les sous-domaines du calcul et ecriture sur disque des fichiers basename_000n.
 *
 * Domaines pour 0 <= n < nb_parties_.
 *   Si des "sous-domaines" sont definies (dans le champ domaine.ss_domaines()),
 *   on genere aussi un fichier par sous-domaine.
 *
 */
void DomaineCutter::ecrire_domaines(const Nom& basename, const Decouper::DomainesFileOutputType format, IntVect& elem_part, const int reorder, const Static_Int_Lists* som_raccord)
{
  assert(nb_parties_ >= 0);
  const Domaine& domaine = ref_domaine_.valeur();
  DomaineCutter_Correspondance dc_correspondance;

  // Needed for HDF5 Domaines output:
#ifdef MPI_
  FichierHDFPar fic_hdf;
#else
  FichierHDF fic_hdf;
#endif
  Nom nom_fichier_hdf5(basename);
  nom_fichier_hdf5+=Nom(".Zones");
  nom_fichier_hdf5 = nom_fichier_hdf5.nom_me(nb_parties_, "p", 1);

  // Build temp arrays to eventually reorder the partition numbering
  ArrOfInt ia(nb_parties_+1);
  ArrOfInt ja;
  ja.set_smart_resize(1);
  int nnz=0;
  ia[0]=1;

  //To detect my parts (when running in parallel)
  ArrOfInt myDomaines(nb_parties_);
  myDomaines = 0;
  ArrsOfInt otherProcDomaines(Process::nproc());

  //if some domaines are splitted between multiple procs,
  //we assign consecutive indices to each of its fragment
  //(reading the .Zones files during Scatter will be more efficient)
  // Possible values for domaines_index[part]:
  // -2    : means that part is detained by multiple procs but not by me
  // -1    : means that part is detained by a single proc
  // i >=0 : means that my proc detains the i-th fragment of part
  ArrOfInt domaines_index(nb_parties_);
  domaines_index = -2;

  const int nbelem = domaine.nb_elem();

  Cerr << "Generation of " << nb_parties_ << " parts:" << finl;
  IntVect EdgeCut(nb_parties_);
  ArrsOfInt Neighbours(nb_parties_);
  // 2 loops if reorder=1
  for (int loop=0; loop<1+reorder; loop++)
    {
      if (reorder)
        {
          Cerr << "====================================" << finl;
          if (loop==0)
            Cerr << "FIRST PASS, analyzing the partition:" << finl;
          else
            Cerr << "SECOND PASS, after reordering the partition:" << finl;
          Cerr << "====================================" << finl;
        }

      if(loop == reorder)
        {
          // check to see which part is shared between multiple processors:
          // 1- everyone sends the number of the parts that belong to them to the master process
          // 2- master process will assign a unique positive number to each fragment of a shared domaine
          // 3- if a part is owned by a single process, it is indicated with the index -1
          // 4- the master process scatters the indices to all the proc
          ArrsOfInt domaines_indices(Process::nproc());

          for(int i=0; i < nbelem; i++)
            {
              const int part = elem_part[i];
              myDomaines[part] = 1;
            }

          for(int p=0; p<Process::nproc(); p++)
            {
              if(p==0)
                otherProcDomaines[p] = myDomaines;
              else
                {
                  otherProcDomaines[p].resize_array(nb_parties_);
                  otherProcDomaines[p] = 0;
                }
            }
          if(Process::je_suis_maitre())
            {
              for(int p=0; p<Process::nproc(); p++)
                {
                  domaines_indices[p].resize_array(nb_parties_);
                  domaines_indices[p] = -2;
                  if(p!=0)
                    recevoir(otherProcDomaines[p], p, 0, p+2001);
                }

              for(int part=0; part<nb_parties_; part++)
                {
                  int s = 0;
                  for(int proc=0; proc < Process::nproc(); proc++)
                    {
                      if(otherProcDomaines[proc][part])
                        domaines_indices[proc][part] = s++;
                    }

                  if(s<=1)
                    {
                      if(s==0)   //empty part: master process will write it
                        myDomaines[part] = 1;

                      //part is detained by a single proc
                      for(int proc=0; proc < Process::nproc(); proc++)
                        domaines_indices[proc][part] = -1;
                    }
                }

              for(int p=0; p<Process::nproc(); p++)
                {
                  if(p==0)
                    domaines_index = domaines_indices[p];
                  else
                    envoyer(domaines_indices[p], 0, p, p+2002);
                }
            }
          else
            {
              envoyer(myDomaines, Process::me(), 0, Process::me()+2001);
              recevoir(domaines_index, 0, Process::me(), Process::me()+2002);
            }

          if (format == Decouper::HDF5_SINGLE)  // create HDF5 file only once!
            {
              fic_hdf.create(nom_fichier_hdf5);
              if(Process::nproc() > 1)
                {
                  // creating datasets
                  Noms dataset_names;
                  if(Process::je_suis_maitre())
                    {
                      for(int part=0; part<nb_parties_; part++)
                        {
                          if(domaines_index[part] == -1)
                            {
                              std::string dname = "/zone_"  + std::to_string(part);
                              Nom dataset_name(dname);
                              dataset_names.add(dataset_name);
                            }
                          else
                            {
                              for(int proc=0; proc < Process::nproc(); proc++)
                                {
                                  if(domaines_indices[proc][part] >=0)
                                    {
                                      std::string dname = "/zone_"  + std::to_string(part) + "_" + std::to_string(domaines_indices[proc][part]);
                                      Nom dataset_name(dname);
                                      dataset_names.add(dataset_name);
                                    }
                                }
                            }
                        }

                    }

                  // estimation of an upper bound of the datasets' size
                  int ipart = 0;
                  while(!myDomaines[ipart]) ipart++;
                  Domaine dom_tmp;
                  construire_sous_domaine(ipart, dc_correspondance, dom_tmp);
                  Sortie_Brute os_tmp;
                  writeData(dom_tmp, os_tmp);
                  double sz_ = (double)os_tmp.get_size();
                  sz_ *= 1.5;
                  sz_ = Process::mp_max(sz_);
                  envoyer_broadcast(dataset_names,0);
                  long sz_l = lround(sz_);
                  fic_hdf.create_datasets(dataset_names, sz_l);
                }
            }

        }
      for (int i_part = 0; i_part < nb_parties_; i_part++)
        {
          if(!myDomaines[i_part])
            continue;

          assert(domaines_index[i_part] > -2);
          Cerr << " Construction of part number " << i_part << finl;
          if(domaines_index[i_part] >= 0)
            Cerr << "This part is shared between multiple processors" << finl;

          Domaine sous_domaine;
          construire_sous_domaine(i_part, dc_correspondance, sous_domaine, som_raccord);
          // On affiche quelques informations...
          {
            const Joints& joints = sous_domaine.faces_joint();
            const int nb_joints = joints.size();
            Cerr << "  Number of nodes    : " << sous_domaine.nb_som() << finl;
            Cerr << "  Number of elements : " << sous_domaine.nb_elem() << finl;
            Cerr << "  Number of joints   : " << nb_joints << finl;
            //            char s[200];
            int nbfaces_total=0;
            int nbelemdist_total=0;
            int nbsom_total=0;
            Neighbours[i_part].resize_array(0);
            Neighbours[i_part].resize_array(nb_joints);
            for (int i = 0; i < nb_joints; i++)
              {
                const Joint& joint = joints[i];
                const int pe = joint.PEvoisin();
                Neighbours[i_part][i] = pe;
                const int nbsom = joint.joint_item(Joint::SOMMET).items_communs().size_array();
                nbsom_total+=nbsom;
                const int nbfaces = joint.nb_faces();
                nbfaces_total+=nbfaces;
                const int nbelemdist = joint.joint_item(Joint::ELEMENT).items_distants().size_array();
                nbelemdist_total+=nbelemdist;
                // Beurk: snprintf pas beau et dangereux, mais c'est plus simple...
                assert(sizeof(int)==sizeof(int));
                Cerr<< "  Joint "<<i<<" PeVoisin "<<pe<<" NbSommets "<<nbsom<<" NbFaces "<<nbfaces;
                if (Decouper::print_more_infos) Cerr <<" NbElemDist "<<nbelemdist;
                Cerr<<finl;
              }
            Cerr<<"               Total:    NbSommets "<<nbsom_total<<" NbFaces "<<nbfaces_total;
            if (Decouper::print_more_infos) Cerr<<" NbElemDist "<<nbelemdist_total;
            EdgeCut(i_part)=nbfaces_total;
            Cerr<<finl;

          }
          if (reorder && loop==0)
            {
              const Joints& joints = sous_domaine.faces_joint();
              const int nb_joints = joints.size();
              // Resize ja:
              ja.resize_array(nnz+nb_joints+1);
              // Fortran numerotation:
              ja[nnz]=i_part+1;
              nnz++;
              ia[i_part+1]=ia[i_part]+1;
              for (int i = 0; i < nb_joints; i++)
                {
                  const int pe = joints[i].PEvoisin();
                  ja[nnz] = pe+1;
                  nnz++;
                  ia[i_part+1]++;
                }
            }
          else
            {
              // Write .Zones file(s):
              if (format == Decouper::BINARY_MULTIPLE)
                {
                  Nom nom_fichier(basename);
                  nom_fichier+=Nom(".Zones");
                  //nom_fichier = nom_fichier.nom_me(i_part);
                  construire_nom_fichier_sous_domaine(basename,i_part, nb_parties_, domaines_index[i_part], nom_fichier);
                  Cerr << "Writing part " << i_part << " into the "
                       << (format == Decouper::BINARY_MULTIPLE ? "binary" : "ascii")
                       << " file " << nom_fichier << finl;
                  SFichier os;
                  if (format == Decouper::BINARY_MULTIPLE)
                    os.set_bin(1);
                  const int ok = os.ouvrir(nom_fichier);
                  if (!ok)
                    {
                      Cerr << "DomaineCutter::ecrire_domaines : Error while opening file " << finl;
                      exit();
                    }
                  writeData(sous_domaine, os);
                }
              else if (format == Decouper::HDF5_SINGLE)
                {
                  Sortie_Brute os_hdf;
                  writeData(sous_domaine, os_hdf);

                  std::string dname = "/zone_" + std::to_string(i_part);
                  if(domaines_index[i_part] >=0)
                    dname += "_" + std::to_string(domaines_index[i_part]);
                  Nom datasetname(dname);
                  if(Process::nproc() > 1)
                    fic_hdf.fill_dataset(datasetname, os_hdf);
                  else
                    fic_hdf.create_and_fill_dataset_SW(datasetname, os_hdf);

                }
              else
                {
                  Cerr << "DomaineCutter::ecrire_domaines : Unsupported output file type!" << finl;
                  Process::exit(1);
                }
            }

          // Ecritures des fichiers sous-domaines .ssz
          const LIST(REF(Sous_Domaine)) & liste_sous_domaines = domaine.ss_domaines();
          const int nb_sous_domaines = liste_sous_domaines.size();
          if (nb_sous_domaines>0)
            {
              Cerr << " Writing of files .ssz ..." << finl;
              // Un fichier par sous-domaine, dans chaque fichier on trouve toutes les parties...
              // Si on est en train d'ecrire la premiere partie, on efface le fichier
              IOS_OPEN_MODE mode;
              if (i_part == 0)
                mode = ios::out; // Premiere partie, on ecrase le fichier
              else
                mode = (ios::out | ios::app); // Mode append

              // Boucle sur les sous-domaines
              const ArrOfInt& liste_inverse_elements = dc_correspondance.liste_inverse_elements_;

              for (int i_sous_domaine = 0; i_sous_domaine < nb_sous_domaines; i_sous_domaine++)
                {
                  // Indices des elements du sous-domaine qui sont dans le sous-domaine:
                  const Sous_Domaine& sous_dom = liste_sous_domaines[i_sous_domaine];
                  ArrOfInt elements;
                  {
                    const int n = sous_dom.nb_elem_tot();
                    elements.set_smart_resize(1);
                    int i;
                    for (i = 0; i < n; i++)
                      {
                        const int i_elem_global = sous_dom[i];
                        const int i_elem_local = liste_inverse_elements[i_elem_global];
                        if (i_elem_local >= 0)
                          elements.append_array(i_elem_local);
                      }
                  }
                  Nom nom_fichier(sous_dom.le_nom() + Nom(".ssz"));
                  Cerr << " Subarea " << i_sous_domaine
                       << "  file_name " << nom_fichier
                       << "  nb_elements in this part " << elements.size_array()
                       << finl;
                  // Le fichier sous-domaines est toujours en ascii
                  SFichier os;
                  const int ok = os.ouvrir(nom_fichier, mode);
                  if (!ok)
                    {
                      Cerr << "DomaineCutter::ecrire_domaines :\n Error while opening file"
                           << nom_fichier << finl;
                      exit();
                    }
                  os << elements;
                }
            }
        }
      if (reorder && loop==0)
        {
          // Reduce the bandwith of a the matrix connectivity between parts:
          // ToDo forcer a ne pas changer la partition 0 !
          ArrOfInt riord(nb_parties_+1);
          ArrOfInt levels(nb_parties_+1);
          ArrOfInt mask(nb_parties_+1);
          int maskval = 1;
          for (int i_part=0 ; i_part<nb_parties_+1; i_part++)
            mask[i_part] = maskval;
          int init=1;
          int nlev;
          F77NAME(PERPHN)(&nb_parties_, ja.addr(), ia.addr(), &init, mask.addr(), &maskval, &nlev, riord.addr(), levels.addr());

          // Renumber the elem_part array:
          ArrOfInt renum(nb_parties_);
          for (int i_part=0; i_part < nb_parties_; i_part++)
            renum[riord[i_part]-1]=i_part;
          int size=elem_part.size_reelle();
          for (int i=0; i<size; i++)
            elem_part[i]=renum[elem_part[i]];
          elem_part.echange_espace_virtuel();
          // Rebuild the liste_elems_sous_domaines_
          liste_elems_sous_domaines_.reset();
          calculer_listes_elements_sous_domaines(elem_part, nb_parties_, nbelem, liste_elems_sous_domaines_);
        }

      /*
      // Print statistics exactly as Fluent:
      // http://combust.hit.edu.cn:8080/fluent/Fluent60_help/html/ug/node984.htm
      Cerr << ">> Partitions:" << finl;
      Cerr << "
               P   Cells I-Cells Cell Ratio  Faces I-Faces Face Ratio Neighbors" << finl;

               0     134      10      0.075    217      10      0.046         1
               1     137      19      0.139    222      19      0.086         2
               2     134      19      0.142    218      19      0.087         2
               3     137      10      0.073    223      10      0.045         1
      Cerr << "------" << finl;
      Cerr << "Partition count             = " << nb_parties_ << finl;
      Cerr << "Cell variation              = (134 - 138)
      Cerr << "Mean cell variation         = (  -1.1% -    1.1%)
      Cerr << "Intercell variation         = (10 - 19)
      Cerr << "Intercell ratio variation   = (   7.3% -   14.2%);
      Cerr << "Global intercell ratio      =   10.7%
      Cerr << "Face variation              = (217 - 223)
      Cerr << "Interface variation         = (10 - 19)
      Cerr << "Interface ratio variation   = (   4.5% -    8.7%)
      Cerr << "Global interface ratio      =    3.4%
      Cerr << "Neighbor variation          = (1 - 2) */
    }

  // if my part is shared with other procs, then my whole part may contain joints that I don't have
  // calling mp_sum wouldn't be correct though, because the shared joints would be counted several times
  // so we need to gather neighbours of each part
  if (Decouper::print_more_infos)   // involves communication: do it only if requested
    {
      if(Process::je_suis_maitre())
        {
          for(int proc=1; proc<Process::nproc(); proc++)
            {
              for(int i_part=0; i_part<nb_parties_; i_part++)
                {
                  if(otherProcDomaines[proc][i_part])
                    {
                      ArrOfInt tmp_neighbours;
                      tmp_neighbours.set_smart_resize(1);
                      recevoir(tmp_neighbours, proc, 0, proc+2003);

                      Neighbours[i_part].set_smart_resize(1);
                      for(int i=0; i<tmp_neighbours.size_array(); i++)
                        Neighbours[i_part].append_array(tmp_neighbours[i]);
                    }
                }
              ArrOfInt tmp_edge_cut(nb_parties_);
              tmp_edge_cut = 0;
              recevoir(tmp_edge_cut, proc, 0, proc+2008);

              for(int i_part=0; i_part<nb_parties_; i_part++)
                EdgeCut(i_part) += tmp_edge_cut[i_part];

            }

          Cout << "\nQuality of partitioning --------------------------------------------" << finl;
          int total_edge_cut = 0;
          for (int i_part=0; i_part<nb_parties_; i_part++)
            total_edge_cut+=EdgeCut(i_part);
          Cout << "Total number of edge-cut (faces shared by processes) : " << total_edge_cut << finl;
          if (total_edge_cut>0)
            {
              double mean_edgecut_domaine = total_edge_cut / nb_parties_;
              int max_edgecut_domaine = local_min_vect(EdgeCut);
              int min_edgecut_domaine = local_max_vect(EdgeCut);

              double load_imbalance = double(max_edgecut_domaine / mean_edgecut_domaine);
              Cout << "Number of edge-cut per Domaine (min/mean/max) : " << min_edgecut_domaine  << " / "
                   << (int) (mean_edgecut_domaine) << " / " << max_edgecut_domaine  << " Load imbalance: " << load_imbalance
                   << "\n" << finl;
            }

          int mean_neighbours = 0;
          int min_neighbours = nb_parties_;
          int max_neighbours = 0;
          for (int i_part = 0; i_part < nb_parties_; i_part++)
            {
              Neighbours[i_part].array_trier_retirer_doublons();
              int nb_neighbours = Neighbours[i_part].size_array();
              mean_neighbours += nb_neighbours;
              if(nb_neighbours < min_neighbours)   min_neighbours = nb_neighbours;
              if(nb_neighbours > max_neighbours)   max_neighbours = nb_neighbours;
            }

          mean_neighbours/=nb_parties_;
          if (mean_neighbours>0)
            {
              double load_imbalance = double(max_neighbours / mean_neighbours);
              Cout << "Number of neighbours per Domaine (min/mean/max) : " << min_neighbours << " / "
                   << (int) (mean_neighbours) << " / " <<max_neighbours << " Load imbalance: " << load_imbalance
                   << "\n" << finl;
            }
        }
      else
        {
          for(int i_part=0; i_part < nb_parties_; i_part++)
            if(myDomaines[i_part])
              envoyer(Neighbours[i_part], Process::me(), 0, Process::me()+2003);
          envoyer(EdgeCut, Process::me(), 0, Process::me()+2008);
        }
    }

  if (format == Decouper::HDF5_SINGLE)
    fic_hdf.close();

}
