/* $Header: /home/teuler/cvsroot/lib/jmccfftm.f90,v 6.5 2000/03/01 17:39:46 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void ccfftm(isign,n,m,scale,x,ldx,y,ldy,table,work,isys)


  /* Arguments */
  INTEGER isign;
  INTEGER n, m, ldx, ldy;
  REAL8 scale;
  REAL8 x[];
  REAL8 y[];
  REAL8 table[];
  REAL8 work[];
  INTEGER isys;

{

 /* Variables locales */

  INTEGER i, j;
  INTEGER ioff;
  INTEGER ntable, nwork;
  INTEGER nfact;
  INTEGER fact[100];
  char nomsp[] = "CCFFTM";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);
  if (ldx < n) jmerreur2(nomsp,9,ldx,n);
  if (ldy < n) jmerreur2(nomsp,14,ldy,n);

  /* Gestion de table */
  ntable = 100+2*n;

  /* Gestion de work */
  nwork = 4*n*m;

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
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
    for (j=0;j<=m-1;j++) {
      work[j+m*i]     =       scale*x[2*i  +2*ldx*j];
      work[j+m*(n+i)] = isign*scale*x[2*i+1+2*ldx*j];
    }
  }

  /* On appelle le sous-programme principal */
  ioff = 0;
  jmccm1d(m,n,fact,100,0,table,ntable,100+0,work,nwork,&ioff);

  /* On recopie le tableau de travail dans le tableau de sortie */
  for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
    for (j=0;j<=m-1;j++) {
      y[2*i  +2*ldy*j] =       work[ioff+j+m*i];
      y[2*i+1+2*ldy*j] = isign*work[ioff+j+m*(n+i)];
    }
  }

}
