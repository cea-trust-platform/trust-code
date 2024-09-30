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

#ifndef Discret_Thyd_included
#define Discret_Thyd_included

#include <Discret_Thermique.h>


class Convection_Diffusion_Temperature;
class Navier_Stokes_std;
class Fluide_Ostwald;
class Equation_base;
class Fluide_base;

/*! @brief classe Discret_Thyd Cette classe est la classe de base representant une discretisation
 *
 *     spatiale appliquee aux problemes de thermo-hydrauliques.
 *     Les methodes virtuelles pures sont a implementer dans les classes
 *     derivees pour typer et discretiser les champs portes par les
 *     equations liees a la discretisation.
 *
 * @sa Discret_Thermique
 */
class Discret_Thyd : public Discret_Thermique
{
  Declare_base(Discret_Thyd);

public :

  void vitesse(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&, int nb_comp = 1) const;
  void translation(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void entcor(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void pression(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&) const;
  void pression_en_pa(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&) const;
  void divergence_U(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&) const;
  void gradient_P(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&, int nb_comp = 1) const;
  void concentration(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&, int = 1, const Nom nom_champ = "concentration") const;
  void diametre_hydraulique_face(const Domaine_dis_base&, const DoubleVect&, const Schema_Temps_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void section_passage(const Domaine_dis_base&, const DoubleVect&, const Schema_Temps_base&, OWN_PTR(Champ_Fonc_base)&) const;

  // Methodes virtuelles
  virtual void creer_champ_vorticite(const Schema_Temps_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const;
  virtual void grad_u(const Domaine_dis_base&, const Domaine_Cl_dis_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const;
  virtual void proprietes_physiques_fluide_Ostwald(const Domaine_dis_base&, Fluide_Ostwald&, const Navier_Stokes_std&, const Champ_Inc_base&) const;

  // cette methode permets de calculer/visualiser y dans tout le domaine : utile pour k-omega SST et les forces parois
  virtual void distance_paroi_globale(const Schema_Temps_base& , Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const;
  virtual void y_plus(const Domaine_dis_base& ,const Domaine_Cl_dis_base&,  const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)& ) const;
  virtual void grad_T(const Domaine_dis_base& z,const Domaine_Cl_dis_base& zcl, const Champ_Inc_base& eqn,OWN_PTR(Champ_Fonc_base)& ch) const;
  virtual void h_conv(const Domaine_dis_base& z,const Domaine_Cl_dis_base& zcl, const Champ_Inc_base& eqn,OWN_PTR(Champ_Fonc_base)& ch, Motcle& nom, int temp_ref) const;

  //pour VEF implemente const =0;
  virtual void critere_Q(const Domaine_dis_base& ,const Domaine_Cl_dis_base&,  const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)& ) const;

  inline virtual void reynolds_maille(const Domaine_dis_base&, const Fluide_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const
  {
    Cerr << "Reynolds_maille keyword not available for this discretization." << finl;
    Process::exit();
  }

  inline virtual void courant_maille(const Domaine_dis_base&, const Schema_Temps_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const
  {
    Cerr << "Courant_maille keyword not available for this discretization." << finl;
    Process::exit();
  }

  inline virtual void estimateur_aposteriori(const Domaine_dis_base&, const Domaine_Cl_dis_base&, const Champ_Inc_base&, const Champ_Inc_base&, const Champ_Don_base&, OWN_PTR(Champ_Fonc_base)&) const
  {
    Cerr << "estimateur_aposteriori keyword not available for this discretization." << finl;
    Cerr << "You should use it with the VEF_Aposteriori_discretisation !! " << finl;
    Process::exit();
  }

  inline virtual void taux_cisaillement(const Domaine_dis_base&, const Domaine_Cl_dis_base& ,const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const
  {
    Cerr << "Taux_cisaillement keyword not available for this discretization." << finl;
    Process::exit();
  }
};

#endif /* Discret_Thyd_included */
