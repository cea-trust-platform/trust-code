/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Assembleur_P_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Assembleur_P_CoviMAC_included
#define Assembleur_P_CoviMAC_included

#include <Matrice_Morse_Sym.h>
#include <Assembleur_base.h>
#include <Zone_CoviMAC.h>
#include <Ref_Zone_CoviMAC.h>
#include <Ref_Zone_Cl_CoviMAC.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS:Assembleur_Pression_CoviMAC
//
//////////////////////////////////////////////////////////////////////////////

class Assembleur_P_CoviMAC : public Assembleur_base
{
  Declare_instanciable(Assembleur_P_CoviMAC);

public:
  void associer_zone_dis_base(const Zone_dis_base& )         ;
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& )   ;
  const Zone_dis_base& zone_dis_base() const                 ;
  const Zone_Cl_dis_base& zone_Cl_dis_base() const           ;
  int assembler(Matrice&)                                 ;
  int assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u);
  int modifier_secmem(DoubleTab&)
  {
    return 1;
  }
  int modifier_solution(DoubleTab&)                       ;
  void completer(const Equation_base& )                      ;
  inline const Equation_base& equation() const                ;

protected :
  REF(Equation_base) mon_equation                            ;
  REF(Zone_CoviMAC) la_zone_CoviMAC                                  ;
  REF(Zone_Cl_CoviMAC) la_zone_Cl_CoviMAC                            ;
  int has_P_ref;
  int stencil_done;
  IntVect tab1, tab2;//tableaux tab1 / tab2 de la Matrice_Morse (ne changent pas)
};

inline const Equation_base& Assembleur_P_CoviMAC::equation() const
{
  return  mon_equation.valeur();
}

#endif
