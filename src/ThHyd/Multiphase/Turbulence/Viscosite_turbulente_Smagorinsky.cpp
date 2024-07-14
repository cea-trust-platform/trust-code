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

#include <Viscosite_turbulente_Smagorinsky.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_instanciable(Viscosite_turbulente_Smagorinsky, "Viscosite_turbulente_Smago", Viscosite_turbulente_LES_base);
// XD type_diffusion_turbulente_multiphase_smago type_diffusion_turbulente_multiphase_deriv smago 1 LES Smagorinsky type.

Sortie& Viscosite_turbulente_Smagorinsky::printOn(Sortie& os) const { return os; }

Entree& Viscosite_turbulente_Smagorinsky::readOn(Entree& is)
{
  mod_const_ = 0.18; // par default
  Param param(que_suis_je());
  set_param(param);
  param.ajouter("cs", &mod_const_); // XD_ADD_P floattant Smagorinsky's model constant. By default it is se to 0.18.
  param.lire_avec_accolades_depuis(is);

  if (mod_const_ < 0.) Process::exit("The smagorinsky's constant must be positive !");
  else Cerr << "LES Smagorinsky model used with constant Cs = " << mod_const_ << finl;

  pb_->creer_champ("taux_cisaillement"); // On en aura besoin pour le calcul de la viscosite turbulente

  return Viscosite_turbulente_LES_base::readOn(is);
}

void Viscosite_turbulente_Smagorinsky::eddy_viscosity(DoubleTab& nu_t) const
{
  const DoubleTab& tc = pb_->get_champ("taux_cisaillement").valeurs();
  assert(nu_t.dimension(0) == tc.dimension(0) && tc.dimension(1) <= nu_t.dimension(1));
  //on met 0 pour les composantes au-dela de k.dimension(1) (ex. : vapeur dans Pb_Multiphase)
  for (int i = 0; i < nu_t.dimension(0); i++)
    for (int n = 0; n < nu_t.dimension(1); n++)
      nu_t(i, n) = n < tc.dimension(1) ? mod_const_ * mod_const_ * l_(i) * l_(i) * tc(i, n) : 0; // XXX attention l_ resize comme nb_elem pas tot ...

  nu_t.echange_espace_virtuel();
}
