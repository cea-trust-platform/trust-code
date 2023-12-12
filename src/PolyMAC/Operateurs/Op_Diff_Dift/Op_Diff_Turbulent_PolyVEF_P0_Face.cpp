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

#include <Op_Diff_Turbulent_PolyVEF_P0_Face.h>
#include <Viscosite_turbulente_base.h>
#include <PolyVEF_P0_discretisation.h>
#include <Op_Diff_PolyVEF_P0_Face.h>
#include <Pb_Multiphase.h>
#include <Pb_Multiphase.h>
#include <vector>

Implemente_instanciable( Op_Diff_Turbulent_PolyVEF_P0_Face, "Op_Diff_Turbulent_PolyVEF_P0_Face|Op_Diff_Turbulente_PolyVEF_P0_Face", Op_Diff_PolyVEF_P0_Face );

Sortie& Op_Diff_Turbulent_PolyVEF_P0_Face::printOn(Sortie& os) const { return Op_Diff_PolyVEF_P0_base::printOn(os); }

Entree& Op_Diff_Turbulent_PolyVEF_P0_Face::readOn(Entree& is)
{
  corr_.typer_lire(equation().probleme(), "viscosite_turbulente", is); // lecture de la correlation ...
  if (pbm_.est_nul()) associer_pbm(ref_cast(Pb_Multiphase, equation().probleme()));
  ajout_champs_op_face(champs_compris_);
  return is;
}

void Op_Diff_Turbulent_PolyVEF_P0_Face::completer()
{
  Op_Diff_PolyVEF_P0_Face::completer();
  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (corr_.non_nul() && ref_cast(Viscosite_turbulente_base, corr_.valeur()).gradu_required())
    equation().probleme().creer_champ("gradient_vitesse");
  if (corr_.non_nul())
    corr_->completer();
}

void Op_Diff_Turbulent_PolyVEF_P0_Face::creer_champ(const Motcle& motlu)
{
  Op_Diff_PolyVEF_P0_Face::creer_champ(motlu);
  if (pbm_.est_nul()) associer_pbm(ref_cast(Pb_Multiphase, equation().probleme()));
  creer_champs(motlu, champs_compris_);
}

void Op_Diff_Turbulent_PolyVEF_P0_Face::preparer_calcul()
{
  Op_Diff_PolyVEF_P0_Face::preparer_calcul();
  prepare_calcul_op_proto (true /* Eq_QDM */);
}

void Op_Diff_Turbulent_PolyVEF_P0_Face::mettre_a_jour(double temps)
{
  Op_Diff_PolyVEF_P0_Face::mettre_a_jour(temps);
  mettre_a_jour_champs_op_face(temps);
}

void Op_Diff_Turbulent_PolyVEF_P0_Face::modifier_mu(DoubleTab& mu) const
{
  if (corr_.est_nul()) return; //rien a faire
  const DoubleTab& rho = equation().milieu().masse_volumique().passe(), *alpha =
                           sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : NULL;
  int i, nl = mu.dimension(0), D = dimension, n, N = equation().inconnue().valeurs().line_size()/D, cR = rho.dimension(0) == 1, d;
  DoubleTrav nu_t(nl, N); //viscosite turbulente : toujours scalaire
  ref_cast(Viscosite_turbulente_base, corr_.valeur()).eddy_viscosity(nu_t); //remplissage par la correlation
  if (mu.nb_dim() == 2) //nu scalaire
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        mu(i, n) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_t(i, n);
  else if (mu.nb_dim() == 3) //nu anisotrope diagonal
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++)
          mu(i, n, d) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_t(i, n);
  else if (mu.nb_dim() == 4) //nu anisotrope complet
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++)
          mu(i, n, d, d) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_t(i, n);
  else
    abort();

  mu.echange_espace_virtuel();
}
