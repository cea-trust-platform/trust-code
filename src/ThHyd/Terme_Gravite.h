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
// File:        Terme_Gravite.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Gravite_included
#define Terme_Gravite_included


#include <Ref_Champ_Don_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Terme_Gravite
//    Cette represente un terme de gravite. Elle contient
//    une reference sur champ donne qui represente une gravite.
// .SECTION voir aussi
//    Champ_Don_base
//    Classe hors hierarchie TrioU
//////////////////////////////////////////////////////////////////////////////
class Terme_Gravite
{
public :

  inline Terme_Gravite() {}
  inline void associer_gravite(const Champ_Don_base& );

protected:

  REF(Champ_Don_base) gravite;
};


// Description:
//    Associe un champ representant la gravite a l'objet Gravite.
// Precondition:
// Parametre: Champ_Don_base& g
//    Signification: un champ donne representant une gravite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Terme_Gravite::associer_gravite(const Champ_Don_base& g)
{
  gravite=g;
}

#endif
