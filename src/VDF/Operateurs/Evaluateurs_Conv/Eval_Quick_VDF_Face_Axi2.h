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
// File:        Eval_Quick_VDF_Face_Axi2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Quick_VDF_Face_Axi2_included
#define Eval_Quick_VDF_Face_Axi2_included

#include <Eval_Conv_VDF_Face.h>
#include <Eval_Conv_VDF.h>

// .DESCRIPTION class Eval_Quick_VDF_Face_Axi2
// Evaluateur VDF pour la convection en coordonnees cylindriques : Le champ convecte est un Champ_Face
// Schema de convection Quick
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

class Eval_Quick_VDF_Face_Axi2 : public Eval_Conv_VDF_Face<Eval_Quick_VDF_Face_Axi2>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_AXI = true;
  static constexpr bool IS_QUICK = true;

  inline int face_amont_princ(int num_face,int i) const { return la_zone->face_amont_princ(num_face, i); }
  inline int face_amont_conj(int ,int ,int ) const;
  inline double dist_face(int ,int ,int ) const;
  inline double dist_elem_period(int n1, int n2, int k) const { return dist_face(n1,n2,k); }
  inline double dim_face(int ,int ) const;
  inline double dist_elem(int ,int ,int ) const;
  inline double dim_elem(int ,int ) const;
  inline double conv_quick_sharp_plus(const double& psc,const double& vit_0, const double& vit_1,
                                      const double& vit_0_0, const double& dx, const double& dm, const double& dxam) const ;

  inline double conv_quick_sharp_moins(const double& psc,const double& vit_0,const double& vit_1,
                                       const double& vit_1_1,const double& dx, const double& dm,const double& dxam) const;
};

inline double Eval_Quick_VDF_Face_Axi2::dim_elem(int n1, int k) const
{
  const IntTab& elem_faces_ = la_zone->elem_faces();
  return dist_face(elem_faces_(n1,k), elem_faces_(n1,k+dimension), k) ;
}

inline double Eval_Quick_VDF_Face_Axi2::dist_elem(int n1, int n2, int k) const
{
  const DoubleTab& xp_ = la_zone->xp();
  double d_teta, dist ;
  if (k != 1) dist = xp_(n2,k)-xp_(n1,k) ;
  else
    {
      d_teta = xp_(n2,1) - xp_(n1,1);
      if (d_teta < 0) d_teta += 2.0*M_PI;
      dist = d_teta * xp_(n1,0);
    }
  return dist ;
}

inline double Eval_Quick_VDF_Face_Axi2::dist_face(int n1, int n2, int k) const
{
  double d_teta , dist ;
  const DoubleTab& xv_ = la_zone->xv();
  if (k != 1) dist =  xv_(n2,k) - xv_(n1,k);
  else
    {
      d_teta = xv_(n2,1) - xv_(n1,1);
      if (d_teta < 0) d_teta += 2.0*M_PI;
      if (d_teta > M_PI) d_teta -= M_PI ;
      dist = d_teta*xv_(n1,0);
    }
  return dist ;
}

inline double Eval_Quick_VDF_Face_Axi2::dim_face(int n1, int k) const
{
  const IntTab& face_voisins_ = la_zone->face_voisins();
  const int elem0 = face_voisins_(n1,0), elem1 = face_voisins_(n1,1) ;
  if (elem0 < 0 ) return  dim_elem(elem1, k) ;
  if (elem1 < 0 ) return  dim_elem(elem0, k) ;
  return (dim_elem(elem0, k) + dim_elem(elem1, k)) * 0.5 ;
}

inline int Eval_Quick_VDF_Face_Axi2::face_amont_conj(int num_face, int k, int i) const
{
  const IntTab& face_voisins_ = la_zone->face_voisins();
  const IntTab& elem_faces_   = la_zone->elem_faces();
  const IntVect& orientation_ = la_zone->orientation();
  int ori = orientation_(num_face), elem1 = face_voisins_(num_face,1);
  int face,elem_bis = -2, face_conj = -1 ;

  if(elem1 != -1)
    {
      face = elem_faces_(elem1, k+i*dimension);
      elem_bis = face_voisins_(face,i);
      if (elem_bis != -1) face_conj = elem_faces_(elem_bis, ori);
      else face_conj = -1;
    }
  if ((elem1==-1) || (elem_bis==-1))
    {
      elem1 = face_voisins_(num_face,0);
      if(elem1 != -1)
        {
          face = elem_faces_(elem1, k+i*dimension);
          elem_bis = face_voisins_(face,i);
          assert(elem_bis!=-2);
          if (elem_bis != -1) face_conj = elem_faces_(elem_bis, ori);
          else face_conj = -1;
        }
    }
  return face_conj;
}

inline double Eval_Quick_VDF_Face_Axi2::conv_quick_sharp_plus(const double& psc,const double& vit_0, const double& vit_1,
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

inline double Eval_Quick_VDF_Face_Axi2::conv_quick_sharp_moins(const double& psc,const double& vit_0,const double& vit_1,
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

#endif /* Eval_Quick_VDF_Face_Axi2_included */
