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
// File:        Terme_Source_Coriolis.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Sources
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Source_Coriolis_included
#define Terme_Source_Coriolis_included

#include <Ref_Navier_Stokes_std.h>
#include <TRUSTVect.h>

class Terme_Source_Coriolis
{

public:

  void associer_eqn(const Navier_Stokes_std&) ;
  inline const DoubleVect& omega() const;
  inline const Navier_Stokes_std& eq_hydraulique() const;
  void mettre_a_jour(double temps)
  {
    ;
  }

protected :
  DoubleVect omega_;
  REF(Navier_Stokes_std) eq_hydraulique_;
  Entree& lire_donnees(Entree& );
  double dim;

};

//
//   Fonctions inline de la classe Terme_Source_Coriolis
//

inline const DoubleVect& Terme_Source_Coriolis::omega() const
{
  return omega_;
}

inline const Navier_Stokes_std& Terme_Source_Coriolis::eq_hydraulique() const
{
  return eq_hydraulique_.valeur();
}

#endif
