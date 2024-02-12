/* $Header: /home/teuler/cvsroot/lib/jmccfft.f90,v 6.4 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void ccfft(isign,n,scale,x,y,table,work,isys)


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
  INTEGER ioff;
  INTEGER ntable, nwork;
  INTEGER nfact;
  INTEGER fact[100];
  char nomsp[] = "CCFFT";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);

  /* Gestion de table */
  ntable = 100+2*n;

  /* Gestion de work */
  nwork = 4*n;

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

  /* On copie le tableau d'entree dans le tableau de travail */
  /* On en profite pour premultiplier et pour tenir compte du signe */
  for (i=0;i<=n-1;i++) {
    work[i]   =       scale* x[2*i];
    work[n+i] = isign*scale* x[2*i+1];
  }
  ioff = 0;

  /* On appelle le sous-programme principal */
  jmccm1d(1,n,fact,100,0,table,ntable,100+0,work,nwork,&ioff);

  /* On recopie dans le tableau d'arrivee */
  for (i=0;i<=n-1;i++) {
    y[2*i]   =         work[ioff  +i];
    y[2*i+1] = isign * work[ioff+n+i];
  }

}
