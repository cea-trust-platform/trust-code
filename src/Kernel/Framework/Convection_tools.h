/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Convection_tools.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <ArrOfInt.h>
#include <Double.h>

//////////////////////////////////////////////////////////////
//   Fonctions limiteurs de MUSCL
////////////////////////////////////////////////////////////////


double minmod(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) (dabs(grad1)<dabs(grad2)) ? gradlim=grad1 : gradlim=grad2 ;
  return gradlim;
}

double vanleer(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) gradlim=2.*grad1*grad2/(grad1+grad2) ;
  return gradlim;
}

double vanalbada(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) gradlim=grad1*grad2*(grad1+grad2)/(grad1*grad1+grad2*grad2) ;
  return gradlim;
}


double chakravarthy(double grad1, double grad2)
{
  /*
    Cerr << " limiteur chakavarthy non preconise (non symetrique) " << finl;
    exit();
    return 0;
  */
  double gradlim=0.;
  if ((grad1*grad2)>0)
    {
      gradlim=dmin(grad1/grad2,1.8); // 1<<beta<<2
      gradlim=dmax(gradlim,0.);
      gradlim*=grad2;
    }
  return gradlim;
}

double superbee(double grad1, double grad2)
{
  /*
    Cerr << " limiteur superbee non preconise (source d'instabilites) " << finl;
    exit();
    return 0;
  */
  double gradlim=0.;
  if ((grad1*grad2)>0)
    {
      double gradlim1,gradlim2;
      gradlim1=dmin(2*(grad1/grad2),1);
      gradlim2=dmin(grad1/grad2,2);
      gradlim=dmax(gradlim1,gradlim2);
      gradlim=dmax(gradlim,0.);
      gradlim*=grad2;
    }
  return gradlim;
}
