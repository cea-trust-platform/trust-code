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

#include <Dirichlet_paroi_fixe.h>

Implemente_instanciable(Dirichlet_paroi_fixe, "Paroi_fixe", Dirichlet_homogene);
// XD paroi_fixe condlim_base paroi_fixe -1 Keyword to designate a situation of adherence to the wall called bord (edge) (normal and tangential velocity at the edge is zero).


Sortie& Dirichlet_paroi_fixe::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Dirichlet_paroi_fixe::readOn(Entree& s)
{
  if (app_domains.size() == 0)
    app_domains = {Motcle("Hydraulique"), Motcle("Thermique"),
                   Motcle("Transport_Keps_V2"), Motcle("Transport_Keps_Bas_Re"),
                   Motcle("Transport_Keps_Rea"), Motcle("Transport_Keps"),
                   Motcle("Turbulence"), Motcle("indetermine"),
                   Motcle("Transport_Komega")
                  };

  Dirichlet_homogene::readOn(s);
  int nb_comp = Objet_U::dimension;
  le_champ_front->fixer_nb_comp(nb_comp);
  return s;
}
