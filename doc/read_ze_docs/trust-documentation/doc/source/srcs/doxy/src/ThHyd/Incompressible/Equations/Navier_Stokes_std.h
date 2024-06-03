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

#ifndef Navier_Stokes_std_included
#define Navier_Stokes_std_included

#include <Traitement_particulier_NS.h>
#include <Navier_Stokes_IBM_impl.h>
#include <Operateur_Grad.h>
#include <Operateur_Conv.h>
#include <Operateur_Diff.h>
#include <Operateur_Div.h>
#include <Assembleur.h>
#include <Champ_Fonc.h>
#include <Champ_Don.h>
#include <TRUST_Ref.h>

class Fluide_base;

/*! @brief classe Navier_Stokes_std Cette classe porte les termes de l'equation de la dynamique
 *
 *     pour un fluide sans modelisation de la turbulence.
 *     On suppose l'hypothese de fluide incompressible: div U = 0
 *     On considere la masse volumique constante (egale a rho_0) sauf dans le
 *     terme des forces de gravite (hypotheses de Boussinesq).
 *     Sous ces hypotheses, on utilise la forme suivante des equations de
 *     Navier_Stokes:
 *        DU/dt = div(terme visqueux) - gradP/rho_0 + Bt(T-T0)g + autres sources/rho_0
 *        div U = 0
 *     avec DU/dt : derivee particulaire de la vitesse
 *          rho_0 : masse volumique de reference
 *          T0    : temperature de reference
 *          Bt    : coefficient de dilatabilite du fluide
 *          g     : vecteur gravite
 *     Rq : l'implementation de la classe permet bien sur de negliger
 *          certains termes de l'equation (le terme visqueux, le terme
 *          convectif, tel ou tel terme source).
 *     L'inconnue est le champ de vitesse.
 *
 *     Pour le traitement des cas un peu particulier : ajout de Traitement_particulier
 *     exemple : THI, canal (CA)
 *
 * @sa Equation_base Pb_Hydraulique Pb_Thermohydraulique
 */
class Navier_Stokes_std : public Equation_base, public Navier_Stokes_IBM_impl
{
  Declare_instanciable_sans_constructeur(Navier_Stokes_std);

public :

  Navier_Stokes_std();
  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_pb_base(const Probleme_base&) override;
  inline void associer_fluide(const Fluide_base& un_fluide ) { le_fluide = un_fluide; }
  const Milieu_base& milieu() const override;
  Milieu_base& milieu() override;
  const Fluide_base& fluide() const;
  Fluide_base& fluide();
  void associer_milieu_base(const Milieu_base& ) override;
  int nombre_d_operateurs() const override;
  int nombre_d_operateurs_tot() const override;
  const Operateur& operateur(int) const override;
  Operateur& operateur(int) override;
  const Operateur& operateur_fonctionnel(int) const override;
  Operateur& operateur_fonctionnel(int) override;
  Operateur_Div& operateur_divergence();
  const Operateur_Div& operateur_divergence() const;
  Operateur_Grad& operateur_gradient();
  const Operateur_Grad& operateur_gradient() const;
  Operateur_Diff& operateur_diff();
  const Operateur_Diff& operateur_diff() const;
  const Champ_Inc& inconnue() const override;
  Champ_Inc& inconnue() override;
  SolveurSys& solveur_pression();
  void discretiser() override;
  virtual void discretiser_vitesse();
  virtual void discretiser_grad_p();
  void completer() override;
  Entree& lire_cond_init(Entree&) override;
  bool initTimeStep(double dt) override;
  void mettre_a_jour(double temps) override;
  void abortTimeStep() override;
  int impr(Sortie& os) const override;

  void dimensionner_matrice_sans_mem(Matrice_Morse& matrice) override;

  /*
    interface {dimensionner,assembler}_blocs
    specificites : prend en compte le gradient de pression (en dernier)
  */
  int has_interface_blocs() const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;


