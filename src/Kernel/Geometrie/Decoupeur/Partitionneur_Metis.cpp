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
// File:        Partitionneur_Metis.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////
#include <Partitionneur_Metis.h>
#include <Domaine.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Param.h>

#include <Matrix_tools.h>
#include <Matrice_Morse.h>
#include <Array_tools.h>

inline void not_implemented(const Nom& chaine)
{
  Cerr << chaine << " is not implemented yet to the METIS API." << finl;
  Process::exit();
}

Implemente_instanciable_sans_constructeur(Partitionneur_Metis,"Partitionneur_Metis",Partitionneur_base);

Partitionneur_Metis::Partitionneur_Metis()
{
  nb_parties_ = -1;
  nb_essais_ = 1;
  algo_ = PMETIS;
  match_type_ = -1;
  ip_type_ = -1;
  ref_type_ = -1;
  use_weights_ = 0;
  use_segment_to_build_connectivite_elem_elem_=0;
}

Sortie& Partitionneur_Metis::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Metis::printOn invalid\n" << finl;
  exit();
  return os;
}

Entree& Partitionneur_Metis::readOn(Entree& is)
{
  Partitionneur_base::readOn(is);
  return is;
}

void Partitionneur_Metis::set_param(Param& param)
{
  param.ajouter("nb_parts",&nb_parties_,Param::REQUIRED);
  param.ajouter("nb_essais",&nb_essais_);
  param.ajouter_condition("(value_of_nb_parts_ge_1)_and_(value_of_nb_parts_le_100000)","The following condition must be satisfied : 1 <= nb_parties <= 100000");
  param.ajouter_non_std("pmetis",(this));
  param.ajouter_non_std("kmetis",(this));
  param.ajouter_non_std("match_type",(this));
  param.ajouter_non_std("initial_partition_type",(this));
  param.ajouter_non_std("refinement_type",(this));
  param.ajouter_flag("use_weights",&use_weights_);
  param.ajouter_flag("use_segment_to_build_connectivite_elem_elem",&use_segment_to_build_connectivite_elem_elem_); // option pour construire le grpah a partir des liens (segment) pour reseau electrique, sides ....
}

int Partitionneur_Metis::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="pmetis")
    {
      Cerr << " Algorithm PMETIS" << finl;
      algo_ = PMETIS;
      return 1;
    }
  else if (mot=="kmetis")
    {
      Cerr << " Algorithm KMETIS" << finl;
      algo_ = KMETIS;
      return 1;
    }
  else if (mot=="match_type")
    {
      not_implemented(mot);
      return 1;
    }
  else if (mot=="initial_partition_type")
    {
      not_implemented(mot);
      return 1;
    }
  else if (mot=="refinement_type")
    {
      not_implemented(mot);
      return 1;
    }
  else
    return Partitionneur_base::lire_motcle_non_standard(mot,is);
  return 1;
}

void Partitionneur_Metis::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

#ifndef NO_METIS
static void construire_graph_from_segment(const Domaine& dom,
                                          const int use_weights,
                                          Graph_Type& graph)
{
  const IntTab& liaisons = dom.zone(0).les_elems();


  // ****************************************************************
  // PREMIERE ETAPE: calcul du nombre de vertex et edges du graph:
  int nb_edges = liaisons.size_array(); // 2 liens par liaison
  int nb_elem=liaisons.local_max_vect()+1;  // mouif
  graph.nvtxs = nb_elem;
  graph.xadj = imalloc(nb_elem+1, "readgraph: xadj");
  graph.vwgts = NULL;
  if (! use_weights)
    {
      graph.weightflag = 0;
      graph.ewgts = NULL;
    }
  else
    {
      abort();
      graph.weightflag = 1;
      graph.ewgts = imalloc(nb_edges, "readgraph: ewgts");
    }

  // on construit connectivite item item
  IntTab stencyl(0,2);
  stencyl.set_smart_resize(1);
  int size=0;
  int nbl=liaisons.dimension(0);
  for (int i=0; i<nbl; i++)
    {
      stencyl.resize(size+2,2);
      int n1=liaisons(i,0);
      int n2=liaisons(i,1);
      {
        stencyl(size,0)=n1;
        stencyl(size,1)=n2;

        size++;
        stencyl(size,0)=n2;
        stencyl(size,1)=n1;

        size++;
      }
    }
  tableau_trier_retirer_doublons(stencyl);
  Matrice_Morse A;
  Matrix_tools::allocate_morse_matrix(nb_elem,nb_elem,stencyl,A);

  nb_edges=A.tab2_.size_array(); // des liens peuvent etre doubles
  graph.nedges = nb_edges;
  graph.adjncy = imalloc(nb_edges, "readgraph: adjncy");

  //
  assert(A.tab1_.size_array()==graph.nvtxs+1);
  for (int c=0; c<graph.nvtxs+1; c++)
    {
      graph.xadj[c]=A.tab1_[c]-1;
    }
  // assert(A.tab2_.size_array()==graph.nedges);
  for (int c=0; c<graph.nedges; c++)
    {
      graph.adjncy[c]=A.tab2_[c]-1;
    }
}

