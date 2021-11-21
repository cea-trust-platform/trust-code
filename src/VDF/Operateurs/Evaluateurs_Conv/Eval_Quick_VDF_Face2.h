/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_Quick_VDF_Face2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Quick_VDF_Face2_included
#define Eval_Quick_VDF_Face2_included

#include <Eval_Conv_VDF_Face.h>
#include <Eval_Conv_VDF.h>

// .DESCRIPTION class Eval_Quick_VDF_Face2
// Evaluateur VDF pour la convection
// Le champ convecte est un Champ_Face
// Schema de convection Quick
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

class Eval_Quick_VDF_Face2 : public Eval_Conv_VDF_Face<Eval_Quick_VDF_Face2>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_QUICK = true;

  inline int face_amont_conj(int num_face,int i,int k) const { return la_zone->face_amont_conj(num_face, i, k); }
  inline int face_amont_princ(int num_face,int i) const { return la_zone->face_amont_princ(num_face, i); }
  inline double dim_elem(int n1,int k) const { return la_zone->dim_elem(n1,k); }
  inline double dim_face(int n1,int k) const { return la_zone->dim_face(n1,k); }
  inline double dist_face(int n1,int n2,int k) const { return la_zone->dist_face(n1,n2,k); }
  inline double dist_elem(int n1,int n2,int k) const { return la_zone->dist_elem(n1,n2,k); }
  inline double dist_elem_period(int n1, int n2, int k) const { return la_zone->dist_elem_period(n1,n2,k); }
  inline double dist_face_period(int n1,int n2,int k) const { return la_zone->dist_face_period(n1,n2,k); }
  inline double conv_quick_sharp_plus(const double& psc,const double& vit_0, const double& vit_1,
                                      const double& vit_0_0, const double& dx, const double& dm, const double& dxam) const ;

  inline double conv_quick_sharp_moins(const double& psc,const double& vit_0,const double& vit_1,
                                       const double& vit_1_1,const double& dx, const double& dm,const double& dxam) const;
};


inline double Eval_Quick_VDF_Face2::conv_quick_sharp_plus(const double& psc,const double& vit_0, const double& vit_1,
                                                          const double& vit_0_0, const double& dx,
                                                          const double& dm, const double& dxam) const
{
  double cf, curv, delta_0 = vit_0 - vit_0_0, delta = vit_1 - vit_0, dd1,utc, delta_delta;
  curv = (delta/dx - delta_0/dxam)/dm ;
  // Calcul de cf:
  delta_delta = delta_0+delta;
  dd1 = dabs(delta_delta);
  if (dd1 < 1.e-5) cf = 0.125;
  else
    {
      utc = delta_0/delta_delta;
      cf = sharp2(utc);
    }
  return (0.5*(vit_0 + vit_1) - cf*(dx*dx)*curv)*psc;
}

inline double Eval_Quick_VDF_Face2::conv_quick_sharp_moins(const double& psc,const double& vit_0,const double& vit_1,
                                                           const double& vit_1_1,const double& dx,
                                                           const double& dm,const double& dxam) const
{
  double cf, curv, delta_1 = vit_1_1 - vit_1, delta = vit_1 - vit_0, dd1,utc, delta_delta;
  curv = ( delta_1/dxam - delta/dx )/dm ;
  // Calcul de cf:
  delta_delta = delta_1+delta;
  dd1 = dabs(delta_delta);
  if (dd1 < 1.e-5) cf = 0.125;
  else
    {
      utc = delta_1/delta_delta;
      cf = sharp2(utc);
    }
  return (0.5*(vit_0 + vit_1) - cf*(dx*dx)*curv)*psc;
}

#endif /* Eval_Quick_VDF_Face2_included */
