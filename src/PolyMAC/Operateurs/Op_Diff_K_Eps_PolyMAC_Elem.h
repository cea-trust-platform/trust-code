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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Op_Diff_K_Eps_PolyMAC_Elem.h
// Directory : $POLYMAC_ROOT/src/Operateurs
//
/////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_K_Eps_PolyMAC_Elem_included
#define Op_Diff_K_Eps_PolyMAC_Elem_included

#define PRDT_K_DEFAUT 1
#define PRDT_EPS_DEFAUT 1.3

#include <Op_Diff_PolyMAC_Elem.h>
#include <Op_Diff_K_Eps_base.h>
#include <Champ_Fonc.h>


/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Op_Diff_K_Eps_PolyMAC_Elem
//
// <Description of class Op_Diff_K_Eps_PolyMAC_Elem>
//
/////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps_PolyMAC_Elem : public Op_Diff_K_Eps_base
{
  Declare_instanciable( Op_Diff_K_Eps_PolyMAC_Elem ) ;

public:
  void associer_diffusivite_turbulente();
  void associer_diffusivite(const Champ_base& diffu);

  inline void associer_Pr_K_Eps(double, double);
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc& );
  void dimensionner(Matrice_Morse& mat) const;
  void completer();
  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const;
  void calculer_nu_sur_pr() const;

protected:
  double Prdt_K;
  double Prdt_Eps;
  mutable Champ_Fonc diffusivite_turbulente_;
  REF(Champ_base) vraie_diffusivite_turbulente_;
  Op_Diff_PolyMAC_Elem op_polymac;
};

inline void Op_Diff_K_Eps_PolyMAC_Elem::associer_Pr_K_Eps(double Pr_K, double Pr_Eps)
{
  Prdt_K = Pr_K;
  Prdt_Eps = Pr_Eps;
}

#endif /* Op_Diff_K_Eps_PolyMAC_Elem_included */
