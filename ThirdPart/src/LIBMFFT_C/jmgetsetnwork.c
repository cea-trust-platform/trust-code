/* $Header: /home/teuler/cvsroot/lib/jmgetsetnwork.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmgetsetnwork(nwork,type)

  /* Subroutine qui permet de stocker une valeur statique */
  /* Ceci evite de recourir a un common ... */


  /* Arguments */
  INTEGER *nwork;
  char type[1];

{

 /* Variables locales */


  /* Variable statique */
  static INTEGER nwork_last = -1;

  if (strcmp(type,"s") == 0) {
    nwork_last = *nwork;
  } else if (strcmp(type,"g") == 0) {
    *nwork = nwork_last;
  }

}
