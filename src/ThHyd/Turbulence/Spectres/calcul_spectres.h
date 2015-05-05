/****************************************************************************
* Copyright (c) 2015, CEA
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        calcul_spectres.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence/Spectres
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <IntTab.h>
#include <DoubleTab.h>

/*
  extern "C" void calc_sp_nouveau(DoubleTab&  ,int , double, double& );
  extern "C" void calc_sp_nouveau_3_vit(DoubleTab& , DoubleTab& , DoubleTab& , int , double ,double& ,double&);
*/

extern "C" void calc_sp_nouveau_2(DoubleTab&  ,int , double , double&, double&);
extern "C" void calc_Ec(DoubleVect& ,int ,double& );
extern "C" void calc_sp_operateur(DoubleTab& , DoubleTab& , DoubleTab& , int , double ,int , double, DoubleVect&);
extern "C" void calc_sp_nouveau_2_operateur(DoubleTab&  ,int , double ,int , double,DoubleVect& );
#ifdef F77_Majuscule

//extern "C" void F77DECLARE(CALCUL0SPECTRE0AP0TF) (double*,double*,double*,double*,int*,double*,double*,double*);
//extern "C" void F77DECLARE(CALCUL0SPECTRE0AP0CHP) (double*,double*,int*,double*,double*,double*);
extern "C" void F77DECLARE(CALCUL0SPECTRE0AP0CHP0VERIF) (double*,double*,double*,double*,double*,int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,integer *wl, integer *wm, integer *wn, integer *iwork);
extern "C" void F77DECLARE(CALCUL0SPECTRE0AP0CHP0PER) (double*,double*,int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,integer *wl, integer *wm, integer *wn, integer *iwork);
//extern "C" void F77DECLARE(CALCUL0SPECTRE0AP0CHP0FACE) (double*,double*double*,double*,int*,double*,double*,double*);
//extern "C" void F77DECLARE (CALCUL0TR0INVERSE) (double*,double*,double*,double*,int*,double*);
//extern "C" void F77DECLARE (CALCUL0TR0INVERSE0FACE) (double*,double*,double*,double*,double*,double*,int*,double*);
//extern "C" void F77DECLARE (TG) (double*, int*);
//extern "C" void F77DECLARE (EXTRAPOLE) (double*,double*,double*,double*,double*,int*,double*,double*,double*);
//extern "C" void F77DECLARE (EXTRAPOLE0E) (double*,double*,double*,double*,double*,int*,double*,double*,double*);
//extern "C" void F77DECLARE (REND0PER0CHP) (double*,int*);
extern "C" void F77DECLARE(CALCUL0SPECTRE0AVEC030VIT) (double*,double*,double*,double*,int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,integer *wl, integer *wm, integer *wn, integer *iwork);

#else

//extern "C" void F77DECLARE(calcul0spectre0ap0tf) (double*,double*,double*,double*,int*,double*,double*,double*);
//extern "C" void F77DECLARE(calcul0spectre0ap0chp) (double*,double*,int*,double*,double*,double*);
extern "C" void F77DECLARE(calcul0spectre0ap0chp0verif) (double*,double*,double*,double*,double*,int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,integer *wl, integer *wm, integer *wn, integer *iwork);
extern "C" void F77DECLARE(calcul0spectre0ap0chp0per) (double*,double*,int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,integer *wl, integer *wm, integer *wn, integer *iwork);
//extern "C" void F77DECLARE(calcul0spectre0ap0chp0face) (double*,double*,double*,double*,int*,double*,double*,double*);
/* extern "C" void F77DECLARE(calcul0tr0inverse) (double*,double*,double*,double*,int*,double*); */
/* extern "C" void F77DECLARE(calcul0tr0inverse0face) (double*,double*,double*,double*,double*,double*,int*,double*); */
/* extern "C" void F77DECLARE (tg) (double*, int*); */
/* extern "C" void F77DECLARE (extrapole) (double*,double*,double*,double*,double*,int*,double*,double*,double*); */
/* extern "C" void F77DECLARE (extrapole0e) (double*,double*,double*,double*,double*,int*,double*,double*,double*); */
/* extern "C" void F77DECLARE (rend0per0chp) (double*,int*); */
extern "C" void F77DECLARE(calcul0spectre0avec030vit) (double*,double*,double*,double*,int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,integer *wl, integer *wm, integer *wn, integer *iwork);

