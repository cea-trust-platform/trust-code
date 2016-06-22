/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        interface_INITGAUSS.cpp
// Directory:   $TRUST_ROOT/src/EF/fortran
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <interface_INITGAUSS.h>
#include <arch_fort.h>
#include <ArrOfDouble.h>
#include <ArrOfInt.h>
#include <LecFicDiffuse.h>
#include <Double.h>
#include <Param.h>
extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(INITGAUSS)(const integer* idimg, const integer* nbnn, const integer* npgau, doublereal* xgau, doublereal* frgau, doublereal* dfrgau, doublereal* poigau);
#else
  int F77DECLARE(initgauss)(const integer* idimg, const integer* nbnn, const integer* npgau, doublereal* xgau, doublereal* frgau, doublereal* dfrgau, doublereal* poigau);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(INITGAUSS)(const integer* idimg, const integer* nbnn, const integer* npgau, doublereal* xgau, doublereal* frgau, doublereal* dfrgau, doublereal* poigau)
{
  return F77NAME(initgauss)( idimg,  nbnn,  npgau,  xgau,  frgau,  dfrgau,  poigau);
}
#endif

void interface_INITGAUSS::initialiser_les_commons()
{
}
void interface_INITGAUSS::init()
{
  initialiser_les_commons();
  init_sans_initialiser_les_commons();
}
void interface_INITGAUSS::init_sans_initialiser_les_commons()
{
}
void interface_INITGAUSS::init_from_common()
{
}
void interface_INITGAUSS::check_common()
{
}

void interface_INITGAUSS::compare_common()
{
}

void interface_INITGAUSS::Compute(const int& idimg, const int& nbnn, const int& npgau, ArrOfDouble& xgau, ArrOfDouble& frgau, ArrOfDouble& dfrgau, ArrOfDouble& poigau) const
{
  F77NAME(INITGAUSS)(&idimg, &nbnn, &npgau,  xgau.addr(),  frgau.addr(),  dfrgau.addr(),  poigau.addr());
}
void interface_INITGAUSS::init_from_file(const Nom& nom_fic)
{
  LecFicDiffuse entree(nom_fic);
  Param param("interface_INITGAUSS");
  set_param(param);
  param.read(entree);
}

void interface_INITGAUSS::dump(Sortie& os)
{
  Param param("interface_INITGAUSS");
  set_param(param);
  param.print(os);
}

void  interface_INITGAUSS::set_param(Param& param)
{
}


