/* $Header: /home/teuler/cvsroot/lib/jmcsfftm.f90,v 6.4 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void csfftm(isign,n,m,scale,x,ldx,y,ldy,table,work,isys)


  /* Arguments */
  INTEGER isign;
  INTEGER m, n, ldx, ldy;
  REAL8 scale;
  REAL8 x[];
  REAL8 y[];
  REAL8 table[];
  REAL8 work[];
  INTEGER isys;

{

 /* Variables locales */

  INTEGER i;
  INTEGER ntable, nwork;
  INTEGER nfact;
  INTEGER fact[100];
  INTEGER dimx, debx, incx, jumpx;
  INTEGER dimy, deby, incy, jumpy;
  char nomsp[] = "CSFFTM";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);
  if (ldx < n/2+1) jmerreur2(nomsp,10,ldx,n/2+1);
  if (ldy < n    ) jmerreur2(nomsp,14,ldy,n    );
  if ((n%2) != 0 && (m%2) != 0) 
    jmerreur2(nomsp,22,n,m);

  /* Gestion de table */
  ntable = 100+2*n;

  /* Gestion de work */
  nwork = 2*n*m;

  /* Test sur isign */
  if (isign == 0) {
    /* Pour la factorisation */
    jmfact(n,fact,100,    0,&nfact);
    for (i=0;i<nfact;i++) { table[i] = fact[i]; }
    /* Pour les sinus et cosinus */
    jmtable(table,ntable,100+0,n);
    return;
  } else {
    nfact = nint(table[0]);
    for (i=0;i<nfact;i++) { fact[i] = nint(table[i]); }
  }

  /* On appelle le sous-programme principal */
  dimx = 2*ldx*m ; debx = 0 ; incx = 1 ; jumpx = 2*ldx;
  dimy = ldy*m   ; deby = 0 ; incy = 1 ; jumpy = ldy;
  jmcsm1dxy(m,n,fact,100,0,table,ntable,100+0,work,nwork, 
    x,dimx,debx,incx,jumpx,y,dimy,deby,incy,jumpy,isign,scale);

}
