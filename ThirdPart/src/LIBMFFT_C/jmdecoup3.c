/* $Header: /home/teuler/cvsroot/lib/jmdecoup3.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

/* Pour tronconner en dimension 3 de facon a tenir dans le nwork disponible */

#include "jmfft.h"
#include "jmfft2.h"

void jmdecoup3(n,m,nmr,nwork,debut,lpair,ideb,ifin,jdeb,jfin,nmtemp,nwork_temp,fini)


  /* Arguments */
  INTEGER n, m, nmr, nwork;
  INTEGER debut, lpair;
  INTEGER *nmtemp, *nwork_temp;
  INTEGER *ideb, *jdeb;
  INTEGER *ifin, *jfin;
  INTEGER *fini;

{

 /* Variables locales */

  INTEGER ijdeb, ijfin;
  char nomsp[] = "JMDECOUP3";

  /* n*m*nr est l'espace total qu'il faudrait pour work. */
  /* Malheureusement, on n'a que nwork au plus */
  /* On va donc decouper n et m en morceaux pour tenir */

  /* Gestion de debut */
  if (debut) {
    *ideb = 0;
    *jdeb = 0;
  } else {
    if (*ifin < n-1) {
      *ideb = *ifin+1;
      *jdeb = *jfin;
    } else {
      *ideb = 0;
      *jdeb = *jfin+1;
    }
  }

  /* Gestion de nmtemp */
  *nmtemp = nwork/nmr;
  /* Si l impair, on doit eviter que nmtemp soit impair (routine cs et sc) */
  if (!lpair && (*nmtemp%2) != 0) *nmtemp = *nmtemp-1;
  /* Pour simplifier, on passe par des indices 2d */
  ijdeb = *ideb+*jdeb*n;
  ijfin = min(ijdeb+*nmtemp-1,n*m-1);
  *nmtemp = ijfin-ijdeb+1;
  /* On verifie que nmtemp n'est pas nul */
  if (*nmtemp <= 0) {
    jmerreur4(nomsp,6,n,m,nmr,nwork);
  }
  *nwork_temp = *nmtemp*nmr;

  /* On deduit ifin et jfin de ijfin */
  *jfin = ijfin/n;
  *ifin = ijfin-n**jfin;

  /* Gestion de fin */
  if (*ifin == n-1 && *jfin == m-1) {
    *fini = 1;
  } else {
    *fini = 0;
  }

}
