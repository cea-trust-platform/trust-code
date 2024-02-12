/* $Header: /home/teuler/cvsroot/lib/jmtransp.f90,v 6.1 1999/10/22 08:35:20 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmtransp(n,m,l,work,nwork,ioff)


  /* Arguments */
  INTEGER n, m, l;
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER ioff1, ioff2;
  INTEGER ij, k;

  ioff1 = *ioff;
  ioff2 = nwork/2-ioff1;

  /* On transpose (nm)(l) en (l)(nm) en distinguant les parties reelles et im. */
  if (m*n >= 16 || l < 8) {

    for (k=0;k<=l-1;k++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (ij=0;ij<=m*n-1;ij++) {
        work[ioff2+      ij*l+k] = work[ioff1+      k*n*m+ij];
        work[ioff2+n*m*l+ij*l+k] = work[ioff1+n*m*l+k*n*m+ij];
      }
    }

  } else {

    for (ij=0;ij<=m*n-1;ij++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (k=0;k<=l-1;k++) {
        work[ioff2+      ij*l+k] = work[ioff1+      k*n*m+ij];
        work[ioff2+n*m*l+ij*l+k] = work[ioff1+n*m*l+k*n*m+ij];
      }
    }

  }

  *ioff = ioff2;

}
