/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Pb_Multiphase_HEM.h>
#include <EChaine.h>

Implemente_instanciable(Pb_Multiphase_HEM, "Pb_HEM|Pb_Multiphase_HEM", Pb_Multiphase);
// XD Pb_Multiphase_HEM Pb_Multiphase Pb_HEM -1 A problem that allows the resolution of 2-phases mechanicaly and thermally coupled with 3 equations

Sortie& Pb_Multiphase_HEM::printOn(Sortie& os) const { return Pb_Multiphase::printOn(os); }
Entree& Pb_Multiphase_HEM::readOn(Entree& is) { return Pb_Multiphase::readOn(is); }

void Pb_Multiphase_HEM::typer_lire_correlation_hem()
{
  // Particular treatment in cas of Homogeneous Equilibrium Mechanical calculation using the Pb_HEM class
  // We enforce a interfacial flux correlation with constant coefficient
  if (has_correlation("flux_interfacial"))
    {
      for (auto &&corr : correlations)
        {
          if (corr.second.valeur().que_suis_je() == "Flux_interfacial_Coef_Constant")
            {
              Cout << "Flux_interfacial_Coef_Constant is already defined." << finl;
            }
          else
            {
              Cerr << "An interfacial flux correlation is already defined in the data file, but from a different kind that coef_constant." << finl;
              Cerr << "For a Pb_HEM, the interfacial flux correlation must be of the kind coef_constant. Please either remove the line as it will be automatically added or modify the expression." << finl;
              Process::exit();
            }
        }
    }
  else
    {
      // The coefficient 1e10 is enforced. If the user wants to modify it, we invite her to add the line in the data file.
      Cout << "Pb_HEM: we add a interfacial flux correlation with constant coefficient." << finl;
      Nom corr_FICC("{ flux_interfacial coef_constant { ");
      for (int ii = 0; ii < nb_phases(); ii++)
        {
          corr_FICC += nom_phase(ii);
          corr_FICC += " 1e10 ";
        }
      corr_FICC += "} }";
      Cout << "Expression added: " << corr_FICC << finl;
      Cout << "Please add this line to the data file directly if you want to modify the enforced coefficient 1e10." << finl;

      EChaine corr_flux_inter_coef_const(corr_FICC);
      lire_correlations(corr_flux_inter_coef_const);
    }
}
