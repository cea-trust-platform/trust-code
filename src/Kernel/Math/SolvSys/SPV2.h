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
// File:        SPV2.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////
#ifndef SPV2_H
#define SPV2_H
extern "C" {
#ifdef F77_Majuscule
  void F77DECLARE(ILUTV2)(const int* n, const double* a, const int* ja,  const int* ia,const int* lfil,const double* droptol, const double* alu, const int* jlu, const int* ju, const int* iwk,const double* w,const int* jw, const int* ierr);

#else
  void F77DECLARE(ilutv2)(const int* n, const double* a, const int* ja,  const int* ia,const int* lfil,const double* droptol, const double* alu, const int* jlu, const int* ju, const int* iwk,const double* w,const int* jw, const int* ierr);

#endif
}
#ifndef F77_Majuscule
inline void F77NAME(ILUTV2)(const int* n, const double* a, const int* ja,  const int* ia,const int* lfil,const double* droptol, const double* alu, const int* jlu,const int* ju,   const int* iwk,const double* w,const int* jw, const int* ierr)
{
  F77NAME(ilutv2)( n,a,ja,ia,lfil,droptol,alu,jlu,ju,iwk,w,jw,  ierr)
  ;
}

#endif

//MILU0
extern "C" {
#ifdef F77_Majuscule
  void F77DECLARE(MILU0V2)(const int* n, const double* a, const int* ja,  const int* ia, const double* alu, const int* jlu, const int* ju, const int* jw, const int* ierr);

#else
  void F77DECLARE(milu0v2)(const int* n, const double* a, const int* ja,  const int* ia, const double* alu, const int* jlu, const int* ju, const int* jw, const int* ierr);

#endif
}
#ifndef F77_Majuscule
inline void F77NAME(MILU0V2)(const int* n, const double* a, const int* ja,  const int* ia, const double* alu, const int* jlu, const int* ju, const int* jw, const int* ierr)
{
  F77NAME(milu0v2)( n,a,ja,ia,alu,jlu,ju,jw,  ierr)
  ;
}

#endif

//ILU0
extern "C" {
#ifdef F77_Majuscule
  void F77DECLARE(ILU0V2)(const int* n, const double* a, const int* ja,  const int* ia, const double* alu, const int* jlu, const int* ju, const int* jw, const int* ierr);

#else
  void F77DECLARE(ilu0v2)(const int* n, const double* a, const int* ja,  const int* ia, const double* alu, const int* jlu, const int* ju, const int* jw, const int* ierr);

#endif
}
#ifndef F77_Majuscule
inline void F77NAME(ILU0V2)(const int* n, const double* a, const int* ja,  const int* ia, const double* alu, const int* jlu, const int* ju, const int* jw, const int* ierr)
{
  F77NAME(ilu0v2)( n,a,ja,ia,alu,jlu,ju,jw,  ierr)
  ;
}

#endif
extern "C" {
#ifdef F77_Majuscule


  void F77DECLARE(LUSOLV2) (const int* const,const double* const,
                            const double* const,const double* const,
                            const int* const,const int* const);

  void F77DECLARE(LUTSOLV2) (const int* const,const double* const,
                             const double* const,const double* const,
                             const int* const,const int* const);

  void F77DECLARE(PGMRESV2)(const int* const, const int* const, const double* const, double* const,
                            const double* const, const double* const, const int* const, const int* const,
                            const double* const, const int* const, const int* const, const double* const,
                            const int* const, const int* const, int* const);
  void F77NAME(CGV2)(const int* const a, const double* const b,
                     double* c, int* d,double* e, double* f );

  void F77NAME(BCGSTABV2)(const int* const a, const double* const b,
                          double* c, int* d,double* e, double* f );

  void F77NAME(GMRESV2)(const int* const a, const double* const b,
                        double* c, int* d,double* e, double* f );

#else

  void F77DECLARE(lusolv2) (const int* const,const double* const,
                            const double* const,const double* const,
                            const int* const,const int* const);

  void F77DECLARE(lutsolv2) (const int* const,const double* const,
                             const double* const,const double* const,
                             const int* const,const int* const);

  void F77DECLARE(pgmresv2)(const int* const, const int* const, const double* const, double* const,
                            const double* const, const double* const, const int* const, const int* const,
                            const double* const, const int* const, const int* const, const double* const,
                            const int* const, const int* const, int* const);
  void F77NAME(cgv2)(const int* const a, const double* const b,
                     double* c, int* d,double* e, double* f );

  void F77NAME(bcgstabv2)(const int* const a, const double* const b,
                          double* c, int* d,double* e, double* f );

  void F77NAME(gmresv2)(const int* const a, const double* const b,
                        double* c, int* d,double* e, double* f );

#endif
}
#ifndef F77_Majuscule

inline void F77NAME(LUSOLV2) (const int* const a, const double* const b,
                              const double* const c, const double* const d,
                              const int* const e, const int* const f)
{
  F77NAME(lusolv2)(a,b,c,d,e,f);
}

inline void F77NAME(LUTSOLV2) (const int* const a, const double* const b,
                               const double* const c, const double* const d,
                               const int* const e, const int* const f)
{
  F77NAME(lutsolv2)(a,b,c,d,e,f);
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
inline void F77NAME(PGMRESV2)(const int* const a, const int* const b,
                              const double* const c, double* const d,
                              const double* const e, const double* const f,
                              const int* const g, const int* const h,
                              const double* const i, const int* const j,
                              const int* const k, const double* const l,
                              const int* const m, const int* const n, int* const o)
{
  F77NAME(pgmresv2)(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);
}
inline void F77NAME(CGV2)(const int* const a, const double* const b,
                          double* c,int* d,double* e,double* f )
{
  F77NAME(cgv2)(a,b,c,d,e,f);
}

inline void F77NAME(BCGSTABV2)(const int* const a, const double* const b,
                               double* c,int* d,double* e,double* f )
{
  F77NAME(bcgstabv2)(a,b,c,d,e,f);
}

inline void F77NAME(GMRESV2)(const int* const a, const double* const b,
                             double* c,int* d,double* e,double* f )
{
  F77NAME(gmresv2)(a,b,c,d,e,f);
}
#endif
#endif
