/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Pb_Conduction.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Pb_Conduction_included
#define Pb_Conduction_included

#include <Probleme_base.h>
#include <Conduction.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Pb_Conduction
//    Cette classe represente un probleme de conduction avec rho et Cp non uniformes :
//      (rho.Cp) dT/dt -div (k grad T) = (rho.Cp) f
//    Cette classe est exactement identique a Pb_Conduction si ce n'est qu'elle force l'utilisateur a l'usage d'un probleme
//    de nature differente.
// .SECTION voir aussi
//     Probleme_base
//     Pb_Conduction
//////////////////////////////////////////////////////////////////////////////
class Pb_Conduction : public Probleme_base
{

  Declare_instanciable(Pb_Conduction);

public:

  int nombre_d_equations() const;
  const Equation_base& equation(int) const ;
  Equation_base& equation(int);

protected:

  Conduction  eq_conduction;

};

#endif




