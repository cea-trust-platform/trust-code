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

#include <Pb_Thermohydraulique_Turbulent_QC.h>
#include <Modele_turbulence_scal_Prandtl.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Champ_Uniforme.h>
#include <Domaine_VF.h>
#include <Parser_U.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Modele_turbulence_scal_Prandtl, "Modele_turbulence_scal_Prandtl", Modele_turbulence_scal_diffturb_base);
// XD prandtl modele_turbulence_scal_base prandtl -1 The Prandtl model. For the scalar equations, only the model based on Reynolds analogy is available. If K_Epsilon was selected in the hydraulic equation, Prandtl must be selected for the convection-diffusion temperature equation coupled to the hydraulic equation and Schmidt for the concentration equations.

Sortie& Modele_turbulence_scal_Prandtl::printOn(Sortie& s) const { return Modele_turbulence_scal_diffturb_base::printOn(s); }

Entree& Modele_turbulence_scal_Prandtl::readOn(Entree& is)
{
  Modele_turbulence_scal_diffturb_base::readOn(is);
  // si on  a lu une fonction, on initialise le Parser
  if (definition_fonction_ != Nom())
    {
      fonction_.setNbVar(2);
      fonction_.addVar("alpha");
      fonction_.addVar("nu_t");
      fonction_.setString(definition_fonction_);
      fonction_.parseString();
    }

  if (LePrdt_fct_ != Nom())
    {
      fonction1_.setNbVar(3);
      fonction1_.addVar("x");
      fonction1_.addVar("y");
      fonction1_.addVar("z");
      fonction1_.setString(LePrdt_fct_);
      fonction1_.parseString();
      Cerr << "L'expression du nombre de Prandtl turbulent est " << LePrdt_fct_ << finl;
    }
  else
    Cerr << "La valeur par defaut du nombre de Prandtl turbulent est " << LePrdt_ << finl;

  Cerr << "L'expression du nombre de Prandtl turbulent est " << LePrdt_fct_ << finl;
  return is;
}

void Modele_turbulence_scal_Prandtl::set_param(Param& param)
{
  param.ajouter("Prdt", &LePrdt_fct_); // XD_ADD_P chaine Keyword to modify the constant (Prdt) of Prandtl model : Alphat=Nut/Prdt Default value is 0.9
  param.ajouter("Prandt_turbulent_fonction_nu_t_alpha", &definition_fonction_); // XD_ADD_P chaine Optional keyword to specify turbulent diffusivity (by default, alpha_t=nu_t/Prt) with another formulae, for example: alpha_t=nu_t2/(0,7*alpha+0,85*nu_tt) with the string nu_t*nu_t/(0,7*alpha+0,85*nu_t) where alpha is the thermal diffusivity.
  Modele_turbulence_scal_base::set_param(param);
}

/*! @brief Calcule la diffusivite turbulente et la loi de paroi.
 *
 * @param (double)
 */
void Modele_turbulence_scal_Prandtl::mettre_a_jour(double)
{
  calculer_diffusivite_turbulente();
  const Milieu_base& mil = equation().probleme().milieu();
  const Turbulence_paroi_scal& lp = loi_paroi();
  if (lp.non_nul())
    loipar_->calculer_scal(diffusivite_turbulente_);

  const Probleme_base& mon_pb = mon_equation_->probleme();
  DoubleTab& lambda_t = conductivite_turbulente_.valeurs();
  lambda_t = diffusivite_turbulente_.valeurs();
  const bool uniforme = sub_type(Champ_Uniforme, mon_pb.milieu().capacite_calorifique().valeur());
  const DoubleTab& tab_Cp = mon_pb.milieu().capacite_calorifique()->valeurs();
  const DoubleTab& tab_rho = mon_pb.milieu().masse_volumique()->valeurs();
  if (sub_type(Pb_Thermohydraulique_Turbulent_QC, mon_pb))
    {
      CDoubleArrView Cp = static_cast<const DoubleVect&>(tab_Cp).view_ro();
      DoubleArrView lambda = static_cast<DoubleVect&>(lambda_t).view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), lambda_t.size(),
                           KOKKOS_LAMBDA(
                             const int i)
      {
        lambda(i) *= Cp(uniforme ? 0 : i);
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
      if (equation().probleme().is_dilatable())
        multiplier_par_rho_si_dilatable(lambda_t, mil);
    }
  else
    lambda_t *= mon_equation_->domaine_dis()->nb_elem() > 0 ? tab_rho(0, 0) * tab_Cp(0, 0) : 1.0;
  lambda_t.echange_espace_virtuel();
  diffusivite_turbulente_->valeurs().echange_espace_virtuel();
}

