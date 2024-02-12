/* $Header: /home/teuler/cvsroot/lib/jmccm1dp.f90,v 6.1 1999/10/22 08:35:19 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void jmccm1dp(p,q,m,table,ntable,itable,ntable2,mtable,work,nwork,ioff)

  /* On fait m t.f. d'ordre q en base p (p**q) */
  /* Note : n n'est pas utilise ici */


  /* Arguments */
  INTEGER p, q, m;
  INTEGER ntable,itable,ntable2, mtable;
  REAL8 table[];
  INTEGER nwork;
  REAL8 work[];
  INTEGER *ioff;

{

 /* Variables locales */

  INTEGER i, k, jl, jp, kp;
  REAL8 ck, sk, tk, uk, cpjk, spjk;
  INTEGER pqq, pi, pqi, pqii;
  INTEGER ikpr, ikpi, ijpr, ijpi;
  INTEGER itr, iti, jtr, jti;
  INTEGER ioff1, ioff2;
  REAL8 c11, c12, c21, c22;

  /* On joue sur ioff pour alterner entre le haut et le bas de work */
  ioff1 = *ioff;
  ioff2 = nwork/2-ioff1;

  /* Pour le calcul des cos(2*pi/p) */
  pqq = ipow(p,(q-1));

  /* Boucle sur les etapes */
  for (i=0;i<=q-1;i++) {

    pi   = ipow(p,i);
    pqi  = ipow(p,(q-i));
    pqii = ipow(p,(q-i-1));

    for (k=0;k<=pi-1;k++) {

      for (jp=0;jp<=p-1;jp++) {

        for (jl=0;jl<=m*pqii-1;jl++) {

          ijpr = ioff2 + jl + m*((k+jp*pi)*pqii);
          ijpi = ijpr + nwork/4;

          work[ijpr] = 0;
          work[ijpi] = 0;

        }

      }

      for (kp=0;kp<=p-1;kp++) {

        itr = itable+mtable*kp*pqii*k;
        iti = itr + ntable2;
        ck = table[itr];
        sk = table[iti];

        for (jp=0;jp<=p-1;jp++) {

          /* Gymanstique infernale pour recuperer cos(2*pi/p) etc */
          jtr = itable+mtable*pqq*(jp*kp%p);
          jti = jtr + ntable2;
          cpjk = table[jtr];
          spjk = table[jti];
          c11 = (cpjk*ck-spjk*sk);
          c12 = (cpjk*sk+spjk*ck);
          c21 = (cpjk*sk+spjk*ck);
          c22 = (cpjk*ck-spjk*sk);

#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
          for (jl=0;jl<=m*pqii-1;jl++) {

            ikpr = ioff1+jl+m*(k*pqi+kp*pqii);
            ikpi = ikpr + nwork/4;
            tk = work[ikpr];
            uk = work[ikpi];

            ijpr = ioff2+jl+m*((k+jp*pi)*pqii);
            ijpi = ijpr + nwork/4;

            work[ijpr] = work[ijpr] + tk*c11-uk*c12;
            work[ijpi] = work[ijpi] + tk*c21+uk*c22;

          }

        }

      }

    }

    /* On alterne les offsets */
    ioff1 = nwork/2 - ioff1;
    ioff2 = nwork/2 - ioff2;

  /* Fin boucle sur le nombre d'etapes */
  }

  *ioff = ioff1;

}
