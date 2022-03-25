/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Assembleur_P_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Solveurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Assembleur_P_PolyMAC_included
#define Assembleur_P_PolyMAC_included

#include <Matrice_Morse_Sym.h>
#include <Assembleur_base.h>
#include <Zone_PolyMAC.h>
#include <Ref_Zone_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS:Assembleur_Pression_PolyMAC
//
//////////////////////////////////////////////////////////////////////////////

class Assembleur_P_PolyMAC : public Assembleur_base
{
  Declare_instanciable(Assembleur_P_PolyMAC);

public:
  void associer_zone_dis_base(const Zone_dis_base& ) override;
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& ) override;
  const Zone_dis_base& zone_dis_base() const override;
  const Zone_Cl_dis_base& zone_Cl_dis_base() const override;
  int assembler(Matrice&) override;
  int assembler_rho_variable(Matrice&, const Champ_Don_base& rho) override;
  int assembler_QC(const DoubleTab&, Matrice&) override;
  int assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u) override;
  void dimensionner_continuite(matrices_t matrices) const override;
  void assembler_continuite(matrices_t matrices, DoubleTab& secmem) const override;
  int modifier_secmem(DoubleTab&) override
  {
    return 1;
  }
  /* prise en compte des variations de pression aux CLs lors du calcul d'increments de pression.
     fac est le coefficient tel que p_final - press = fac * sol */
  void modifier_secmem_pour_incr_p(const DoubleTab& press, const double fac, DoubleTab& incr) const override;
  int modifier_solution(DoubleTab&) override;
  void completer(const Equation_base& ) override;
  inline const Equation_base& equation() const;

protected :
  REF(Equation_base) mon_equation;
  REF(Zone_PolyMAC) la_zone_PolyMAC;
  REF(Zone_Cl_PolyMAC) la_zone_Cl_PolyMAC;
  int has_P_ref;
  int stencil_done;
  IntVect tab1, tab2;//tableaux tab1 / tab2 de la Matrice_Morse (ne changent pas)
};

inline const Equation_base& Assembleur_P_PolyMAC::equation() const
{
  return  mon_equation.valeur();
}

#endif
