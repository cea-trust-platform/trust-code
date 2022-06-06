/****************************************************************************
* Copyright (c) 2020, CEA
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

#include <SolveurSys.h>
#include <stat_counters.h>
#include <Motcle.h>
#include <Param.h>

Implemente_deriv(SolveurSys_base);
Implemente_instanciable(SolveurSys,"SolveurSys",DERIV(SolveurSys_base));

Sortie& SolveurSys::printOn(Sortie& s ) const
{
  return DERIV(SolveurSys_base)::printOn(s);
}

Entree& SolveurSys::readOn(Entree& is )
{
  Param param(que_suis_je());
  Nom solver_name;
  param.ajouter("solveur_pression",&solver_name,Param::REQUIRED);
  param.lire_sans_accolade(is);

  Nom type_solv_sys("Solv_");
  type_solv_sys+=solver_name;
  typer(type_solv_sys);
  return is >> valeur();
}

static int nested_solver = 0;


int SolveurSys::resoudre_systeme(const Matrice_Base& matrice,
                                 const DoubleVect& secmem,
                                 DoubleVect& solution)
{
  valeur().save_matrice_secmem_conditionnel(matrice, secmem, solution);

  // Cas de solveurs emboites: n'afficher que le temps du solveur "exterieur"
  // temporaire : test issu du baltik IJK_FT en commentaire car sinon erreur dans .TU avec PETSC (solveurs Ax=B => 0%)
  //if (nested_solver == 0)
  statistiques().begin_count(solv_sys_counter_);
  nested_solver++;

  int nb_iter = valeur().resoudre_systeme(matrice,secmem,solution);

  nested_solver--;
  // temporaire : test issu du baltik IJK_FT en commentaire car sinon erreur avec script Check_solver.sh pour test PETSC_VEF
  //if (nested_solver == 0)
  //  {
  statistiques().end_count(solv_sys_counter_, nb_iter);
  // Si limpr vaut -1, on n'imprime pas
  if (valeur().limpr()>=0)
    {
      Cout << " Convergence in " << nb_iter << " iterations for " << le_nom() << finl;
    }
  if (valeur().limpr()==1)
    {
      Cout << "clock Ax=B: " << statistiques().last_time(solv_sys_counter_) << " s for " << le_nom() << finl;
    }

//  }
  return nb_iter;
}




int SolveurSys::resoudre_systeme(const Matrice_Base& matrice,
                                 const DoubleVect& secmem,
                                 DoubleVect& solution,
                                 int niter_max)
{
  return valeur().resoudre_systeme(matrice,secmem,solution,niter_max);
}


