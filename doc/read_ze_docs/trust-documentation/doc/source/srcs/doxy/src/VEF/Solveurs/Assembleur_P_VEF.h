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

#ifndef Assembleur_P_VEF_included
#define Assembleur_P_VEF_included

#include <Matrice_Morse_Sym.h>
#include <Assembleur_base.h>
#include <TRUST_Ref.h>

class Domaine_Cl_VEF;
class Equation_base;
class Domaine_VEF;

class Assembleur_P_VEF: public Assembleur_base
{
  Declare_instanciable(Assembleur_P_VEF);

public:
  void associer_domaine_dis_base(const Domaine_dis_base&) override;
  void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base&) override;
  const Domaine_dis_base& domaine_dis_base() const override;
  const Domaine_Cl_dis_base& domaine_Cl_dis_base() const override;
  int assembler(Matrice&) override;
  int assembler_mat(Matrice&, const DoubleVect&, int, int) override;
  int remplir(Matrice&, const DoubleTab&);
  inline int remplir(Matrice&, const DoubleVect&)
  {
    Process::exit();
    return 1;
  }

  int assembler_QC(const DoubleTab&, Matrice&) override;
  int modifier_secmem(DoubleTab&) override;
  int modifier_solution(DoubleTab&) override;
  virtual int modifier_matrice(Matrice&);
  void completer(const Equation_base&) override;
  inline const Equation_base& equation() const;

protected:
  REF(Equation_base) mon_equation;
  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;
  DoubleTab les_coeff_pression;
  int has_P_ref = 0;
  void calculer_inv_volume(DoubleTab& inv_volumes_entrelaces, const Domaine_Cl_VEF& domaine_Cl_VEF, const DoubleVect& volumes_entrelaces);

};

inline const Equation_base& Assembleur_P_VEF::equation() const
{
  if (mon_equation.est_nul())
    {
      Cerr << "\nError in Assembleur_P_VEF::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation.valeur();
}

#endif
