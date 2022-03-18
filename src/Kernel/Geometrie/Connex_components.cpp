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
// File:        Connex_components.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Connex_components.h>
#include <communications.h>
#include <TRUSTTab.h>
#include <ArrOfBit.h>

// Description: Calcul des ensembles connexes par faces d'elements
//  non "marques" (les elements sont relies entre eux par un graphe
//  symetrique passant par les faces).
//  Une portion de domaine connexe porte un numero 0 <= i < N
//  unique et est delimite soit par un bord, soit par un element voisin "marque"
//  par num_compo[elem] = -1.
//  Cette methode est sequentielle (peut etre appelee sur un seul processeur)
// Parametre: elem_faces
// Description: graph de connectivite elements-faces (pour chaque element i, indices
//  des faces de cet element), voir Zone_VF::elem_faces()
// Parametre: faces_elem
// Description: graph de connectivite faces-elements (pour chaque face, indice du
//  ou des deux elements adjacents), voir Zone_VF::face_voisins()
// Parametre: num_compo
//  En entree, un tableau tel que num_compo.size_array() == elem_faces.dimension_tot(0),
//  et dont certaines valeurs valent -1 (elements marques), et d'autres non.
//  En sortie: les elements pour lesquels num_compo = -1 ne sont pas modifies, les autres
//  sont numerotes par composante connexe locale. On remplit tout le tableau
//  jusqu'a size_tot() y compris les elements virtuels. Les numeros de composantes
//  sont locaux a ce processeur.
// Valeur de retour: N, nombre de composantes connexes locales.
int search_connex_components_local(const IntTab& elem_faces, const IntTab& faces_elem, IntVect& num_compo)
{
  const int nbelem = num_compo.size_totale();
  const int nb_voisins = elem_faces.dimension(1);
  assert(elem_faces.dimension_tot(0) == nbelem);
  {
    int i;
    for (i = 0; i < nbelem; i++)
      if (num_compo[i] != -1)
        num_compo[i] = -2;
  }
  int start_element = 0;
  int num_compo_courant = 0;
  ArrOfInt liste_elems;
  liste_elems.set_smart_resize(1);
  ArrOfInt tmp_liste;
  tmp_liste.set_smart_resize(1);
  do
    {
      // Cherche le prochain element non attribue a une composante connexe
      while (start_element < nbelem && num_compo[start_element] >= -1)
        start_element++;
      if (start_element == nbelem)
        break;
      // Recherche des elements de la composante connexe a partir de cet element
      liste_elems.resize_array(1);
      liste_elems[0] = start_element;
      num_compo[start_element] = num_compo_courant;
      while (liste_elems.size_array() > 0)
        {
          tmp_liste.resize_array(0);
          const int liste_elems_size = liste_elems.size_array();
          for (int i_elem = 0; i_elem < liste_elems_size; i_elem++)
            {
              const int elem = liste_elems[i_elem];
              // Ajout des voisins non attribues de cet element dans la liste a
              // traiter a l'etape suivante
              for (int j = 0; j < nb_voisins; j++)
                {
                  const int face = elem_faces(elem, j);
                  const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - elem;
                  if (voisin >= 0)
                    {
                      const int num = num_compo[voisin];
                      if (num == -2)
                        {
                          num_compo[voisin] = num_compo_courant;
                          tmp_liste.append_array(voisin);
                        }
                    }
                }
            }
          liste_elems = tmp_liste;
        }
      num_compo_courant++;
    }
  while (1);
  // Renvoie le nombre de composantes connexes locales trouvees
  return num_compo_courant;
}

