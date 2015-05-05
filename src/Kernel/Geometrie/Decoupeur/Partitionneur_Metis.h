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
// File:        Partitionneur_Metis.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Partitionneur_Metis_included
#define Partitionneur_Metis_included

#include <Partitionneur_base.h>
#include <Ref_Domaine.h>
#include <metis.h>

// .DESCRIPTION
//  Partition d'un domaine en nb_parties parties equilibrees en utilisant
//  la librairie METIS. Voir construire_partition

class Partitionneur_Metis : public Partitionneur_base
{
  Declare_instanciable(Partitionneur_Metis);
public:

  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void associer_domaine(const Domaine& domaine);
  void construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const;


private:

  // Parametres du partitionneur
  REF(Domaine) ref_domaine_;
  int nb_parties_;

  // Options de metis
  enum AlgoMetis { PMETIS, KMETIS };
  AlgoMetis algo_;
  int match_type_;
  int ip_type_;
  int ref_type_;
  int nb_essais_;

  // Drapeau: utiliser ou pas la ponderation des edges dans metis.
  //  C'est mieux de l'utiliser s'il y a des bords periodiques, le
  //  graphe est mieux equilibre. En revanche, metis utilisera plus
  //  de memoire (peut poser probleme sur les gros maillages).
  //  Cette option n'est pas indispensable: le maillage genere est
  //  valide dans avec ou sans l'option car on verifie de toutes facons
  //  la partition generee par metis (voir (***))
  int use_weights_;
  int use_segment_to_build_connectivite_elem_elem_;


};
#ifndef NO_METIS
typedef struct
{
  const char * name;
  int option;
} Metis_String_Option;

// Metis 5.0 introduit le type idx_t
#ifdef IDXTYPEWIDTH
#define METIS
typedef int idx_t;
#endif

struct graph_def
{
  idx_t nvtxs;                        /* The number of vertices */
  idx_t nedges;                        /* The total number of edges */
  idx_t weightflag;
  idx_t *xadj;                        /* CRS storage scheme */
  idx_t *adjncy;
  idx_t *vwgts;
  idx_t *ewgts;
};

typedef struct graph_def Graph_Type;
idx_t *imalloc(int, const char *);
#endif
#endif
