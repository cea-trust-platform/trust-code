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
// File:        Echange_externe_impose_H.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_externe_impose_H_included
#define Echange_externe_impose_H_included

#include <Echange_global_impose.h>
#include <Ref_Fluide_Quasi_Compressible.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_externe_impose_H:
//    Cette classe represente le cas particulier de la classe
//    Echange_externe_impose pour une equation en enthalpie..
//
// .SECTION voir aussi
//    Echange_impose_base Echange_externe_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_externe_impose_H  : public Echange_externe_impose
{

  Declare_instanciable(Echange_externe_impose_H);

  int compatible_avec_eqn(const Equation_base&) const;
  void completer();

  double T_ext(int num) const;
  double T_ext(int num,int k) const;
  inline Champ_front& T_ext();
  inline const Champ_front& T_ext() const;

protected :
  REF(Fluide_Quasi_Compressible) le_fluide;

};

// Description:
//    Renvoie le champ T_ext de temperature imposee a la frontiere.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_front&
//    Signification: le champ T_ext de temperature imposee a la frontiere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_front& Echange_externe_impose_H::T_ext()
{
  return le_champ_front;
}

// Description:
//    Renvoie le champ T_ext de temperature imposee a la frontiere.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_front&
//    Signification: le champ T_ext de temperature imposee a la frontiere
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_front& Echange_externe_impose_H::T_ext() const
{
  return le_champ_front;
}

#endif
