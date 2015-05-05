/* $Header: /home/teuler/cvsroot/lib/jmccm1d3.f90,v 6.2 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmccm1d3(p,n,m,table,ntable,itable,ntable2,mtable,work,nwork,ioff)


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
  REAL8 x1, x2, x3, y1, y2, y3, t1, t2, t3, u1, u2, u3;
  REAL8 c2, s2, c3, s3;
  INTEGER it1,iu1,it2,iu2,it3,iu3;
  INTEGER jt1,ju1,jt2,ju2,jt3,ju3;
  REAL8 r,s,t,u;

  /* Gestion des constantes cosinus */
  static REAL8 ctwopi3, stwopi3;
  static INTEGER first = 1;

  INTEGER ioff1, ioff2;

  /* On recupere cos et sin de 2*pi/3 */
  if (first) {
    first = 0;
    ctwopi3 = -((REAL8)(1))/((REAL8)(2));
    stwopi3 = sqrt(((REAL8)(3)))/((REAL8)(2));
  }

  /* On joue sur ioff pour alterner entre le haut et le bas de work */
  ioff1 = *ioff;
  ioff2 = nwork/2-ioff1;

  /* Boucle sur les etapes */
  for (i=0;i<=p-1;i++) {

    if (m*ipow(3,(p-i-1)) >= 16 || ipow(3,i) < 8) {

      for (k=0;k<=ipow(3,i)-1;k++) {

        /* Les sinus et cosinus */
        c2 = table[itable+        mtable*  ipow(3,(p-i-1))*k];
        s2 = table[itable+ntable2+mtable*  ipow(3,(p-i-1))*k];
        c3 = table[itable+        mtable*2*ipow(3,(p-i-1))*k];
        s3 = table[itable+ntable2+mtable*2*ipow(3,(p-i-1))*k];

        /* Les indices */
        it1 = ioff1        +m*(k*ipow(3,(p-i))             );
        iu1 = ioff1+nwork/4+m*(k*ipow(3,(p-i))             );
        it2 = ioff1        +m*(k*ipow(3,(p-i))+  ipow(3,(p-i-1)));
        iu2 = ioff1+nwork/4+m*(k*ipow(3,(p-i))+  ipow(3,(p-i-1)));
        it3 = ioff1        +m*(k*ipow(3,(p-i))+2*ipow(3,(p-i-1)));
        iu3 = ioff1+nwork/4+m*(k*ipow(3,(p-i))+2*ipow(3,(p-i-1)));
        jt1 = ioff2        +m*( k        *ipow(3,(p-i-1)));
        ju1 = ioff2+nwork/4+m*( k        *ipow(3,(p-i-1)));
        jt2 = ioff2        +m*((k+  ipow(3,i))*ipow(3,(p-i-1)));
        ju2 = ioff2+nwork/4+m*((k+  ipow(3,i))*ipow(3,(p-i-1)));
        jt3 = ioff2        +m*((k+2*ipow(3,i))*ipow(3,(p-i-1)));
        ju3 = ioff2+nwork/4+m*((k+2*ipow(3,i))*ipow(3,(p-i-1)));

#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (jl=0;jl<=m*ipow(3,(p-i-1))-1;jl++) {

          r = (c2*work[it2+jl])-(s2*work[iu2+jl]);
          s = (c2*work[iu2+jl])+(s2*work[it2+jl]);
          t = (c3*work[it3+jl])-(s3*work[iu3+jl]);
          u = (c3*work[iu3+jl])+(s3*work[it3+jl]);
          x1 = work[it1+jl];
          y1 = work[iu1+jl];
          work[jt1+jl] = x1 + r + t;
          work[ju1+jl] = y1 + s + u;
          work[jt2+jl] = x1 + ctwopi3*(r+t) - stwopi3*(s-u);
          work[ju2+jl] = y1 + ctwopi3*(s+u) + stwopi3*(r-t);
          work[jt3+jl] = x1 + ctwopi3*(r+t) + stwopi3*(s-u);
          work[ju3+jl] = y1 + ctwopi3*(s+u) - stwopi3*(r-t);

        }

      }

    } else {

      for (jl=0;jl<=m*ipow(3,(p-i-1))-1;jl++) {

#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (k=0;k<=ipow(3,i)-1;k++) {

          t1 = work[ioff1+jl        +m*(k*ipow(3,(p-i))             )];
          u1 = work[ioff1+jl+nwork/4+m*(k*ipow(3,(p-i))             )];
          t2 = work[ioff1+jl        +m*(k*ipow(3,(p-i))+  ipow(3,(p-i-1)))];
          u2 = work[ioff1+jl+nwork/4+m*(k*ipow(3,(p-i))+  ipow(3,(p-i-1)))];
          t3 = work[ioff1+jl        +m*(k*ipow(3,(p-i))+2*ipow(3,(p-i-1)))];
          u3 = work[ioff1+jl+nwork/4+m*(k*ipow(3,(p-i))+2*ipow(3,(p-i-1)))];

          /* Les sinus et cosinus */
          c2 = table[itable+        mtable*  ipow(3,(p-i-1))*k];
          s2 = table[itable+ntable2+mtable*  ipow(3,(p-i-1))*k];
          c3 = table[itable+        mtable*2*ipow(3,(p-i-1))*k];
          s3 = table[itable+ntable2+mtable*2*ipow(3,(p-i-1))*k];

          /* On premultiplie */
          x1 = t1;
          y1 = u1;
          x2 = c2*t2-s2*u2;
          y2 = c2*u2+s2*t2;
          x3 = c3*t3-s3*u3;
          y3 = c3*u3+s3*t3;

          /* Il reste a multiplier par les twopi3 */
          work[ioff2+jl        +m*( k        *ipow(3,(p-i-1)))] = 
            x1 + x2                    + x3;
          work[ioff2+jl+nwork/4+m*( k        *ipow(3,(p-i-1)))] = 
            y1 + y2                    + y3;
          work[ioff2+jl        +m*((k+  ipow(3,i))*ipow(3,(p-i-1)))] = 
            x1 + ctwopi3*x2-stwopi3*y2 + ctwopi3*x3+stwopi3*y3;
          work[ioff2+jl+nwork/4+m*((k+  ipow(3,i))*ipow(3,(p-i-1)))] = 
            y1 + ctwopi3*y2+stwopi3*x2 + ctwopi3*y3-stwopi3*x3;
          work[ioff2+jl        +m*((k+2*ipow(3,i))*ipow(3,(p-i-1)))] = 
            x1 + ctwopi3*x2+stwopi3*y2 + ctwopi3*x3-stwopi3*y3;
          work[ioff2+jl+nwork/4+m*((k+2*ipow(3,i))*ipow(3,(p-i-1)))] = 
            y1 + ctwopi3*y2-stwopi3*x2 + ctwopi3*y3+stwopi3*x3;

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
