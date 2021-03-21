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
// File:        Flux_interfacial_CoviMAC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/CoviMAC
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Flux_interfacial_CoviMAC_included
#define Flux_interfacial_CoviMAC_included

#include <Source_base.h>
#include <Ref_Correlation.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Flux_interfacial_CoviMAC
//    Cette classe implemente dans CoviMAC un operateur de frottement interfacial
//    de la forme F_{kl} = - F_{lk} = - C_{kl} (u_k - u_l)
//    le calcul de C_{kl} est realise par la hierarchie Coefficient_Flux_interfacial_base
// .SECTION voir aussi
//    Operateur_CoviMAC_base Operateur_base
//////////////////////////////////////////////////////////////////////////////
class Flux_interfacial_CoviMAC: public Source_base
{
  Declare_instanciable(Flux_interfacial_CoviMAC);
public :
  int has_interface_blocs() const
  {
    return 1;
  }
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const;
  void check_multiphase_compatibility() const {}; //of course

  // DoubleTab& ajouter(DoubleTab& sec) const
  // {
  //   abort();
  //   return sec;
  // }
  // DoubleTab& calculer(DoubleTab& sec) const
  // {
  //   abort();
  //   return sec;
  // }

  DoubleTab& ajouter(DoubleTab& secmem) const
  {
    ajouter_blocs({}, secmem, {});
    return secmem;
  }

  DoubleTab& calculer(DoubleTab& secmem) const
  {
    secmem = 0;
    return ajouter(secmem);
  }
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) { };
  void associer_pb(const Probleme_base& ) { };
  void mettre_a_jour(double temps) { };
private:
  REF(Correlation) correlation_; //correlation donnant le coeff de frottement interfacial
};

#endif