/*! @brief Calcule la diffusivite turbulente.
 *
 * diffusivite_turbulente = viscosite_turbulente / Prdt_turbulent
 *
 * @return (Champ_Fonc&) la diffusivite turbulente nouvellement calculee
 * @throws les champs diffusivite_turbulente et viscosite_turbulente
 * doivent avoir le meme nombre de valeurs nodales
 */
Champ_Fonc& Modele_turbulence_scal_Prandtl::calculer_diffusivite_turbulente()
{
  DoubleTab& tab_alpha_t = diffusivite_turbulente_->valeurs();
  const DoubleTab& tab_nu_t = la_viscosite_turbulente_->valeurs();
  double temps = la_viscosite_turbulente_->valeur().temps();
  const DoubleTab& xp = ref_cast(Domaine_VF,mon_equation_->domaine_dis().valeur()).xp();

  int n = tab_alpha_t.size();
  if (tab_nu_t.size() != n)
    {
      Cerr << "Les DoubleTab des champs diffusivite_turbulente et viscosite_turbulente" << finl;
      Cerr << "doivent avoir le meme nombre de valeurs nodales" << finl;
      exit();
    }

  if (definition_fonction_ != Nom())
    {
      // modif VB pour utiliser l'equation qui approche Yakhot : LePrdt = 0.7/Pe-t + 0.85
      // Pe-t est un nombre de Peclet turbulent defini par Pr*(nut/nu)
      // On a alors alpha_t = nut * nut / ( 0.7 alpha + 0.85 nut )

      const Milieu_base& milieu = mon_equation_->milieu();
      const Champ_Don& alpha = milieu.diffusivite();
      if (!alpha.non_nul())
        {
          // GF si cette condition est bloquante, on peut ameliorer en
          // tentant de creer un parser sans la variable alpha, si ok on peut dire que alpha existe , est constant et vaut 1.
          Cerr << "Erreur dans Modele_turbulence_scal_prandt, l'option Prandt_turbulent_fonction_nu_t_alpha n'est disponible que pour des milieux ayant defini la diffusivite" << finl;
          exit();
        }
      double d_alpha = 0.;
      int is_alpha_unif = sub_type(Champ_Uniforme, alpha.valeur());
      if (is_alpha_unif)
        {
          d_alpha = alpha->valeurs()(0, 0);
          fonction_.setVar("alpha", d_alpha);
        }
      ToDo_Kokkos("Critical");
      for (int i = 0; i < n; i++)
        {
          if (!is_alpha_unif)
            fonction_.setVar("alpha", alpha->valeurs()(i));
          fonction_.setVar("nu_t", tab_nu_t[i]);

          tab_alpha_t[i] = fonction_.eval();
        }
    }
  else
    {
      if (LePrdt_fct_ != Nom())
        {
          ToDo_Kokkos("Critical");
          for (int i = 0; i < n; i++)
            {
              double x = xp(i, 0);
              double y = xp(i, 1);
              double z = 0;
              if (xp.nb_dim() == 3)
                {
                  z = xp(i, 2);
                }
              fonction1_.setVar("x", x);
              fonction1_.setVar("y", y);
              fonction1_.setVar("z", z);
              double NbPrandtlCell = fonction1_.eval();
              tab_alpha_t[i] = tab_nu_t[i] / NbPrandtlCell;
            }
        }
      else
        {
          double inv_LePrdt = 1./LePrdt_;
          // ToDo Kokkos: implement operator_mutiply(u,v,alpha);
          // if u = v * alpha is a regular pattern in the code...
          CDoubleArrView nu_t = static_cast<const DoubleVect&>(tab_nu_t).view_ro();
          DoubleArrView alpha_t = static_cast<DoubleVect&>(tab_alpha_t).view_rw();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                               Kokkos::RangePolicy<>(0, n), KOKKOS_LAMBDA(
                                 const int i)
          {
            alpha_t[i] = nu_t[i] * inv_LePrdt;
          });
          end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
        }
    }

  diffusivite_turbulente_->changer_temps(temps);

  if (equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(diffusivite_turbulente_.valeurs(), equation().probleme().milieu());
  return diffusivite_turbulente_;
}

