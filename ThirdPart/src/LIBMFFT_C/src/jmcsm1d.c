/* $Header: /home/teuler/cvsroot/lib/jmcsm1d.f90,v 6.2 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmcsm1d(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,ioff)


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
  REAL8 t, u, v, w, tt, uu, vv, ww;
  REAL8 c, s;
  INTEGER it;

  /* Gestion de work */
  ioff1 = *ioff;
  ioff2 = nwork/2 - ioff1;

  /* On doit faire m T.F. complexes -> reelles de longueur n */
  /* Si m est pair */
  if ((m%2) == 0) {

    /* On distribue */

    if (m/2 >= 16 || n/2 < 8) {

      for (i=0;i<=n/2;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m/2-1;j++) {
          it = n-i;
          if (i == 0) it = 0;
          t = work[ioff1        +i*m+j    ];
          u = work[ioff1+nwork/4+i*m+j    ];
          v = work[ioff1        +i*m+j+m/2];
          w = work[ioff1+nwork/4+i*m+j+m/2];
          work[ioff2        + i*m/2+j] = (t-w);
          work[ioff2+nwork/4+ i*m/2+j] = (u+v);
          work[ioff2        +it*m/2+j] = (t+w);
          work[ioff2+nwork/4+it*m/2+j] = (v-u);
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
          t = work[ioff1        +i*m+j    ];
          u = work[ioff1+nwork/4+i*m+j    ];
          v = work[ioff1        +i*m+j+m/2];
          w = work[ioff1+nwork/4+i*m+j+m/2];
          work[ioff2        + i*m/2+j] = (t-w);
          work[ioff2+nwork/4+ i*m/2+j] = (u+v);
          work[ioff2        +it*m/2+j] = (t+w);
          work[ioff2+nwork/4+it*m/2+j] = (v-u);
        }
      }

    }

    /* On fait m/2 t.f. complexes -> complexes de longueur n */
    jmccm1d(m/2,n,fact,nfact,ifact,table,ntable,itable,work,nwork,&ioff2);
    ioff1 = nwork/2 - ioff2;

    /* On reconstitue */

    if (m/2 >= 16 || n < 8) {

      for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m/2-1;j++) {
          work[ioff1+i*m+j    ] = work[ioff2        +i*m/2+j];
          work[ioff1+i*m+j+m/2] = work[ioff2+nwork/4+i*m/2+j];
        }
      }

    } else {

      for (j=0;j<=m/2-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n-1;i++) {
          work[ioff1+i*m+j    ] = work[ioff2        +i*m/2+j];
          work[ioff1+i*m+j+m/2] = work[ioff2+nwork/4+i*m/2+j];
        }
      }

    }

  /* Si m n'est pas pair mais que n l'est */
  } else if ((n%2) == 0) {

    /* On distribue */

    if (m >= 16 || n/2 < 8) {

      for (i=0;i<=n/2-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m-1;j++) {
          /* Note : Signe - sur les parties imaginaires pour inversion */
          t =  work[ioff1        +      i*m+j];
          u = -work[ioff1+nwork/4+      i*m+j];
          v =  work[ioff1        +(n/2-i)*m+j];
          w = -work[ioff1+nwork/4+(n/2-i)*m+j];
          c = table[itable+i];
          s = table[itable+i+n];
          tt = (t+v)/2;
          uu = (u-w)/2;
          vv = (c*(t-v)+s*(u+w))/2;
          ww = (c*(u+w)-s*(t-v))/2;
          /* Note : le facteur 2 et le signe - viennent de l'inversion Fourier */
          work[ioff2        +m*i+j] =  2*(tt-ww);
          work[ioff2+nwork/4+m*i+j] = -2*(uu+vv);
        }
      }

    } else {

      for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2-1;i++) {
          /* Note : Signe - sur les parties imaginaires pour inversion */
          t =  work[ioff1        +      i*m+j];
          u = -work[ioff1+nwork/4+      i*m+j];
          v =  work[ioff1        +(n/2-i)*m+j];
          w = -work[ioff1+nwork/4+(n/2-i)*m+j];
          c = table[itable+i];
          s = table[itable+i+n];
          tt = (t+v)/2;
          uu = (u-w)/2;
          vv = (c*(t-v)+s*(u+w))/2;
          ww = (c*(u+w)-s*(t-v))/2;
          /* Note : le facteur 2 et le signe - viennent de l'inversion Fourier */
          work[ioff2        +m*i+j] =  2*(tt-ww);
          work[ioff2+nwork/4+m*i+j] = -2*(uu+vv);
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

    /* On reconstitue */

    if (m >= 16 || n/2 < 8) {

      for (i=0;i<=n/2-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m-1;j++) {
          /* Note : le signe - vient de l'inversion */
          work[ioff1+m*(2*i  )+j] =  work[ioff2        +m*i+j];
          work[ioff1+m*(2*i+1)+j] = -work[ioff2+nwork/4+m*i+j];
        }
      }

    } else {

      for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2-1;i++) {
          /* Note : le signe - vient de l'inversion */
          work[ioff1+m*(2*i  )+j] =  work[ioff2        +m*i+j];
          work[ioff1+m*(2*i+1)+j] = -work[ioff2+nwork/4+m*i+j];
        }
      }

    }

  }

  *ioff = ioff1;

}
