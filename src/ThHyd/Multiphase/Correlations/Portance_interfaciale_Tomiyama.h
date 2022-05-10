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
// File:        Portance_interfaciale_Tomiyama.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Portance_interfaciale_Tomiyama_included
#define Portance_interfaciale_Tomiyama_included
#include <Portance_interfaciale_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Portance_interfaciale_Tomiyama
//      coefficients de portance interfaciale d'un ecoulement a bulles deformables
//      Le coefficient renvoye par cette classe est toujours >0, c'est Portanc_interfaciale_PolyMAC_V2 qui gere les signes
//////////////////////////////////////////////////////////////////////////////

class Portance_interfaciale_Tomiyama : public Portance_interfaciale_base
{
  Declare_instanciable(Portance_interfaciale_Tomiyama);
public:
  void coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                   const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma,
                   const DoubleTab& ndv, int e, DoubleTab& coeff) const override;
protected:
  double g_=9.81;
  int n_l = -1; //phase liquide

};

#endif
