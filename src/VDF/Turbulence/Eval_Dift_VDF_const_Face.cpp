/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_Dift_VDF_const_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Eval_Dift_VDF_const_Face.h>
#include <Modele_turbulence_hyd_K_Eps_2_Couches.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Modele_turbulence_hyd_K_Eps_V2_VDF.h>
#include <Turbulence_hyd_sous_maille_VDF.h>
#include <Paroi_negligeable_VDF.h>


void Eval_Dift_VDF_const_Face::associer_modele_turbulence(const Mod_turb_hyd_base& mod)
{
  // On remplit la reference au modele de turbulence et le tableau k:
  le_modele_turbulence = mod;
  indic_bas_Re=0;
  indic_lp_neg=0;
  if (sub_type(Modele_turbulence_hyd_K_Eps,le_modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_K_Eps& mod_K_eps = ref_cast(Modele_turbulence_hyd_K_Eps,le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());
    }
  else if (sub_type(Modele_turbulence_hyd_K_Eps_2_Couches,le_modele_turbulence.valeur()))
    {
      indic_bas_Re = 1;
      const Modele_turbulence_hyd_K_Eps_2_Couches& mod_K_eps = ref_cast(Modele_turbulence_hyd_K_Eps_2_Couches, le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());
    }
  else if (sub_type(Modele_turbulence_hyd_K_Eps_V2_VDF,le_modele_turbulence.valeur()))
    {
      indic_bas_Re = 1;
      const Modele_turbulence_hyd_K_Eps_V2_VDF& mod_K_eps = ref_cast(Modele_turbulence_hyd_K_Eps_V2_VDF, le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());
    }
  else if (sub_type(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,le_modele_turbulence.valeur()))
    {
      indic_bas_Re = 1;
      const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_K_eps = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds, le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());
    }
  else if (sub_type(Mod_turb_hyd_ss_maille,le_modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_ss_maille& mod_ss_maille = ref_cast(Mod_turb_hyd_ss_maille, le_modele_turbulence.valeur());
      k_.ref(mod_ss_maille.energie_cinetique_turbulente().valeurs());
    }
  // On remplit la reference aux lois de paroi et le tableau tau_tan:
  if  (!(sub_type(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,le_modele_turbulence.valeur())) && !(sub_type(Modele_turbulence_hyd_K_Eps_2_Couches,le_modele_turbulence.valeur()))  && !(sub_type(Modele_turbulence_hyd_K_Eps_V2_VDF,le_modele_turbulence.valeur())))
    {
      loipar = ref_cast(Turbulence_paroi_base,le_modele_turbulence->loi_paroi().valeur());
      if (sub_type(Paroi_negligeable_VDF,le_modele_turbulence->loi_paroi().valeur()))
        indic_lp_neg = 1;

    }
}

//// mettre_a_jour
//

void Eval_Dift_VDF_const_Face::mettre_a_jour( )
{
  Eval_Dift_VDF_const::mettre_a_jour( ) ;
  if (sub_type(Modele_turbulence_hyd_K_Eps,le_modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_K_Eps& mod_K_eps =
        ref_cast( Modele_turbulence_hyd_K_Eps,le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());

    }
  else if (sub_type(Modele_turbulence_hyd_K_Eps_2_Couches,le_modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_K_Eps_2_Couches& mod_K_eps =
        ref_cast( Modele_turbulence_hyd_K_Eps_2_Couches,le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());

    }
  else if (sub_type(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,le_modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_K_eps =
        ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds, le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());

    }
  else if (sub_type(Modele_turbulence_hyd_K_Eps_V2_VDF,le_modele_turbulence.valeur()))
    {
      indic_bas_Re = 1;
      const Modele_turbulence_hyd_K_Eps_V2_VDF& mod_K_eps =
        ref_cast( Modele_turbulence_hyd_K_Eps_V2_VDF,le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.K_Eps().valeurs());
    }
  if (!(sub_type(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,le_modele_turbulence.valeur())) && !(sub_type(Modele_turbulence_hyd_K_Eps_2_Couches,le_modele_turbulence.valeur())) && !(sub_type(Modele_turbulence_hyd_K_Eps_V2_VDF,le_modele_turbulence.valeur())) )
    {
      tau_tan_.ref(loipar->Cisaillement_paroi());
    }
}
