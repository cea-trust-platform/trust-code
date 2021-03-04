/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Partitionneur_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////
#include <Partitionneur_base.h>
#include <Domaine.h>
#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <Reordonner_faces_periodiques.h>
#include <ArrOfBit.h>
#include <Array_tools.h>
#include <Param.h>
#include <IntLists.h>
#include <communications.h>

Implemente_deriv(Partitionneur_base);

Implemente_base(Partitionneur_base,"Partitionneur_base",Objet_U);

Sortie& Partitionneur_base::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Partitionneur_base::readOn(Entree& is)
{
  Cerr << "Reading parameters of " << que_suis_je() << finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Partitionneur_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
  exit();
  return -1;
}

void Partitionneur_base::declarer_bords_periodiques(const Noms& noms_bords_periodiques)
{
  liste_bords_periodiques_ = noms_bords_periodiques;
}

// Description: corrige la partition pour que l'element 0 du domaine initial
//  se trouve sur le premier sous-domaine de la partition.
//  On echange le premier sous-domaine et celui qui contient l'element 0.
// ToDo: is it necessary ? involves communication in parallel...
void Partitionneur_base::corriger_elem0_sur_proc0(ArrOfInt& elem_part)
{
  Cerr << "Correction of the splitting to put the element 0 on processor 0." << finl;
  int pe_to_xchange =  elem_part[0];
  envoyer_broadcast(pe_to_xchange, 0);
  if (pe_to_xchange == 0)
    {
      Cerr << " No correction to be made" << finl;
      return;
    }

  Cerr << " Exchange of parts 0 and " << pe_to_xchange << finl;
  const int n = elem_part.size_array();
  for (int i = 0; i < n; i++)
    {
      const int pe = elem_part[i];
      if (pe == 0)
        elem_part[i] = pe_to_xchange;
      else if (pe == pe_to_xchange)
        elem_part[i] = 0;
    }
}

// Description: construction (taille et contenu) du tableau elements avec
//  pour chaque face du bord donne, l'indice de l'element de la zone adjacent
//  a cette face.
// Parametre: som_elem
// Signification: connectivite sommet-elements de la zone, calculee a l'aide
//  de construire_connectivite_som_elem
// Parametre: faces
// Signification: les faces du bord a traiter (pour chaque face, indices des sommets)
// Parametre: nom_faces
// Signification: un nom de bord a imprimer en cas d'erreur
// Parametre: elements
// Signification: le tableau a remplir.
static void chercher_elems_voisins_faces(const Static_Int_Lists& som_elem,
                                         const IntTab& faces,
                                         const Nom& nom_faces,
                                         ArrOfInt& elements)
{
  const int nb_faces = faces.dimension(0);
  elements.resize_array(nb_faces);
  if (nb_faces == 0)
    return;
  const int nb_som_faces = faces.dimension(1);
  ArrOfInt une_face(nb_som_faces);
  ArrOfInt voisins;
  for (int i = 0; i < nb_faces; i++)
    {
      for (int j = 0; j < nb_som_faces; j++)
        une_face[j] = faces(i, j);
      find_adjacent_elements(som_elem, une_face, voisins);
      const int nb_voisins = voisins.size_array();
      if (nb_voisins != 1)
        {
          Cerr << "Error in chercher_elems_voisins_faces : the face " << i
               << "\n of boundary " << nom_faces << " has " << nb_voisins
               << " neighboring elements with the indices : " << voisins << finl;
          Process::exit();
        }
      elements[i] = voisins[0];
    }
}

