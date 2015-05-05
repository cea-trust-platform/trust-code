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
// File:        Raccords.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Raccords_included
#define Raccords_included




#include <Raccord.h>
#include <List.h>



Declare_liste(Raccord);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Raccords
//    Cette represente une liste d'objets de type Raccord.
// .SECTION voir aussi
//    Raccord
//////////////////////////////////////////////////////////////////////////////
class Raccords : public LIST(Raccord)
{
  Declare_instanciable(Raccords);
public :
  void associer_zone(const Zone&);
  int nb_faces() const;
  inline int nb_raccords() const;
  //JY 26/08/97
  int nb_faces(Type_Face type) const;
};


// Description:
//    Renvoie le nombre de raccords contenus dans la liste.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de raccords contenus dans la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Raccords::nb_raccords() const
{
  return size();
}
#endif
