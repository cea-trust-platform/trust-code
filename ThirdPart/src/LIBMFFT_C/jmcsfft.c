/* $Header: /home/teuler/cvsroot/lib/jmcsfft.f90,v 6.4 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void csfft(isign,n,scale,x,y,table,work,isys)


  /* Arguments */
  INTEGER isign;
  INTEGER n;
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
  char nomsp[] = "CSFFT";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if ((n%2) != 0) jmerreur1(nomsp,24,n);

  /* Gestion de table */
  ntable = 100+2*n;

  /* Gestion de work */
  nwork = 2*n;

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
  dimx = 2*(n/2)+2 ; debx = 0 ; incx = 1 ; jumpx = 0;
  dimy = n         ; deby = 0 ; incy = 1 ; jumpy = 0;
  jmcsm1dxy(1,n,fact,100,0,table,ntable,100+0,work,nwork, 
    x,dimx,debx,incx,jumpy,y,dimy,deby,incy,jumpy,isign,scale);

}
