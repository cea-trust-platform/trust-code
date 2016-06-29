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
// File:        Op_Diff_K_Eps_VEF_base.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////


// .DESCRIPTION class Op_Diff_K_Eps_VEF_Face
//   Cette classe represente l'operateur de diffusion turbulente
//   La discretisation est VEF
//  Les methodes pour l'implicite sont codees.

#ifndef Op_Diff_K_Eps_VEF_base_included
#define Op_Diff_K_Eps_VEF_base_included

#define PRDT_K_DEFAUT 1
#define PRDT_EPS_DEFAUT 1.3

#include <Op_Diff_K_Eps_base.h>
#include <Ref_Champ_Fonc.h>
#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Ref_Champ_P1NC.h>
#include <Op_VEF_Face.h>
#include <Ref_Champ_Don_base.h>

class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_K_Eps_VEF_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps_VEF_base : public Op_Diff_K_Eps_base
{

  Declare_base_sans_constructeur(Op_Diff_K_Eps_VEF_base);

public:

  inline Op_Diff_K_Eps_VEF_base(double Prandt_K = PRDT_K_DEFAUT ,
                                double Prandt_Eps =PRDT_EPS_DEFAUT );
  void completer();
  void associer_diffusivite(const Champ_base& ch_diff);
  virtual void associer_diffusivite_turbulente() =0;
  inline void associer_diffusivite_turbulente(const Champ_Fonc& );
  inline void associer_Pr_K_Eps(double, double);
  inline const Champ_Fonc& diffusivite_turbulente() const;
  inline const Champ_base& diffusivite() const
  {
    return diffusivite_.valeur();
  } ;

protected:
  double Prdt_K;
  double Prdt_Eps;
  REF(Champ_Fonc) diffusivite_turbulente_;
  REF(Champ_Don_base) tmp;
  REF(Champ_base) diffusivite_;
};


//
// Fonctions inline de la classe Op_Diff_K_Eps_VEF_base
//

inline Op_Diff_K_Eps_VEF_base::Op_Diff_K_Eps_VEF_base(double Prandt_K ,
                                                      double Prandt_Eps )
  : Prdt_K(Prandt_K) , Prdt_Eps(Prandt_Eps) {}

inline const Champ_Fonc& Op_Diff_K_Eps_VEF_base::diffusivite_turbulente() const
{
  return diffusivite_turbulente_.valeur();
}

inline void Op_Diff_K_Eps_VEF_base::associer_diffusivite_turbulente(const Champ_Fonc& ch)
{
  diffusivite_turbulente_=ch;
}

inline void Op_Diff_K_Eps_VEF_base::associer_Pr_K_Eps(double Pr_K, double Pr_Eps)
{
  Prdt_K = Pr_K;
  Prdt_Eps = Pr_Eps;
}

#endif
