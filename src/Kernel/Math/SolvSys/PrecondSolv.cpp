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
// File:        PrecondSolv.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <PrecondSolv.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc_Sym.h>
#include <Check_espace_virtuel.h>

Implemente_instanciable(PrecondSolv,"PrecondSolv",Precond_base);
//
// printOn et readOn

Sortie& PrecondSolv::printOn(Sortie& s ) const
{
  return s;
}

Entree& PrecondSolv::readOn(Entree& is )
{
  is >> solveur;
  solveur.nommer("PrecondSolv");
  // Pour eviter trop d'affichage (Convergence)
  if (!solveur.valeur().limpr())
    solveur.valeur().fixer_limpr(-1);
  return is;
}

void PrecondSolv::prepare_(const Matrice_Base& m, const DoubleVect& src)
{
  solveur.valeur().reinit();
  Precond_base::prepare_(m, src);
}

// Description:
//    Calcule la solution du systeme lineaire: A * solution = b
//    avec la methode de relaxation PrecondSolv.
int PrecondSolv::preconditionner_(const Matrice_Base& matrice,
                                  const DoubleVect& b,
                                  DoubleVect& solution)
{
  double norme=mp_norme_vect(b);
  if (norme > 0.)
    {
      DoubleVect b2(b);
      // Certains solveurs peuvent avoir besoin de l'espace virtuel,
      // on calcule tout le vecteur:
      assert_espace_virtuel_vect(b2);
      operator_multiply(b2, 1. / norme, VECT_ALL_ITEMS);

      solveur.resoudre_systeme(matrice, b2, solution);

      if (echange_ev_solution_)
        {
          // On veut renvoyer un vecteur avec espace virtuel a jour
          // Plutot que de refaire un echange on multiplie tous les elements
          // mais il faut que l'espace virtuel soit deja a jour avant:
          assert_espace_virtuel_vect(solution);
          operator_multiply(solution, norme, VECT_ALL_ITEMS);
        }
      else
        {
          operator_multiply(solution, norme);
        }
    }
  else
    {
      if (echange_ev_solution_)
        {
          operator_egal(solution, 0., VECT_ALL_ITEMS);
        }
      else
        {
          operator_egal(solution, 0.);
        }
    }
  return 1;
}

