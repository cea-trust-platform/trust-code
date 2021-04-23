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
// File:        Flux_interfacial_Coef_Constant.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Flux_interfacial_Coef_Constant.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Flux_interfacial_Coef_Constant, "Flux_interfacial_Coef_Constant", Flux_interfacial_base);

Sortie& Flux_interfacial_Coef_Constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_interfacial_Coef_Constant::readOn(Entree& is)
{
  Param param(que_suis_je());
  //un parametre par phase du probleme
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, pb_.valeur());
  h_phase.resize(pbm.nb_phases());
  for (int n = 0; n < pbm.nb_phases(); n++)
    param.ajouter(pbm.nom_phase(n), &h_phase(n), Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Flux_interfacial_Coef_Constant::coeffs(const double& dh, const double *alpha, const double *T, const double p, const double *nv,
                                            const double *lambda, const double *mu, const double *rho, const double *Cp,
                                            DoubleTab& hi, DoubleTab& dT_hi, DoubleTab& da_hi, DoubleTab& dp_hi) const
{
  int k, l, N = hi.dimension(0);
  for (k = 0; k < N; k++) for (l = 0; l < N; l++) hi(k, l) = h_phase(k); //ne depend que de la phase de depart
  dT_hi = 0, da_hi = 0, dp_hi = 0; //pas trop dur
}
