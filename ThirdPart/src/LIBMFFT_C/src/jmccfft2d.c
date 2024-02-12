/* $Header: /home/teuler/cvsroot/lib/jmccfft2d.f90,v 6.7 2000/03/01 17:39:46 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void ccfft2d(isign,n,m,scale,x,ldx,y,ldy,table,work,isys)


  /* Arguments */
  INTEGER isign;
  INTEGER n, m, ldx, ldy;
  REAL8 scale;
  REAL8 x[];
  REAL8 y[];
  REAL8 table[];
  REAL8 work[];
  INTEGER isys;

{

 /* Variables locales */

  INTEGER i, j;
  INTEGER ioff;
  INTEGER ntable, nwork;
  INTEGER nfact, mfact;
  INTEGER fact[100];
  INTEGER ideb, ifin, jdeb, jfin, n_temp, m_temp, nwork_temp;
  INTEGER debut, fin;
  char nomsp[] = "CCFFT2D";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);
  if (ldx < n) jmerreur2(nomsp,9,ldx,n);
  if (ldy < n) jmerreur2(nomsp,14,ldy,n);

  /* Gestion de table */
  ntable = 100+2*(n+m);

  /* Test sur isign */
  if (isign == 0) {
    /* Pour la factorisation */
    jmfact(n,fact,100,    0,&nfact);
    jmfact(m,fact,100,nfact,&mfact);
    for (i=0;i<mfact;i++) { table[i] = fact[i]; }
    /* Pour les sinus et cosinus */
    jmtable(table,ntable,100+0  ,n);
    jmtable(table,ntable,100+2*n,m);
    return;
  } else {
    nfact = nint(table[0]);
    mfact = nint(table[nfact]) + nfact;
    for (i=0;i<mfact;i++) { fact[i] = nint(table[i]); }
  }

  /* Gestion de work */
  /* nwork = 4*n*m */
  /* nwork = 512*max(n,m) */
  jmgetnwork(&nwork,512*max(n,m),4*max(n,m));

  /* On fait les T.F. sur la premiere dimension en tronconnant sur la deuxieme */
  debut = 1;
  while(1) {

    /* Tronconnage */
    /* Note : on met npair a .true. car il n'y a pas de restriction dans ce cas */
    jmdecoup(m,4*n,nwork,debut,1,&m_temp,&jdeb,&jfin,&nwork_temp,&fin);

    /* On copie le tableau d'entree dans le tableau de travail */
    /* On en profite pour premultiplier et pour tenir compte du signe */
    /* Note : On copie en transposant */
    for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (j=jdeb;j<=jfin;j++) {
        work[j-jdeb+m_temp*i]     =       scale*x[2*i  +j*2*ldx];
        work[j-jdeb+m_temp*(n+i)] = isign*scale*x[2*i+1+j*2*ldx];
      }
    }
    ioff = 0;

    /* Attention : ioff1 est peut-etre modifie en sortie */
    jmccm1d(m_temp,n,fact,100,0    ,table,ntable,100+0  ,work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee */
    for (i=0;i<=n-1;i++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (j=jdeb;j<=jfin;j++) {
        y[2*i  +j*2*ldy] = work[ioff+j-jdeb+m_temp*i];
        y[2*i+1+j*2*ldy] = work[ioff+j-jdeb+m_temp*(n+i)];
      }
    }

    /* A-t-on fini ? */
    if (fin) {
      break;
    } else {
      debut = 0;
      continue;
    }

  }

  /* On fait les T.F. sur l'autre dimension */
  debut = 1;
  while(1) {

    /* Tronconnage */
    jmdecoup(n,4*m,nwork,debut,1,&n_temp,&ideb,&ifin,&nwork_temp,&fin);

    /* On copie */
    for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (i=ideb;i<=ifin;i++) {
        work[i-ideb+n_temp*j]     = y[2*i  +j*2*ldy];
        work[i-ideb+n_temp*(m+j)] = y[2*i+1+j*2*ldy];
      }
    }
    ioff = 0;

    jmccm1d(n_temp,m,fact,100,nfact,table,ntable,100+2*n,work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee */
    for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (i=ideb;i<=ifin;i++) {
        y[2*i  +j*2*ldy] =       work[ioff+i-ideb+n_temp*j];
        y[2*i+1+j*2*ldy] = isign*work[ioff+i-ideb+n_temp*(m+j)];
      }
    }

    /* A-t-on fini ? */
    if (fin) {
      break;
    } else {
      debut = 0;
      continue;
    }

  }

}
