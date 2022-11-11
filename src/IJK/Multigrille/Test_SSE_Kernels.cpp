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

#include <Test_SSE_Kernels.h>
#include <simd_tools.h>
#include <iostream>
#include <sys/time.h>
#include <IJK_Field.h>
#include <Param.h>


Implemente_instanciable(Test_SSE_Kernels, "Test_SSE_Kernels", Interprete);

Sortie& Test_SSE_Kernels::printOn(Sortie& s ) const
{
  return s;
}

Entree& Test_SSE_Kernels::readOn(Entree& is )
{
  return is;
}

Entree& Test_SSE_Kernels::interpreter(Entree& is)
{
  int i=0;
  int nmax=1000;
  Param param(que_suis_je());
  param.ajouter("nmax", &nmax);
  param.lire_avec_accolades(is);

  /*
  test64x64_float(64*64,64,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(64*64,64,0,0,false);
  i++;
  if (i>nmax) return is;
  test68x68_float(68*68,68,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(68*68,68,0,0,false);
  i++;
  if (i>nmax) return is;
  test72x72_float(72*72,72,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(72*72,72,0,0,false);
  i++;
  if (i>nmax) return is;
  test76x76_float(76*76,76,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(76*76,76,0,0,false);
  i++;
  if (i>nmax) return is;
  test80x80_float(80*80,80,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(80*80,80,0,0,false);
  i++;
  if (i>nmax) return is;
  test64x64_double(64*64,64,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_double(64*64,64,0,0,false);
  i++;
  if (i>nmax) return is;
  test68x68_double(68*68,68,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_double(68*68,68,0,0,false);
  i++;
  if (i>nmax) return is;
  test72x72_double(72*72,72,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_double(72*72,72,0,0,false);
  i++;
  if (i>nmax) return is;
  test76x76_double(76*76,76,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_double(76*76,76,0,0,false);
  i++;
  if (i>nmax) return is;
  test80x80_double(80*80,80,0,0,false);
  i++;
  if (i>nmax) return is;
  testgeneric_double(80*80,80,0,0,false);
  i++;
  if (i>nmax) return is;
  test64x64_float(64*64,64,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(64*64,64,0,0,true);
  i++;
  if (i>nmax) return is;
  test68x68_float(68*68,68,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(68*68,68,0,0,true);
  i++;
  if (i>nmax) return is;
  test72x72_float(72*72,72,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(72*72,72,0,0,true);
  i++;
  if (i>nmax) return is;
  test76x76_float(76*76,76,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(76*76,76,0,0,true);
  i++;
  if (i>nmax) return is;
  test80x80_float(80*80,80,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(80*80,80,0,0,true);
  i++;
  if (i>nmax) return is;
  test64x64_double(64*64,64,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_double(64*64,64,0,0,true);
  i++;
  if (i>nmax) return is;
  test68x68_double(68*68,68,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_double(68*68,68,0,0,true);
  i++;
  if (i>nmax) return is;
  test72x72_double(72*72,72,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_double(72*72,72,0,0,true);
  i++;
  if (i>nmax) return is;
  test76x76_double(76*76,76,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_double(76*76,76,0,0,true);
  i++;
  if (i>nmax) return is;
  test80x80_double(80*80,80,0,0,true);
  i++;
  if (i>nmax) return is;
  testgeneric_double(80*80,80,0,0,true);
  i++;
  if (i>nmax) return is;
  // suite
  testgeneric_float(68*68, 68, 128, (68-64)/2, true);
  i++;
  if (i>nmax) return is;
  test68x68_float(68*68, 68, 128, (68-64)/2, true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(68*68, 68, 128, (68-64)/2, false);
  i++;
  if (i>nmax) return is;
  test68x68_float(68*68, 68, 128, (68-64)/2, false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(72*72, 72, 128, (72-64)/2, true);
  i++;
  if (i>nmax) return is;
  test72x72_float(72*72, 72, 128, (72-64)/2, true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(72*72, 72, 128, (72-64)/2, false);
  i++;
  if (i>nmax) return is;
  test72x72_float(72*72, 72, 128, (72-64)/2, false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(76*76, 76, 128, (76-64)/2, true);
  i++;
  if (i>nmax) return is;
  test76x76_float(76*76, 76, 128, (76-64)/2, true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(76*76, 76, 128, (76-64)/2, false);
  i++;
  if (i>nmax) return is;
  test76x76_float(76*76, 76, 128, (76-64)/2, false);
  i++;
  if (i>nmax) return is;
  testgeneric_float(80*80, 80, 128, (80-64)/2, true);
  i++;
  if (i>nmax) return is;
  test80x80_float(80*80, 80, 128, (80-64)/2, true);
  i++;
  if (i>nmax) return is;
  testgeneric_float(80*80, 80, 128, (80-64)/2, false);
  i++;
  if (i>nmax) return is;
  test80x80_float(80*80, 80, 128, (80-64)/2, false);
  i++;
  if (i>nmax) return is;
  */
  return is;
}

