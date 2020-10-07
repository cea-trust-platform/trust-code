/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Interpolation_IBM_hybrid.h
// Directory : $GENEPI3_ROOT/src/EF/Interpolation_IBM
//
/////////////////////////////////////////////////////////////////////////////

#ifndef Interpolation_IBM_hybrid_included
#define Interpolation_IBM_hybrid_included

#include <Interpolation_IBM_elem_fluid.h>
#include <Interpolation_IBM_mean_gradient.h>
#include <IntList.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Interpolation_IBM_hybrid
//
// <Description of class Interpolation_IBM_hybrid>
//
/////////////////////////////////////////////////////////////////////////////

class Interpolation_IBM_hybrid : public Interpolation_IBM_elem_fluid
{

  Declare_instanciable( Interpolation_IBM_hybrid ) ;

public :
  void discretise(const Discretisation_base&, Zone_dis_base&);
  inline IntList& getSommetsVoisinsOf(int i)
  {
    return sommets_voisins_[i];
  };
protected :
  void computeFluidElems(Zone_dis_base&);
  void computeSommetsVoisins(Zone_dis_base&);
  Champ_Don is_dirichlet_lu_;
  Champ_Don solid_elems_lu_;
  IntList* sommets_voisins_;
  friend class Source_PDF;
  Interpolation_IBM_mean_gradient* my_mean_gradient;

};

#endif /* Interpolation_IBM_hybrid_included */
