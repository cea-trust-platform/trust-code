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

#ifndef Schema_Adams_Bashforth_base_included
#define Schema_Adams_Bashforth_base_included

#include <Schema_Explicite_Multi_TimeStep_base.h>
#include <TRUSTTabs_forward.h>

/*! @brief classe Schema_Adams_Bashforth_base
 *
 * @sa Schema_Temps_base
 */
class Schema_Adams_Bashforth_base: public Schema_Explicite_Multi_TimeStep_base
{

  Declare_base(Schema_Adams_Bashforth_base);

public :

  void   associer_pb(const Probleme_base&) override;
  void   modifier_second_membre(const Equation_base& eqn, DoubleTab& secmem) override ;
  void   compute_coefficients(double time_step, const DoubleTab& times) override;

  inline const DoubleTab& coefficients() const override;
  inline DoubleTab& coefficients() override;

protected :

  virtual void   compute_adams_bashforth_coefficients(double time_step, const DoubleTab& times)     = 0;


private :

  DoubleTab adams_bashforth_coefficients_;
};

inline const DoubleTab& Schema_Adams_Bashforth_base::coefficients() const
{
  return adams_bashforth_coefficients_;
}

inline DoubleTab& Schema_Adams_Bashforth_base::coefficients()
{
  return adams_bashforth_coefficients_;
}

#endif
