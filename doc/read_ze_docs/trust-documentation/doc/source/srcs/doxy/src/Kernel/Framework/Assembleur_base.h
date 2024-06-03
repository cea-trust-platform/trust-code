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

#ifndef Assembleur_base_included
#define Assembleur_base_included

#include <TRUSTTabs_forward.h>
#include <Equation_base.h>
#include <SolveurSys.h>

class Matrice_Morse_Sym;
class Domaine_Cl_dis_base;
class Champ_Don_base;
class Domaine_dis_base;
class Matrice;

class Assembleur_base : public Objet_U
{
  Declare_base(Assembleur_base);
public:
  int set_resoudre_increment_pression(int flag);
  int get_resoudre_increment_pression() const;
  int set_resoudre_en_u(int flag);
  int get_resoudre_en_u() const;
  virtual int assembler(Matrice&)=0;
  inline virtual int assembler_mat(Matrice&, const DoubleVect&, int incr_pression, int resoudre_en_u)
  {
    Cerr << "assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u) must be overloaded by " << que_suis_je() << finl;
    exit();
    return 1;
  }

  virtual int assembler_rho_variable(Matrice&, const Champ_Don_base& rho);
  virtual int assembler_QC(const DoubleTab&, Matrice&);
  virtual void completer(const Equation_base&) =0;
  virtual void associer_domaine_dis_base(const Domaine_dis_base&) =0;
  virtual void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base&) =0;
  virtual const Domaine_dis_base& domaine_dis_base() const =0;
  virtual const Domaine_Cl_dis_base& domaine_Cl_dis_base() const =0;
  virtual int modifier_secmem(DoubleTab&)=0;
  /* prise en compte des variations de pression aux CLs lors du calcul d'increments de pression.
     Utilise seulement par PolyMAC_P0P1NC. fac est le coefficient tel que p_final - press = fac * sol */
  virtual void modifier_secmem_pour_incr_p(const DoubleTab& press, const double fac, DoubleTab& incr) const { };
  virtual int modifier_solution(DoubleTab&)=0;

  /* dimensionnement / assemblage de l'equation de continuite sum_k alpha_k = 1 en Pb_continuite */
  /* meme interface que dimensionner/assembler_blocs dans Equation_base */
  virtual void dimensionner_continuite(matrices_t matrices, int aux_only = 0) const
  {
    Process::exit(Nom("dimensionner_continuite(...) must be overloaded by ") + que_suis_je());
  }

  virtual void assembler_continuite(matrices_t matrices, DoubleTab& secmem, int aux_only = 0) const
  {
    Process::exit(Nom("assembler_continuite(...) must be overloaded by ") + que_suis_je());
  }

  virtual DoubleTab norme_continuite() const
  {
    Process::exit(Nom("norme_continuite(...) must be overloaded by ") + que_suis_je());
    return DoubleTab();
  }

  virtual void corriger_vitesses(const DoubleTab& dP, DoubleTab& dv) const
  {
    Process::exit(Nom("corriger_vitesses(...) must be overloaded by ") + que_suis_je());
  }

private:
  // Drapeau, indique si le solveur resout un increment de pression ou
  // la pression.
  int resoudre_increment_pression_;
  int resoudre_en_u_;
};

#endif


