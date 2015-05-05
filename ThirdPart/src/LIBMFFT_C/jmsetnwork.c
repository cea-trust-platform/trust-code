/* $Header: /home/teuler/cvsroot/lib/jmsetnwork.f90,v 6.2 2000/02/18 17:23:39 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmsetnwork(nwork)

  /* Subroutine appelee par l'utilisateur pour augmenter le nwork */
  /* des routines 2d et 3d */


  /* Arguments */
  INTEGER nwork;

{

 /* Variables locales */

  char nomsp[] = "JMSETNWORK";
  INTEGER nwork2;

  if (nwork <= 0) {
    jmerreur1(nomsp,4,nwork);
  }

  nwork2 = nwork;
  jmgetsetnwork(&nwork2,"s");

}
