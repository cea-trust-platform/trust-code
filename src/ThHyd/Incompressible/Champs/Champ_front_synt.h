/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Champ_front_synt_included
#define Champ_front_synt_included

#include <Ch_front_var_instationnaire_dep.h>
#include <Champ_front_base.h>
#include <TRUST_Ref.h>

class Champ_Inc_base;

/*! @brief classe Champ_front_synt Classe derivee de Champ_front_base
 *
 * @sa Champ_front_base
 */

class Champ_front_synt : public Ch_front_var_instationnaire_dep
{
  Declare_instanciable(Champ_front_synt);

public:
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  Champ_front_base& affecter_(const Champ_front_base& ch) override;
  void mettre_a_jour(double temps) override;

protected :
  const double amp_ = 1.452762113;
  const double Cmu_=0.09;

  OBS_PTR(Champ_Inc_base) ref_inco_; // sera une référence à l'inconnue
  double temps_d_avant_= 0.; // variable auxiliaire pour acceder au temps precedent dans mettre_a_jour

  // Informartions faces utilisees
  int nb_face_=-1; // nombre de faces du bord
  DoubleTab centreGrav_; // centre de gravités des faces du bord ; aura les dimension (nb_face,3)

  // PARAMETRES LUS (7)
  //// Champs caracterisant la statistique // lenghtScale et timeScale sont fonction des precedents
  OWN_PTR(Champ_front_base) moyenne_;
  OWN_PTR(Champ_front_base) turbKinEn_; // energie cinetique turbulente (k)
  OWN_PTR(Champ_front_base) turbDissRate_; // taux de dissipation turbulente (epsilon)
  DoubleVect dir_fluct_;
  //// Parametres discretisation en nombres d'onde
  int nbModes_ = -10; // nombres d'interlles de la discretisation en kappa
  double KeOverKmin_= 0.;
  double ratioCutoffWavenumber_= 0.; // au lieu de prendre kappa_mesh comme plus grand nombre d'onde, on prend kappa_mesh/ratioCutoffWavenumber (ratioCutoffWavenumber>1 permet de mieux discretiser les fluctuations => aspect plus lisse)

  int ecriture_ = 0, dim_ = -123;

  // Quantités auxiliaires aux valeurs constantes durant la simulation
  ArrOfDouble timeScale_; // echelle temporelle de turbulence ; aura la longueur nb_face ; utilise pour correlation temporelle
  ArrOfDouble kappa_center_; // valeur des normes de vecteurs d'onde kappa_n au mileu des intervalles de la discretisation en kappa ; aura la longueur nbModes
  DoubleTab amplitude_; // amplitudes des modes \hat{u}_n  ; aura les dimensions (nb_face,nbModes)
  int ncM_=-1 ; // sera 0 ou 1 selon si moyenne est uniforme ou pas

  double energy_spectrum_(const double& kappa, const double& tke, const double& ke, const double& keta);
};

#endif /* Champ_front_synt_included */

