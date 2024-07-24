/****************************************************************************
* Copyright (c) 2024, CEA
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
#ifndef Vecteur3_included
#define Vecteur3_included
#include <assert.h>
#include <TRUSTTab.h>

//  Attention: le constructeur par defaut n'initialise pas le vecteur !
class Vecteur3
{
public:
  Vecteur3() {};
  Vecteur3(const Vecteur3& w)
  {
    for (int i = 0; i < 3; i++)
      v[i] = w.v[i];
  }
  Vecteur3(double x, double y, double z)
  {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }
  void  set(double x, double y, double z)
  {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }

  void set(const DoubleTab& tab, int i)
  {
    assert(tab.line_size() == 3);
    assert(i >= 0 && i < tab.dimension_tot(0));
    const double *ptr = tab.addr() + i * 3;
    for (int j = 0; j < 3; j++)
      v[j] = ptr[j];
  }
  double length() const
  {
    return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  };
  Vecteur3(const DoubleTab& tab, int i)
  {
    assert(tab.line_size() == 3);
    assert(i >= 0 && i < tab.dimension_tot(0));
    const double *ptr = tab.addr() + i * 3;
    for (int j = 0; j < 3; j++)
      v[j] = ptr[j];
  }
  Vecteur3(const ArrOfDouble& arr)
  {
    assert(arr.size_array() == 3);
    for (int i = 0; i < 3; i++)
      v[i] = arr[i];
  }
  Vecteur3& operator=(double x)
  {
    for (int i = 0; i < 3; i++)
      v[i] = x;
    return *this;
  }
  Vecteur3& operator*=(double x)
  {
    for (int i = 0; i < 3; i++)
      v[i] *= x;
    return *this;
  }
  Vecteur3& operator+=(const Vecteur3& x)
  {
    for (int i = 0; i < 3; i++)
      v[i] += x[i];
    return *this;
  }

  Vecteur3& operator=(const Vecteur3& w)
  {
    for (int i = 0; i < 3; i++)
      v[i] = w[i];
    return *this;
  }
  double               operator[](int i) const
  {
    assert(i>=0 && i<3);
    return v[i];
  }
  double&              operator[](int i)
  {
    assert(i>=0 && i<3);
    return v[i];
  }
  inline        double norme_Linfini();
  static inline void   produit_vectoriel(const Vecteur3& x, const Vecteur3& y, Vecteur3& resu);
  static inline double produit_scalaire(const Vecteur3& x, const Vecteur3& y);
  friend class Matrice33;
  friend Vecteur3 operator-(const Vecteur3&, const Vecteur3&);
protected:
  double v[3];
  void init()
  {
    for (int i = 0; i < 3; i++)
      v[i] = 0.;
  }
};

inline Vecteur3 operator-(const Vecteur3&, const Vecteur3&);

#endif
