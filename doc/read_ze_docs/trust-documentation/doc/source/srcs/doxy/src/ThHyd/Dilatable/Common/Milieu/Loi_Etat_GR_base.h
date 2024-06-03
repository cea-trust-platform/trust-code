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

#ifndef Loi_Etat_GR_base_included
#define Loi_Etat_GR_base_included

#ifndef RU_
//constante des gaz
#define RU_ 8.3143*4.18448
#endif

#include <Loi_Etat_base.h>

/*! @brief classe Loi_Etat_GR_base Cette classe represente la loi d'etat base pour les gaz reels.
 *
 *      Elle definit un fluide dilatable dont la loi d'etat est :
 *          rho=rho(Pth,H)
 *          T  =  T(Pth,H)
 *
 * @sa Fluide_Dilatable_base Loi_Etat_base
 */

class Loi_Etat_GR_base : public Loi_Etat_base
{
  Declare_base_sans_constructeur(Loi_Etat_GR_base);

public :
  Loi_Etat_GR_base();
  void initialiser_inco_ch() override;
  void calculer_lambda() override;
  void initialiser() override;
  void remplir_T() override;
  void calculer_Cp() override;
  const Nom type_fluide() const override;
  double De_DP(double,double) const override;
  double De_DT(double,double) const override;
  double Cp_calc(double,double) const;
  double inverser_Pth(double,double) override;
  void calculer_masse_volumique() override;

  // Methodes virtuelles pures
  virtual double calculer_temperature(double,double) = 0;
  double calculer_H(double,double) const override = 0;
  double Drho_DP(double,double) const override = 0;
  double Drho_DT(double,double) const override = 0;
  virtual double DT_DH(double,double) const = 0;
  double calculer_masse_volumique(double,double) const override = 0;

  // Methodes inlines
  inline double masse_molaire() const { return MMole_; }

protected :
  double MMole_, Cp_, R;
  DoubleTab tab_TempC, tab_Cp;
};

#endif /* Loi_Etat_GR_base_included */
