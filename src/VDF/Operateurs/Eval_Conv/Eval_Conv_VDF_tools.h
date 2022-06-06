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

#ifndef Eval_Conv_VDF_tools_included
#define Eval_Conv_VDF_tools_included

#include <TRUSTTab.h>
#include <Double.h>

class Eval_Conv_VDF_tools
{
public:
  virtual ~Eval_Conv_VDF_tools() {}
  // DANGER !!!! FAUT JAMAIS ENTRER
  virtual int amont_amont(int face, int i) const { return dont_call<int>(__func__); }
  virtual int face_amont_conj(int ,int ,int ) const { return dont_call<int>(__func__); }
  virtual int face_amont_princ(int ,int ) const { return dont_call<int>(__func__); }
  virtual double dim_elem(int ,int ) const { return dont_call<double>(__func__); }
  virtual double dim_face(int ,int ) const { return dont_call<double>(__func__); }
  virtual double dist_elem(int ,int ,int ) const { return dont_call<double>(__func__); }
  virtual double dist_elem_period(int , int , int ) const { return dont_call<double>(__func__); }
  virtual double conv_centre(const double,const double,const double,const double,const double,double,double,double,double) const { return dont_call<double>(__func__); }
  virtual double conv_quick_sharp_plus(const double,const double,const double,const double,const double,const double,const double) const { return dont_call<double>(__func__); }
  virtual double conv_quick_sharp_moins(const double,const double,const double,const double,const double,const double,const double) const { return dont_call<double>(__func__); }
  virtual void calcul_g(const double,const double,const double,double&,double&,double&,double&) const { return dont_call<void>(__func__); }

  template <typename Type_Double>
  void qcentre(const double, const int, const int, const int, const int, const int, const DoubleTab&, Type_Double& ) const { return dont_call<void>(__func__); }

  template <typename Type_Double>
  void quick_fram(const double, const int, const int, const int, const int, const int, const DoubleTab&, Type_Double& ) const { return dont_call<void>(__func__); }

protected:
  int face_amont_conj_axi_impl(int ,int ,int ,int , const IntTab& , const IntTab& , const IntVect&) const;
  double dist_face_axi_impl(int ,int ,int ,const DoubleTab&) const;
  double dist_elem_axi_impl(int ,int ,int ,const DoubleTab&) const;
  double conv_quick_sharp_plus_impl(const double,const double,const double,const double,const double,const double,const double) const ;
  double conv_quick_sharp_moins_impl(const double,const double,const double,const double,const double,const double,const double) const;
  void calcul_g_impl(const double,const double,const double,double&,double&,double&,double& ) const ;

  template <typename Type_Double>
  void qcentre2_impl(const double,const int,const int,const int,const int,const int,const DoubleTab&,Type_Double&) const;

  template <typename Type_Double>
  void qcentre4_impl(const int,const double,const double,const double,const double,const int,const int,const int,const int,const int,const DoubleTab&,Type_Double&) const;

  template <typename Type_Double>
  void quick_fram_impl(const int,const double,const double,const double,const double,const double,const double,const int,const int,const int,const int,const int,const DoubleTab&,Type_Double&) const;

private:
  template <typename type>
  type dont_call (const char * nom_fct) const
  {
    Cerr << "What ??? You should not call the function " << nom_fct << finl;
    throw;
  }
};

