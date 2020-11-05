/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Navier_Stokes_std.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Navier_Stokes_std_included
#define Navier_Stokes_std_included

#include <Ref_Fluide_Incompressible.h>
#include <Operateur_Conv.h>
#include <Operateur_Diff.h>
#include <Operateur_Div.h>
#include <Operateur_Grad.h>
#include <Assembleur.h>
#include <Traitement_particulier_NS.h>
#include <Champ_Fonc.h>
#include <Champ_Don.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Navier_Stokes_std
//    Cette classe porte les termes de l'equation de la dynamique
//    pour un fluide sans modelisation de la turbulence.
//    On suppose l'hypothese de fluide incompressible: div U = 0
//    On considere la masse volumique constante (egale a rho_0) sauf dans le
//    terme des forces de gravite (hypotheses de Boussinesq).
//    Sous ces hypotheses, on utilise la forme suivante des equations de
//    Navier_Stokes:
//       DU/dt = div(terme visqueux) - gradP/rho_0 + Bt(T-T0)g + autres sources/rho_0
//       div U = 0
//    avec DU/dt : derivee particulaire de la vitesse
//         rho_0 : masse volumique de reference
//         T0    : temperature de reference
//         Bt    : coefficient de dilatabilite du fluide
//         g     : vecteur gravite
//    Rq : l'implementation de la classe permet bien sur de negliger
//         certains termes de l'equation (le terme visqueux, le terme
//         convectif, tel ou tel terme source).
//    L'inconnue est le champ de vitesse.
//
//    Pour le traitement des cas un peu particulier : ajout de Traitement_particulier
//    exemple : THI, canal (CA)
// .SECTION voir aussi
//      Equation_base Pb_Hydraulique Pb_Thermohydraulique
//////////////////////////////////////////////////////////////////////////////
class Navier_Stokes_std : public Equation_base
{
  Declare_instanciable_sans_constructeur(Navier_Stokes_std);

public :

  Navier_Stokes_std();
  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void associer_pb_base(const Probleme_base&);
  inline void associer_fluide(const Fluide_Incompressible& );
  const Milieu_base& milieu() const;
  Milieu_base& milieu();
  const Fluide_Incompressible& fluide() const;
  Fluide_Incompressible& fluide();
  void associer_milieu_base(const Milieu_base& );
  int nombre_d_operateurs() const;
  int nombre_d_operateurs_tot() const;
  const Operateur& operateur(int) const;
  Operateur& operateur(int);
  const Operateur& operateur_fonctionnel(int) const;
  Operateur& operateur_fonctionnel(int);
  Operateur_Div& operateur_divergence();
  const Operateur_Div& operateur_divergence() const;
  Operateur_Grad& operateur_gradient();
  const Operateur_Grad& operateur_gradient() const;
  const Champ_Inc& inconnue() const;
  Champ_Inc& inconnue();
  SolveurSys& solveur_pression();
  virtual void discretiser();
  virtual void completer();
  virtual bool initTimeStep(double dt);
  virtual void mettre_a_jour(double temps);
  virtual void abortTimeStep();
  virtual int impr(Sortie& os) const;

  void dimensionner_matrice_sans_mem(Matrice_Morse& matrice);

  /*
    interface {dimensionner,assembler}_blocs
    specificites : prend en compte le gradient de pression (en dernier)
  */
  virtual void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const;
  virtual void assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const;


  int sauvegarder(Sortie&) const;
  int reprendre(Entree&);

  virtual DoubleTab& derivee_en_temps_inco(DoubleTab& );
  virtual DoubleTab& corriger_derivee_expl(DoubleTab& );
  virtual DoubleTab& corriger_derivee_impl(DoubleTab& );

  virtual int preparer_calcul();

  inline Matrice& matrice_pression();
  inline Assembleur& assembleur_pression();
  inline Champ_Inc& pression();
  inline Champ_Inc& grad_P();
  inline const Champ_Inc& grad_P() const;
  inline Champ_Inc& pression_pa();
  inline Champ_Inc& div();
  inline const Champ_Inc& pression() const;
  inline const Champ_Inc& pression_pa() const;
  inline const Champ_Inc& div() const;

