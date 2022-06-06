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


#ifndef Source_PDF_EF_included
#define Source_PDF_EF_included

#include <Source_PDF_base.h>
#include <Ref_Zone_EF.h>
#include <Ref_Zone_Cl_EF.h>
#include <Champ_Fonc.h>
#include <Matrice.h>

class Probleme_base;

//
// .DESCRIPTION class Source_PDF_EF
//  Source term for the penalisation of the momentum in the Immersed Boundary Method (IBM)
//
// .SECTION voir aussi Source_PDF_base
//
//

class Source_PDF_EF : public Source_PDF_base
{

  Declare_instanciable(Source_PDF_EF);

public:
  void associer_pb(const Probleme_base& ) override;
  DoubleTab& ajouter_(const DoubleTab&, DoubleTab&) const override;
  DoubleTab& ajouter_(const DoubleTab&, DoubleTab&, const int) const override;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  void verif_ajouter_contrib(const DoubleTab& vitesse, Matrice_Morse& matrice) const ;
  DoubleVect diag_coeff_elem(ArrOfDouble&, const DoubleTab&, int) const override ;
  DoubleTab compute_coeff_elem() const override;
  DoubleTab compute_coeff_matrice_pression() const override;
  void multiply_coeff_volume(DoubleTab&) const override;
  void correct_pressure(const DoubleTab&,DoubleTab&,const DoubleTab&) const override;
  void correct_incr_pressure(const DoubleTab&,DoubleTab&) const override;
  void correct_vitesse(const DoubleTab&,DoubleTab&) const override;
  void calculer_vitesse_imposee();
  void test(Matrice&) const;
  void updateChampRho();
  int impr(Sortie&) const override;
protected:
  Champ_Don champ_nodal_;
  void compute_vitesse_imposee_projete(const DoubleTab&, const DoubleTab&, double, double) override;
  void calculer_vitesse_imposee_hybrid() override;
  void calculer_vitesse_imposee_elem_fluid() override;
  void calculer_vitesse_imposee_mean_grad() override;
  void calculer_vitesse_imposee_power_law_tbl() override;
  void rotate_imposed_velocity(DoubleTab&) override;
  DoubleTab compute_pond(const DoubleTab&, const DoubleTab&, const DoubleVect&, int&, int&) const ;
  REF(Zone_EF) la_zone_EF;
  REF(Zone_Cl_EF) la_zone_Cl_EF;
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) override;
  void compute_indicateur_nodal_champ_aire() override;
};

#endif
