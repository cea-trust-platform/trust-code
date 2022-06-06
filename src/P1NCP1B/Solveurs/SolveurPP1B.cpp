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

#include <SolveurPP1B.h>
#include <Champ_P1_isoP1Bulle.h>
#include <stat_counters.h>

Implemente_instanciable(SolveurPP1B,"SolveurPP1B",SolveurSys_base);

// printOn
Sortie& SolveurPP1B::printOn(Sortie& s ) const
{
  return s  << que_suis_je();
}


// readOn
Entree& SolveurPP1B::readOn(Entree& is )
{
  return is;
}

int SolveurPP1B::resoudre_systeme(const Matrice_Base& A,
                                  const DoubleVect& second_membre,
                                  DoubleVect& x)
{
  // Stop the solv_sys_counter_ counter to not count the changing base
  statistiques().end_count(solv_sys_counter_,0,-1);
  DoubleVect b(second_membre);
  assembleur_pression_.changer_base_second_membre(b);
  assembleur_pression_.changer_base_pression(x);
  int nb_iter=solveur_pression_.resoudre_systeme(A,b,x);
  assembleur_pression_.changer_base_pression_inverse(x);
  statistiques().begin_count(solv_sys_counter_);
  return nb_iter;
}



int SolveurPP1B::resoudre_systeme(const Matrice_Base& A,
                                  const DoubleVect& b,
                                  DoubleVect& x,

                                  int niter_max)
{
  return SolveurPP1B::resoudre_systeme(A,b,x);
}


// Affectation de l'assembleur et du solveur en pression
int SolveurPP1B::associer(const Assembleur_P_VEFPreP1B& assembleur_pression, const SolveurSys& solveur_pression)
{
  assembleur_pression_=assembleur_pression;
  solveur_pression_=solveur_pression;
  fixer_limpr(-1); // Pas d'impression
  return 0;
}
