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
// File:        Sparskit.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sparskit_inclus
#define Sparskit_inclus

#include <arch.h>
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
// FORMATS/formats.f :
// csrbnd  : converts a compressed sparse row format into a banded format (linpack style)
// ORDERINGS/levset.f :
// BSF     : Breadth-First Search traversal (Cuthill-McKee ordering)
// rversp  : routine to reverse a given permutation (e.g., for RCMK)
// INFO/infofun.f :
// bandwidth :  Computes  ml     = lower_bandwidth(A)
//                       mu     = upper_bandwidth(A)
//                       iband  = max_bandwidth(A)
//                       bndav  = average_bandwidth(A)
// UNARY/unary :
// dvperm : permutes a real vector (in-place)
// ivperm : permutes an integer vector (in-place)
//

#ifdef _NO_SPARSKIT_
static inline void ERREUR(char * msg)
{
  Cerr << "SPARSKIT : " << msg << finl;
  exit();
}
inline void F77NAME(ILUT)(const int* const a, const double* const b,
                          const int* const c, const int* const d,
                          const int* const e, double* const f, double* const g,
                          int* const h, const int* const i,
                          int* const j, double* const k, int* const l,

                          int* const p)
{
  ERREUR("ILUT pas code");
}

inline void F77NAME(LUSOL) (const int* const,const double* const,
                            const double* const,const double* const,
                            const int* const,const int* const)
{
  ERREUR("LUSOL pas code");
}

inline void F77NAME(LUTSOL) (const int* const,const double* const,
                             const double* const,const double* const,
                             const int* const,const int* const)
{
  ERREUR("LUTSOL pas code");
}

inline void F77NAME(PGMRES)(const int* const a, const int* const b,
                            const double* const c, double* const d,
                            const double* const e, const double* const f,
                            const int* const g, const int* const h,
                            const double* const i, const int* const j,
                            const int* const k, const double* const l,
                            const int* const m, const int* const n, int* const o)
{
  ERREUR("PGMRES pas code");
}

inline void F77NAME(AMUX)(const int* n, const double* x, const double* y,
                          const double* a,const int* ja, const int* ia)
{
  ERREUR("AMUX pas code");
}

inline void F77NAME(ATMUX)(const int* n, const double* x, const double* y,
                           const double* a,const int* ja, const int* ia)
{
  ERREUR("ATMUX pas code");
}

inline void F77NAME(AMUDIA)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e,
                            const double* const f,
                            const double* g, const int* h, const int* i)
{
  ERREUR("AMUDIA pas code");
}

inline void F77NAME(AMUB)(const int* const a, const int* const b,
                          const int* const c, const double* const d,
                          const int* const e, const int* const f, const double* const g,
                          const int* const h, const int* const i,
                          double* const j, int* const k, int* const l,
                          const int* const m, const int* const n, int* const o)
{
  ERREUR("AMUB pas code");
}

inline void F77NAME(APLB)(const int* const a, const int* const b,
                          const int* const c,
                          const double* const d, const int* const e,const int* const f,
                          const double* const g,const int* const h,const int* const i,
                          const double* const j,const int* const k,const int* const l,
                          const int* const m,const int* const n,const int* const o)
{
  ERREUR("APLB pas code");
}

inline void F77NAME(APLB1)(const int* const a, const int* const b,
                           const int* const c,
                           const double* const d, const int* const e, const int* const f,
                           const double* const g, const int* const h, const int* const i,
                           const double* const j, const int* const k, const int* const l,
                           const int* const m, const int* const n, const int* const o)
{
  ERREUR("APLB1 pas code");
}

inline void F77NAME(APLSB)(const int* const a, const int* const b,
                           const double* const c,
                           const int* const d, const int* const e, const int* const f,
                           const double* const g, const int* const h, const int* const i,
                           const double* const j, const int* const k, const int* const l,
                           const int* const m, const int* const n, const int* const o)
{
  ERREUR("APLSB pas code");
}

inline void F77NAME(APLSB1)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e, const int* const f,
                            const double* const g, const int* const h, const int* const i,
                            const double* const j, const int* const k, const int* const l,
                            const int* const m, const int* const n, const int* const o)
{
  ERREUR("APLSB1 pas code");
}

