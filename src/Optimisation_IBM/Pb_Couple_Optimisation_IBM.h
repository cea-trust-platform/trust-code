/****************************************************************************
* Copyright (c) 2025, CEA
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
// File:        Pb_Couple_Optimisation_IBM.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Pb_Couple_Optimisation_IBM_included
#define Pb_Couple_Optimisation_IBM_included

#include <Probleme_Couple.h>
#include <Interpolation_IBM_base.h>
#include <TRUST_Ref.h>

class Cond_lim_base;

class Schema_Temps_base;
class Discretisation_base;

class Pb_Couple_Optimisation_IBM: public Probleme_Couple
{
  Declare_instanciable(Pb_Couple_Optimisation_IBM);

public:
  int  associer_(Objet_U&) override;
  void validateTimeStep() override;
  void initialize() override;
  void le_modele_interpolation_IBM(const Interpolation_IBM_base&);
  inline Interpolation_IBM_base& my_interpolation_IBM();

protected:
  OBS_PTR(Interpolation_IBM_base) my_interpolation_IBM_;
};

inline Interpolation_IBM_base& Pb_Couple_Optimisation_IBM::my_interpolation_IBM()
{
  return my_interpolation_IBM_;
}
#endif
