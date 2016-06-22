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
// File:        Schema_Adams_Moulton_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Adams_Moulton_base_included
#define Schema_Adams_Moulton_base_included
//
// .DESCRIPTION class Schema_Adams_Moulton_base
//  Il herite de schema implicite base et porte un solveur par exemple
//  le Simpler pour effectuer les Faire_un_pas_de_temps..
// .SECTION voir aussi
//

#include <Schema_Implicite_Multi_TimeStep_base.h>
#include <DoubleTab.h>

class Probleme_Couple;

class Schema_Adams_Moulton_base : public Schema_Implicite_Multi_TimeStep_base
{
  Declare_base(Schema_Adams_Moulton_base);

public :

  void   associer_pb(const Probleme_base&);
  double changer_temps(Equation_base& eqn, const double& temps);
  void   update_time_derivative(Equation_base& eqn, const DoubleTab& data);
  void   mettre_a_jour_equation(Equation_base& eqn, const double& temps);
  void   store_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters);
  void   modify_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters);
  void   compute_coefficients(double time_step, const DoubleTab& times) const;
  void   add_multi_timestep_data(const Equation_base& eqn, Matrice_Base& mat_morse, DoubleTab& secmem) const;

  inline const DoubleTab& coefficients() const;
  inline DoubleTab& coefficients();

  //pour les schemas en temps a pas multiples
  void modifier_second_membre_full_impl(const Equation_base& eqn, DoubleTab& secmem);

protected :

  virtual void   compute_adams_moulton_coefficients(double time_step, const DoubleTab& times) const = 0;
  mutable DoubleTab adams_moulton_coefficients_;
};

inline const DoubleTab& Schema_Adams_Moulton_base::coefficients() const
{
  return adams_moulton_coefficients_;
}

inline DoubleTab& Schema_Adams_Moulton_base::coefficients()
{
  return adams_moulton_coefficients_;
}

#endif