inline void F77NAME(APMBT)(const int* const a, const int* const b,
                           const int* const c,
                           const double* const d, const int* const e,const int* const f,
                           const double* const g,const int* const h,const int* const i,
                           const double* const j,const int* const k,const int* const l,
                           const int* const m, const int* const n,const int* const o)
{
  ERREUR("APMBT pas code");
}

inline void F77NAME(APLSBT)(const int* const a, const int* const b,
                            const int* const c,
                            const double* const d, const int* const e,const int* const f,
                            const double* const g,const int* const h,const int* const i,
                            const double* const j,const int* const k,const int* const l,
                            const int* const m, const int* const n,const int* const o)
{
  ERREUR("APLSBT pas code");
}

inline void F77NAME(DIAMUA)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e, const double* const f,
                            const double* const g, const int* const h, const int* const i)
{
  ERREUR("DIAMUA pas code");
}

inline void F77NAME(APLSCA)(const int* const a, const double* const b,
                            const int* const c,
                            const int* const d, const double* const e,const int* const f)
{
  ERREUR("APLSCA pas code");
}

inline void F77NAME(APLDIA)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e, const double* const f,
                            const double* const g, const int* const h, const int* const i,
                            const int* const j)
{
  ERREUR("APLDIA pas code");
}
//inline void F77NAME(DINFO1)(const int* const a, const int* const b,
//               const double* const c,
//               const int* const d, const int* const e, const int* const f,
//               const char* const g, const char* const h, const char* const i,
//               const double* const j,const int* const k,const int* const l){
//           ERREUR("PGMRES pas code");
//               }
inline void F77NAME(BANDWIDTH)(const int* const a,const int* const b,
                               const double* const c,
                               const int* const d, const int* const e, const int* const f,
                               const double* const g)
{
  ERREUR("BANDWIDTH pas code");
}
inline void F77NAME(NONZ)(const int* const a,const int* const b,
                          const int* const c,
                          const double* const d,
                          const double* const e, const int* const f, const int* const g,
                          const int* const h, const int* const i, const int* const j,
                          const int* const k)
{
  ERREUR("NONZ pas code");
}
inline void F77NAME(DIAG_DOMI)(const int* const a,const int* const b,
                               const int* const c,
                               const double* const d, const int* const e,
                               const double* const f, const double* const g,
                               const double* const h,
                               const double* const i, const double* const j)
{
  ERREUR("DIAG_DOMI pas code");
}
inline void F77NAME(FROBNORM)(const int* const a,const int* const b,
                              const double* const c,const int* const d,
                              const double* const e,const double* const f)
{
  ERREUR("FROBNORM pas code");
}
inline void F77NAME(ANSYM)(const int* const a,const int* const b,
                           const double* const c,
                           const int* const d,const double* const e,const double* const f,
                           const int* const g,const double* const h,const int* const i,
                           const double* const j,const double* const k,const double* const l)
{
  ERREUR("ANSYM pas code");
}
inline void F77NAME(DISTAIJ)(const int* const a,const int* const b,
                             const int* const c,
                             const int* const d,const double* const e,const double* const f,
                             const double* const g)
{
  ERREUR("DISTAIJ pas code");
}
inline void F77NAME(SKYLINE)(const int* const a,const int* const b,
                             const int* const c,
                             const double* const d,
                             const int* const e,const double* const f,const int* const g)
{
  ERREUR("SKYLINE pas code");
}
inline void F77NAME(DISTDIAG)(const int* const a,const int* const b,
                              const int* const c,
                              const double* const d,const double* const e)
{
  ERREUR("DISTDIAG pas code");
}
inline void F77NAME(BANDPART)(const int* const a,const int* const b,
                              const double* const c,const double* const d,
                              const int* const e,const int* const f)
{
  ERREUR("BANDPART pas code");
}
inline void  F77NAME(N_IMP_DIAG)(const int* const a,const int* const b,
                                 const double* const c,
                                 const int* const d,const int* const e,
                                 const int* const f,const double* const g)
{
  ERREUR("N_IMP_DIAG pas code");
}
inline void F77NAME(NONZ_LUD)(const int* const a,const int* const b,
                              const double* const c,const int* const d,
                              const int* const e, const int* const f)
{
  ERREUR("NONZ_LUD pas code");
}
inline void F77NAME(AVNZ_COL)(const int* const a,const int* const b,
                              const double* const c,const double* const d,
                              const int* const e, const double* const f,
                              const double* const g)
{
  ERREUR("AVNZ_COL pas code");
}
inline void F77NAME(VBRINFO)(const int* const a, const int* const b,
                             const int* const c, const int* const d,
                             const double* const e, const int* const f,
                             const int* const g, const int* const h,
                             const int* const i)
{
  ERREUR("VBRINFO pas code");
}
inline void F77DECLARE(CSRCSC)(const int* const,const int* const,const int* const,const int* const,
                               const double*  const , const  int*  const , const  int* const,
                               double* , int* , int* )
{
  ERREUR("CSRCSC pas code");
}
inline void F77DECLARE(GETU)(const int* const, const double* const,
                             const int* const, const int* const,
                             double* ,int* ,int* )
{
  ERREUR("GETU pas code");
}

