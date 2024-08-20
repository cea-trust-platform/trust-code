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

#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <DP_Impose.h>
#include <Param.h>

/*! @brief Lit les specifications d'un Delta P impose a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws pas un champ
 */

// XD type_perte_charge_deriv objet_lecture type_perte_charge_deriv -1 not_set

// XD type_perte_charge_dp type_perte_charge_deriv dp 0  DP field should have 3 components defining dp, dDP/dQ, Q0
// XD attr dp_field field_base dp_field 0 the parameters of the previous formula (DP = dp + dDP/dQ * (Q - Q0)): uniform_field 3 dp dDP/dQ Q0 where Q0 is a mass flow rate (kg/s).

// XD type_perte_charge_dp_regul type_perte_charge_deriv dp_regul 1 Keyword used to regulate the DP value in order to match a target flow rate. Syntax  : dp_regul { DP0 d deb d eps e }
// XD attr DP0 floattant DP0 0 initial value of DP
// XD attr deb chaine deb 0 target flow rate in kg/s
// XD attr eps chaine eps 0 strength of the regulation (low values might be slow to find the target flow rate, high values might oscillate around the target value)


// XD DP_Impose source_base DP_Impose 0 Source term to impose a pressure difference according to the formula : DP = dp + dDP/dQ * (Q - Q0)
// XD attr aco chaine(into=["{"]) aco 0 Opening curly bracket.
// XD attr dp_type type_perte_charge_deriv dp_type 0 mass flow rate (kg/s).
// XD attr surface chaine(into=["surface"]) surface 0 not_set
// XD attr bloc_surface bloc_lecture bloc_surface 0 Three syntaxes are possible for the surface definition block: NL2 For VDF and VEF: { X|Y|Z = location subzone_name } NL2 Only for VEF: { Surface surface_name }. NL2 For polymac { Surface surface_name Orientation champ_uniforme }.
// XD attr acof chaine(into=["}"]) acof 0 Closing curly bracket.
Entree& DP_Impose::lire_donnees(Entree& is)
{
  Motcle acc_ouverte("{");
  Motcle motlu;
  is >> motlu;
  if (motlu != acc_ouverte)
    {
      Cerr << "On attendait le mot cle" << acc_ouverte << " a la place de " << motlu << finl;
      Process::exit();
    }
  is >> motlu;
  regul_ = 0;
  if (motlu == "dp")
    is >> DP_;
  else if (motlu == "dp_regul")
    {
      regul_ = 1;
      Nom eps_str, deb_str;
      Param param("dp_regul");
      param.ajouter("DP0", &dp_regul_, Param::REQUIRED);
      param.ajouter("deb", &deb_str, Param::REQUIRED);
      param.ajouter("eps", &eps_str, Param::REQUIRED);
      param.lire_avec_accolades(is);
      deb_cible_.setNbVar(1), eps_.setNbVar(1);
      deb_cible_.setString(deb_str), eps_.setString(eps_str);
      deb_cible_.addVar("t"), eps_.addVar("t");
      deb_cible_.parseString(), eps_.parseString();
    }
  else
    {
      Cerr << "On attendait le mot cle dp ou dp_regul a la place de " << motlu << finl;
      Process::exit();
    }
  return is;
}

void DP_Impose::mettre_a_jour(double temps)
{
  if (regul_) return;
  DP_->mettre_a_jour(temps);
}

void DP_Impose::update_dp_regul(const Equation_base& eqn, double deb, DoubleVect& bilan)
{
  if (!regul_) return;
  double t = eqn.probleme().schema_temps().temps_courant(), dt = eqn.probleme().schema_temps().pas_de_temps();
  deb_cible_.setVar(0, t), eps_.setVar(0, t);
  double deb_cible = deb_cible_.eval(), eps = eps_.eval(), f_min = std::pow(1 - eps, dt), f_max = std::pow(1 + eps, dt); //bande de variation de K
  if (eps > 0.0) dp_regul_ *= std::min(std::max(std::pow(deb_cible / std::fabs(deb), 2), f_min), f_max);

  //pour le fichier de suivi : seulement sur le maitre, car Source_base::imprimer() fait une somme sur les procs
  if (!Process::me()) bilan(0) = dp_regul_, bilan(1) = deb, bilan(2) = deb_cible;
}
