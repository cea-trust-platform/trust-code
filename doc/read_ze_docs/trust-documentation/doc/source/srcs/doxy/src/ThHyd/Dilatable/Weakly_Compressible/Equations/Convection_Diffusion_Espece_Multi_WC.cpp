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

#include <Convection_Diffusion_Espece_Multi_WC.h>
#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Fluide_Weakly_Compressible.h>
#include <Loi_Etat_Multi_GP_WC.h>
#include <Op_Conv_negligeable.h>
#include <Probleme_base.h>
#include <TRUSTTrav.h>
#include <Param.h>
#include <Discretisation_base.h>
#include <Statistiques.h>

extern Stat_Counter_Id assemblage_sys_counter_;

Implemente_instanciable(Convection_Diffusion_Espece_Multi_WC,"Convection_Diffusion_Espece_Multi_WC",Convection_Diffusion_Espece_Multi_base);
// XD convection_diffusion_espece_multi_WC eqn_base convection_diffusion_espece_multi_WC -1 Species conservation equation for a multi-species weakly-compressible fluid.

Sortie& Convection_Diffusion_Espece_Multi_WC::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Multi_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Multi_WC::readOn(Entree& is)
{
  return Convection_Diffusion_Espece_Multi_base::readOn(is);
}

const Champ_base& Convection_Diffusion_Espece_Multi_WC::diffusivite_pour_pas_de_temps() const
{
  return le_fluide->nu_sur_Schmidt();
}

const Champ_base& Convection_Diffusion_Espece_Multi_WC::vitesse_pour_transport() const
{
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,probleme().equation(0));
  return eqn_hydr.rho_la_vitesse() /* rho * u and not u */;
}

/*! @brief Associe l inconnue de l equation a la loi d etat,
 *
 */
void Convection_Diffusion_Espece_Multi_WC::completer()
{
  Convection_Diffusion_Espece_Multi_base::completer();
  Fluide_Weakly_Compressible& le_fluideQC=ref_cast(Fluide_Weakly_Compressible,fluide());
  Loi_Etat_Multi_GP_WC& loi_etat = ref_cast_non_const(Loi_Etat_Multi_GP_WC,le_fluideQC.loi_etat().valeur());
  loi_etat.associer_inconnue(l_inco_ch.valeur());
}

DoubleTab& Convection_Diffusion_Espece_Multi_WC::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee=0.;
  return derivee_en_temps_inco_sans_solveur_masse_impl(*this,derivee,true /* explicit */);
}

void Convection_Diffusion_Espece_Multi_WC::assembler( Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  Convection_Diffusion_Fluide_Dilatable_Proto::assembler_impl(*this,matrice,inco,resu);
}

void Convection_Diffusion_Espece_Multi_WC::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  statistiques().begin_count(assemblage_sys_counter_);
  Convection_Diffusion_Fluide_Dilatable_Proto::assembler_blocs(*this,matrices, secmem, semi_impl);
  schema_temps().ajouter_blocs(matrices, secmem, *this);

  if (!discretisation().is_polymac_family())
    {
      const std::string& nom_inco = inconnue().le_nom().getString();
      Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
      modifier_pour_Cl(*mat,secmem);
    }
  statistiques().end_count(assemblage_sys_counter_);

}
