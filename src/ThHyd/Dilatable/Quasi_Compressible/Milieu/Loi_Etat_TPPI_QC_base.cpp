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

#include <Fluide_Dilatable_base.h>
#include <Loi_Etat_TPPI_QC_base.h>
#include <Champ_Uniforme.h>
#include <Domaine_VF.h>
#include <Champ_Don.h>

Implemente_base(Loi_Etat_TPPI_QC_base, "Loi_Etat_TPPI_QC_base", Loi_Etat_TPPI_base);
// XD loi_etat_tppi_base loi_etat_base loi_etat_tppi_base -1 Basic class for thermo-physical properties interface (TPPI) used for dilatable problems

Sortie& Loi_Etat_TPPI_QC_base::printOn(Sortie& os) const { return os << que_suis_je() << finl; }
Entree& Loi_Etat_TPPI_QC_base::readOn(Entree& is) { return Loi_Etat_TPPI_base::readOn(is); }

void Loi_Etat_TPPI_QC_base::preparer_calcul()
{
  Loi_Etat_TPPI_base::preparer_calcul();
  if (!vec_press_filled_) init_vec_press();
}

void Loi_Etat_TPPI_QC_base::init_vec_press()
{
  const double Pth = le_fluide->pression_th();
  vec_press_.resize(le_fluide->inco_chaleur().valeurs().dimension(0));
  for (auto &itr : vec_press_) itr = Pth;
  vec_press_filled_ = true;
}

// Dans l'ordre on fait ca
//
//    calculer_Cp();
//    calculer_mu();
//    calculer_lambda();
//    calculer_nu();
//    calculer_alpha();
//    calculer_mu_sur_Sc();
//    calculer_nu_sur_Sc();
void Loi_Etat_TPPI_QC_base::calculer_Cp()
{
  if (!vec_press_filled_) init_vec_press();
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  SpanD temp_span = tab_ICh.get_span(), p_span = SpanD(vec_press_);

  /* Step 2 : Mu */
  Champ_Don& mu = le_fluide->viscosite_dynamique();
  DoubleTab& tab_mu = mu->valeurs();
  TPPI_->tppi_get_mu_pT(p_span, temp_span, tab_mu.get_span());
  tab_mu.echange_espace_virtuel();
  mu->mettre_a_jour(temperature_->temps());

  /* Step 3 : Lambda */
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda->valeurs();
  TPPI_->tppi_get_lambda_pT(p_span, temp_span, tab_lambda.get_span());
  tab_lambda.echange_espace_virtuel();

  /* Step 4 : Alpha */
  Champ_Don& alpha = le_fluide->diffusivite();
  DoubleTab& tab_alpha = alpha->valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();

  const bool isVDF = (alpha->que_suis_je() == "Champ_Fonc_P0_VDF") ? true : false;

  if (isVDF)
    for (int i = 0; i < tab_alpha.dimension(0); i++)
      tab_alpha(i, 0) = tab_lambda(i, 0) / (tab_rho(i, 0) * Cp_);
  else
    {
      const IntTab& elem_faces = ref_cast(Domaine_VF, le_fluide->vitesse().domaine_dis_base()).elem_faces();
      const int nfe = elem_faces.line_size();
      for (int i = 0; i < tab_alpha.dimension(0); i++)
        {
          double rhoelem = 0.;
          for (int face = 0; face < nfe; face++)
            rhoelem += tab_rho(elem_faces(i, face), 0);
          rhoelem /= nfe;
          tab_alpha(i, 0) = tab_lambda(i, 0) / (rhoelem * Cp_);
        }
    }
  tab_alpha.echange_espace_virtuel();
}

void Loi_Etat_TPPI_QC_base::calculer_masse_volumique()
{
  if (!vec_press_filled_) init_vec_press();
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  SpanD temp_span = tab_ICh.get_span(),  p_span = SpanD(vec_press_), rho_span = tab_rho_np1.get_span();
  TPPI_->tppi_get_rho_pT(p_span, temp_span, rho_span);

  for (int som = 0; som < tab_rho.size(); som++) tab_rho(som, 0) = 0.5 * (tab_rho_n(som) + tab_rho_np1(som));

  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
}
