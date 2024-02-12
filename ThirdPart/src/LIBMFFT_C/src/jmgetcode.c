/* $Header: /home/teuler/cvsroot/lib/jmgetcode.f90,v 6.2 2000/02/08 11:37:54 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmgetcode(code)


  /* Arguments */
  INTEGER *code;

{

 /* Variables locales */


  jmgetsetcode(code,"g");

}
