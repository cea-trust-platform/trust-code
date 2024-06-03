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
#ifndef Matrice33_included
#define Matrice33_included

#include <TRUSTTabs_forward.h>
#include <assert.h>

/*! @brief une matrice 3x3.
 *
 * Convention pour m(i,j): i est l'indice de ligne, j l'indice de colonne entre 0 et 2 inclus.
 *   Attention: le constructeur par defaut n'initialise pas la matrice !
 *
 */
class Matrice33
{
public:
  Matrice33()
  {
  }
  ;
  Matrice33(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22)
  {
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
  }
  Matrice33(const DoubleTab& tab)
  {
    assert(tab.nb_dim() == 2 && tab.dimension(0) == 3 && tab.dimension(1) == 3);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        m[i][j] = tab(i, j);
  }
  double operator()(int i, int j) const
  {
    assert(i >= 0 && i < 3 && j >= 0 && j < 3);
    return m[i][j];
  }
  double& operator()(int i, int j)
  {
    assert(i >= 0 && i < 3 && j >= 0 && j < 3);
    return m[i][j];
  }
  inline double norme_Linfini();
  static inline void produit(const Matrice33& m, const Vecteur3& x, Vecteur3& y);
  static inline void produit_matriciel(const Matrice33& m1, const Matrice33& m2, Matrice33& res);
  static inline void transpose(const Matrice33& matrice, Matrice33& matrice_transpose);
  static inline double inverse(const Matrice33& m, Matrice33& resu, int exit_on_error = 1);

protected:
  double m[3][3];
  void init()
  {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        m[i][j] = 0.;
  }
};

#endif
