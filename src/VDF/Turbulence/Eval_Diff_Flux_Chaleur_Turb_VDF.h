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
// File:        Eval_Diff_Flux_Chaleur_Turb_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_Flux_Chaleur_Turb_VDF_included
#define Eval_Diff_Flux_Chaleur_Turb_VDF_included

#define PRDT_UTETA_DEFAUT 1

#include <Evaluateur_VDF.h>
#include <Champ_Face.h>
#include <Ref_Champ_Uniforme.h>
#include <Ref_Champ_Fonc.h>

class Champ_Don;

///////////////////////////////////////////
// class Eval_Diff_Flux_Chaleur_Turb_VDF
//////////////////////////////////////////

class Eval_Diff_Flux_Chaleur_Turb_VDF : public Evaluateur_VDF
{

public:

  inline Eval_Diff_Flux_Chaleur_Turb_VDF(double Prandt_uteta = PRDT_UTETA_DEFAUT);
  void associer_diff_turb(const Champ_Fonc& );
  inline const Champ_Fonc& diffusivite_turbulente() const;
  void mettre_a_jour( );

protected:

  double Prdt_UTETA;
  REF(Champ_Fonc) diffusivite_turbulente_;
  DoubleVect dv_diffusivite_turbulente;
  double dist_face(int, int, int) const;
  double dist_face_period(int, int, int) const;

};

//
//  Fonctions inline de la classe Eval_Diff_Flux_Chaleur_Turb_VDF
//

inline Eval_Diff_Flux_Chaleur_Turb_VDF::Eval_Diff_Flux_Chaleur_Turb_VDF(double Prandt_uteta)

  : Prdt_UTETA(Prandt_uteta) {}

inline const Champ_Fonc& Eval_Diff_Flux_Chaleur_Turb_VDF::diffusivite_turbulente() const
{
  return diffusivite_turbulente_.valeur();
}


#endif
