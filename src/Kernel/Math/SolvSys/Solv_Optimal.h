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

#ifndef Solv_Optimal_included
#define Solv_Optimal_included

#include <Interprete.h>
#include <solv_iteratif.h>
#include <SolveurSys.h>

class Solv_Optimal: public solv_iteratif
{
  Declare_instanciable(Solv_Optimal);
public :
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect&) override;
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect& , int) override;
  void reinit() override;
  void prepare_resol(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution, int nmax=100);

protected :
  SolveurSys le_solveur_;
  int n_resol_,n_reinit_,freq_recalc_,fichier_solveur_non_recree_;
  Nom fichier_solveur_;
};

class Test_solveur : public Interprete
{
  Declare_instanciable(Test_solveur);

public:
  Entree& interpreter(Entree&) override;
};

#endif
