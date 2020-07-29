/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Eval_Dift_VDF_Multi_inco_var.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Dift_VDF_Multi_inco_var_included
#define Eval_Dift_VDF_Multi_inco_var_included

#include <Eval_Diff_VDF_Multi_inco_var.h>
#include <Ref_Champ_Fonc.h>

//
// .DESCRIPTION class Eval_Dift_VDF_Multi_inco_var
//
// Cette classe represente un evaluateur de flux diffusif total
// (laminaire et  turbulent) pour un vecteur d'inconnues
// avec une diffusivite par inconnue.
// Le champ de diffusivite associe a chaque inconnue
// n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_Multi_inco_var
//

class Eval_Dift_VDF_Multi_inco_var : public Eval_Diff_VDF_Multi_inco_var
{

public:

  inline Eval_Dift_VDF_Multi_inco_var();
  void associer_diff_turb(const Champ_Fonc& );
  inline const Champ_Fonc& diffusivite_turbulente() const;

protected:

  REF(Champ_Fonc) diffusivite_turbulente_;
  DoubleVect dv_diffusivite_turbulente;

};

//
//  Fonctions inline de la classe Eval_Dift_VDF_Multi_inco_var
//

inline Eval_Dift_VDF_Multi_inco_var::Eval_Dift_VDF_Multi_inco_var() {}

inline const Champ_Fonc& Eval_Dift_VDF_Multi_inco_var::diffusivite_turbulente() const
{
  return diffusivite_turbulente_.valeur();
}




#endif
