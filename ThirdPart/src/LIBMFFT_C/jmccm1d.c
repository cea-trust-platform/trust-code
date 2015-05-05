/* $Header: /home/teuler/cvsroot/lib/jmccm1d.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmccm1d(m,n,fact,nfact,ifact,table,ntable,itable,work,nwork,ioff)


  /* Arguments */
  INTEGER m, n;
  INTEGER nfact, ifact;
  INTEGER fact[];
  INTEGER ntable,itable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER nterms;
  INTEGER np, pp, lastnp, premier;
  INTEGER nprod, nprod1, nprod2;
  INTEGER n2, p2, n3, p3, n5, p5;
  INTEGER i;
  static INTEGER copyright = 0;

  /* Comme tout le monde passe par la, on envoie le copyright (une fois) */
  if (!copyright) {
    copyright = 1;
    printf("%s\n"," ");
    printf("%s\n","************************************************************");
    printf("%s\n","* Portable Fourier transforms by JMFFTLIB                  *");
    printf("%s\n","* Author : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) *");
    printf("%s\n","************************************************************");
    printf("%s\n"," ");
  }

  /* On recupere les facteurs */
  nterms = fact[ifact];
  n2 = fact[ifact+1];
  p2 = fact[ifact+2];
  n3 = fact[ifact+3];
  p3 = fact[ifact+4];
  n5 = fact[ifact+5];
  p5 = fact[ifact+6];
  nprod = n2*n3*n5;
  for (i=7;i<=nterms-1;i+=2) {
    nprod = nprod*ipow(fact[ifact+i+1],fact[ifact+i]);
  }

  /* On fait n3*n5 T.F. de n2 (qui est en puissances de 2) */
  if (n2 != 1) {
    jmccm1d2(p2,n2,m*(nprod/n2),table,ntable,itable,n,n/n2,work,nwork,ioff);
  }

  /* On transpose (on tient compte de ioff) en permutant les deux parties */
  /* On en profite pour multiplier par le bon wij */
  if (n2 != 1 && nprod != n2) {
    jmcctranspcs(m,n,n2,nprod/n2,table,ntable,itable,work,nwork,ioff);
  }

  /* On fait n5*n2 T.F. de n3 (en puissances de 3) */
  if (n3 != 1) {
    jmccm1d3(p3,n3,m*(nprod/n3),table,ntable,itable,n,n/n3,work,nwork,ioff);
  }

  /* On transpose (on tient compte de ioff) en permutant les deux parties */
  /* On en profite pour multiplier par le bon wij */
  if (n3 != 1 && nprod != n3) {
    jmcctranspcs(m*n2,n,n3,nprod/(n2*n3), 
      table,ntable,itable,work,nwork,ioff);
  }

  /* On fait n2*n3 T.F. de n5 (en puissances de 5) */
  if (n5 != 1) {
    jmccm1d5(p5,n5,m*(nprod/n5),table,ntable,itable,n,n/n5,work,nwork,ioff);
  }

  /* On transpose s'il y a lieu (si on a fait quelque chose et s'il reste des */
  /* termes a traiter */
  if (n5 != 1 && nprod != n5 && nterms > 7) {
    jmcctranspcs(m*n2*n3,n,n5,nprod/(n2*n3*n5), 
      table,ntable,itable,work,nwork,ioff);
  }
  nprod1 = m*n2*n3;
  nprod2 = n2*n3*n5;
  lastnp = n5;

  /* On passe aux nombres premiers autres que 2, 3 et 5 */
  for (i=7;i<=nterms-1;i+=2) {

    pp = fact[ifact+i];
    premier = fact[ifact+i+1];
    np = ipow(premier,pp);

    jmccm1dp(premier,pp,m*(nprod/np), 
      table,ntable,itable,n,n/np,work,nwork,ioff);

    nprod1 = nprod1 * lastnp;
    nprod2 = nprod2 * np;
    if (np != 1 && nprod != np && nterms > i+1) {
      jmcctranspcs(nprod1,n,np,nprod/nprod2, 
        table,ntable,itable,work,nwork,ioff);
    }
    lastnp = np;

  }

}
