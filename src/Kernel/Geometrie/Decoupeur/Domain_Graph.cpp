/****************************************************************************
* Copyright (c) 2025, CEA
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

namespace
{

template <typename _SIZE_>
void construire_connectivite_real_som_virtual_elem(const _SIZE_       nb_sommets,
                                                   const IntTab_T<_SIZE_>&      les_elems,
                                                   Static_Int_Lists_32_64<_SIZE_>& som_elem,
                                                   const IntTab_T<_SIZE_>& elem_virt_pe_num,
                                                   const SmallArrOfTID_T<_SIZE_>& offsets)
{
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;

  // Nombre d'elements du domaine
  const int_t nb_elem = les_elems.dimension_tot(0);
  const int_t local_nb_elem = les_elems.dimension(0);
  // Nombre de sommets par element
  const int nb_sommets_par_element = les_elems.dimension_int(1);

  // Construction d'un tableau initialise a zero : pour chaque sommet,
  // nombre d'elements voisins de ce sommet
  ArrOfInt_t nb_elements_voisins(nb_sommets);

  // Premier passage : on calcule le nombre d'elements voisins de chaque
  // sommet pour creer la structure de donnees

  //real elements
  for (int_t elem = 0; elem < nb_elem; elem++)
    {
      for (int i = 0; i < nb_sommets_par_element; i++)
        {
          int_t sommet = les_elems(elem, i);
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
  for (int_t elem = 0; elem < nb_elem; elem++)
    {
      for (int i = 0; i < nb_sommets_par_element; i++)
        {
          int_t sommet = les_elems(elem, i);
          if(sommet >= nb_sommets) continue;
          // GF cas des polyedres
          if (sommet==-1) break;
          int_t n = (nb_elements_voisins[sommet])++;

          int_t elem_num_global = -1;
          if(elem>= local_nb_elem)
            {
              int proc_of_elem = static_cast<int>(elem_virt_pe_num(elem-local_nb_elem, 0));  // first col of this array is always int
              int_t elem_number_on_local_proc = elem_virt_pe_num(elem-local_nb_elem, 1);
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
}

template <typename _SIZE_>
void Domain_Graph::construire_graph_from_segment(const Domaine_32_64<_SIZE_>& dom,
                                                 bool use_weights)
{
  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;

  const IntTab_t& liaisons = dom.les_elems();

  // ****************************************************************
  // PREMIERE ETAPE: calcul du nombre de vertex et edges du graph:
  int_t nb_edges = liaisons.size_array(); // 2 liens par liaison
  int_t nb_elem=liaisons.local_max_vect()+1;  // mouif <- [ABN] lol

  assert(nb_elem < std::numeric_limits<_SIZE_>::max());

  nvtxs = nb_elem;
  xadj.resize_array(nb_elem+1);
  vwgts.resize_array(0);
  if (! use_weights)
    {
      weightflag = 0;
      ewgts.resize_array(0);
    }
  else
    {
      abort();
      weightflag = 1;
      ewgts.resize_array(nb_edges);
    }

  // on construit connectivite item item
  IntTab_t stencil(0,2);

  int_t size=0;
  int_t nbl=liaisons.dimension(0);
  for (int i=0; i<nbl; i++)
    {
      stencil.resize(size+2,2);
      int_t n1=liaisons(i,0);
      int_t n2=liaisons(i,1);
      {
        stencil(size,0)=n1;
        stencil(size,1)=n2;

        size++;
        stencil(size,0)=n2;
        stencil(size,1)=n1;

        size++;
      }
    }
  tableau_trier_retirer_doublons(stencil);

  // Dimensioning tab1, tab2
  IntTab_t tab1, tab2;
  tab1.resize(nb_elem+1);
  tab1 = 1;
  tab1[nb_elem]=size+1;
  tab2.resize(size);

  Matrix_tools::fill_csr_arrays(nb_elem,nb_elem,stencil,tab1, tab2);

  nb_edges=tab2.size_array(); // des liens peuvent etre doubles
  nedges = nb_edges;
  adjncy.resize_array(nb_edges);
  //
  assert(tab1.size_array()==nvtxs+1);

  for (int_t c=0; c<static_cast<_SIZE_>(nvtxs+1); c++)  // overflow check done before
    xadj[c]=tab1[c]-1;
  for (int_t c=0; c<static_cast<_SIZE_>(nedges); c++)
    adjncy[c]=tab2[c]-1;
}

// Si use_weights, on pondere les liens entre les elements periodiques
// pour les forcer a etre sur le meme processeur. Cela diminue le nombre
// de corrections a faire ensuite (voir (***))
template<typename _SIZE_>
void Domain_Graph::construire_graph_elem_elem(const Domaine_32_64<_SIZE_>& dom,
                                              const Noms& liste_bords_periodiques,
                                              bool use_weights,
                                              Static_Int_Lists_32_64<_SIZE_>& graph_elements_perio)
{
  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;
  using Poly_geom_base_t = Poly_geom_base_32_64<_SIZE_>;

  Static_Int_Lists_32_64<_SIZE_> som_elem;
  const Elem_geom_base_32_64<_SIZE_>& type_elem = dom.type_elem().valeur();
  IntTab faces_element_reference;
  const int is_regular = type_elem.get_tab_faces_sommets_locaux(faces_element_reference);
  // Invoke proper method if Polygon or Polyedron ...:
  if (! is_regular)
    ref_cast(Poly_geom_base_t, type_elem).get_tab_faces_sommets_locaux(faces_element_reference,0);
  int nb_faces_par_element = faces_element_reference.dimension(0);
  const int nb_sommets_par_face = faces_element_reference.dimension(1);

  const IntTab_t& elem_som = dom.les_elems();
  const int_t nb_elem = dom.nb_elem();

  SmallArrOfTID_t offsets(Process::nproc());
  trustIdType totsum = Process::mppartial_sum(nb_elem);
  if (totsum > std::numeric_limits<_SIZE_>::max())
    {
      Cerr << "Are you trying to partition a Domain_64 with a non-64b 'Partitionneur'? Total number of elements is too big!" << finl;
      Process::exit(-1);
    }
  offsets = static_cast<_SIZE_>(totsum);
  envoyer_all_to_all(offsets, offsets);
  int_t my_offset = offsets[Process::me()];

  Cerr << " Construction of the som_elem connectivity" << finl;
  if(Process::is_parallel())
    {
      //what we're doing here is not equivalent to construire_connectivite_som_elem with virtual elements set to 1
      //in the latter, we also build the connectivity for virtual nodes
      //here, we add the connectivity of real nodes only with virtual elements
      // + we want som_elem to contain global numerotation for elements
      IntTab_t elem_virt_pe_num;
      dom.construire_elem_virt_pe_num(elem_virt_pe_num);
      construire_connectivite_real_som_virtual_elem(dom.nb_som(), elem_som, som_elem, elem_virt_pe_num, offsets);
    }
  else
    construire_connectivite_som_elem(dom.nb_som(), elem_som, som_elem,
                                     0 /* ne pas inclure les elements virtuels */);

  int_t nb_connexions_perio = 0;
  if (liste_bords_periodiques.size() > 0)
    {
      Cerr << " Construction of graph connectivity for periodic boundaries" << finl;
      nb_connexions_perio = Partitionneur_base_32_64<_SIZE_>::calculer_graphe_connexions_periodiques(dom,
                                                                                                     liste_bords_periodiques,
                                                                                                     som_elem,
                                                                                                     my_offset,
                                                                                                     graph_elements_perio);
    }

  // ****************************************************************
  // PREMIERE ETAPE: calcul du nombre de vertex et edges du graph:

  // Nombre total de faces de bord:
  const int_t nb_faces_bord = dom.nb_faces_frontiere();

  // Chaque element du maillage est un "vertex" du graph.
  // Les "edges" du graph relient chaque element a ses voisins par une face.
  // Il y a autant d'edges que de faces ayant deux voisins, fois 2
  // Formule classique: nb_faces internes = nnn/2 avec :
  int_t nnn = nb_elem * nb_faces_par_element - nb_faces_bord + nb_connexions_perio;
  if (sub_type(Poly_geom_base_t, dom.type_elem().valeur()))
    {
      const Poly_geom_base_t& poly=ref_cast(Poly_geom_base_t,dom.type_elem().valeur());
      nnn= poly.get_somme_nb_faces_elem() - nb_faces_bord + nb_connexions_perio;
    }

  const int_t nb_edges = nnn + nb_faces_bord;

  nvtxs = nb_elem + dom.nb_faces_joint(); //each joint face is linked to a virtual element
  xadj.resize_array(nb_elem+1);
  xadj = -1;
  adjncy.resize_array(nb_edges+nb_faces_bord);
  adjncy = -1;
  edgegsttab.resize_array(nb_edges+nb_faces_bord);
  edgegsttab = -1;

  vwgts.resize_array(0); //nullptr
  if (! use_weights)
    {
      weightflag = 0;
      ewgts.resize_array(0); //nullptr
    }
  else
    {
      weightflag = 1;
      ewgts.resize_array(nb_edges + nb_faces_bord);
    }

  vtxdist.resize_array(Process::nproc()+1);
  for(int p = 0; p < Process::nproc(); p++)
    vtxdist[p] = offsets[p];
  // TODO IG mp_sum
  vtxdist[Process::nproc()] = Process::mp_sum((int)nb_elem);

  Cerr << " Construction of the elem_elem connectivity" << finl;
  // ***************************************************************
  // DEUXIEME ETAPE: remplissage du graph
  //  Algorithme: Pour chaque element, boucle sur les faces de l'element
  //  et on ajoute un "edge" entre l'element et ses voisins. On cherche
  //  l'element voisin par une face a l'aide de la fonction find_adjacent_elements.
  //
  // Deux tableaux de travail:
  SmallArrOfTID_t une_face(nb_sommets_par_face); // Les sommets de la face en cours
  SmallArrOfTID_t voisins; // Les elements voisins d'une_face

  int error = 0;
  int_t edge_count = 0;
  for (int_t i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      xadj[i_elem] = edge_count;

      if (!is_regular)
        {
          ref_cast(Poly_geom_base_t,type_elem).get_tab_faces_sommets_locaux(faces_element_reference,i_elem);
          int nb_faces_elem       = faces_element_reference.dimension(0);
          while ( faces_element_reference(nb_faces_elem-1,0)==-1)
            nb_faces_elem--;
          nb_faces_par_element= nb_faces_elem;
        }
      for (int i_face = 0; i_face < nb_faces_par_element; i_face++)
        {
          // Construction de cette face de l'element:
          // (indice des sommets de la face dans le domaine)
          {
            for (int i = 0; i < nb_sommets_par_face; i++)
              {
                const int i_som = faces_element_reference(i_face, i);
                if (i_som<0)
                  une_face[i] = i_som;
                else
                  {
                    const int_t sommet = elem_som(i_elem, i_som);
                    une_face[i] = sommet;
                  }
              }
          }
          // Recherche des elements voisins de cette face:
          // (indices des elements contenant les sommets de la face)
          find_adjacent_elements(som_elem, une_face, voisins);

          const int nb_voisins = voisins.size_array();
          int_t elem_voisin = -1;
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
                const int_t elem = voisins[0];
                if (elem != i_elem + my_offset)
                  error = 3; // l'element i_elem n'est pas voisin: erreur interne
                else
                  elem_voisin = -1;
                break;
              }
            case 2:
              {
                // Le cas le plus courant:
                const int_t elem0 = voisins[0];
                const int_t elem1 = voisins[1];
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
          const int_t n = graph_elements_perio.get_list_size(i_elem);
          for (int_t i = 0; i < n; i++)
            {
              const int_t elem_voisin = graph_elements_perio(i_elem, i);
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
                ewgts[edge_count] = 4;
              edge_count++;
            }
        }
      else if (use_weights)
        {
          if(Process::is_sequential())
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

  std::map<int_t,int_t> global_to_local_index;
  int_t cnt=nb_elem;
  for(int_t e=0; e<nb_edges + nb_faces_bord; e++)
    {
      int_t vertex = static_cast<_SIZE_>(adjncy[e]);  // overflow check done aboveS
      if( my_offset <= vertex && vertex < nb_elem+my_offset) //neighbour belongs to me
        edgegsttab[e] = vertex - my_offset;
      else
        {
          if(global_to_local_index.find(vertex) != global_to_local_index.end())
            edgegsttab[e] = global_to_local_index[vertex];
          else
            {
              global_to_local_index[vertex] =cnt++;
              edgegsttab[e] = global_to_local_index[vertex];
            }
        }
    }

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


// Explicit instanciations:
template void Domain_Graph::construire_graph_from_segment(const Domaine_32_64<int>& dom, bool use_weights );
template void Domain_Graph::construire_graph_elem_elem(const Domaine_32_64<int>& dom, const Noms& liste_bords_periodiques,
                                                       bool use_weights, Static_Int_Lists_32_64<int>& graph_elements_perio);

#if INT_is_64_ == 2
template void Domain_Graph::construire_graph_from_segment(const Domaine_32_64<trustIdType>& dom, bool use_weights );
template void Domain_Graph::construire_graph_elem_elem(const Domaine_32_64<trustIdType>& dom, const Noms& liste_bords_periodiques,
                                                       bool use_weights, Static_Int_Lists_32_64<trustIdType>& graph_elements_perio);
#endif
