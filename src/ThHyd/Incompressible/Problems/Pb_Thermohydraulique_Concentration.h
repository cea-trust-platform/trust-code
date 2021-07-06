/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Pb_Thermohydraulique_Concentration.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Problems
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Pb_Thermohydraulique_Concentration_included
#define Pb_Thermohydraulique_Concentration_included

#include <Pb_Fluide_base.h>
#include <Navier_Stokes_std.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Pb_Thermohydraulique_Concentration
//    Cette classe represente un probleme de thermohydraulique avec
//    concentrations :
//     - Equations de Navier_Stokes en regime laminaire pour un fluide
//       incompressible
//     - Equation d'energie en regime laminaire
//     - Une equation de convection-diffusion pour un ou plusieurs
//       constituants caracterises par leurs concentrations
//       En general, on couple les equations d'energie et de concentration aux
//       equations de Navier-Stokes par l'intermediaire du terme source
//       des forces de volume dans lequel on prend en compte de petites
//       variations de la masse volumique en fonction de la temperature et
//       de la concentration.
// .SECTION voir aussi
//      Pb_Fluide_base
//////////////////////////////////////////////////////////////////////////////
class Pb_Thermohydraulique_Concentration : public Pb_Fluide_base
{

  Declare_instanciable(Pb_Thermohydraulique_Concentration);

public:

  int nombre_d_equations() const;
  const Equation_base& equation(int) const ;
  Equation_base& equation(int);
  void associer_milieu_base(const Milieu_base& );
  int verifier();

protected:

  Navier_Stokes_std eq_hydraulique;
  Convection_Diffusion_Temperature eq_thermique;
  Convection_Diffusion_Concentration eq_concentration;

};



#endif
