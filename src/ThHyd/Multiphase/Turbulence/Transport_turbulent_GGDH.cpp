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

#include <Transport_turbulent_GGDH.h>
#include <Param.h>
#include <Probleme_base.h>

#include <Pb_Multiphase.h>
#include <TRUSTTrav.h>
#include <MD_Vector_tools.h>

Implemente_instanciable(Transport_turbulent_GGDH, "Transport_turbulent_GGDH|Transport_turbulent_anisotrope", Transport_turbulent_base);

Sortie& Transport_turbulent_GGDH::printOn(Sortie& os) const
{
  return os;
}

Entree& Transport_turbulent_GGDH::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("C_s", &C_s);
  param.lire_avec_accolades_depuis(is);

  if ((C_s <0) && (dimension == 2)) C_s = 1;
  if ((C_s <0) && (dimension == 3)) C_s = 1.5;

  return is;
}

void Transport_turbulent_GGDH::modifier_mu(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb, DoubleTab& nu) const
{
  const DoubleTab& mu0 = eq.diffusivite_pour_transport().passe(), &nu0 = eq.diffusivite_pour_pas_de_temps().passe(), //viscosites moleculaires
                   *alp = sub_type(Pb_Multiphase, pb_.valeur()) ? &pb_->get_champ("alpha").passe() : nullptr; //produit par alpha si Pb_Multiphase
  int i, nl = nu.dimension(0), n, N = nu.dimension(1), d, db, D = dimension;
  //par la viscosite turbulente : tenseur de Reynolds, facteur k / eps
  DoubleTrav Rij(0, N, D, D), k_sur_eps(0, N);
  MD_Vector_tools::creer_tableau_distribue(nu.get_md_vector(), Rij);
  MD_Vector_tools::creer_tableau_distribue(nu.get_md_vector(), k_sur_eps);
  visc_turb.reynolds_stress(Rij), visc_turb.k_over_eps(k_sur_eps);
  //formule pour passer de nu a mu : mu0 / nu0 * C_s * k / eps * <u'i u'_j>
  for (i = 0; i < nl; i++)
    for (n = 0; n < N; n++)
      for (d = 0; d < D; d++)
        for (db = 0; db < D; db++)
          nu(i, n, d, db) += (alp ? (*alp)(i, n) : 1) * mu0(i, n) / nu0(i, n) * C_s * std::max(k_sur_eps(i, n) * Rij(i, n, d, db), visc_turb.limiteur() * nu(i, n, d, db));
}
