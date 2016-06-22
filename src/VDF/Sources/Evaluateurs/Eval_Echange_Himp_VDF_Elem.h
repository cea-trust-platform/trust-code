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
// File:        Eval_Echange_Himp_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Echange_Himp_VDF_Elem_included
#define Eval_Echange_Himp_VDF_Elem_included

#include <DoubleTab.h>
#include <Evaluateur_Source_VDF_Elem.h>
#include <Ref_Champ_Don.h>
#include <Champ_Don.h>
#include <Ref_Champ_Inc.h>

class Eval_Echange_Himp_VDF_Elem: public Evaluateur_Source_VDF_Elem
{

public:
  Eval_Echange_Himp_VDF_Elem()
  {
    rho_cst=true;
    rho_=DMAXFLOAT;
    Cp_cst=true;
    Cp_=DMAXFLOAT;
  }
  void associer_champs(const Champ_Don& ,const Champ_Don& , const Champ_Inc& ,const Champ_Inc& ,const double );
  void mettre_a_jour( );
  inline double calculer_terme_source(int ) const;
  inline void calculer_terme_source(int , DoubleVect& ) const;

protected:
  REF(Champ_Don) rho;
  bool rho_cst;
  double rho_;
  REF(Champ_Don) champ_Cp_;
  bool Cp_cst;
  double Cp_;
  REF(Champ_Inc) T;
  REF(Champ_Inc) T_voisin;
  DoubleTab Tcourant;
  DoubleTab Tvois;
  double h_;
};

inline double Eval_Echange_Himp_VDF_Elem::calculer_terme_source(int num_elem) const
{
  double rhocp=(rho_cst?rho_:rho->valeurs()(num_elem,0))*(Cp_cst?Cp_:champ_Cp_->valeurs()(num_elem,0));
  return h_*(Tvois(num_elem)-Tcourant(num_elem))/rhocp * volumes(num_elem)*porosite_vol(num_elem);
}

inline void Eval_Echange_Himp_VDF_Elem::calculer_terme_source(int , DoubleVect& ) const { }

#endif

