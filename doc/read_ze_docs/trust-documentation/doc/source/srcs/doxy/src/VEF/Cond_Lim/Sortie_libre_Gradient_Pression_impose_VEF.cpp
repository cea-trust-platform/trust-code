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

#include <Sortie_libre_Gradient_Pression_impose_VEF.h>
#include <Discretisation_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_P0_VEF.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_impose_VEF, "Frontiere_ouverte_Gradient_Pression_impose_VEF", Neumann_sortie_libre);

Sortie& Sortie_libre_Gradient_Pression_impose_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << finl;
}

Entree& Sortie_libre_Gradient_Pression_impose_VEF::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };

  s >> le_champ_front;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1, dimension);
  return s;
}

double Sortie_libre_Gradient_Pression_impose_VEF::flux_impose(int face) const
{
  if (le_champ_front.valeurs().size() == 1)
    {
      double a1 = trace_pression_int[face];
      double a2 = coeff[face];
      double a3 = le_champ_front(0, 0);
      double Pimp = a1 + a2 * a3;
      return Pimp;
    }
  else if (le_champ_front.valeurs().line_size() == 1)
    {
      return (trace_pression_int[face] + coeff[face] * le_champ_front(face, 0));
    }
  else
    Cerr << "Sortie_libre_Gradient_Pression_impose_VEF::flux_impose erreur" << finl;
  exit();
  return 0.;
}

double Sortie_libre_Gradient_Pression_impose_VEF::flux_impose(int face, int ncomp) const
{
  if (ncomp == 0) return flux_impose(face);

  Cerr << "Sortie_libre_Gradient_Pression_impose_VEF::flux_impose(int  , int )" << finl;
  Cerr << "On ne sait imposer que la composante normale du gradient" << finl;
  Process::exit();
  return 0.;
}
