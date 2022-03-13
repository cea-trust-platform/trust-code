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
// File:        Parser_Eval.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Parser_Eval_included
#define Parser_Eval_included

#include <Vect_Parser_U.h>
#include <TRUSTTabs.h>
#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Parser_Eval
//    Cette classe a pour fonction d evaleur les valeurs prises par une fonction analytique.
//    La fonction peut dependre de l espace (x, y, z), du temps (t) et d'un champ parametre (val).
// .SECTION voir aussi
//    Classe non instanciable.
//////////////////////////////////////////////////////////////////////////////////////////////////
class Parser_Eval
{

public :

  inline VECT(Parser_U)& fonction()
  {
    return fonction_;
  };
  Parser_U& parser(int i)
  {
    return fonction_[i];
  };
  Parser_U& parser(int i) const
  {
    return fonction_[i];
  };

  //Cas d une fonction a evaluer qui depend de l espace
  //
  void eval_fct(const DoubleVect& positions,DoubleVect& val) const;
  void eval_fct(const DoubleTab& positions,DoubleTab& val) const;
  void eval_fct(const DoubleTab& positions,DoubleVect& val,const int& ncomp) const;

  //Cas d une fonction a evaluer qui depend de l espace et du temps
  //
  void eval_fct(const DoubleVect& positions,const double& tps,DoubleVect& val) const;
  void eval_fct(const DoubleTab& positions,const double& tps,DoubleTab& val) const;
  void eval_fct(const DoubleTab& positions,const double& tps,DoubleVect& val,const int& ncomp) const;

  //Cas d une fonction a evaluer qui depend de l espace, du temps et des valeurs d un champ inconnu
  //
  void eval_fct(const DoubleTab& positions,const double& tps,const DoubleTab& val_param,DoubleTab& val) const;
  void eval_fct(const DoubleTab& positions,const double& tps,const DoubleTab& val_param,DoubleVect& val,const int& ncomp) const;

  // Fonction generale qui depend de plusieurs champs inconnus
  //
  void eval_fct(const DoubleTabs& variables, DoubleTab& val) const;


protected :

  mutable VECT(Parser_U) fonction_;
};


#endif
