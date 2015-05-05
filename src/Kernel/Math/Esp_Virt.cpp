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
// File:        Esp_Virt.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Esp_Virt.h>

Implemente_instanciable_sans_constructeur(Esp_Virt,"Esp_Virt",Objet_U);


// Description:
//    Lecture d'un espace virtuel dans un flot d'entree
//    Un espace virtuel est represente par ses 3 attributs PE_voisin_, deb_ et nb_
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
Entree& Esp_Virt::readOn(Entree& is)
{
  is >> PE_voisin_;
  is >> desc_ev_;
  return is;
}


// Description:
//    Ecriture d'un espace virtuel sur un flot de sortie
//    Un espace virtuel est represente par ses 3 attributs PE_voisin_, deb_ et nb_
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
Sortie& Esp_Virt::printOn(Sortie& os) const
{
  os << PE_voisin_ << finl;
  os << desc_ev_ << finl;
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
Esp_Virt::Esp_Virt()
{
  PE_voisin_ = -1;
}


// Description:
// Precondition:
//    Affecte le numero de processus a l'espace virtuel
// Parametre: const int pe
//    Signification: numero de processus voisin
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Esp_Virt::affecte_PE_voisin(int pe)
{
  PE_voisin_ = pe;
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
void Esp_Virt::affecte_deb(int new_deb,int num)
{
  desc_ev_[num].affecte_deb(new_deb);
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
void Esp_Virt::affecte_nb(int n,int num)
{
  desc_ev_[num].affecte_nb(n);
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
void Esp_Virt::affecte_descripteur(const VECT(Descripteur)& vdesc)
{
  desc_ev_=vdesc;
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
int Esp_Virt::num_PE_voisin() const
{
  return PE_voisin_;
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
int Esp_Virt::deb(int num) const
{
  return desc_ev_[num].deb();
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
int Esp_Virt::nb(int num) const
{
  return desc_ev_[num].nb();
}

// Description:
//    Retourne le descripteur de l'espace virtuel
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: VECT(Descripteur)&
//    Signification: descripteur de l'espace virtuel
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
VECT(Descripteur)& Esp_Virt::desc_ev()
{
  return desc_ev_;
}

// Description:
//    Retourne le descripteur de l'espace virtuel
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: VECT(Descripteur)&
//    Signification: descripteur de l'espace virtuel
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const VECT(Descripteur)& Esp_Virt::desc_ev() const
{
  return desc_ev_;
}
