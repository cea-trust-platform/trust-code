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
// File:        Schema_Explicite_Multi_TimeStep_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Explicite_Multi_TimeStep_base_included
#define Schema_Explicite_Multi_TimeStep_base_included


#include <Schema_Temps_base.h>

//////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION
//     classe Schema_Explicite_Multi_TimeStep_base
// .SECTION voir aussi
//     Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class Schema_Explicite_Multi_TimeStep_base: public Schema_Temps_base
{

  Declare_base(Schema_Explicite_Multi_TimeStep_base);

public :

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  int nb_valeurs_futures() const;
  double temps_futur(int i) const;
  double temps_defaut() const;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////
  virtual void       modifier_second_membre(const Equation_base& eqn, DoubleTab& secmem)         = 0;
  virtual void       compute_coefficients(double time_step, const DoubleTab& times)                   = 0;
  virtual DoubleTab& coefficients()                                                                   = 0;
  virtual int              nb_pas_dt_seuil()                                                          const = 0;
  virtual int              nb_valeurs_passees()                                                       const = 0;
  virtual const DoubleTab& coefficients()                                                             const = 0;

  int faire_un_pas_de_temps_eqn_base(Equation_base&)   ;

  inline void completer();

protected :

  void authorized_equation(const Equation_base& eqn);
  void authorized_equation(const Equation_base& eqn) const;
};

inline void Schema_Explicite_Multi_TimeStep_base::completer()
{
}

#endif
