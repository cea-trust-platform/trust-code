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

#include <Convection_Diffusion_Espece_Multi_base.h>
#include <Fluide_Dilatable_base.h>
#include <Probleme_base.h>
#include <Param.h>

Implemente_base(Convection_Diffusion_Espece_Multi_base,"Convection_Diffusion_Espece_Multi_base",Convection_Diffusion_Espece_Fluide_Dilatable_base);

Sortie& Convection_Diffusion_Espece_Multi_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Multi_base::readOn(Entree& is)
{
  alias_=inconnue().le_nom();
  Convection_Diffusion_Espece_Fluide_Dilatable_base::readOn(is);
  Nom conv = "Convection_", diff = "Diffusion_";
  conv += alias_;
  diff += alias_;
  terme_convectif.set_fichier(conv);
  terme_convectif.set_description((Nom)"Convective flux =Integral(-rho*Y*u*ndS) [kg/s] if SI units used");
  terme_diffusif.set_fichier(diff);
  terme_diffusif.set_description((Nom)"Diffusive flux=Integral(rho*D*grad(Y)*ndS) [kg/s] if SI units used");
  l_inco_ch->add_synonymous(alias_);
  champs_compris_.ajoute_champ(l_inco_ch);
  return is;
}

void Convection_Diffusion_Espece_Multi_base::set_param(Param& param)
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::set_param(param);
  param.ajouter("alias",&alias_);
}

// FIXME : TODO : factorize
int Convection_Diffusion_Espece_Multi_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      //associe mu_sur_Sc dans la diffusivite
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      ref_cast_non_const(Champ_base,terme_diffusif.diffusivite()).nommer("mu_sur_Schmidt");
      is >> terme_diffusif;
      // Il faut appeler associer_diffusivite_pour_pas_de_temps
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else
    return Convection_Diffusion_Espece_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
}

bool Convection_Diffusion_Espece_Multi_base::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  if (Convection_Diffusion_Espece_Fluide_Dilatable_base::has_champ(nom, ref_champ))
    return true;

  // a revoir ..... a mon avis
  if (probleme().equation(0).has_champ(nom, ref_champ))
    return true;

  return false; /* rien trouve */
}

bool Convection_Diffusion_Espece_Multi_base::has_champ(const Motcle& nom) const
{
  if (Convection_Diffusion_Espece_Fluide_Dilatable_base::has_champ(nom))
    return true;

  // a revoir ..... a mon avis
  if (probleme().equation(0).has_champ(nom))
    return true;

  return false; /* rien trouve */
}

const Champ_base& Convection_Diffusion_Espece_Multi_base::get_champ(const Motcle& nom) const
{
  OBS_PTR(Champ_base) ref_champ;

  if (Convection_Diffusion_Espece_Fluide_Dilatable_base::has_champ(nom, ref_champ))
    return ref_champ;

  // a revoir ..... a mon avis
  if (probleme().equation(0).has_champ(nom, ref_champ))
    return ref_champ;

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}
