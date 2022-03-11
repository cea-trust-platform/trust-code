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
// File:        Assembleur_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Assembleur_base_included
#define Assembleur_base_included

#include <Equation_base.h>
#include <SolveurSys.h>
#include <TRUSTTab.h>
class Zone_dis_base;
class Zone_Cl_dis_base;
class Matrice_Morse_Sym;
class Champ_Don_base;
class Matrice;

class Assembleur_base : public Objet_U
{

  Declare_base(Assembleur_base);

public:
  int set_resoudre_increment_pression(int flag)               ;
  int get_resoudre_increment_pression() const                    ;
  int set_resoudre_en_u(int flag)                             ;
  int get_resoudre_en_u() const                                  ;
  virtual int assembler(Matrice&)=0                              ;
  inline virtual int assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u)
  {
    Cerr<<"assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u) must be overloaded by "<<que_suis_je()<<finl;
    exit();
    return 1;
  };
  virtual int assembler_rho_variable(Matrice&,
                                     const Champ_Don_base& rho) ;
  virtual int assembler_QC(const DoubleTab&, Matrice&)           ;
  virtual void completer(const Equation_base& ) =0                  ;
  virtual void associer_zone_dis_base(const Zone_dis_base&) =0      ;
  virtual void associer_zone_cl_dis_base(const Zone_Cl_dis_base&) =0;
  virtual const Zone_dis_base& zone_dis_base() const =0             ;
  virtual const Zone_Cl_dis_base& zone_Cl_dis_base() const =0       ;
  virtual int modifier_secmem(DoubleTab&)=0                      ;
  virtual int modifier_solution(DoubleTab&)=0                    ;
  virtual void corriger_vitesses(const DoubleTab& dP, DoubleTab& dv) const
  {
    Cerr<<"corriger_vitesses(const DoubleTab& dP, DoubleTab& dv) must be overloaded by "<<que_suis_je()<<finl;
    exit();
  }

private:
  // Drapeau, indique si le solveur resout un increment de pression ou
  // la pression.
  int resoudre_increment_pression_;
  int resoudre_en_u_;
};

#endif


