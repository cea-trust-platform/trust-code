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
// File:        Champ_Uniforme_inst.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Uniforme_inst_included
#define Champ_Uniforme_inst_included


#include <Champ_Uniforme.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_Uniforme_inst
//     Un champ constant dans l'espace mais dependant du temps.
//     Cette classe est abstraite, une classe concrete derivant de
//     Champ_Uniforme_inst devra surcharger la methode me_calculer(double t)
//     de calcul du champ en fonction du temps.
// .SECTION voir aussi
//     Champ_Uniforme
//     Classe abstraite
//     Methode abstraite
//       void me_calculer(double t)
//////////////////////////////////////////////////////////////////////////////
class Champ_Uniforme_inst : public Champ_Uniforme
{

  Declare_base(Champ_Uniforme_inst);

public :

  inline void mettre_a_jour(double );
  virtual void me_calculer(double t) =0;

private :

  double temps_;
};


// Description:
//    Mise a jour en temps du champ.
// Precondition:
// Parametre: double temps
//    Signification: temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Champ_Uniforme_inst::mettre_a_jour(double un_temps)
{
  me_calculer(un_temps);
}

#endif
