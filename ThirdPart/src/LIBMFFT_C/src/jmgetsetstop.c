/* $Header: /home/teuler/cvsroot/lib/jmgetsetstop.f90,v 6.2 2000/03/10 11:57:58 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmgetsetstop(arret,type)

  /* Subroutine qui permet de stocker une valeur statique */
  /* Ceci evite de recourir a un common ... */


  /* Arguments */
  INTEGER *arret;
  char type[1];

{

 /* Variables locales */


  /* Variable statique */
  static INTEGER arret_last = 1;

  if (strcmp(type,"s") == 0) {
    arret_last = *arret;
  } else if (strcmp(type,"g") == 0) {
    *arret = arret_last;
  }

}
