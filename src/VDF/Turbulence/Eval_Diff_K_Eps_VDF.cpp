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
// File:        Eval_Diff_K_Eps_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Eval_Diff_K_Eps_VDF.h>
#include <Champ_Fonc.h>
#include <Champ_Uniforme.h>

void Eval_Diff_K_Eps_VDF::associer_diff_turb(const Champ_Fonc& diffu)
{
  diffusivite_turbulente_ = diffu;
}

void Eval_Diff_K_Eps_VDF::associer_Pr_K_Eps(double Pr_K, double Pr_Eps)
{
  Prdt_K = Pr_K;
  Prdt_Eps = Pr_Eps;
  Prdt[0] = Pr_K;
  Prdt[1] = Pr_Eps;
}

void Eval_Diff_K_Eps_VDF::mettre_a_jour( )
{
  //  Cerr << " AVANT dv_diffusivite_turbulente.ref(diffusivite_turbulente_->valeurs()); " << finl;
  //Debog::verifier("Eval_Diff_K_Eps_VDF::mettre_a_jour : dv_diffusivite_turbulente",diffusivite_turbulente_->valeurs());
  dv_diffusivite_turbulente.ref(diffusivite_turbulente_->valeurs());
  //  Cerr << " APRES dv_diffusivite_turbulente.ref(diffusivite_turbulente_->valeurs()); " << finl;
  if (sub_type(Champ_Uniforme, diffusivite_.valeur()))
    db_diffusivite = diffusivite_.valeur()(0,0);
}

void Eval_Diff_K_Eps_VDF::associer_mvolumique(const Champ_Don& mvol)
{
  masse_volumique_ = mvol.valeur();
  dv_mvol.ref(mvol.valeurs());
}

// Description:
// associe le champ de diffusivite
void Eval_Diff_K_Eps_VDF::associer(const Champ_base& diffu)
{
  diffusivite_ =  diffu;
  if (sub_type(Champ_Uniforme, diffu))
    {
      db_diffusivite = diffu(0,0);
    }
}
