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
// File:        Domain_Graph.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////
#include <Domaine.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Poly_geom_base.h>
#include <Matrix_tools.h>
#include <Matrice_Morse.h>
#include <Array_tools.h>
#include <communications.h>
#include <Domain_Graph.h>
#include <Partitionneur_base.h>
#include <map>

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
static idx_t *imalloc(int n, const char * msg)
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

static void construire_connectivite_real_som_virtual_elem(const int       nb_sommets,
                                                          const IntTab&      les_elems,
                                                          Static_Int_Lists& som_elem,
                                                          const IntTab& elem_virt_pe_num,
                                                          const  ArrOfInt& offsets)
{
  // Nombre d'elements du domaine
  const int nb_elem = les_elems.dimension_tot(0);
  const int local_nb_elem = les_elems.dimension(0);
  // Nombre de sommets par element
  const int nb_sommets_par_element = les_elems.dimension(1);

  // Construction d'un tableau initialise a zero : pour chaque sommet,
  // nombre d'elements voisins de ce sommet
  ArrOfInt nb_elements_voisins(nb_sommets);

  // Premier passage : on calcule le nombre d'elements voisins de chaque
  // sommet pour creer la structure de donnees
  int elem, i;

  //real elements
  for (elem = 0; elem < nb_elem; elem++)
    {
      for (i = 0; i < nb_sommets_par_element; i++)
        {
          int sommet = les_elems(elem, i);
          if(sommet >= nb_sommets) continue; //skipping virtual node
          // GF cas des polyedres
          if (sommet==-1) break;
          nb_elements_voisins[sommet]++;
        }
    }

  som_elem.set_list_sizes(nb_elements_voisins);

  // On reutilise le tableau pour stocker le nombre d'elements dans
  // chaque liste pendant qu'on la remplit
  nb_elements_voisins = 0;

  // Remplissage du tableau des elements voisins.
  for (elem = 0; elem < nb_elem; elem++)
    {
      for (i = 0; i < nb_sommets_par_element; i++)
        {
          int sommet = les_elems(elem, i);
          if(sommet >= nb_sommets) continue;
          // GF cas des polyedres
          if (sommet==-1) break;
          int n = (nb_elements_voisins[sommet])++;

          int elem_num_global = -1;
          if(elem>= local_nb_elem)
            {
              int proc_of_elem = elem_virt_pe_num(elem-local_nb_elem, 0);
              int elem_number_on_local_proc = elem_virt_pe_num(elem-local_nb_elem, 1);
              elem_num_global = elem_number_on_local_proc + offsets[proc_of_elem];
            }
          else
            elem_num_global = elem + offsets[Process::me()];

          som_elem.set_value(sommet, n, elem_num_global);
        }
    }

  // Tri de toutes les listes dans l'ordre croissant
  som_elem.trier_liste(-1);
}


#ifndef NO_METIS

void Domain_Graph::free_memory()
{

  free(xadj);
  free(adjncy);
  free(edgegsttab);
  if (ewgts)
    free(ewgts);

  free(vtxdist);

}


