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
// File:        Faces_Internes.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Faces_Internes.h>

Implemente_liste(Faces_Interne);
Implemente_instanciable(Faces_Internes,"Faces_Internes",LIST(Faces_Interne));


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
Sortie& Faces_Internes::printOn(Sortie& os) const
{
  return LIST(Faces_Interne)::printOn(os);
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
Entree& Faces_Internes::readOn(Entree& is)
{
  return LIST(Faces_Interne)::readOn(is);
}


// Description:
//    Associe une zone a tous les objets Faces_Interne
//    de la liste.
// Precondition:
// Parametre: Zone& une_zone
//    Signification: la zone a associer aux Faces_Interne
//                   de la liste
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Faces_Internes::associer_zone(const Zone& une_zone)
{
  LIST_CURSEUR(Faces_Interne) curseur(*this);
  while(curseur)
    {
      curseur->associer_zone(une_zone);
      ++curseur;
    }
}


// Description:
//    Renvoie le nombre total de faces contenues dans
//    la liste des Faces_Interne, i.e. la somme de toutes
//    les faces de tous les objet Faces_Interne contenu dans
//    la liste.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre total de faces contenues dans
//                   la liste des Faces_Interne
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Faces_Internes::nb_faces() const
{
  int nombre=0;
  CONST_LIST_CURSEUR(Faces_Interne) curseur(*this);
  while(curseur)
    {
      nombre+=curseur->nb_faces();
      ++curseur;
    }
  return nombre;
}

// //    JY 26/08/97
// Description:
//    Renvoie le nombre total de faces de type specifie
//    contenues dans la liste des Faces_Interne
//    i.e. la somme de toutes les faces de type specifie
//    de tous les objet Faces_Interne contenu dans
//    la liste.
// Precondition:
// Parametre: Type_Face type
//    Signification: le type des faces a compter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre total de faces contenues dans
//                   la liste des Faces_Interne
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Faces_Internes::nb_faces(Type_Face type) const
{
  CONST_LIST_CURSEUR(Faces_Interne) curseur(*this);
  int nombre=0;
  while(curseur)
    {
      if (type == curseur->faces().type_face())
        nombre+=curseur->nb_faces();
      ++curseur;
    }
  return nombre;
}
