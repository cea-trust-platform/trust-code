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
// File:        Champ_Inc.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Inc.h>

Implemente_deriv(Champ_Inc_base);
Implemente_instanciable(Champ_Inc,"Champ_Inc",DERIV(Champ_Inc_base));


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime le champ sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition:
Sortie& Champ_Inc::printOn(Sortie& os) const
{
  return DERIV(Champ_Inc_base)::printOn(os);
}


// Description:
//    Lecture du champ a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree sur lequel lire
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_Inc::readOn(Entree& is)
{
  return DERIV(Champ_Inc_base)::readOn(is);
}


// Description:
//    Appel a l'objet sous-jacent
//    Fixe le nombre de pas de temps a conserver.
// Precondition:
// Parametre: int i
//    Signification: le nombre de pas de temps a conserver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de pas de temps a conserver
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_Inc::fixer_nb_valeurs_temporelles(int i)
{
  return valeur().fixer_nb_valeurs_temporelles(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de pas de temps conserve.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de pas de temps conserve
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Inc::nb_valeurs_temporelles() const
{
  return valeur().nb_valeurs_temporelles() ;
}

// Description:
//    Appel a l'objet sous-jacent
//    Fixe le nombre de degres de liberte par composante.
// Precondition:
// Parametre: int i
//    Signification: le nombre de degres de libertee par composante
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de degres de libertee par composante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_Inc::fixer_nb_valeurs_nodales(int i)
{
  return valeur().fixer_nb_valeurs_nodales(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de degres de liberte par composante.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de degres de liberte par composante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Inc::nb_valeurs_nodales() const
{
  return valeur().nb_valeurs_nodales();
}

// Description:
//    Appel a l'objet sous-jacent
//    Associe le champ a une zone discretisee.
// Precondition:
// Parametre: Zone_dis_base& z
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour: Zone_dis_base&
//    Signification: la zone discretise associee au champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Zone_dis_base& Champ_Inc::associer_zone_dis_base(const Zone_dis_base& z)
{
  valeur().associer_zone_dis_base(z);
  return z;
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie la zone discretisee associee au champ.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_dis_base&
//    Signification:
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Zone_dis_base& Champ_Inc::zone_dis_base() const
{
  return valeur().zone_dis_base();
}

// Description:
//    Appel a l'objet sous-jacent
//    Fixe le temps courant.
// Precondition:
// Parametre: double& t
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Champ_Inc::changer_temps(const double& t)
{
  return valeur().changer_temps(t);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le temps courant
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_Inc::temps() const
{
  return valeur().temps();
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs au temps courant
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc::valeurs()
{
  return valeur().valeurs();
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs au temps courant
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps courant
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Inc::valeurs() const
{
  return valeur().valeurs();
}

// Description:
//    Appel a l'objet sous-jacent
//    Operateur de cast d'un Champ_Inc en un DoubleTab&
//    Renvoie le tableau des valeurs au temps courant
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Inc::operator DoubleTab& ()
{
  return valeur().valeurs();
}

// Description:
//    Appel a l'objet sous-jacent
//    Operateur de cast d'un Champ_Inc en un DoubleTab&
//    Renvoie le tableau des valeurs au temps courant
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps courant
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Champ_Inc::operator const DoubleTab& () const
{
  return valeur().valeurs();
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs au temps t+i.
// Precondition:
// Parametre: int i
//    Signification: l'indice de decalage dans le futur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps t+i
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc::futur(int i)
{
  return valeur().futur(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs au temps t+i.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice de decalage dans le futur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps t+i
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Inc::futur(int i) const
{
  return valeur().futur(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs au temps t-i.
// Precondition:
// Parametre: int i
//    Signification: l'indice de decalage dans le futur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps t-i
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc::passe(int i)
{
  return valeur().passe(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs au temps t-i.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice de decalage dans le futur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs au temps t-i
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Inc::passe(int i) const
{
  return valeur().passe(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Avance le temps courant du champ de i pas de temps.
// Precondition:
// Parametre: int i
//    Signification: le nombre de pas de temps duquel on avance
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc_base&
//    Signification: le champ inconnu place au nouveau temps courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Inc_base& Champ_Inc::avancer(int i)
{
  return valeur().avancer(i);
}

// Description:
//    Appel a l'objet sous-jacent
//    Recule le temps courant du champ de i pas de temps.
// Precondition:
// Parametre: int i
//    Signification: le nombre de pas de temps duquel on recule
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc_base&
//    Signification: le champ inconnu place au nouveau temps courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Inc_base& Champ_Inc::reculer(int i)
{
  return valeur().reculer(i);
}


// Description:
//    Appel a l'objet sous-jacent
//    Lit les valeurs du champs a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_Inc::lire_donnees(Entree& is)
{
  return valeur().lire_donnees(is);
}

// Description:
//    Appel a l'objet sous-jacent
//    Effectue une reprise sur un flot d'entree.
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
int Champ_Inc::reprendre(Entree& is)
{
  return valeur().reprendre(is);
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
int Champ_Inc::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

// Description:
//    Appel a l'objet sous-jacent
//    calcule l'integrale de l'une des composantes
//    du champ sur le domaine qui lui correspond
// Precondition:
// Parametre: int ncomp
//    Signification: le numero de la composante integree en espace
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: valeur de l'integrale
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_Inc::integrale_espace(int ncomp) const
{
  return valeur().integrale_espace(ncomp);
}

// Description:
void Champ_Inc::mettre_a_jour(double un_temps)
{
  valeur().mettre_a_jour(un_temps);
}

// Description:
//    Appel a l'objet sous-jacent
//    Associe le champ a l'equation dont il est une inconnue
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation auquel le champ doit s'associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Inc::associer_eqn(const Equation_base& eqn)
{
  valeur().associer_eqn(eqn);
}

// Description:
//    Appel a l'objet sous-jacent
//    Calcule la trace du champ sur une frontiere et renvoie
//    les valeurs.
// Precondition:
// Parametre: Frontiere_dis_base& fr
//    Signification: frontiere discretisee sur laquelle on veut
//                   calculer la trace du champ au temps tps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: DoubleTab& x, double tps
//    Signification: les valeurs du champ sur la frontiere au temps tps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: les valeurs du champ sur la frontiere au temps tps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  return valeur().trace(fr, x, tps,distant);
}

