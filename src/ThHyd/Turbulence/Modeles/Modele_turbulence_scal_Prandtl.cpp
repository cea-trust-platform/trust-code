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

#include <Modele_turbulence_scal_Prandtl.h>
#include <Pb_Thermohydraulique_Turbulent_QC.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Param.h>
#include <Champ_Uniforme.h>
#include <Motcle.h>
#include <Parser_U.h>
#include <Domaine_VF.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_scal_Prandtl, "Modele_turbulence_scal_Prandtl", Modele_turbulence_scal_diffturb_base);

Modele_turbulence_scal_Prandtl::Modele_turbulence_scal_Prandtl() :
  LePrdt(0.9)
{
}

Sortie& Modele_turbulence_scal_Prandtl::printOn(Sortie& s) const
{

  return Modele_turbulence_scal_diffturb_base::printOn(s);
}

Entree& Modele_turbulence_scal_Prandtl::readOn(Entree& is)
{
  Modele_turbulence_scal_diffturb_base::readOn(is);
  /*
   // GF on  peut forcer le modele a toujours utilise le parser
   //pour l'instant c'est bloquant pour les constituants
   if (definition_fonction==Nom())
   {
   definition_fonction=Nom("nu_t/");
   Nom nprtd(LePrdt);
   definition_fonction+=nprtd;
   }
   */

  // si on  a lu une fonction, on initialise le Parser
  if (definition_fonction != Nom())
    {
      fonction.setNbVar(2);
      fonction.addVar("alpha");
      fonction.addVar("nu_t");
      fonction.setString(definition_fonction);
      fonction.parseString();
    }

  if (LePrdt_fct != Nom())
    {
      fonction1.setNbVar(3);
      fonction1.addVar("x");
      fonction1.addVar("y");
      fonction1.addVar("z");
      fonction1.setString(LePrdt_fct);
      fonction1.parseString();
      Cerr << "L'expression du nombre de Prandtl turbulent est " << LePrdt_fct << finl;
    }
  else
    {
      Cerr << "La valeur par defaut du nombre de Prandtl turbulent est " << LePrdt << finl;
    }

  Cerr << "L'expression du nombre de Prandtl turbulent est " << LePrdt_fct << finl;
  return is;
}

void Modele_turbulence_scal_Prandtl::set_param(Param& param)
{
  param.ajouter("Prdt", &LePrdt_fct);
  param.ajouter("Prandt_turbulent_fonction_nu_t_alpha", &definition_fonction);
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
    {
      loipar->calculer_scal(diffusivite_turbulente_);
    }
  const Probleme_base& mon_pb = mon_equation->probleme();
  DoubleTab& lambda_t = conductivite_turbulente_.valeurs();
  lambda_t = diffusivite_turbulente_.valeurs();
  const bool Ccp = sub_type(Champ_Uniforme, mon_pb.milieu().capacite_calorifique().valeur());
  const DoubleTab& tab_Cp = mon_pb.milieu().capacite_calorifique().valeurs();
  const DoubleTab& tab_rho = mon_pb.milieu().masse_volumique().valeurs();
  if (sub_type(Pb_Thermohydraulique_Turbulent_QC, mon_pb))
    {
      for (int i = 0; i < lambda_t.size(); i++)
        lambda_t(i) *= tab_Cp(Ccp ? 0 : i);
      if (equation().probleme().is_dilatable())
        multiplier_par_rho_si_dilatable(lambda_t, mil);
    }
  else
    lambda_t *= mon_equation->domaine_dis()->nb_elem() > 0 ? tab_rho(0, 0) * tab_Cp(0, 0) : 1.0;
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
  DoubleTab& alpha_t = diffusivite_turbulente_.valeurs();
  const DoubleTab& nu_t = la_viscosite_turbulente->valeurs();
  double temps = la_viscosite_turbulente->temps();

  //Domaine& domaine = mon_equation->domaine_dis().domaine();
  const DoubleTab& xp = ref_cast(Domaine_VF,mon_equation->domaine_dis().valeur()).xp();
  //domaine.calculer_centres_gravite(xp);

  // if (temps != la_diffusivite_turbulente.temps())
  {
    int n = alpha_t.size();
    if (nu_t.size() != n)
      {
        Cerr << "Les DoubleTab des champs diffusivite_turbulente et viscosite_turbulente" << finl;
        Cerr << "doivent avoir le meme nombre de valeurs nodales" << finl;
        exit();
      }

    if (definition_fonction != Nom())
      {
        // modif VB pour utiliser l'equation qui approche Yakhot : LePrdt = 0.7/Pe-t + 0.85
        // Pe-t est un nombre de Peclet turbulent defini par Pr*(nut/nu)
        // On a alors alpha_t = nut * nut / ( 0.7 alpha + 0.85 nut )

        const Milieu_base& milieu = mon_equation.valeur().milieu();
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
            d_alpha = alpha(0, 0);
            fonction.setVar("alpha", d_alpha);
          }
        for (int i = 0; i < n; i++)
          {
            if (!is_alpha_unif)
              fonction.setVar("alpha", alpha(i));
            fonction.setVar("nu_t", nu_t[i]);

            alpha_t[i] = fonction.eval();
          }
      }
    else
      for (int i = 0; i < n; i++)
        {
          if (LePrdt_fct != Nom())
            {
              double x = xp(i, 0);
              double y = xp(i, 1);
              double z = 0;
              if (xp.nb_dim() == 3)
                {
                  z = xp(i, 2);
                }
              fonction1.setVar("x", x);
              fonction1.setVar("y", y);
              fonction1.setVar("z", z);
              double NbPrandtlCell = fonction1.eval();
              alpha_t[i] = nu_t[i] / NbPrandtlCell;
            }
          else
            {
              alpha_t[i] = nu_t[i] / LePrdt;
            }
        }

    diffusivite_turbulente_.changer_temps(temps);
  }
  if (equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(diffusivite_turbulente_.valeurs(), equation().probleme().milieu());
  return diffusivite_turbulente_;
}

