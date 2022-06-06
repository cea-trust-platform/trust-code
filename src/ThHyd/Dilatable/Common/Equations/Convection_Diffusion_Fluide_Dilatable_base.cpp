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

#include <Convection_Diffusion_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Probleme_base.h>

Implemente_base(Convection_Diffusion_Fluide_Dilatable_base,"Convection_Diffusion_Fluide_Dilatable_base",Convection_Diffusion_std);

Sortie& Convection_Diffusion_Fluide_Dilatable_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

Entree& Convection_Diffusion_Fluide_Dilatable_base::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul() && le_fluide.non_nul());
  return Convection_Diffusion_std::readOn(is);
}

void Convection_Diffusion_Fluide_Dilatable_base::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
}

int Convection_Diffusion_Fluide_Dilatable_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);
}

void Convection_Diffusion_Fluide_Dilatable_base::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_Dilatable_base& un_fluide = ref_cast(Fluide_Dilatable_base,un_milieu);
  associer_fluide(un_fluide);
}

void Convection_Diffusion_Fluide_Dilatable_base::associer_fluide(const Fluide_Dilatable_base& un_fluide)
{
  le_fluide = un_fluide;
}

int Convection_Diffusion_Fluide_Dilatable_base::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

const Champ_Don& Convection_Diffusion_Fluide_Dilatable_base::diffusivite_pour_transport() const
{
  return is_thermal() ? milieu().conductivite() /* lambda */ : le_fluide->mu_sur_Schmidt() /* rho * D */;
}

const Fluide_Dilatable_base& Convection_Diffusion_Fluide_Dilatable_base::fluide() const
{
  if (!le_fluide.non_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}

Fluide_Dilatable_base& Convection_Diffusion_Fluide_Dilatable_base::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
}

const Milieu_base& Convection_Diffusion_Fluide_Dilatable_base::milieu() const
{
  return fluide();
}

Milieu_base& Convection_Diffusion_Fluide_Dilatable_base::milieu()
{
  return fluide();
}