#endif


#ifndef F77_Majuscule

/* inline void F77NAME(CALCUL0SPECTRE0AP0TF)(double* x1,double* x2,double* x3,double* x4,int* i1,double* x5,double* x6,double* x7) */
/* { */
/*   F77NAME(calcul0spectre0ap0tf)(x1, x2,x3,x4, i1,x5,x6,x7); */
/* } */

/* inline void F77NAME(CALCUL0SPECTRE0AP0CHP)(double* x1,double* x2,int* i1,double* x3,double* x4,double* x5) */
/* { */
/*   F77NAME(calcul0spectre0ap0chp)(x1, x2, i1, x3, x4, x5); */
/* } */

inline void F77NAME(CALCUL0SPECTRE0AP0CHP0VERIF)(double* x1,double* x2,double* x6,double* x7,double* x8,int* i1,double* x3,double* x4,double* x5,double* x9,double* x10,double* x11,double* x12,double* x13,double* x14,double* x15,double* x16,double* x17,double* x18,integer *wl, integer *wm, integer *wn, integer *iwork)
{
  F77NAME(calcul0spectre0ap0chp0verif)(x1, x2,x6,x7,x8,  i1, x3, x4, x5, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18,wl, wm, wn, iwork);
}

inline void F77NAME(CALCUL0SPECTRE0AP0CHP0PER)(double* x1,double* x2,int* i1,double* x3,double* x4,double* x5, double* x8,double* x9,double* x10,double* x11,double* x12,double* x13,double* x14,double* x15,double* x16,double* x17,double* x18,double* x19,double* x20,integer *wl, integer *wm, integer *wn, integer *iwork)
{
  F77NAME(calcul0spectre0ap0chp0per)(x1, x2, i1, x3, x4, x5, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20,wl, wm, wn, iwork);
}

/* inline void F77NAME(CALCUL0SPECTRE0AP0CHP0FACE)(double* x6,double* x7,double* x1,double* x2,int* i1,double* x3,double* x4,double* x5) */
/* { */
/*   F77NAME(calcul0spectre0ap0chp0face)(x6,x7,x1, x2, i1, x3, x4, x5); */
/* } */

/* inline void F77NAME(CALCUL0TR0INVERSE)(double* x1,double* x2,double* x3,double* x4,int* i1,double* x5) */
/* { */
/*   F77NAME(calcul0tr0inverse)(x1, x2, x3, x4, i1,  x5); */
/* } */

/* inline void F77NAME(CALCUL0TR0INVERSE0FACE)(double* x6,double* x7,double* x1,double* x2,double* x3,double* x4,int* i1,double* x5) */
/* { */
/*   F77NAME(calcul0tr0inverse0face)(x6,x7,x1, x2, x3, x4, i1,  x5); */
/* } */

/* inline void F77NAME(TG)(double* x1,int* i1) */
/* { */
/*   F77NAME(tg)(x1,i1); */
/* } */

/* inline void F77NAME(EXTRAPOLE) (double* x1,double* x2,double* x3,double* x4,double* x5,int* i1,double* x6,double* x7,double* x8) */
/* { */
/*   F77NAME(extrapole) (x1,x2,x3,x4,x5,i1,x6,x7,x8); */
/* } */

/* inline void F77NAME(EXTRAPOLE0E) (double* x1,double* x2,double* x3,double* x4,double* x5,int* i1,double* x6,double* x7,double* x8) */
/* { */
/*   F77NAME(extrapole0e) (x1,x2,x3,x4,x5,i1,x6,x7,x8); */
/* } */

/* inline void F77NAME(REND0PER0CHP) (double* x1, int* i1) */
/* { */
/*   F77NAME(rend0per0chp) (x1,i1); */
/* } */

inline void F77NAME(CALCUL0SPECTRE0AVEC030VIT)(double* x1,double* x6,double* x7,double* x2,int* i1,double* x3,double* x4,double* x5,double* x8,double* x9,double* x10,double* x11,double* x12,double* x13,double* x14,double* x15,double* x16,double* x17,double* x18,double* x19,double* x20,integer *wl, integer *wm, integer *wn, integer *iwork)
{
  F77NAME(calcul0spectre0avec030vit)(x1, x6, x7, x2, i1, x3, x4, x5, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20,wl, wm, wn, iwork);
}


#endif


