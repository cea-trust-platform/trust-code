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
// File:        Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi.cpp
// Directory:   $TRUST_ROOT/src/VDF/Axi/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi.h>
#include <Modele_turbulence_hyd_K_Eps_2_Couches.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Champ_P0_VDF.h>

Implemente_instanciable_sans_constructeur(Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi,"Op_Diff_K_Eps_Bas_Re_VDF_const_P0_VDF_Axi",Op_Diff_K_Eps_Bas_Re_VDF_base);

implemente_It_VDF_Elem(Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi)

Sortie& Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::readOn(Entree& s )
{
  return s ;
}



// Description:
// complete l'iterateur et l'evaluateur
void Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::associer(const Zone_dis& zone_dis,
                                                 const Zone_Cl_dis& zone_cl_dis,
                                                 const Champ_Inc& ch_diffuse)
{
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);
  Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff = (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
  eval_diff.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  eval_diff.associer_inconnue(inco );        // Eval_VDF_Elem::associer_inconnue
}

// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff_turb = (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
  eval_diff_turb.associer(ch_diff);
}

// Description:
// associe le champ de diffusivite_turbulente a l'evaluateur
void Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::associer_diffusivite_turbulente()
{
  assert(mon_equation.non_nul());
  if(sub_type(Transport_K_KEps,mon_equation.valeur()))
    {
      // Cerr << "dans Transport_K_KEps Op_Diff_K_Eps_VDF_Elem " << finl;
      //Cerr << "equation.que_suis_je() Op_Diff_K_Eps_VDF_Elem" << mon_equation->que_suis_je() << finl;
      const Transport_K_KEps& eqn_transport = ref_cast(Transport_K_KEps,mon_equation.valeur());
      const Modele_turbulence_hyd_K_Eps_2_Couches& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_2_Couches,eqn_transport.modele_turbulence());
      const Champ_Fonc& diff_turb = mod_turb.viscosite_turbulente();
      Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff = (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
      eval_diff.associer_diff_turb(diff_turb);
    }
  else if(sub_type(Transport_K_Eps_Bas_Reynolds,mon_equation.valeur()))
    {
      //   Cerr << "dans Transport_K_Eps_Bas_Reynolds Op_Diff_K_Eps_VDF_Elem" << finl;
      //   Cerr << "equation.que_suis_je() Op_Diff_K_Eps_VDF_Elem " << mon_equation->que_suis_je() << finl;
      const Transport_K_Eps_Bas_Reynolds& eqn_transport = ref_cast(Transport_K_Eps_Bas_Reynolds,mon_equation.valeur());
      const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,eqn_transport.modele_turbulence());
      const Champ_Fonc& diff_turb = mod_turb.viscosite_turbulente();
      Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff = (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
      eval_diff.associer_diff_turb(diff_turb);
    }
  else if(sub_type(Transport_K_Eps,mon_equation.valeur()))
    {
      //   Cerr << "dans Transport_K_Eps_Bas Op_Diff_K_Eps_VDF_Elem" << finl;
      //   Cerr << "equation.que_suis_je() Op_Diff_K_Eps_VDF_Elem " << mon_equation->que_suis_je() << finl;
      const Transport_K_Eps& eqn_transport = ref_cast(Transport_K_Eps,mon_equation.valeur());
      const Modele_turbulence_hyd_K_Eps& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps,eqn_transport.modele_turbulence());
      const Champ_Fonc& diff_turb = mod_turb.viscosite_turbulente();
      Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff = (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
      eval_diff.associer_diff_turb(diff_turb);
    }
}

const Champ_Fonc& Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::diffusivite_turbulente() const
{
  const Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff =
    (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
  return eval_diff.diffusivite_turbulente();
}

const Champ_base& Op_Diff_K_Eps_Bas_Re_VDF_Elem_Axi::diffusivite() const
{
  const Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi& eval_diff_turb =
    (Eval_Diff_K_Eps_Bas_Re_VDF_const_Elem_Axi&) iter.evaluateur();
  return eval_diff_turb.diffusivite();
}

