/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Operateur_Evanescence.h>
#include <Discretisation_base.h>
#include <Champ_base.h>
#include <Champ_Don.h>

Implemente_deriv(Operateur_Evanescence_base);
Implemente_instanciable(Operateur_Evanescence,"Operateur_Evanescence",DERIV(Operateur_Evanescence_base));

Sortie& Operateur_Evanescence::printOn(Sortie& os) const { return Operateur::ecrire(os); }
Entree& Operateur_Evanescence::readOn(Entree& is) { return Operateur::lire(is); }

void Operateur_Evanescence::typer()
{
  Cerr << "Operateur_Evanescence::typer(" << typ << ")" << finl;
  Equation_base& eqn = mon_equation.valeur();
  Nom nom_type = eqn.discretisation().get_name_of_type_for(que_suis_je(), typ, eqn);
  DERIV(Operateur_Evanescence_base)::typer(nom_type);
  Cerr << valeur().que_suis_je() << finl;
}
