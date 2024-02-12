#define INTEGER long
#define NBITS_INTEGER 32
#define REAL8 double
void ccfft(
  INTEGER isign,
  INTEGER n,
  REAL8 scale,
  REAL8 x[],
  REAL8 y[],
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void ccfft2d(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx,
  REAL8 y[],
  INTEGER ldy,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void ccfft3d(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  INTEGER l,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx1,
  INTEGER ldx2,
  REAL8 y[],
  INTEGER ldy1,
  INTEGER ldy2,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void ccfftm(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx,
  REAL8 y[],
  INTEGER ldy,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void csfft(
  INTEGER isign,
  INTEGER n,
  REAL8 scale,
  REAL8 x[],
  REAL8 y[],
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void csfft2d(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx,
  REAL8 y[],
  INTEGER ldy,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void csfft3d(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  INTEGER l,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx1,
  INTEGER ldx2,
  REAL8 y[],
  INTEGER ldy1,
  INTEGER ldy2,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void csfftm(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx,
  REAL8 y[],
  INTEGER ldy,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void scfft(
  INTEGER isign,
  INTEGER n,
  REAL8 scale,
  REAL8 x[],
  REAL8 y[],
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void scfft2d(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx,
  REAL8 y[],
  INTEGER ldy,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void scfft3d(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  INTEGER l,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx1,
  INTEGER ldx2,
  REAL8 y[],
  INTEGER ldy1,
  INTEGER ldy2,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void scfftm(
  INTEGER isign,
  INTEGER n,
  INTEGER m,
  REAL8 scale,
  REAL8 x[],
  INTEGER ldx,
  REAL8 y[],
  INTEGER ldy,
  REAL8 table[],
  REAL8 work[],
  INTEGER isys
);
void cfftmlt(
  REAL8 xr[],
  REAL8 xi[],
  REAL8 work[],
  REAL8 trigs[],
  INTEGER ifax[],
  INTEGER inc,
  INTEGER jump,
  INTEGER n,
  INTEGER m,
  INTEGER isign
);
void cftfax(
  INTEGER n,
  INTEGER ifax[],
  REAL8 trigs[]
);
void rfftmlt(
  REAL8 x[],
  REAL8 work[],
  REAL8 trigs[],
  INTEGER ifax[],
  INTEGER incx,
  INTEGER jumpx,
  INTEGER n,
  INTEGER m,
  INTEGER isign
);
void fftfax(
  INTEGER n,
  INTEGER ifax[],
  REAL8 trigs[]
);
void jmgetcode(
  INTEGER *code
);
void jmgeterreur(
  INTEGER *arret
);
void jmgetstop(
  INTEGER *arret
);
void jmgetmessage(
  INTEGER code,
  char message[]
);
void jmgetnwork(
  INTEGER *nwork,
  INTEGER nwork_def,
  INTEGER nwork_min
);
void jmsetcode(
  INTEGER code
);
void jmseterreur(
  INTEGER arret
);
void jmsetstop(
  INTEGER arret
);
void jmsetnwork(
  INTEGER nwork
);
