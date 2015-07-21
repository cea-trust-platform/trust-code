/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Diffu_totale_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Diffu_totale_base_included
#define Diffu_totale_base_included
#include <Ref_Eq_couch_lim.h>
#include <DoubleTab.h>
#include <Ref_MuLambda_TBLE_base.h>
class Eq_couch_lim ;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Diffu_totale_base
//    Classe abstraite calculant la diffusivite totale (somme diffusivite
//    moleculaire et diffusivite turbulente) dans les equations de
//    couche limite simplifiees necessaires
//    a l'utilisation des lois de parois de type Balaras
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class Diffu_totale_base : public Objet_U
{
  Declare_base_sans_constructeur(Diffu_totale_base);

public :

  Diffu_totale_base() ;
  virtual double calculer_a_local(int ind) = 0 ;
  virtual double calculer_D_local(int ind) = 0 ;
  void associer_eqn(Eq_couch_lim& e)
  {
    eq_couch_lim = e;
  };
  void associer_mulambda(const MuLambda_TBLE_base& m)
  {
    mu_lambda = m;
  };
  double getKappa()
  {
    return Kappa;
  };
  void setKappa(double k)
  {
    Kappa=k;
  };

protected :

  REF(Eq_couch_lim) eq_couch_lim;
  REF(MuLambda_TBLE_base) mu_lambda;

private:
  double Kappa;
};


#endif
