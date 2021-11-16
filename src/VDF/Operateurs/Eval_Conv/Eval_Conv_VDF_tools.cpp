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
// File:        Eval_Conv_VDF_tools.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Conv
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Eval_Conv_VDF_tools.h>

// quick pour un champ face
double Eval_Conv_VDF_tools::conv_quick_sharp_plus_impl(const double& psc,const double& vit_0, const double& vit_1,
                                                       const double& vit_0_0, const double& dx,
                                                       const double& dm, const double& dxam) const
{
  double cf, curv, delta_0 = vit_0 - vit_0_0, delta = vit_1 - vit_0, dd1,utc, delta_delta;
  curv = (delta/dx - delta_0/dxam)/dm ;
  // Calcul de cf:
  delta_delta = delta_0+delta;
  dd1 = std::fabs(delta_delta);
  if (dd1 < 1.e-5) cf = 0.125;
  else
    {
      utc = delta_0/delta_delta;
      cf = sharp2(utc);
    }
  return (0.5*(vit_0 + vit_1) - cf*(dx*dx)*curv)*psc;
}

// quick pour un champ face
double Eval_Conv_VDF_tools::conv_quick_sharp_moins_impl(const double& psc,const double& vit_0,const double& vit_1,
                                                        const double& vit_1_1,const double& dx,
                                                        const double& dm,const double& dxam) const
{
  double cf, curv, delta_1 = vit_1_1 - vit_1, delta = vit_1 - vit_0, dd1,utc, delta_delta;
  curv = ( delta_1/dxam - delta/dx )/dm ;
  // Calcul de cf:
  delta_delta = delta_1+delta;
  dd1 = std::fabs(delta_delta);
  if (dd1 < 1.e-5) cf = 0.125;
  else
    {
      utc = delta_1/delta_delta;
      cf = sharp2(utc);
    }
  return (0.5*(vit_0 + vit_1) - cf*(dx*dx)*curv)*psc;
}

int Eval_Conv_VDF_tools::face_amont_conj_axi_impl(int num_face, int k, int i, int dimension,
                                                  const IntTab& face_voisins, const IntTab& elem_faces,
                                                  const IntVect& orientation) const
{
  int ori = orientation(num_face), elem1 = face_voisins(num_face,1);
  int face,elem_bis = -2, face_conj = -1 ;

  if(elem1 != -1)
    {
      face = elem_faces(elem1, k+i*dimension);
      elem_bis = face_voisins(face,i);
      if (elem_bis != -1) face_conj = elem_faces(elem_bis, ori);
      else face_conj = -1;
    }
  if ((elem1==-1) || (elem_bis==-1))
    {
      elem1 = face_voisins(num_face,0);
      if(elem1 != -1)
        {
          face = elem_faces(elem1, k+i*dimension);
          elem_bis = face_voisins(face,i);
          assert(elem_bis!=-2);
          if (elem_bis != -1) face_conj = elem_faces(elem_bis, ori);
          else face_conj = -1;
        }
    }
  return face_conj;
}

double Eval_Conv_VDF_tools::dist_face_axi_impl(int n1, int n2, int k, const DoubleTab& xv) const
{
  double d_teta , dist ;
  if (k != 1) dist =  xv(n2,k) - xv(n1,k);
  else
    {
      d_teta = xv(n2,1) - xv(n1,1);
      if (d_teta < 0) d_teta += 2.0*M_PI;
      if (d_teta > M_PI) d_teta -= M_PI ;
      dist = d_teta*xv(n1,0);
    }
  return dist ;
}

double Eval_Conv_VDF_tools::dist_elem_axi_impl(int n1, int n2, int k, const DoubleTab& xp) const
{
  double d_teta, dist ;
  if (k != 1) dist = xp(n2,k)-xp(n1,k) ;
  else
    {
      d_teta = xp(n2,1) - xp(n1,1);
      if (d_teta < 0) d_teta += 2.0*M_PI;
      dist = d_teta * xp(n1,0);
    }
  return dist ;
}

// Calcul des coefficients g1,g2,g3,g4 a partir de dxam,dx,dxav
void Eval_Conv_VDF_tools::calcul_g_impl(const double& dxam, const double& dx, const double& dxav, double& g1, double& g2, double& g3, double& g4) const
{
  g1 = -dx*dx*(dx/2+dxav)/(4*(dx+dxam+dxav)*(dx+dxam)*dxam);
  g2 =  (dx+2*dxam)*(dx+2*dxav)/(8*dxam*(dx+dxav));
  g3 =  (dx+2*dxam)*(dx+2*dxav)/(8*dxav*(dx+dxam));
  g4 = -dx*dx*(dx/2+dxam)/(4*(dx+dxam+dxav)*dxav*(dx+dxav));
}
