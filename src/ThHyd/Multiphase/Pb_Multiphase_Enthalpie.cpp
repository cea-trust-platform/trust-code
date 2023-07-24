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

#include <Pb_Multiphase_Enthalpie.h>

Implemente_instanciable(Pb_Multiphase_Enthalpie, "Pb_Multiphase_Enthalpie", Pb_Multiphase);

Sortie& Pb_Multiphase_Enthalpie::printOn(Sortie& os) const { return Pb_Multiphase::printOn(os); }

Entree& Pb_Multiphase_Enthalpie::readOn(Entree& is)
{
  resolution_en_T_ = false; // resolution en h
  return Pb_Multiphase::readOn(is);
}

const Equation_base& Pb_Multiphase_Enthalpie::equation(int i) const
{
  if      (i == 0) return eq_qdm;
  else if (i == 1) return eq_masse;
  else if (i == 2) return eq_energie_enthalpie;
  else if (i < 3 + eq_opt.size()) return eq_opt[i - 3].valeur();
  else
    {
      Cerr << "Pb_Multiphase_Enthalpie::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_qdm; //pour renvoyer quelque chose
}

Equation_base& Pb_Multiphase_Enthalpie::equation(int i)
{
  if      (i == 0) return eq_qdm;
  else if (i == 1) return eq_masse;
  else if (i == 2) return eq_energie_enthalpie;
  else if (i < 3 + eq_opt.size()) return eq_opt[i - 3].valeur();
  else
    {
      Cerr << "Pb_Multiphase_Enthalpie::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_qdm; //pour renvoyer quelque chose
}

void Pb_Multiphase_Enthalpie::associer_milieu_base(const Milieu_base& mil)
{
  /* controler le type de milieu ici */
  eq_qdm.associer_milieu_base(mil);
  eq_energie_enthalpie.associer_milieu_base(mil);
  eq_masse.associer_milieu_base(mil);
}

int Pb_Multiphase_Enthalpie::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_qdm.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_energie_enthalpie.domaine_Cl_dis();
  return tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
}
