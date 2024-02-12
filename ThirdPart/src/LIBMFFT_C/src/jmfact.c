/* $Header: /home/teuler/cvsroot/lib/jmfact.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmfact(n,fact,nfact,ideb,ifin)


  /* Arguments */
  INTEGER n, nfact, ideb;
  INTEGER *ifin;
  INTEGER fact[];

{

 /* Variables locales */

  INTEGER m;
  INTEGER n2, p2, n3, p3, n5, p5;
  char nomsp[] = "JMFACT";
  /* Nombres premiers */
#define npremiers 7
  INTEGER premiers[npremiers-1+1] = {7,11,13,17,19,23,29};
  INTEGER ip, premier, pp, np;

  m = n;

  /* Etude des puissances de deux */
  p2 = 0;
  n2 = 1;
  while(1) {
    if ((m%2) == 0) {
      p2 = p2+1;
      n2 = n2*2;
      m  = m/2;
    } else {
      break;
    }
  }
  *ifin = ideb+3;
  if (*ifin > nfact) 
    jmerreur2(nomsp,7,nfact,*ifin);
  fact[ideb+1] = n2;
  fact[ideb+2] = p2;

  /* Etude des puissances de trois */
  p3 = 0;
  n3 = 1;
  while(1) {
    if ((m%3) == 0) {
      p3 = p3+1;
      n3 = n3*3;
      m  = m/3;
    } else {
      break;
    }
  }
  *ifin = *ifin+2;
  if (*ifin > nfact) 
    jmerreur2(nomsp,7,nfact,*ifin);
  fact[ideb+3] = n3;
  fact[ideb+4] = p3;

  /* Etude des puissances de cinq */
  p5 = 0;
  n5 = 1;
  while(1) {
    if ((m%5) == 0) {
      p5 = p5+1;
      n5 = n5*5;
      m  = m/5;
    } else {
      break;
    }
  }
  *ifin = *ifin+2;
  if (*ifin > nfact) 
    jmerreur2(nomsp,7,nfact,*ifin);
  fact[ideb+5] = n5;
  fact[ideb+6] = p5;

  /* On met a jour le nombre de termes */
  fact[ideb] = 7;

  /* Si on a fini */
  if (n2*n3*n5 == n) return;

  /* Il reste maintenant des facteurs premiers bizarres */
  /* On va boucler tant qu'on n'a pas fini ou tant qu'on n'a pas epuise la liste */

  for (ip=0;ip<=npremiers-1;ip++) {

    premier = premiers[ip];

    pp = 0;
    np = 1;
    while(1) {
      if ((m%premier) == 0) {
        pp = pp+1;
        np = np*premier;
        m  = m/premier;
      } else {
        break;
      }
    }
    *ifin = *ifin+2;
    if (*ifin > nfact) 
      jmerreur2(nomsp,7,nfact,*ifin);
    fact[*ifin-2] = pp;
    fact[*ifin-1] = premier;
    fact[ideb] = fact[ideb] + 2;

    /* Si le nombre est completement factorise, inutile de continuer */
    if (m == 1) break;

  }

  /* On regarde si la factorisation est terminee */
  if (m == 1) {
    return;
  } else {
    jmerreur1(nomsp,3,n);
  }
  
}
