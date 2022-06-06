/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Equation_included
#define Equation_included

#include <Equation_base.h>




Declare_deriv(Equation_base);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Equation
//     Classe generique de la hierarchie des equations. Un objet Equation peut
//     referencer n'importe quel objet derivant de Equation_base.
//     La plupart des methodes appellent les methodes de l'objet Probleme
//     sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//     Eqn_base Operateur Proprietes Champ_Inc
//////////////////////////////////////////////////////////////////////////////
class Equation : public DERIV(Equation_base)
{
  Declare_instanciable(Equation);
public :
  //  inline Equation(const Equation_base&);
  inline int nombre_d_operateurs() const;
  inline const Operateur& operateur(int) const;
  inline Operateur& operateur(int);
  inline const Champ_Inc& inconnue() const;
  inline Champ_Inc& inconnue();
  inline void discretiser();
  //
  inline int sauvegarder(Sortie& ) const override;
  inline int reprendre(Entree& ) override;
  //
  inline void associer_sch_tps_base(const Schema_Temps_base&);
  inline void associer_milieu_base(const Milieu_base&);
  inline const Milieu_base& milieu() const;
  inline Milieu_base& milieu();
};
/*
inline Equation::Equation(const Equation_base& eqn)
  : DERIV(Equation_base)(eqn)
{
}
*/


// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le nombre d'operateurs de l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'operateur de l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Equation::nombre_d_operateurs() const
{
  return valeur().nombre_d_operateurs();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le i-eme operateur.
//    (version const)
// Precondition:
// Parametre: int
//    Signification: indice de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur&
//    Signification: i-eme operateur de l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Operateur& Equation::operateur(int i) const
{
  return valeur().operateur(i);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le i-eme operateur.
// Precondition:
// Parametre: int
//    Signification: indice de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur&
//    Signification: le i-eme operateur de l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Operateur& Equation::operateur(int i)
{
  return valeur().operateur(i);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le champ inconnue.
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
inline const Champ_Inc& Equation::inconnue() const
{
  return valeur().inconnue();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Discretise l'equation: type l'inconnue.
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
// Postcondition: l'equation est discretisee
inline void Equation::discretiser()
{
  valeur().discretiser();
}
// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le champ inconnue.
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
inline Champ_Inc& Equation::inconnue()
{
  return valeur().inconnue();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Effectue une sauvegarde des inconnues de l'equation.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie de sauvegarde
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord: les inconnues sont sauvegardees
// Postcondition: la methode ne modifie pas l'objet
inline int Equation::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Effectue une reprise apres une sauvegarde.
// Precondition:
// Parametre: Entree& is
//    Signification: flot d'entre pour la reprise
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage (1 si reprise OK)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'objet sauvegarde est restaure
inline int Equation::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Associe un schema en temps a l'equation.
// Precondition:
// Parametre: Schema_Temps_base& un_schema_temps
//    Signification: le schema en temps a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un schema en temps associe
inline void Equation::associer_sch_tps_base(const Schema_Temps_base& un_schema_temps)
{
  valeur().associer_sch_tps_base(un_schema_temps);
}
// Description:
//    Appel a l'objet sous-jacent.
//    Associe un milieu physique a l'equation.
//    NOTE: cette methode est abstraite dans Equation_base et prend un
//          dans ses derivees. Voir par exemple: Navier_Stokes_std
// Precondition:
// Parametre: Milieu_base& mil
//    Signification: le milieu physique de l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu physique associe
inline void Equation::associer_milieu_base(const Milieu_base& mil)
{
  valeur().associer_milieu_base(mil);
}
// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le milieu physique de l'equation.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu physique de l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Milieu_base& Equation::milieu() const
{
  return valeur().milieu();
}
// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie le milieu physique de l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu physique de l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Milieu_base& Equation::milieu()
{
  return valeur().milieu();
}

#endif