// Description:
//  Calcul d'un graphe de connectivite entre les elements lies par des faces periodiques.
//  Si l'element i est voisin de l'element j par une face periodique, alors il existe
//  k tel que graph(i,k)==j et il existe k2 tel que graph(j,k2)==i.
// Parametre: zone
// Signification: la zone a traiter
// Parametre: liste_bords_periodiques
// Signification: liste des noms des bords periodiques. ATTENTION: on suppose que les
//  faces des bords periodiques sont rangees selon la convention des bords periodiques.
//  Voir check_faces_periodiques().
// Parametre: som_elem
// Signification: la connectivite sommets-elements pour la zone donnee.
// Parametre: graph
// Signification: On y stocke le resultat.
// Valeur de retour: nombre d'elements dans le graphe (egal au nombre de faces periodiques)
int Partitionneur_base::calculer_graphe_connexions_periodiques(const Zone& zone,
                                                               const Noms& liste_bords_periodiques,
                                                               const Static_Int_Lists& som_elem,
                                                               const int my_offset,
                                                               Static_Int_Lists& graph)
{
  const int nb_elem = zone.nb_elem();

  // Pour chaque element, combient a-t-il de faces periodiques ?
  ArrOfInt nb_faces_perio(nb_elem);
  // Liste de correspondances element0 <=> element1
  // entre l'element voisin d'une face et l'element voisin de la face periodique opposee
  IntTab correspondances(0,2);
  correspondances.set_smart_resize(1);

  // Premiere etape: remplissage de nb_faces_perio et correspondances
  // Parcours des bords periodiques
  const int nb_bords = zone.nb_bords();
  for (int i_bord = 0; i_bord < nb_bords; i_bord++)
    {
      const Bord& bord = zone.bord(i_bord);
      if (!liste_bords_periodiques.contient_(bord.le_nom()))
        continue;
      Cerr << " Checking of the boundary " << bord.le_nom();
      {
        ArrOfDouble delta;
        ArrOfDouble erreur;
        const int ok = Reordonner_faces_periodiques::check_faces_periodiques(zone.bord(i_bord),
                                                                             delta,
                                                                             erreur);
        const int d = delta.size_array();
        Cerr << " Delta = ";
        int i;
        for (i = 0; i < d; i++) Cerr << delta[i] << " ";
        Cerr << " Error = ";
        for (i = 0; i < d; i++) Cerr << erreur[i] << " ";
        Cerr << finl;
        if (!ok)
          {
            Cerr << "You need to use the Corriger_frontiere_periodique keyword on the periodic boundaries." << finl;
            Cerr << "See the reference manual to use this keyword on your data file." << finl;
            exit();
          }
      }
      ArrOfInt elems_voisins;
      chercher_elems_voisins_faces(som_elem,
                                   bord.faces().les_sommets(),
                                   bord.le_nom(),
                                   elems_voisins);

      // Parcours des faces du bord periodique, 2 a 2.
      // On suppose que les faces apparaissent dans l'ordre:
      //  d'abord toutes les faces d'une extremite du domaine,
      //  puis, dans le meme ordre, les faces de l'autre extremite.
      assert(bord.nb_faces() % 2 == 0); // Nombre pair, forcement
      const int nb_faces = bord.nb_faces() / 2;
      for (int i = 0; i < nb_faces; i++)
        {
          // Les indices des deux elements "voisins" par la face periodique:
          int elem0 = elems_voisins[i];
          int elem1 = elems_voisins[i+nb_faces]; // Indice de la face perio correspondante

          ++nb_faces_perio[elem0 - my_offset*(elem0 >= my_offset)];
          ++nb_faces_perio[elem1 - my_offset*(elem1 >= my_offset)];
          if (elem0 == elem1)
            {
              Cerr << "Error in calculer_correspondance_faces_perio: the faces " << i
                   << " and " << i + nb_faces
                   << "\n of the boundary " << bord.le_nom()
                   << " are neighbors of the same element " << elem0 << finl;
            }
          const int n = correspondances.dimension(0);
          correspondances.resize(n+1, 2);
          correspondances(n, 0) = elem0;
          correspondances(n, 1) = elem1;
        }
    }

  // Deuxieme etape:
  // Construction de "graph" a partir du tableau correspondances.
  graph.set_list_sizes(nb_faces_perio);
  // On recycle le tableau nb_faces_perio pour stocker le nombre
  // d'elements deja remplis dans chaque liste:
  nb_faces_perio = 0;
  const int n = correspondances.dimension(0);
  for (int i = 0; i < n; i++)
    {
      const int elem0 = correspondances(i, 0);
      const int elem1 = correspondances(i, 1);

      const int j0 = nb_faces_perio[elem0 - my_offset*(elem0 >= my_offset)]++;
      graph.set_value(elem0 - my_offset*(elem0 >= my_offset), j0, elem1);
      const int j1 = nb_faces_perio[elem1 - my_offset*(elem1 >= my_offset)]++;
      graph.set_value(elem1 - my_offset*(elem1 >= my_offset), j1, elem0);
    }
  Cerr << " There is " << n*2 << " periodic connections." << finl;
  return n * 2;
}

