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
#include <interface_CALCULSI.h>
#include <arch_fort.h>
#include <LecFicDiffuse.h>
#include <Param.h>
extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(CALCULSI)(const integer* nbnn, const doublereal* co3d, const doublereal* poro, const integer* ip, const integer* npgau, const doublereal* xgau, const doublereal* frgau, const doublereal* dfrgau, const doublereal* poigau, doublereal* detj, const doublereal* cnorme, doublereal* si);
#else
  int F77DECLARE(calculsi)(const integer* nbnn, const doublereal* co3d, const doublereal* poro, const integer* ip, const integer* npgau, const doublereal* xgau, const doublereal* frgau, const doublereal* dfrgau, const doublereal* poigau, doublereal* detj, const doublereal* cnorme, doublereal* si);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(CALCULSI)(const integer* nbnn, const doublereal* co3d, const doublereal* poro, const integer* ip, const integer* npgau, const doublereal* xgau, const doublereal* frgau, const doublereal* dfrgau, const doublereal* poigau, doublereal* detj, const doublereal* cnorme, doublereal* si)
{
  return F77NAME(calculsi)( nbnn,  co3d,  poro,  ip,  npgau,  xgau,  frgau,  dfrgau,  poigau,  detj,  cnorme,  si);
}
#endif

void interface_CALCULSI::initialiser_les_commons()
{
}
void interface_CALCULSI::init()
{
  initialiser_les_commons();
  init_sans_initialiser_les_commons();
}
void interface_CALCULSI::init_sans_initialiser_les_commons()
{
}
void interface_CALCULSI::init_from_common()
{
}
void interface_CALCULSI::check_common()
{
}

void interface_CALCULSI::compare_common()
{
}

void interface_CALCULSI::Compute(const int nbnn, const ArrOfDouble& co3d, const ArrOfDouble& poro, const int ip, const int npgau, const ArrOfDouble& xgau, const ArrOfDouble& frgau, const ArrOfDouble& dfrgau, const ArrOfDouble& poigau, ArrOfDouble& detj, const ArrOfDouble& cnorme, ArrOfDouble& si) const
{
  F77NAME(CALCULSI)(&nbnn,  co3d.addr(),  poro.addr(), &ip, &npgau,  xgau.addr(),  frgau.addr(),  dfrgau.addr(),  poigau.addr(),  detj.addr(),  cnorme.addr(),  si.addr());
}
void interface_CALCULSI::init_from_file(const Nom& nom_fic)
{
  LecFicDiffuse entree(nom_fic);
  Param param("interface_CALCULSI");
  set_param(param);
  param.read(entree);
}

void interface_CALCULSI::dump(Sortie& os)
{
  Param param("interface_CALCULSI");
  set_param(param);
  param.print(os);
}

void  interface_CALCULSI::set_param(Param& param)
{
}


