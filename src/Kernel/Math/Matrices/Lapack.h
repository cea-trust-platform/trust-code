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
// File:        Lapack.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Lapack_inclus
#define Lapack_inclus

#include <arch.h>
//
// .DESCRIPTION
// Encapsulation de Lapack:
//

extern "C"
{

#ifdef F77_Majuscule

  void F77DECLARE(DPBTRF)(const char* const, const int* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(DPBTRS)(const char* const, const int* const, const int* const, const int* const, const double* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(DGBSV)(int*, int*,int*, int*, double*,int*, int*, double*, int*, int*);

  void F77DECLARE(DGBTRS)(const char* const, const int* const, const int* const, const int* const, const int* const, const double* const, const int* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(DGBTRF)(const int*, const int*, const int*, const int*, double*, const int*, int*, int*);

  void F77DECLARE(DTRTRS)(const char* const, const char* const, const char* const, const int* const, const int* const, const double* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(DTRTRS)(const char* const, const char* const, const char* const, const int* const, const int* const, const double* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(SSBMV)(const char* const, const int* const, const int* const, const double* const, const double* const, const int* const, const double* const, const int* const, const double* const, const double* const, const int* const);

  void F77DECLARE(DLASWP)(const int* const, double*, const int* const, const int* const, const int* const, const int* const, const int* const);

  void F77DECLARE(DGBTF2)(const int* const, const int* const,const int* const,const int* const, double* , const int* const, int* , int*);


  void F77DECLARE(DPBCON)(const char* const, const int* const, const int* const, const double* const, const int* const, const double* const, const double* const, const double* const, const int* const, const int* const);

  // MODIF ELI LAUCOIN (26/11/2007) : j'ajoute les routines d'interpolation SVD
  void F77DECLARE(DGESVD)(const char*   const, // JOBU
                          const char*   const, // JOBVT
                          const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // S
                          const double* const, // U
                          const int*    const, // LDU
                          const double* const, // VT
                          const int*    const, // LDVT
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(DGESSD)(const char*   const, // JOBZ
                          const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // S
                          const double* const, // U
                          const int*    const, // LDU
                          const double* const, // VT
                          const int*    const, // LDVT
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const, // IWORK
                          const int*    const  // INFO
                         );

  // FIN MODIF ELI LAUCOIN (26/11/2007)

  // MODIF ELI LAUCOIN (19/03/2008) : j'ajoute les routines suivantes pour la resolution LU
  void F77DECLARE(DGETRF)(const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const, // IPIV
                          const int*    const // INFO
                         );

  void F77DECLARE(DGETRI)(const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const, // IPIV
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const // INFO
                         );
  // FIN MODIF ELI LAUCOIN (19/03/2008)

  void F77DECLARE(DSYEV)(const char*   const, // JOBZ
                         const char*   const, // UPLO
                         const int*    const, // N
                         const double* const, // A
                         const int*    const, // LDA
                         const double* const, // W
                         const double* const, // WORK
                         const int*    const, // LWORK
                         const int*    const  // INFO
                        );

  void F77DECLARE(DSYEVD)(const char*   const, // JOBZ
                          const char*   const, // UPLO
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // W
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const, // IWORK
                          const int*    const, // LIWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(DGELS)(const char*   const, // TRANS
                         const int*    const, // M
                         const int*    const, // N
                         const int*    const, // NRHS
                         const double* const, // A
                         const int*    const, // LDA
                         const double* const, // B
                         const int*    const, // LDB
                         const double* const, // WORK
                         const int*    const, // LWORK
                         const int*    const  // INFO
                        );

  void F77DECLARE(DGELSY)(const int*    const, // M
                          const int*    const, // N
                          const int*    const, // NRHS
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // B
                          const int*    const, // LDB
                          const int*    const, // JPVT
                          const double* const, // RCOND
                          const int*    const, // RANK
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(DGGLSE)(const int*    const, // M
                          const int*    const, // N
                          const int*    const, // P
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // B
                          const int*    const, // LDB
                          const double* const, // C
                          const double* const, // D
                          const double* const, // X
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(DGEQRF)(const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // TAU
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(DORGQR)(const int*    const, // M
                          const int*    const, // N
                          const int*    const, // K
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // TAU
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(DPOTRF)(const char*   const, // UPLO
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const  // INFO
                         );

  void F77DECLARE(DPOTRI)(const char*   const, // UPLO
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const  // INFO
                         );

  void F77DECLARE(DPPSV)(const char*   const, // UPLO
                         const int*    const, // N
                         const int*    const, // NRHS
                         const double* const, // AP
                         const double* const, // B
                         const int*    const, // LDB
                         const int*    const  // INFO
                        );

#else

  void F77DECLARE(dpbtrf)(const char* const, const int* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(dpbtrs)(const char* const, const int* const, const int* const, const int* const, const double* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(dgbsv)(int*, int*,int*, int*, double*,int*, int*, double*, int*, int*);

  void F77DECLARE(dgbtrs)(const char* const, const int* const, const int* const, const int* const, const int* const, const double* const, const int* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(dgbtrf)(const int*, const int*, const int*, const int*, double*, const int*, int*, int*);

  void F77DECLARE(dtrtrs)(const char* const, const char* const, const char* const, const int* const, const int* const, const double* const, const int* const, const double* const, const int* const, const int* const);

  void F77DECLARE(ssbmv)(const char* const, const int* const, const int* const, const double* const, const double* const, const int* const, const double* const, const int* const, const double* const, const double* const, const int* const);

  void F77DECLARE(dsbmv)(const char* const, const int* const, const int* const, const double* const, const double* const, const int* const, const double* const, const int* const, const double* const, const double* const, const int* const);

  void F77DECLARE(ilaenv)(const int* const, const char* const, const char* const, const int* const, const int* const, const int* const,const int* const);



  void F77DECLARE(dpbcon)(const char* const, const int* const, const int* const, const double* const, const int* const, const double* const, const double* const, const double* const, const int* const, const int* const);
  void F77DECLARE(dlaswp)(const int* const, double*, const int* const, const int* const, const int* const, const int* const, const int* const);

  void F77DECLARE(dgbtf2)(const int* const, const int* const,const int* const,const int* const, double* , const int* const, int* , int*);

  // MODIF ELI LAUCOIN (26/11/2007) :  j'ajoute les routines d'interpolation SVD

  void F77DECLARE(dgesdd)(const char*   const,
                          const int*    const,
                          const int*    const,
                          const double* const,
                          const int*    const,
                          const double* const,
                          const double* const,
                          const int*    const,
                          const double* const,
                          const int*    const,
                          const double* const,
                          const int*    const,
                          const int*    const,
                          const int*    const );

  void F77DECLARE(dgesvd)(const char*   const, // JOBU
                          const char*   const, // JOBVT
                          const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // S
                          const double* const, // U
                          const int*    const, // LDU
                          const double* const, // VT
                          const int*    const, // LDVT
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  // FIN MODIF ELI LAUCOIN (26/11/2007)

  // MODIF ELI LAUCOIN (19/03/2008) : j'ajoute les routines suivantes pour la resolution LU
  void F77DECLARE(dgetrf)(const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const, // IPIV
                          const int*    const  // INFO
                         );

  void F77DECLARE(dgetri)(const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const, // IPIV
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );
  // FIN MODIF ELI LAUCOIN (19/03/2008)

  void F77DECLARE(dsyev)(const char*   const, // JOBZ
                         const char*   const, // UPLO
                         const int*    const, // N
                         const double* const, // A
                         const int*    const, // LDA
                         const double* const, // W
                         const double* const, // WORK
                         const int*    const, // LWORK
                         const int*    const  // INFO
                        );


  void F77DECLARE(dsyevd)(const char*   const, // JOBZ
                          const char*   const, // UPLO
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // W
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const, // IWORK
                          const int*    const, // LIWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(dgels)(const char*   const, // TRANS
                         const int*    const, // M
                         const int*    const, // N
                         const int*    const, // NRHS
                         const double* const, // A
                         const int*    const, // LDA
                         const double* const, // B
                         const int*    const, // LDB
                         const double* const, // WORK
                         const int*    const, // LWORK
                         const int*    const  // INFO
                        );

  void F77DECLARE(dgelsy)(const int*    const, // M
                          const int*    const, // N
                          const int*    const, // NRHS
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // B
                          const int*    const, // LDB
                          const int*    const, // JPVT
                          const double* const, // RCOND
                          const int*    const, // RANK
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(dgglse)(const int*    const, // M
                          const int*    const, // N
                          const int*    const, // P
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // B
                          const int*    const, // LDB
                          const double* const, // C
                          const double* const, // D
                          const double* const, // X
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(dgeqrf)(const int*    const, // M
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // TAU
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(dorgqr)(const int*    const, // M
                          const int*    const, // N
                          const int*    const, // K
                          const double* const, // A
                          const int*    const, // LDA
                          const double* const, // TAU
                          const double* const, // WORK
                          const int*    const, // LWORK
                          const int*    const  // INFO
                         );

  void F77DECLARE(dpotrf)(const char*   const, // UPLO
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const  // INFO
                         );

  void F77DECLARE(dpotri)(const char*   const, // UPLO
                          const int*    const, // N
                          const double* const, // A
                          const int*    const, // LDA
                          const int*    const  // INFO
                         );

  void F77DECLARE(dppsv)(const char*   const, // UPLO
                         const int*    const, // N
                         const int*    const, // NRHS
                         const double* const, // AP
                         const double* const, // B
                         const int*    const, // LDB
                         const int*    const  // INFO
                        );

#endif
}

#ifndef F77_Majuscule

inline void F77NAME(DPBTRF)(const char* const a, const int* const b, const int* const c, const double* const d, const int* const e, const int* const f)
{
  F77NAME(dpbtrf)(a,b,c,d,e,f);
}

inline void F77NAME(DPBTRS)(const char* const a, const int* const b, const int* const c, const int* const d, const double* const e, const int* const f, const double* const g, const int* const h, const int* const i)
{
  F77NAME(dpbtrs)(a,b,c,d,e,f,g,h,i);
}

inline void F77NAME(DGBSV)(int* a, int* b,int* c, int* d, double* e,int* f, int* g, double* h, int* i, int* j)
{
  F77NAME(dgbsv)(a,b,c,d,e,f,g,h,i,j);
}

inline void F77NAME(DGBTRS)(const char* const a, const int* const b, const int* const c, const int* const d, const int* const e, const double* const f, const int* const g, const int* const h, const double* const i, const int* const j, const int* const k)
{
  F77NAME(dgbtrs)(a,b,c,d,e,f,g,h,i,j,k);
}

inline void F77NAME(DGBTRF)(const int* a, const int* b, const int* c, const int* d, double* e, const int* f, int* g, int* h)
{
  F77NAME(dgbtrf)(a,b,c,d,e,f,g,h);
}

inline void F77NAME(DTRTRS)(const char* const a, const char* const b, const char* const c, const int* const d, const int* const e,
                            const double* const f, const int* const g, const double* const h, const int* const i, const int* const j)
{
  F77NAME(dtrtrs)(a,b,c,d,e,f,g,h,i,j);
}

inline void F77NAME(SSBMV)(const char* const a, const int* const b, const int* const c, const double* const d, const double* const e, const int* const f, const double* const g, const int* const h, const double* const i, const double* const j, const int* const k)
{
  F77NAME(ssbmv)(a,b,c,d,e,f,g,h,i,j,k);
}

inline void F77NAME(DPBCON)(const char* const a, const int* const b, const int* const c, const double* const d, const int* const e, const double* const f, const double* const g, const double* const h, const int* const i, const int* const j)
{
  F77NAME(dpbcon)(a,b,c,d,e,f,g,h,i,j);
}

inline void F77NAME(ILAENV)(const int* const a, const char* const b, const char* const c, const int* const d, const int* const e, const int* const f,const int* const g)
{
  F77NAME(ilaenv)(a,b,c,d,e,f,g);
}

inline void F77NAME(DLASWP)(const int* const a, double* b, const int* const c, const int* const d, const int* const e, const int* const f, const int* const g)
{
  F77NAME(dlaswp)(a,b,c,d,e,f,g);
}


inline void F77NAME(DGBTF2)(const int* const a, const int* const b,const int* const c,const int* const d, double* e, const int* const f, int* g, int* h)
{
  F77NAME(dgbtf2)(a,b,c,d,e,f,g,h);
}

// MODIF ELI LAUCOIN (26/11/2007) : j'ajoute les routines d'interpolation SVD

inline void F77NAME(DGESDD)(const char*   const JOBZ,
                            const int*    const M,
                            const int*    const N,
                            const double* const A,
                            const int*    const LDA,
                            const double* const S,
                            const double* const U,
                            const int*    const LDU,
                            const double* const VT,
                            const int*    const LDVT,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const IWORK,
                            const int*    const INFO
                           )
{
  F77NAME(dgesdd)( JOBZ, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK,LWORK, IWORK, INFO );
}

inline void F77NAME(DGESVD)(const char*   const JOBU,
                            const char*   const JOBVT,
                            const int*    const M,
                            const int*    const N,
                            const double* const A,
                            const int*    const LDA,
                            const double* const S,
                            const double* const U,
                            const int*    const LDU,
                            const double* const VT,
                            const int*    const LDVT,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const INFO )
{
  F77NAME(dgesvd)(JOBU,JOBVT,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,INFO);
}

// FIN MODIF ELI LAUCOIN (26/11/2007)

// MODIF ELI LAUCOIN (19/03/2008) : j'ajoute les routines suivantes pour la resolution LU
inline void F77NAME(DGETRF)(const int*    const M,
                            const int*    const N,
                            const double* const A,
                            const int*    const LDA,
                            const int*    const IPIV,
                            const int*    const INFO)
{
  F77NAME(dgetrf)(M,N,A,LDA,IPIV,INFO);
}

inline void F77NAME(DGETRI)(const int*    const N,
                            const double* const A ,
                            const int*    const LDA,
                            const int*    const IPIV,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const INFO
                           )
{
  F77NAME(dgetri)(N,A,LDA,IPIV,WORK,LWORK,INFO);
}
// FIN MODIF ELI LAUCOIN (19/03/2008)

inline void F77NAME(DSYEV)(const char*   const JOBZ,
                           const char*   const UPLO,
                           const int*    const N,
                           const double* const A,
                           const int*    const LDA,
                           const double* const W,
                           const double* const WORK,
                           const int*    const LWORK,
                           const int*    const INFO
                          )
{
  F77NAME(dsyev)(JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, INFO);
}

inline void F77NAME(DSYEVD)(const char*   const JOBZ,
                            const char*   const UPLO,
                            const int*    const N,
                            const double* const A,
                            const int*    const LDA,
                            const double* const W,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const IWORK,
                            const int*    const LIWORK,
                            const int*    const INFO
                           )
{
  F77NAME(dsyevd)(JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, IWORK, LIWORK, INFO);
}

inline void F77NAME(DGELS)(const char*   const TRANS,
                           const int*    const M,
                           const int*    const N,
                           const int*    const NRHS,
                           const double* const A,
                           const int*    const LDA,
                           const double* const B,
                           const int*    const LDB,
                           const double* const WORK,
                           const int*    const LWORK,
                           const int*    const INFO
                          )
{
  F77NAME(dgels)(TRANS, M, N, NRHS, A, LDA, B, LDB, WORK, LWORK, INFO);
}

inline void F77NAME(DGELSY)(const int*    const M,
                            const int*    const N,
                            const int*    const NRHS,
                            const double* const A,
                            const int*    const LDA,
                            const double* const B,
                            const int*    const LDB,
                            const int*    const JPVT,
                            const double* const RCOND,
                            const int*    const RANK,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const INFO
                           )
{
  F77NAME(dgelsy)(M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK, WORK, LWORK, INFO);
}

inline void F77DECLARE(DGGLSE)(const int*    const M,
                               const int*    const N,
                               const int*    const P,
                               const double* const A,
                               const int*    const LDA,
                               const double* const B,
                               const int*    const LDB,
                               const double* const C,
                               const double* const D,
                               const double* const X,
                               const double* const WORK,
                               const int*    const LWORK,
                               const int*    const INFO
                              )
{
  F77NAME(dgglse)(M, N, P, A, LDA, B, LDB, C, D, X, WORK, LWORK, INFO);
}

inline void F77NAME(DGEQRF)(const int*    const M,
                            const int*    const N,
                            const double* const A,
                            const int*    const LDA,
                            const double* const TAU,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const INFO
                           )
{
  F77NAME(dgeqrf)(M, N, A, LDA, TAU, WORK, LWORK, INFO);
}

inline void F77NAME(DORGQR)(const int*    const M,
                            const int*    const N,
                            const int*    const K,
                            const double* const A,
                            const int*    const LDA,
                            const double* const TAU,
                            const double* const WORK,
                            const int*    const LWORK,
                            const int*    const INFO
                           )
{
  F77NAME(dorgqr)(M, N, K, A, LDA, TAU, WORK, LWORK, INFO);
}

inline  void F77NAME(DPOTRF)(const char*   const UPLO,
                             const int*    const N,
                             const double* const A,
                             const int*    const LDA,
                             const int*    const INFO
                            )
{
  F77NAME(dpotrf)(UPLO, N, A, LDA, INFO);
}

inline  void F77DECLARE(DPOTRI)(const char*   const UPLO,
                                const int*    const N,
                                const double* const A,
                                const int*    const LDA,
                                const int*    const INFO
                               )
{
  F77NAME(dpotri)(UPLO, N, A, LDA, INFO);
}

inline void F77DECLARE(DPPSV)(const char*   const UPLO,
                              const int*    const N,
                              const int*    const NRHS,
                              const double* const AP,
                              const double* const B,
                              const int*    const LDB,
                              const int*    const INFO
                             )
{
  F77NAME(dppsv)(UPLO, N, NRHS, AP, B, LDB, INFO);
}
#endif

#endif
