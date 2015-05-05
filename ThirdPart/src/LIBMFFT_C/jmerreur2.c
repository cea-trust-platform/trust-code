/* $Header: /home/teuler/cvsroot/lib/jmerreur2.f90,v 6.3 2000/03/10 11:57:58 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmerreur2(nomsp,code,var1,var2)


  /* Arguments */
  char nomsp[];
  INTEGER code;
  INTEGER var1, var2;

{

 /* Variables locales */

  INTEGER arret;
  char message[80];

  jmgetstop(&arret);
  if (arret == 1) {
    jmgetmessage(code,message);
    printf("JMFFT Erreur dans %s : %s "
      "(%d %d)\n",nomsp,message,var1,var2);
    exit(1);
  } else {
    jmsetcode(code);
  }

}
