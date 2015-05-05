/* $Header: /home/teuler/cvsroot/lib/jmcsm1dxy.f90,v 6.2 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

/* Variante de jmcsm1d ou on fournit x en entree et en sortie */
#include "jmfft.h"
#include "jmfft2.h"

void jmcsm1dxy(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,x,dimx,debx,incx,jumpx,y,dimy,deby,incy,jumpy,isign,scale)


  /* Arguments */
  INTEGER m, n;
  INTEGER nfact, ifact;
  INTEGER fact[];
  INTEGER ntable,itable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER dimx, debx, incx, jumpx;
  INTEGER dimy, deby, incy, jumpy;
  REAL8 x[];
  REAL8 y[];
  INTEGER isign;
  REAL8 scale;

{

 /* Variables locales */

  INTEGER i, j;
  REAL8 t, u, v, w, tt, uu, vv, ww;
  REAL8 c, s;
  INTEGER it;
  INTEGER ioff;

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
          t =       scale*x[debx+incx*(2*i  )+jumpx*(j    )];
          u = isign*scale*x[debx+incx*(2*i+1)+jumpx*(j    )];
          v =       scale*x[debx+incx*(2*i  )+jumpx*(j+m/2)];
          w = isign*scale*x[debx+incx*(2*i+1)+jumpx*(j+m/2)];
          work[         i*m/2+j] = (t-w);
          work[nwork/4+ i*m/2+j] = (u+v);
          work[        it*m/2+j] = (t+w);
          work[nwork/4+it*m/2+j] = (v-u);
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
          t =       scale*x[debx+incx*(2*i  )+jumpx*(j    )];
          u = isign*scale*x[debx+incx*(2*i+1)+jumpx*(j    )];
          v =       scale*x[debx+incx*(2*i  )+jumpx*(j+m/2)];
          w = isign*scale*x[debx+incx*(2*i+1)+jumpx*(j+m/2)];
          work[         i*m/2+j] = (t-w);
          work[nwork/4+ i*m/2+j] = (u+v);
          work[        it*m/2+j] = (t+w);
          work[nwork/4+it*m/2+j] = (v-u);
        }
      }

    }

    /* On fait m/2 t.f. complexes -> complexes de longueur n */
    ioff = 0;
    jmccm1d(m/2,n,fact,nfact,ifact,table,ntable,itable,work,nwork,&ioff);

    /* On reconstitue */

    if (m/2 >= 16 || n < 8) {

      for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m/2-1;j++) {
          y[deby+jumpy*(j    )+incy*i] = work[ioff        +i*m/2+j];
          y[deby+jumpy*(j+m/2)+incy*i] = work[ioff+nwork/4+i*m/2+j];
        }
      }

    } else {

      for (j=0;j<=m/2-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n-1;i++) {
          y[deby+jumpy*(j    )+incy*i] = work[ioff        +i*m/2+j];
          y[deby+jumpy*(j+m/2)+incy*i] = work[ioff+nwork/4+i*m/2+j];
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
          t =        scale*x[debx+(2*i)  *incx+j*jumpx];
          u = -isign*scale*x[debx+(2*i+1)*incx+j*jumpx];
          v =        scale*x[debx+(2*(n/2-i)  )*incx+j*jumpx];
          w = -isign*scale*x[debx+(2*(n/2-i)+1)*incx+j*jumpx];
          c = table[itable+i];
          s = table[itable+i+n];
          tt = (t+v)/2;
          uu = (u-w)/2;
          vv = (c*(t-v)+s*(u+w))/2;
          ww = (c*(u+w)-s*(t-v))/2;
          /* Note : le facteur 2 et le signe - viennent de l'inversion Fourier */
          work[        m*i+j] =  2*(tt-ww);
          work[nwork/4+m*i+j] = -2*(uu+vv);
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
          t =        scale*x[debx+(2*i)  *incx+j*jumpx];
          u = -isign*scale*x[debx+(2*i+1)*incx+j*jumpx];
          v =        scale*x[debx+(2*(n/2-i)  )*incx+j*jumpx];
          w = -isign*scale*x[debx+(2*(n/2-i)+1)*incx+j*jumpx];
          c = table[itable+i];
          s = table[itable+i+n];
          tt = (t+v)/2;
          uu = (u-w)/2;
          vv = (c*(t-v)+s*(u+w))/2;
          ww = (c*(u+w)-s*(t-v))/2;
          /* Note : le facteur 2 et le signe - viennent de l'inversion Fourier */
          work[        m*i+j] =  2*(tt-ww);
          work[nwork/4+m*i+j] = -2*(uu+vv);
        }
      }

    }

    /* On fait m t.f. complexes de taille n/2 */
    ioff = 0;
    fact[ifact+1] = fact[ifact+1]/2; /* Revient a remplacer n2 par n2/2*/
    fact[ifact+2] = fact[ifact+2]-1; /* Revient a remplacer p2 par p2-1*/
    jmccm1d(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,&ioff);
    fact[ifact+1] = fact[ifact+1]*2; /* On retablit les valeurs initiales*/
    fact[ifact+2] = fact[ifact+2]+1;

    /* On reconstitue */

    if (m >= 16 || n/2 < 8) {

      for (i=0;i<=n/2-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=0;j<=m-1;j++) {
          /* Note : le signe - vient de l'inversion */
          y[deby+incy*(2*i  )+jumpy*j] =  work[ioff        +m*i+j];
          y[deby+incy*(2*i+1)+jumpy*j] = -work[ioff+nwork/4+m*i+j];
        }
      }

    } else {

#pragma _CRI ivdep
      for (j=0;j<=m-1;j++) {
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2-1;i++) {
          /* Note : le signe - vient de l'inversion */
          y[deby+incy*(2*i  )+jumpy*j] =  work[ioff        +m*i+j];
          y[deby+incy*(2*i+1)+jumpy*j] = -work[ioff+nwork/4+m*i+j];
        }
      }

    }

  }

}
