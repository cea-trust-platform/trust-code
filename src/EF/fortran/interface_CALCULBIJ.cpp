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
//////////////////////////////////////////////////////////////////////////////
//
// File:        interface_CALCULBIJ.cpp
// Directory:   $TRUST_ROOT/src/EF/fortran
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <interface_CALCULBIJ.h>
#include <arch_fort.h>
#include <LecFicDiffuse.h>
#include <Double.h>
#include <Param.h>
extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(CALCULBIJ)(const integer* nbnn, const integer* nbsomtot, const doublereal* xl, const integer* num, doublereal* bij, const doublereal* poro, const integer* ip, const integer* npgau, const doublereal* xgau, const doublereal* frgau, const doublereal* dfrgau, const doublereal* poigau, doublereal* detj, doublereal* ajm1, doublereal* aj, doublereal* df, doublereal* volume, doublereal* volume_sommet, doublereal* iphi);
#else
  int F77DECLARE(calculbij)(const integer* nbnn, const integer* nbsomtot, const doublereal* xl, const integer* num, doublereal* bij, const doublereal* poro, const integer* ip, const integer* npgau, const doublereal* xgau, const doublereal* frgau, const doublereal* dfrgau, const doublereal* poigau, doublereal* detj, doublereal* ajm1, doublereal* aj, doublereal* df, doublereal* volume, doublereal* volume_sommet, doublereal* iphi);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(CALCULBIJ)(const integer* nbnn, const integer* nbsomtot, const doublereal* xl, const integer* num, doublereal* bij, const doublereal* poro, const integer* ip, const integer* npgau, const doublereal* xgau, const doublereal* frgau, const doublereal* dfrgau, const doublereal* poigau, doublereal* detj, doublereal* ajm1, doublereal* aj, doublereal* df, doublereal* volume, doublereal* volume_sommet, doublereal* iphi)
{
  return F77NAME(calculbij)( nbnn,  nbsomtot,  xl,  num,  bij,  poro,  ip,  npgau,  xgau,  frgau,  dfrgau,  poigau,  detj,  ajm1,  aj,  df,  volume,  volume_sommet,  iphi);
}
#endif

void interface_CALCULBIJ::initialiser_les_commons()
{
}
void interface_CALCULBIJ::init()
{
  initialiser_les_commons();
  init_sans_initialiser_les_commons();
}
void interface_CALCULBIJ::init_sans_initialiser_les_commons()
{
}
void interface_CALCULBIJ::init_from_common()
{
}
void interface_CALCULBIJ::check_common()
{
}

void interface_CALCULBIJ::compare_common()
{
}

void interface_CALCULBIJ::Compute(const int& nbnn, const int& nbsomtot, const ArrOfDouble& xl, const ArrOfInt& num, ArrOfDouble& bij, const ArrOfDouble& poro, const int& ip, const int& npgau, const ArrOfDouble& xgau, const ArrOfDouble& frgau, const ArrOfDouble& dfrgau, const ArrOfDouble& poigau, ArrOfDouble& detj, ArrOfDouble& ajm1, ArrOfDouble& aj, ArrOfDouble& df, double& volume, ArrOfDouble& volume_sommet, ArrOfDouble& iphi) const
{
  F77NAME(CALCULBIJ)(&nbnn, &nbsomtot,  xl.addr(),  num.addr(),  bij.addr(),  poro.addr(), &ip, &npgau,  xgau.addr(),  frgau.addr(),  dfrgau.addr(),  poigau.addr(),  detj.addr(),  ajm1.addr(),  aj.addr(),  df.addr(), &volume,  volume_sommet.addr(),  iphi.addr());
}
void interface_CALCULBIJ::init_from_file(const Nom& nom_fic)
{
  LecFicDiffuse entree(nom_fic);
  Param param("interface_CALCULBIJ");
  set_param(param);
  param.read(entree);
}

void interface_CALCULBIJ::dump(Sortie& os)
{
  Param param("interface_CALCULBIJ");
  set_param(param);
  param.print(os);
}

void  interface_CALCULBIJ::set_param(Param& param)
{
}


