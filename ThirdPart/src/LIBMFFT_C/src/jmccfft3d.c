/* $Header: /home/teuler/cvsroot/lib/jmccfft3d.f90,v 6.8 2000/03/01 17:39:46 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void ccfft3d(isign,n,m,l,scale,x,ldx1,ldx2,y,ldy1,ldy2,table,work,isys)


  /* Arguments */
  INTEGER isign;
  INTEGER n, m, l, ldx1, ldx2, ldy1, ldy2;
  REAL8 scale;
  REAL8 x[];
  REAL8 y[];
  REAL8 table[];
  REAL8 work[];
  INTEGER isys;

{

 /* Variables locales */

  INTEGER i, j, k;
  INTEGER ioff;
  INTEGER ntable, nwork;
  INTEGER nfact, mfact, lfact;
  INTEGER fact[100];
  INTEGER ideb, ifin, i1, i2, jdeb, jfin, j1, j2, kdeb, kfin;
  INTEGER nwork_temp, nmtemp, mltemp, nltemp, iwork;
  INTEGER debut, fini;
  char nomsp[] = "CCFFT3D";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);
  if (l < 1) jmerreur1(nomsp,8,l);
  if (ldx1 < n) jmerreur2(nomsp,11,ldx1,n);
  if (ldx2 < m) jmerreur2(nomsp,13,ldx2,m);
  if (ldy1 < n) jmerreur2(nomsp,17,ldy1,n);
  if (ldy2 < m) jmerreur2(nomsp,20,ldy2,m);

  /* Gestion de table */
  ntable = 100+2*(n+m+l);

  /* Test sur isign */
  if (isign == 0) {
    /* Pour la factorisation */
    jmfact(n,fact,100,    0,&nfact);
    jmfact(m,fact,100,nfact,&mfact);
    jmfact(l,fact,100,mfact,&lfact);
    for (i=0;i<lfact;i++) { table[i] = fact[i]; }
    /* Pour les sinus et cosinus */
    jmtable(table,ntable,100+0,      n);
    jmtable(table,ntable,100+2*n,    m);
    jmtable(table,ntable,100+2*(n+m),l);
    return;
  } else {
    nfact = nint(table[0]);
    mfact = nint(table[nfact]) + nfact;
    lfact = nint(table[mfact]) + mfact;
    for (i=0;i<lfact;i++) { fact[i] = nint(table[i]); }
  }

  /* Gestion de work */
  /* nwork = 4*n*m*l */
  /* nwork = 512*max(n,m,l) */
  jmgetnwork(&nwork,512*max(max(n,m),l),4*max(max(n,m),l));

  /* On fait les T.F. sur la troisieme dimension en tronconnant sur la premiere */
  /* et la deuxieme */
  debut = 1;
  fini  = 0;
  while (!fini) {

    /* Tronconnage */
    /* Note : on met npair a .true. car il n'y a pas de restriction dans ce cas */
    jmdecoup3(n,m,4*l,nwork,debut,1,&ideb,&ifin,&jdeb,&jfin,&nmtemp,&nwork_temp,&fini);
    debut = 0;

    /* On copie le tableau d'entree dans le tableau de travail */
    /* On en profite pour premultiplier et pour tenir compte du signe */
    /* On prend garde a la gestion des extremites */
    for (k=0;k<=l-1;k++) {
      iwork = 0;
      for (j=jdeb;j<=jfin;j++) {
        i1 = 0;
        i2 = n-1;
        if (j == jdeb) i1 = ideb;
        if (j == jfin) i2 = ifin;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=i1;i<=i2;i++) {
          work[             iwork+k*nmtemp] =       scale*x[2*i  +2*ldx1*j+2*ldx1*ldx2*k];
          work[nwork_temp/4+iwork+k*nmtemp] = isign*scale*x[2*i+1+2*ldx1*j+2*ldx1*ldx2*k];
          iwork = iwork+1;
        }
      }
    }

    /* On fait les T.F. sur la troisieme dimension */
    ioff = 0;
    jmccm1d(nmtemp,l,fact,100,mfact,table,ntable,100+2*(n+m),work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee */
    for (k=0;k<=l-1;k++) {
      iwork = 0;
      for (j=jdeb;j<=jfin;j++) {
        i1 = 0;
        i2 = n-1;
        if (j == jdeb) i1 = ideb;
        if (j == jfin) i2 = ifin;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=i1;i<=i2;i++) {
          y[2*i  +2*ldy1*j+2*ldy1*ldy2*k] = work[ioff             +iwork+k*nmtemp];
          y[2*i+1+2*ldy1*j+2*ldy1*ldy2*k] = work[ioff+nwork_temp/4+iwork+k*nmtemp];
          iwork = iwork+1;
        }
      }
    }

  }

  /* On fait les T.F. sur la deuxieme dimension en tronconnant sur la premiere */
  /* et la troisieme */
  debut = 1;
  fini  = 0;
  while (!fini) {

    /* Tronconnage */
    jmdecoup3(n,l,4*m,nwork,debut,1,&ideb,&ifin,&kdeb,&kfin,&nltemp,&nwork_temp,&fini);
    debut = 0;

    /* On copie le tableau d'entree dans le tableau de travail */
    /* On prend garde a la gestion des extremites */
    for (j=0;j<=m-1;j++) {
      iwork = 0;
      for (k=kdeb;k<=kfin;k++) {
        i1 = 0;
        i2 = n-1;
        if (k == kdeb) i1 = ideb;
        if (k == kfin) i2 = ifin;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=i1;i<=i2;i++) {
          work[             iwork+j*nltemp] = y[2*i  +2*ldy1*j+2*ldy1*ldy2*k];
          work[nwork_temp/4+iwork+j*nltemp] = y[2*i+1+2*ldy1*j+2*ldy1*ldy2*k];
          iwork = iwork+1;
        }
      }
    }

    /* On fait les T.F. sur la deuxieme dimension */
    ioff = 0;
    jmccm1d(nltemp,m,fact,100,nfact,table,ntable,100+2*n    ,work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee */
    for (j=0;j<=m-1;j++) {
      iwork = 0;
      for (k=kdeb;k<=kfin;k++) {
        i1 = 0;
        i2 = n-1;
        if (k == kdeb) i1 = ideb;
        if (k == kfin) i2 = ifin;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (i=i1;i<=i2;i++) {
          y[2*i  +2*ldy1*j+2*ldy1*ldy2*k] = work[ioff             +iwork+j*nltemp];
          y[2*i+1+2*ldy1*j+2*ldy1*ldy2*k] = work[ioff+nwork_temp/4+iwork+j*nltemp];
          iwork = iwork+1;
        }
      }
    }

  }

  /* On fait les T.F. sur la premiere dimension en tronconnant sur la deuxieme */
  /* et la troisieme */
  debut = 1;
  fini  = 0;
  while (!fini) {

    /* Tronconnage */
    jmdecoup3(m,l,4*n,nwork,debut,1,&jdeb,&jfin,&kdeb,&kfin,&mltemp,&nwork_temp,&fini);
    debut = 0;

    /* On copie le tableau d'entree dans le tableau de travail */
    /* On prend garde a la gestion des extremites */
    for (i=0;i<=n-1;i++) {
      iwork = 0;
      for (k=kdeb;k<=kfin;k++) {
        j1 = 0;
        j2 = m-1;
        if (k == kdeb) j1 = jdeb;
        if (k == kfin) j2 = jfin;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=j1;j<=j2;j++) {
          work[             iwork+i*mltemp] = y[2*i  +2*ldy1*j+2*ldy1*ldy2*k];
          work[nwork_temp/4+iwork+i*mltemp] = y[2*i+1+2*ldy1*j+2*ldy1*ldy2*k];
          iwork = iwork+1;
        }
      }
    }

    /* On fait les T.F. sur la premiere dimension */
    ioff = 0;
    jmccm1d(mltemp,n,fact,100,0    ,table,ntable,100+0      ,work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee en tenant compte du signe */
    for (i=0;i<=n-1;i++) {
      iwork = 0;
      for (k=kdeb;k<=kfin;k++) {
        j1 = 0;
        j2 = m-1;
        if (k == kdeb) j1 = jdeb;
        if (k == kfin) j2 = jfin;
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
        for (j=j1;j<=j2;j++) {
          y[2*i  +2*ldy1*j+2*ldy1*ldy2*k] =       work[ioff             +iwork+i*mltemp];
          y[2*i+1+2*ldy1*j+2*ldy1*ldy2*k] = isign*work[ioff+nwork_temp/4+iwork+i*mltemp];
          iwork = iwork+1;
        }
      }
    }

  }

}
