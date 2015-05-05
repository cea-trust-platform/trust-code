#include "jmfft.h"
#define CCFFT ccfft_
#define CCFFT2D ccfft2d_
#define CCFFT3D ccfft3d_
#define CCFFTM ccfftm_
#define CSFFT csfft_
#define CSFFT2D csfft2d_
#define CSFFT3D csfft3d_
#define CSFFTM csfftm_
#define SCFFT scfft_
#define SCFFT2D scfft2d_
#define SCFFT3D scfft3d_
#define SCFFTM scfftm_
#define CFFTMLT cfftmlt_
#define CFTFAX cftfax_
#define RFFTMLT rfftmlt_
#define FFTFAX fftfax_
#define JMGETCODE jmgetcode_
#define JMGETERREUR jmgeterreur_
#define JMGETSTOP jmgetstop_
#define JMGETMESSAGE jmgetmessage_
#define JMGETNWORK jmgetnwork_
#define JMSETCODE jmsetcode_
#define JMSETERREUR jmseterreur_
#define JMSETSTOP jmsetstop_
#define JMSETNWORK jmsetnwork_
void CCFFT(INTEGER *isign,INTEGER *n,REAL8 *scale,REAL8 x[],REAL8 y[],REAL8 table[],REAL8 work[],INTEGER *isys) {
  ccfft(*isign,*n,*scale,x,y,table,work,*isys);
}
void CCFFT2D(INTEGER *isign,INTEGER *n,INTEGER *m,REAL8 *scale,REAL8 x[],INTEGER *ldx,REAL8 y[],INTEGER *ldy,REAL8 table[],REAL8 work[],INTEGER *isys) {
  ccfft2d(*isign,*n,*m,*scale,x,*ldx,y,*ldy,table,work,*isys);
}
void CCFFT3D(INTEGER *isign,INTEGER *n,INTEGER *m,INTEGER *l,REAL8 *scale,REAL8 x[],INTEGER *ldx1,INTEGER *ldx2,REAL8 y[],INTEGER *ldy1,INTEGER *ldy2,REAL8 table[],REAL8 work[],INTEGER *isys) {
  ccfft3d(*isign,*n,*m,*l,*scale,x,*ldx1,*ldx2,y,*ldy1,*ldy2,table,work,*isys);
}
void CCFFTM(INTEGER *isign,INTEGER *n,INTEGER *m,REAL8 *scale,REAL8 x[],INTEGER *ldx,REAL8 y[],INTEGER *ldy,REAL8 table[],REAL8 work[],INTEGER *isys) {
  ccfftm(*isign,*n,*m,*scale,x,*ldx,y,*ldy,table,work,*isys);
}
void CSFFT(INTEGER *isign,INTEGER *n,REAL8 *scale,REAL8 x[],REAL8 y[],REAL8 table[],REAL8 work[],INTEGER *isys) {
  csfft(*isign,*n,*scale,x,y,table,work,*isys);
}
void CSFFT2D(INTEGER *isign,INTEGER *n,INTEGER *m,REAL8 *scale,REAL8 x[],INTEGER *ldx,REAL8 y[],INTEGER *ldy,REAL8 table[],REAL8 work[],INTEGER *isys) {
  csfft2d(*isign,*n,*m,*scale,x,*ldx,y,*ldy,table,work,*isys);
}
void CSFFT3D(INTEGER *isign,INTEGER *n,INTEGER *m,INTEGER *l,REAL8 *scale,REAL8 x[],INTEGER *ldx1,INTEGER *ldx2,REAL8 y[],INTEGER *ldy1,INTEGER *ldy2,REAL8 table[],REAL8 work[],INTEGER *isys) {
  csfft3d(*isign,*n,*m,*l,*scale,x,*ldx1,*ldx2,y,*ldy1,*ldy2,table,work,*isys);
}
void CSFFTM(INTEGER *isign,INTEGER *n,INTEGER *m,REAL8 *scale,REAL8 x[],INTEGER *ldx,REAL8 y[],INTEGER *ldy,REAL8 table[],REAL8 work[],INTEGER *isys) {
  csfftm(*isign,*n,*m,*scale,x,*ldx,y,*ldy,table,work,*isys);
}
void SCFFT(INTEGER *isign,INTEGER *n,REAL8 *scale,REAL8 x[],REAL8 y[],REAL8 table[],REAL8 work[],INTEGER *isys) {
  scfft(*isign,*n,*scale,x,y,table,work,*isys);
}
void SCFFT2D(INTEGER *isign,INTEGER *n,INTEGER *m,REAL8 *scale,REAL8 x[],INTEGER *ldx,REAL8 y[],INTEGER *ldy,REAL8 table[],REAL8 work[],INTEGER *isys) {
  scfft2d(*isign,*n,*m,*scale,x,*ldx,y,*ldy,table,work,*isys);
}
void SCFFT3D(INTEGER *isign,INTEGER *n,INTEGER *m,INTEGER *l,REAL8 *scale,REAL8 x[],INTEGER *ldx1,INTEGER *ldx2,REAL8 y[],INTEGER *ldy1,INTEGER *ldy2,REAL8 table[],REAL8 work[],INTEGER *isys) {
  scfft3d(*isign,*n,*m,*l,*scale,x,*ldx1,*ldx2,y,*ldy1,*ldy2,table,work,*isys);
}
void SCFFTM(INTEGER *isign,INTEGER *n,INTEGER *m,REAL8 *scale,REAL8 x[],INTEGER *ldx,REAL8 y[],INTEGER *ldy,REAL8 table[],REAL8 work[],INTEGER *isys) {
  scfftm(*isign,*n,*m,*scale,x,*ldx,y,*ldy,table,work,*isys);
}
void CFFTMLT(REAL8 xr[],REAL8 xi[],REAL8 work[],REAL8 trigs[],INTEGER ifax[],INTEGER *inc,INTEGER *jump,INTEGER *n,INTEGER *m,INTEGER *isign) {
  cfftmlt(xr,xi,work,trigs,ifax,*inc,*jump,*n,*m,*isign);
}
void CFTFAX(INTEGER *n,INTEGER ifax[],REAL8 trigs[]) {
  cftfax(*n,ifax,trigs);
}
void RFFTMLT(REAL8 x[],REAL8 work[],REAL8 trigs[],INTEGER ifax[],INTEGER *incx,INTEGER *jumpx,INTEGER *n,INTEGER *m,INTEGER *isign) {
  rfftmlt(x,work,trigs,ifax,*incx,*jumpx,*n,*m,*isign);
}
void FFTFAX(INTEGER *n,INTEGER ifax[],REAL8 trigs[]) {
  fftfax(*n,ifax,trigs);
}
void JMGETCODE(INTEGER *code) {
  jmgetcode(code);
}
void JMGETERREUR(INTEGER *arret) {
  jmgeterreur(arret);
}
void JMGETSTOP(INTEGER *arret) {
  jmgetstop(arret);
}
void JMGETMESSAGE(INTEGER *code,char message[]) {
  jmgetmessage(*code,message);
}
void JMGETNWORK(INTEGER *nwork,INTEGER *nwork_def,INTEGER *nwork_min) {
  jmgetnwork(nwork,*nwork_def,*nwork_min);
}
void JMSETCODE(INTEGER *code) {
  jmsetcode(*code);
}
void JMSETERREUR(INTEGER *arret) {
  jmseterreur(*arret);
}
void JMSETSTOP(INTEGER *arret) {
  jmsetstop(*arret);
}
void JMSETNWORK(INTEGER *nwork) {
  jmsetnwork(*nwork);
}
