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
// File:        Fluide_reel_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_reel_base_included
#define Fluide_reel_base_included

#include <Fluide_base.h>
#include <Param.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Fluide_reel_base
//    Cette classe represente un fluide reel ainsi que
//    ses proprietes:
//        - viscosite cinematique, (mu)
//        - viscosite dynamique,   (nu)
//        - masse volumique,       (rho)
//        - diffusivite,           (alpha)
//        - conductivite,          (lambda)
//        - capacite calorifique,  (Cp)
//        - dilatabilite thermique du constituant (beta_co)
// .SECTION voir aussi
//     Milieu_base
//////////////////////////////////////////////////////////////////////////////
class Fluide_reel_base: public Fluide_base
{
  Declare_base(Fluide_reel_base);
public :

  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  virtual void mettre_a_jour(double temps);
  virtual int initialiser(const double& temps);
  virtual void set_param(Param& param);
  virtual void set_T0(double T0);
protected :

  // densite
  virtual double     rho_(const double T, const double P) const = 0;
  virtual double  dP_rho_(const double T, const double P) const = 0;
  virtual double  dT_rho_(const double T, const double P) const = 0;
  // enthalpie
  virtual double       h_(const double T, const double P) const = 0;
  virtual double    dP_h_(const double T, const double P) const = 0;
  virtual double    dT_h_(const double T, const double P) const = 0;
  // lois champs "faibles" -> pas de derivees
  virtual double      cp_(const double T, const double P) const = 0;
  virtual double    beta_(const double T, const double P) const = 0;
  virtual double      mu_(const double T) const = 0;
  virtual double  lambda_(const double T) const = 0;

  void mettre_a_jour_tabs(const double t);

  int isotherme_ = 0;
  double T_ref_ = -1;
  double P_ref_ = -1;
};

#endif
