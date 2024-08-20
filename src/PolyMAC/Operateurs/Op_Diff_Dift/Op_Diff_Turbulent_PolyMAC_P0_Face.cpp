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

#include <Op_Diff_Turbulent_PolyMAC_P0_Face.h>
#include <PolyMAC_P0_discretisation.h>
#include <Op_Diff_PolyMAC_P0_Face.h>
#include <Pb_Multiphase.h>
#include <Pb_Multiphase.h>
#include <vector>

Implemente_instanciable( Op_Diff_Turbulent_PolyMAC_P0_Face, "Op_Diff_Turbulent_PolyMAC_P0_Face|Op_Diff_Turbulente_PolyMAC_P0_Face", Op_Diff_PolyMAC_P0_Face );

Sortie& Op_Diff_Turbulent_PolyMAC_P0_Face::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0_base::printOn(os); }

Entree& Op_Diff_Turbulent_PolyMAC_P0_Face::readOn(Entree& is)
{
  //lecture de la correlation de viscosite turbulente
  Correlation_base::typer_lire_correlation(corr_, equation().probleme(), "viscosite_turbulente", is);
  associer_proto(ref_cast(Pb_Multiphase, equation().probleme()), champs_compris_);
  ajout_champs_proto_face();
  return is;
}

void Op_Diff_Turbulent_PolyMAC_P0_Face::get_noms_champs_postraitables(Noms& noms,Option opt) const
{
  Op_Diff_PolyMAC_P0_Face::get_noms_champs_postraitables(noms,opt);
  get_noms_champs_postraitables_proto(que_suis_je(), noms, opt);
}

void Op_Diff_Turbulent_PolyMAC_P0_Face::creer_champ(const Motcle& motlu)
{
  Op_Diff_PolyMAC_P0_Face::creer_champ(motlu);
  creer_champ_proto_face(motlu);
}

void Op_Diff_Turbulent_PolyMAC_P0_Face::completer()
{
  Op_Diff_PolyMAC_P0_Face::completer();
  completer_proto_face(*this);
}

void Op_Diff_Turbulent_PolyMAC_P0_Face::preparer_calcul()
{
  Op_Diff_PolyMAC_P0_Face::preparer_calcul();
  call_compute_nu_turb();
}

void Op_Diff_Turbulent_PolyMAC_P0_Face::mettre_a_jour(double temps)
{
  // MAJ nu_t
  nu_ou_lambda_turb_ = 0.;
  Op_Diff_PolyMAC_P0_Face::mettre_a_jour(temps);
  call_compute_nu_turb();
  mettre_a_jour_proto_face(temps);
}

void Op_Diff_Turbulent_PolyMAC_P0_Face::modifier_mu(DoubleTab& mu) const
{
  if (corr_.est_nul()) return; //rien a faire
  const DoubleTab& rho = equation().milieu().masse_volumique().passe(), *alpha =
                           sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  int i, nl = mu.dimension(0), n, N = equation().inconnue().valeurs().line_size(), cR = rho.dimension(0) == 1, d, D = dimension;
  if (mu.nb_dim() == 2) //nu scalaire
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        mu(i, n) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_ou_lambda_turb_(i, n);
  else if (mu.nb_dim() == 3) //nu anisotrope diagonal
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++)
          mu(i, n, d) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_ou_lambda_turb_(i, n);
  else if (mu.nb_dim() == 4) //nu anisotrope complet
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++)
          mu(i, n, d, d) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_ou_lambda_turb_(i, n);
  else
    abort();

  mu.echange_espace_virtuel();
}
