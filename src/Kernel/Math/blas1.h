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
// File:        blas1.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _BLAS1_H_
#define _BLAS1_H_



//
// .DESCRIPTION Appel aux blas1
//

#include <arch.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _NO_BLAS1_
extern "C"
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     Appel aux blas1
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
{

#ifdef F77_Majuscule
#ifndef CRAY
  double F77DECLARE(DASUM)(const integer *n, const double* const dx, const integer *incx);


  void F77DECLARE(DAXPY)(const integer *n, const double * const da, const double * const dx,
                         const integer *incx, double * const dy, const integer *incy);

  void F77DECLARE(DCOPY)(const integer *n,
                         const double * const dx, const integer *incx,
                         double * const dy, const integer *incy);


  double F77DECLARE(DDOT)(const integer *n, const double * const dx, const integer *incx,
                          const double * const dy, const integer *incy);

  double F77DECLARE(DNRM2)(const integer *n, const double * const dx, const integer *incx);

  void F77DECLARE(DROT)(const integer *n, double * const dx, const integer *incx, double * const dy,
                        const integer *incy, const double * const c, const double * const s);

  void F77DECLARE(DROTG)(double * const da, double * const db, double * const c, double * const s);

  void F77DECLARE(DSCAL)(const integer *n, double * const da, double * const dx, const integer *incx);

  void F77DECLARE(DSWAP)(const integer *n, double * const dx, const integer *incx, double * const dy,
                         const integer *incy);

  integer F77DECLARE(IDAMAX)(const integer *n, const double * const dx, const integer *incx);
#endif

  float F77DECLARE(SDOT)(const integer *n, const float *cx,
                         const integer *incx, const float *cy, const integer *incy);

  void F77DECLARE(SAXPY)(const integer *n, const float *da, const float *dx,
                         const integer *incx, float *dy, const integer *incy);

  void F77DECLARE(SCOPY)(const integer *n, float *dx, const integer *incx,
                         float *dy, const integer *incy);

  float F77DECLARE(SASUM)(const integer *n, const float *dx, const integer *incx);

  float F77DECLARE(SNRM2)(const integer *n, const float *dx, const integer *incx);

  void F77DECLARE(SSCAL)(const integer *n, const double * const da, float *dx,
                         const integer *incx);

  integer F77DECLARE(ISAMAX)(const integer *n, const float * const dx, const integer *incx);

  void F77DECLARE(SSWAP)(const integer *n, float *dx, const integer *incx,
                         float *dy, integer *incy);

  void F77DECLARE(SROT)(const integer *n,
                        double * const dx,
                        const integer *incx,
                        double * const dy,
                        const integer *incy,
                        const double * const c,
                        const double * const s);

  void F77DECLARE(SROTG)(double * const da,
                         double * const db,
                         double * const c,
                         double * const s);
#else

  double F77DECLARE(dasum)(const integer *n, const double* const dx, const integer *incx);


  void F77DECLARE(daxpy)(const integer *n, const double * const da, const double * const dx,
                         const integer *incx, double * const dy, const integer *incy);

  void F77DECLARE(dcopy)(const integer *n,
                         const double * const dx, const integer *incx,
                         double * const dy, const integer *incy);


  double F77DECLARE(ddot)(const integer *n, const double * const dx, const integer *incx,
                          const double * const dy, const integer *incy);

  double F77DECLARE(dnrm2)(const integer *n, const double * const dx, const integer *incx);

  void F77DECLARE(drot)(const integer *n, double * const dx, const integer *incx, double * const dy,
                        const integer *incy, const double * const c, const double * const s);

  void F77DECLARE(drotg)(double * const da, double * const db, double * const c, double * const s);

  void F77DECLARE(dscal)(const integer *n, double * const da, double * const dx, const integer *incx);

  void F77DECLARE(dswap)(const integer *n, double * const dx, const integer *incx, double * const dy,
                         const integer *incy);

  integer F77DECLARE(idamax)(const integer *n, const double * const dx, const integer *incx);

  float F77DECLARE(sdot)(const integer *n, const float *cx,
                         const integer *incx, const float *cy, const integer *incy);

  void F77DECLARE(saxpy)(const integer *n, const float *da, const float *dx,
                         const integer *incx, float *dy, const integer *incy);

  void F77DECLARE(scopy)(const integer *n, float *dx, const integer *incx,
                         float *dy, const integer *incy);

  float F77DECLARE(sasum)(const integer *n, const float *dx, const integer *incx);

  float F77DECLARE(snrm2)(const integer *n, const float *dx, const integer *incx);

  void F77DECLARE(sscal)(const integer *n, const float *da, float *dx,
                         const integer *incx);

  integer F77DECLARE(isamax)(const integer *n, const float * const dx, const integer *incx);

  void F77DECLARE(sswap)(const integer *n, float *dx, const integer *incx,
                         float *dy, integer *incy);

  void F77DECLARE(srot)(const integer *n,
                        double * const dx,
                        const integer *incx,
                        double * const dy,
                        const integer *incy,
                        const double * const c,
                        const double * const s);

  void F77DECLARE(srotg)(double * const da,
                         double * const db,
                         double * const c,
                         double * const s);

#endif
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
inline double F77NAME(DASUM)(const integer *n,
                             const double* const dx,
                             const integer *incx)
{
  return F77NAME(dasum)(n ,dx ,incx);
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
inline void F77NAME(DAXPY)(const integer *n,
                           const double * const da,
                           const double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  F77NAME(daxpy)(n ,da, dx ,incx, dy, incy);
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
inline void F77NAME(DCOPY)(const integer *n,
                           const double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  F77NAME(dcopy)(n , dx , incx, dy, incy);
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
inline double F77NAME(DDOT)(const integer *n,
                            const double * const dx,
                            const integer *incx,
                            const double * const dy,
                            const integer *incy)
{
  return F77NAME(ddot)(n , dx , incx, dy, incy);
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
inline double F77NAME(DNRM2)(const integer *n,
                             const double * const dx,
                             const integer *incx)
{
  return F77NAME(dnrm2)(n , dx , incx);
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
inline void F77NAME(DROT)(const integer *n,
                          double * const dx,
                          const integer *incx,
                          double * const dy,
                          const integer *incy,
                          const double * const c,
                          const double * const s)
{
  F77NAME(drot)(n , dx , incx, dy, incy, c, s);
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
inline void F77NAME(DROTG)(double * const da,
                           double * const db,
                           double * const c,
                           double * const s)
{
  F77NAME(drotg)(da , db , c, s);
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
inline void F77NAME(DSCAL)(const integer *n,
                           double * const da,
                           double * const dx,
                           const integer *incx)
{
  F77NAME(dscal)(n , da , dx, incx);
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
inline void F77NAME(DSWAP)(const integer *n,
                           double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  F77NAME(dswap)(n , dx , incx, dy, incy);
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
inline integer F77NAME(IDAMAX)(const integer *n,
                               const double * const dx,
                               const integer *incx)
{
  return F77NAME(idamax)(n , dx , incx);
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
inline float F77NAME(SDOT)(const integer *n,
                           const float *dx,
                           const integer *incx,
                           const float *dy,
                           const integer *incy)
{
  return F77NAME(sdot)(n , dx , incx, dy, incy);
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
inline void F77NAME(SAXPY)(const integer *n,
                           const float *da,
                           const float *dx,
                           const integer *incx,
                           float *dy,
                           const integer *incy)
{
  F77NAME(saxpy)(n , da, dx , incx, dy, incy);
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
inline void F77NAME(SCOPY)(const integer *n,
                           float *dx,
                           const integer *incx,
                           float *dy,
                           const integer *incy)
{
  F77NAME(scopy)(n , dx , incx, dy, incy);
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
inline float F77NAME(SASUM)(const integer *n,
                            const float *dx,
                            const integer *incx)
{
  return F77NAME(sasum)(n , dx , incx);
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
inline float F77NAME(SNRM2)(const integer *n,
                            const float *dx,
                            const integer *incx)
{
  return F77NAME(snrm2)(n , dx , incx);
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
inline void F77NAME(SSCAL)(const integer *n,
                           const float *da,
                           float *dx,
                           const integer *incx)
{
  F77NAME(sscal)(n , da, dx , incx);
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
inline integer F77NAME(ISAMAX)(const integer *n,
                               const float * const dx,
                               const integer *incx)
{
  return F77NAME(isamax)(n , dx , incx);
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
inline void F77NAME(SSWAP)(const integer *n,
                           float *dx,
                           const integer *incx,
                           float *dy,
                           integer *incy)
{
  F77NAME(sswap)(n , dx , incx, dy, incy);
}

inline void F77NAME(SROT)(const integer *n,
                          double * const dx,
                          const integer *incx,
                          double * const dy,
                          const integer *incy,
                          const double * const c,
                          const double * const s)
{
  F77NAME(srot)(n,dx,incx,dy,incy,c,s);
}

inline void F77NAME(SROTG)(double * const da,
                           double * const db,
                           double * const c,
                           double * const s)
{
  F77NAME(srotg)(da,db,c,s);
}
#endif

#ifdef CRAY
inline double F77NAME(DASUM)(const integer *n,
                             const double* const dx,
                             const integer *incx)
{
  return F77NAME(SASUM)(n , dx , incx);
}


inline void F77NAME(DAXPY)(const integer *n,
                           const double * const da,
                           const double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  F77NAME(SAXPY)(n,da,dx,incx,dy,incy);
}

inline void F77NAME(DCOPY)(const integer *n,
                           const double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  F77NAME(SCOPY)(n,dx,incx,dy,incy);
}

inline double F77NAME(DDOT)(const integer *n,
                            const double * const dx,
                            const integer *incx,
                            const double * const dy,
                            const integer *incy)
{
  return F77NAME(SDOT)(n,dx,incx,dy,incy);
}

inline double F77NAME(DNRM2)(const integer *n,
                             const double * const dx,
                             const integer *incx)
{
  return F77NAME(SNRM2)(n,dx,incx);
}

inline void F77NAME(DROT)(const integer *n,
                          double * const dx,
                          const integer *incx,
                          double * const dy,
                          const integer *incy,
                          const double * const c,
                          const double * const s)
{
  F77NAME(SROT)(n,dx,incx,dy,incy,c,s);
}

inline void F77NAME(DROTG)(double * const da,
                           double * const db,
                           double * const c,
                           double * const s)
{
  F77NAME(SROTG)(da,db,c,s);
}

inline void F77NAME(DSCAL)(const integer *n,
                           double * const da,
                           double * const dx,
                           const integer *incx)
{
  F77NAME(SSCAL)(n,da,dx,incx);
}

inline void F77NAME(DSWAP)(const integer *n,
                           double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  F77NAME(SSWAP)(n,dx,incx,dy,incy);
}

inline integer F77NAME(IDAMAX)(const integer *n,
                               const double * const dx,
                               const integer *incx)
{
  return F77NAME(ISAMAX)(n,dx,incx);
}
#endif  // CRAY

#else //_NO_BLAS1_

// .DESCRIPTION
static inline void ERREUR(const char* const ch)
{
  //        cerr << ch << endl;
  exit();
}

// .DESCRIPTION
inline double F77NAME(DASUM)(const integer *n,
                             const double* const dx,
                             const integer *incx)
{
  double somme=0;
  for(integer i=0; i<*n; i+=*incx)
    somme+=dx[i];
  return somme;
}


// .DESCRIPTION
inline void F77NAME(DAXPY)(const integer *n,
                           const double * const da,
                           const double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  int j =0;
  for(int i=0; i<*n; i+=*incx)
    {
      dy[i]+=*da*dx[j];
      j+=*incy;
    }
}

// .DESCRIPTION
inline void F77NAME(DCOPY)(const integer *n,
                           const double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  int j=0;
  for(int i=0; i<*n; i+=*incx)
    {
      dy[i]=dx[j];
      j+=*incy;
    }

}


// .DESCRIPTION
inline double F77NAME(DDOT)(const integer *n,
                            const double * const dx,
                            const integer *incx,
                            const double * const dy,
                            const integer *incy)
{
  int j=0;
  double psc=0;
  for(int i=0; i<*n; i+=*incx)
    {
      psc+=dx[i]*dy[j];
      j+=*incy;
    }
  return psc;
}

// .DESCRIPTION
inline double F77NAME(DNRM2)(const integer *n,
                             const double * const dx,
                             const integer *incx)
{
  integer un=1;
  return sqrt(F77NAME(DDOT)(n, dx, &un, dx, &un));
}

// .DESCRIPTION
inline void F77NAME(DROT)(const integer *n,
                          double * const dx,
                          const integer *incx,
                          double * const dy,
                          const integer *incy,
                          const double * const c,
                          const double * const s)
{
  ERREUR("DROT : pas code!");
}

// .DESCRIPTION
inline void F77NAME(DROTG)(double * const da,
                           double * const db,
                           double * const c,
                           double * const s)
{
  ERREUR("DROTG : pas code!");
}

// .DESCRIPTION
inline void F77NAME(DSCAL)(const integer *n,
                           double * const da,
                           double * const dx,
                           const integer *incx)
{
  for(int i=0; i<*n; i+= *incx)
    dx[i]*=*da;
}

// .DESCRIPTION
inline void F77NAME(DSWAP)(const integer *n,
                           double * const dx,
                           const integer *incx,
                           double * const dy,
                           const integer *incy)
{
  int j=0;
  double tmp;
  for(int i=0; i<*n; i+= *incx)
    {
      tmp=dx[i];
      dx[i]=dy[j];
      dy[j]=tmp;
      j+=*incy;
    }

}

// .DESCRIPTION
inline integer F77NAME(IDAMAX)(const integer *n,
                               const double * const dx,
                               const integer *incx)
{
  int imax=0;
  double themax=dx[0];
  for(int i=1; i<*n; i+=*incx)
    if(dx[i]>themax)
      {
        imax=i;
        themax=dx[i];
      }
  return imax;
}

// .DESCRIPTION
inline float F77NAME(SDOT)(const integer *n,
                           const float *dx,
                           const integer *incx,
                           const float *dy,
                           const integer *incy)
{
  ERREUR("SDOT : pas code!");
  return 0;
}

// .DESCRIPTION
inline void F77NAME(SAXPY)(const integer *n,
                           const float *da,
                           const float *dx,
                           const integer *incx,
                           float *dy,
                           const integer *incy)
{
  ERREUR("SAXPY : pas code!");
}

// .DESCRIPTION
inline void F77NAME(SCOPY)(const integer *n,
                           float *dx,
                           const integer *incx,
                           float *dy,
                           const integer *incy)
{
  ERREUR("SCOPY : pas code!");
}

// .DESCRIPTION
inline float F77NAME(SASUM)(const integer *n,
                            const float *dx,
                            const integer *incx)
{
  ERREUR("SASUM : pas code!");
  return 0;
}

// .DESCRIPTION
inline float F77NAME(SNRM2)(const integer *n,
                            const float *dx,
                            const integer *incx)
{
  ERREUR("SNRM2 : pas code!");
  return 0;
}

// .DESCRIPTION
inline void F77NAME(SSCAL)(const integer *n,
                           const float *da,
                           float *dx,
                           const integer *incx)
{
  ERREUR("SSCAL : pas code!");
}

// .DESCRIPTION
inline integer F77NAME(ISAMAX)(const integer *n,
                               const float * const dx,
                               const integer *incx)
{
  ERREUR("ISAMAX : pas code!");
  return 0;
}

// .DESCRIPTION
inline void F77NAME(SSWAP)(const integer *n,
                           float *dx,
                           const integer *incx,
                           float *dy,
                           integer *incy)
{
  ERREUR("SSWAP : pas code!");
}
#endif //_NO_BLAS1_

#endif //_BLAS1_H_

