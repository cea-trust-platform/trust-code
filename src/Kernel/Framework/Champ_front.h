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
// File:        Champ_front.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_included
#define Champ_front_included


#include <Champ_front_base.h>

Declare_deriv(Champ_front_base);


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front
//     Classe generique de la hierarchie des champs aux frontieres,
//     un objet Champ_front peut referencer n'importe quel objet derivant de
//     Champ_front_base.
//     La plupart des methodes appellent les methodes de l'objet Probleme
//     sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//     Champ_front_base
//////////////////////////////////////////////////////////////////////////////
class Champ_front : public DERIV(Champ_front_base), public Champ_Proto
{

  Declare_instanciable(Champ_front);

public:

  Champ_front(const Champ_front_base& x);
  inline Champ_front& operator=(const Champ_front_base& ch_base) ;
  inline Champ_front& operator=(const Champ_front& ch_base) ;
  inline void nommer(const Nom& name) ;
  inline const Nom& le_nom() const ;
  inline void associer_fr_dis_base(const Frontiere_dis_base& ) ;
  inline const Frontiere_dis_base& frontiere_dis() const ;
  inline Frontiere_dis_base& frontiere_dis() ;
  inline int nb_comp() const ;
  inline void mettre_a_jour(double temps);
  inline void calculer_coeffs_echange(double temps);

  inline DoubleTab& valeurs() ;
  inline const DoubleTab& valeurs() const ;

  inline const Noms& get_synonyms() const;
  inline void add_synonymous(const Nom& nom);
};


// Description:
//    Constructeur par copie d'un Champ_front_base dans
//    un Champ_front.
// Precondition:
// Parametre: Champ_front_base& x
//    Signification: le champ a copier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_front::Champ_front(const Champ_front_base& x)
  :DERIV(Champ_front_base)(x)
{}


// Description:
//    Operateur d'affectation d'un Champ_front_base dans un
//    Champ_front.
// Precondition:
// Parametre: Champ_front_base& ch_base
//    Signification: le champ partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_front&
//    Signification: le resultat de l'affectation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_front& Champ_front::operator=(const Champ_front_base& ch_base)
{
  DERIV(Champ_front_base)::operator=(ch_base);
  return *this;
}


// Description:
//    Operateur d'affectation d'un Champ_front_base dans un
//    Champ_front.
// Precondition:
// Parametre: Champ_front_base& ch_base
//    Signification: le champ partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_front&
//    Signification: le resultat de l'affectation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_front& Champ_front::operator=(const Champ_front& ch_front)
{
  DERIV(Champ_front_base)::operator=(ch_front.valeur());
  return *this;
}

// Description:
//    Appel a l'objet sous-jacent
//    Donne un nom au champ
// Precondition:
// Parametre: Nom& name
//    Signification: le nom a donner au champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Champ_front::nommer(const Nom& name)
{
  valeur().nommer(name) ;
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nom du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Champ_front::le_nom() const
{
  return valeur().le_nom();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la frontiere discretisee associee au champ
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee associee au champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Frontiere_dis_base& Champ_front::frontiere_dis() const
{
  return valeur().frontiere_dis();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la frontiere discretisee associee au champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee associee au champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Frontiere_dis_base& Champ_front::frontiere_dis()
{
  return valeur().frontiere_dis();
}


// Description:
//    Appel a l'objet sous-jacent
//    Associe une frontiere discretisee au champ.
// Precondition:
// Parametre: Frontiere_dis_base& fr
//    Signification: la frontiere discretisee a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Champ_front::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  valeur().associer_fr_dis_base(fr);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de composantes du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de composantes du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Champ_front::nb_comp() const
{
  return valeur().nb_comp();
}


// Description:
//    Appel a l'objet sous-jacent
//    Effectue une mise a jour en temps.
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
inline void Champ_front::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}


// Description:
//    Calcul (si necessaire) les coefficients d'echange pour
//    un couplage
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
inline void Champ_front::calculer_coeffs_echange(double temps)
{
  valeur().calculer_coeffs_echange(temps);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline DoubleTab& Champ_front::valeurs()
{
  return valeur().valeurs();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs du champ
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const DoubleTab& Champ_front::valeurs() const
{
  return valeur().valeurs();
}

inline const Noms& Champ_front::get_synonyms() const
{
  return valeur().get_synonyms();
}

inline void Champ_front::add_synonymous(const Nom& nom)
{
  valeur().add_synonymous(nom);
}
#endif
