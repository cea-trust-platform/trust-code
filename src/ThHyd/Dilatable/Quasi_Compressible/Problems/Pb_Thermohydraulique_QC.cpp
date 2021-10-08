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
// File:        Pb_Thermohydraulique_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Problems
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_Thermohydraulique_QC.h>

Implemente_instanciable(Pb_Thermohydraulique_QC,"Pb_Thermohydraulique_QC",Pb_QC_base);

Sortie& Pb_Thermohydraulique_QC::printOn(Sortie& os) const { return Probleme_base::printOn(os); }

Entree& Pb_Thermohydraulique_QC::readOn(Entree& is) { return Probleme_base::readOn(is); }

// Description:
//    Renvoie 2 car il y a 2 equations : Navier_Stokes_QC et Convection_Diffusion_Chaleur_QC
int Pb_Thermohydraulique_QC::nombre_d_equations() const { return 2; }

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_QC si i=0
//    Renvoie l'equation de la thermique de type Convection_Diffusion_Chaleur_QC si i=1
const Equation_base& Pb_Thermohydraulique_QC::equation(int i) const
{
  return equation_impl(i,eq_hydraulique,eq_thermique);
}

Equation_base& Pb_Thermohydraulique_QC::equation(int i)
{
  return equation_impl(i,eq_hydraulique,eq_thermique);
}

// Description:
//    Teste la compatibilite des equations de la thermique et de l'hydraulique.
int Pb_Thermohydraulique_QC::verifier()
{
  return verifier_impl(eq_hydraulique,eq_thermique, true /* is_thermal */);
}
