/****************************************************************************
* Copyright (c) 2024, CEA
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
#ifndef Domain_Graph_included
#define Domain_Graph_included

#include <metis.h>   // to have idx_t, the entity index of METIS (32 or 64b depending on how METIS/PETSc was compiled)

#ifndef MPI_

#ifdef idx_t
#error Type idx_t is alreay defined without MPI, and hence without PETSc - this Should not happen
#endif

#define idx_t trustIdType
#endif

/*! @brief Build the graph of the domain that the METIS/PARMETIS/PTSCOTCH libraries need.
 *
 */
class Domain_Graph
{
public:

  using ArrOfIdx_ = TRUSTArray<idx_t, idx_t>;

  template <typename _SIZE_>
  void construire_graph_from_segment(const Domaine_32_64<_SIZE_>& dom,
                                     bool use_weights );

  template <typename _SIZE_>
  void construire_graph_elem_elem(const Domaine_32_64<_SIZE_>& dom,
                                  const Noms& liste_bords_periodiques,
                                  bool use_weights,
                                  Static_Int_Lists_32_64<_SIZE_>& graph_elements_perio);

  //
  // All members are public to facilitate access from Partitionneur_* classes (rather than friend-ing everything):
  //

  idx_t nvtxs;                /* The number of vertices */
  idx_t nedges;               /* The total number of edges */
  idx_t weightflag;
  ArrOfIdx_ xadj;             /* CRS storage scheme */
  ArrOfIdx_ adjncy;
  ArrOfIdx_ vwgts;
  ArrOfIdx_ ewgts;
  ArrOfIdx_ vtxdist;          // for parmetis: distribution of the initial graph on the procs
  ArrOfIdx_ edgegsttab;       // for ptscotch: local numbering of vertices
};

#endif
