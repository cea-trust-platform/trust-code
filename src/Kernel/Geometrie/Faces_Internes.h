/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Faces_Interne.h>
#include <List.h>


#ifndef Faces_Internes_included
#define Faces_Internes_included

Declare_liste(Faces_Interne);



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Class Faces_Internes
//    Cette classe represente une liste d'objets de type Faces_Interne
// .SECTION voir aussi
//    Faces_Interne
//////////////////////////////////////////////////////////////////////////////
class Faces_Internes : public LIST(Faces_Interne)
{
  Declare_instanciable(Faces_Internes);

public :

  void associer_zone(const Zone&);
  int nb_faces() const;
  inline int nb_faces_internes() const;
  //JY 26/08/97
  int nb_faces(Type_Face type) const;
};


// Description:
//    Renvoie le nombre d'objet Face_Interne stockes
//    dans la liste.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'objet Face_Interne stockes
//                   dans la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Faces_Internes::nb_faces_internes() const
{
  return size();
}

#endif
