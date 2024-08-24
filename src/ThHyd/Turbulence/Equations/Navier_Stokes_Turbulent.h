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

#ifndef Navier_Stokes_Turbulent_included
#define Navier_Stokes_Turbulent_included

#include <Modele_turbulence_hyd.h>
#include <Navier_Stokes_std.h>
#include <Champ_Fonc.h>

/*! @brief classe Navier_Stokes_Turbulent Cette classe represente l'equation de la dynamique pour un fluide
 *
 *      visqueux verifiant la condition d'incompressibilite div U = 0 avec
 *      modelisation de la turbulence.
 *      Un membre de type Modele_turbulence_hyd representera le modele de turbulence.
 *
 * @sa Navier_Stokes_std Modele_turbulence_hyd Pb_Hydraulique_Turbulent, Pb_Thermohydraulique_Turbulent
 */
class Navier_Stokes_Turbulent: public Navier_Stokes_std
{
  Declare_instanciable(Navier_Stokes_Turbulent);

public:
  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  inline const Champ_Fonc& viscosite_turbulente() const { return le_modele_turbulence->viscosite_turbulente(); }
  inline const Modele_turbulence_hyd& modele_turbulence() const { return le_modele_turbulence; }
  int sauvegarder(Sortie&) const override;
  int reprendre(Entree&) override;
  int preparer_calcul() override;
  bool initTimeStep(double dt) override;
  void mettre_a_jour(double) override;
  void completer() override;
  const Champ_Don& diffusivite_pour_transport() const override;
  const Champ_base& diffusivite_pour_pas_de_temps() const override;

  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom, Option opt = NONE) const override;

  void imprimer(Sortie&) const override;
  const RefObjU& get_modele(Type_modele type) const override;
  void imprime_residu(SFichier&) override;
  Nom expression_residu() override;

protected:
  Entree& lire_op_diff_turbulent(Entree& is);
  Modele_turbulence_hyd le_modele_turbulence;

private:
  int typer_lire_mod_turb_hyd(Entree& s);
};

#endif /* Navier_Stokes_Turbulent_included */
