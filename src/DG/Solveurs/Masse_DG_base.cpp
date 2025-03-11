/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Schema_Euler_Implicite.h>
#include <Op_Diff_negligeable.h>
#include <Masse_DG_base.h>
#include <Domaine_Cl_DG.h>
#include <Domaine_DG.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Operateur.h>

Implemente_base(Masse_DG_base, "Masse_DG_base", Solveur_Masse_base);
Sortie& Masse_DG_base::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }
Entree& Masse_DG_base::readOn(Entree& s) { return s; }

void Masse_DG_base::associer_domaine_dis_base(const Domaine_dis_base& le_dom_dis_base)
{
  le_dom_dg_ = ref_cast(Domaine_DG, le_dom_dis_base);
}

void Masse_DG_base::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
{
  le_dom_Cl_dg_ = ref_cast(Domaine_Cl_DG, le_dom_Cl_dis_base);
}
