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

#ifndef Convection_tools_included
#define Convection_tools_included


//////////////////////////////////////////////////////////////
//   Fonctions limiteurs de MUSCL
////////////////////////////////////////////////////////////////

#pragma omp declare target
inline double minmod(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) (std::fabs(grad1)<std::fabs(grad2)) ? gradlim=grad1 : gradlim=grad2 ;
  return gradlim;
}
#pragma omp end declare target

#pragma omp declare target
inline double vanleer(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) gradlim=2.*grad1*grad2/(grad1+grad2) ;
  return gradlim;
}
#pragma omp end declare target

#pragma omp declare target
inline double vanalbada(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) gradlim=grad1*grad2*(grad1+grad2)/(grad1*grad1+grad2*grad2) ;
  return gradlim;
}
#pragma omp end declare target

#pragma omp declare target
inline double chakravarthy(double grad1, double grad2)
{
  /*
    Cerr << " limiteur chakavarthy non preconise (non symetrique) " << finl;
    exit();
    return 0;
  */
  double gradlim=0.;
  if ((grad1*grad2)>0)
    {
      gradlim=std::min(grad1/grad2,1.8); // 1<<beta<<2
      gradlim=std::max(gradlim,0.);
      gradlim*=grad2;
    }
  return gradlim;
}
#pragma omp end declare target

#pragma omp declare target
inline double superbee(double grad1, double grad2)
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
      gradlim1=std::min(2*(grad1/grad2),1.);
      gradlim2=std::min(grad1/grad2,2.);
      gradlim=std::max(gradlim1,gradlim2);
      gradlim=std::max(gradlim,0.);
      gradlim*=grad2;
    }
  return gradlim;
}
#pragma omp end declare target

#pragma omp declare target
inline double FCT_LIMITEUR(double grad1, double grad2, int cas)
{
  switch(cas)
    {
    case 1:
      return minmod(grad1, grad2);
    case 2:
      return vanleer(grad1, grad2);
    case 3:
      return vanalbada(grad1, grad2);
    case 4:
      return chakravarthy(grad1, grad2);
    case 5:
      return superbee(grad1, grad2);
    default:
      return -1;
    }
}
#pragma omp end declare target

#endif