  int sauvegarder(Sortie&) const override;
  int reprendre(Entree&) override;

  DoubleTab& derivee_en_temps_inco(DoubleTab& ) override;
  DoubleTab& corriger_derivee_expl(DoubleTab& ) override;
  DoubleTab& corriger_derivee_impl(DoubleTab& ) override;

  int preparer_calcul() override;

  inline Matrice& matrice_pression() { return matrice_pression_; }
  inline Assembleur& assembleur_pression() { return assembleur_pression_; }
  inline Champ_Inc& pression() { return la_pression; }
  inline Champ_Inc& grad_P() { return gradient_P; }
  inline const Champ_Inc& grad_P() const { return gradient_P; }
  inline Champ_Inc& pression_pa() { return la_pression_en_pa; }
  inline Champ_Inc& div() { return divergence_U; }
  inline const Champ_Inc& pression() const { return la_pression; }
  inline const Champ_Inc& pression_pa() const { return la_pression_en_pa; }
  inline const Champ_Inc& div() const { return divergence_U; }

  virtual const Champ_Don& diffusivite_pour_transport() const;
  virtual const Champ_base& diffusivite_pour_pas_de_temps() const;
  virtual const Champ_base& vitesse_pour_transport() const;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////

  const Motcle& domaine_application() const override;

  virtual inline const Champ_Inc& vitesse() const { return la_vitesse; }
  virtual inline Champ_Inc& vitesse() { return la_vitesse; }

  virtual void projeter();
  virtual int projection_a_faire();
  virtual void sauver() const;
  virtual void calculer_la_pression_en_pa();
  virtual void calculer_pression_hydrostatique(Champ_base& pression_hydro) const;
  int verif_Cl() const override;

  virtual const Champ_Inc& rho_la_vitesse() const;
  inline Operateur_Conv& get_terme_convectif() { return terme_convectif; }

  virtual void renewing_jacobians( DoubleTab& derivee );
  virtual void div_ale_derivative( DoubleTrav& derivee_ale, double timestep, DoubleTab& derivee, DoubleTrav& secmemP );
  virtual void update_pressure_matrix( void );

protected:
  virtual void discretiser_assembleur_pression();
  REF(Fluide_base) le_fluide;

  Champ_Inc la_vitesse, la_pression, divergence_U, gradient_P, la_pression_en_pa;
  Champ_Fonc la_vorticite, grad_u, critere_Q, pression_hydrostatique_, porosite_volumique, combinaison_champ;
  Champ_Fonc distance_paroi_globale, y_plus, Reynolds_maille, Courant_maille, Taux_cisaillement;

  Operateur_Conv terme_convectif;
  Operateur_Diff terme_diffusif;
  Operateur_Div divergence;
  Operateur_Grad gradient;
  Matrice matrice_pression_;
  Assembleur assembleur_pression_;
  SolveurSys solveur_pression_;

  int projection_initiale;
  double dt_projection, seuil_projection, seuil_uzawa, max_div_U, seuil_divU, raison_seuil_divU;
  mutable double cumulative_;

  Traitement_particulier_NS le_traitement_particulier;

  void uzawa(const DoubleTab&, const Matrice_Base&, SolveurSys&, DoubleTab&, DoubleTab&);
  Nom chaine_champ_combi;
  int methode_calcul_pression_initiale_;
  // pour genepi il est important d avoir divu =0 car accumulation d'erreur
  // meme si c'est pas faisable avec tous les schemas
  int div_u_nul_et_non_dsurdt_divu_;

private :
  // Pression au debut du pas de temps, utile pour abortTimeStep, notamment en Piso
  // WEC : a terme, mettre 2 cases en temps pour le champ la_pression
  DoubleTab P_n;
  // Pour ne pas calculer un gradient de plus si pas de postraitement du gradient de pression
  mutable int postraitement_gradient_P_;
  double LocalFlowRateRelativeError() const;  // Estimation of a flow rate relative error
};

#endif /* Navier_Stokes_std_included */
