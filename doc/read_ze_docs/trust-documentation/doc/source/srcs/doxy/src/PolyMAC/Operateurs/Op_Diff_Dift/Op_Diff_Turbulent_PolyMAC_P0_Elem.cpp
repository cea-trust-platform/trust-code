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

#include <Op_Diff_Turbulent_PolyMAC_P0_Elem.h>
#include <Op_Diff_Turbulent_PolyMAC_P0_Face.h>
#include <PolyMAC_P0_discretisation.h>
#include <Viscosite_turbulente_base.h>
#include <Transport_turbulent_base.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Pb_Multiphase.h>

Implemente_instanciable( Op_Diff_Turbulent_PolyMAC_P0_Elem, "Op_Diff_Turbulent_PolyMAC_P0_Elem|Op_Diff_Turbulente_PolyMAC_P0_Elem", Op_Diff_PolyMAC_P0_Elem );

Sortie& Op_Diff_Turbulent_PolyMAC_P0_Elem::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0_base::printOn(os); }

Entree& Op_Diff_Turbulent_PolyMAC_P0_Elem::readOn(Entree& is)
{
  //lecture de la correlation de viscosite turbulente
  corr.typer_lire(equation().probleme(), "transport_turbulent", is);
  return is;
}

void Op_Diff_Turbulent_PolyMAC_P0_Elem::creer_champ(const Motcle& motlu)
{
  Op_Diff_PolyMAC_P0_Elem::creer_champ(motlu);
}

void Op_Diff_Turbulent_PolyMAC_P0_Elem::mettre_a_jour(double temps)
{
  Op_Diff_PolyMAC_P0_Elem::mettre_a_jour(temps);
}

void Op_Diff_Turbulent_PolyMAC_P0_Elem::completer()
{
  Op_Diff_PolyMAC_P0_Elem::completer();
  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (corr.non_nul() && ref_cast(Transport_turbulent_base, corr.valeur()).gradu_required())
    equation().probleme().creer_champ("gradient_vitesse");
}

void Op_Diff_Turbulent_PolyMAC_P0_Elem::modifier_mu(DoubleTab& mu) const
{
  if (corr.est_nul()) return; //rien a faire

  const Operateur_base& op_qdm = equation().probleme().equation(0).operateur(0).l_op_base();
  if (!sub_type(Op_Diff_Turbulent_PolyMAC_P0_Face, op_qdm))
    Process::exit(que_suis_je() + ": no turbulent momentum diffusion found!");
  const Correlation& corr_visc = ref_cast(Op_Diff_Turbulent_PolyMAC_P0_Face, op_qdm).correlation();
  if (corr.est_nul() || !sub_type(Viscosite_turbulente_base, corr_visc.valeur()))
    Process::exit(que_suis_je() + ": no turbulent viscosity correlation found!");
  //un "simple" appel a la correlation!
  ref_cast(Transport_turbulent_base, corr.valeur()).modifier_mu(ref_cast(Convection_Diffusion_std, equation()), ref_cast(Viscosite_turbulente_base, corr_visc.valeur()), mu);
  mu.echange_espace_virtuel();
}
