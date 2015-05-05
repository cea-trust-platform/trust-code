/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        EqnF22base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EqnF22base_included
#define EqnF22base_included

#include <Operateur_Diff.h>
#include <Ref_Milieu_base.h>
#include <Ref_Modele_turbulence_hyd_K_Eps_V2.h>
#include <Matrice_Morse.h>
#include <SolveurSys.h>
#include <Equation_base.h>
class Motcle;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe EqnF22base
//    Cette classe represente l'equation de  la quantite F22
//    associee au modele de turbulence (k,eps,v2).
//    On instanciera un objet de cette classe uniquement si
//    on utilise le modele (k,eps,v2) pour traiter la turbulence
// .SECTION voir aussi
//    Equation_base
//////////////////////////////////////////////////////////////////////////////
class EqnF22base : public Equation_base
{

  Declare_base_sans_constructeur(EqnF22base);

public:

  EqnF22base();
  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void associer_milieu_base(const Milieu_base&);
  void associer_modele_turbulence(const Modele_turbulence_hyd_K_Eps_V2& );
  const Milieu_base& milieu() const ;
  Milieu_base& milieu() ;
  inline const Modele_turbulence_hyd_K_Eps_V2& modele_turbulence() const;
  inline Modele_turbulence_hyd_K_Eps_V2& modele_turbulence();
  int nombre_d_operateurs() const;
  const Operateur& operateur(int) const;
  Operateur& operateur(int);
  virtual const Champ_Inc& inconnue() const;
  virtual Champ_Inc& inconnue();
  void discretiser();
  void discretiser_F22(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const;

  virtual void resoudre() =0;
  virtual void completer();
  virtual int preparer_calcul();

  /////////////////////////////////////////////////////

  const Motcle& domaine_application() const;

  virtual int sauvegarder( Sortie&) const;
  virtual int reprendre( Entree&);


protected :
  Matrice_Morse matF22;
  Operateur_Diff terme_diffusif;

  Champ_Inc le_champ_F22;
  SolveurSys solveur;

  double C1, C2;
  REF(Milieu_base) le_fluide;
  REF(Modele_turbulence_hyd_K_Eps_V2) mon_modele;


};


// Description:
//    Renvoie le champ inconnue de l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue de l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& EqnF22base::inconnue()
{
  return le_champ_F22;
}


// Description:
//    Renvoie le champ inconnue de l'equation.
//    Un champ vecteur contenant K et epsilon.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue de l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Inc& EqnF22base::inconnue() const
{
  return le_champ_F22;
}


// Description:
//   Renvoie le modele de turbulence associe a l'equation.
//   (version const)
// Precondition: un modele de turbulence doit avoir ete associe
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Modele_turbulence_hyd_K_Eps&
//    Signification: le modele de turbulence associe a l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Modele_turbulence_hyd_K_Eps_V2& EqnF22base::modele_turbulence() const
{
  assert(mon_modele.non_nul());
  return mon_modele.valeur();
}


// Description:
//   Renvoie le modele de turbulence associe a l'equation.
// Precondition: un modele de turbulence doit avoir ete associe
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Modele_turbulence_hyd_K_Eps&
//    Signification: le modele de turbulence associe a l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Modele_turbulence_hyd_K_Eps_V2& EqnF22base::modele_turbulence()
{
  assert(mon_modele.non_nul());
  return mon_modele.valeur();
}

#endif


