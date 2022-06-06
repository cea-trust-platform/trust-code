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

#ifndef Portance_interfaciale_PolyMAC_P0_included
#define Portance_interfaciale_PolyMAC_P0_included

#include <Source_base.h>
#include <Correlation.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Portance_interfaciale_PolyMAC_P0
//    Cette classe implemente dans PolyMAC_P0 un operateur de portance interfaciale
//    de la forme F_{n_l} = - F_{k} = C_{n_l, k} (u_k - u_n_l) x rot(u_n_l) ou la phase
//    n_l est la phase liquide porteuse et k une phase gazeuse
//    le calcul de C_{n_l, k} est realise par la hierarchie Portance_interfaciale_base
// .SECTION voir aussi
//    Operateur_PolyMAC_P0_base Operateur_base
//////////////////////////////////////////////////////////////////////////////
class Portance_interfaciale_PolyMAC_P0: public Source_base
{
  Declare_instanciable(Portance_interfaciale_PolyMAC_P0);
public :
  int has_interface_blocs() const override
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  void check_multiphase_compatibility() const override {}; //of course

  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) override { };
  void associer_pb(const Probleme_base& ) override { };
  void mettre_a_jour(double temps) override { };

  const Correlation& correlation() const {return correlation_;};

private:
  Correlation correlation_; //correlation donnant le coeff de portance interfaciale
  int n_l = -1; //phase liquide

};

#endif
