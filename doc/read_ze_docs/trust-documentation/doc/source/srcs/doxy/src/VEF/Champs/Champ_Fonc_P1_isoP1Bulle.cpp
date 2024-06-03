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

#include <Champ_Fonc_P1_isoP1Bulle.h>
#include <Domaine_VEF.h>

Implemente_instanciable(Champ_Fonc_P1_isoP1Bulle,"Champ_Fonc_P1_isoP1Bulle",Champ_Fonc_base);

Sortie& Champ_Fonc_P1_isoP1Bulle::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_P1_isoP1Bulle::readOn(Entree& s) { return s ; }

int Champ_Fonc_P1_isoP1Bulle::fixer_nb_valeurs_nodales(int n)
{
  assert(n < 0);
  const Domaine_VEF& zvef = domaine_vef();
  const MD_Vector& md = zvef.md_vector_p1b();
  creer_tableau_distribue(md);
  return n;
}

void Champ_Fonc_P1_isoP1Bulle::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

DoubleTab& Champ_Fonc_P1_isoP1Bulle::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  return Champ_P1iP1B_implementation::trace(fr, valeurs(), x,distant);
}

void Champ_Fonc_P1_isoP1Bulle::completer(const Domaine_Cl_dis_base& zcl)
{
  Champ_P1iP1B_implementation::completer(zcl);
}