// Fram4 pour centre 4 (inutile je pense mais bon)
inline double Fram4(const double s1,const double s2, const double s3,const double s4)
{
  double smin0 = std::min(s4,s2), smax0 = std::max(s4,s2), smin1 = std::min(s3,s1), smax1 = std::max(s3,s1);
  double sr0 = (s3-smin0)/(smax0-smin0+DMINFLOAT), sr1 = (s2-smin1)/(smax1-smin1+DMINFLOAT);
  double fr = 2.*std::max(std::fabs(sr0-0.5),std::fabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return std::min(fr,1.0);
}

// Fram pour QUICK
inline double Fram(const double s1,const double s2, const double s3,const double s4)
{
  double smin0 = std::min(s4,s2), smax0 = std::max(s4,s2), smin1 = std::min(s3,s1), smax1 = std::max(s3,s1);
  double sr0 = (std::fabs(smax0-smin0)<DMINFLOAT ? 0. : (s3-smin0)/(smax0-smin0));
  double sr1 = (std::fabs(smax1-smin1)<DMINFLOAT ? 0. : (s2-smin1)/(smax1-smin1));
  double fr = 2.*std::max(std::fabs(sr0-0.5),std::fabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return std::min(fr,1.0);
}

// Fonction de calcul de cf(limiteur de pente) dans le schema Quick-sharp
inline double sharp2(const double utc)
{
  double cf;
  if ( (utc <= -1) || (utc >= 1.5) ) cf = 0.125;
  else if ((utc > -1) && (utc <= 0) ) cf = 0.5 + 0.375*utc;
  else if ((utc <= 0.25) && (utc > 0) ) cf = 0.5 - 0.625*sqrt(utc);
  else if ((utc > 0.25) && (utc <= 1.) ) cf = 0.25*(1.-utc);
  else cf = 0.25*(utc-1.);
  return cf;
}

template <typename Type_Double>
void Eval_Conv_VDF_tools::qcentre2_impl(const double psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face,
                                        const DoubleTab& transporte,Type_Double& flux) const
{
  int k, ncomp = flux.size_array();
  Type_Double T0(ncomp), T1(ncomp);
  for (k=0; k<ncomp; k++)
    {
      T0[k] = transporte(num0,k);
      T1[k] = transporte(num1,k);
    }
  for (k=0; k<ncomp; k++) flux[k] =0.5 * (T0[k] + T1[k]) * psc ;
}

template <typename Type_Double>
void Eval_Conv_VDF_tools::qcentre4_impl(const int ori,const double dx, const double dxam, const double dxav, const double psc, const int num0, const int num1,
                                        const int num0_0, const int num1_1, const int face, const DoubleTab& transporte,Type_Double& flux) const
{
  int k, ncomp = flux.size_array();
  Type_Double T0(ncomp), T0_0(ncomp), T1(ncomp), T1_1(ncomp);
  for (k=0; k<ncomp; k++)
    {
      T0[k] = transporte(num0,k);
      T0_0[k] = transporte(num0_0,k);
      T1[k] = transporte(num1,k);
      T1_1[k] = transporte(num1_1,k);
    }
  const double g1 = -dx*dx*(dx/2+dxav)/(4*(dx+dxam+dxav)*(dx+dxam)*dxam), g2 = (dx+2*dxam)*(dx+2*dxav)/(8*dxam*(dx+dxav));
  const double g3 = (dx+2*dxam)*(dx+2*dxav)/(8*dxav*(dx+dxam)), g4 = -dx*dx*(dx/2+dxam)/(4*(dx+dxam+dxav)*dxav*(dx+dxav));
  for (k=0; k<ncomp; k++) flux[k] =( g1*T0_0[k] + g2*T0[k] + g3*T1[k] + g4*T1_1[k] ) * psc ;
}

template <typename Type_Double>
void Eval_Conv_VDF_tools::quick_fram_impl(const int ori,const double dx, const double dm0, const double dxam0, const double dm1, const double dxam1, const double psc,
                                          const int num0, const int num1, const int num0_0, const int num1_1, const int face, const DoubleTab& transporte,Type_Double& flux) const
{
  const int ncomp = flux.size_array();
  double fr, T0,T0_0=0,T1,T1_1=0,trans_amont,curv;

  for (int k=0; k<ncomp; k++)
    {
      T0 = transporte(num0,k);
      T0_0 = (num0_0!=-1?transporte(num0_0,k):0);
      T1 = transporte(num1,k);
      T1_1 = (num1_1!=-1?transporte(num1_1,k):0);

      if (psc > 0)
        {
          assert(num0_0!=-1);
          trans_amont = T0;
          curv = ( (T1 - T0)/dx - (T0 - T0_0)/dxam0 )/dm0 ;
        }
      else
        {
          assert(num1_1!=-1);
          trans_amont = T1;
          curv = ( (T1_1 - T1)/dxam1 - (T1 - T0)/dx )/dm1;
        }
      flux[k] = 0.5*(T0+T1) - 0.125*(dx*dx)*curv;
      // On applique le filtre Fram:
      fr = ( num0_0 != -1 && num1_1 != -1 ) ? Fram(T0_0,T0,T1,T1_1) : 1.;
      flux[k] = ((1.-fr)*flux[k] + fr*trans_amont)*psc;
    }
}

#endif /* Eval_Conv_VDF_tools_included */
