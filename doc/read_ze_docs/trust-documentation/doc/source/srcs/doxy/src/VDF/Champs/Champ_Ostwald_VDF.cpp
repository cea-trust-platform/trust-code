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

#include <Champ_Ostwald_VDF.h>
#include <Champ_Uniforme.h>
#include <Fluide_Ostwald.h>

Implemente_instanciable(Champ_Ostwald_VDF, "Champ_Ostwald_VDF", Champ_Ostwald);

Sortie& Champ_Ostwald_VDF::printOn(Sortie& os) const { return os << valeurs()(0, 0); }

Entree& Champ_Ostwald_VDF::readOn(Entree& is) { return is; }

void Champ_Ostwald_VDF::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
  Champ_Don_base::mettre_a_jour(tps);
}

/*! @brief Calcul la viscosite mu en fonction de la consistance et de l'indice de structure en utilisant la loi d'Ostwald.
 *
 * Pour
 *    des viscsites tres faible et tres fortes on utilise une regression
 *
 * @param (DoubleTab&) les viscosite au temps precedent
 */

void Champ_Ostwald_VDF::calculer_mu(DoubleTab& mu_tab)
{
  const double d_n = mon_fluide_->indice_struct().valeurs()(0, 0);

  for (int i = 0; i < nb_valeurs_nodales(); i++)
    {
      if (sub_type(Champ_Uniforme, mon_fluide_->consistance().valeur()))
        {
          const double d_k = mon_fluide_->consistance().valeurs()(0, 0);
          if (mu_tab[i] < 1.E-4)
            mu_tab[i] = d_k * pow(0.5 * 1.E-4, (d_n - 1.) / 2.);
          else if (mu_tab[i] > 1.E16)
            mu_tab[i] = d_k * pow(0.5 * 1.E16, (d_n - 1.) / 2.);
          else
            mu_tab[i] = d_k * pow(0.5 * mu_tab[i], (d_n - 1.) / 2.);
        }
      else  // K varie en fonction de la temperature
        {
          const DoubleTab& K_tab = mon_fluide_->consistance().valeurs();
          if (mu_tab[i] < 1.E-4)
            mu_tab[i] = K_tab[i] * pow(0.5 * 1.E-4, (d_n - 1.) / 2.);
          else if (mu_tab[i] > 1.E16)
            mu_tab[i] = K_tab[i] * pow(0.5 * 1.E16, (d_n - 1.) / 2.);
          else
            mu_tab[i] = K_tab[i] * pow(0.5 * mu_tab[i], (d_n - 1.) / 2.);
        }
    }
}

/*! @brief Calcul le champ Ostwald : calcul de D::D
 *
 *         puis calcul de mu
 *
 * @param (double) temps ou le calcul est effectue
 */

void Champ_Ostwald_VDF::me_calculer(double tps)
{
  if (temps_ != tps)
    {
      // Cerr<< "Calcul de Mu Ostwald"<<finl;
      mon_champ_->calculer_dscald_centre_element(valeurs());
      calculer_mu(valeurs());
    }
}

/*! @brief Initialise le champs
 *
 */
int Champ_Ostwald_VDF::initialiser(const double un_temps)
{
  mettre_a_jour(un_temps);
  return 1;
}
