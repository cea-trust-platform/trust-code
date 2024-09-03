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
#include <Transport_turbulent_base.h>
#include <Correlation.h>
#include <Champ_Fonc.h>
#include <TRUST_Ref.h>
#include <vector>

class Operateur_Diff_base;
class Champs_compris;
class Probleme_base;
class Equation_base;

class Op_Dift_Multiphase_proto
{
private:
  void ajout_champs_(const bool /* is_face */);
  void creer_champ_(const Motcle& , const bool /* is_face */);
  void completer_(const Operateur_Diff_base&, const bool /* is_face */);
  void mettre_a_jour_(const double, const bool /* is_face */);

public:
  void associer_proto(const Probleme_base&, Champs_compris& );

  inline const Correlation& correlation() const { return corr_ ; }

  void ajout_champs_proto_face() { ajout_champs_(true); }
  void ajout_champs_proto_elem() { ajout_champs_(false); }

  void get_noms_champs_postraitables_proto(const Nom& , Noms& nom, Option opt) const;

  void creer_champ_proto_face(const Motcle& motlu) { creer_champ_(motlu, true); }
  void creer_champ_proto_elem(const Motcle& motlu) { creer_champ_(motlu, false); }

  void completer_proto_face(const Operateur_Diff_base& op) { completer_(op, true); }
  void completer_proto_elem(const Operateur_Diff_base& op) { completer_(op, false); }

  void mettre_a_jour_proto_face(const double temps) { mettre_a_jour_(temps, true); }
  void mettre_a_jour_proto_elem(const double temps) { mettre_a_jour_(temps, false); }

  inline DoubleTab& viscosite_turbulente() { return nu_ou_lambda_turb_; }
  inline DoubleTab& diffusivite_turbulente() { return nu_ou_lambda_turb_; }

  // remplissage par la correlation : ICI c'est NU_T ET PAS MU_T => m2/s et pas kg/ms
  inline void call_compute_nu_turb()
  {
    ref_cast(Viscosite_turbulente_base, corr_.valeur()).eddy_viscosity(nu_ou_lambda_turb_);
  }

  // remplissage par la correlation : ICI c'est LAMBDA_T ET PAS ALPHA_T => W/mK et pas m2/s
  inline void call_compute_diff_turb(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb)
  {
    ref_cast(Transport_turbulent_base, corr_.valeur()).modifier_mu(eq, visc_turb, nu_ou_lambda_turb_);
  }

protected:
  bool is_pbm_ = true;
  DoubleTab nu_ou_lambda_turb_; // comme le nom dit
  Correlation corr_; // correlation de viscosite/transport turbulente
  std::vector<Champ_Fonc> nu_ou_lambda_turb_post_, mu_ou_alpha_turb_post_; // champ de postraitement
  Motcles noms_nu_ou_lambda_turb_post_, noms_mu_ou_alpha_turb_post_; //leurs noms
  REF(Probleme_base) pbm_;
  REF(Champs_compris) le_chmp_compris_;
};

#endif /* Op_Dift_Multiphase_proto_included */
