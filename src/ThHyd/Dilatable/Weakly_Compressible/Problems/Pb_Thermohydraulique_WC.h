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
// File:        Pb_Thermohydraulique_WC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Problems
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Pb_Thermohydraulique_WC_included
#define Pb_Thermohydraulique_WC_included

#include <Pb_WC_base.h>
#include <Navier_Stokes_WC.h>
#include <Convection_Diffusion_Chaleur_WC.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Pb_Thermohydraulique_WC
//     Cette classe represente un probleme de thermohydraulique en fluide weakly compressible:
//      - Equations de Navier_Stokes en regime laminaire
//        pour un fluide weakly compressible
//      - Equation d'energie exprimee en temperature (gaz parfait) ou enthalpie (gaz reel)
//        en regime laminaire pour un fluide weakly compressible
// .SECTION voir aussi
//     Probleme_base Navier_Stokes_WC Convection_Diffusion_Chaleur_WC
//////////////////////////////////////////////////////////////////////////////

class Pb_Thermohydraulique_WC : public Pb_WC_base
{
  Declare_instanciable(Pb_Thermohydraulique_WC);
public:
  int nombre_d_equations() const;
  const Equation_base& equation(int) const ;
  Equation_base& equation(int);
  int verifier();

protected:
  Navier_Stokes_WC eq_hydraulique;
  Convection_Diffusion_Chaleur_WC eq_thermique;
};

#endif /* Pb_Thermohydraulique_WC_included */