// Description: recherche des composantes connexes d'un graphe local (non distribue
//  sur les processeurs) non symetrique.
// Parametre: graph
// Description:
//  On suppose que "graph" est un tableau a deux colonnes
//  contenant des couples d'indices de "sommets" d'un graphe. Ces couples definissent
//  un graphe dont on veut chercher les composantes connexes:
//  deux indices i et j sont dans la meme composante connexe si et seulement si il
//  existe une suite de couples qui relient directement ou indirectement i et j.
// Parametre: connex_components
//  On suppose que connex_components est dimensionne a nb_sommets, le nombre de sommets du graphe.
//  On met dans connex_components[i] le numero de la composante connexe du graphe a laquelle appartient
//  le "sommet" i. On attribue tous les numeros i tels que 0 <= i < N
// Valeur de retour: le nombre N de composantes connexes trouvees.
int compute_graph_connex_components(const IntTab& graph, ArrOfInt& connex_components)
{
  // connex_components doit deja avoir la bonne taille en entree !
  const int nb_sommets = connex_components.size_array();

  // renum_data definit des listes chainees de numeros de "sommets" appartenant a
  //  la meme composante connexe.
  // renum_data(i,0)= numero du premier "sommet" de la liste a laquelle appartient i
  // renum_data(i,1)= numero du "sommet" suivant dans la liste
  IntTab renum_data(nb_sommets, 2);
  // Au debut, chaque sommet est toute seule dans une liste:
  int i_sommet;
  for (i_sommet = 0; i_sommet < nb_sommets; i_sommet++)
    {
      renum_data(i_sommet, 0) = i_sommet;
      renum_data(i_sommet, 1) = -1; // fin de liste
    }
  const int nbcouples = graph.dimension(0);
  for (int i_couple = 0; i_couple < nbcouples; i_couple++)
    {
      const int compo1 = graph(i_couple, 0); // la plus petite
      const int compo2 = graph(i_couple, 1); // la plus grande
      assert(compo1 != compo2);
      // Si les deux composantes sont deja dans la meme liste,
      // ne rien faire.
      if (renum_data(compo1, 0) == renum_data(compo2, 0))
        continue;
      // Reunir la liste1 contenant compo1 et la liste2 contenant compo2:
      // 1) trouver la fin de la premiere liste
      int fin_liste1 = compo1;
      for (;;)
        {
          const int next = renum_data(fin_liste1, 1);
          if (next < 0)
            break;
          fin_liste1 = next;
        }
      // 2) brancher la liste2 a la fin de la liste1 :
      const int debut_liste2 = renum_data(compo2, 0);
      renum_data(fin_liste1, 1) = debut_liste2;
      // 2) mettre a jour le debut de liste pour liste2 :
      i_sommet = debut_liste2;
      const int debut_liste1 = renum_data(compo1, 0);
      do
        {
          renum_data(i_sommet, 0) = debut_liste1;
          i_sommet = renum_data(i_sommet, 1);
        }
      while (i_sommet >= 0);
    }

  // Creation d'une numerotation contigue pour les composantes:
  // Prochain numero a attribuer
  int count = 0;
  connex_components = -1;
  for (i_sommet = 0; i_sommet < nb_sommets; i_sommet++)
    {
      if (connex_components[i_sommet] < 0)
        {
          // sommet pas encore traite
          // Associe un nouveau numero a tous les sommets de la composante
          // connexe a laquelle appartient i_sommet:
          for (int i = renum_data(i_sommet, 0); i >= 0; i = renum_data(i, 1))
            connex_components[i] = count;
          // Nouveau numero pour la prochaine composante
          count++;
        }
    }
  // On renvoie le nombre de composantes connexes trouvees
  return count;
}

