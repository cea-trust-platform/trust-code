/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Assembleur_P_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Solveurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Assembleur_P_VEF_included
#define Assembleur_P_VEF_included

#include <Matrice_Morse_Sym.h>
#include <Assembleur_base.h>
#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Ref_Equation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS:Assembleur_Pression_VEF
//
//////////////////////////////////////////////////////////////////////////////

class Assembleur_P_VEF : public Assembleur_base
{
  Declare_instanciable(Assembleur_P_VEF);

public:
  void associer_zone_dis_base(const Zone_dis_base& )         ;
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& )   ;
  const Zone_dis_base& zone_dis_base() const                 ;
  const Zone_Cl_dis_base& zone_Cl_dis_base() const           ;
  int assembler(Matrice&)              ;
  int assembler_mat(Matrice&, const DoubleVect&, int, int);
  int remplir(Matrice&, const DoubleTab&);
  inline int remplir(Matrice&, const DoubleVect&)
  {
    assert(0);
    exit();
    return 1;
  };
  int assembler_QC(const DoubleTab&, Matrice&)            ;
  int modifier_secmem(DoubleTab&)                         ;
  int modifier_solution(DoubleTab&)                       ;
  virtual int modifier_matrice(Matrice&)                        ;
  void completer(const Equation_base& )                      ;
  inline const Equation_base& equation() const                ;

protected :
  REF(Equation_base) mon_equation                            ;
  REF(Zone_VEF) la_zone_VEF                                  ;
  REF(Zone_Cl_VEF) la_zone_Cl_VEF                            ;
  DoubleTab les_coeff_pression                               ;
  int has_P_ref;
  void calculer_inv_volume(DoubleTab& inv_volumes_entrelaces, const Zone_Cl_VEF& zone_Cl_VEF,const DoubleVect& volumes_entrelaces) ;

};

inline const Equation_base& Assembleur_P_VEF::equation() const
{
  if (mon_equation.non_nul()==0)
    {
      Cerr << "\nAssembleur_P_VEF::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return  mon_equation.valeur();
}

#endif
