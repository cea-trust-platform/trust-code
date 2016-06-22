/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        RRK2.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RRK2_included
#define RRK2_included





#include <Schema_Temps_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe RRK2
//     Cette classe represente un  schema de Runge Kutta
//     rationel d'ordre 2 qui s'ecrit:
//     U(n+1) = U(n) + (2g1 (g1,b) - b(g1,g1))/(b,b)
//     ou : g1 = dt f(U(n))
//          g2 = dt f(U(n) + c2 g1)
//          b = b1 g1 + b2 g2
//     on choisit b1=2, b2=-1, c2=1/2
// .SECTION voir aussi
//     Schema_Temps_base RK3
//////////////////////////////////////////////////////////////////////////////
class RRK2: public Schema_Temps_base
{

  Declare_instanciable(RRK2);

public :

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  virtual int nb_valeurs_temporelles() const;
  virtual int nb_valeurs_futures() const;
  virtual double temps_futur(int i) const;
  virtual double temps_defaut() const;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////

  virtual int faire_un_pas_de_temps_eqn_base(Equation_base&);
  inline void completer();
};

inline void RRK2::completer()
{
}


#endif
