/* $Header: /home/teuler/cvsroot/lib/jmccm1d5.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmccm1d5(p,n,m,table,ntable,itable,ntable2,mtable,work,nwork,ioff)


  /* Arguments */
  INTEGER p, n, m;
  INTEGER ntable,itable,ntable2, mtable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER i, k, jl;
  REAL8 x0,x1,x2,x3,x4,y0,y1,y2,y3,y4,t0,t1,t2,t3,t4,u0,u1,u2,u3,u4;
  REAL8 c1, s1, c2, s2, c3, s3, c4, s4;
  INTEGER ioff1, ioff2;

  /* Gestion des constantes cosinus */
  static REAL8 twopi5;
  static REAL8 ctwopi51, ctwopi52, ctwopi53, ctwopi54;
  static REAL8 stwopi51, stwopi52, stwopi53, stwopi54;
  static INTEGER first = 1;

  /* On recupere cos et sin de 2*pi/5 */
  if (first) {
    first = 0;
    twopi5   = 2*acos(((REAL8)(-1)))/((REAL8)(5));
    ctwopi51 = cos(  twopi5);
    stwopi51 = sin(  twopi5);
    ctwopi52 = cos(2*twopi5);
    stwopi52 = sin(2*twopi5);
    ctwopi53 = cos(3*twopi5);
    stwopi53 = sin(3*twopi5);
    ctwopi54 = cos(4*twopi5);
    stwopi54 = sin(4*twopi5);
  }

  /* On joue sur ioff pour alterner entre le haut et le bas de work */
  ioff1 = *ioff;
  ioff2 = nwork/2-ioff1;

  /* Boucle sur les etapes */
  for (i=0;i<=p-1;i++) {

    if (m*ipow(5,(p-i-1)) >= 16 || ipow(5,i) < 8) {

      for (k=0;k<=ipow(5,i)-1;k++) {

#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (jl=0;jl<=m*ipow(5,(p-i-1))-1;jl++) {

          t0 = work[ioff1+jl        +m*(k*ipow(5,(p-i))             )];
          u0 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))             )];
          t1 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+  ipow(5,(p-i-1)))];
          u1 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+  ipow(5,(p-i-1)))];
          t2 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+2*ipow(5,(p-i-1)))];
          u2 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+2*ipow(5,(p-i-1)))];
          t3 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+3*ipow(5,(p-i-1)))];
          u3 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+3*ipow(5,(p-i-1)))];
          t4 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+4*ipow(5,(p-i-1)))];
          u4 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+4*ipow(5,(p-i-1)))];

          /* Les sinus et cosinus */
          c1 = table[itable+        mtable*  ipow(5,(p-i-1))*k];
          s1 = table[itable+ntable2+mtable*  ipow(5,(p-i-1))*k];
          c2 = table[itable+        mtable*2*ipow(5,(p-i-1))*k];
          s2 = table[itable+ntable2+mtable*2*ipow(5,(p-i-1))*k];
          c3 = table[itable+        mtable*3*ipow(5,(p-i-1))*k];
          s3 = table[itable+ntable2+mtable*3*ipow(5,(p-i-1))*k];
          c4 = table[itable+        mtable*4*ipow(5,(p-i-1))*k];
          s4 = table[itable+ntable2+mtable*4*ipow(5,(p-i-1))*k];

          /* On premultiplie */
          x0 = t0;
          y0 = u0;
          x1 = c1*t1-s1*u1;
          y1 = c1*u1+s1*t1;
          x2 = c2*t2-s2*u2;
          y2 = c2*u2+s2*t2;
          x3 = c3*t3-s3*u3;
          y3 = c3*u3+s3*t3;
          x4 = c4*t4-s4*u4;
          y4 = c4*u4+s4*t4;

          /* Il reste a multiplier par les twopi5 */
          work[ioff2+jl        +m*( k        *ipow(5,(p-i-1)))] =   
            x0 + x1                    + x2                    
               + x3                    + x4;
          work[ioff2+jl+nwork/4+m*( k        *ipow(5,(p-i-1)))] =   
            y0 + y1                    + y2                    
               + y3                    + y4;
          work[ioff2+jl        +m*((k+  ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi51*x1 - stwopi51*y1 
               + ctwopi52*x2 - stwopi52*y2 
               + ctwopi53*x3 - stwopi53*y3 
               + ctwopi54*x4 - stwopi54*y4;
          work[ioff2+jl+nwork/4+m*((k+  ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi51*y1 + stwopi51*x1 
               + ctwopi52*y2 + stwopi52*x2 
               + ctwopi53*y3 + stwopi53*x3 
               + ctwopi54*y4 + stwopi54*x4;
          work[ioff2+jl        +m*((k+2*ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi52*x1 - stwopi52*y1 
               + ctwopi54*x2 - stwopi54*y2 
               + ctwopi51*x3 - stwopi51*y3 
               + ctwopi53*x4 - stwopi53*y4;
          work[ioff2+jl+nwork/4+m*((k+2*ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi52*y1 + stwopi52*x1 
               + ctwopi54*y2 + stwopi54*x2 
               + ctwopi51*y3 + stwopi51*x3 
               + ctwopi53*y4 + stwopi53*x4;
          work[ioff2+jl        +m*((k+3*ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi53*x1 - stwopi53*y1 
               + ctwopi51*x2 - stwopi51*y2 
               + ctwopi54*x3 - stwopi54*y3 
               + ctwopi52*x4 - stwopi52*y4;
          work[ioff2+jl+nwork/4+m*((k+3*ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi53*y1 + stwopi53*x1 
               + ctwopi51*y2 + stwopi51*x2 
               + ctwopi54*y3 + stwopi54*x3 
               + ctwopi52*y4 + stwopi52*x4;
          work[ioff2+jl        +m*((k+4*ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi54*x1 - stwopi54*y1 
               + ctwopi53*x2 - stwopi53*y2 
               + ctwopi52*x3 - stwopi52*y3 
               + ctwopi51*x4 - stwopi51*y4;
          work[ioff2+jl+nwork/4+m*((k+4*ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi54*y1 + stwopi54*x1 
               + ctwopi53*y2 + stwopi53*x2 
               + ctwopi52*y3 + stwopi52*x3 
               + ctwopi51*y4 + stwopi51*x4;

        }

      }

    } else {

      for (jl=0;jl<=m*ipow(5,(p-i-1))-1;jl++) {

#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (k=0;k<=ipow(5,i)-1;k++) {

          t0 = work[ioff1+jl        +m*(k*ipow(5,(p-i))             )];
          u0 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))             )];
          t1 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+  ipow(5,(p-i-1)))];
          u1 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+  ipow(5,(p-i-1)))];
          t2 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+2*ipow(5,(p-i-1)))];
          u2 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+2*ipow(5,(p-i-1)))];
          t3 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+3*ipow(5,(p-i-1)))];
          u3 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+3*ipow(5,(p-i-1)))];
          t4 = work[ioff1+jl        +m*(k*ipow(5,(p-i))+4*ipow(5,(p-i-1)))];
          u4 = work[ioff1+jl+nwork/4+m*(k*ipow(5,(p-i))+4*ipow(5,(p-i-1)))];

          /* Les sinus et cosinus */
          c1 = table[itable+        mtable*  ipow(5,(p-i-1))*k];
          s1 = table[itable+ntable2+mtable*  ipow(5,(p-i-1))*k];
          c2 = table[itable+        mtable*2*ipow(5,(p-i-1))*k];
          s2 = table[itable+ntable2+mtable*2*ipow(5,(p-i-1))*k];
          c3 = table[itable+        mtable*3*ipow(5,(p-i-1))*k];
          s3 = table[itable+ntable2+mtable*3*ipow(5,(p-i-1))*k];
          c4 = table[itable+        mtable*4*ipow(5,(p-i-1))*k];
          s4 = table[itable+ntable2+mtable*4*ipow(5,(p-i-1))*k];

          /* On premultiplie */
          x0 = t0;
          y0 = u0;
          x1 = c1*t1-s1*u1;
          y1 = c1*u1+s1*t1;
          x2 = c2*t2-s2*u2;
          y2 = c2*u2+s2*t2;
          x3 = c3*t3-s3*u3;
          y3 = c3*u3+s3*t3;
          x4 = c4*t4-s4*u4;
          y4 = c4*u4+s4*t4;

          /* Il reste a multiplier par les twopi5 */
          work[ioff2+jl        +m*( k        *ipow(5,(p-i-1)))] =   
            x0 + x1                    + x2                    
               + x3                    + x4;
          work[ioff2+jl+nwork/4+m*( k        *ipow(5,(p-i-1)))] =   
            y0 + y1                    + y2                    
               + y3                    + y4;
          work[ioff2+jl        +m*((k+  ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi51*x1 - stwopi51*y1 
               + ctwopi52*x2 - stwopi52*y2 
               + ctwopi53*x3 - stwopi53*y3 
               + ctwopi54*x4 - stwopi54*y4;
          work[ioff2+jl+nwork/4+m*((k+  ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi51*y1 + stwopi51*x1 
               + ctwopi52*y2 + stwopi52*x2 
               + ctwopi53*y3 + stwopi53*x3 
               + ctwopi54*y4 + stwopi54*x4;
          work[ioff2+jl        +m*((k+2*ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi52*x1 - stwopi52*y1 
               + ctwopi54*x2 - stwopi54*y2 
               + ctwopi51*x3 - stwopi51*y3 
               + ctwopi53*x4 - stwopi53*y4;
          work[ioff2+jl+nwork/4+m*((k+2*ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi52*y1 + stwopi52*x1 
               + ctwopi54*y2 + stwopi54*x2 
               + ctwopi51*y3 + stwopi51*x3 
               + ctwopi53*y4 + stwopi53*x4;
          work[ioff2+jl        +m*((k+3*ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi53*x1 - stwopi53*y1 
               + ctwopi51*x2 - stwopi51*y2 
               + ctwopi54*x3 - stwopi54*y3 
               + ctwopi52*x4 - stwopi52*y4;
          work[ioff2+jl+nwork/4+m*((k+3*ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi53*y1 + stwopi53*x1 
               + ctwopi51*y2 + stwopi51*x2 
               + ctwopi54*y3 + stwopi54*x3 
               + ctwopi52*y4 + stwopi52*x4;
          work[ioff2+jl        +m*((k+4*ipow(5,i))*ipow(5,(p-i-1)))] =   
            x0 + ctwopi54*x1 - stwopi54*y1 
               + ctwopi53*x2 - stwopi53*y2 
               + ctwopi52*x3 - stwopi52*y3 
               + ctwopi51*x4 - stwopi51*y4;
          work[ioff2+jl+nwork/4+m*((k+4*ipow(5,i))*ipow(5,(p-i-1)))] =   
            y0 + ctwopi54*y1 + stwopi54*x1 
               + ctwopi53*y2 + stwopi53*x2 
               + ctwopi52*y3 + stwopi52*x3 
               + ctwopi51*y4 + stwopi51*x4;

        }

      }

    }

    /* On alterne les offsets */
    ioff1 = nwork/2-ioff1;
    ioff2 = nwork/2-ioff2;

  /* Fin boucle sur le nombre d'etapes */
  }

  *ioff = ioff1;

}
