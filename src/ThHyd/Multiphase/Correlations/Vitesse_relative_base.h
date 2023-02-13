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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Vitesse_relative_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Vitesse_relative_base_included
#define Vitesse_relative_base_included
#include <TRUSTTab.h>
#include <Correlation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Vitesse_relative_base
//      correlations de vitesse relative de la forme
//      ur = (v_k - v_l)
//
//////////////////////////////////////////////////////////////////////////////

class Vitesse_relative_base : public Correlation_base
{
  Declare_base(Vitesse_relative_base);

public:
  struct input_t
  {
    double dh = 0.0;       // diametre hydraulique
    DoubleTab sigma; // tension superficielle sigma(ind_trav), ind_trav = (n*(N-1)-(n-1)*(n)/2) + (m-n-1)
    DoubleTab alpha; // taux de vide
    DoubleTab rho;   // masse volumique
    DoubleTab mu;    // viscosite
    DoubleTab d_bulles;   // diametre bulles
    DoubleTab k;     // energie cinetique turbulente
    DoubleTab nut;   // energie cinetique turbulente
    DoubleTab v;     // v(n, d) : vitesse de la phase n dans la direction d
    DoubleTab gradAlpha;// gradAlpha(n,d) : gradient du taux de vide ; calcule que si requis par la correlation
    DoubleVect g;    // vecteur gravite
  };
  /* valeurs de sortie */
  struct output_t
  {
    DoubleTab vr; // vr(n, m, d) : vitesse relative des phases n et m dans la direction d (v(n, d) - v(m, d))
    DoubleTab dvr;//dvr(n, m, d, D*l+d2) : derivee de la vitesse relative des phases n et m dans la direction d (v(n, d) - v(m, d))
                  // par rapport a la vitesse de la phase l selon la direction d2 
  };
  virtual void vitesse_relative(const input_t& input, output_t& output) const = 0;
  virtual bool needs_grad_alpha() const {return 0;};

protected:
  int n_l = -1, n_g = -1; // phases traitees : liquide / gaz continu
};

#endif