inline void F77NAME(CG)(const int* const a, const double* const b,
                        double* c, int* d,double* e,double* f )
{
  ERREUR("CG pas code");
}

inline void F77NAME(BCGSTAB)(const int* const a, const double* const b,
                             double* c, int* d,double* e,double* f )
{
  ERREUR("BCGSTAB pas code");
}

inline void F77NAME(GMRES)(const int* const a, const double* const b,
                           double* c, int* d,double* e,double* f )
{
  ERREUR("GMRES pas code");
}

inline void F77NAME(GMRESBIS)(const int* const a, const double* const b,
                              double* c, int* d,double* e,double* f )
{
  ERREUR("GMRESBIS pas code");
}

inline double F77NAME(DISTDOT)(const int* const a, const double* const b,
                               const int* const c, const double* const d,
                               const int* const e)
{
  ERREUR("DISTDOT pas code");
  return 0;
}



#else //_NO_SPARSKIT_

extern "C" {
#ifdef F77_Majuscule
  //////////////////
  // Ajout Solveur direct Cholesky
  void  F77DECLARE(DPERM) (const int*, const double*,
                           const int*, const int*,
                           double*, int*, int*,
                           const int*,  const int*,
                           const int*);

  void  F77DECLARE(DVPERM) (const int*, double*, const int*);

  void F77DECLARE(PERPHN)(const int* a ,const int* b,
                          const int* c,const int* e,
                          const int* f,const int* g,int* h,
                          int* i, int* j);
  //////////////////
  // AJOUT pour Multigrille
  //////////////////
  void F77DECLARE(CSORT) (const int*, double*, int*,
                          int*, int*, const int*);


  //////////////////
  void F77DECLARE(ILUT)(const int* const, const double* const, const int* const, const int* const,
                        const int* const, double* const, double* const, int* const, const int* const,
                        int* const, double* const, int* const,
                        int* const);

  void F77DECLARE(LUSOL) (const int* const,const double* const,
                          const double* const,const double* const,
                          const int* const,const int* const);

  void F77DECLARE(LUTSOL) (const int* const,const double* const,
                           const double* const,const double* const,
                           const int* const,const int* const);

  void F77DECLARE(PGMRES)(const int* const, const int* const, const double* const, double* const,
                          const double* const, const double* const, const int* const, const int* const,
                          const double* const, const int* const, const int* const, const double* const,
                          const int* const, const int* const, int* const);

  void F77DECLARE(AMUX)(const int* n, const double* x, const double* y,
                        const double* a,const int* ja, const int* ia);

  void F77DECLARE(ATMUX)(const int* n, const double* x, const double* y,
                         const double* a,const int* ja, const int* ia);

  void F77DECLARE(AMUDIA)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const double* const,
                          const double*, const int*, const int* );

  void F77DECLARE(AMUB)(const int* const, const int* const, const int* const, const double* const,
                        const int* const, const int* const, const double* const,
                        const int* const, const int* const, double* const, int* const, int* const,
                        const int* const, const int* const, int* const);

  void F77DECLARE(APLB)(const int* const, const int* const,const int* const,
                        const double* const, const int* const,const int* const,
                        const double* const,const int* const,const int* const,
                        const double* const,const int* const,const int* const,
                        const int* const,const int* const,const int* const);

  void F77DECLARE(APLB1)(const int* const, const int* const,const int* const,
                         const double* const, const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const int* const,const int* const,const int* const);

  void F77DECLARE(APLSB)(const int* const, const int* const, const double* const,
                         const int* const, const int* const, const int* const,
                         const double* const, const int* const, const int* const,
                         const double* const, const int* const, const int* const,
                         const int* const, const int* const, const int* const);

  void F77DECLARE(APLSB1)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const int* const,
                          const double* const, const int* const, const int* const,
                          const double* const, const int* const, const int* const,
                          const int* const, const int* const, const int* const);

  void F77DECLARE(APMBT)(const int* const, const int* const,const int* const,
                         const double* const, const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const int* const,const int* const,const int* const);

  void F77DECLARE(APLSBT)(const int* const, const int* const,const int* const,
                          const double* const, const int* const,const int* const,
                          const double* const,const int* const,const int* const,
                          const double* const,const int* const,const int* const,
                          const int* const,const int* const,const int* const);

  void F77DECLARE(DIAMUA)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const double* const,
                          const double* const, const int* const, const int* const);

  void F77DECLARE(APLSCA)(const int* const, const double* const, const int* const,
                          const int* const, const double* const,const int* const);

  void F77DECLARE(APLDIA)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const double* const,
                          const double* const, const int* const, const int* const,
                          const int* const);
  void F77DECLARE(DINFO1)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const int* const,
                          const char* const, const char* const, const char* const,
                          const double* const,const int* const,const int* const);
  void F77DECLARE(BANDWIDTH)(const int* const,const int* const, const double* const,
                             const int* const, const int* const, const int* const,
                             const double* const);
  void F77DECLARE(NONZ)(const int* const,const int* const, const int* const,
                        const double* const,
                        const double* const, const int* const, const int* const,
                        const int* const, const int* const, const int* const,
                        const int* const);
  void F77DECLARE(DIAG_DOMI)(const int* const,const int* const,const int* const,
                             const double* const, const int* const,
                             const double* const,const double* const,
                             const double* const,
                             const double* const, const double* const);
  void F77DECLARE(FROBNORM)(const int* const,const int* const,
                            const double* const,const int* const,
                            const double* const,const double* const) ;
  void F77DECLARE(ANSYM)(const int* const,const int* const,const double* const,
                         const int* const,const double* const,const double* const,
                         const int* const,const double* const,const int* const,
                         const double* const,const double* const,const double* const);
  void F77DECLARE(DISTAIJ)(const int* const,const int* const,const int* const,
                           const int* const,const double* const,const double* const,
                           const double* const);
  void F77DECLARE(SKYLINE)(const int* const,const int* const,const int* const,
                           const double* const,
                           const int* const,const double* const,const int* const);
  void F77DECLARE(DISTDIAG)(const int* const,const int* const,const int* const,
                            const double* const,const double* const);
  void F77DECLARE(BANDPART)(const int* const,const int* const,
                            const double* const,const double* const,
                            const int* const,const int* const);
  void  F77DECLARE(N_IMP_DIAG)(const int* const,const int* const,const double* const,
                               const int* const,const int* const,
                               const int* const,const double* const);
  void F77DECLARE(NONZ_LUD)(const int* const,const int* const,
                            const double* const,const int* const,
                            const int* const, const int* const);
  void F77DECLARE(AVNZ_COL)(const int* const,const int* const,
                            const double* const,const double* const,
                            const int* const, const double* const,
                            const double* const);
  void F77DECLARE(VBRINFO)(const int* const, const int* const,
                           const int* const, const int* const,
                           const double* const, const int* const,
                           const int* const, const int* const,
                           const int* const);
  void F77DECLARE(CSRCSC)(const int* const,const int* const,const int* const,const int* const,
                          const double*  const , const  int*  const , const  int* const,
                          double* , int* , int* );
  void F77DECLARE(GETU)(const int* const, const double* const,
                        const int* const, const int* const,
                        double* ,int* ,int* );

  void F77NAME(CG)(const int* const a, const double* const b,
                   double* c, int* d,double* e, double* f );

  void F77NAME(BCGSTAB)(const int* const a, const double* const b,
                        double* c, int* d,double* e, double* f );

  void F77NAME(GMRES)(const int* const a, const double* const b,
                      double* c, int* d,double* e, double* f );

  void F77NAME(GMRESBIS)(const int* const a, const double* const b,
                         double* c, int* d,double* e, double* f );



