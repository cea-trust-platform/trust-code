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
// File:        Eval_Echange_Himp_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Eval_Echange_Himp_VDF_Elem.h>
#include <Champ_Inc.h>
#include <Champ_Uniforme.h>

void Eval_Echange_Himp_VDF_Elem::associer_champs(const Champ_Don& ch_rho,
                                                 const Champ_Don& Cp,const Champ_Inc& tc, const Champ_Inc& tv,
                                                 const double dh)
{
  cerr << rho_cst << " " << Cp_cst << endl;
  this->rho=ch_rho;
  rho_cst=sub_type(Champ_Uniforme,ch_rho.valeur())?true:false;
  rho_=ch_rho(0,0);
  this->champ_Cp_=Cp;
  Cp_cst=sub_type(Champ_Uniforme,Cp.valeur())?true:false;
  Cp_=Cp(0,0);
  cerr << rho_cst << " " << Cp_cst << endl;
  this->h_ = dh;
  T = tc;
  T_voisin = tv;
  Tcourant.ref(T->valeurs());
  mettre_a_jour();
}

void Eval_Echange_Himp_VDF_Elem::mettre_a_jour( )
{
  Tcourant.ref(T->valeurs());
  Tvois.ref(T_voisin->valeurs());
}
