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

#include <Dirichlet_paroi_fixe_iso_Genepi2.h>

Implemente_instanciable( Dirichlet_paroi_fixe_iso_Genepi2, "paroi_fixe_iso_Genepi2_sans_contribution_aux_vitesses_sommets", Dirichlet_paroi_fixe ) ;

// XD paroi_fixe_iso_Genepi2_sans_contribution_aux_vitesses_sommets paroi_fixe paroi_fixe_iso_Genepi2_sans_contribution_aux_vitesses_sommets -1 Boundary condition to obtain iso Geneppi2, without interest

Sortie& Dirichlet_paroi_fixe_iso_Genepi2::printOn( Sortie& os ) const
{
  Dirichlet_paroi_fixe::printOn( os );
  return os;
}

Entree& Dirichlet_paroi_fixe_iso_Genepi2::readOn( Entree& is )
{
  Dirichlet_paroi_fixe::readOn( is );
  return is;
}
