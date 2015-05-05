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
// File:        Esp_Dist.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Esp_Dist.h>

Implemente_instanciable_sans_constructeur(Esp_Dist,"Esp_Dist",ArrOfInt);


// Description:
//    Lecture d'un espace distant dans un flot d'entree.
//    Un espace distant est represente par son tableau d'indices et le processus voisin concerne.
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
Entree& Esp_Dist::readOn(Entree& is)
{
  ArrOfInt::readOn(is);
  is >> PE_voisin_;
  is >> desc_ed_;
  return is;
}


// Description:
//    Ecriture d'un espace distant sur un flot de sortie
//    Un espace distant est represente par son tableau d'indices et le processus voisin concerne.
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
Sortie& Esp_Dist::printOn(Sortie& os) const
{
  ArrOfInt::printOn(os);
  os << PE_voisin_<<finl;
  os << desc_ed_;
  return os;
}


// Description:
//    Constructeur par defaut
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
Esp_Dist::Esp_Dist()
{
  PE_voisin_ = -1;
}

// Description:
//    Constructeur avec un processus voisin donne.
// Precondition:
// Parametre: int pe
//    Signification: numero du processus voisin
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Esp_Dist::Esp_Dist(int pe)
{
  PE_voisin_ = pe;
}

// Description:
//    Constructeur avec processus voisin et tableau d'indices donnes
// Precondition:
// Parametre: int pe
//    Signification: numero du processus voisin
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const ArrOfInt& A
//    Signification: tableau des indices des elements a envoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Esp_Dist::Esp_Dist(int pe,const ArrOfInt& A)
{
  PE_voisin_ = pe;
  ArrOfInt::operator=(A);
}


// Description:
//    Affectation d'un tableau d'indices
// Precondition:
// Parametre: const ArrOfInt& A
//    Signification: tableau des indices des elements a envoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Esp_Dist&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Esp_Dist& Esp_Dist::operator=(const ArrOfInt& A)
{
  ArrOfInt::operator=(A);
  return(*this);
}

// Description:
//    Copie du processus voisin et du tableau d'indices d'un espace distant donne
// Precondition:
// Parametre: const Esp_Dist& A
//    Signification: espace distant a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Esp_Dist&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Esp_Dist& Esp_Dist::operator=(const Esp_Dist& A)
{
  affecte_PE_voisin(A.num_PE_voisin());
  ArrOfInt::operator=((const ArrOfInt&) A);
  desc_ed_=A.desc_ed_;
  return(*this);
}


// Description:
//    Affecte le numero du processus voisin
// Precondition:
// Parametre: const int pe
//    Signification: numero du processus voisin
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Esp_Dist::affecte_PE_voisin(const int pe)
{
  PE_voisin_ = pe;
}


// Description:
//    Retourne le numero du processus voisin
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const int
//    Signification: numero du processus voisin
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Esp_Dist::num_PE_voisin() const
{
  return PE_voisin_;
}

// Description:
//    Retourne le descripteur de l'espace distant
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: VECT(Descripteur)&
//    Signification: descripteur de l'espace distant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
VECT(Descripteur)& Esp_Dist::desc_ed()
{
  return desc_ed_;
}

// Description:
//    Retourne le descripteur de l'espace distant
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: VECT(Descripteur)&
//    Signification: descripteur de l'espace distant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const VECT(Descripteur)& Esp_Dist::desc_ed() const
{
  return desc_ed_;
}

// Description:
//    Ajoute un espace distant
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
void Esp_Dist::ajoute_espace_distant(int pe, const ArrOfInt& v,
                                     VECT(Descripteur)& vdesc_ed)
{
  int i;
  PE_voisin_ = pe;
  (*this).resize_array(v.size_array());
  for(i=0; i<v.size_array(); i++)
    (*this)(i) = v(i);
  desc_ed_.dimensionner(vdesc_ed.size());
  for(i=0; i<vdesc_ed.size(); i++)
    {
      desc_ed_[i].affecte_deb(vdesc_ed[i].deb());
      desc_ed_[i].affecte_nb(vdesc_ed[i].nb());
      desc_ed_[i].affecte_stride(vdesc_ed[i].stride());
    }
}
