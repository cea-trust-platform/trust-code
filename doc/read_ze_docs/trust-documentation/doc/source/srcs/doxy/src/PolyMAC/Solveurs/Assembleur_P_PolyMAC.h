/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Assembleur_P_PolyMAC_included
#define Assembleur_P_PolyMAC_included

#include <Matrice_Morse_Sym.h>
#include <Assembleur_base.h>
#include <Domaine_PolyMAC.h>
#include <TRUST_Ref.h>

class Domaine_Cl_PolyMAC;

class Assembleur_P_PolyMAC: public Assembleur_base
{
  Declare_instanciable(Assembleur_P_PolyMAC);
public:
  void associer_domaine_dis_base(const Domaine_dis_base&) override;
  void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base&) override;
  const Domaine_dis_base& domaine_dis_base() const override;
  const Domaine_Cl_dis_base& domaine_Cl_dis_base() const override;

  int assembler(Matrice&) override;
  int assembler_rho_variable(Matrice&, const Champ_Don_base& rho) override;
  int assembler_QC(const DoubleTab&, Matrice&) override;
  int assembler_mat(Matrice&, const DoubleVect&, int incr_pression, int resoudre_en_u) override;

  int modifier_secmem(DoubleTab&) override;
  int modifier_solution(DoubleTab&) override;

  void completer(const Equation_base&) override;
  inline const Equation_base& equation() const { return mon_equation.valeur(); }

  /* corrige les vitesses pour une correction en pression donnee de type (-Cp, Cv) */
  void corriger_vitesses(const DoubleTab& dP, DoubleTab& dv) const override
  {
    rec.ajouter_multvect(dP, dv);
    dv.echange_espace_virtuel();
  }

protected:
  REF(Equation_base) mon_equation;
  REF(Domaine_PolyMAC) le_dom_PolyMAC;
  REF(Domaine_Cl_PolyMAC) le_dom_Cl_PolyMAC;

  DoubleTab les_coeff_pression;

  int has_P_ref = 0, stencil_done = 0;
  IntVect tab1, tab2; //tableaux tab1 / tab2 de la Matrice_Morse (ne changent pas)
  Matrice_Morse rec; //pour reconstruire les vitesses
};

#endif /* Assembleur_P_PolyMAC_included */
