/* $Header: /home/teuler/cvsroot/lib/jmtable.f90,v 6.2 2000/03/10 09:44:45 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmtable(table,ntable,itable,n)


  /* Arguments */
  INTEGER ntable, itable, n;
  REAL8 table[];

{

 /* Variables locales */

  static REAL8 twopi;
  REAL8 temp, temp1, temp2;

  INTEGER i;

  twopi = 2 * acos(((REAL8)(-1)));

  /* Calcul des sinus et cosinus */

  /* Si n est multiple de 4, astuces en serie */
  if ((n%4) == 0) {
    /* On se debarrasse des cas limite */
    table[itable+      0] =  1;
    table[itable+n+    0] =  0;
    table[itable+    n/4] =  0;
    table[itable+n+  n/4] =  1;
    table[itable+    n/2] = -1;
    table[itable+n+  n/2] =  0;
    table[itable+  3*n/4] =  0;
    table[itable+n+3*n/4] = -1;
    /* Cas general */
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
    for (i=1;i<=n/4-1;i++) {
      temp = cos(twopi*((REAL8)(i))/((REAL8)(n)));
      table[itable+    i]     =  temp;
      table[itable+    n/2-i] = -temp;
      table[itable+    n/2+i] = -temp;
      table[itable+    n-i]   =  temp;
      table[itable+n+  n/4+i] =  temp;
      table[itable+n+  n/4-i] =  temp;
      table[itable+n+3*n/4+i] = -temp;
      table[itable+n+3*n/4-i] = -temp;
    }

  /* Si n est simplement multiple de 2 (sans etre multiple de 4) */
  } else if ((n%2) == 0) {
    /* On se debarrasse des cas limite */
    table[itable+    0] =  1;
    table[itable+n+  0] =  0;
    table[itable+  n/2] = -1;
    table[itable+n+n/2] =  0;
    /* Cas general */
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
    for (i=1;i<=n/2-1;i++) {
      temp1 = cos(twopi*((REAL8)(i))/((REAL8)(n)));
      table[itable+      i] =  temp1;
      table[itable+  n/2+i] = -temp1;
      temp2 = sin(twopi*((REAL8)(i))/((REAL8)(n)));
      table[itable+n+    i] =  temp2;
      table[itable+n+n/2+i] = -temp2;
    }

  /* Si n est impair */
  } else {
    /* On se debarrasse des cas limite */
    table[itable+  0] =  1;
    table[itable+n+0] =  0;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
    for (i=1;i<=n/2;i++) {
      temp1 = cos(twopi*((REAL8)(i))/((REAL8)(n)));
      table[itable+    i] =  temp1;
      table[itable+  n-i] =  temp1;
      temp2 = sin(twopi*((REAL8)(i))/((REAL8)(n)));
      table[itable+n+  i] =  temp2;
      table[itable+n+n-i] = -temp2;
    }

  }

}
