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
// File:        Champ_Don_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Don_base.h>

Implemente_base(Champ_Don_base,"Champ_Don_base",Champ_base);


// Description:
//    Simple appel a:
//      Champ_base::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Don_base::printOn(Sortie& os) const
{
  return Champ_base::printOn(os);
}


// Description:
//    Simple appel a:
//      Champ_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_Don_base::readOn(Entree& is)
{
  return Champ_base::readOn(is);
}


// Description:
//    Fixe le nombre de degres de liberte par composante
// Precondition:
// Parametre: int nb_noeuds
//    Signification: le nombre de degre de liberte par composante
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de degres de liberte par composante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le tableaux des valeurs peut etre redimensionne
int Champ_Don_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  valeurs_.resize(nb_noeuds, nb_compo_);
  return nb_noeuds;
}

// Description:
//    Provoque une erreur !
//    A surcharger par les classes derivees !
//    non virtuelle pure par commoditees de developpement !
// Precondition:
// Parametre: Champ_base&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Don_base::affecter_(const Champ_base&)
{
  Cerr << "Champ_Don_base::affecter_ : "
       << que_suis_je()
       << "::affecter_ must be overloaded in derived classes";
  Cerr << finl;
  exit();
  // pour les compilos !!
  return *this;
}

// Description:
//    Provoque une erreur !
//    A surcharger par les classes derivees !
//    non virtuelle pure par commoditees de developpement !
// Precondition:
// Parametre: Champ_base&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Parametre: int
//    Signification: l'indice de la composante du champ a affecter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Don_base::affecter_compo(const Champ_base&, int)
{
  Cerr << "Champ_Don_base::affecter_compo must be overloaded in derived classes";
  Cerr << finl;
  exit();
  // pour les compilos !!
  return *this;
}

// Description:
//    NE FAIT RIEN
//    Mise a jour en temps.
// Precondition:
// Parametre: double
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
void Champ_Don_base::mettre_a_jour(double t)
{
  changer_temps(t);
  valeurs_.echange_espace_virtuel();
}

// Description:
//   NE FAIT RIEN
//   A surcharger dans les classes derivees
//   Provoque l'initialisation du champ si necessaire
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
int Champ_Don_base::initialiser(const double un_temps)
{
  mettre_a_jour(un_temps);
  return 1;
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
//    Reprise a partir d'un flot d'entree.
// Precondition:
// Parametre:Entree&
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_Don_base::reprendre(Entree& )
{
  return 1;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
//    Sauvegarder sur un flot de sortie
// Precondition:
// Parametre: Sortie&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Don_base::sauvegarder(Sortie& ) const
{
  return 1;
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
//    Sauvegarder sur un flot de sortie
// Precondition:
// Parametre: Sortie&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Don_base::imprime(Sortie& os, int ncomp) const
{
  Cerr << que_suis_je() << "::imprime not coded." << finl;
  exit();
  return 1;
}

// Description:
//    Surcharge Champ_base::valeurs()
//    Renvoie le tableau des valeurs
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champs
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Don_base::valeurs()
{
  return valeurs_;
}

// Description:
//    Surcharge Champ_base::valeurs()
//    Renvoie le tableau des valeurs
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champs
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Don_base::valeurs() const
{
  return valeurs_;
}

// Description:
//    Fixe le nombre de composantes et le nombre de valeurs nodales.
// Precondition:
// Parametre: int
//    Signification: le nombre de noeud par composante du champ
//                   (le nombre de dl par composante)
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int
//    Signification: le nombre de composante du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Don_base::dimensionner(int nb_noeuds, int nb_compo)
{
  fixer_nb_comp(nb_compo);
  fixer_nb_valeurs_nodales(nb_noeuds);
}
