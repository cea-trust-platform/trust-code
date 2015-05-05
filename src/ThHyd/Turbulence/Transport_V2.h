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
// File:        Transport_V2.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Transport_V2_included
#define Transport_V2_included

#include <Op_Diff_K_Eps_Bas_Re_base.h>
#include <Operateur_Diff.h>
#include <Operateur_Conv.h>
#include <Ref_Milieu_base.h>
#include <Ref_Modele_turbulence_hyd_K_Eps.h>
#include <Ref_Modele_turbulence_hyd_K_Eps_V2.h>
class Motcle;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Transport_V2
//    Cette classe represente l'equation de transport de la quantite v2
//    associee au modele de turbulence (k,eps,v2).
//    On instanciera un objet de cette classe uniquement si
//    on utilise le modele (k,eps,v2) pour traiter la turbulence
// .SECTION voir aussi
//    Equation_base
//////////////////////////////////////////////////////////////////////////////
class Transport_V2: public Equation_base
{

  Declare_instanciable_sans_constructeur(Transport_V2);

public:

  Transport_V2();
  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  inline void associer_Champ_Inconnu(const Champ_Inc& );
  void associer_milieu_base(const Milieu_base&);
  void associer_modele_turbulence(const Modele_turbulence_hyd_K_Eps_V2& );
  void discretiser();
  void discretiser_V2(const Schema_Temps_base& sch,Zone_dis& z, Champ_Inc& ch) const;
  const Milieu_base& milieu() const ;
  Milieu_base& milieu() ;
  inline const Modele_turbulence_hyd_K_Eps_V2& modele_turbulence() const;
  inline Modele_turbulence_hyd_K_Eps_V2& modele_turbulence();
  int nombre_d_operateurs() const;
  const Operateur& operateur(int) const;
  Operateur& operateur(int);
  inline const Champ_Inc& vitesse_transportante();
  virtual const Champ_Inc& inconnue() const;
  virtual Champ_Inc& inconnue();
  int controler_V2();

  /////////////////////////////////////////////////////
  const Motcle& domaine_application() const;

protected :

  Champ_Inc le_champ_V2;
  Operateur_Diff terme_diffusif;
  // Op_Diff_K_Eps_Bas_Re terme_diffusif;
  Operateur_Conv terme_convectif;

  REF(Milieu_base) le_fluide;
  REF(Champ_Inc) inco_eqn_associee;
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
inline Champ_Inc& Transport_V2::inconnue()
{
  return le_champ_V2;
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
inline const Champ_Inc& Transport_V2::inconnue() const
{
  return le_champ_V2;
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
inline const Modele_turbulence_hyd_K_Eps_V2& Transport_V2::modele_turbulence() const
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
inline Modele_turbulence_hyd_K_Eps_V2& Transport_V2::modele_turbulence()
{
  assert(mon_modele.non_nul());
  return mon_modele.valeur();
}


// Description:
//    Associe un champ de vitesse (transportante)a l'equation.
// Precondition:
// Parametre: Champ_Inc& vit
//    Signification: le champ de vitesse a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Transport_V2::associer_Champ_Inconnu(const Champ_Inc& vit)
{
  inco_eqn_associee = vit;
}

#endif


