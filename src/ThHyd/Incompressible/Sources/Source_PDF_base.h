/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Source_PDF_base_included
#define Source_PDF_base_included

#include <Interpolation_IBM_base.h>
#include <Source_dep_inco_base.h>
#include <TRUST_Deriv.h>
#include <PDF_model.h>
#include <TRUSTTab.h>

class Probleme_base;
class Champ_Don_base;

/*! @brief class Source_PDF_base Base class for the source terms for the penalisation of the momentum in the Immersed Boundary Method (IBM)
 *
 *
 *
 * @sa Source_dep_inco_base, and Source_PDF
 */

class Source_PDF_base : public Source_dep_inco_base
{

  Declare_base(Source_PDF_base);

public:
  inline const int& getInterpolationBool() const { return interpolation_bool_; }
  inline const bool& get_matrice_pression_variable_bool_() const { return  matrice_pression_variable_bool_; }
  void associer_pb(const Probleme_base& ) override;
  DoubleTab& ajouter_(const DoubleTab&, DoubleTab&) const override;
  virtual DoubleTab& ajouter_(const DoubleTab&, DoubleTab&, const int) const;
  DoubleTab& calculer(DoubleTab&) const override;
  DoubleTab& calculer(DoubleTab&, const int) const;
  DoubleTab& calculer_pdf(DoubleTab& ) const;
  void mettre_a_jour(double ) override;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  double fonct_coeff(const double, const double, const double) const;
  virtual DoubleVect diag_coeff_elem(ArrOfDouble&, const DoubleTab&, int) const ;
  virtual DoubleTab compute_coeff_elem() const;
  virtual DoubleTab compute_coeff_matrice_pression() const;
  virtual void multiply_coeff_volume(DoubleTab&) const;
  virtual void correct_pressure(const DoubleTab&,DoubleTab&,const DoubleTab&) const;
  virtual void correct_incr_pressure(const DoubleTab&,DoubleTab&) const;
  virtual void correct_vitesse(const DoubleTab&,DoubleTab&) const;
  void calculer_vitesse_imposee();
  void updateChampRho();
  inline const PDF_model& get_modele() const { return modele_lu_; }
  int impr(Sortie&) const override;
  void ouvrir_fichier(SFichier&, const Nom&, const int) const override;
  inline const DoubleTab& get_sec_mem_pdf() const { return sec_mem_pdf; }
  inline void set_sec_mem_pdf(DoubleTab& it) { sec_mem_pdf = it; }

protected:
  virtual void compute_vitesse_imposee_projete(const DoubleTab&, const DoubleTab&, double, double);
  virtual void calculer_vitesse_imposee_hybrid();
  virtual void calculer_vitesse_imposee_elem_fluid();
  virtual void calculer_vitesse_imposee_mean_grad();
  virtual void calculer_vitesse_imposee_power_law_tbl();
  virtual void calculer_vitesse_imposee_power_law_tbl_u_star();
  virtual void rotate_imposed_velocity(DoubleTab&);
  ArrOfDouble get_tuvw_local() const;
  void associer_domaines(const Domaine_dis_base&, const Domaine_Cl_dis_base&) override;
  virtual void compute_indicateur_nodal_champ_aire();
  int type_vitesse_imposee_ = -1;

  OWN_PTR(Champ_Don_base) champ_rotation_lu_, champ_rotation_, champ_aire_lu_, champ_aire_, champ_rho_;

  int transpose_rotation_ = -1;
  DoubleTab indicateur_nodal_champ_aire_;
  DoubleTab vitesse_imposee_;
  PDF_model modele_lu_;
  double temps_relax_ = -100.;
  double echelle_relax_ = -100.;
  bool matrice_pression_variable_bool_ = false;
  bool penalized_ = false;
  DoubleTab sec_mem_pdf; // part of the source term computed with the imposed velocity

  // FOR THE INTERPOLATION
  OWN_PTR(Interpolation_IBM_base) interpolation_lue_;
  int interpolation_bool_ = 0;
};

#endif /* Source_PDF_base */