  virtual const Champ_Don& diffusivite_pour_transport();
  virtual const Champ_base& diffusivite_pour_pas_de_temps();
  virtual const Champ_base& vitesse_pour_transport();

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  virtual const Motcle& domaine_application() const;

  virtual inline const Champ_Inc& vitesse() const;
  virtual inline Champ_Inc& vitesse();

  virtual void projeter();
  virtual int projection_a_faire();

  virtual void sauver() const;

  virtual void calculer_la_pression_en_pa();

  virtual void calculer_pression_hydrostatique(Champ_base& pression_hydro) const;
  virtual int verif_Cl() const;


  virtual const Champ_Inc& rho_la_vitesse() const;

  inline Operateur_Conv& get_terme_convectif();

  virtual void renewing_jacobians( DoubleTab& derivee );
  virtual void div_ale_derivative( DoubleTrav& derivee_ale, double timestep, DoubleTab& derivee, DoubleTrav& secmemP );
  virtual void update_pressure_matrix( void );

  // <IBM> Methods for Immersed Boundary Method (IBM)
  inline const int& get_matrice_pression_penalisee_H1() const;
  inline const int& get_correction_matrice_pression() const;
  inline const int& get_correction_vitesse_modifie() const;
  inline const int& get_correction_pression_modifie() const;
  inline const int& get_gradient_pression_qdm_modifie() const;
  inline const int& get_i_source_pdf() const;
  inline const DoubleTab& get_champ_coeff_pdf_som() const;
  inline const int& get_correction_vitesse_projection_initiale() const;
  inline const int& get_correction_calcul_pression_initiale() const;
  inline const int& get_correction_matrice_projection_initiale() const;
  // </IBM>
protected:
  virtual void discretiser_assembleur_pression();


  REF(Fluide_Incompressible) le_fluide;

  Champ_Inc la_vitesse;
  Champ_Inc la_pression;
  Champ_Inc divergence_U;
  Champ_Inc gradient_P;
  Champ_Inc la_pression_en_pa;
  Champ_Fonc la_vorticite;
  Champ_Fonc grad_u;
  Champ_Fonc critere_Q;
  Champ_Fonc pression_hydrostatique_;
  Champ_Fonc porosite_volumique;
  Champ_Fonc combinaison_champ;
  Champ_Fonc y_plus;
  Champ_Fonc Reynolds_maille;
  Champ_Fonc Courant_maille;
  Champ_Fonc Taux_cisaillement;

  Operateur_Conv terme_convectif;
  Operateur_Diff terme_diffusif;
  Operateur_Div divergence;
  Operateur_Grad gradient;
  Matrice matrice_pression_;
  Assembleur assembleur_pression_;
  SolveurSys solveur_pression_;
  int projection_initiale;
  double dt_projection;
  double seuil_projection;
  double seuil_uzawa;
  double max_div_U;
  double seuil_divU;
  double raison_seuil_divU;
  double LocalFlowRateRelativeError_;
  mutable double cumulative_;

  Traitement_particulier_NS le_traitement_particulier;

  void uzawa(const DoubleTab&,
             const Matrice_Base&,
             SolveurSys&,
             DoubleTab&,
             DoubleTab&);
  Nom chaine_champ_combi;
  int methode_calcul_pression_initiale_;
  // pour genepi il est important d avori divu =0 car accumulation d'erreur
  // meme si c'est pas faisable avec tous les schemas
  int div_u_nul_et_non_dsurdt_divu_;

