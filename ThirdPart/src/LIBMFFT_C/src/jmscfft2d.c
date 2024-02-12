/* $Header: /home/teuler/cvsroot/lib/jmscfft2d.f90,v 6.5 2000/02/22 17:25:26 teuler Exp $ */
/* JMFFTLIB : A library of portable fourier transform subroutines */
/* emulating Cray SciLib */
/* Author   : Jean-Marie Teuler, CNRS-IDRIS (teuler@idris.fr) */
/*  */
/* Permission is granted to copy and distribute this file or modified */
/* versions of this file for no fee, provided the copyright notice and */
/* this permission notice are preserved on all copies. */

#include "jmfft.h"
#include "jmfft2.h"

void scfft2d(isign,n,m,scale,x,ldx,y,ldy,table,work,isys)


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
  INTEGER ntable, nwork, ioff;
  INTEGER nfact, mfact;
  INTEGER fact[100];
  INTEGER ideb, ifin, jdeb, jfin, n_temp, m_temp, nwork_temp;
  INTEGER debut, fin;
  INTEGER dimx, debx, incx, jumpx;
  INTEGER dimy, deby, incy, jumpy;
  INTEGER signe;
  INTEGER npair, mpair;
  char nomsp[] = "SCFFT2D";

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
  if (ldx < n    ) jmerreur2(nomsp, 9,ldx,n    );
  if (ldy < n/2+1) jmerreur2(nomsp,16,ldy,n/2+1);
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
    jmdecoup(m,2*n,nwork,debut,npair,&m_temp,&jdeb,&jfin,&nwork_temp,&fin);

    /* On fait les T.F. reelles sur la premiere dimension */
    /* Tout se passe comme si on faisait une T.F. 1D multiple (d'ordre m) */
    dimx = ldx*m   ; debx = jdeb*ldx   ; incx = 1 ; jumpx = ldx;
    dimy = 2*ldy*m ; deby = jdeb*2*ldy ; incy = 1 ; jumpy = 2*ldy;
    signe = 1;
    jmscm1dxy(m_temp,n,fact,100,0,table,ntable,100+0, 
      work,nwork_temp, 
      x,dimx,debx,incx,jumpx,y,dimy,deby,incy,jumpy,signe,scale);

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
    jmdecoup(n/2+1,4*m,nwork,debut,mpair,&n_temp,&ideb,&ifin,&nwork_temp,&fin);

    /* On copie */
    for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (i=ideb;i<=ifin;i++) {
        work[             n_temp*j+i-ideb] = y[2*i  +2*ldy*j] ;
        work[nwork_temp/4+n_temp*j+i-ideb] = y[2*i+1+2*ldy*j] ;
      }
    }
    ioff = 0;

    /* On fait les T.F. sur l'autre dimension (divisee par deux bien sur) */
    /* On va chercher l'autre table des cosinus */
    jmccm1d(n_temp,m,fact,100,nfact,table,ntable,100+2*n,work,nwork_temp,&ioff);

    /* On recopie dans le tableau d'arrivee */
    for (j=0;j<=m-1;j++) {
#pragma _CRI ivdep
#pragma loop noalias
#pragma loop novrec
#pragma vdir nodep
      for (i=ideb;i<=ifin;i++) {
        y[2*i  +2*ldy*j] =       work[ioff             +n_temp*j+i-ideb];
        y[2*i+1+2*ldy*j] = isign*work[ioff+nwork_temp/4+n_temp*j+i-ideb];
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
