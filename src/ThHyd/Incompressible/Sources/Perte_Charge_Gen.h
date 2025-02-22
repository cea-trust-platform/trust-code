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

#ifndef Perte_Charge_Gen_included
#define Perte_Charge_Gen_included

#include <Terme_Source_Qdm.h>
#include <Domaine_forward.h>
#include <Champ_Don_base.h>
#include <Source_base.h>
#include <TRUST_Ref.h>
#include <Parser_U.h>

class Domaine_Cl_PolyMAC;
class Domaine_VF;
class Champ_Inc_base;
class Fluide_base;
class Param;

class Perte_Charge_Gen : public Source_base, public Terme_Source_Qdm
{
  Declare_base(Perte_Charge_Gen);
public:

  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override { } //rien

  void associer_pb(const Probleme_base&) override;  //!< associe le_fluide et la_vitesse
  void completer() override;

protected:

  virtual void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_domaines(const Domaine_dis_base&,const Domaine_Cl_dis_base&) override;

  //! Appele pour chaque face par ajouter()
  /**
     Utilise les intermediaires de calcul : u, norme_u, dh_valeur, reynolds
     Retourne le resultat calcule dans p_charge.

     Avantage : bonne factorisation.
     Inconvenient : cout de l'appel d'une methode virtuelle a
     l'interieur d'une boucle.

     \param u La vitesse a la face courante. 2 ou 3 composantes
     \param pos toto
     \param t titi
     \param norme_u La norme de la vitesse a la face courante
     \param dh Le diametre hydraulique a la face courante (tire de diam_hydr)
     \param nu la viscosite cinematique
     \param reynolds Le nombre de reynolds a la face courante : norme_u * dh_valeur / nu
     \param coeff_ortho coefficient dans la direction orthogonale
     \param coeff_long coefficient dans la direction longitudinale
     \param u_l vitesse dans la direction longitudinale
     \param v_valeur direction_longitudinale p_charge a 2 ou 3 composantes
     La perte de charge vaut -coeff_long*u_l*v_valeur -coeff_ortho(u -u_v*v_valeur)
  */
  virtual void coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos,
                                   double t, double norme_u, double dh, double nu, double reynolds,
                                   double& coeff_ortho, double& coeff_long, double& u_l, DoubleVect& v_valeur) const = 0;

  //! Diametre hydraulique utilise dans le calcul de la perte de charge
  OWN_PTR(Champ_Don_base) diam_hydr;
  //! Fluide associe au probleme
  OBS_PTR(Fluide_base) le_fluide;
  //! Vitesse associee a l'equation resolue
  OBS_PTR(Champ_Inc_base) la_vitesse;
  //! Domaine dans laquelle s'applique la perte de charge
  OBS_PTR(Domaine_VF) le_dom_vf_;
  OBS_PTR(Domaine_Cl_dis_base) le_dom_Cl_dis_;

  // Cas d'une sous-domaine
  bool sous_domaine = false; //!< Le terme est-il limite a une sous-domaine ?
  Nom nom_sous_domaine; //!< Nom de la sous-domaine, initialise dans readOn()
  OBS_PTR(Sous_Domaine) le_sous_domaine; //!< Initialise dans completer()
  int implicite_ = 1;

  mutable Parser_U lambda;
};

#endif /* Perte_Charge_Gen_included */
