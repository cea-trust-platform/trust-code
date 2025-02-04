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

#include <Convection_Diffusion_Temperature_IBM.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Param.h>

Implemente_instanciable(Convection_Diffusion_Temperature_IBM, "Convection_Diffusion_Temperature_IBM", Convection_Diffusion_Temperature);

Sortie& Convection_Diffusion_Temperature_IBM::printOn(Sortie& is) const
{
  return Convection_Diffusion_Temperature::printOn(is);
}

Entree& Convection_Diffusion_Temperature_IBM::readOn(Entree& is)
{
  Convection_Diffusion_Temperature::readOn(is);
  readOn_IBM(is, *this);
  return is;
}

void Convection_Diffusion_Temperature_IBM::set_param(Param& param)
{
  Convection_Diffusion_Temperature::set_param(param);
  set_param_IBM(param);
}

int Convection_Diffusion_Temperature_IBM::preparer_calcul()
{
  Equation_base::preparer_calcul();

  if (is_IBM())
    preparer_calcul_IBM();

  return 1;
}

bool Convection_Diffusion_Temperature_IBM::initTimeStep(double dt)
{
  Convection_Diffusion_Temperature::initTimeStep(dt);

  if (is_IBM())
    initTimeStep_IBM(dt);

  return true;
}

// ajoute les contributions des operateurs et des sources
void Convection_Diffusion_Temperature_IBM::assembler(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  assembler_proto(matrice, inco, resu);
}

// for IBM methods; on ajoute source PDF au RHS
void Convection_Diffusion_Temperature_IBM::derivee_en_temps_inco_sources(DoubleTrav& secmem)
{
  if (is_IBM())
    derivee_en_temps_inco_IBM(secmem);
}

void Convection_Diffusion_Temperature_IBM::verify_scheme()
{
  // for IBM methods
  if (is_IBM() && equation_non_resolue() == 0)
    {
      Cerr << "*******(IBM) Use an implicit time scheme (at least Euler explicit + diffusion) with Source_PDF_base.*******" << finl;
      abort();
    }
}
