/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Pb_Hydraulique_IBM.h>
#include <Fluide_Ostwald.h>

Implemente_instanciable(Pb_Hydraulique_IBM,"Pb_Hydraulique_IBM",Pb_Fluide_base);
// XD pb_hydraulique_ibm Pb_base pb_hydraulique_ibm -1 Resolution of the IBM Navier-Stokes equations.
// XD   attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD   attr navier_stokes_ibm navier_stokes_ibm navier_stokes_ibm 0 IBM Navier-Stokes equations.

Sortie& Pb_Hydraulique_IBM::printOn(Sortie& os) const { return Pb_Fluide_base::printOn(os); }
Entree& Pb_Hydraulique_IBM::readOn(Entree& is) { return Pb_Fluide_base::readOn(is); }

const Equation_base& Pb_Hydraulique_IBM::equation(int i) const
{
  if (i == 0) return eq_hydraulique_;
  else if (i < 1 + eq_opt_.size() && i > 0) return eq_opt_[i - 1].valeur();
  else
    {
      Cerr << "Pb_Hydraulique::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_hydraulique_;
}

Equation_base& Pb_Hydraulique_IBM::equation(int i)
{
  if (i == 0) return eq_hydraulique_;
  else if (i < 1 + eq_opt_.size() && i > 0) return eq_opt_[i - 1].valeur();
  else
    {
      Cerr << "Pb_Hydraulique::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_hydraulique_;
}

void Pb_Hydraulique_IBM::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Fluide_base,mil) && ref_cast(Fluide_base, mil).is_incompressible())
    eq_hydraulique_.associer_milieu_base(mil);
  else if (sub_type(Fluide_Ostwald,mil))
    {
      Cerr << "Je suis non newto" << finl;
      eq_hydraulique_.associer_milieu_base(mil);
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a "
           << finl;
      Cerr << "un probleme de type Pb_Hydraulique " << finl;
      exit();
    }
}
