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

#ifndef Interpolation_IBM_mean_gradient_proto_included
#define Interpolation_IBM_mean_gradient_proto_included

#include <Domaine_dis_base.h>
#include <TRUSTLists.h>


/*! @brief : class Interpolation_IBM_mean_gradient_proto
 *
 *  Pure C++ class to allow multiple inheritance in Interpolation_IBM_hybrid
 *
 *
 *
 */

class Interpolation_IBM_mean_gradient_proto
{

public :
  inline IntList& getSommetsVoisinsOf(int i)
  {
    return sommets_voisins_[i];
  };

protected :
  void computeSommetsVoisins(Domaine_dis_base& le_dom_EF, const Champ_Don_base& solid_points, const Champ_Don_base& corresp_elems, bool has_corres);

  OWN_PTR(Champ_Don_base) is_dirichlet_lu_;
  OWN_PTR(Champ_Don_base) is_dirichlet_;

  OWN_PTR(Champ_Don_base) solid_elems_lu_;
  OWN_PTR(Champ_Don_base) solid_elems_;

  IntLists sommets_voisins_;
};

#endif /* Interpolation_IBM_mean_gradient_proto_included */
