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
// File:        Descripteur.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Descripteur.h>

Implemente_instanciable_sans_constructeur(Descripteur,"Descripteur",Objet_U);


// Description:
//    Lecture d'un espace virtuel dans un flot d'entree
//    Un espace virtuel est represente par ses 3 attributs deb_ ,nb_ et stride_
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Descripteur::readOn(Entree& is)
{
  is >> deb_;
  is >> nb_;
  is >> stride_;
  return is;
}


// Description:
//    Ecriture d'un espace virtuel sur un flot de sortie
//    Un espace virtuel est represente par ses 3 attributs deb_, nb_ et stride_
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Descripteur::printOn(Sortie& os) const
{
  os << deb_ << finl;
  os << nb_ << finl;
  os << stride_ << finl;
  return os;
}


// Description:
// Precondition:
//    Constructeur par defaut.
//    Tous les attributs sont fixes a -1
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
Descripteur::Descripteur()
{
  stride_ = -1;
  deb_ = -1;
  nb_ = -1;
}


// Description:
// Precondition:
//    Affecte le stride
// Parametre: const int stride
//    Signification: stride
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Descripteur::affecte_stride(int the_stride)
{
  stride_ = the_stride;
}


// Description:
// Precondition:
//    Affecte le l'indice de debut de la zone virtuelle
// Parametre: const int deb
//    Signification: indice de debut de la zone virtuelle
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Descripteur::affecte_deb(int new_deb)
{
  deb_ = new_deb;
}


// Description:
// Precondition:
//    Affecte le nombre d'elements de la zone virtuelle
// Parametre: const int nb
//    Signification: nombre d'elements de la zone virtuelle
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Descripteur::affecte_nb(int nb_new)
{
  nb_ = nb_new;
}


// Description:
//    Retourne le numero de processus voisin de la zone virtuelle
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const int
//    Signification: numero de processus voisin de la zone virtuelle
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Descripteur::stride() const
{
  return stride_;
}


// Description:
//    Retourne l'indice de debut de la zone virtuelle
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const int
//    Signification: indice de debut de la zone virtuelle
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Descripteur::deb() const
{
  return deb_;
}


// Description:
//    Retourne le nombre d'elements de la zone virtuelle
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const int
//    Signification: nombre d'elements de la zone virtuelle
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Descripteur::nb() const
{
  return nb_;
}

