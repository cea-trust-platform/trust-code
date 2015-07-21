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
// File:        interface_CALCULBIJ.h
// Directory:   $TRUST_ROOT/src/EF/fortran
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#ifndef interface_CALCULBIJ_included
#define interface_CALCULBIJ_included

#include <assert.h>
class ArrOfInt;
class ArrOfDouble;
class Nom;
class Sortie;
class Param;

class interface_CALCULBIJ
{
public:
  void initialiser_les_commons();
  void init();
  void init_sans_initialiser_les_commons();
  void check_common();
  void compare_common();
  void set_param(Param&);
  void dump(Sortie&);
  void init_from_common();
  void init_from_file(const Nom& );
  void Compute(const int& nbnn, const int& nbsomtot, const ArrOfDouble& xl, const ArrOfInt& num, ArrOfDouble& bij, const ArrOfDouble& poro, const int& ip, const int& npgau, const ArrOfDouble& xgau, const ArrOfDouble& frgau, const ArrOfDouble& dfrgau, const ArrOfDouble& poigau, ArrOfDouble& detj, ArrOfDouble& ajm1, ArrOfDouble& aj, ArrOfDouble& df, double& volume, ArrOfDouble& volume_sommet, ArrOfDouble& iphi) const;

};
#endif
