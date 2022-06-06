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
#include <Partitionneur_Metis.h>
#include <Domaine.h>
#include <Static_Int_Lists.h>

#include <Param.h>

#include <communications.h>
#include <Domain_Graph.h>
#include <metis.h>

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

  if(Process::nproc() >1)
    {
      Cerr << "WARNING! You're using a sequential algorithm on " << nproc() << "processors " << finl;
      Cerr << "Use PARMETIS for parallel domain cutting" << finl;
      Process::exit();
    }
  return 1;
}

void Partitionneur_Metis::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

// Description:
//  Calcule le graphe de connectivite pour Metis, appelle le partitionneur
//  et remplit elem_part (pour chaque element, numero de la partie qui lui
//  est attribuee).
//  Les parties sont equilibrees de facon a minimiser le nombre de faces de joint
//  et a equilibrer le nombre d'elements par partie.
// Precondition:
//  domaine associe et nombre de parties initialise
void Partitionneur_Metis::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
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

      int nb_elem = ref_domaine_.valeur().zone(0).nb_elem_tot();
      if (use_segment_to_build_connectivite_elem_elem_==1)
        nb_elem = ref_domaine_.valeur().nb_som();
      elem_part.resize(nb_elem);
      elem_part = 0;
      return;
    }

  if (ref_domaine_.valeur().zone(0).nb_elem() == 0)
    return;

  Cerr << "Partitionneur_Metis::construire_partition" << finl;
  Cerr << " Construction of graph connectivity..." << finl;
  Static_Int_Lists graph_elements_perio;
  //const Domaine& dom = ref_domaine_.valeur();
  Domain_Graph graph;
  if (use_segment_to_build_connectivite_elem_elem_==0)
    {
      graph.construire_graph_elem_elem(ref_domaine_.valeur(), liste_bords_periodiques_,
                                       use_weights_,
                                       graph_elements_perio);

    }
  else
    {
      graph.construire_graph_from_segment(ref_domaine_.valeur(), use_weights_);

    }
  std::vector<int> partition(graph.nvtxs);
  int int_parts = nb_parties_;
  int edgecut = 0; // valeur renvoyee par metis (nombre total de faces de joint)

  switch(algo_)
    {
    case PMETIS:
      {
        Cerr << "===============" << finl;
        Cerr << "Call for PMETIS" << finl;
        Cerr << "===============" << finl;
        // Voir le manual.pdf de METIS 5.0
        int options[METIS_NOPTIONS];
        METIS_SetDefaultOptions(options);
        //options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB|METIS_PTYPE_KWAY; // Methode de partitionnement
        //options[METIS_OPTION_OBJTYPE]=METIS_OBJTYPE_CUT|METIS_OBJTYPE_VOL; // Objective type
        //options[METIS_OPTION_CTYPE]=METIS_CTYPE_SHEM|METIS_CTYPE_RM; // Matching scheme during coarsening
        //options[METIS_OPTION_IPTYPE]=METIS_IPTYPE_GROW; // Algorithm during initial partitioning
        //options[METIS_OPTION_RTYPE]=METIS_RTYPE_FM;   // Algorithm for refinement
        options[METIS_OPTION_NCUTS]=nb_essais_;         // Nombre de partitionnements testes pour en prendre le meilleur
        options[METIS_OPTION_NUMBERING]=0;              // Numerotation C qui demarre a 0
        options[METIS_OPTION_DBGLVL]=111111111;         // Mode verbose maximal
        //options[METIS_OPTION_NO2HOP]=1;                 // 5.1.0: not perform any 2-hop matchings (as 5.0.3)
        int ncon=1;

        // Implementation reduite (plusieurs valeurs par defaut->NULL) pour METIS 5.0
        int status = METIS_PartGraphRecursive(&graph.nvtxs, &ncon, graph.xadj.addr(),
                                              graph.adjncy.addr(), graph.vwgts.addr(), NULL, graph.ewgts.addr(),
                                              &int_parts, NULL, NULL, options,
                                              &edgecut, partition.data());
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
        int options[METIS_NOPTIONS];
        METIS_SetDefaultOptions(options);
        //options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB|METIS_PTYPE_KWAY; // Methode de partitionnement
        //options[METIS_OPTION_OBJTYPE]=METIS_OBJTYPE_CUT|METIS_OBJTYPE_VOL; // Objective type
        //options[METIS_OPTION_CTYPE]=METIS_CTYPE_SHEM; // Matching scheme during coarsening
        //options[METIS_OPTION_IPTYPE]=METIS_IPTYPE_GROW; // Algorithm during initial partitioning
        //options[METIS_OPTION_RTYPE]=METIS_RTYPE_FM; // Algorithm for refinement
        options[METIS_OPTION_NCUTS]=nb_essais_;         // Nombre de partitionnements testes pour en prendre le meilleur
        options[METIS_OPTION_NUMBERING]=0;     // Numerotation C qui demarre a 0
        options[METIS_OPTION_DBGLVL]=111111111; // Mode verbose maximal
        int ncon=1;
        // Conseil de la doc Metis 4.0 : METIS_PartGraphKway si int_parts>8, METIS_PartGraphRecursive sinon...
        // En effet semble plus rapide, mais edgecut en sortie est moins bon...
        int status = METIS_PartGraphKway(&graph.nvtxs, &ncon, graph.xadj.addr(),
                                         graph.adjncy.addr(), graph.vwgts.addr(), NULL, graph.ewgts.addr(),
                                         &int_parts, NULL, NULL, options ,
                                         &edgecut, partition.data());
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
  Cerr << "Partitioning quality : edgecut = " << edgecut << finl;
  Cerr << "-> It is roughly the total number of edges (faces) which will be shared by the processors." << finl;
  Cerr << "-> The lesser this number is, the lesser the total volume of communication between processors." << finl;
  Cerr << "-> You can increase nb_essais option (default 1) to try to reduce (but at a higher CPU cost) this number." << finl;
  Cerr << "===============" << finl;

  const int n = graph.nvtxs;
  elem_part.resize(n);
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

  if (use_segment_to_build_connectivite_elem_elem_==0)
    {
      Cerr << "Correction elem0 on processor 0" << finl;
      corriger_elem0_sur_proc0(elem_part);
    }

#endif
}