// Description:
//  Recherche les composantes connexes d'un ensemble d'elements distribue sur
//  tous les processeurs. Cette methode est parallele et doit etre appelee en
//  meme temps sur tous les processeurs.
// Parametre: num_compo
// Description:
//  num_compo doit avoir au moins une couche d'elements virtuels et doit contenir
//  le resultat de la methode search_connex_components_local(). Les elements
//  virtuels ont donc ete remplis avec des numeros de composantes connexes locales.
//  Le tableau num_compo ne doit PAS avoir subi echange_espace_virtuel !!!
//  On cherche a l'aide des elements virtuels les composantes connexes connectees
//  entre elles entre deux processeurs et on leur attribue un numero unique i tel
//  que tous les indices 0 <= i < N sont utilises.
// Parametre: nb_local_components
// Description: doit contenir le nombre de composantes connexes locales utilisees
//  dans num_compo a l'entree (egale a la valeur de retour de la fonction
//  search_connex_components_local()).
// Valeur de retour: nombre N de composantes connexes globales trouvees.
int compute_global_connex_components(IntVect& num_compo, int nb_local_components)
{
  const int nbelem = num_compo.size();
  const int nbelem_tot = num_compo.size_totale();
  //int i;

  // Transformation des indices locaux de composantes connexes en un indice global
  // (on ajoute un decalage aux indices globaux avec mppartial_sum())
  const int decalage = mppartial_sum(nb_local_components);
  const int nb_total_components = Process::mp_sum(nb_local_components);
  for (int i = 0; i < nbelem_tot; i++)
    if (num_compo[i] >= 0)
      num_compo[i] += decalage;

  // Pour trouver les correspondances entre un numero de composante locale et un
  // numero de la meme composante sur le processeur voisin, on cree une copie du
  // tableau num_compo sur laquelle on fait un echange_espace_virtuel(). Ainsi,
  // sur les cases virtuelles du tableau, on a dans num_compo le numero de la
  // composante locale et dans copie_compo le numero de cette meme composante sur
  // le processeur proprietaire de l'element. Donc ces deux numeros designent
  // la meme composante connexe.
  IntVect copie_compo(num_compo);
  copie_compo.echange_espace_virtuel();

  // Recherche des equivalences entre les numeros des composantes locales et
  // les numeros des composantes voisines. On construit un graphe dont les
  // liens relient les composantes equivalentes.
  // Tableau de marqeurs pour les equivalences deja trouvees.
  // Dimensions = nb composantes locales * nb_composantes total
  //  (pour ne pas prendre en compte la meme composante plusieurs fois).
  ArrOfBit markers(nb_local_components * nb_total_components);
  markers = 0;
  // Tableau de correspondances entre composantes connexes locales et distantes
  IntTab graph;
  graph.set_smart_resize(1);
  int graph_size = 0;
  // Parcours des elements virtuels uniquement
  for (int i = nbelem; i < nbelem_tot; i++)
    {
      int compo = num_compo[i];
      if (compo < 0)
        continue;
      int compo2 = copie_compo[i];
      // Index du couple compo2/compo dans le tableau markers
      // Le tableau num_compo ne doit contenir que des composantes locales:
      assert(compo >= decalage && compo - decalage < nb_local_components);
      // compo2 est forcement une composante distante.
      assert(compo2 < decalage || compo2 - decalage >= nb_local_components);
      const int index = (compo - decalage) * nb_total_components + compo2;
      if (!markers.testsetbit(index))
        {
          graph.resize(graph_size+1, 2);
          // On met le plus petit numero de composante en colonne 0:
          if (compo2 < compo)
            {
              int tmp = compo;
              compo = compo2;
              compo2 = tmp;
            }
          graph(graph_size, 0) = compo;
          graph(graph_size, 1) = compo2;
          graph_size++;
        }
    }

  ArrOfInt renum;
  if (Process::je_suis_maitre())
    {
      // Reception des portions de graphe des autres processeurs
      IntTab tmp;
      const int nproc = Process::nproc();
      int pe;
      for (pe = 1; pe < nproc; pe++)
        {
          recevoir(tmp, pe, 54 /* tag */);
          const int n2 = tmp.dimension(0);
          graph.resize(graph_size + n2, 2);
          for (int i = 0; i < n2; i++)
            {
              graph(graph_size, 0) = tmp(i, 0);
              graph(graph_size, 1) = tmp(i, 1);
              graph_size++;
            }
        }
      // Calcul des composantes connexes du graphe
      renum.resize_array(nb_total_components);
      const int n = compute_graph_connex_components(graph, renum);
      Process::Journal() << "compute_global_connex_components: nb_components=" << n << finl;
    }
  else
    {
      // Envoi du graphe local au processeur 0
      envoyer(graph, 0, 54 /* tag */);
    }

  // Reception des composantes connexes
  envoyer_broadcast(renum, 0 /* processeur source */);

  // Renumerotation des composantes dans num_compo
  for (int i = 0; i < nbelem_tot; i++)
    {
      const int x = num_compo[i];
      if (x >= 0)
        {
          const int new_x = renum[x];
          num_compo[i] = new_x;
        }
    }
  // Verification: si on fait un echange espace virtuel,
  //  cela ne doit par changer le numero des composantes
  //  connexes !

  int nb_components = 0;
  // Tous les processeurs possedent le meme tableau renum, tout le monde
  //  calcule donc le meme maximum !
  if (renum.size_array() > 0)
    nb_components = max_array(renum) + 1;
  return nb_components;
}

