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
// File:        Perte_Charge_Singuliere.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Perte_Charge_Singuliere_included
#define Perte_Charge_Singuliere_included


#include <Perte_Charge.h>

class Domaine;
class Zone_dis_base;
class IntVect;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Perte_Charge_Singuliere
//    Cette classe derivee de Perte_Charge est utilisee lorsque l'on veut
//    modeliser une perte de charge sur une surface (passage d'une grille par exemple)
//    par exemple)
// .SECTION voir aussi
//     Perte_Charge Perte_Charge_Reguliere
//////////////////////////////////////////////////////////////////////////////
class Perte_Charge_Singuliere : public Perte_Charge
{

public :

  Entree& lire_donnees(Entree& );
  void lire_surfaces(Entree&, const Domaine&, const Zone_dis_base&, IntVect&);
  inline double K() const;

protected :

  double K_;
};


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Singuliere::K() const
{
  return K_;
}

#endif
