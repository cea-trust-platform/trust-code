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
// File:        DescFactory.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <DescFactory.h>

DescFactory* DescFactory::_instance = 0;

// Description:
//    Retourne un pointeur sur l'instance de la DescFactory
//    Cree un nouvel objet DescFactory si aucune instance n'a deja ete creee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DescFactory*
//    Signification: pointeur sur l'instance de la DescFactory
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DescFactory* DescFactory::Instance()
{
  if (_instance == 0)
    {
      _instance = new DescFactory;
    }
  return _instance;
}


// Description:
//    Operateur d'affichage d'un etat de la DescFactory mem sur le flot de sortie os
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const DescFactory& mem
//    Signification: la DescFactory a examiner
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie& le flot de sortie modifie
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& operator << (Sortie& os, const DescFactory& desc)
{
  return os;
}


// Description:
//    Destruction de la DescFactory
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
DescFactory::~DescFactory()
{
}

// Description:
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
DescStructure& DescFactory::creer_structure_base(int deb, int nb, int stride)
{
  DescStructure desc;
  desc.ajoute_descripteur(deb,nb,stride);
  return les_descripteurs_.add(desc);
}

// Description:
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
DescStructure& DescFactory::creer_structure_test(const DescStructure& desc)
{
  return les_descripteurs_.add(desc);
}

// Description:
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
//DescStructure& DescFactory::creer_structure(const Nom& type)
//{
//}
