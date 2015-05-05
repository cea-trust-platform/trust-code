/* $Header: /home/teuler/cvsroot/lib/jmscm1dxy.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

/* Variante de jmscm1d ou on fournit x en entree et en sortie */
#include "jmfft.h"
#include "jmfft2.h"

void jmscm1dxy(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,x,dimx,debx,incx,jumpx,y,dimy,deby,incy,jumpy,isign,scale)


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
  REAL8 scale;
  REAL8 x[];
  REAL8 y[];
  INTEGER isign;

{

 /* Variables locales */

  INTEGER i, j;
  REAL8 t, u, v, w;
  REAL8 c, s;
  INTEGER is, it;
  INTEGER ioff;

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
          work[        i*m/2+j] = x[debx+i*incx+(j)    *jumpx];
          work[nwork/4+i*m/2+j] = x[debx+i*incx+(j+m/2)*jumpx];
        }
      }

    } else {

      for (j=0;j<=m/2-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n-1;i++) {
          work[        i*m/2+j] = x[debx+i*incx+(j)    *jumpx];
          work[nwork/4+i*m/2+j] = x[debx+i*incx+(j+m/2)*jumpx];
        }
      }

    }

    /* On fait m/2 t.f. complexes de longueur n */
    ioff = 0;
    jmccm1d(m/2,n,fact,nfact,ifact,table,ntable,itable,work,nwork,&ioff);

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
          t = work[ioff        + i*m/2+j];
          u = work[ioff+nwork/4+ i*m/2+j];
          v = work[ioff        +it*m/2+j];
          w = work[ioff+nwork/4+it*m/2+j];
          y[deby+(2*i)  *incy+(j)    *jumpy] =       scale*(t+v)/2;
          y[deby+(2*i+1)*incy+(j)    *jumpy] = isign*scale*(u-w)/2;
          y[deby+(2*i)  *incy+(j+m/2)*jumpy] =       scale*(u+w)/2;
          y[deby+(2*i+1)*incy+(j+m/2)*jumpy] = isign*scale*(v-t)/2;
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
          t = work[ioff        + i*m/2+j];
          u = work[ioff+nwork/4+ i*m/2+j];
          v = work[ioff        +it*m/2+j];
          w = work[ioff+nwork/4+it*m/2+j];
          y[deby+(2*i)  *incy+(j)    *jumpy] =       scale*(t+v)/2;
          y[deby+(2*i+1)*incy+(j)    *jumpy] = isign*scale*(u-w)/2;
          y[deby+(2*i)  *incy+(j+m/2)*jumpy] =       scale*(u+w)/2;
          y[deby+(2*i+1)*incy+(j+m/2)*jumpy] = isign*scale*(v-t)/2;
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
          work[        m*i+j] = x[debx+incx*(2*i  )+jumpx*j];
          work[nwork/4+m*i+j] = x[debx+incx*(2*i+1)+jumpx*j];
        }
      }

    } else {

      for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=0;i<=n/2-1;i++) {
          work[        m*i+j] = x[debx+incx*(2*i  )+jumpx*j];
          work[nwork/4+m*i+j] = x[debx+incx*(2*i+1)+jumpx*j];
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
          t = work[ioff        +is*m+j];
          u = work[ioff+nwork/4+is*m+j];
          v = work[ioff        +it*m+j];
          w = work[ioff+nwork/4+it*m+j];
          c = table[itable+i];
          s = table[itable+i+n];
          y[deby+(2*i  )*incy+j*jumpy] = 
                  scale*((t+v)/2 + c*(u+w)/2 - s*(v-t)/2);
          y[deby+(2*i+1)*incy+j*jumpy] = 
            isign*scale*((u-w)/2 + c*(v-t)/2 + s*(u+w)/2);
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
          t = work[ioff        +is*m+j];
          u = work[ioff+nwork/4+is*m+j];
          v = work[ioff        +it*m+j];
          w = work[ioff+nwork/4+it*m+j];
          c = table[itable+i];
          s = table[itable+i+n];
          y[deby+(2*i  )*incy+j*jumpy] = 
                  scale*((t+v)/2 + c*(u+w)/2 - s*(v-t)/2);
          y[deby+(2*i+1)*incy+j*jumpy] = 
            isign*scale*((u-w)/2 + c*(v-t)/2 + s*(u+w)/2);
        }
      }

    }

  }

}
