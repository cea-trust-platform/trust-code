/* $Header: /home/teuler/cvsroot/lib/jmgetnwork.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmgetnwork(nwork,nwork_def,nwork_min)

  /* On recupere la valeur de nwork si elle a ete augmentee par l'utilisateur */
  /* Sinon on prend la valeur par defaut */
  /* Il s'agit du nwork des routines 2d et 3d */


  /* Arguments */
  INTEGER *nwork;
  INTEGER nwork_def, nwork_min;

{

 /* Variables locales */

  INTEGER nwork_loc;
  char nomsp[] = "JMGETNWORK";

  jmgetsetnwork(&nwork_loc,"g");

  /* Valeur par defaut */
  if (nwork_loc == -1) {
    *nwork = nwork_def;
  /* Valeur invalide (trop petite) */
  } else if (nwork_loc < nwork_min) {
    jmerreur2(nomsp,5,nwork_loc,nwork_min);
  /* Valeur correcte */
  } else {
    *nwork = nwork_loc;
  }

}
