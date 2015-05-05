/* $Header: /home/teuler/cvsroot/lib/jmcsfft2d.f90,v 6.5 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void csfft2d(isign,n,m,scale,x,ldx,y,ldy,table,work,isys)


  /* Arguments */
  INTEGER isign;
  INTEGER m, n, ldx, ldy;
  REAL8 scale;
  REAL8 x[];
  REAL8 y[];
  REAL8 table[];
  REAL8 work[];
  INTEGER isys;

{

 /* Variables locales */

  INTEGER i, j;
  INTEGER ntable, nwork, ioff;
  INTEGER nfact, mfact;
  INTEGER fact[100];
  INTEGER ideb, ifin, jdeb, jfin, n_temp, m_temp, nwork_temp;
  INTEGER debut, fin;
  INTEGER dimy, deby, incy, jumpy;
  INTEGER signe;
  REAL8 scale_temp;
  INTEGER npair, mpair;
  char nomsp[] = "CSFFT2D";

  /* Positionnement a 0 du code de retour */
  jmsetcode(0);

  /* Gestion de npair et mpair */
  npair = ( (n%2) == 0 );
  mpair = ( (m%2) == 0 );

  /* Verification des conditions */
  if (isign != 0 && isign !=-1 && isign != 1) 
    jmerreur1(nomsp,2,isign);
  if (n < 1) jmerreur1(nomsp,23,n);
  if (m < 1) jmerreur1(nomsp,21,m);
  if (ldx < n/2+1) jmerreur2(nomsp,10,ldx,n/2+1);
  if (ldy < n+2  ) jmerreur2(nomsp,15,ldy,n+2  );
  if ( !npair && !mpair) 
    jmerreur2(nomsp,22,n,m);

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
  /* nwork = 2*2*(n/2+1)*m */
  /* nwork = 512*max(n,m) */
  jmgetnwork(&nwork,512*max(n,m),4*max(n,m));

  /* On fait les T.F. sur la premiere dimension en tronconnant sur la deuxieme */
  debut = 1;
  while(1) {

    /* Tronconnage */
    jmdecoup(n/2+1,4*m,nwork,debut,mpair,&n_temp,&ideb,&ifin,&nwork_temp,&fin);

    /* On copie le tableau d'entree dans le tableau de travail sans permuter */
    /* les dimensions (n en premier) pour faire d'abord la tf sur m */
    /* On en profite pour premultiplier et pour tenir compte du signe */
    for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (i=ideb;i<=ifin;i++) {
        work[             n_temp*j+i-ideb] =       scale*x[2*i  +2*ldx*j];
        work[nwork_temp/4+n_temp*j+i-ideb] = isign*scale*x[2*i+1+2*ldx*j];
      }
    }
    ioff = 0;

    /* On fait la FFT complexe -> complexe sur la deuxieme dimension (m) */
    jmccm1d(n_temp,m,fact,100,nfact,table,ntable,100+2*n,work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee */
    for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (i=ideb;i<=ifin;i++) {
         y[2*i  +ldy*j] = work[ioff+             n_temp*j+i-ideb];
         y[2*i+1+ldy*j] = work[ioff+nwork_temp/4+n_temp*j+i-ideb];
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
    jmdecoup(m,2*n,nwork,debut,npair,&m_temp,&jdeb,&jfin,&nwork_temp,&fin);

    /* On fait la FFT complexe -> reel sur le premiere dimension (n) */
    dimy = ldy*m   ; deby = jdeb*ldy   ; incy = 1 ; jumpy = ldy;
    signe = 1;
    scale_temp = ((REAL8)(1));
    jmcsm1dxy(m_temp,n,fact,100,0,table,ntable,100+0, 
      work,nwork_temp, 
      y,dimy,deby,incy,jumpy,y,dimy,deby,incy,jumpy,signe,scale_temp);

    /* A-t-on fini ? */
    if (fin) {
      break;
    } else {
      debut = 0;
      continue;
    }

  }

}
