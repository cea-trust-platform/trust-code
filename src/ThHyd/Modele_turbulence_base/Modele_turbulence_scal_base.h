/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Modele_turbulence_scal_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Modele_turbulence_base
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_scal_base_included
#define Modele_turbulence_scal_base_included


#include <Turbulence_paroi_scal.h>
#include <Ref_Convection_Diffusion_std.h>
#include <Ref_Champ_base.h>

class Motcle;
class Equation_base;
class Zone_dis;
class Zone_Cl_dis;
class Param;
class Schema_Temps_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Modele_turbulence_scal_base
//    Cette classe represente un modele de turbulence pour une equation de
//    convection-diffusion d'un scalaire couplee a Navier_Stokes. On utilise
//    deux classes derivees de cette classe de base qui representent le
//    modele de turbulence (k,eps) et le modele de turbulence sous maille.
//    Ces deux modeles ont en commun le calcul d'une diffusivite turbulente.
// .SECTION voir aussi
//    Modele_turbulence_scal_Prandtl Modele_turb_scal_Prandtl_sous_maille
//    Classe abstraite
//    Methode abstraite
//      void mettre_a_jour(double )
//      Entree& lire(const Motcle&, Entree&)
//////////////////////////////////////////////////////////////////////////////
class Modele_turbulence_scal_base : public Champs_compris_interface, public Objet_U
{

  Declare_base_sans_constructeur(Modele_turbulence_scal_base);

public:

  Modele_turbulence_scal_base();
  virtual int preparer_calcul();
  virtual bool initTimeStep(double dt);
  virtual void mettre_a_jour(double ) =0;
  inline const Champ_Fonc& diffusivite_turbulente() const;
  inline const Turbulence_paroi_scal& loi_paroi() const;
  inline int loi_paroi_non_nulle() const;
  inline Turbulence_paroi_scal& loi_paroi();
  virtual void discretiser();
  void discretiser_diff_turb(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const;
  void associer_eqn(const Equation_base& );
  virtual void completer();
  virtual void associer(const Zone_dis& , const Zone_Cl_dis& );
  void a_faire(Sortie&) const;
  virtual int sauvegarder(Sortie& ) const;
  virtual int reprendre(Entree& );
  inline Convection_Diffusion_std& equation();
  inline const Convection_Diffusion_std& equation() const;

  //Methodes de l interface des champs postraitables
  //////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  int limpr_nusselt( double, double, double) const;
  virtual void imprimer(Sortie&) const;

  virtual void set_param(Param&);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
protected:

  Champ_Fonc la_diffusivite_turbulente;
  REF(Convection_Diffusion_std) mon_equation;
  Turbulence_paroi_scal loipar;
  double dt_impr_nusselt_;

protected :

  Champs_compris champs_compris_;

};


// Description:
//    Renvoie la diffusivite turbulente.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: le champ representant la diffusivite turbulente
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Fonc& Modele_turbulence_scal_base::diffusivite_turbulente() const
{
  return la_diffusivite_turbulente;
}


// Description:
//    Renvoie la loi de turbulence sur la paroi
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Turbulence_paroi_scal&
//    Signification: la loi de turbulence sur la paroi
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Turbulence_paroi_scal& Modele_turbulence_scal_base::loi_paroi() const
{
  return loipar;
}

// Description:
//    Renvoie si oui ou non loi de paroi
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Modele_turbulence_scal_base::loi_paroi_non_nulle() const
{
  return loipar.non_nul();
}

// Description:
//    Renvoie la loi de turbulence sur la paroi
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Turbulence_paroi_scal&
//    Signification: la loi de turbulence sur la paroi
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Turbulence_paroi_scal& Modele_turbulence_scal_base::loi_paroi()
{
  return loipar;
}

//    Renvoie l'equation associee au modele de turbulence.
//    (c'est une equation du type Convection_Diffusion_std)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Convection_Diffusion_std&
//    Signification: l'equation associee au modele de turbulence
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Convection_Diffusion_std& Modele_turbulence_scal_base::equation()
{
  return mon_equation.valeur();
}

//    Renvoie l'equation associee au modele de turbulence.
//    (c'est une equation du type Convection_Diffusion_std)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Convection_Diffusion_std&
//    Signification: l'equation associee au modele de turbulence
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Convection_Diffusion_std& Modele_turbulence_scal_base::equation() const
{
  return mon_equation.valeur();
}

#endif
