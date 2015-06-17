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
// File:        Eval_Diff_K_Eps_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_K_Eps_VDF_included
#define Eval_Diff_K_Eps_VDF_included

#define PRDT_K_DEFAUT 1
#define PRDT_EPS_DEFAUT 1.3

#include <Evaluateur_VDF.h>
#include <Ref_Champ_Fonc.h>
#include <Ref_Champ_Uniforme.h>

#include <Champ_Don.h>
#include <Ref_Champ_Don_base.h>
#include <Ref_Champ_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Eval_Diff_K_Eps_VDF
//
//////////////////////////////////////////////////////////////////////////////

class Eval_Diff_K_Eps_VDF : public Evaluateur_VDF
{

public:

  inline Eval_Diff_K_Eps_VDF(double Prandt_K = PRDT_K_DEFAUT ,
                             double Prandt_Eps =PRDT_EPS_DEFAUT );
  void associer_diff_turb(const Champ_Fonc& );
  void associer_mvolumique(const Champ_Don& );
  void associer(const Champ_base& );
  void associer_Pr_K_Eps(double,double);
  inline const Champ_Fonc& diffusivite_turbulente() const;
  inline const Champ_base& diffusivite() const;
  void mettre_a_jour( );

protected:

  double Prdt_K;
  double Prdt_Eps;
  double Prdt[2];
  REF(Champ_Fonc) diffusivite_turbulente_;
  DoubleVect dv_diffusivite_turbulente;
  REF(Champ_Don_base) masse_volumique_;
  DoubleVect dv_mvol;
  REF(Champ_base) diffusivite_;
  double db_diffusivite;

};

//
//  Fonctions inline de la classe Eval_Diff_K_Eps_VDF
//



inline Eval_Diff_K_Eps_VDF::Eval_Diff_K_Eps_VDF(double Prandt_K ,double Prandt_Eps)
  : Prdt_K(Prandt_K) , Prdt_Eps(Prandt_Eps),db_diffusivite(-123.)
{
  Prdt[0]=Prandt_K;
  Prdt[1]=Prandt_Eps;
}

//inline Eval_Diff_K_Eps_VDF::Eval_Diff_K_Eps_VDF(double Prandt_K ,double Prandt_Eps)
//: Prdt_K(Prandt_K) , Prdt_Eps(Prandt_Eps) { Prdt[0]=Prdt_K;Prdt[1]=Prdt_Eps;}

inline const Champ_Fonc& Eval_Diff_K_Eps_VDF::diffusivite_turbulente() const
{
  return diffusivite_turbulente_.valeur();
}

inline const Champ_base& Eval_Diff_K_Eps_VDF::diffusivite() const
{
  return diffusivite_.valeur();
}
#endif
