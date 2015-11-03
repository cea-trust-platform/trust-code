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
// File:        MorEqn.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <MorEqn.h>
#include <Motcle.h>

// Description:
//    Associe une equation a l'objet.
//    Affecte le membre MorEqn::mon_equation avec l'objet
//    passe en parametre.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation a laquelle on veut s'associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void MorEqn::associer_eqn(const Equation_base& eqn)
{
  mon_equation=eqn;
}

// Calcul des valeurs liees a un morceau d equation (Operateurs, ...) pour postraitement
//
void MorEqn::calculer_pour_post(Champ& espace_stockage,const Nom& option, int comp) const
{
  Cerr<<"The method calculer_pour_post(...) is currently not coded"<<finl;
  Cerr<<"for the piece of the regarded equation and option chosen"<<finl;
  Cerr<<"Contact TRUST support for the coding of this method"<<finl;
  Process::exit();
}

Motcle MorEqn::get_localisation_pour_post(const Nom& option) const
{
  Cerr<<"MorEqn : the method get_localisation_pour_post is not coded"<<finl;
  Process::exit();
  throw;
  return MorEqn::get_localisation_pour_post(option);
}

