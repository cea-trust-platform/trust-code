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

#include <Schema_Implicite_base.h>
#include <Solveur_Lineaire_Std.h>
#include <SolveurSys_base.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <Motcle.h>
#include <Nom.h>
#include <Param.h>

Implemente_instanciable(Solveur_Lineaire_Std,"Solveur_lineaire_std",Solveur_lineaire);

Sortie& Solveur_Lineaire_Std::printOn(Sortie& os ) const
{
  Cerr << " The implicit solver used is of type Solveur_Lineaire_Std " << finl;
  return Solveur_lineaire::printOn(os);
}

Entree& Solveur_Lineaire_Std::readOn(Entree& is )
{
  Cerr << " The implicit solver used is of type Solveur_Lineaire_Std " << finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Solveur_Lineaire_Std::set_param(Param& param)
{
  param.ajouter_non_std("solveur",(this),Param::REQUIRED);
}

int Solveur_Lineaire_Std::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  int retval = 1;
  if (mot=="solveur")
    {
      Nom nom_solveur("Solv_");
      Nom type_solv_sys;
      is >> type_solv_sys;
      nom_solveur+=type_solv_sys;
      Cerr << "nom_solveur " << nom_solveur << finl;
      solveur.typer(nom_solveur);
      is >> solveur.valeur();
      solveur.nommer("solveur_implicite");
    }
  else retval = -1;

  return retval;
}

bool Solveur_Lineaire_Std::iterer_eqn(Equation_base& equation, const DoubleTab& inconnue, DoubleTab& result, double dt, int numero_iteration, int& ok)
{
  assert( numero_iteration == 1 );
  Matrice_Morse matrix;
  DoubleTrav    rhs(result);
  equation.dimensionner_matrice(matrix);
  equation.assembler_avec_inertie(matrix, inconnue, rhs);
  solveur.valeur().reinit();
  solveur.resoudre_systeme(matrix, rhs, result);
  return true;
}
