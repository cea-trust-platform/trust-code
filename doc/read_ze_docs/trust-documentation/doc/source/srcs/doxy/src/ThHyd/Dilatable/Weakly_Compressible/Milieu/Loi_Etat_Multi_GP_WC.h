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

#ifndef Loi_Etat_Multi_GP_WC_included
#define Loi_Etat_Multi_GP_WC_included

#include <Loi_Etat_Multi_GP_base.h>
#include <Champ_Inc.h>

/*! @brief classe Loi_Etat_Multi_GP_WC Cette classe represente la loi d'etat pour un melange de gaz parfaits.
 *
 *
 *
 * @sa Fluide_Dilatable_base Loi_Etat_base Loi_Etat_Multi_GP_base
 */

class Loi_Etat_Multi_GP_WC : public Loi_Etat_Multi_GP_base
{
  Declare_instanciable_sans_constructeur(Loi_Etat_Multi_GP_WC);

public :
  Loi_Etat_Multi_GP_WC();
  void initialiser_inco_ch() override;
  void calculer_masse_molaire(DoubleTab& M) const override;
  void calculer_tab_Cp(DoubleTab& cp) const override;
  void calculer_masse_volumique() override;
  void calculer_mu_sur_Sc() override; // returns rho * D
  void calculer_nu_sur_Sc() override; // returns D
  double calculer_masse_volumique(double,double) const override;
  double calculer_masse_volumique(double P,double T,double r) const override;

  // Methodes inlines
  inline const Champ_Don& masse_molaire_especes() const { return molar_mass_; }
  inline const Champ_Don& visc_dynamique_especes() const { return mu_; }
  inline const Champ_Don& coeff_diffusion_especes() const { return diffusion_coeff_; }
  inline const Champ_Don& cp_especes() const { return cp_; }

protected:
  Champ_Don molar_mass_, mu_, diffusion_coeff_, cp_;
  int num_espece_;

private :
  void calculer_mu_wilke() override;
  void update_Yn_values(Champ_Don& Yn, double temps) ;
};

#endif /* Loi_Etat_Multi_GP_WC_included */
