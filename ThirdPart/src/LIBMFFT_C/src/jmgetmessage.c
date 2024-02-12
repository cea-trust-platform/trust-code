/* $Header: /home/teuler/cvsroot/lib/jmgetmessage.f90,v 6.2 2000/02/08 11:37:54 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmgetmessage(code,message)


  /* Arguments */
  INTEGER code;
  char message[];

{

 /* Variables locales */

#define mm 26
  char *messages[] = {
    "Pas d'erreur",
    "Isign doit etre egal a -1 ou 1",
    "Isign doit etre egal a 0, -1 ou 1",                
    "Nombres premiers trop grands",
    "Nwork negatif ou nul",
    "Nwork trop petit",
    "Tronconnage impossible",
    "Trop de facteurs premiers",
    "l doit etre >= 1",
    "ldx doit etre >= n",
    "ldx doit etre >= n/2+1",
    "ldx1 doit etre >= n",
    "ldx1 doit etre >= n/2+1",
    "ldx2 doit etre >= m",
    "ldy doit etre >= n",
    "ldy doit etre >= n+2",
    "ldy doit etre >= n/2+1",
    "ldy1 doit etre >= n",
    "ldy1 doit etre >= n+2",
    "ldy1 doit etre >= n/2+1",
    "ldy2 doit etre >= m",
    "m doit etre >= 1",
    "m ou n doit etre pair",
    "n doit etre >= 1",
    "n doit etre pair",
    "n ou m ou l doit etre pair"
    };

  if (code < 0 || code >= mm) {
    printf("JMFFT GETMESSAGE Code invalide : %d\n",code);
  }

  message = messages[code];

}
