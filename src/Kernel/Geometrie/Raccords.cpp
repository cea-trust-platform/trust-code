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
// File:        Raccords.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Raccords.h>
Implemente_liste(Raccord);
Implemente_instanciable(Raccords,"Raccords",LIST(Raccord));


// Description:
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
Sortie& Raccords::printOn(Sortie& os) const
{
  return LIST(Raccord)::printOn(os);
}

// Description:
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
Entree& Raccords::readOn(Entree& is)
{
  return LIST(Raccord)::readOn(is);
}


// Description:
//    Associe une zone a tous les raccords de la liste.
// Precondition:
// Parametre: Zone& une_zone
//    Signification: la zone a associer
//    Valeurs par defaut:
//    Contraintes: reference constanet
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Raccords::associer_zone(const Zone& une_zone)
{
  LIST_CURSEUR(Raccord) curseur(*this);
  while(curseur)
    {
      curseur.valeur()->associer_zone(une_zone);
      ++curseur;
    }
}

// Description:
//    Renvoie le nombre de face total des Raccords
//    de la liste. (la somme des faces de tous les
//    raccords de la liste).
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de face total des Raccords
//                   de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Raccords::nb_faces() const
{
  CONST_LIST_CURSEUR(Raccord) curseur(*this);
  int nombre=0;
  while(curseur)
    {
      nombre+=curseur.valeur()->nb_faces();
      ++curseur;
    }
  return nombre;
}


// //    JY 26/08/97
// Description:
//    Renvoie le nombre de faces du type specifie
//    contenues dans la liste de raccords.
//    (somme des faces de ce type sur tous les
//     raccords de la liste)
// Precondition:
// Parametre: Type_Face type
//    Signification: le type des faces a compter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces du type specifie
//                   contenues dans la liste de raccords
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Raccords::nb_faces(Type_Face type) const
{
  CONST_LIST_CURSEUR(Raccord) curseur(*this);
  int nombre=0;
  while(curseur)
    {
      if (type == curseur.valeur()->faces().type_face())
        nombre+=curseur.valeur()->nb_faces();
      ++curseur;
    }
  return nombre;
}
