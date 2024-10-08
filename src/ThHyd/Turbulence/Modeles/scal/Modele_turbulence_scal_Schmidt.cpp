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

#include <Modele_turbulence_scal_Schmidt.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Convection_Diffusion_std.h>
#include <Param.h>

Implemente_instanciable(Modele_turbulence_scal_Schmidt, "Modele_turbulence_scal_Schmidt", Modele_turbulence_scal_diffturb_base);
// XD schmidt modele_turbulence_scal_base schmidt -1 The Schmidt model. For the scalar equations, only the model based on Reynolds analogy is available. If K_Epsilon was selected in the hydraulic equation, Schmidt must be selected for the convection-diffusion temperature equation coupled to the hydraulic equation and Schmidt for the concentration equations.

Sortie& Modele_turbulence_scal_Schmidt::printOn(Sortie& s) const { return Modele_turbulence_scal_diffturb_base::printOn(s); }

Entree& Modele_turbulence_scal_Schmidt::readOn(Entree& is)
{
  Modele_turbulence_scal_diffturb_base::readOn(is);
  Cerr << "La valeur du nombre de Schmidt turbulent est de " << LeScturb_ << finl;
  return is;
}

void Modele_turbulence_scal_Schmidt::set_param(Param& param)
{
  param.ajouter("ScTurb", &LeScturb_); // XD_ADD_P floattant Keyword to modify the constant (Sct) of Schmlidt model : Dt=Nut/Sct Default value is 0.7.
  Modele_turbulence_scal_base::set_param(param);
}

/*! @brief Renvoie 1 si le mot cle passe en parametre est un nom de champ de l'objet
 *
 * @param (Motcle& mot) le mot cle a comparer aux noms de champs connus
 * @return (int) 0 si le mot n'est pas un nom de champ 1 sinon
 */
int Modele_turbulence_scal_Schmidt::comprend_champ(const Motcle& mot) const
{
  if (mot == Motcle("diffusion_turbulente"))
    return 1;
  else
    return 0;
}

/*! @brief Renvoie 1 si un champ fonction (Champ_Fonc) du nom specifie est porte par le modele de turbulence.
 *
 *     Renvoie 0 sinon.
 *
 * @param (Motcle& mot) le nom d'un champ fonction du modele de turbulence
 * @param (OBS_PTR(Champ_base)& ch_ref) la reference sur le champ recherche (si il a ete trouve)
 * @return (int) 1 si un champ fonction du nom specifie a ete trouve 0 sinon
 */
int Modele_turbulence_scal_Schmidt::a_pour_Champ_Fonc(const Motcle& mot,
                                                      OBS_PTR(Champ_base) &ch_ref) const
{
  if (mot == Motcle("diffusion_turbulente"))
    {
      ch_ref = diffusivite_turbulente_.valeur();
      return 1;
    }
  return 0;
}

/*! @brief Calcule le coefficient turbulent utilise dans l equation et la loi de paroi.
 *
 * @param (double)
 */
void Modele_turbulence_scal_Schmidt::mettre_a_jour(double)
{
  calculer_diffusion_turbulente();
  const Milieu_base& mil = equation().probleme().milieu();
  if (loi_paroi_non_nulle())
    loipar_->calculer_scal(diffusivite_turbulente_);

  DoubleTab& lambda_t = conductivite_turbulente_->valeurs();
  lambda_t = diffusivite_turbulente_->valeurs();
  if (equation().probleme().is_dilatable())
    multiplier_par_rho_si_dilatable(lambda_t, mil);
  conductivite_turbulente_->valeurs().echange_espace_virtuel();
  diffusivite_turbulente_->valeurs().echange_espace_virtuel();
}

/*! @brief Calcule la diffusion turbulente.
 *
 * diffusion_turbulente = viscosite_turbulente / Sc_turbulent
 *
 * @return (Champ_Fonc_base&) la diffusion turbulente nouvellement calculee
 * @throws les champs diffusivite_turbulente et viscosite_turbulente
 * doivent avoir le meme nombre de valeurs nodales
 */
Champ_Fonc_base& Modele_turbulence_scal_Schmidt::calculer_diffusion_turbulente()
{
  DoubleTab& alpha_t = diffusivite_turbulente_->valeurs();
  const DoubleTab& nu_t = la_viscosite_turbulente_->valeurs();
  double temps = la_viscosite_turbulente_->temps();
  int n = alpha_t.size();
  if (nu_t.size() != n)
    {
      Cerr << "Les DoubleTab des champs diffusivite_turbulente et viscosite_turbulente" << finl;
      Cerr << "doivent avoir le meme nombre de valeurs nodales" << finl;
      exit();
    }

  for (int i = 0; i < n; i++)
    alpha_t[i] = nu_t[i] / LeScturb_;
  diffusivite_turbulente_->changer_temps(temps);
  if (equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(diffusivite_turbulente_->valeurs(), equation().probleme().milieu());
  return diffusivite_turbulente_;
}
