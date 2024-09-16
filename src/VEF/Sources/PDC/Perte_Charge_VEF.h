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

#ifndef Perte_Charge_VEF_included
#define Perte_Charge_VEF_included

#include <Terme_Source_Qdm.h>
#include <Source_base.h>
#include <TRUST_Ref.h>
#include <Parser_U.h>

class Sous_domaine_VF;
class Domaine_Cl_VEF;
class Champ_Inc_base;
class Domaine_VEF;
class Fluide_base;
class Param;

//! Factorise les fonctionnalites de plusieurs pertes de charge en VEF, vitesse aux faces
/**
   Perte_Charge_Isotrope, Perte_Charge_Directionnelle et
   Perte_Charge_Anisotrope heritent de Perte_Charge_VEF. Elles
   doivent surcharger essentiellement readOn() et perte_charge().
   readOn() est suppose lire au moins diam_hydr et sous_domaine.

   Ces classes sont censees remplacer Perte_Charge_VEF_Face
   et Perte_Charge_VEF_P1NC.
*/

class Perte_Charge_VEF : public Source_base, public Terme_Source_Qdm
{
  Declare_base(Perte_Charge_VEF);

public:
  DoubleTab& ajouter(DoubleTab& ) const override; //!< Appelle perte_charge pour chaque face ou cela est necessaire
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override ;
  DoubleTab& calculer(DoubleTab& ) const override ;
  void associer_pb(const Probleme_base&) override;  //!< associe le_fluide et la_vitesse
  void completer() override;

protected:
  virtual void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_domaines(const Domaine_dis_base&,const Domaine_Cl_dis_base&) override;   //!< associe le_dom_VEF et le_dom_Cl_VEF

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
                                   double& coeff_ortho, double& coeff_long,
                                   double& u_l, DoubleVect& v_valeur) const=0;

  //! Diametre hydraulique utilise dans le calcul de la perte de charge
  Champ_Don diam_hydr;
  //! Fluide associe au probleme
  REF(Fluide_base) le_fluide;
  //! Vitesse associee a l'equation resolue
  REF(Champ_Inc_base) la_vitesse;
  //! Domaine dans laquelle s'applique la perte de charge
  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;

  // Cas d'un sous-domaine
  bool sous_domaine=false; //!< Le terme est-il limite a un sous-domaine ?
  Nom nom_sous_domaine; //!< Nom du sous-domaine, initialise dans readOn()
  REF(Sous_domaine_VF) le_sous_domaine_dis; //!< Initialise dans completer()
  int implicite_;

  mutable Parser_U lambda;
};

#endif
