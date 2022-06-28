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
#include <Partitionneur_Parmetis.h>
#include <Domaine.h>
#include <Static_Int_Lists.h>
#include <parmetis++.h>
#include <Param.h>

#include <communications.h>
#include <Poly_geom_base.h>
#include <Matrix_tools.h>
#include <Matrice_Morse.h>
#include <Array_tools.h>
#include <Comm_Group_MPI.h>

#include <Domain_Graph.h>
#include <MD_Vector_tools.h>

inline void not_implemented(const Nom& chaine)
{
  Cerr << chaine << " is not implemented yet to the PARMETIS API." << finl;
  Process::exit();
}

Implemente_instanciable_sans_constructeur(Partitionneur_Parmetis,"Partitionneur_Parmetis",Partitionneur_base);

Partitionneur_Parmetis::Partitionneur_Parmetis()
{
  nb_parties_ = -1;
  use_weights_ = 0;
}

Sortie& Partitionneur_Parmetis::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Parmetis::printOn invalid\n" << finl;
  exit();
  return os;
}

Entree& Partitionneur_Parmetis::readOn(Entree& is)
{
  Partitionneur_base::readOn(is);
  return is;
}

void Partitionneur_Parmetis::set_param(Param& param)
{
  param.ajouter("nb_parts",&nb_parties_,Param::REQUIRED);
  param.ajouter_condition("(value_of_nb_parts_ge_1)_and_(value_of_nb_parts_le_100000)","The following condition must be satisfied : 1 <= nb_parties <= 100000");
  param.ajouter_flag("use_weights",&use_weights_);
}

int Partitionneur_Parmetis::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Partitionneur_base::lire_motcle_non_standard(mot,is);
}

void Partitionneur_Parmetis::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

// Description:
//  Calcule le graphe de connectivite pour parmetis, appelle le partitionneur
//  et remplit elem_part (pour chaque element, numero de la partie qui lui
//  est attribuee).
void Partitionneur_Parmetis::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
#ifndef PARMETIS_
  Cerr << "PARMETIS is not compiled with this version. Use another partition tool like Tranche." << finl;
  Process::exit();
#else
  if (!ref_domaine_.non_nul())
    {
      Cerr << "Error in Partitionneur_Parmetis::construire_partition\n";
      Cerr << " The domain has not been associated" << finl;
      exit();
    }
  if (nb_parties_ <= 0)
    {
      Cerr << "Error in Partitionneur_Parmetis::construire_partition\n";
      Cerr << " The parts number has not been initialized" << finl;
      exit();
    }

  // Cas particulier: si nb_parts == 1, METIS ne veut rien faire...
  //ToDo: I don't know is that's the case with ParMetis
  if (nb_parties_ == 1)
    {

      int nb_elem = ref_domaine_.valeur().zone(0).nb_elem();
      elem_part.resize_array(nb_elem);
      elem_part = 0;
      return;
    }

  if (ref_domaine_.valeur().zone(0).nb_elem() == 0)
    return;

  Cerr << "Partitionneur_Parmetis::construire_partition" << finl;
  Cerr << " Construction of graph connectivity..." << finl;
  Static_Int_Lists graph_elements_perio;
  //const Domaine& dom = ref_domaine_.valeur();
  Domain_Graph graph;
  graph.construire_graph_elem_elem(ref_domaine_.valeur(), liste_bords_periodiques_,
                                   use_weights_,
                                   graph_elements_perio);


  std::vector<idx_t> partition(graph.nvtxs);

  idx_t int_parts = nb_parties_;
  idx_t edgecut = 0; // valeur renvoyee par parmetis (nombre total de faces de joint)
  Cerr << " Call for PARMETIS" << finl;
  idx_t options[3];
  options[0] = 1; //personnalized options
  options[1] = 111111111; // Mode verbose maximal;
  options[2] = 0; //random seed

  idx_t ncon=1;
  real_t ubvec = 1.05f; //recommanded value
  idx_t numflag = 0; //numerotation C
  std::vector<real_t> tpwgts(ncon*int_parts, (real_t)(1.0/int_parts)); //we want the weight to be equally distributed on each sub_somain
  MPI_Comm comm = Comm_Group_MPI::get_trio_u_world();
  int status = ParMETIS_V3_PartKway(graph.vtxdist.addr(), graph.xadj.addr(), graph.adjncy.addr(),
                                    graph.vwgts.addr(), graph.ewgts.addr(), &graph.weightflag,
                                    &numflag, &ncon, &int_parts, tpwgts.data(), &ubvec, options ,
                                    &edgecut, partition.data(), &comm);
  if (status != METIS_OK)
    {
      Cerr << "Call to PARMETIS failed." << finl;
      if (status == METIS_ERROR)        Cerr << "It seems there is a PARMETIS internal error." << finl;
      Cerr << "Contact TRUST support." << finl;
      exit();
    }

  Cerr << "Partitioning quality : edgecut = " << edgecut << finl;
  Cerr << "-> It is roughly the total number of edges (faces) which will be shared by the processors." << finl;
  Cerr << "-> The lesser this number is, the lesser the total volume of communication between processors." << finl;
  Cerr << "-> You can increase nb_essais option (default 1) to try to reduce (but at a higher CPU cost) this number." << finl;
  Cerr << "===============" << finl;

  MD_Vector_tools::creer_tableau_distribue(ref_domaine_.valeur().zone(0).md_vector_elements(), elem_part);
  const int n = ref_domaine_.valeur().zone(0).nb_elem();
  for (int i = 0; i < n; i++)
    elem_part[i] = partition[i];

  // Correction de la partition pour la periodicite. (***)
  if (graph_elements_perio.get_nb_lists() > 0)
    {
      Cerr << "Correction of the partition for the periodicity" << finl;
      corriger_bords_avec_liste(ref_domaine_.valeur(),
                                liste_bords_periodiques_,
                                0,
                                elem_part);
      Cerr << "  If this number is high, we can improve the splitting with the option use_weights\n"
           << "  but it takes more memory)" << finl;
    }

  Cerr << "Correction elem0 on processor 0" << finl;
  corriger_elem0_sur_proc0(elem_part);
  elem_part.echange_espace_virtuel();

#endif
}

