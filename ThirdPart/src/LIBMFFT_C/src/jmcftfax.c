/* $Header: /home/teuler/cvsroot/lib/jmcftfax.f90,v 6.4 2000/02/08 11:49:13 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void cftfax(n,ifax,trigs)


  /* Constantes pour les arguments */
#define nfax 19

  /* Arguments */
  INTEGER n;
  REAL8 trigs[];
  INTEGER ifax[];

{

 /* Variables locales */

  INTEGER ntrigs ;
  INTEGER ifin;
  char nomsp[] = "CFTFAX";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  ntrigs = 2*n;

  /* Factorisation de n dans ifax */
  jmfact(n,ifax,nfax,0,&ifin);

  /* Preparation des tables */
  jmtable(trigs,ntrigs,0,n);

}
