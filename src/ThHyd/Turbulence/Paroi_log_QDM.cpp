/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_log_QDM.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////
#include <Paroi_log_QDM.h>
#include <Param.h>

Paroi_log_QDM::Paroi_log_QDM()
{
  A_plus = A_DEF;
  Kappa = K_DEF;
  Erugu = E_DEF;
  Cmu = CMU;
}

void Paroi_log_QDM::set_param(Param& param)
{
  param.ajouter("Kappa",&Kappa);
  param.ajouter("Erugu",&Erugu);
  param.ajouter("A_plus",&A_plus);
}

void Paroi_log_QDM::init_lois_paroi_hydraulique_()
{
  double (*pf)(double,double,double);
  DoubleVect les_valeurs(40);
  DoubleVect les_params(40);
  double val=0;
  int i;
  pf = &Fdypar;

  for (i=0; i<40; i++)
    les_params[i] = i+1;

  for (i=0; i<40; i++)
    {
      val += integrale(les_params[i]-1,les_params[i],Kappa,A_plus,pf);
      les_valeurs[i] = val*les_params[i];
    }

  table_hyd.remplir(les_params,les_valeurs);
}

double Paroi_log_QDM::calcul_lm_plus(double d_plus)
{
  return Kappa*d_plus*(1-exp(-d_plus/A_plus));
}

double Fdypar_direct(double lm_plus)
{
  return 2./(1+sqrt(1.0+4*lm_plus*lm_plus));
}

double Fdypar(double d_plus,double kappa,double a_plus)
{
  double lm_plus = kappa*d_plus*(1-exp(-d_plus/a_plus));
  return 2./(1+sqrt(1.0+4*lm_plus*lm_plus));
}

double integrale(double a,double b,double param1,double param2,double (*pf) (double,double,double))
{
  double sum = 0.5 * ( pf(a,param1,param2) + pf(b,param1,param2) ) * (b-a);
  return sum;
}

