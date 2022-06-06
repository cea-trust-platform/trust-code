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


#ifndef Pb_Hydraulique_included
#define Pb_Hydraulique_included

#include <Pb_Fluide_base.h>
#include <Champ_Don.h>
#include <Navier_Stokes_std.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Pb_Hydraulique
//     Cette classe represente un probleme hydraulique standard dans lequel
//     on resout les equations de Navier Stokes en regime laminaire
//     pour un fluide incompressible
//     La formulation est de type vitesse pression
// .SECTION voir aussi
//      Navier_Stokes_std Pb_Fluide_base Fluide_base
//////////////////////////////////////////////////////////////////////////////
class Pb_Hydraulique : public Pb_Fluide_base
{
  Declare_instanciable(Pb_Hydraulique);

public :

  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override;
  Equation_base& equation(int) override;
  void associer_milieu_base(const Milieu_base& ) override;

protected :

  Navier_Stokes_std eq_hydraulique;

};


#endif
