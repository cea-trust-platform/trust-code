/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Mod_turb_hyd.h
// Directory:   $TRUST_ROOT/src/ThHyd/Modele_turbulence_base
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Mod_turb_hyd_included
#define Mod_turb_hyd_included

#include <Mod_turb_hyd_base.h>

Declare_deriv(Mod_turb_hyd_base);



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Mod_turb_hyd
//    Classe generique de la hierarchie des modeles de turbulence utilises
//    par une equation_base
//    un objet Mod_turb_hyd peut referencer n'importe quel objet derivant de
//    Mod_turb_hyd_base.
//    La plupart des methodes appellent les methodes de l'objet Mod_turb_hyd
//    sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//     Mod_turb_hyd_base MorEqn
//////////////////////////////////////////////////////////////////////////////
class Mod_turb_hyd : public MorEqn , public DERIV(Mod_turb_hyd_base)
{
  Declare_instanciable(Mod_turb_hyd);

public:

  inline Mod_turb_hyd& operator=(const Mod_turb_hyd_base& );
  inline const Champ_Fonc& viscosite_turbulente() const;
  /*Modification pour utilisation des fonctions de la classe MorEqn*/
  inline const Turbulence_paroi& loi_paroi() const;

  inline int preparer_calcul();
  inline void mettre_a_jour(double );
  inline void discretiser();
  inline void completer();
  inline int sauvegarder(Sortie& os) const;
  inline int reprendre(Entree& is);
  inline void imprimer(Sortie& os) const;
};


// Description:
//    Operateur d'affectation d'un Mod_turb_hyd_base dans
//    un Mod_turb_hyd.
// Precondition:
// Parametre: Mod_turb_hyd_base& x
//    Signification: partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Mod_turb_hyd&
//    Signification: resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Mod_turb_hyd& Mod_turb_hyd::operator=(const Mod_turb_hyd_base& x)
{
  DERIV(Mod_turb_hyd_base)::operator=(x);
  return *this;
}


// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie la viscosite turbulente.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: le champ representant la viscosite turbulente
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Fonc& Mod_turb_hyd::viscosite_turbulente() const
{
  return valeur().viscosite_turbulente();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie la loi de turbulence sur la paroi
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Turbulence_paroi&
//    Signification: la loi de turbulence sur la paroi
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Turbulence_paroi& Mod_turb_hyd::loi_paroi() const
{
  return valeur().loi_paroi();
}

// Description:
//    Appel a l'objet sous-jacent
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Mod_turb_hyd::preparer_calcul()
{
  return valeur().preparer_calcul();
}


// Description:
//    Appel a l'objet sous-jacent.
//    Effectue une mise a jour en temps
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Mod_turb_hyd::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}


// Description:
//    Appel a l'objet sous-jacent
//    Effectue une sauvegarde sur un flot de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Mod_turb_hyd::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}


// Description:
//    Appel a l'objet sous-jacent
//    Effectue une reprise sur un flot d'entree
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Mod_turb_hyd::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}


// Description:
//    Appel a l'objet sous-jacent
//    Discretise le modele de turbulence
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Mod_turb_hyd::discretiser()
{
  valeur().discretiser();
}


// Description:
//    Appel a l'objet sous-jacent
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Mod_turb_hyd::completer()
{
  valeur().completer();
  valeur().loi_paroi()->completer();
}

// Description:
//    Appel a l'objet sous-jacent
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Mod_turb_hyd::imprimer(Sortie& os) const
{
  valeur().imprimer(os);
}


#endif

