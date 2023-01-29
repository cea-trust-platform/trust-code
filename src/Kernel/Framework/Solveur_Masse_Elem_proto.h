/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Solveur_Masse_Elem_proto_included
#define Solveur_Masse_Elem_proto_included

#include <Ref_Solveur_Masse_base.h>
#include <TRUSTTabs_forward.h>
#include <Interface_blocs.h>
#include <TRUST_Ref.h>

class Domaine_VF;

class Solveur_Masse_Elem_proto
{
public:
  void associer_masse_proto(const Solveur_Masse_base& , const Domaine_VF&) ;
  void preparer_calcul_proto();
  DoubleTab& appliquer_impl_proto(DoubleTab& ) const;
  void dimensionner_blocs_proto(matrices_t , const tabs_t& semi_impl = {}) const;
  void ajouter_blocs_proto(matrices_t , DoubleTab& , double , const tabs_t& , int ) const;

protected:
  REF(Solveur_Masse_base) solv_mass_;
  REF2(Domaine_VF) le_dom_;
};

#endif /* Solveur_Masse_Elem_proto_included */
