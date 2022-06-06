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


#ifndef Pb_Hydraulique_Concentration_included
#define Pb_Hydraulique_Concentration_included

#include <Pb_Fluide_base.h>
#include <Navier_Stokes_std.h>
#include <Convection_Diffusion_Concentration.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Pb_Hydraulique_Concentration
//    Cette classe represente un probleme d'hydraulique avec transport
//    d'un ou plusieurs constituants:
//       - Equations de Navier_Stokes en regime laminaire
//         pour un fluide incompressible
//       - Equations de convection-diffusion en regime laminaire
//         En fait si on transporte plusieurs constituants on utilisera une
//         seule equation de convection-diffusion avec une inconnue vectorielle.
//         En general, on couple les 2 equations par l'intermediaire du terme
//         source des forces de volume de Navier_Stokes dans lequel on prend
//         en compte de petites variations de la masse volumique en fonction
//         du ou des constituants
// .SECTION voir aussi
//    Pb_Fluide_base
//////////////////////////////////////////////////////////////////////////////
class Pb_Hydraulique_Concentration : public Pb_Fluide_base
{

  Declare_instanciable(Pb_Hydraulique_Concentration);

public:

  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override ;
  Equation_base& equation(int) override;
  void associer_milieu_base(const Milieu_base& ) override;
  int verifier() override;

protected:

  Navier_Stokes_std eq_hydraulique;
  Convection_Diffusion_Concentration eq_concentration;

};



#endif
