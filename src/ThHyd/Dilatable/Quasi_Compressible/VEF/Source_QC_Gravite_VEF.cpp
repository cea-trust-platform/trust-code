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

#include <Fluide_Quasi_Compressible.h>
#include <Source_QC_Gravite_VEF.h>

Implemente_instanciable(Source_QC_Gravite_VEF,"Source_QC_Gravite_VEF",Source_Gravite_Fluide_Dilatable_base);

Sortie& Source_QC_Gravite_VEF::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Source_QC_Gravite_VEF::readOn(Entree& is) { return is; }

void Source_QC_Gravite_VEF::associer_zones(const Zone_dis& zone,const Zone_Cl_dis& zone_cl)
{
  associer_zones_impl(zone,zone_cl);
}

DoubleTab& Source_QC_Gravite_VEF::ajouter(DoubleTab& resu) const
{
  const Fluide_Quasi_Compressible& fluide = ref_cast(Fluide_Quasi_Compressible,le_fluide.valeur());
  const DoubleTab& tab_rho = fluide.rho_discvit();
  const double rho_m = fluide.get_traitement_rho_gravite() ? le_fluide->moyenne_vol(tab_rho) : 0.0;
  ajouter_impl(mon_equation.valeur(),g,dimension,rho_m,tab_rho,resu);
  return resu;
}
