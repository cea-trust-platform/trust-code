/* $Header: /home/teuler/cvsroot/lib/jmrfftmlt.f90,v 6.5 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void rfftmlt(x,work,trigs,ifax,incx,jumpx,n,m,isign)


  /* Constantes pour les arguments */
#define nfax 19

  /* Arguments */
  INTEGER incx, jumpx, n, m, isign;
  REAL8 x[];
  REAL8 work[];
  REAL8 trigs[];
  INTEGER ifax[];

{

 /* Variables locales */

  INTEGER ntrigs, nwork;
  REAL8 scale;
  INTEGER dimx, debx;
  INTEGER signe;
  REAL8 scale_temp;
  char nomsp[] = "RFFTMLT";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign !=-1 && isign != 1) 
    jmerreur1(nomsp,1,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);
  if ((n%2) != 0 && (m%2) != 0) 
    jmerreur2(nomsp,22,n,m);

  /* Gestion de trigs */
  ntrigs = 2*n;

  /* Gestion de work */
  nwork = 2*n*m;

  if (isign == -1) {

    /* On appelle le sous-programme principal */
    scale = ((REAL8)(1))/((REAL8)(n));
    dimx = m*(n+2);
    debx = 0;
    signe = -1;
    jmscm1dxy(m,n,ifax,nfax,0,trigs,ntrigs,0,work,nwork, 
      x,dimx,debx,incx,jumpx,x,dimx,debx,incx,jumpx,signe,scale);

  } else {

    /* On appelle le sous-programme principal */
    dimx = m*(n+2);
    debx = 0;
    signe = 1;
    scale_temp = ((REAL8)(1));
    jmcsm1dxy(m,n,ifax,nfax,0,trigs,ntrigs,0,work,nwork, 
      x,dimx,debx,incx,jumpx,x,dimx,debx,incx,jumpx,signe,scale_temp);

  }

}
