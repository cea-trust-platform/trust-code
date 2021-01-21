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
// File:        Domain_Graph.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Domain_Graph_included
#define Domain_Graph_included

// .DESCRIPTION
//  Build the graph of the domain that the METIS/PARMETIS/PTSCOTHC libraries need

#include <metis.h>

#ifndef NO_METIS
// Metis 5.0 introduit le type idx_t
#ifdef IDXTYPEWIDTH
#define METIS
typedef int idx_t;
#endif
#endif

class Domain_Graph
{

  friend class Partitionneur_Metis;
  friend class Partitionneur_Parmetis;
public:

  void construire_graph_from_segment(const Domaine& dom,
                                     const int use_weights );

  void construire_graph_elem_elem(const Domaine& dom,
                                  const Noms& liste_bords_periodiques,
                                  const int use_weights,
                                  Static_Int_Lists& graph_elements_perio);


  void free_memory();

private:
#ifndef NO_METIS
  idx_t nvtxs;                        /* The number of vertices */
  idx_t nedges;                        /* The total number of edges */
  idx_t weightflag;
  idx_t *xadj;                        /* CRS storage scheme */
  idx_t *adjncy;
  idx_t *vwgts;
  idx_t *ewgts;
  idx_t *vtxdist;                     //for parmetis: distribution of the initial graph on the procs
#endif

};

#endif
