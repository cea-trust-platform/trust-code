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

#include <Typer_Lire.h>
#include <Synonyme_info.h>
#include <Interprete_bloc.h>

Implemente_instanciable(Typer_Lire,"Typer_Lire|Type_Read",Lire);

// Description:
//  appel a la methode printOn de la classe Interprete
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
Sortie& Typer_Lire::printOn(Sortie& os) const
{
  return Lire::printOn(os);
}

// Description:
//  appel a la methode readOn de la classe Interprete
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
Entree& Typer_Lire::readOn(Entree& is)
{
  return Lire::readOn(is);
}

// Description:
// Read an object.
Entree& Typer_Lire::interpreter(Entree& is)
{
  // Example in a data file:
  // Typer_Lire name type { ... }
  Nom name;
  is >> name; // The object name is read from the input stream is

  if (objet_existant(name)) //nom d'un objet existant -> on le lit
    {
      Objet_U& object=objet(name);
      return is >> object; // Then "{ ... }" is read by the object readOn
    }
  else //pas le nom d'un objet existant -> on lit le type, on cree l'objet, puis on le lit
    {
      // Lire name type { ... }
      DerObjU ref;
      Nom type;
      is >> type;
      if (type=="{")
        {
          Objet_U& object=objet(name); // Pour faire planter avec le message d'erreur habituel
          return is >> object;
        }
      ref.typer(type);
      //on ajoute l'objet a l'interprete courant...
      Objet_U& obj = Interprete_bloc::interprete_courant().ajouter(name, ref);
      return is >> obj;           //et on lit
    }
}

