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

#include <Modele_turbulence_scal.h>
#include <Discretisation_base.h>
#include <Equation_base.h>

Implemente_instanciable(Modele_turbulence_scal, "Modele_turbulence_scal", DERIV(Modele_turbulence_scal_base));

Sortie& Modele_turbulence_scal::printOn(Sortie& s) const
{
  return s << valeur().que_suis_je() << finl;
}

Entree& Modele_turbulence_scal::readOn(Entree& s)
{
  Motcle typ;
  s >> typ;
  Motcle nom1("Modele_turbulence_scal_");
  nom1 += typ;
  if (typ == "sous_maille_dyn")
    {
      nom1 += "_";
      Nom disc = equation().discretisation().que_suis_je();
      if (disc == "VEFPreP1B")
        disc = "VEF";
      nom1 += disc;
    }
  Cerr << nom1 << finl;
  DERIV(Modele_turbulence_scal_base)::typer(nom1);
  valeur().associer_eqn(equation());
  valeur().associer(equation().domaine_dis(), equation().domaine_Cl_dis());
  s >> valeur();
  return s;
}
