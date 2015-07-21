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
//////////////////////////////////////////////////////////////////////////////
//
// File:        MuLambda_TBLE_Fcts_T.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MuLambda_TBLE_Fcts_T_included
#define MuLambda_TBLE_Fcts_T_included

#include <MuLambda_TBLE_base.h>
#include <Parser_U.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe MuLambda_TBLE_Fcts_T
//    Classe abstraite calculant Mu et Lambda suivant une fonction de T
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class MuLambda_TBLE_Fcts_T : public MuLambda_TBLE_base
{
  Declare_instanciable(MuLambda_TBLE_Fcts_T);

public :

  void initialiser(const Milieu_base&);

  // "ind" represente le numero d'une maille TBLE sur l'instance eq_couch_lim referencee dans cette classe
  virtual double getNu(REF(Eq_couch_lim) eq_T, int ind) ;

  virtual double getAlpha(REF(Eq_couch_lim) eq_T, int ind) ;

  void setFcts(Nom&, Nom&) ;

private :

  Parser_U p_mu;
  Parser_U p_lambda;

  Nom chaine_mu;
  Nom chaine_lambda;

  double rho; // supposees cte
  double rhoCp;

};


#endif