// Si use_weights, on pondere les liens entre les elements periodiques
// pour les forcer a etre sur le meme processeur. Cela diminue le nombre
// de corrections a faire ensuite (voir (***))
static void construire_graph_elem_elem(const Domaine& dom,
                                       const Noms& liste_bords_periodiques,
                                       const int use_weights,
                                       Graph_Type& graph,
                                       Static_Int_Lists& graph_elements_perio)
{
  Static_Int_Lists som_elem;
  const Zone& zone = dom.zone(0);
  const Elem_geom_base& type_elem = zone.type_elem().valeur();
  IntTab faces_element_reference;
  const int is_regular =
    type_elem.get_tab_faces_sommets_locaux(faces_element_reference);
  if (! is_regular)
    {
      Cerr << "Error in Partitionneur_Metis::construire_graph_elem_elem\n"
           << " The type of element is not supported" << finl;
    }
  const int nb_faces_par_element = faces_element_reference.dimension(0);
  const int nb_sommets_par_face = faces_element_reference.dimension(1);

  const IntTab& elem_som = zone.les_elems();

  Cerr << " Construction of the som_elem connectivity" << finl;
  construire_connectivite_som_elem(dom.nb_som(),
                                   elem_som,
                                   som_elem,
                                   0 /* ne pas inclure les elements virtuels */);

  int nb_connexions_perio = 0;
  if (liste_bords_periodiques.size() > 0)
    {
      Cerr << " Construction of graph connectivity for periodic boundaries" << finl;
      nb_connexions_perio = Partitionneur_base::calculer_graphe_connexions_periodiques(dom.zone(0),
                                                                                       liste_bords_periodiques,
                                                                                       som_elem,
                                                                                       graph_elements_perio);
    }

  // ****************************************************************
  // PREMIERE ETAPE: calcul du nombre de vertex et edges du graph:

  // Nombre total de faces de bord:
  const int nb_faces_bord = zone.nb_faces_frontiere();
  // Chaque element du maillage est un "vertex" du graph.
  // Les "edges" du graph relient chaque element a ses voisins par une face.
  // Il y a autant d'edges que de faces ayant deux voisins, fois 2
  const int nb_elem = zone.nb_elem();

  // Formule classique: nb_faces internes = nnn/2 avec :
  const int nnn = nb_elem * nb_faces_par_element - nb_faces_bord + nb_connexions_perio;
  const int nb_edges = nnn;

  graph.nvtxs = nb_elem;
  graph.nedges = nb_edges;
  graph.xadj = imalloc(nb_elem+1, "readgraph: xadj");
  graph.adjncy = imalloc(nb_edges, "readgraph: adjncy");
  graph.vwgts = NULL;
  if (! use_weights)
    {
      graph.weightflag = 0;
      graph.ewgts = NULL;
    }
  else
    {
      graph.weightflag = 1;
      graph.ewgts = imalloc(nb_edges, "readgraph: ewgts");
    }

  Cerr << " Construction of the elem_elem connectivity" << finl;
  // ***************************************************************
  // DEUXIEME ETAPE: remplissage du graph
  //  Algorithme: Pour chaque element, boucle sur les faces de l'element
  //  et on ajoute un "edge" entre l'element et ses voisins. On cherche
  //  l'element voisin par une face a l'aide de la fonction find_adjacent_elements.
  //
  // Deux tableaux de travail:
  ArrOfInt une_face(nb_sommets_par_face); // Les sommets de la face en cours
  ArrOfInt voisins; // Les elements voisins d'une_face
  voisins.set_smart_resize(1);

  int error = 0;
  int edge_count = 0;
  int i_elem;
  for (i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      graph.xadj[i_elem] = edge_count;
      int i_face;
      for (i_face = 0; i_face < nb_faces_par_element; i_face++)
        {
          // Construction de cette face de l'element:
          // (indice des sommets de la face dans le domaine)
          {
            int i;
            for (i = 0; i < nb_sommets_par_face; i++)
              {
                const int i_som = faces_element_reference(i_face, i);
                const int sommet = elem_som(i_elem, i_som);
                une_face[i] = sommet;
              }
          }
          // Recherche des elements voisins de cette face:
          // (indices des elements contenant les sommets de la face)
          find_adjacent_elements(som_elem, une_face, voisins);
          const int nb_voisins = voisins.size_array();
          int elem_voisin = -1;
          switch (nb_voisins)
            {
            case 0:
              {
                // Aucun voisin: erreur interne
                error = 3;
                break;
              }
            case 1:
              {
                // Un seul voisin, c'est une face frontiere
                const int elem = voisins[0];
                if (elem != i_elem)
                  error = 3; // l'element i_elem n'est pas voisin: erreur interne
                else
                  elem_voisin = -1;
                break;
              }
            case 2:
              {
                // Le cas le plus courant:
                const int elem0 = voisins[0];
                const int elem1 = voisins[1];
                if (elem0 == i_elem)
                  elem_voisin = elem1;
                else if (elem1 == i_elem)
                  elem_voisin = elem0;
                else
                  error = 3; // l'element i_elem n'est pas voisin: erreur interne
                break;
              }
            default:
              {
                // Plus de deux voisins
                error = 2;
              }
            }

          if (error)
            break;

          if (elem_voisin >= 0)
            {
              if (edge_count >= nb_edges)
                {
                  error = 1;
                  break;
                }
              graph.adjncy[edge_count] = elem_voisin;
              if (use_weights)
                graph.ewgts[edge_count] = 1;
              edge_count++;
            }
        }
      // Ajout des connexions supplementaires pour les faces periodiques
      if (nb_connexions_perio > 0)
        {
          const int n = graph_elements_perio.get_list_size(i_elem);
          for (int i = 0; i < n; i++)
            {
              const int elem_voisin = graph_elements_perio(i_elem, i);
              if (edge_count >= nb_edges)
                {
                  error = 1;
                  break;
                }
              graph.adjncy[edge_count] = elem_voisin;
              // Les connexions entre faces periodiques sont fortes:
              // on veut que ces elements soient sur le meme processeur
              // Attention, si on met un poids nettement plus eleve que les autres
              // edges, on degrade la qualite du decoupage !
              if (use_weights)
                {
                  graph.ewgts[edge_count] = 4;
                }
              edge_count++;
            }
        }
      else if (use_weights)
        {
          Cerr << "Warning: You specify use_weights option with Metis but you didn't use Periodique keyword" << finl;
          Cerr << "to define the boundary where periodicity apply." << finl;
          Cerr << "Either suppress use_weight or add Periodique keyword." << finl;
          Process::exit();
        }
      if (error > 0)
        break;
    }
  graph.xadj[nb_elem] = edge_count;

  if (edge_count != nb_edges)
    {
      Cerr << "Error in Partitionneur_Metis::construire_graph_elem_elem\n"
           << " The number of element-element connections is smaller than expected\n"
           << " The number of boundary faces is wrong\n"
           << " You must discretize the domain to check." << finl;
      Process::exit();
    }
  else if (error == 1)
    {
      Cerr << "Error in Partitionneur_Metis::construire_graph_elem_elem\n"
           << " The number of element-element connections is greater than expected\n"
           << " The number of boundary faces is wrong.\n"
           << " You must discretize the domain to check." << finl;
      Process::exit();
    }
  else if (error == 2)
    {
      Cerr << "Error in Partitionneur_Metis::construire_graph_elem_elem\n"
           << " Problem in the mesh: one internal face has more than two neighboring elements\n"
           << " List of neighboring elements:" << voisins
           << "\n Nodes of the face:" << une_face;
      Process::exit();
    }
  else if (error)
    {
      Cerr << "Internal error in Partitionneur_Metis::construire_graph_elem_elem\n"
           << " (error " << error << ") Problem in neighborhood algorithms"
           << finl;
      Process::exit();
    }
}
#endif
// Description:
//  Calcule le graphe de connectivite pour Metis, appelle le partitionneur
//  et remplit elem_part (pour chaque element, numero de la partie qui lui
//  est attribuee).
//  Les parties sont equilibrees de facon a minimiser le nombre de faces de joint
//  et a equilibrer le nombre d'elements par partie.
// Precondition:
//  domaine associe et nombre de parties initialise
void Partitionneur_Metis::construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const
{
#ifdef NO_METIS
  Cerr << "METIS is not compiled with this version. Use another partition tool like Tranche." << finl;
  Process::exit();
#else
  if (!ref_domaine_.non_nul())
    {
      Cerr << "Error in Partitionneur_Metis::construire_partition\n";
      Cerr << " The domain has not been associated" << finl;
      exit();
    }
  if (nb_parties_ <= 0)
    {
      Cerr << "Error in Partitionneur_Metis::construire_partition\n";
      Cerr << " The parts number has not been initialized" << finl;
      exit();
    }

  // Cas particulier: si nb_parts == 1, METIS ne veut rien faire...
  if (nb_parties_ == 1)
    {

      int nb_elem = ref_domaine_.valeur().zone(0).nb_elem();
      if (use_segment_to_build_connectivite_elem_elem_==1)
        nb_elem = ref_domaine_.valeur().nb_som();
      elem_part.resize_array(nb_elem);
      elem_part = 0;
      return;
    }

  if (ref_domaine_.valeur().zone(0).nb_elem() == 0)
    return;

  Cerr << "Partitionneur_Metis::construire_partition" << finl;
  Cerr << " Construction of graph connectivity..." << finl;
  Graph_Type graph;
  Static_Int_Lists graph_elements_perio;
  if (use_segment_to_build_connectivite_elem_elem_==0)
    {
      construire_graph_elem_elem(ref_domaine_.valeur(), liste_bords_periodiques_,
                                 use_weights_,
                                 graph,
                                 graph_elements_perio);

    }
  else
    {
      construire_graph_from_segment(ref_domaine_.valeur(), use_weights_,
                                    graph);

    }
  idx_t * partition = imalloc(graph.nvtxs,"Allocation partition");
#ifdef METIS
  idx_t int_parts = nb_parties_;
#endif
  idx_t edgecut = 0; // valeur renvoyee par metis (nombre total de faces de joint)

  switch(algo_)
    {
    case PMETIS:
      {
        Cerr << "===============" << finl;
        Cerr << "Call for PMETIS" << finl;
        Cerr << "===============" << finl;
        // Voir le manual.pdf de METIS 5.0
        idx_t options[METIS_NOPTIONS];
        METIS_SetDefaultOptions(options);
        //options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB|METIS_PTYPE_KWAY; // Methode de partitionnement
        //options[METIS_OPTION_OBJTYPE]=METIS_OBJTYPE_CUT|METIS_OBJTYPE_VOL; // Objective type
        //options[METIS_OPTION_CTYPE]=METIS_CTYPE_SHEM|METIS_CTYPE_RM; // Matching scheme during coarsening
        //options[METIS_OPTION_IPTYPE]=METIS_IPTYPE_GROW; // Algorithm during initial partitioning
        //options[METIS_OPTION_RTYPE]=METIS_RTYPE_FM;   // Algorithm for refinement
        options[METIS_OPTION_NCUTS]=nb_essais_;         // Nombre de partitionnements testes pour en prendre le meilleur
        options[METIS_OPTION_NUMBERING]=0;              // Numerotation C qui demarre a 0
        options[METIS_OPTION_DBGLVL]=111111111;         // Mode verbose maximal
        options[METIS_OPTION_NO2HOP]=1;                 // 5.1.0: not perform any 2-hop matchings (as 5.0.3)
        idx_t ncon=1;

        // Implementation reduite (plusieurs valeurs par defaut->NULL) pour METIS 5.0
        int status = METIS_PartGraphRecursive(&graph.nvtxs, &ncon, graph.xadj,
                                              graph.adjncy, graph.vwgts, NULL, graph.ewgts,
                                              &int_parts, NULL, NULL, options,
                                              &edgecut, partition);
        if (status != METIS_OK)
          {
            Cerr << "Call to METIS failed." << finl;
            if (status == METIS_ERROR_INPUT)  Cerr << "It seems there is an input error." << finl;
            if (status == METIS_ERROR_MEMORY) Cerr << "It seems it couldn't allocate enough memory." << finl;
            if (status == METIS_ERROR)        Cerr << "It seems there is a METIS internal error." << finl;
            Cerr << "Contact TRUST support." << finl;
            exit();
          }
        Cerr << "===============" << finl;
        break;
      }
    case KMETIS:
      {
        Cerr << " Call for KMETIS" << finl;
        idx_t options[METIS_NOPTIONS];
        METIS_SetDefaultOptions(options);
        //options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB|METIS_PTYPE_KWAY; // Methode de partitionnement
        //options[METIS_OPTION_OBJTYPE]=METIS_OBJTYPE_CUT|METIS_OBJTYPE_VOL; // Objective type
        //options[METIS_OPTION_CTYPE]=METIS_CTYPE_SHEM; // Matching scheme during coarsening
        //options[METIS_OPTION_IPTYPE]=METIS_IPTYPE_GROW; // Algorithm during initial partitioning
        //options[METIS_OPTION_RTYPE]=METIS_RTYPE_FM; // Algorithm for refinement
        options[METIS_OPTION_NCUTS]=nb_essais_;         // Nombre de partitionnements testes pour en prendre le meilleur
        options[METIS_OPTION_NUMBERING]=0;     // Numerotation C qui demarre a 0
        options[METIS_OPTION_DBGLVL]=111111111; // Mode verbose maximal
        idx_t ncon=1;
        // Conseil de la doc Metis 4.0 : METIS_PartGraphKway si int_parts>8, METIS_PartGraphRecursive sinon...
        // En effet semble plus rapide, mais edgecut en sortie est moins bon...
        int status = METIS_PartGraphKway(&graph.nvtxs, &ncon, graph.xadj,
                                         graph.adjncy, graph.vwgts, NULL, graph.ewgts,
                                         &int_parts, NULL, NULL, options ,
                                         &edgecut, partition);
        if (status != METIS_OK)
          {
            Cerr << "Call to METIS failed." << finl;
            if (status == METIS_ERROR_INPUT)  Cerr << "It seems there is an input error." << finl;
            if (status == METIS_ERROR_MEMORY) Cerr << "It seems it couldn't allocate enough memory." << finl;
            if (status == METIS_ERROR)        Cerr << "It seems there is a METIS internal error." << finl;
            Cerr << "Contact TRUST support." << finl;
            exit();
          }
        break;
      }
    default:
      {
        Cerr << "Internal error Partitionneur_Metis: not coded" << finl;
        exit();
      }
    }
  cerr << "Partitioning quality : edgecut = " << edgecut << endl;
  Cerr << "-> It is roughly the total number of edges (faces) which will be shared by the processors." << finl;
  Cerr << "-> The lesser this number is, the lesser the total volume of communication between processors." << finl;
  Cerr << "-> You can increase nb_essais option (default 1) to try to reduce (but at a higher CPU cost) this number." << finl;
  Cerr << "===============" << finl;

  free(graph.xadj);
  free(graph.adjncy);
  if (graph.ewgts)
    free(graph.ewgts);

  const int n = graph.nvtxs;
  elem_part.resize_array(n);
  for (int i = 0; i < n; i++)
    elem_part[i] = partition[i];

  // Correction de la partition pour la periodicite. (***)
  if (graph_elements_perio.get_nb_lists() > 0)
    {
      Cerr << "Correction of the partition for the periodicity" << finl;
      corriger_bords_avec_liste(ref_domaine_.valeur(),
                                liste_bords_periodiques_,
                                elem_part);
      Cerr << "  If this number is high, we can improve the splitting with the option use_weights\n"
           << "  but it takes more memory)" << finl;
    }

  if (use_segment_to_build_connectivite_elem_elem_==0)
    {
      Cerr << "Correction elem0 on processor 0" << finl;
      corriger_elem0_sur_proc0(elem_part);
    }
  free(partition);
#endif
}

// Description:
// Precondition:
// Parametre: int n
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Precondition:
// Parametre: char * msg
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int*
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
#ifndef NO_METIS
idx_t *imalloc(int n, const char * msg)
{
  idx_t *ptr;

  if (n == 0)
    return NULL;

  ptr = (idx_t *)malloc(sizeof(idx_t)*n);

  if (ptr == NULL)
    {
      printf("TRUST has detected a memory allocation fail: %s int\n", msg);
      Process::exit();
    }
  return ptr;
}
#endif