void Domain_Graph::construire_graph_from_segment(const Domaine& dom,
                                                 const int use_weights)
{
  const IntTab& liaisons = dom.zone(0).les_elems();

  // ****************************************************************
  // PREMIERE ETAPE: calcul du nombre de vertex et edges du graph:
  int nb_edges = liaisons.size_array(); // 2 liens par liaison
  int nb_elem=liaisons.local_max_vect()+1;  // mouif
  nvtxs = nb_elem;
  xadj = imalloc(nb_elem+1, "readgraph: xadj");
  vwgts = NULL;
  if (! use_weights)
    {
      weightflag = 0;
      ewgts = NULL;
    }
  else
    {
      abort();
      weightflag = 1;
      ewgts = imalloc(nb_edges, "readgraph: ewgts");
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

  nb_edges=A.get_tab2().size_array(); // des liens peuvent etre doubles
  nedges = nb_edges;
  adjncy = imalloc(nb_edges, "readgraph: adjncy");

  //
  assert(A.get_tab1().size_array()==nvtxs+1);
  for (int c=0; c<nvtxs+1; c++)
    {
      xadj[c]=A.get_tab1()(c)-1;
    }
  for (int c=0; c<nedges; c++)
    {
      adjncy[c]=A.get_tab2()(c)-1;
    }

}
#endif

// Si use_weights, on pondere les liens entre les elements periodiques
// pour les forcer a etre sur le meme processeur. Cela diminue le nombre
// de corrections a faire ensuite (voir (***))
#ifndef NO_METIS
void Domain_Graph::construire_graph_elem_elem(const Domaine& dom,
                                              const Noms& liste_bords_periodiques,
                                              const int use_weights,
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
      Cerr << "Error in Domain_Graph::construire_graph_elem_elem\n"
           << " The type of element is not supported" << finl;
      ref_cast(Poly_geom_base,type_elem).get_tab_faces_sommets_locaux(faces_element_reference,0);
    }
  int nb_faces_par_element = faces_element_reference.dimension(0);
  const int nb_sommets_par_face = faces_element_reference.dimension(1);

  const IntTab& elem_som = zone.les_elems();
  const int nb_elem = zone.nb_elem();

  ArrOfInt offsets(Process::nproc());
  offsets = mppartial_sum(nb_elem);
  envoyer_all_to_all(offsets, offsets);
  int my_offset = offsets[Process::me()];

  Cerr << " Construction of the som_elem connectivity" << finl;
  if(Process::nproc() > 1)
    {
      //what we're doing here is not equivalent to construire_connectivite_som_elem with virtual elements set to 1
      //in the latter, we also build the connectivity for virtual nodes
      //here, we add the connectivity of real nodes only with virtual elements
      // + we want som_elem to contain global numerotation for elements
      IntTab elem_virt_pe_num;
      zone.construire_elem_virt_pe_num(elem_virt_pe_num);
      construire_connectivite_real_som_virtual_elem(dom.nb_som(),
                                                    elem_som,
                                                    som_elem,
                                                    elem_virt_pe_num,
                                                    offsets);

    }
  else
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
  // Formule classique: nb_faces internes = nnn/2 avec :
  int nnn = nb_elem * nb_faces_par_element - nb_faces_bord + nb_connexions_perio;
  if (sub_type(Poly_geom_base,zone.type_elem().valeur()))
    {
      const Poly_geom_base& poly=ref_cast(Poly_geom_base,zone.type_elem().valeur());
      nnn= poly.get_somme_nb_faces_elem() - nb_faces_bord + nb_connexions_perio;
    }

  const int nb_edges = nnn + nb_faces_bord;

  nvtxs = nb_elem + zone.nb_faces_joint(); //each joint face is linked to a virtual element
  xadj = imalloc(nb_elem+1, "readgraph: xadj");
  adjncy = imalloc(nb_edges + nb_faces_bord, "readgraph: adjncy");
  edgegsttab = imalloc(nb_edges + nb_faces_bord, "readgraph: edgegsttab");
  vwgts = NULL;
  if (! use_weights)
    {
      weightflag = 0;
      ewgts = NULL;
    }
  else
    {
      weightflag = 1;
      ewgts = imalloc(nb_edges + nb_faces_bord, "readgraph: ewgts");
    }

  vtxdist = imalloc(Process::nproc()+1, "readgraph: vtxdist");
  for(int p = 0; p < Process::nproc(); p++)
    vtxdist[p] = offsets[p];
  vtxdist[Process::nproc()] = Process::mp_sum(nb_elem);

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
      xadj[i_elem] = edge_count;
      int i_face;

      if (!is_regular)
        {
          ref_cast(Poly_geom_base,type_elem).get_tab_faces_sommets_locaux(faces_element_reference,i_elem);
          int nb_faces_elem       = faces_element_reference.dimension(0);
          while ( faces_element_reference(nb_faces_elem-1,0)==-1)
            nb_faces_elem--;
          nb_faces_par_element= nb_faces_elem;
        }
      for (i_face = 0; i_face < nb_faces_par_element; i_face++)
        {
          // Construction de cette face de l'element:
          // (indice des sommets de la face dans le domaine)
          {
            int i;
            for (i = 0; i < nb_sommets_par_face; i++)
              {
                const int i_som = faces_element_reference(i_face, i);
                if (i_som<0)
                  une_face[i] = i_som;
                else
                  {
                    const int sommet = elem_som(i_elem, i_som);
                    une_face[i] = sommet;
                  }
              }
          }
          //Cerr << "\tFACE#" << i_face << finl;
          // Recherche des elements voisins de cette face:
          // (indices des elements contenant les sommets de la face)
          find_adjacent_elements(som_elem, une_face, voisins);
          //Cerr << voisins << finl;

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
                if (elem != i_elem + my_offset)
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
                if (elem0 == i_elem + my_offset) //neighbours contain global numerotation
                  elem_voisin = elem1;
                else if (elem1 == i_elem + my_offset)
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
              adjncy[edge_count] = elem_voisin;

              if (use_weights)
                {
                  //internal connection have more weight:
                  //it's better if the generated partition is not dispatched on several processors
                  if( my_offset <= elem_voisin && elem_voisin < nb_elem+my_offset) //neighbour belongs to me = strong connection
                    ewgts[edge_count] = 4;
                  else
                    ewgts[edge_count] = 1; // -1 peut faire des partitions discontinues ou pas equilibrees du tout

                }

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
              adjncy[edge_count] = elem_voisin;
              // Les connexions entre faces periodiques sont fortes:
              // on veut que ces elements soient sur le meme processeur
              // Attention, si on met un poids nettement plus eleve que les autres
              // edges, on degrade la qualite du decoupage !
              if (use_weights)
                {
                  ewgts[edge_count] = 4;
                }
              edge_count++;
            }
        }
      else if (use_weights)
        {
          if(Process::nproc() == 1)
            {
              Cerr << "Warning: You specify use_weights option with Metis but you didn't use Periodique keyword" << finl;
              Cerr << "to define the boundary where periodicity apply." << finl;
              Cerr << "Either suppress use_weight or add Periodique keyword." << finl;
              Process::exit();
            }
        }
      if (error > 0)
        break;
    }
  xadj[nb_elem] = edge_count;
  nedges = edge_count;

  Cerr << "MY_OFFSET = " << my_offset << finl;
  std::map<int,int> global_to_local_index;
  int cnt=nb_elem;
  for(int e=0; e<nb_edges + nb_faces_bord; e++)
    {
      int vertex = adjncy[e];
      if( my_offset <= vertex && vertex < nb_elem+my_offset) //neighbour belongs to me
        {
          edgegsttab[e] = vertex - my_offset;
        }
      else
        {
          std::map<int,int>::iterator it = global_to_local_index.find(vertex);
          if(it != global_to_local_index.end())
            {
              edgegsttab[e] = global_to_local_index[vertex];

            }
          else
            {
              global_to_local_index[vertex] =cnt++;
              edgegsttab[e] = global_to_local_index[vertex];
            }

        }
    }

  Cerr << "XADJ:" << finl;
  for(int i=0; i< nb_elem+1; i++)
    Cerr << xadj[i] << " ";
  Cerr << finl;

  Cerr << "adjncy:" << finl;
  for(int i=0; i< nb_edges + nb_faces_bord; i++)
    Cerr << adjncy[i] << " ";
  Cerr << finl;

  Cerr << "edgegsttab:" << finl;
  for(int i=0; i< nb_edges + nb_faces_bord; i++)
    Cerr << edgegsttab[i] << " ";
  Cerr << finl;

  if (error == 1)
    {
      Cerr << "Error in Domaine_Graph::construire_graph_elem_elem\n"
           << " The number of element-element connections is greater than expected\n"
           << " The number of boundary faces is wrong.\n"
           << " You must discretize the domain to check." << finl;
      Process::exit();
    }
  else if (error == 2)
    {
      Cerr << "Error in Domain_Graph::construire_graph_elem_elem\n"
           << " Problem in the mesh: one internal face has more than two neighboring elements\n"
           << " List of neighboring elements:" << voisins
           << "\n Nodes of the face:" << une_face;
      Process::exit();
    }
  else if (error)
    {
      Cerr << "Internal error in Domain_Graph::construire_graph_elem_elem\n"
           << " (error " << error << ") Problem in neighborhood algorithms"
           << finl;
      Process::exit();
    }

}
#endif
