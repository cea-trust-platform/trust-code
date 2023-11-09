/****************************************************************************
* Copyright (c) 2023, CEA
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
  DP_.mettre_a_jour(temps);
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
