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

#ifndef Viscosite_turbulente_base_included
#define Viscosite_turbulente_base_included
#include <TRUSTTab.h>
#include <Correlation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Viscosite_turbulente_base
//    correlations de viscosite turbulente decrivant le tenseur de Reynolds R_{ij} = - < u'_i u'_j>
//    Methodes implementees :
//    - eddy_viscosity(nu_t) -> remplit le DoubleTab "nu_t" tel que
//                              <u'_i u'_j>  = - nu_t (dui / dx_j + duj / dx_i + (partie trace))
//    - reynolds_stress(R_ij) -> remplit R_ij = < u'_i u'_j> (pour utilisation par un modele GGDH dans une autre equation)
//                               format : R_ij(e, n, i, j) avec e l'indice d'element, n l'indice de phase, (i, j) les composantes
//    - k_over_eps(k_sur_eps) -> renvoie la quantite "k / epsilon" (utilisee par exemple dans le GGDH)
//    Dans les deux cas, les tableaux doivent etre dimensionnes par la fonction appelante.
//////////////////////////////////////////////////////////////////////////////

class Viscosite_turbulente_base : public Correlation_base
{
  Declare_base(Viscosite_turbulente_base);
public:
  virtual void eddy_viscosity(DoubleTab& nu_t) const = 0;
  virtual void reynolds_stress(DoubleTab& R_ij) const = 0;
  virtual void k_over_eps(DoubleTab& k_sur_eps) const = 0;
  virtual void eps(DoubleTab& eps) const = 0;
  inline double limiteur() const {return limiter_;};
  virtual int gradu_required() const  {  return 0; };

private:
  double limiter_ = 0.01; //"limiteur" fournissant une valeur minimale de la viscosite turbulente
};

#endif
