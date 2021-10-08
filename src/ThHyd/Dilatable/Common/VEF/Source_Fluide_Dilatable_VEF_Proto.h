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
// File:        Source_Fluide_Dilatable_VEF_Proto.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/VEF
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Fluide_Dilatable_VEF_Proto_included
#define Source_Fluide_Dilatable_VEF_Proto_included

#include <Ref_Zone_Cl_VEF.h>
#include <Ref_Zone_VEF.h>

class Equation_base;
class Zone_Cl_dis;
class DoubleVect;
class DoubleTab;
class Zone_dis;

class Source_Fluide_Dilatable_VEF_Proto
{
protected:
  void associer_zones_impl(const Zone_dis& zone,const Zone_Cl_dis& zone_cl);
  void associer_volume_porosite_impl(const Zone_dis& zone, DoubleVect& volumes, DoubleVect& porosites);
  void ajouter_impl(const Equation_base& eqn, const DoubleVect& g,
                    const int dimension, const double rho_m,
                    const DoubleTab& tab_rho, DoubleTab& resu) const;

  REF(Zone_VEF) la_zone;
  REF(Zone_Cl_VEF) la_zone_Cl;
};

#endif /* Source_Fluide_Dilatable_VEF_Proto_included */
