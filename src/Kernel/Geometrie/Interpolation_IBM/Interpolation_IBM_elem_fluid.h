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
// File      : Interpolation_IBM_elem_fluid.h
// Directory : $GENEPI3_ROOT/src/EF/Interpolation_IBM
//
/////////////////////////////////////////////////////////////////////////////

#ifndef Interpolation_IBM_elem_fluid_included
#define Interpolation_IBM_elem_fluid_included

#include <Interpolation_IBM_base.h>
#include <Champ_Don.h>
#include <Zone.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Interpolation_IBM_elem_fluid
//
// <Description of class Interpolation_IBM_elem_fluid>
//
/////////////////////////////////////////////////////////////////////////////

class Interpolation_IBM_elem_fluid : public Interpolation_IBM_base
{

  Declare_instanciable( Interpolation_IBM_elem_fluid ) ;

public :
  void discretise(const Discretisation_base&, Zone_dis_base& la_zone_EF);
protected :
  void computeFluidElems(Zone_dis_base&);
  Champ_Don solid_points_lu_;
  Champ_Don fluid_points_lu_;
  Champ_Don fluid_elems_lu_;
  Champ_Don corresp_elems_lu_;
  Champ_Don solid_points_;
  Champ_Don fluid_points_;
  Champ_Don fluid_elems_;
  Champ_Don corresp_elems_;
  friend class Source_PDF;
};

#endif /* Interpolation_IBM_elem_fluid_included */