// Description:
//  Modifie elem_part pour assurer les proprietes suivantes :
//  1) Les elements possedant un sommet de bord sont associes
//     a un processeur possedant une face de bord adjacente a ce sommet.
//  2) Si un processeur possede un sommet periodique reel, il
//     possede forcement le renum_som_perio associe (donc un element
//     qui possede ce sommet et un face periodique).
//  Cette propriete est indispensable pour le periodique (existence
//  de renum_som_perio pour tous les sommets).
//  Pour les autres bords, cette correction est peut-etre inutile, mais
//  pas sur. Sans cette correction, il peut exister des sommets de bord
//  isoles (un processeur possede un sommet de bord mais aucune face).
//  Si on cherche les sommets de bord en parcourant les faces de bord,
//  c'est faux. Avec cette correction, cet algorithme est correct.
int Partitionneur_base::corriger_sommets_bord(const Domaine& domaine,
                                              const Noms& liste_bords_perio,
                                              const ArrOfInt& renum_som_perio,
                                              const Static_Int_Lists& som_elem,
                                              IntTab& elem_part)
{
  const int nb_som_tot = domaine.nb_som_tot();
  const Zone& zone = domaine.zone(0);
  const int nb_elem = zone.nb_elem_tot();
  const int nb_elem_tot = zone.nb_elem_tot();

  // Premiere etape :
  // Marquage des sommets de bord :
  ArrOfBit sommet_bord(nb_som_tot);
  ArrOfBit sommet_bord_perio(nb_som_tot);
  sommet_bord = 0;
  sommet_bord_perio = 0;
  // element_bord indique si l'element est adjacent a une face de bord
  ArrOfBit element_bord(nb_elem_tot);
  // element_bord_perio indique si l'element est adjacent a une face de bord periodique
  ArrOfBit element_bord_perio(nb_elem_tot);
  element_bord = 0;
  element_bord_perio = 0;

  const int nb_bords = zone.nb_bords();
  for (int i_bord = 0; i_bord < nb_bords; i_bord++)
    {
      const Bord& bord = zone.bord(i_bord);
      const IntTab& faces_sommets = bord.faces().les_sommets();
      const int nb_faces_bord = faces_sommets.dimension(0);
      const int nb_som_face = faces_sommets.dimension(1);
      ArrOfInt elems_voisins;
      const int is_perio = (liste_bords_perio.contient_(bord.le_nom()));

      chercher_elems_voisins_faces(som_elem,
                                   faces_sommets,
                                   bord.le_nom(),
                                   elems_voisins);
      // Pour chaque element voisin des faces du bord, on marque cet element.
      // On associe a chaque sommet de bord l'indice de la partie la plus
      // petite qui contient une face de bord adjacente au sommet.
      for (int i_face = 0; i_face < nb_faces_bord; i_face++)
        {
          const int elem_voisin = elems_voisins[i_face];
          element_bord.setbit(elem_voisin);
          if (is_perio)
            element_bord_perio.setbit(elem_voisin);
          for (int i_som = 0; i_som < nb_som_face; i_som++)
            {
              const int som = faces_sommets(i_face, i_som);
              sommet_bord.setbit(som);
              if (is_perio)
                sommet_bord_perio.setbit(som);
            }
        }
    }

  // Deuxieme etape:
  // Parcours des elements qui n'ont pas de face de bord mais qui ont
  //  des sommets de bord:
  //  On construit pour chaque sommet de bord de l'element, la liste des
  //  "parties autorisees"
  //    Si le sommet renum_som_perio est adjacent a une face de bord periodique,
  //    les parties autorisees sont celles qui possedent une face periodique
  //    adjacente au sommet.
  //    Sinon, c'est les parties contenant une face de bord adjacente au sommet
  //  On calcule l'intersection de ces listes et si l'element n'est pas dans
  //  une partie autorisee, on en choisit une et on le met dedans.
  int count = 0;
  {
    ArrOfInt parties_autorisees;
    parties_autorisees.set_smart_resize(1);
    ArrOfInt tmp;
    tmp.set_smart_resize(1);
    const IntTab& elements = zone.les_elems();
    const int nb_som_elem = elements.dimension(1);
    for (int elem = 0; elem < nb_elem; elem++)
      {
        // L'element a-t-il un sommet periodique ?
        int has_som_perio = 0;
        int isom;
        for (isom = 0; isom < nb_som_elem; isom++)
          if (sommet_bord_perio[elements(elem, isom)])
            has_som_perio = 1;
        // Boucle sur les sommets de l'element:
        parties_autorisees.resize_array(0);
        int nb_sommets_bord = 0; // Nombre de sommets de bord de l'element
        for (isom = 0; isom < nb_som_elem; isom++)
          {
            const int som = elements(elem, isom);
            // Ne traiter que les sommets de bord perio si l'element a un bord perio,
            // sinon ne traiter que les sommets de bord.
            if (has_som_perio && !sommet_bord_perio[som])
              continue;
            if (!sommet_bord[som])
              continue;

            nb_sommets_bord++;
            // On met dans tmp la liste des parties associees aux elements de bord adjacents
            {
              tmp.resize_array(0);
              const int renum_som = renum_som_perio[som];
              const int n = som_elem.get_list_size(renum_som);
              for (int i = 0; i < n; i++)
                {
                  const int elem2 = som_elem(renum_som, i);
                  int test;
                  if (has_som_perio)
                    test = element_bord_perio[elem2];
                  else
                    test = element_bord[elem2];
                  if (test)
                    {
                      const int p = elem_part[elem2];
                      tmp.append_array(p);
                    }
                }
              array_trier_retirer_doublons(tmp);
            }
            // Calcul de l'intersection entre tmp et parties_autorisees
            if (parties_autorisees.size_array() > 0)
              array_calculer_intersection(parties_autorisees, tmp);
            else
              parties_autorisees = tmp;
          }
        if (nb_sommets_bord > 0)
          {
            // Est-ce que l'element appartient a une partie autorisee ?
            const int n = parties_autorisees.size_array();
            if (n == 0)
              {
                Cerr << "Partitionneur_base::corriger_sommets_bord : Error. No part authorized because of the periodicity for the mesh element " << elem << finl;
                elem_part[elem] = -1;
              }
            else
              {
                int i;
                const int p = elem_part[elem];
                for (i = 0; i < n; i++)
                  if (parties_autorisees[i] == p)
                    break;
                if (i >= n)
                  {
                    // Il faut affecter une autre partie:
                    elem_part[elem] = parties_autorisees[0];
                    count++;
                  }
              }
          }
      }
    Cerr << "Partitionneur_base::corriger_sommets_bord : " << count << " modified elements" << finl;
  }
  return count;
}

