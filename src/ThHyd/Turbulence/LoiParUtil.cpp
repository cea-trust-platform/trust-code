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
// File:        LoiParUtil.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////
#include <LoiParUtil.h>
#include <Double.h>


static double valmin_ss_couche_log=20;
static double valmax_ss_couche_log=100;

static inline double fn_uplus(double dplus)
{
  static double chi=1./0.41;
  static double beta=5.5;
  //   if(dplus<valmin_ss_couche_log)
  //     return dplus;
  //   else
  //   if(dplus>valmax_ss_couche_log)
  //     // WARNING max depasse ...
  return chi*log(dplus)+beta;
  //   else
  //     return chi*log(dplus)+beta;
}
static inline double fn_uplus_dplus(double dplus)
{
  return dplus*fn_uplus(dplus);
}
static double fn_uplus_dplus_inv(double uplusdplus)
{
  double b=(fn_uplus_dplus(valmax_ss_couche_log)-fn_uplus_dplus(valmin_ss_couche_log))/(valmax_ss_couche_log-valmin_ss_couche_log);
  static double a=fn_uplus_dplus(0.5*(valmin_ss_couche_log+valmax_ss_couche_log))-b*0.5*(valmin_ss_couche_log+valmax_ss_couche_log);
  static double seuil=1.e-12;
  double x=(uplusdplus-a)/b;
  double err=uplusdplus-fn_uplus_dplus(x);
  double fx=fn_uplus_dplus(x);
  while(dabs(err/b)>seuil*x)
    {
      b=fn_uplus_dplus(x+1)-fx;
      x+=err/b;
      fx=fn_uplus_dplus(x);
      err=uplusdplus-fx;
    }
  return x;
}



//
//
//
//

double calculer_u_tau(double nu, double us, double delta, int& type_couche)
{
  double uplusdplus=us*delta/nu;
  //if(fn_uplus_dplus(valmin_ss_couche_log)>=uplusdplus)
  if(0)
    {
      // sous couche visqueuse :
      type_couche=0;
      double dplus=sqrt(uplusdplus);
      //       if(dplus > valmin_ss_couche_log)
      //         {
      //           Cerr << "PB .... " << finl;
      //           exit();
      //         }
      double uplus=uplusdplus/dplus;
      return us/uplus;
    }
  else
    //if(fn_uplus_dplus(valmax_ss_couche_log)>=uplusdplus)
    if(1)
      {
        double dplus=fn_uplus_dplus_inv(uplusdplus);
        //Cerr << "y+ = " << dplus << finl;
        //         if( (dplus < valmin_ss_couche_log) || (dplus > valmax_ss_couche_log) )
        //         {
        //           Cerr << "PB .... " << finl;
        //           exit();
        //         }
        type_couche=1;
        double uplus=uplusdplus/dplus;
        return us/uplus;
      }
    else
      {
        type_couche=2;
      }
  return -1;
}