#else // F77_Majuscule
  ////////////////////
  // AJOUT pour SolvCHolesky
  //////////////////
  void  F77DECLARE(dperm) (const int*, const double*,
                           const int*, const int*,
                           double*, int*, int*,
                           const int*,  const int*,
                           const int*);

  void  F77DECLARE(dvperm) (const int*, double*, const int*);

  void F77DECLARE(perphn)(const int* a ,const int* b,
                          const int* c,const int* e,
                          const int* f ,const int* g, int* h,
                          int* i , int* j) ;
  //////////////////
  // AJOUT pour Multigrille
  //////////////////
  void F77DECLARE(csort) (const int*, double*, int*,
                          int*, int*, const int*);
  //////////////////

  void F77DECLARE(ilut)(const int* const, const double* const, const int* const, const int* const,
                        const int* const, double* const, double* const, int* const, const int* const,
                        int* const, double* const, int* const,
                        int* const);

  void F77DECLARE(lusol) (const int* const,const double* const,
                          const double* const,const double* const,
                          const int* const,const int* const);

  void F77DECLARE(lutsol) (const int* const,const double* const,
                           const double* const,const double* const,
                           const int* const,const int* const);

  void F77DECLARE(pgmres)(const int* const, const int* const, const double* const, double* const,
                          const double* const, const double* const, const int* const, const int* const,
                          const double* const, const int* const, const int* const, const double* const,
                          const int* const, const int* const, int* const);

  void F77DECLARE(amux)(const int* n, const double* x, const double* y,
                        const double* a,const int* ja, const int* ia);

  void F77DECLARE(atmux)(const int* n, const double* x, const double* y,
                         const double* a,const int* ja, const int* ia);

  void F77DECLARE(amudia)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const double* const,
                          const double*, const int*, const int* );

  void F77DECLARE(amub)(const int* const, const int* const, const int* const, const double* const,
                        const int* const, const int* const, const double* const,
                        const int* const, const int* const, double* const, int* const, int* const,
                        const int* const, const int* const, int* const);

  void F77DECLARE(aplb)(const int* const, const int* const,const int* const,
                        const double* const, const int* const,const int* const,
                        const double* const,const int* const,const int* const,
                        const double* const,const int* const,const int* const,
                        const int* const,const int* const,const int* const);

  void F77DECLARE(aplb1)(const int* const, const int* const,const int* const,
                         const double* const, const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const int* const,const int* const,const int* const);

  void F77DECLARE(aplsb)(const int* const, const int* const, const double* const,
                         const int* const, const int* const, const int* const,
                         const double* const, const int* const, const int* const,
                         const double* const, const int* const, const int* const,
                         const int* const, const int* const, const int* const);

  void F77DECLARE(aplsb1)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const int* const,
                          const double* const, const int* const, const int* const,
                          const double* const, const int* const, const int* const,
                          const int* const, const int* const, const int* const);

  void F77DECLARE(apmbt)(const int* const, const int* const,const int* const,
                         const double* const, const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const double* const,const int* const,const int* const,
                         const int* const,const int* const,const int* const);

  void F77DECLARE(aplsbt)(const int* const, const int* const,const int* const,
                          const double* const, const int* const,const int* const,
                          const double* const,const int* const,const int* const,
                          const double* const,const int* const,const int* const,
                          const int* const,const int* const,const int* const);

  void F77DECLARE(diamua)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const double* const,
                          const double* const, const int* const, const int* const);

  void F77DECLARE(aplsca)(const int* const, const double* const, const int* const,
                          const int* const, const double* const,const int* const);

  void F77DECLARE(apldia)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const double* const,
                          const double* const, const int* const, const int* const,
                          const int* const);
  void F77DECLARE(dinfo1)(const int* const, const int* const, const double* const,
                          const int* const, const int* const, const int* const,
                          const char* const, const char* const, const char* const,
                          const double* const,const int* const,const int* const);

  void F77DECLARE(bandwidth)(const int* const,const int* const, const double* const,
                             const int* const, const int* const, const int* const,
                             const double* const);
  void F77DECLARE(nonz)(const int* const,const int* const, const int* const,
                        const double* const,
                        const double* const, const int* const, const int* const,
                        const int* const, const int* const, const int* const,
                        const int* const);
  void F77DECLARE(diag_domi)(const int* const,const int* const,const int* const,
                             const double* const, const int* const,
                             const double* const,const double* const,
                             const double* const,
                             const double* const, const double* const);
  void F77DECLARE(frobnorm)(const int* const,const int* const,
                            const double* const,const int* const,
                            const double* const,const double* const) ;
  void F77DECLARE(ansym)(const int* const,const int* const,const double* const,
                         const int* const,const double* const,const double* const,
                         const int* const,const double* const,const int* const,
                         const double* const,const double* const,const double* const);
  void F77DECLARE(distaij)(const int* const,const int* const,const int* const,
                           const int* const,const double* const,const double* const,
                           const double* const);
  void F77DECLARE(skyline)(const int* const,const int* const,const int* const,
                           const double* const,
                           const int* const,const double* const,const int* const);
  void F77DECLARE(distdiag)(const int* const,const int* const,const int* const,
                            const double* const,const double* const);
  void F77DECLARE(bandpart)(const int* const,const int* const,
                            const double* const,const double* const,
                            const int* const,const int* const);
  void  F77DECLARE(n_imp_diag)(const int* const,const int* const,const double* const,
                               const int* const,const int* const,
                               const int* const,const double* const);
  void F77DECLARE(nonz_lud)(const int* const,const int* const,
                            const double* const,const int* const,
                            const int* const, const int* const);
  void F77DECLARE(avnz_col)(const int* const,const int* const,
                            const double* const,const double* const,
                            const int* const, const double* const,
                            const double* const);
  void F77DECLARE(vbrinfo)(const int* const, const int* const,
                           const int* const, const int* const,
                           const double* const, const int* const,
                           const int* const, const int* const,
                           const int* const);
  void F77DECLARE(csrcsc)(const int* const,const int* const,const int* const,const int* const,
                          const double*  const , const  int*  const , const  int* const,
                          double* , int* , int* );
  void F77DECLARE(getu)(const int* const, const double* const,
                        const int* const, const int* const,
                        double* ,int* ,int* );


  void F77NAME(cg)(const int* const a, const double* const b,
                   double* c, int* d,double* e, double* f );

  void F77NAME(bcgstab)(const int* const a, const double* const b,
                        double* c, int* d,double* e, double* f );

  void F77NAME(gmres)(const int* const a, const double* const b,
                      double* c, int* d,double* e, double* f );

  void F77NAME(gmresbis)(const int* const a, const double* const b,
                         double* c, int* d,double* e, double* f );

  double F77NAME(distdot)(const integer* const a, const double* const b,
                          const integer* const c, const double* const d,
                          const integer* const e);

