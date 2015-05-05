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
// File:        Solv_TDMA.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#include <Solv_TDMA.h>
#include <DoubleVect.h>

void Solv_TDMA::resoudre(const DoubleVect& ma, const DoubleVect& mb, const DoubleVect& mc, const DoubleVect& sm, DoubleVect& vi, int M)
{
  DoubleVect malpha, mbeta; //2 vecteurs intermediares
  DoubleVect my; //vecteur solution du systeme L.y=F
  int i;
  malpha.resize(M);
  mbeta.resize(M);
  my.resize(M);

  //L'algorithme de Thomas (TDMA) repose sur la decomposition LU de la matrice
  //tridiagonale a inverser. Soit A.x = y le systeme lineaire a inverser avec A = LU.
  //On resoud d'abord par descente le syteme L.z = y avec z = U.x et on resoud ensuite
  //par remontee le systeme U.x = z.

  // Les 3 diagonales de la matrice sont chacune conservees sous forme de vecteur
  // ma : diagonale principale ; mb : diagonale secondaire inferieure
  // mc : diagonale secondaire superieure
  // M est l'ordre de la matrice A

  //La matrice L est composee d'une diagonale principale (malpha) et d'une diagonale
  //secondaire inferieure (mb). La matrice U est composee d'une diagonale principale
  //(dont tous les elements sont egaux a 1) et d'une diagonale secondaire superieure
  //(mbeta).

  //Resolution correcte d'un systeme lineaire 3*3 antisymetrique, le 23.05.2003

  //Remplissage de malpha et mbeta

  malpha(0) = ma(0);
  mbeta(0) = mc(0)/ma(0);

  for (i = 1 ; i<M-1 ; i++)
    {
      malpha(i) = ma(i) - mb(i-1)*mbeta(i-1);
      mbeta(i) = mc(i)/malpha(i);
    }
  malpha(M-1) = ma(M-1) - mb(M-2)*mbeta(M-2);

  //Resolution du premier systeme par descente

  my(0) = sm(0)/malpha(0);

  for (i = 1 ; i<M ; i++)
    my(i) = (sm(i)-mb(i-1)*my(i-1))/malpha(i);

  //Resolution du premier systeme par remontee

  vi(M-1) = my(M-1);

  for (i = M-2 ; i>=0 ; i--)
    vi(i) = my(i) - mbeta(i)*vi(i+1);
}
