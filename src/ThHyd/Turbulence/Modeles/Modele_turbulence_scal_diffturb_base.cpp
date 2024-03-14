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

#include <Modele_turbulence_scal_diffturb_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Convection_Diffusion_std.h>

Implemente_base(Modele_turbulence_scal_diffturb_base, "Modele_turbulence_scal_diffturb_base", Modele_turbulence_scal_base);

Sortie& Modele_turbulence_scal_diffturb_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_scal_diffturb_base::readOn(Entree& is)
{
  return Modele_turbulence_scal_base::readOn(is);
}

/*! @brief Associe une viscosite turbulente au modele de turbulence.
 *
 * @param (Champ_Fonc& visc_turb) le champ fonction representant la viscosite turbulente a associer.
 */
void Modele_turbulence_scal_diffturb_base::associer_viscosite_turbulente(const Champ_Fonc& visc_turb)
{
  la_viscosite_turbulente = visc_turb;
}

/*! @brief Complete le modele de turbulence: Appelle Modele_turbulence_scal_base::completer()
 *
 *     associe la viscosite turbulente du probleme
 *     au modele de turbulence.
 *
 */
void Modele_turbulence_scal_diffturb_base::completer()
{
  Modele_turbulence_scal_base::completer();
  const Probleme_base& mon_pb = equation().probleme();
  const RefObjU& modele_turbulence = mon_pb.equation(0).get_modele(TURBULENCE);
  const Modele_turbulence_hyd_base& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
  const Champ_Fonc& visc_turb = mod_turb_hydr.viscosite_turbulente();
  associer_viscosite_turbulente(visc_turb);
}

/*! @brief NE FAIT RIEN
 *
 * @param (Entree&) un flot d'entree
 * @return (int) renvoie toujours 1
 */
int Modele_turbulence_scal_diffturb_base::reprendre(Entree&)
{
  return 1;
}
