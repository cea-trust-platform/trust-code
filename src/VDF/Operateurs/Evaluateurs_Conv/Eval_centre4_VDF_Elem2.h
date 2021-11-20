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
// File:        Eval_centre4_VDF_Elem2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_centre4_VDF_Elem2_included
#define Eval_centre4_VDF_Elem2_included

#include <Eval_Conv_VDF_Elem.h>
#include <Eval_Conv_VDF.h>

// .DESCRIPTION class Eval_centre4_VDF_Elem2
//
// Evaluateur VDF pour la convection Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Centre ordre 4
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

class Eval_centre4_VDF_Elem2 : public Eval_Conv_VDF_Elem<Eval_centre4_VDF_Elem2>, public Eval_Conv_VDF
{

public:
  static constexpr bool IS_CENTRE4 = true;
  // Methodes particuliers CENTRE4
  inline int amont_amont(int face, int i) const { return la_zone->amont_amont(face, i); }
  inline double dist_elem(int n1, int n2, int k) const { return la_zone->dist_elem_period(n1,n2,k); }
  inline double qcentre(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  inline void qcentre(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;
};

// Fram4
inline double Fram4(const double& s1,const double& s2, const double& s3,const double& s4)
{
  double smin0 = dmin(s4,s2), smax0 = dmax(s4,s2), smin1 = dmin(s3,s1), smax1 = dmax(s3,s1);
  double sr0 = (s3-smin0)/(smax0-smin0+DMINFLOAT), sr1 = (s2-smin1)/(smax1-smin1+DMINFLOAT);
  double fr = 2.*dmax(dabs(sr0-0.5),dabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return dmin(fr,1.0);
}

// qcentre pour un champ transporte scalaire
inline double Eval_centre4_VDF_Elem2::qcentre(const double& psc, const int num0, const int num1,
                                              const int num0_0, const int num1_1, const int face,
                                              const DoubleTab& transporte) const
{

  const int ori = orientation(face);
  double flux, dx = dist_elem(num0, num1, ori), dxam = dist_elem(num0_0, num0, ori), dxav = dist_elem(num1, num1_1, ori);
  const double T0 = transporte(num0), T0_0 = transporte(num0_0), T1 = transporte(num1), T1_1 = transporte(num1_1);
  const double g1 = -dx*dx*(dx/2+dxav)/(4*(dx+dxam+dxav)*(dx+dxam)*dxam), g2 = (dx+2*dxam)*(dx+2*dxav)/(8*dxam*(dx+dxav));
  const double g3 = (dx+2*dxam)*(dx+2*dxav)/(8*dxav*(dx+dxam)), g4 = -dx*dx*(dx/2+dxam)/(4*(dx+dxam+dxav)*dxav*(dx+dxav));
  flux =  g1*T0_0+g2*T0+g3*T1+g4*T1_1;
  return flux*psc;
}

// qcentre pour un champ transporte vectoriel
inline void Eval_centre4_VDF_Elem2::qcentre(const double& psc, const int num0, const int num1,
                                            const int num0_0, const int num1_1, const int face,
                                            const DoubleTab& transporte,ArrOfDouble& flux) const
{

  int k, ncomp = flux.size_array(), ori = orientation(face);
  const double dx = dist_elem(num0, num1, ori), dxam = dist_elem(num0_0, num0, ori), dxav = dist_elem(num1, num1_1, ori);
  ArrOfDouble T0(ncomp), T0_0(ncomp), T1(ncomp), T1_1(ncomp);
  for (k=0; k<ncomp; k++)
    {
      T0(k) = transporte(num0,k);
      T0_0(k) = transporte(num0_0,k);
      T1(k) = transporte(num1,k);
      T1_1(k) = transporte(num1_1,k);
    }
  const double g1 = -dx*dx*(dx/2+dxav)/(4*(dx+dxam+dxav)*(dx+dxam)*dxam), g2 = (dx+2*dxam)*(dx+2*dxav)/(8*dxam*(dx+dxav));
  const double g3 = (dx+2*dxam)*(dx+2*dxav)/(8*dxav*(dx+dxam)), g4 = -dx*dx*(dx/2+dxam)/(4*(dx+dxam+dxav)*dxav*(dx+dxav));
  for (k=0; k<ncomp; k++) flux(k) =( g1*T0_0(k) + g2*T0(k) + g3*T1(k) + g4*T1_1(k) ) * psc ;
}

#endif /* Eval_centre4_VDF_Elem2_included */
