/* $Header: /home/teuler/cvsroot/lib/jmdecoup.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

/* Pour tronconner de facon a tenir dans le nwork disponible */

#include "jmfft.h"
#include "jmfft2.h"

void jmdecoup(n,nr,nwork,debut,mpair,n_temp,ideb,ifin,nwork_temp,fin)


  /* Arguments */
  INTEGER n, nr, nwork;
  INTEGER debut, mpair;
  INTEGER *n_temp, *ideb, *nwork_temp;
  INTEGER *ifin;
  INTEGER *fin;

{

 /* Variables locales */

  char nomsp[] = "JMDECOUP";

  /* n*nr est l'espace total qu'il faudrait pour work. */
  /* Malheureusement, on n'a que nwork au plus */
  /* On va donc decouper n en morceaux pour tenir */

  /* Gestion de debut */
  if (debut) {
    *ideb = 0;
  } else {
    *ideb = *ifin+1;
  }

  /* Gestion de n_temp et ifin */
  *n_temp = nwork/nr;
  /* Si m impair, on doit eviter que n_temp soit impair (routine cs et sc) */
  if (!mpair && (*n_temp%2) != 0) *n_temp = *n_temp-1;
  *ifin = min(*ideb+*n_temp-1,n-1);
  *n_temp = *ifin-*ideb+1;
  /* On verifie que n_temp n'est pas nul */
  if (*n_temp <= 0) {
    jmerreur3(nomsp,6,n,nr,nwork);
  }
  *nwork_temp = *n_temp*nr;

  /* Gestion de fin */
  if (*ifin == n-1) {
    *fin = 1;
  } else {
    *fin = 0;
  }

}
