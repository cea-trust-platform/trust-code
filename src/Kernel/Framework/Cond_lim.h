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
// File:        Cond_lim.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Cond_lim_included
#define Cond_lim_included

#include <Cond_lim_base.h>


Declare_deriv(Cond_lim_base);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Cond_lim
//    Classe generique servant a representer n'importe quelle classe
//    derivee de la classe Cond_lim_base
//    La plupart des methodes appellent les methodes de l'objet Probleme
//    sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//    Cond_lim_base
//////////////////////////////////////////////////////////////////////////////
class Cond_lim : public DERIV(Cond_lim_base)
{

  Declare_instanciable(Cond_lim);

public:

  //  Cond_lim(const Cond_lim_base& cl_base) : DERIV(Cond_lim_base)(cl_base) {}
  inline Cond_lim& operator=(const Cond_lim_base& cl_base);
  inline Cond_lim& operator=(const Cond_lim& cl);
  void adopt(Cond_lim& cl);
  inline Frontiere_dis_base& frontiere_dis();
  inline const Frontiere_dis_base& frontiere_dis() const;
  inline int operator == (const Cond_lim& x) const;
  inline void mettre_a_jour(double temps);
  inline void calculer_coeffs_echange(double temps);
  inline void completer();
  inline void injecter_dans_champ_inc(Champ_Inc& ) const;
  inline int compatible_avec_eqn(const Equation_base&) const;
  inline int compatible_avec_discr(const Discretisation_base&) const;

  inline void set_modifier_val_imp(int);

};


// Description:
//    Operateur d'affectation d'un objet de type Cond_lim_base&
//    dans une objet de type Cond_lim& .
// Precondition:
// Parametre: Cond_lim_base& cl_base
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Cond_lim&
//    Signification: renvoie (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Cond_lim& Cond_lim::operator=(const Cond_lim_base& cl_base)
{
  DERIV(Cond_lim_base)::operator=(cl_base);
  return *this;
}

// Description:
//    Operateur d'affectation d'un objet de type Cond_lim&
//    dans une objet de type Cond_lim& .
// Precondition:
// Parametre: Cond_lim_base& cl_base
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Cond_lim&
//    Signification: renvoie (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Cond_lim& Cond_lim::operator=(const Cond_lim& cl)
{
  DERIV(Cond_lim_base)::operator=(cl.valeur());
  return *this;
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la frontiere discretisee associee a la condition aux limites.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee associee a la condition aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Frontiere_dis_base& Cond_lim::frontiere_dis()
{
  return valeur().frontiere_dis();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la frontiere discretisee associee a la condition aux limites.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee associee a la condition aux limites
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Frontiere_dis_base& Cond_lim::frontiere_dis() const
{
  return valeur().frontiere_dis();
}

// Description:
//    Operateur d'egalite logique (comparaison) entre 2
//    conditions aux limites.
// Precondition:
// Parametre: Cond_lim& x
//    Signification: l'operande droit de l'operateur d'egalite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: 1 si l'objet est egal a l'objet condition aux limites
//                   passe en parametre 0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Cond_lim::operator == (const Cond_lim& x) const
{
  return (DERIV(Cond_lim_base)&) x == (DERIV(Cond_lim_base)&) *this ;
}


// Description:
//    Appel a l'objet sous-jacent
//    Mise a jour en temps de la condition aux limites.
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
inline void Cond_lim::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}


// Description:
//    Appel a l'objet sous-jacent
//    Calcul des coeffs d'echange de la condition aux limites.
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
inline void Cond_lim::calculer_coeffs_echange(double temps)
{
  valeur().calculer_coeffs_echange(temps);
}


// Description:
//    Appel a l'objet sous-jacent.
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
inline void Cond_lim::completer()
{
  valeur().completer();
}

// Description:
//    Appel a l'objet sous-jacent.
//    permet a une condition aux limites de s'injecter dans le Champ_Inc
//    avec l'equation passee en paramtre.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle on doit verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: 1 si compatible 0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Cond_lim::injecter_dans_champ_inc(Champ_Inc& ch) const
{
  valeur().injecter_dans_champ_inc(ch);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie 1 si les conditions aux limites sont compatibles
//    avec l'equation passee en paramtre.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle on doit verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: 1 si compatible 0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Cond_lim::compatible_avec_eqn(const Equation_base& eqn) const
{
  return valeur().compatible_avec_eqn(eqn);
}


// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie 1 si les conditions aux limites sont compatibles
//    avec la discretisation passee en paramtre.
// Precondition:
// Parametre: Discretisation_base& dis
//    Signification: la discretisation avec laquelle on doit verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: 1 si compatible 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Cond_lim::compatible_avec_discr(const Discretisation_base& dis) const
{
  return valeur().compatible_avec_discr(dis);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Positionnement du drapeau modifier_val_imp.
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
inline void Cond_lim::set_modifier_val_imp(int drap)
{
  valeur().set_modifier_val_imp(drap);
}

#endif
