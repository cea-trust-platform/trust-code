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
// File:        Eval_centre_VDF_Elem2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_centre_VDF_Elem2_included
#define Eval_centre_VDF_Elem2_included

#include <Eval_Conv_VDF_Elem.h>
#include <Eval_Conv_VDF.h>

// .DESCRIPTION class Eval_centre_VDF_Elem2
//
// Evaluateur VDF pour la convection Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Centre ordre 2
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

class Eval_centre_VDF_Elem2 : public Eval_Conv_VDF_Elem<Eval_centre_VDF_Elem2>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_CENTRE = true;
  inline int amont_amont(int face, int i) const { return la_zone->amont_amont(face, i); }
  inline double dim_elem(int n1, int k) const { return la_zone->dim_elem(n1,k); }
  inline double dist_elem(int n1, int n2, int k) const { return la_zone->dist_elem(n1,n2,k); }
  inline double dist_face_elem1(int num_face,int n1) const { return la_zone->dist_face_elem1(num_face, n1); }

  inline double qcentre(const double& psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face,const DoubleTab& transporte) const
  {
    return qcentre2_impl(psc,num0,num1,num0_0,num1_1,face,transporte);
  }

  inline void qcentre(const double& psc, const int num0, const int num1, const int num0_0, const int num1_1,const int face,const DoubleTab& transporte,ArrOfDouble& flux) const
  {
    qcentre2_impl(psc,num0,num1,num0_0,num1_1,face,transporte,flux);
  }
};

#endif /* Eval_centre_VDF_Elem2_included */
