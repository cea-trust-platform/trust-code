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
// File:        Linear_algebra_tools_impl.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Linear_algebra_tools_impl_H
#define Linear_algebra_tools_impl_H
#include <Linear_algebra_tools.h>

// Description: calcul de la norme Linfini de la matrice
// Propriete: on note |x| la norme Linfini de x (vecteur ou matrice)
//  On a |m * x| <= |m| * |x|
// En pratique: c'est le max sur j de la somme sur i de fabs(m(i,j))
inline double Matrice33::norme_Linfini()
{
  double x = fabs(m[0][0]) + fabs(m[0][1]) + fabs(m[0][2]);
  double y = fabs(m[1][0]) + fabs(m[1][1]) + fabs(m[1][2]);
  double z = fabs(m[2][0]) + fabs(m[2][1]) + fabs(m[2][2]);
  double resu = ((x > y) ? x : y);
  resu = ((resu > z) ? resu : z);
  return resu;
}

// Description: produit avec de la matrice avec le vecteur x.
inline void Matrice33::produit(const Matrice33& m, const Vecteur3& x, Vecteur3& y)
{
  y.v[0] = m.m[0][0] * x.v[0] + m.m[0][1] * x.v[1] + m.m[0][2] * x.v[2];
  y.v[1] = m.m[1][0] * x.v[0] + m.m[1][1] * x.v[1] + m.m[1][2] * x.v[2];
  y.v[2] = m.m[2][0] * x.v[0] + m.m[2][1] * x.v[1] + m.m[2][2] * x.v[2];
}

inline void Vecteur3::produit_vectoriel(const Vecteur3& x, const Vecteur3& y, Vecteur3& z)
{
  z.v[0] = x.v[1] * y.v[2] - x.v[2] * y.v[1];
  z.v[1] = x.v[2] * y.v[0] - x.v[0] * y.v[2];
  z.v[2] = x.v[0] * y.v[1] - x.v[1] * y.v[0];
}

inline double Vecteur3::produit_scalaire(const Vecteur3& x, const Vecteur3& y)
{
  double r = x.v[0] * y.v[0] + x.v[1] * y.v[1] + x.v[2] * y.v[2];
  return r;
}

// Description: norme L_infini, c'est le max des abs(v[i])
inline double Vecteur3::norme_Linfini()
{
  double x = fabs(v[0]);
  double y = fabs(v[1]);
  double z = fabs(v[2]);
  double resu = ((x > y) ? x : y);
  resu = ((resu > z) ? resu : z);
  return resu;
}

// Description: calcul de l'inverse.
//  Si le determinant de "matrice" est nul, exit() si exit_on_error (valeur par defaut)
//   sinon on ne remplit pas matrice_inv et on renvoie 0.
// Valeur de retour: determinant de la "matrice" (pas de l'inverse !)
inline double Matrice33::inverse(const Matrice33& matrice, Matrice33& matrice_inv, int exit_on_error)
{
  const double a00 = matrice.m[0][0];
  const double a01 = matrice.m[0][1];
  const double a02 = matrice.m[0][2];
  const double a10 = matrice.m[1][0];
  const double a11 = matrice.m[1][1];
  const double a12 = matrice.m[1][2];
  const double a20 = matrice.m[2][0];
  const double a21 = matrice.m[2][1];
  const double a22 = matrice.m[2][2];
  // calcul de valeurs temporaires pour optimisation
  const double t4 = a00*a11;
  const double t6 = a00*a12;
  const double t8 = a01*a10;
  const double t10 = a02*a10;
  const double t12 = a01*a20;
  const double t14 = a02*a20;
  const double t = t4*a22-t6*a21-t8*a22+t10*a21+t12*a12-t14*a11;
  if (t==0.)
    {
      if (exit_on_error)
        {
          Cerr << "Error in Matrice33::inverse: determinant is null" << finl;
          Process::exit();
        }
      // To avoid the compiler to complain about "might be non initialized":
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          matrice_inv.m[i][j] = 0.;
      return 0.;
    }
  const double t17 = 1/(t);

  //calcul de la matrice inverse
  matrice_inv.m[0][0] = (a11*a22-a12*a21)*t17;
  matrice_inv.m[0][1] = -(a01*a22-a02*a21)*t17;
  matrice_inv.m[0][2] = -(-a01*a12+a02*a11)*t17;
  matrice_inv.m[1][0] = (-a10*a22+a12*a20)*t17;
  matrice_inv.m[1][1] = (a00*a22-t14)*t17;
  matrice_inv.m[1][2] = -(t6-t10)*t17;
  matrice_inv.m[2][0] = -(-a10*a21+a11*a20)*t17;
  matrice_inv.m[2][1] = -(a00*a21-t12)*t17;
  matrice_inv.m[2][2] = (t4-t8)*t17;
  return t;
}

inline Vecteur3 operator-(const Vecteur3& x, const Vecteur3& y)
{
  Vecteur3 z;
  z.v[0] = x.v[0] - y.v[0];
  z.v[1] = x.v[1] - y.v[1];
  z.v[2] = x.v[2] - y.v[2];
  return z;
}
#endif