  // <IBM> Members required by the Immersed Boundary Method (IBM)
  int postraiter_gradient_pression_sans_masse_;
  int correction_matrice_projection_initiale_;
  int correction_calcul_pression_initiale_;
  int correction_vitesse_projection_initiale_;
  int correction_matrice_pression_;
  int matrice_pression_penalisee_H1_;
  int correction_vitesse_modifie_;
  int correction_pression_modifie_;
  int gradient_pression_qdm_modifie_;
  int i_source_pdf_;
  DoubleTab champ_coeff_pdf_som_;
  // </IBM>

private :
  // Pression au debut du pas de temps, utile pour abortTimeStep, notamment en Piso
  // WEC : a terme, mettre 2 cases en temps pour le champ la_pression
  DoubleTab P_n;
  // Pour ne pas calculer un gradient de plus si pas de postraitement du gradient de pression
  mutable int postraitement_gradient_P_;
};

// Description:
//    Renvoie le champ de pression
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la pression
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& Navier_Stokes_std::pression()
{
  return la_pression;
}

inline Champ_Inc& Navier_Stokes_std::pression_pa()
{
  return la_pression_en_pa;
}

// Description:
//    Renvoie le gradient de pression
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant le gradient de pression
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet


inline Champ_Inc& Navier_Stokes_std::grad_P()
{
  return gradient_P;
}

inline const Champ_Inc& Navier_Stokes_std::grad_P() const
{
  return gradient_P;
}

// Description:
//    Renvoie le champ de pression
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la pression
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Inc& Navier_Stokes_std::pression() const
{
  return la_pression;
}

inline const Champ_Inc& Navier_Stokes_std::pression_pa() const
{
  return la_pression_en_pa;
}

inline const Champ_Inc& Navier_Stokes_std::vitesse() const
{
  return la_vitesse;
}

inline Champ_Inc& Navier_Stokes_std::vitesse()
{
  return la_vitesse;
}

inline Matrice& Navier_Stokes_std::matrice_pression()
{
  return matrice_pression_;
}

inline Assembleur& Navier_Stokes_std::assembleur_pression()
{
  return assembleur_pression_;
}
inline Champ_Inc& Navier_Stokes_std::div()
{
  return divergence_U;
}
inline const Champ_Inc& Navier_Stokes_std::div() const
{
  return divergence_U;
}

// Description:
//    Associe un fluide incompressible a l'equation.
// Precondition:
// Parametre: Fluide_Incompressible& un_fluide
//    Signification: le fluide incompressible a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
// Postcondition: l'equation a un fluide associe
inline void Navier_Stokes_std::associer_fluide(const Fluide_Incompressible& un_fluide)
{
  le_fluide = un_fluide;
}

inline Operateur_Conv& Navier_Stokes_std::get_terme_convectif()
{
  return terme_convectif;
}

//
// <IBM> Methods for Immersed Boundary Method (IBM)
//

inline const int& Navier_Stokes_std::get_matrice_pression_penalisee_H1() const
{
  return matrice_pression_penalisee_H1_;
}

inline const int& Navier_Stokes_std::get_correction_matrice_pression() const
{
  return correction_matrice_pression_;
}

inline const int& Navier_Stokes_std::get_correction_vitesse_modifie() const
{
  return correction_vitesse_modifie_;
}

inline const int& Navier_Stokes_std::get_correction_pression_modifie() const
{
  return correction_pression_modifie_;
}

inline const int& Navier_Stokes_std::get_gradient_pression_qdm_modifie() const
{
  return gradient_pression_qdm_modifie_;
}

inline const int& Navier_Stokes_std::get_i_source_pdf() const
{
  return i_source_pdf_;
}

inline const DoubleTab& Navier_Stokes_std::get_champ_coeff_pdf_som() const
{
  return champ_coeff_pdf_som_;
}

inline const int& Navier_Stokes_std::get_correction_vitesse_projection_initiale() const
{
  return correction_vitesse_projection_initiale_;
}

inline const int& Navier_Stokes_std::get_correction_calcul_pression_initiale() const
{
  return correction_calcul_pression_initiale_;
}

inline const int& Navier_Stokes_std::get_correction_matrice_projection_initiale() const
{
  return correction_matrice_projection_initiale_;
}
// </IBM>

#endif
