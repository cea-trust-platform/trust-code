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

#ifndef Domaine_PolyVEF_included
#define Domaine_PolyVEF_included

#include <Domaine_PolyMAC_P0.h>

class Domaine_PolyVEF : public Domaine_PolyMAC_P0
{
  Declare_base(Domaine_PolyVEF);
public :
  //surfaces orientees som->elem et som->som entre ces volumes au niveau d'une face
  inline void surf_elem_som(int f, DoubleTab& Sa) const;
  void discretiser() override;
};

class Domaine_PolyVEF_P0 : public Domaine_PolyVEF
{
  Declare_instanciable(Domaine_PolyVEF_P0);
public :
  void discretiser() override;

  //MD_Vectors pour la pression (elems + faces de bord)
  mutable MD_Vector mdv_elems_fbord;
};

class Domaine_PolyVEF_P0P1 : public Domaine_PolyVEF
{
  Declare_instanciable(Domaine_PolyVEF_P0P1);
public :
  void discretiser() override;

  //MD_Vector pour Champ_Elem_PolyVEF_P0P1 (elems + sommets)
  mutable MD_Vector mdv_elems_soms;
};

inline void Domaine_PolyVEF::surf_elem_som(int f, DoubleTab& Sa) const
{
  const IntTab& f_s = face_sommets(), &f_e = face_voisins();
  const DoubleTab& xs = domaine().coord_sommets(), &nf = face_normales();
  int i, j, e, n_sf = 0, d, D = dimension;
  double vz[3] = { 0, 0, 1 }, xa[3] = { 0, }, x_es[3] = { 0, }, x_esb[3] = { 0, }, x_ea[3] = { 0, }, fac;
  for (n_sf = 0; n_sf < f_s.dimension(1) && f_s(f, n_sf) >= 0; ) n_sf++; //nombre de sommets a la face
  Sa.set_smart_resize(1), Sa.resize(n_sf, D < 3 ? 2 : 5, D);
  for (Sa = 0, i = 0; i < n_sf; i++)
    {
      int s = f_s(f, i), ib = i + 1 < n_sf ? i + 1 : 0, sb = D < 3 ? -1 : f_s(f, ib);
      if (D == 3)
        for (d = 0; d < D; d++) xa[d] = (xs(s, d) + xs(sb, d)) / 2;
      for (j = 0; j < 2; j++)
        if ((e = f_e(f, j)) >= 0)
          {
            for (d = 0; d < D; d++) x_es[d] = (xp_(e, d) + xs(s, d)) / 2;
            if (D == 3)
              for (d = 0; d < D; d++) x_esb[d] = (xp_(e, d) + xs(sb, d)) / 2, x_ea[d] = (xp_(e, d) + 2 * xa[d]) / 3;
            //surface s -> e
            auto v = cross(D, 3, x_es, D < 3 ? vz : x_ea, &xv_(f, 0), D < 3 ? NULL : &xv_(f, 0));
            for (fac = (dot(&xp_(e, 0), &v[0], &xs(s, 0)) > 0 ? 1. : -1.) / (D - 1), d = 0; d < D; d++) Sa(i, j, d) += fac * v[d];
            if (D < 3) continue;
            //3D : surfaces sb -> e et s -> sb
            v = cross(3, 3, x_esb, x_ea, &xv_(f, 0), &xv_(f, 0));
            for (fac = (dot(&xp_(e, 0), &v[0], &xs(sb, 0)) > 0 ? 0.5 : -0.5), d = 0; d < D; d++) Sa(ib, j, d) += fac * v[d];
            v = cross(3, 3, xa, x_ea, &xv_(f, 0), &xv_(f, 0));
            for (fac = (dot(&xs(sb, 0), &v[0], &xs(s, 0)) > 0 ? 0.5 : -0.5), d = 0; d < D; d++) Sa(i, 2, d) += fac * v[d], Sa(i, 3 + j, d) += fac * v[d];
          }
        else //vers l'exterieur
          {
            auto v = cross(D, 3, &xv_(f, 0), D < 3 ? &vz[0] : xa, &xs(s, 0), D < 3 ? NULL : &xs(s, 0));
            for (fac = (dot(&nf(f, 0), &v[0]) > 0 ? 1. : -1.) / (D - 1), d = 0; d < D; d++) Sa(i, j, d) += fac * v[d]; //s -> ext
            if (D == 3)
              for (d = 0; d < D; d++) Sa(ib, j, d) += fac * v[d]; //3D : sb -> ext
          }
    }
}

#endif /* Domaine_PolyVEF_included */
