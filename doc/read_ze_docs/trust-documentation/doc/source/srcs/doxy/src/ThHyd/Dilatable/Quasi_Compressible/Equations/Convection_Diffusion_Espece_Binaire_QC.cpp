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

#include <Convection_Diffusion_Espece_Binaire_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Loi_Etat_Binaire_GP_QC.h>

Implemente_instanciable(Convection_Diffusion_Espece_Binaire_QC,"Convection_Diffusion_Espece_Binaire_QC",Convection_Diffusion_Espece_Binaire_base);
// XD convection_diffusion_espece_binaire_QC eqn_base convection_diffusion_espece_binaire_QC -1 Species conservation equation for a binary quasi-compressible fluid.

Sortie& Convection_Diffusion_Espece_Binaire_QC::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Binaire_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Binaire_QC::readOn(Entree& is)
{
  return Convection_Diffusion_Espece_Binaire_base::readOn(is);
}

void Convection_Diffusion_Espece_Binaire_QC::completer()
{
  assert(le_fluide->loi_etat().valeur().que_suis_je() == "Loi_Etat_Binaire_Gaz_Parfait_QC");
  Convection_Diffusion_Espece_Binaire_base::completer();
}