#endif //F77_Majuscule

}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
#ifndef F77_Majuscule
inline void F77NAME(ILUT)(const int* const a, const double* const b,
                          const int* const c, const int* const d,
                          const int* const e, double* const f, double* const g,
                          int* const h, const int* const i,
                          int* const j, double* const k, int* const l,

                          int* const p)
{
  F77NAME(ilut)(a,b,c,d,e,f,g,h,i,j,k,l,p);
}

inline void F77NAME(LUSOL) (const int* const a, const double* const b,
                            const double* const c, const double* const d,
                            const int* const e, const int* const f)
{
  F77NAME(lusol)(a,b,c,d,e,f);
}

inline void F77NAME(LUTSOL) (const int* const a, const double* const b,
                             const double* const c, const double* const d,
                             const int* const e, const int* const f)
{
  F77NAME(lutsol)(a,b,c,d,e,f);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(PGMRES)(const int* const a, const int* const b,
                            const double* const c, double* const d,
                            const double* const e, const double* const f,
                            const int* const g, const int* const h,
                            const double* const i, const int* const j,
                            const int* const k, const double* const l,
                            const int* const m, const int* const n, int* const o)
{
  F77NAME(pgmres)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(AMUX)(const int* n, const double* x, const double* y,
                          const double* a,const int* ja, const int* ia)
{
  F77NAME(amux)(n,x,y,a,ja,ia);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(ATMUX)(const int* n, const double* x, const double* y,
                           const double* a,const int* ja, const int* ia)
{
  F77NAME(atmux)(n,x,y,a,ja,ia);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(AMUDIA)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e,
                            const double* const f,
                            const double* g, const int* h, const int* i)
{
  F77NAME(amudia)(a,b,c,d,e,f,g,h,i);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(AMUB)(const int* const a, const int* const b,
                          const int* const c, const double* const d,
                          const int* const e, const int* const f, const double* const g,
                          const int* const h, const int* const i,
                          double* const j, int* const k, int* const l,
                          const int* const m, const int* const n, int* const o)
{
  F77NAME(amub)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLB)(const int* const a, const int* const b,
                          const int* const c,
                          const double* const d, const int* const e,const int* const f,
                          const double* const g,const int* const h,const int* const i,
                          const double* const j,const int* const k,const int* const l,
                          const int* const m,const int* const n,const int* const o)
{
  F77NAME(aplb)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLB1)(const int* const a, const int* const b,
                           const int* const c,
                           const double* const d, const int* const e, const int* const f,
                           const double* const g, const int* const h, const int* const i,
                           const double* const j, const int* const k, const int* const l,
                           const int* const m, const int* const n, const int* const o)
{
  F77NAME(aplb1)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLSB)(const int* const a, const int* const b,
                           const double* const c,
                           const int* const d, const int* const e, const int* const f,
                           const double* const g, const int* const h, const int* const i,
                           const double* const j, const int* const k, const int* const l,
                           const int* const m, const int* const n, const int* const o)
{
  F77NAME(aplsb)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLSB1)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e, const int* const f,
                            const double* const g, const int* const h, const int* const i,
                            const double* const j, const int* const k, const int* const l,
                            const int* const m, const int* const n, const int* const o)
{
  F77NAME(aplsb1)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APMBT)(const int* const a, const int* const b,
                           const int* const c,
                           const double* const d, const int* const e,const int* const f,
                           const double* const g,const int* const h,const int* const i,
                           const double* const j,const int* const k,const int* const l,
                           const int* const m, const int* const n,const int* const o)
{
  F77NAME(apmbt)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLSBT)(const int* const a, const int* const b,
                            const int* const c,
                            const double* const d, const int* const e,const int* const f,
                            const double* const g,const int* const h,const int* const i,
                            const double* const j,const int* const k,const int* const l,
                            const int* const m, const int* const n,const int* const o)
{
  F77NAME(aplsbt)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(DIAMUA)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e, const double* const f,
                            const double* const g, const int* const h, const int* const i)
{
  F77NAME(diamua)(a,b,c,d,e,f,g,h,i);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLSCA)(const int* const a, const double* const b,
                            const int* const c,
                            const int* const d, const double* const e,const int* const f)
{
  F77NAME(aplsca)(a,b,c,d,e,f);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(APLDIA)(const int* const a, const int* const b,
                            const double* const c,
                            const int* const d, const int* const e, const double* const f,
                            const double* const g, const int* const h, const int* const i,
                            const int* const j)
{
  F77NAME(apldia)(a,b,c,d,e,f,g,h,i,j);
}
//inline void F77NAME(DINFO1)(const int* const a, const int* const b,
//               const double* const c,
//               const int* const d, const int* const e, const int* const f,
//               const char* const g, const char* const h, const char* const i,
//               const double* const j,const int* const k,const int* const l){
//                F77NAME(dinfo1)(a,b,c,d,e,f,g,h,i,j,k,l);
//               }
inline void F77NAME(BANDWIDTH)(const int* const a,const int* const b,
                               const double* const c,
                               const int* const d, const int* const e, const int* const f,
                               const double* const g)
{
  F77NAME(bandwidth)(a,b,c,d,e,f,g);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(NONZ)(const int* const a,const int* const b,
                          const int* const c,
                          const double* const d,
                          const double* const e, const int* const f, const int* const g,
                          const int* const h, const int* const i, const int* const j,
                          const int* const k)
{
  F77NAME(nonz)(a,b,c,d,e,f,g,h,i,j,k);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(DIAG_DOMI)(const int* const a,const int* const b,
                               const int* const c,
                               const double* const d, const int* const e,
                               const double* const f, const double* const g,
                               const double* const h,
                               const double* const i, const double* const j)
{
  F77NAME(diag_domi)(a,b,c,d,e,f,g,h,i,j);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(FROBNORM)(const int* const a,const int* const b,
                              const double* const c,const int* const d,
                              const double* const e,const double* const f)
{
  F77NAME(frobnorm)(a,b,c,d,e,f);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(ANSYM)(const int* const a,const int* const b,
                           const double* const c,
                           const int* const d,const double* const e,const double* const f,
                           const int* const g,const double* const h,const int* const i,
                           const double* const j,const double* const k,const double* const l)
{
  F77NAME(ansym)(a,b,c,d,e,f,g,h,i,j,k,l);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(DISTAIJ)(const int* const a,const int* const b,
                             const int* const c,
                             const int* const d,const double* const e,const double* const f,
                             const double* const g)
{
  F77NAME(distaij)(a,b,c,d,e,f,g);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(SKYLINE)(const int* const a,const int* const b,
                             const int* const c,
                             const double* const d,
                             const int* const e,const double* const f,const int* const g)
{
  F77NAME(skyline)(a,b,c,d,e,f,g);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(DISTDIAG)(const int* const a,const int* const b,
                              const int* const c,
                              const double* const d,const double* const e)
{
  F77NAME(distdiag)(a,b,c,d,e);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(BANDPART)(const int* const a,const int* const b,
                              const double* const c,const double* const d,
                              const int* const e,const int* const f)
{
  F77NAME(bandpart)(a,b,c,d,e,f);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void  F77NAME(N_IMP_DIAG)(const int* const a,const int* const b,
                                 const double* const c,
                                 const int* const d,const int* const e,
                                 const int* const f,const double* const g)
{
  F77NAME(n_imp_diag)(a,b,c,d,e,f,g);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(NONZ_LUD)(const int* const a,const int* const b,
                              const double* const c,const int* const d,
                              const int* const e, const int* const f)
{
  F77NAME(nonz_lud)(a,b,c,d,e,f);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(AVNZ_COL)(const int* const a,const int* const b,
                              const double* const c,const double* const d,
                              const int* const e, const double* const f,
                              const double* const g)
{
  F77NAME(avnz_col)(a,b,c,d,e,f,g);
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(VBRINFO)(const int* const a, const int* const b,
                             const int* const c, const int* const d,
                             const double* const e, const int* const f,
                             const int* const g, const int* const h,
                             const int* const i)
{
  F77NAME(vbrinfo)(a,b,c,d,e,f,g,h,i);
}
inline void F77DECLARE(CSRCSC)(const int* const a,const int* const b,
                               const int* const c,const int* const d,
                               const double* const e, const int* const f,
                               const int* const g, double* h,
                               int* i, int* j)
{
  F77NAME(csrcsc)(a,b,c,d,e,f,g,h,i,j);
}

inline void F77DECLARE(GETU)(const int* const a, const double* const b,
                             const int* const c, const int* const d,
                             double* e,int* f,int* g)
{
  F77NAME(getu)(a,b,c,d,e,f,g);
}


inline void F77NAME(CG)(const int* const a, const double* const b,
                        double* c,int* d,double* e,double* f )
{
  F77NAME(cg)(a,b,c,d,e,f);
}

inline void F77NAME(BCGSTAB)(const int* const a, const double* const b,
                             double* c,int* d,double* e,double* f )
{
  F77NAME(bcgstab)(a,b,c,d,e,f);
}

inline void F77NAME(GMRES)(const int* const a, const double* const b,
                           double* c,int* d,double* e,double* f )
{
  F77NAME(gmres)(a,b,c,d,e,f);
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void F77NAME(GMRESBIS)(const int* const a, const double* const b,
                              double* c,int* d,double* e,double* f )
{
  F77NAME(gmresbis)(a,b,c,d,e,f);
}







//////////////////
// Ajout Solveur direct Cholesky
inline void  F77DECLARE(DPERM) (const int* a, const double* b,
                                const int* c, const int* d,
                                double* e, int* f, int* g,
                                const int* h,  const int* i,
                                const int* j)
{
  F77NAME(dperm)(a,b,c,d,e,f,g,h,i,j);
}

inline void  F77DECLARE(DVPERM) (const int* a, double* b, const int* c)
{
  F77NAME(dvperm)(a,b,c);
}

inline void F77DECLARE(PERPHN)(const int* a ,const int* b,
                               const int* c, const int* e,
                               const int* f, const int* g, int* h,
                               int* i, int* j)
{
  F77NAME(perphn)(a,b,c,e,f,g,h,i,j);
}

//////////////////
// AJOUT pour Multigrille
//////////////////
inline void F77DECLARE(CSORT) (const int* a, double* b, int* c,
                               int* d, int* e, const int* f)
{
  F77NAME(csort)(a,b,c,d,e,f);
}

#endif //F77_Majuscule
#endif //_NO_SPARSKIT_
#endif //Sparskit_inclus



