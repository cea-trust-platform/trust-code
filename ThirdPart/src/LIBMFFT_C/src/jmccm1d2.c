/* $Header: /home/teuler/cvsroot/lib/jmccm1d2.f90,v 6.2 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmccm1d2(p,n,m,table,ntable,itable,ntable2,mtable,work,nwork,ioff)


  /* Arguments */
  INTEGER p, n, m;
  INTEGER ntable,itable,ntable2,mtable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER k, jl;
  INTEGER it1,iu1,it2,iu2;
  INTEGER jt1,ju1,jt2,ju2;
  REAL8 x1, x2, y1, y2;
  REAL8 c, s;
  INTEGER ioff1, ioff2;

  /* On joue sur ioff pour alterner entre le haut et le bas de work */
  ioff1 = *ioff;
  ioff2 = nwork/2-ioff1;

  if ((p%2)==0) {

    /* Si p est pair, on peut travailler entierement en base 4 */
    jmccm1d4(p/2,n,m,table,ntable,itable,ntable2,mtable,work,nwork,&ioff1);
    *ioff = ioff1;

  } else {

    /* On fait les premieres etapes en base 4 */
    jmccm1d4(p/2,n,2*m,table,ntable,itable,ntable2,mtable*2,work,nwork,&ioff1);
    ioff2 = nwork/2-ioff1;
    /* On fait la derniere etape en base 2 */
    if (m >= 16 || ipow(2,(p-1)) < 8) {
      for (k=0;k<=ipow(2,(p-1))-1;k++) {

        /* Les sinus et cosinus */
        c = table[itable+        mtable*k];
        s = table[itable+ntable2+mtable*k];

        /* Les indices */
        it1 = ioff1        +m*(k*2  );
        iu1 = ioff1+nwork/4+m*(k*2  );
        it2 = ioff1        +m*(k*2+1);
        iu2 = ioff1+nwork/4+m*(k*2+1);
        jt1 = ioff2        +m*( k          );
        ju1 = ioff2+nwork/4+m*( k          );
        jt2 = ioff2        +m*((k+ipow(2,(p-1))));
        ju2 = ioff2+nwork/4+m*((k+ipow(2,(p-1))));

#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (jl=0;jl<=m-1;jl++) {
          x1 = work[it1+jl];
          y1 = work[iu1+jl];
          x2 = work[it2+jl];
          y2 = work[iu2+jl];
          work[jt1+jl] = x1 + ( x2*c - y2*s );
          work[ju1+jl] = y1 + ( x2*s + y2*c );
          work[jt2+jl] = x1 - ( x2*c - y2*s );
          work[ju2+jl] = y1 - ( x2*s + y2*c );
        }
      }
    } else {
      for (jl=0;jl<=m-1;jl++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (k=0;k<=ipow(2,(p-1))-1;k++) {
          x1 = work[ioff1+jl        +m*(k*2  )];
          y1 = work[ioff1+jl+nwork/4+m*(k*2  )];
          x2 = work[ioff1+jl        +m*(k*2+1)];
          y2 = work[ioff1+jl+nwork/4+m*(k*2+1)];
          /* Les sinus et cosinus */
          c = table[itable+        mtable*k];
          s = table[itable+ntable2+mtable*k];
          work[ioff2+jl        +m*( k          )] = x1 + ( x2*c - y2*s );
          work[ioff2+jl+nwork/4+m*( k          )] = y1 + ( x2*s + y2*c );
          work[ioff2+jl        +m*((k+ipow(2,(p-1))))] = x1 - ( x2*c - y2*s );
          work[ioff2+jl+nwork/4+m*((k+ipow(2,(p-1))))] = y1 - ( x2*s + y2*c );
        }
      }
    }

    *ioff = ioff2;

  }

}
