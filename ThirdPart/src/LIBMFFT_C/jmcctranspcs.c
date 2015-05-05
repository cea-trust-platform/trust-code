/* $Header: /home/teuler/cvsroot/lib/jmcctranspcs.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmcctranspcs(m,n,n2,n3,table,ntable,itable,work,nwork,ioff)

  /* Cette subroutine permute le contenu du tableau work de la facon suivante */
  /* On considere qu'a l'origine ce tableau est en (m,n3,n2) */
  /* On doit transposer le terme d'ordre (k,j,i) en (k,i,j) */
  /* On en profite pour faire les multiplications par wij */
  /* Le role de n est seulement d'attaquer les bonnes valeurs du tableau table */
  /* (il y a un ecart de n entre les cos et les sin, et le stride entre */
  /* les cos est de n/(n2*n3) */
  /* Note : le sens de n2 et n3 ici n'a rien a voir avec celui de jmccm1d */


  /* Arguments */
  INTEGER m, n;
  INTEGER n2, n3;
  INTEGER ntable,itable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER i, j, k;
  REAL8 t, u, c, s;
  INTEGER ioff1, ioff2;
  INTEGER is;

  /* Gestion des offsets */
  ioff1 = *ioff;
  ioff2 = nwork/2-*ioff;

  /* Gestion du stride */
  is = n/(n2*n3);

  if ( m >= 16 || (n2 < 8 && n3 < 8) ) {

    for (i=0;i<=n2-1;i++) {
      for (j=0;j<=n3-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (k=0;k<=m-1;k++) {
          t = work[ioff1        +k+m*(j+n3*i)];
          u = work[ioff1+nwork/4+k+m*(j+n3*i)];
          c = table[itable+  is*i*j];
          s = table[itable+n+is*i*j];
          work[ioff2        +k+m*(i+n2*j)] = c*t-s*u;
          work[ioff2+nwork/4+k+m*(i+n2*j)] = c*u+s*t;
        }
      }
    }

  } else if ( n2 >= 16 || n3 < 8 ) {

    for (j=0;j<=n3-1;j++) {
      for (k=0;k<=m-1;k++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n2-1;i++) {
          t = work[ioff1        +k+m*(j+n3*i)];
          u = work[ioff1+nwork/4+k+m*(j+n3*i)];
          c = table[itable+  is*i*j];
          s = table[itable+n+is*i*j];
          work[ioff2        +k+m*(i+n2*j)] = c*t-s*u;
          work[ioff2+nwork/4+k+m*(i+n2*j)] = c*u+s*t;
        }
      }
    }

  } else {

    for (i=0;i<=n2-1;i++) {
      for (k=0;k<=m-1;k++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=n3-1;j++) {
          t = work[ioff1        +k+m*(j+n3*i)];
          u = work[ioff1+nwork/4+k+m*(j+n3*i)];
          c = table[itable+  is*i*j];
          s = table[itable+n+is*i*j];
          work[ioff2        +k+m*(i+n2*j)] = c*t-s*u;
          work[ioff2+nwork/4+k+m*(i+n2*j)] = c*u+s*t;
        }
      }
    }

  }

  *ioff = ioff2;

}
