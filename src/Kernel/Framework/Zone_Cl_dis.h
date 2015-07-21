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
// File:        Zone_Cl_dis.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_Cl_dis_included
#define Zone_Cl_dis_included





#include <Zone_Cl_dis_base.h>

Declare_deriv(Zone_Cl_dis_base);


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Zone_Cl_dis
//    Classe generique de la hierarchie des conditions aux limites
//    discretisees. Un objet Zone_Cl_dis peut referencer n'importe quel
//    derivant de Zone_Cl_dis_base.
//    La plupart des methodes appellent les methodes de l'objet Probleme
//    sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//    Zone_Cl_dis_base
//////////////////////////////////////////////////////////////////////////////
class Zone_Cl_dis : public DERIV(Zone_Cl_dis_base)
{

  Declare_instanciable(Zone_Cl_dis);

public :

  inline void associer_eqn(const Equation_base& );
  inline void imposer_cond_lim(Champ_Inc&, double);
  inline void mettre_a_jour(double temps);
  inline const Cond_lim& les_conditions_limites(int ) const;
  inline Cond_lim& les_conditions_limites(int );
  inline Conds_lim&  les_conditions_limites();
  inline const Conds_lim&  les_conditions_limites() const;
  inline int nb_cond_lim() const;
  inline Zone& zone();
  inline const Zone& zone() const;
  inline int nb_faces_Cl() const;

};



// Description:
//    Appel a l'objet sous-jacent.
//    Associe l'equation dont l'objet represente les conditions aux limites.
// Precondition:
// Parametre: Equation_base& equation
//    Signification: l'equation a laquelle seront associees les conditions aux limites
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'objet a son equation associee
inline void Zone_Cl_dis::associer_eqn(const Equation_base& equation)
{
  valeur().associer_eqn(equation);
}


// Description:
//    Appel a l'objet sous-jacent.
//    Impose les conditions aux limites a un temps donne du Champ_Inc
// Precondition:
// Parametre: Champ_Inc& ch
//    Signification: le champ inconnu sur lequel on impose les conditions
//                   aux limites
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Zone_Cl_dis::imposer_cond_lim(Champ_Inc& ch, double temps)
{
  valeur().imposer_cond_lim(ch,temps);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la i-ieme condition aux limites.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'index de la i-ieme condition aux limites
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Cond_lim&
//    Signification: la i-ieme condition aux limites
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Cond_lim& Zone_Cl_dis::les_conditions_limites(int i) const
{
  return valeur().les_conditions_limites(i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la i-ieme condition aux limites.
// Precondition:
// Parametre: int i
//    Signification: l'index de la i-ieme condition aux limites
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Cond_lim&
//    Signification: la i-ieme condition aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Cond_lim& Zone_Cl_dis::les_conditions_limites(int i)
{
  return valeur().les_conditions_limites(i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des conditions aux limites.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Conds_lim&
//    Signification: le tableau des conditions aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Conds_lim& Zone_Cl_dis::les_conditions_limites()
{
  return valeur().les_conditions_limites();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des conditions aux limites.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Conds_lim&
//    Signification: le tableau des conditions aux limites
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Conds_lim& Zone_Cl_dis::les_conditions_limites() const
{
  return valeur().les_conditions_limites();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de conditions aux limites.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de conditions aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone_Cl_dis::nb_cond_lim() const
{
  return valeur().nb_cond_lim();
}


// Description:
//    Appel a l'objet sous-jacent
//    Effectue une mise a jour en temps des conditions aux limites.
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Zone_Cl_dis::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}
inline Zone& Zone_Cl_dis::zone()
{
  return valeur().zone();
}
inline const Zone& Zone_Cl_dis::zone() const
{
  return valeur().zone();
}
inline int Zone_Cl_dis::nb_faces_Cl() const
{
  return valeur().nb_faces_Cl();
}
#endif
