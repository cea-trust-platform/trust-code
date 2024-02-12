/* $Header: /home/teuler/cvsroot/lib/jmscm1d.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmscm1d(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,ioff)


  /* Arguments */
  INTEGER m, n;
  INTEGER nfact, ifact;
  INTEGER fact[];
  INTEGER ntable,itable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER ioff1, ioff2;
  INTEGER i, j;
  REAL8 t, u, v, w;
  REAL8 c, s;
  INTEGER is, it;

  /* Gestion de work */
  ioff1 = *ioff;
  ioff2 = nwork/2 - ioff1;

  /* On doit faire m T.F. reelles de longueur n */
  /* Si m est pair */
  if ((m%2) == 0) {

    /* On distribue */
    if (m/2 >= 16 || n < 8) {

      for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m/2-1;j++) {
          work[ioff2        +i*m/2+j] = work[ioff1+i*m+j    ];
          work[ioff2+nwork/4+i*m/2+j] = work[ioff1+i*m+j+m/2];
        }
      }

    } else {

      for (j=0;j<=m/2-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n-1;i++) {
          work[ioff2        +i*m/2+j] = work[ioff1+i*m+j    ];
          work[ioff2+nwork/4+i*m/2+j] = work[ioff1+i*m+j+m/2];
        }
      }

    }
        
    /* On fait m/2 t.f. complexes de longueur n */
    jmccm1d(m/2,n,fact,nfact,ifact,table,ntable,itable,work,nwork,&ioff2);
    ioff1 = nwork/2 - ioff2;

    /* On regenere le resultat */
    if (m/2 >= 16 || n/2 < 8) {

      for (i=0;i<=n/2;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m/2-1;j++) {
          it = n-i;
          if (i == 0) it = 0;
          t = work[ioff2        + i*m/2+j];
          u = work[ioff2+nwork/4+ i*m/2+j];
          v = work[ioff2        +it*m/2+j];
          w = work[ioff2+nwork/4+it*m/2+j];
          work[ioff1        +i*m+j    ] = (t+v)/2;
          work[ioff1+nwork/4+i*m+j    ] = (u-w)/2;
          work[ioff1        +i*m+j+m/2] = (u+w)/2;
          work[ioff1+nwork/4+i*m+j+m/2] = (v-t)/2;
        }
      }

    } else {

      for (j=0;j<=m/2-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2;i++) {
          it = n-i;
          if (i == 0) it = 0;
          t = work[ioff2        + i*m/2+j];
          u = work[ioff2+nwork/4+ i*m/2+j];
          v = work[ioff2        +it*m/2+j];
          w = work[ioff2+nwork/4+it*m/2+j];
          work[ioff1        +i*m+j    ] = (t+v)/2;
          work[ioff1+nwork/4+i*m+j    ] = (u-w)/2;
          work[ioff1        +i*m+j+m/2] = (u+w)/2;
          work[ioff1+nwork/4+i*m+j+m/2] = (v-t)/2;
        }
      }

    }

  /* Si m n'est pas pair mais que n l'est */
  } else if ((n%2) == 0) {

    /* On distribue les indices pairs et impairs selon n */

    if (m >= 16 || n/2 < 8) {

      for (i=0;i<=n/2-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m-1;j++) {
          work[ioff2+        m*i+j] = work[ioff1+m*(2*i  )+j];
          work[ioff2+nwork/4+m*i+j] = work[ioff1+m*(2*i+1)+j];
        }
      }

    } else {

      for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2-1;i++) {
          work[ioff2+        m*i+j] = work[ioff1+m*(2*i  )+j];
          work[ioff2+nwork/4+m*i+j] = work[ioff1+m*(2*i+1)+j];
        }
      }

    }

    /* On fait m t.f. complexes de taille n/2 */
    fact[ifact+1] = fact[ifact+1]/2; /* Revient a remplacer n2 par n2/2*/
    fact[ifact+2] = fact[ifact+2]-1; /* Revient a remplacer p2 par p2-1*/
    jmccm1d(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,&ioff2);
    fact[ifact+1] = fact[ifact+1]*2; /* On retablit les valeurs initiales*/
    fact[ifact+2] = fact[ifact+2]+1;
    ioff1 = nwork/2 - ioff2;

    /* Maintenant, il faut reconstituer la t.f. reelle */

    if (m >= 16 || n/2 < 8) {

      for (i=0;i<=n/2;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m-1;j++) {
          is = i;
          it = n/2-i;
          if (i == 0 || i == n/2) {
            is = 0;
            it = 0;
          }
          t = work[ioff2        +is*m+j];
          u = work[ioff2+nwork/4+is*m+j];
          v = work[ioff2        +it*m+j];
          w = work[ioff2+nwork/4+it*m+j];
          c = table[itable+i];
          s = table[itable+i+n];
          work[ioff1        +i*m+j] = (t+v)/2 + c*(u+w)/2 - s*(v-t)/2;
          work[ioff1+nwork/4+i*m+j] = (u-w)/2 + c*(v-t)/2 + s*(u+w)/2;
        }
      }

    } else {

      for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2;i++) {
          is = i;
          it = n/2-i;
          if (i == 0 || i == n/2) {
            is = 0;
            it = 0;
          }
          t = work[ioff2        +is*m+j];
          u = work[ioff2+nwork/4+is*m+j];
          v = work[ioff2        +it*m+j];
          w = work[ioff2+nwork/4+it*m+j];
          c = table[itable+i];
          s = table[itable+i+n];
          work[ioff1        +i*m+j] = (t+v)/2 + c*(u+w)/2 - s*(v-t)/2;
          work[ioff1+nwork/4+i*m+j] = (u-w)/2 + c*(v-t)/2 + s*(u+w)/2;
        }
      }

    }

  }

  *ioff = ioff1;

}
