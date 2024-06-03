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

#include <Loi_Etat_GP_QC.h>

Implemente_instanciable(Loi_Etat_GP_QC,"Loi_Etat_Gaz_Parfait_QC",Loi_Etat_Mono_GP_base);
// XD perfect_gaz_QC loi_etat_gaz_parfait_base gaz_parfait_QC 1 Class for perfect gas state law used with a quasi-compressible fluid.
// XD attr Cp double Cp 0 Specific heat at constant pressure (J/kg/K).
// XD attr Cv double Cv 1 Specific heat at constant volume (J/kg/K).
// XD attr gamma double gamma 1 Cp/Cv
// XD attr Prandtl double Prandtl 0 Prandtl number of the gas Pr=mu*Cp/lambda
// XD attr rho_constant_pour_debug field_base rho_constant_pour_debug 1 For developers to debug the code with a constant rho.

Sortie& Loi_Etat_GP_QC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_GP_QC::readOn(Entree& is)
{
  Cerr << "Lecture de la loi d'etat gaz parfait pour le QC ... " << finl;
  return Loi_Etat_Mono_GP_base::readOn(is);
}

double Loi_Etat_GP_QC::calculer_masse_volumique(double P, double T) const
{
  return rho_constant_pour_debug_.non_nul() ? rho_constant_pour_debug_(0,0) :
         Loi_Etat_Mono_GP_base::calculer_masse_volumique(P,T);
}

/*! @brief Calcule la masse volumique
 *
 */
void Loi_Etat_GP_QC::calculer_masse_volumique()
{
  Loi_Etat_Mono_GP_base::calculer_masse_volumique();
}
