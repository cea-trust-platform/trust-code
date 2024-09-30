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

#ifndef Conduction_included
#define Conduction_included

#include <Traitement_particulier_Solide_base.h>
#include <Schema_Temps_base.h>
#include <Operateur_Diff.h>
#include <Equation_base.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>


class Milieu_base;
class Solide;

/*! @brief Classe Conduction Cette classe represente l'equation d'evolution
 *
 *     de la temperature dans un solide de conductivite k et de masse volumique rho et chaleur specifique Cp
 *         (rho.Cp) . dT/dt - div (k grad T) = (rho.Cp) . f
 *     Les termes rho et Cp peuvent etre non uniformes car cette classe
 *     ne fait pas le raccourci de calculer la diffusivite k/(rho*Cp).
 *
 *
 * @sa Equation_base, Conduction
 */
class Conduction : public Equation_base
{
  Declare_instanciable_sans_constructeur(Conduction);
public:
  Conduction();
  void set_param(Param&) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_solide(const Solide& );
  void associer_milieu_base(const Milieu_base& ) override;
  const Milieu_base& milieu() const override;
  Milieu_base& milieu() override;
  const Solide& solide() const;
  Solide& solide();
  int nombre_d_operateurs() const override;
  const Operateur& operateur(int) const override;
  Operateur& operateur(int) override;
  inline const Champ_Inc_base& inconnue() const override;
  inline Champ_Inc_base& inconnue() override;
  void discretiser() override;
  int impr(Sortie& os) const override;

  const Motcle& domaine_application() const override;
  void mettre_a_jour(double temps) override;
  virtual const Champ_Don_base& diffusivite_pour_transport() const;
  virtual const Champ_base& diffusivite_pour_pas_de_temps() const;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  //Methode creer_champ pas codee a surcharger si necessaire
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////

private :
  REF(Solide) le_solide;
  OWN_PTR(Champ_Inc_base) la_temperature;
  Operateur_Diff terme_diffusif;

protected :
  OWN_PTR(Traitement_particulier_Solide_base) le_traitement_particulier;
};

/*! @brief Renvoie le champ inconnue de l'equation, i.
 *
 * e. la temperature.
 *
 * @return (Champ_Inc_base&) le champ inconnue de l'equation: la temperature
 */
inline Champ_Inc_base& Conduction::inconnue()
{
  return la_temperature;
}

/*! @brief Renvoie le champ inconnue de l'equation, i.
 *
 * e. la temperature.
 *     (version const)
 *
 * @return (Champ_Inc_base&) le champ inconnue de l'equation: la temperature
 */
inline const Champ_Inc_base& Conduction::inconnue() const
{
  return la_temperature;
}

#endif
