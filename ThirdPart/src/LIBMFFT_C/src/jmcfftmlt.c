/* $Header: /home/teuler/cvsroot/lib/jmcfftmlt.f90,v 6.4 2000/02/08 11:49:13 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void cfftmlt(xr,xi,work,trigs,ifax,inc,jump,n,m,isign)


  /* Constantes pour les arguments */
#define nfax 19

  /* Arguments */
  INTEGER inc, jump, n, m, isign;
  REAL8 xr[], xi[];
  REAL8 work[];
  REAL8 trigs[];
  INTEGER ifax[];

{

 /* Variables locales */

  INTEGER ntrigs, nwork;
  INTEGER ioff;
  INTEGER i, j;
  char nomsp[] = "CFFTMLT";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign !=-1 && isign != 1) 
    jmerreur1(nomsp,1,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);

  /* Gestion de table */
  ntrigs = 2*n;

  /* Gestion de work */
  nwork = 4*n*m;

  if (isign == 1) {

    for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (j=0;j<=m-1;j++) {
        work[j+m*i]     = xr[jump*j+inc*i];
        work[j+m*(n+i)] = xi[jump*j+inc*i];
      }
    }

  } else {

    for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (j=0;j<=m-1;j++) {
        work[j+m*i]     =  xr[jump*j+inc*i];
        work[j+m*(n+i)] = -xi[jump*j+inc*i];
      }
    }

  }

  /* On appelle le sous-programme principal */
  ioff = 0;
  jmccm1d(m,n,ifax,nfax,0,trigs,ntrigs,0,work,nwork,&ioff);

  /* On recopie le tableau de travail dans le tableau de sortie */
  if (isign == 1) {

    for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (j=0;j<=m-1;j++) {
        xr[jump*j+inc*i] = work[ioff+j+m*i];
        xi[jump*j+inc*i] = work[ioff+j+m*(n+i)];
      }
    }

  } else {

    for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (j=0;j<=m-1;j++) {
        xr[jump*j+inc*i] =  work[ioff+j+m*i];
        xi[jump*j+inc*i] = -work[ioff+j+m*(n+i)];
      }
    }

  }

}
