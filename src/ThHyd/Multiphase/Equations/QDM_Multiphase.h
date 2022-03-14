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
//////////////////////////////////////////////////////////////////////////////
//
// File:        QDM_Multiphase.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef QDM_Multiphase_included
#define QDM_Multiphase_included

#include <Navier_Stokes_std.h>
#include <Operateur_Evanescence.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe QDM_Multiphase
//    Cette classe porte les termes de l'equation de la dynamique
//    pour un fluide sans modelisation de la turbulence.
//    On suppose l'hypothese de fluide quasi compressible.
//    Sous ces hypotheses, on utilise la forme suivante des equations de
//    Navier_Stokes:
//       DU/dt = div(terme visqueux) - gradP/rho + sources/rho
//       div U = W
//    avec DU/dt : derivee particulaire de la vitesse
//         rho   : masse volumique
//    Rq : l'implementation de la classe permet bien sur de negliger
//         certains termes de l'equation (le terme visqueux, le terme
//         convectif, tel ou tel terme source).
//    L'inconnue est le champ de vitesse.
// .SECTION voir aussi
//      Equation_base Pb_Thermohydraulique_QC Navier_Stokes_std
//////////////////////////////////////
////////////////////////////////////////
class QDM_Multiphase : public Navier_Stokes_std
{
  Declare_instanciable(QDM_Multiphase);

public :

  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void completer() override;
  void discretiser_vitesse() override;
  void mettre_a_jour(double temps) override;
  int impr(Sortie& os) const override
  {
    return Equation_base::impr(os); //idem
  }
  bool initTimeStep(double dt) override;
  void abortTimeStep() override;

  void dimensionner_matrice_sans_mem(Matrice_Morse& matrice) override;

  /*
    interface {dimensionner,assembler}_blocs
    specificites : prend en compte l'evanescence (en dernier)
  */
  int has_interface_blocs() const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  const Champ_Don& diffusivite_pour_transport() const override;
  const Champ_base& diffusivite_pour_pas_de_temps() const override;
  const Champ_base& vitesse_pour_transport() const override;
  void assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) override;
  void creer_champ(const Motcle& motlu) override;

protected:
  Entree& lire_cond_init(Entree&) override; //pour lire la pression
  int preparer_calcul() override; //appelle la methode de Equation_base

  std::vector<Champ_Inc> vit_phases_; //vitesses de chaque phase
  Motcles noms_vit_phases_; //leurs noms

  std::vector<Champ_Fonc> grad_vit_phases_; //gradient des vitesses de chaque phase
  Motcles noms_grad_vit_phases_; //leurs noms

  Operateur_Evanescence evanescence;
};

#endif
