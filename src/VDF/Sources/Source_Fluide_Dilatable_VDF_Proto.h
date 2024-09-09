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

#ifndef Source_Fluide_Dilatable_VDF_Proto_included
#define Source_Fluide_Dilatable_VDF_Proto_included

#include <TRUSTTabs_forward.h>
#include <Domaine_Cl_dis.h>
#include <Domaine_dis.h>
#include <TRUST_Ref.h>

class Domaine_Cl_VDF;
class Equation_base;
class Domaine_VDF;

class Source_Fluide_Dilatable_VDF_Proto
{
protected:
  void associer_domaines_impl(const Domaine_dis& domaine,const Domaine_Cl_dis& domaine_cl);
  void associer_volume_porosite_impl(const Domaine_dis& domaine, DoubleVect& volumes, DoubleVect& porosites);
  void ajouter_impl( const DoubleVect& g, const double rho_m, const DoubleTab& tab_rho, DoubleTab& resu) const;

  REF(Domaine_Cl_VDF) le_dom_Cl;
  REF(Domaine_VDF) le_dom;
};

#endif /* Source_Fluide_Dilatable_VDF_Proto_included */
