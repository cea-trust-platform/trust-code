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
// File:        Leap_frog.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Leap_frog_included
#define Leap_frog_included

#include <Schema_Temps_base.h>


/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Leap_frog
//     Cette classe represente un schema en temps Leap_Frog.
//     Si (n<=4)
//        U(n+1) = 2*dt*(dU/dt)(n) + U(n)
//     Si(n>4)
//        U(n+1) = dt*(dU/dt)(n) + gamma*U(n) + (1-2*gamma)*U(n-1)
//                                            + gamma*U(n-2)
// .SECTION voir aussi
//    Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class Leap_frog : public Schema_Temps_base
{

  Declare_instanciable(Leap_frog);

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

inline void Leap_frog::completer()
{
}


#endif



