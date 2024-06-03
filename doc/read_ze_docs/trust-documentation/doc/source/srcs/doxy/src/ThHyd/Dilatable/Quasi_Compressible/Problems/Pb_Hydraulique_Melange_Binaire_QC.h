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

#ifndef Pb_Hydraulique_Melange_Binaire_QC_included
#define Pb_Hydraulique_Melange_Binaire_QC_included

#include <Convection_Diffusion_Espece_Binaire_QC.h>
#include <Pb_Dilatable_Proto.h>
#include <Navier_Stokes_QC.h>
#include <Pb_QC_base.h>

/*! @brief classe Pb_Hydraulique_Melange_Binaire_QC Cette classe represente un probleme de hydraulique binaire en fluide quasi compressible:
 *
 *       - Equations de Navier_Stokes en regime laminaire
 *       - Equation de conv/diff fraction massique en regime laminaire
 *
 * @sa Probleme_base Navier_Stokes_QC Convection_Diffusion_Espece_Binaire_QC
 */

class Pb_Hydraulique_Melange_Binaire_QC : public Pb_QC_base, public Pb_Dilatable_Proto
{
  Declare_instanciable(Pb_Hydraulique_Melange_Binaire_QC);
public:
  int verifier() override;
  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override ;
  Equation_base& equation(int) override;

protected:
  Navier_Stokes_QC eq_hydraulique;
  Convection_Diffusion_Espece_Binaire_QC eq_frac_mass;
};

#endif /* Pb_Hydraulique_Melange_Binaire_QC_included */
