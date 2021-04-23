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
// File:        Masse_ajoutee_Coef_Constant.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_ajoutee_Coef_Constant.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Masse_ajoutee_Coef_Constant, "Masse_ajoutee_Coef_Constant", Masse_ajoutee_base);

Sortie& Masse_ajoutee_Coef_Constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Masse_ajoutee_Coef_Constant::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("beta", &beta);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Masse_ajoutee_Coef_Constant::ajouter(const double *alpha, const double *rho, DoubleTab& a_r) const
{
  int k, l, n, N = a_r.dimension(0);
  double rho_m = 0;
  for (n = 0; n < N; n++) rho_m += alpha[n] * rho[n]; //masse volumique du melange
  for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (l != k)
        a_r(k, l) -= beta * rho_m * alpha[k] * alpha[l], a_r(k, k) += beta * rho_m * alpha[k] * alpha[l];
}