// Description: applique des corrections a elem_part pour que le
//  multi-periodique soit correct :
//  Si un sommet appartient a plusieurs frontieres periodiques,
//  tous les elements adjacents sont rattaches au meme processeur.
int Partitionneur_base::corriger_multiperiodique(const Domaine& domaine,
                                                 const Noms& liste_bords_perio,
                                                 const ArrOfInt& renum_som_perio,
                                                 const Static_Int_Lists& som_elem,
                                                 IntTab& elem_part)
{
  const int nb_som = domaine.nb_som();
  const Zone& zone = domaine.zone(0);
  const int nb_elem = zone.nb_elem();

  // Pour chaque sommet periodique, selectionner une partie a laquelle il appartient
  // (la plus petite parmi les parties des elements periodiques adjacents)
  // Initialise a -1
  ArrOfInt partie_associee(nb_som);
  partie_associee= -1;
  // Pour chaque sommet, a quel(s) bords periodiques appartient-il
  // (somme de 2^n ou n est l'indice du nom du bord dans la liste des bords periodiques)
  // Initialise a 0
  ArrOfInt marqueur_bord(nb_som);

  const int nb_bords_perio = liste_bords_perio.size();
  int deux_puissance_i_bord = 1;
  for (int i_bord = 0; i_bord < nb_bords_perio; i_bord++)
    {
      const Bord& bord = zone.bord(liste_bords_perio[i_bord]);
      const IntTab& faces_sommets = bord.faces().les_sommets();
      const int nb_faces_bord = faces_sommets.dimension(0);
      const int nb_som_face = faces_sommets.dimension(1);
      ArrOfInt elems_voisins;
      chercher_elems_voisins_faces(som_elem,
                                   faces_sommets,
                                   bord.le_nom(),
                                   elems_voisins);
      for (int i_face = 0; i_face < nb_faces_bord; i_face++)
        {
          const int elem_voisin = elems_voisins[i_face];
          const int part = elem_part[elem_voisin];
          for (int i_som = 0; i_som < nb_som_face; i_som++)
            {
              const int som = faces_sommets(i_face, i_som);
              const int old_part = partie_associee[som];
              if (old_part < 0 || old_part > part)
                partie_associee[som] = part;
              marqueur_bord[som] |= deux_puissance_i_bord; // Bitwise OR operator
            }
        }
      deux_puissance_i_bord *= 2;
      if (deux_puissance_i_bord > 65536)
        {
          // De toutes facons, plus de 3 frontieres periodiques, c'est hautement suspect...
          Cerr << "Error in Partitionneur_base::corriger_multiperiodique : there is too many periodic boundaries." << finl;
          exit();
        }
    }

  // Transformer marqueur_bord:
  // 1 si le sommet appartient a plusieurs bords periodiques,
  // 0 sinon
  for (int sommet = 0; sommet < nb_som; sommet++)
    {
      // Compter le nombre de bits a 1 dans le marqueur
      const int marq = marqueur_bord[sommet];
      int n = 0;
      for (int x = 1; x < marq; x = x * 2)
        {
          if (marq & x) // bitwise AND
            n++;
        }
      // Marqueur a 1 si le sommet appartient a plusieurs bords
      marqueur_bord[sommet] = (n > 1);
    }
  // Deuxieme etape: affecter les elements adjacents a un sommet multiperiodique
  // a la partie associee au sommet renum_som_perio de ce sommet.
  const IntTab& les_elems = zone.les_elems();
  const int nb_som_elem = les_elems.dimension(1);
  int count = 0;
  for (int elem = 0; elem < nb_elem; elem++)
    {
      // Cet int vaudra -1 si aucun sommet de l'element est periodique,
      // sinon, c'est la plus petite des parties associees aux sommets periodiques
      int new_part = -1;
      for (int isom = 0; isom < nb_som_elem; isom++)
        {
          const int sommet = les_elems(elem, isom);
          if (marqueur_bord[sommet])
            {
              // Ce sommet appartient a plusieurs frontieres periodiques
              const int renum = renum_som_perio[sommet];
              const int part = partie_associee[renum];
              assert(marqueur_bord[renum]);
              assert(part > -1);
              if (new_part < 0 || new_part > part)
                new_part = part;
            }
        }
      if (new_part >= 0 && new_part != elem_part[elem])
        {
          count++;
          elem_part[elem] = new_part;
        }
    }
  Cerr << "Partitionneur_base::corriger_multiperiodique : " << count << " modified elements" << finl;

  // Add another criteria to fix some problem when building renum_som_perio in parallel:
  // Check every periodic nodes are surrounded by elements on the same parts
  ArrOfInt node(2);
  int another_count=0;
  int err=0;
  for (int som=0; som<nb_som; som++)
    {
      // Periodic nodes:
      node(0)=som;
      node(1)=renum_som_perio(som);
      if (node(0)!=node(1))
        {
          // Loop on each element surrounding the nodes
          IntLists part(2);
          for (int i=0; i<2; i++)
            {
              for (int i_elem=0; i_elem<som_elem.get_list_size(node(i)); i_elem++)
                {
                  int elem = som_elem(node(i),i_elem);
                  part[i].add_if_not(elem_part[elem]);
                }
            }
          // Check if same number:
          if (part[0].size()!=part[1].size())
            {
              Cerr << "Warning: Not the same number of parts around the periodic nodes " << node(0) << " and " << node(1) << " !" << finl;
              Cerr << "We try to fix:" << finl;
              int smaller = ( part[0].size() < part[1].size() ? 0 : 1);
              int bigger  = 1 - smaller;
              int first_part = part[smaller][0]; // Take arbitrary the first part of the smallest list
              for (int i=0; i<part[bigger].size(); i++)
                {
                  int i_part = part[bigger][i];
                  if (!part[smaller].contient(i_part))
                    {
                      // i_part -> first_part on all elements surrounding the node with more parts:
                      for (int i_elem=0; i_elem<som_elem.get_list_size(node(bigger)); i_elem++)
                        {
                          int elem = som_elem(node(bigger),i_elem);
                          if (elem_part[elem] == i_part)
                            {
                              elem_part[elem] = first_part;
                              another_count++;
                              Cerr << "Element " << elem << " moved from part " << i_part << " to " << first_part << finl;
                            }
                        }
                    }
                }
            }
          else
            {
              // Check if same parts around the 2 nodes:
              for (int i=0; i<part[0].size(); i++)
                {
                  int i_part = part[0][i];
                  if (!part[1].contient(i_part))
                    {
                      // Implement an algorithm as just above ?
                      Cerr << "Warning: different parts around the periodic nodes " << node(0) << " and " << node(1) << " !" << finl;
                      //for (int j=0;j<2;j++)
                      //   for (int k=0;k<part[j].size();k++)
                      //      Cerr << "node " << j << " part " << part[j][k] << finl;
                      Cerr << "We try to fix:" << finl;
                      // Look for the j_part not in the part[1] list:
                      for (int j=0; j<part[1].size(); j++)
                        {
                          int j_part = part[1][j];
                          if (!part[0].contient(j_part))
                            {
                              // Choice between i_part and j_part:
                              // We take i_part arbitrary so j_part -> i_part
                              for (int i_elem=0; i_elem<som_elem.get_list_size(node(1)); i_elem++)
                                {
                                  int elem = som_elem(node(1),i_elem);
                                  if (elem_part[elem] == j_part)
                                    {
                                      elem_part[elem] = i_part;
                                      Cerr << "Element " << elem << " moved from part " << j_part << " to " << i_part << finl;
                                      another_count++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  if (err)
    {
      Cerr << "Error in Partitionneur_base::corriger_multiperiodique" << finl;
      Cerr << "It will create possible problems for creation of renum_som_perio array." << finl;
      Cerr << "Contact TRUST support or change the partition options." << finl;
      exit();
    }
  count+=another_count;
  Cerr << "Partitionneur_base::corriger_multiperiodique : plus " << another_count << " another modified elements" << finl;
  return count;
}

// Description: corrige la partition elem_part pour qu'un element i se trouve sur la meme
//  partition elem_part[i] que tous les elements auxquels il est lie dans le graphe
//  (elements d'indices graph_elements_perio(i, j) pour tout j).
// Parametre: graph_elements_perio
// Signification: graphe calcule par la methode calculer_graphe_connexions_periodiques
// Parametre: elem_part
// Signification: pour chaque element, a quelle partie appartient-il.
// Valeur de retour: nombre d'elements dont la partition a ete corrigee.
int Partitionneur_base::corriger_bords_avec_graphe(const Static_Int_Lists& graph_elements_perio,
                                                   const Static_Int_Lists& som_elem,
                                                   const Domaine& domaine,
                                                   const Noms& liste_bords_perio,
                                                   IntTab& elem_part)
{
  // Algorithme: parcours de tous les elements dans l'ordre.
  //  Pour chaque element, associer aux autres elements lies la partie a laquelle appartient
  //  l'element courant. Comme le graphe est symetrique, si un element a deja ete traite,
  //  on ne change rien les fois suivantes. Donc un seul passage suffit.
  const int n = graph_elements_perio.get_nb_lists(); //elem_part.size_array();
  //assert(n == graph_elements_perio.get_nb_lists());
  int count = 0;
  for (int i = 0; i < n; i++)
    {
      const int m = graph_elements_perio.get_list_size(i);
      const int part = elem_part[i];
      for (int j = 0; j < m; j++)
        {
          const int elem2 = graph_elements_perio(i,j);
          if (elem_part[elem2] != part)
            {
              elem_part[elem2] = part;
              count++;
            }
        }
    }
  Cerr << "Partitionneur_base::corriger_bords_avec_graphe : " << count
       << " modified periodic elements" << finl;

  const int nb_sommets_reels = domaine.nb_som();
  ArrOfInt renum_som_perio(nb_sommets_reels);
  // Initialisation du tableau renum_som_perio
  for (int i = 0; i < nb_sommets_reels; i++)
    renum_som_perio[i] = i;
  int parallel_algo = Process::nproc() > 1;
  Reordonner_faces_periodiques::renum_som_perio(domaine, liste_bords_perio, renum_som_perio,
                                                parallel_algo /* pas d'espace virtuel en sequentiel */);

  if (liste_bords_perio.size() > 1)
    count += corriger_multiperiodique(domaine, liste_bords_perio, renum_som_perio, som_elem, elem_part);
  count += corriger_sommets_bord(domaine, liste_bords_perio, renum_som_perio, som_elem, elem_part);
  elem_part.echange_espace_virtuel();
  return count;
}

// Description:
//   Calcul des graphes de connectivite elements periodiques et appel a
//   corriger_periodique_avec_graphe. (Methode a utiliser quand on ne dispose
//   pas encore du graphe de de connectivite, si on a le graphe sous la main,
//   appeler directement corriger_periodique_avec_graphe)
void Partitionneur_base::corriger_bords_avec_liste(const Domaine& dom,
                                                   const Noms& liste_bords_periodiques,
                                                   const int my_offset,
                                                   IntTab& elem_part)
{
  const Zone& zone = dom.zone(0);
  Cerr << "Correction of the splitting for the periodicity" << finl;
  Static_Int_Lists som_elem;
  Cerr << " Construction of the connectivity som_elem" << finl;
  construire_connectivite_som_elem(dom.nb_som_tot(),
                                   zone.les_elems(),
                                   som_elem,
                                   1 /* inclure les elements virtuels */);
  Cerr << " Construction of graph connectivity for periodic elements" << finl;
  Static_Int_Lists graph_elements_perio;
  calculer_graphe_connexions_periodiques(zone,
                                         liste_bords_periodiques,
                                         som_elem,
                                         my_offset,
                                         graph_elements_perio);
  const int count = corriger_bords_avec_graphe(graph_elements_perio,
                                               som_elem,
                                               dom,
                                               liste_bords_periodiques,
                                               elem_part);
  Cerr << "corriger_bords_avec_liste : we have corrected " << count << " elements all in all." << finl;
}


