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

#ifndef Assembleur_P_VEFPreP1B_included
#define Assembleur_P_VEFPreP1B_included

#include <Assembleur_P_VEF.h>
#include <Matrice.h>
class Domaine_VEF;
class Champ_P1_isoP1Bulle;
enum class vecteur { second_membre , pression_inverse , pression };

class Assembleur_P_VEFPreP1B : public Assembleur_P_VEF
{

  Declare_instanciable(Assembleur_P_VEFPreP1B);

public :
  const Domaine_VEF& domaine_Vef() const;
  int assembler(Matrice&) override;
  int assembler_rho_variable(Matrice&, const Champ_Don_base&) override;
  int assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u) override;
  int modifier_secmem(DoubleTab&) override;
  int preparer_solution(DoubleTab&);
  int modifier_solution(DoubleTab&) override;
  int modifier_matrice(Matrice&) override;
  void verifier_dirichlet();
  void completer(const Equation_base& ) override;
  inline const Equation_base& equation() const
  {
    if (mon_equation.non_nul()==0)
      {
        Cerr << "\nError in Assembleur_P_VEFPreP1B::equation() : The equation is unknown !" << finl;
        Process::exit();
      }
    return mon_equation.valeur();
  };

  int modifier_secmem_elem(const DoubleTab&, DoubleTab&);
  int modifier_secmem_som(const DoubleTab&, DoubleTab&);
  int modifier_secmem_aretes(const DoubleTab&, DoubleTab&);

  // Methodes du changement de base P0+P1<->P1Bulle
  int changement_base() const
  {
    return (beta!=0);
  };
  void changer_base_matrice(Matrice&);              // A->A~ (P0+P1->P1Bulle)
  void changer_base_second_membre(DoubleVect&);     // y->y~ (P0+P1->P1Bulle)
  void changer_base_pression_inverse(DoubleVect&);  // x~->x (P1Bulle->P0+P1)
  void changer_base_pression(DoubleVect&);          // x->x~ (P0+P1->P1Bulle) Ne sert que pour les tests
  template <vecteur _v_>
  void changer_base(DoubleVect& v);


  Matrice la_matrice_de_travail_;                // Matrice de travail

protected:

  int init = -1;
  REF(Equation_base) mon_equation;
  void projete_L2(DoubleTab&);
  double alpha=0., beta=0.; // Coefficients du changement de base P0+P1<->P1Bulle
};

#endif



