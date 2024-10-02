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

#include <Masse_PolyMAC_P0P1NC_Elem.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Synonyme_info.h>
#include <Equation_base.h>

Implemente_instanciable(Masse_PolyMAC_P0P1NC_Elem, "Masse_PolyMAC_P0P1NC_Elem|Masse_PolyMAC_P0_Elem", Masse_PolyMAC_P0P1NC_base);
Add_synonym(Masse_PolyMAC_P0P1NC_Elem, "Masse_PolyVEF_P0_Elem");
Add_synonym(Masse_PolyMAC_P0P1NC_Elem, "Masse_PolyVEF_P0P1_Elem");
Add_synonym(Masse_PolyMAC_P0P1NC_Elem, "Masse_PolyVEF_P0P1NC_Elem");


Sortie& Masse_PolyMAC_P0P1NC_Elem::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_PolyMAC_P0P1NC_Elem::readOn(Entree& s) { return s ; }

void Masse_PolyMAC_P0P1NC_Elem::preparer_calcul()
{
  associer_masse_proto(*this, le_dom_PolyMAC.valeur());
  preparer_calcul_proto();
}

DoubleTab& Masse_PolyMAC_P0P1NC_Elem::appliquer_impl(DoubleTab& sm) const
{
  return appliquer_impl_proto(sm);
}

void Masse_PolyMAC_P0P1NC_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  dimensionner_blocs_proto(matrices,semi_impl);
}

void Masse_PolyMAC_P0P1NC_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  ajouter_blocs_proto(matrices, secmem, dt, semi_impl, resoudre_en_increments);
}
