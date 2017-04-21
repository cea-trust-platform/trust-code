void jmccm1d(
  INTEGER m,
  INTEGER n,
  INTEGER fact[],
  INTEGER nfact,
  INTEGER ifact,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmccm1d2(
  INTEGER p,
  INTEGER n,
  INTEGER m,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  INTEGER ntable2,
  INTEGER mtable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmccm1d3(
  INTEGER p,
  INTEGER n,
  INTEGER m,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  INTEGER ntable2,
  INTEGER mtable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmccm1d4(
  INTEGER p,
  INTEGER n,
  INTEGER m,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  INTEGER ntable2,
  INTEGER mtable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmccm1d5(
  INTEGER p,
  INTEGER n,
  INTEGER m,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  INTEGER ntable2,
  INTEGER mtable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmccm1dp(
  INTEGER p,
  INTEGER q,
  INTEGER m,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  INTEGER ntable2,
  INTEGER mtable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmcctranspcs(
  INTEGER m,
  INTEGER n,
  INTEGER n2,
  INTEGER n3,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmcsm1d(
  INTEGER m,
  INTEGER n,
  INTEGER fact[],
  INTEGER nfact,
  INTEGER ifact,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmcsm1dxy(
  INTEGER m,
  INTEGER n,
  INTEGER fact[],
  INTEGER nfact,
  INTEGER ifact,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  REAL8 work[],
  INTEGER nwork,
  REAL8 x[],
  INTEGER dimx,
  INTEGER debx,
  INTEGER incx,
  INTEGER jumpx,
  REAL8 y[],
  INTEGER dimy,
  INTEGER deby,
  INTEGER incy,
  INTEGER jumpy,
  INTEGER isign,
  REAL8 scale
);
void jmdecoup(
  INTEGER n,
  INTEGER nr,
  INTEGER nwork,
  INTEGER debut,
  INTEGER mpair,
  INTEGER *n_temp,
  INTEGER *ideb,
  INTEGER *ifin,
  INTEGER *nwork_temp,
  INTEGER *fin
);
void jmdecoup3(
  INTEGER n,
  INTEGER m,
  INTEGER nmr,
  INTEGER nwork,
  INTEGER debut,
  INTEGER lpair,
  INTEGER *ideb,
  INTEGER *ifin,
  INTEGER *jdeb,
  INTEGER *jfin,
  INTEGER *nmtemp,
  INTEGER *nwork_temp,
  INTEGER *fini
);
void jmerreur1(
  char nomsp[],
  INTEGER code,
  INTEGER var1
);
void jmerreur2(
  char nomsp[],
  INTEGER code,
  INTEGER var1,
  INTEGER var2
);
void jmerreur3(
  char nomsp[],
  INTEGER code,
  INTEGER var1,
  INTEGER var2,
  INTEGER var3
);
void jmerreur4(
  char nomsp[],
  INTEGER code,
  INTEGER var1,
  INTEGER var2,
  INTEGER var3,
  INTEGER var4
);
void jmfact(
  INTEGER n,
  INTEGER fact[],
  INTEGER nfact,
  INTEGER ideb,
  INTEGER *ifin
);
void jmgetsetcode(
  INTEGER *code,
  char type[1]
);
void jmgetseterreur(
  INTEGER *arret,
  char type[1]
);
void jmgetsetstop(
  INTEGER *arret,
  char type[1]
);
void jmgetsetnwork(
  INTEGER *nwork,
  char type[1]
);
void jmscm1d(
  INTEGER m,
  INTEGER n,
  INTEGER fact[],
  INTEGER nfact,
  INTEGER ifact,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
void jmscm1dxy(
  INTEGER m,
  INTEGER n,
  INTEGER fact[],
  INTEGER nfact,
  INTEGER ifact,
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  REAL8 work[],
  INTEGER nwork,
  REAL8 x[],
  INTEGER dimx,
  INTEGER debx,
  INTEGER incx,
  INTEGER jumpx,
  REAL8 y[],
  INTEGER dimy,
  INTEGER deby,
  INTEGER incy,
  INTEGER jumpy,
  INTEGER isign,
  REAL8 scale
);
void jmtable(
  REAL8 table[],
  INTEGER ntable,
  INTEGER itable,
  INTEGER n
);
void jmtransp(
  INTEGER n,
  INTEGER m,
  INTEGER l,
  REAL8 work[],
  INTEGER nwork,
  INTEGER *ioff
);
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define max(a,b) ( (a) > (b) ? (a) : (b) )
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#define nint(x) ( ((x) >= 0) ? (INTEGER)((x) + 0.5) : (INTEGER)((x) + 0.5) - 1)
unsigned INTEGER ipow(
unsigned INTEGER mult,
unsigned INTEGER N
);
