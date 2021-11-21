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
// File:        Eval_Conv_VDF_tools.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Conv_VDF_tools_included
#define Eval_Conv_VDF_tools_included

#include <Double.h>
class DoubleTab;
class ArrOfDouble;

class Eval_Conv_VDF_tools
{
public:
  virtual ~Eval_Conv_VDF_tools() {}
  // DANGER !!!! FAUT JAMAIS ENTRER
  virtual int amont_amont(int face, int i) const { return dont_call<int>(__func__); }
  virtual double qcentre(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const { return dont_call<double>(__func__); }
  virtual double quick_fram(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const { return dont_call<double>(__func__); }
  virtual void qcentre(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const { return dont_call<void>(__func__); }
  virtual void quick_fram(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const { return dont_call<void>(__func__); }

  virtual int face_amont_conj(int ,int ,int ) const { return dont_call<int>(__func__); }
  virtual int face_amont_princ(int ,int ) const { return dont_call<int>(__func__); }
  virtual double dim_elem(int ,int ) const { return dont_call<double>(__func__); }
  virtual double dim_face(int ,int ) const { return dont_call<double>(__func__); }
//  virtual double dist_face(int ,int ,int ) const { return dont_call<double>(__func__); }
  virtual double dist_elem(int ,int ,int ) const { return dont_call<double>(__func__); }
  virtual double dist_elem_period(int , int , int ) const { return dont_call<double>(__func__); }
//  virtual double dist_face_period(int ,int ,int ) const { return dont_call<double>(__func__); }
  virtual double conv_quick_sharp_plus(const double& ,const double& , const double& , const double& , const double& , const double& , const double& ) const { return dont_call<double>(__func__); }
  virtual double conv_quick_sharp_moins(const double& ,const double& ,const double& , const double& ,const double& , const double& ,const double& ) const { return dont_call<double>(__func__); }


protected:
  double qcentre2_impl(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  double qcentre4_impl(const int ,const double , const double , const double ,
                       const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  double quick_fram_impl(const int ,const double , const double , const double ,const double, const double,
                         const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  void qcentre2_impl(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;
  void qcentre4_impl(const int ,const double , const double , const double ,
                     const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;
  void quick_fram_impl(const int ,const double , const double , const double ,const double, const double,
                       const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;

private:
  template <typename type>
  type dont_call (const char * nom_fct) const
  {
    Cerr << "What ??? You should not call the function " << nom_fct << finl;
    throw;
  }
};

// Fram4 pour centre 4 (inutile je pense mais bon)
// TODO : FIXME : should use std::min ... :-)
inline double Fram4(const double& s1,const double& s2, const double& s3,const double& s4)
{
  double smin0 = dmin(s4,s2), smax0 = dmax(s4,s2), smin1 = dmin(s3,s1), smax1 = dmax(s3,s1);
  double sr0 = (s3-smin0)/(smax0-smin0+DMINFLOAT), sr1 = (s2-smin1)/(smax1-smin1+DMINFLOAT);
  double fr = 2.*dmax(dabs(sr0-0.5),dabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return dmin(fr,1.0);
}

// Fram pour QUICK
// TODO : FIXME : should use std::min ... :-)
inline double Fram(const double& s1,const double& s2, const double& s3,const double& s4)
{
  double smin0 = dmin(s4,s2), smax0 = dmax(s4,s2), smin1 = dmin(s3,s1), smax1 = dmax(s3,s1);
  double sr0 = (dabs(smax0-smin0)<DMINFLOAT ? 0. : (s3-smin0)/(smax0-smin0));
  double sr1 = (dabs(smax1-smin1)<DMINFLOAT ? 0. : (s2-smin1)/(smax1-smin1));
  double fr = 2.*dmax(dabs(sr0-0.5),dabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return dmin(fr,1.0);
}

// Fonction de calcul de cf(limiteur de pente) dans le schema Quick-sharp
inline double sharp2(const double& utc)
{
  double cf;
  if ( (utc <= -1) || (utc >= 1.5) ) cf = 0.125;
  else if ((utc > -1) && (utc <= 0) ) cf = 0.5 + 0.375*utc;
  else if ((utc <= 0.25) && (utc > 0) ) cf = 0.5 - 0.625*sqrt(utc);
  else if ((utc > 0.25) && (utc <= 1.) ) cf = 0.25*(1.-utc);
  else cf = 0.25*(utc-1.);
  return cf;
}

#endif /* Eval_Conv_VDF_tools_included */
