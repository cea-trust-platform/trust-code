/****************************************************************************
* Copyright (c) 2025, CEA
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
// File:        Pb_Couple_Optimisation_IBM.cpp
//
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

// Description de Pb_Couple_Optimisation_IBM:Classe heritant de Pb_Couple
// Precondition :
// Parametre :
//     Signification :
//     Valeurs par defaut :
//     Contraintes :
//     Entree :
//     Entree/Sortie :
//     Sortie :
// Retour :
//     Signification :
//     Contraintes :
// Exception :
// Effets de bord :
// Postcondition :
//

#include <Pb_Couple_Optimisation_IBM.h>
#include <verif_cast.h>
#include <Probleme_base.h>

Implemente_instanciable(Pb_Couple_Optimisation_IBM,"Pb_Couple_Optimisation_IBM",Probleme_Couple);

Entree& Pb_Couple_Optimisation_IBM::readOn(Entree& is)
{
  return is;
}

Sortie& Pb_Couple_Optimisation_IBM::printOn(Sortie& os) const
{
  return Probleme_Couple::printOn(os);
}

void Pb_Couple_Optimisation_IBM::initialize( )
{
  Probleme_Couple::initialize();
}

int Pb_Couple_Optimisation_IBM::associer_(Objet_U& ob)
{
  Probleme_Couple::associer_(ob);
  return 1;
}

void Pb_Couple_Optimisation_IBM::le_modele_interpolation_IBM(const Interpolation_IBM_base& un_modele_d_interpolation)
{
  my_interpolation_IBM_ = un_modele_d_interpolation;
}

void Pb_Couple_Optimisation_IBM::validateTimeStep()
{
  Probleme_Couple::validateTimeStep();
}
