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
// File:        Eval_Quick_VDF_Elem2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Quick_VDF_Elem2_included
#define Eval_Quick_VDF_Elem2_included

#include <Eval_Conv_VDF_Elem.h>
#include <Eval_Conv_VDF.h>

// .DESCRIPTION class Eval_Quick_VDF_Elem2
//
// Evaluateur VDF pour la convection
// Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Quick
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

class Eval_Quick_VDF_Elem2 : public Eval_Conv_VDF_Elem<Eval_Quick_VDF_Elem2>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_QUICK = true;
  // Methodes particuliers QUICK
  inline double dim_elem(int n1, int k) const { return la_zone->dim_elem(n1,k); }
  inline double dist_elem(int n1, int n2, int k) const { return la_zone->dist_elem_period(n1,n2,k); }
  inline int amont_amont(int face, int i) const { return la_zone->amont_amont(face, i); }
  inline double quick_fram(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  inline void quick_fram(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;
};

inline double Fram(const double& s1,const double& s2,
                   const double& s3,const double& s4)
{
  double smin0 = dmin(s4,s2);
  double smax0 = dmax(s4,s2);
  double smin1 = dmin(s3,s1);
  double smax1 = dmax(s3,s1);
  /* double sr0 = (smax0-smin0==0 ? 0 : (s3-smin0)/(smax0-smin0)); */
  /* double sr1 = (smax1-smin1==0 ? 0 : (s2-smin1)/(smax1-smin1)); */
  double sr0 = (dabs(smax0-smin0)<DMINFLOAT ? 0. : (s3-smin0)/(smax0-smin0));
  double sr1 = (dabs(smax1-smin1)<DMINFLOAT ? 0. : (s2-smin1)/(smax1-smin1));
  double fr = 2.*dmax(dabs(sr0-0.5),dabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return dmin(fr,1.0);
}


//// quick_fram pour un champ transporte scalaire
//

inline double Eval_Quick_VDF_Elem2::quick_fram(const double& psc, const int num0, const int num1,
                                               const int num0_0, const int num1_1, const int face,
                                               const DoubleTab& transporte) const
{

  double flux,trans_amont,fr;
  int ori = orientation(face);
  double dx = dist_elem(num0, num1, ori);
  double curv;
  double T0 = transporte[num0];
  double T1 = transporte[num1];
  double T0_0=-1,T1_1=-1;
  if (num0_0 != -1) T0_0 = transporte[num0_0];
  if (num1_1 != -1) T1_1 = transporte[num1_1];
  if (psc > 0)
    {
      assert(num0_0!=-1);
      trans_amont = T0;
      double dm = dim_elem(num0, ori);
      double dxam = dist_elem(num0_0, num0, ori);
      curv = ( (T1 - T0)/dx - (T0 - T0_0)/dxam )/dm ;
    }
  else
    {
      assert(num1_1!=-1);
      trans_amont = T1;
      double dm = dim_elem(num1, ori);
      double dxam = dist_elem(num1, num1_1, ori);
      curv = ( (T1_1 - T1)/dxam - (T1 - T0)/dx )/dm;
    }

  flux = 0.5*(T0+T1) - 0.125*(dx*dx)*curv;
  if ( num0_0 != -1 && num1_1 != -1 )
    fr = Fram(T0_0,T0,T1,T1_1);
  else
    fr = 1.;
  flux = ((1.-fr)*flux + fr*trans_amont)*psc;

  return flux;
}

//// quick_fram pour un champ transporte vectoriel
//

inline void Eval_Quick_VDF_Elem2::quick_fram(const double& psc, const int num0, const int num1,
                                             const int num0_0, const int num1_1, const int face,
                                             const DoubleTab& transporte,ArrOfDouble& flux) const
{
  int ori = orientation(face);
  double dx = dist_elem(num0, num1, ori);
  double T0,T0_0=0,T1,T1_1=0,trans_amont,curv;
  double fr;
  double dm0 = dim_elem(num0, ori);
  double dxam0 = (num0_0!=-1?dist_elem(num0_0, num0, ori):0);
  double dm1 = dim_elem(num1, ori);
  double dxam1 = (num1_1!=-1?dist_elem(num1, num1_1, ori):0);

  int ncomp = flux.size_array();
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
      flux(k) = 0.5*(T0+T1) - 0.125*(dx*dx)*curv;
      // On applique le filtre Fram:
      if ( num0_0 != -1 && num1_1 != -1 )
        fr = Fram(T0_0,T0,T1,T1_1);
      else
        fr = 1.;
      flux(k) = ((1.-fr)*flux(k) + fr*trans_amont)*psc;
    }
}



#endif /* Eval_Quick_VDF_Elem2_included */
