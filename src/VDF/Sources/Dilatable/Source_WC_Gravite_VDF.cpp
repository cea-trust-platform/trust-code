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

#include <Fluide_Weakly_Compressible.h>
#include <Source_WC_Gravite_VDF.h>

Implemente_instanciable(Source_WC_Gravite_VDF,"Source_WC_Gravite_VDF",Source_Gravite_Fluide_Dilatable_base);

Sortie& Source_WC_Gravite_VDF::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Source_WC_Gravite_VDF::readOn(Entree& is) { return is; }

void Source_WC_Gravite_VDF::associer_domaines(const Domaine_dis_base& domaine,const Domaine_Cl_dis_base& domaine_cl)
{
  associer_domaines_impl(domaine,domaine_cl);
}

void Source_WC_Gravite_VDF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const DoubleTab& tab_rho = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur()).rho_discvit();
  ajouter_impl(g,0.,tab_rho,secmem);
}

