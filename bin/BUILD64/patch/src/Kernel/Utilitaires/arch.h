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
// File:        arch.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

//
// .DESCRIPTION
//  Gestion des dependances machines pour l'appel a FORTRAN
//  La macro F77DECLARE(TOTO) permet de declarer une routine FORTRAN.
//  La macro F77NAME(TOTO) permet d'appeler une routine FORTRAN.
//  Exemple :
//  toto.f :
//         SUBROUTINE TOTO(DOUBLE X)
//         X=2.0*X
//         RETURN
//         END
//
//  appel_toto.cpp
//  void F77DECLARE(TOTO) (const double* );
//  void main()
//  {
//     double x;
//     F77NAME(TOTO)(x);
//  }

#ifndef _ARCH_H_
#define _ARCH_H_

#include <generique.h>
// PL: stdlib.h necessaire pour exit() partout dans le code excepte g++>3.x?
#include <stdlib.h>

#ifdef _COMPILE_AVEC_GCC_3
#define IOS_OPEN_MODE std::_Ios_Openmode
#define IOS_FORMAT std::_Ios_Fmtflags
#else
#ifdef _COMPILE_AVEC_INTEL
#define IOS_OPEN_MODE std::ios_base::openmode
#define IOS_FORMAT std::ios_base::fmtflags
#else
#ifdef _COMPILE_AVEC_CLANG
#define IOS_OPEN_MODE std::ios_base::openmode
#define IOS_FORMAT std::ios_base::fmtflags
#else
#define IOS_OPEN_MODE long
#define IOS_FORMAT long
#endif
#endif
#endif

#ifdef MICROSOFT
void srand48(long) ;
double drand48();
double atanh(const double&);
#include <iso646.h>
#define NO_RANDOM
#define F77DECLARE(x) F77NAME(x)
#else
#define F77DECLARE(x) F77NAME(x)
#endif

#ifndef F77CALLWITHOUT_
#define F77NAME(x) name2(x,_)
#else
#define F77NAME(x) x
#endif

// Pour eviter problemes de portage 32/64bits
// avec les appels Fortran a partir du C++
// on definit les integer comme des long (4 octets)
// et non plus des long long (4 octets sur 32bits ou 8 octets sur 64bits)
// typedef mieux que #define pour definir un type
typedef long entier;
typedef long integer;
typedef int True_int;
#define INT_is_64_
extern char* pwd();

#endif // _ARCH_H_

