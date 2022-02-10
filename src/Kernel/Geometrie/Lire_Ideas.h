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
// File:        Lire_Ideas.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Lire_Ideas_included
#define Lire_Ideas_included

#include <Interprete_geometrique_base.h>

extern "C"
{
#ifdef F77_Majuscule
  //
  // Subroutine LIREPREM
  //
  void F77DECLARE(LIREPREM)(const char* const,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*);
  //
  // Subroutine LIREIDEAS
  //
  void F77DECLARE(LIREIDEAS)(const char* const,
                             int*,
                             double*,
                             double*,
                             double*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*);
#else
  //
  // Subroutine lireprem
  //
  void F77DECLARE(lireprem)(const char* const,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*,
                            int*);
  //
  // Subroutine lireideas
  //
  void F77DECLARE(lireideas)(const char* const,
                             int*,
                             double*,
                             double*,
                             double*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*,
                             int*);
#endif
}
#ifndef F77_Majuscule
inline void F77DECLARE(LIREPREM)(const char* const name,
                                 int* i2,
                                 int* i3,
                                 int* i4,
                                 int* i5,
                                 int* i6,
                                 int* i7,
                                 int* i8,
                                 int* i9)
{
  F77NAME(lireprem)(name, i2, i3, i4, i5, i6 , i7, i8, i9);
}
//
#endif
//
#ifndef F77_Majuscule
inline void F77DECLARE(LIREIDEAS)( const char* const name,
                                   int* i1,
                                   double* x,
                                   double* y,
                                   double* z,
                                   int* i4,
                                   int* i5,
                                   int* i6,
                                   int* i7,
                                   int* i8,
                                   int* i9,
                                   int* i10,
                                   int* i11,
                                   int* i12,
                                   int* i13,
                                   int* i14)
{

  F77NAME(lireideas)(name, i1, x, y, z, i4, i5, i6, i7, i8,
                     i9, i10, i11, i12, i13, i14);
}
//
#endif


class Lire_Ideas : public Interprete_geometrique_base
{

  Declare_instanciable(Lire_Ideas);

public :

  Entree& interpreter_(Entree&) override;

};


#endif

