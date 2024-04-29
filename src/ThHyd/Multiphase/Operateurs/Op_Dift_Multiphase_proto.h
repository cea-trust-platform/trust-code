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

#ifndef Op_Dift_Multiphase_proto_included
#define Op_Dift_Multiphase_proto_included

#include <Viscosite_turbulente_base.h>
#include <Correlation.h>
#include <Champ_Fonc.h>
#include <TRUST_Ref.h>
#include <vector>

class Champs_compris;
class Pb_Multiphase;
class Equation_base;

class Op_Dift_Multiphase_proto
{
public:
  void associer_proto(const Pb_Multiphase&, Champs_compris& );

  inline const Correlation& correlation() const { return corr_ ; }

  void ajout_champs_op_face();

  void get_noms_champs_postraitables_proto(const Nom& , Noms& nom, Option opt) const;

  void creer_champ_proto(const Motcle& );

  void completer_proto();

  void mettre_a_jour_proto(const double);

  inline DoubleTab& viscosite_turbulente() { return nu_t_; }

  inline void call_compute_nu_turb()
  {
    // remplissage par la correlation : ICI c'est NU_T ET PAS MU_T => m2/s et pas kg/ms
    ref_cast(Viscosite_turbulente_base, corr_.valeur()).eddy_viscosity(nu_t_);
  }

protected:
  DoubleTab nu_t_; // comme le nom dit
  Correlation corr_; // correlation de viscosite/transport turbulente
  std::vector<Champ_Fonc> nu_t_post_; // champ de postraitement
  Motcles noms_nu_t_post_; //leurs noms
  REF(Pb_Multiphase) pbm_;
  REF(Champs_compris) le_chmp_compris_;
};

#endif /* Op_Dift_Multiphase_proto_included */
