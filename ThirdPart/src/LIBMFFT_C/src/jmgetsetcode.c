/* $Header: /home/teuler/cvsroot/lib/jmgetsetcode.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmgetsetcode(code,type)

  /* Subroutine qui permet de stocker le dernier code de retour obtenu */
  /* Ceci evite de recourir a un common ... */


  /* Arguments */
  INTEGER *code;
  char type[1];

{

 /* Variables locales */


  /* Variable statique */
  static INTEGER code_last = 0;

  if (strcmp(type,"s") == 0) {
    code_last = *code;
  } else if (strcmp(type,"g") == 0) {
    *code = code_last;
  }

}
