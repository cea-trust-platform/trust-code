/****************************************************************************
* Copyright (c) 2019, CEA
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
#include <Mod_turb_hyd_RANS.h>
#include <Modele_turbulence_hyd_K_Eps_Realisable.h>
#include <Turbulence_hyd_sous_maille_VDF.h>
#include <Paroi_negligeable_VDF.h>
#include <Transport_K_Eps_base.h>

void Eval_Dift_VDF_const_Face::associer_modele_turbulence(const Mod_turb_hyd_base& mod)
{
  // On remplit la reference au modele de turbulence et le tableau k:
  le_modele_turbulence    = mod;
  indic_bas_Re            = 0;
  indic_lp_neg            = 0;
  indice_keps_realisable_ = 0;

  if (sub_type(Mod_turb_hyd_RANS,le_modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_RANS& mod_K_eps = ref_cast(Mod_turb_hyd_RANS,le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.eqn_transp_K_Eps().inconnue().valeurs());

      if (sub_type(Modele_turbulence_hyd_K_Eps_Realisable,le_modele_turbulence.valeur()))
        {
          indice_keps_realisable_ = 1;
        }
    }
  else if (sub_type(Mod_turb_hyd_ss_maille,le_modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_ss_maille& mod_ss_maille = ref_cast(Mod_turb_hyd_ss_maille, le_modele_turbulence.valeur());
      k_.ref(mod_ss_maille.energie_cinetique_turbulente().valeurs());
    }
  else
    {

      Cerr<<"not implemented"<<finl;
      //Process::exit();
    }
  // On remplit la reference aux lois de paroi et le tableau tau_tan:
  if (le_modele_turbulence->loi_paroi().non_nul())
    {
      loipar = ref_cast(Turbulence_paroi_base,le_modele_turbulence->loi_paroi().valeur());
      if (loipar->use_shear()==false)
        indic_lp_neg = 1;
    }
  else
    {
      if ( indice_keps_realisable_ == 0 )
        {
          indic_bas_Re = 1;
        }
    }
}

//// mettre_a_jour
//

void Eval_Dift_VDF_const_Face::mettre_a_jour( )
{
  Eval_Dift_VDF_const::mettre_a_jour( ) ;
  if (sub_type(Mod_turb_hyd_RANS,le_modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_RANS& mod_K_eps = ref_cast(Mod_turb_hyd_RANS,le_modele_turbulence.valeur());
      k_.ref(mod_K_eps.eqn_transp_K_Eps().inconnue().valeurs());
    }
  if (le_modele_turbulence->loi_paroi().non_nul())
    {
      tau_tan_.ref(loipar->Cisaillement_paroi());
    }
}
