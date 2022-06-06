/****************************************************************************
* Copyright (c) 2022, CEA
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

extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(DLSODECHIMIES)(integer *neq, double *y, double *t, double *tout, double* tau_mel, integer *itol, double *rtol, double *atol, double *rwork, integer *lrw, integer *iwork, integer *liw);
#else
  int F77DECLARE(dlsodechimies)(integer *neq, double *y, double *t, double *tout, double* tau_mel, integer *itol, double *rtol, double *atol, double *rwork, integer *lrw, integer *iwork, integer *liw);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(DLSODECHIMIES)(integer *neq, double *y, double *t, double *tout, double* tau_mel, integer *itol, double *rtol, double *atol, double *rwork, integer *lrw, integer *iwork, integer *liw)
{
  return F77NAME(dlsodechimies)(neq, y, t, tout, tau_mel,itol, rtol, atol, rwork, lrw, iwork, liw);
}
#endif
extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(DLSODE_CHIMIE_S_OLD)(integer *neq, double *y, double *t, double *tout, integer *itol, double *rtol, double *atol, double *rwork, integer *lrw, integer *iwork, integer *liw);
#else
  int F77DECLARE(dlsode_chimie_s_old)(integer *neq, double *y, double *t, double *tout, integer *itol, double *rtol, double *atol, double *rwork, integer *lrw, integer *iwork, integer *liw);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(DLSODE_CHIMIE_S_OLD)(integer *neq, double *y, double *t, double *tout, integer *itol, double *rtol, double *atol, double *rwork, integer *lrw, integer *iwork, integer *liw)
{
  return F77NAME(dlsode_chimie_s_old)(neq, y, t, tout, itol, rtol, atol, rwork, lrw, iwork, liw);
}
#endif

extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(INITTAILLECOMMON)(integer *nreact, integer *ncons);
#else
  int F77DECLARE(inittaillecommon)(integer *nreact, integer *ncons);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(INITTAILLECOMMON)(integer *nreact, integer *ncons)
{
  return F77NAME(inittaillecommon)(nreact, ncons);
}
#endif

extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(INITREACTIONCOMMON)(integer *ir, double *pstochio, integer *pactivite, integer *contrereaction);
#else
  int F77DECLARE(initreactioncommon)(integer *ir, double *pstochio, integer *pactivite, integer *contrereaction);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(INITREACTIONCOMMON)(integer *ir, double *pstochio, integer *pactivite, integer *contrereaction)
{
  return F77NAME(initreactioncommon)(ir, pstochio, pactivite,contrereaction);
}
#endif

extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(SETCWREACTION)(integer *ir, double *rcw);
#else
  int F77DECLARE(setcwreaction)(integer *ir, double *rcw);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(SETCWREACTION)(integer *ir, double *rcw)
{
  return F77NAME(setcwreaction)(ir, rcw);
}
#endif
extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(SETMARQUEUR)(integer *ir);
#else
  int F77DECLARE(setmarqueur)(integer *ir);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(SETMARQUEUR)(integer *ir)
{
  return F77NAME(setmarqueur)(ir);
}
#endif
extern "C" {
#ifdef F77_Majuscule
  int F77DECLARE(PRINTCOMMON)(void);
#else
  int F77DECLARE(printcommon)(void);
#endif
}
#ifndef F77_Majuscule
inline int F77DECLARE(PRINTCOMMON)(void)
{
  return F77NAME(printcommon)();
}
#endif
