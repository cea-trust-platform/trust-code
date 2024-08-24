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

#ifndef Navier_Stokes_Turbulent_QC_included
#define Navier_Stokes_Turbulent_QC_included

#include <Navier_Stokes_Fluide_Dilatable_Proto.h>
#include <Navier_Stokes_Turbulent.h>

/*! @brief classe Navier_Stokes_Turbulent Cette classe represente l'equation de la dynamique pour un fluide
 *
 *      visqueux verifiant la condition d'incompressibilite div U = 0 avec
 *      modelisation de la turbulence.
 *      Un membre de type Modele_turbulence_hyd representera le modele de turbulence.
 *
 * @sa Navier_Stokes_Turbulent Modele_turbulence_hyd Pb_Thermohydraulique_Turbulent_QC
 */
class Navier_Stokes_Turbulent_QC: public Navier_Stokes_Turbulent, public Navier_Stokes_Fluide_Dilatable_Proto
{
  Declare_instanciable(Navier_Stokes_Turbulent_QC);
public:

  void completer() override;
  void mettre_a_jour(double) override;
  bool initTimeStep(double dt) override;

  int preparer_calcul() override;
  int impr(Sortie&) const override;
  void imprimer(Sortie& os) const override;

  DoubleTab& derivee_en_temps_inco(DoubleTab&) override;
  void assembler(Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) override;
  void assembler_avec_inertie(Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) override;
  inline const Champ_Inc& rho_la_vitesse() const override { return rho_la_vitesse_; }
  void discretiser() override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  const Champ_Don& diffusivite_pour_transport() const override;
};

#endif /* Navier_Stokes_Turbulent_QC_included */
