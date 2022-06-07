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


#ifndef _BLAS2_H_
#define _BLAS2_H_

#include <arch.h>
#include <generic.h>

extern "C"
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
{
  void F77DECLARE(DGEMV)(char* trans, integer* M, integer* N, double* alpha,
                         double* const A, integer* lda, const double* dx,
                         integer* incx, double* beta, double* dy, integer* incy);

  void F77DECLARE(DGBMV)(char* trans, integer* M, integer* N, integer* kl, integer* ku,
                         double* alpha, double* const A, integer* lda,
                         double* const dx, integer* incx, double* beta,
                         double* dy, integer* incy);

  void F77DECLARE(DSYMV)(char* uplo, integer* N, double* alpha, const double* A,
                         integer* lda, const double* dx, integer* incx, double* beta,
                         double* dy, integer* incy);

  void F77DECLARE(DSBMV)(char* uplo, integer* N, integer* k, double* alpha,
                         const double* A, integer* lda, const double* dx,
                         integer* incx, double* beta, double* dy, integer* incy);

  void F77DECLARE(DSPMV)(char* uplo, integer* N, double* alpha, double* AP,
                         double* dx, integer* incx, double* beta, double* dy,
                         integer* incy);

  void F77DECLARE(DTRMV)(char* uplo, char* trans, char* diag, integer* N,
                         const double* A, integer* lda, const double* dx,
                         integer* incx);

  // currently not implemented.
  //F77NAME(DTBMV) ( UPLO, TRANS, DIAG, N, K, A, LDA, dx, INCX )

  void F77DECLARE(DTRSV)(char* uplo, char* trans, char* diag, integer* N,
                         double* A, integer* lda, double* dx, integer* incx);

  // currently not implemented.
  //F77NAME(DTBSV) ( UPLO, TRANS, DIAG, N, K, A, LDA, X, INCX )

  // currently not implemented.
  //F77NAME(DTPSV) ( UPLO, TRANS, DIAG, N, AP, X, INCX )

  void F77DECLARE(DGER)(integer* M, integer* N, double* alpha, double* dx,
                        integer* incx, double* dy, integer* incy, double* A,
                        integer* lda);

  void F77DECLARE(DSYR)(char* uplo, integer* N, double* alpha, double* dx,
                        integer* incx, double* A, integer* lda);

  void F77DECLARE(DSPR)(char* uplo, integer* N, double* alpha, double* dx,
                        integer* incx, double* AP);

  void F77DECLARE(DSYR2)(char* uplo, integer* N, double* alpha, double* dx,
                         integer* incx, double* dy, integer* incy, double* A,
                         integer* lda);

  void F77DECLARE(DSPR2)(char* uplo, integer* N, double* alpha, double* dx,
                         integer* incx, double* dy, integer* incy, double* AP);

}

#endif
// _BLAS2_H_
