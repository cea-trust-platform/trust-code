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

#include <Paroi_Temperature_imposee.h>
#include <Discretisation_base.h>
#include <Equation_base.h>

Implemente_instanciable(Paroi_Temperature_imposee, "Paroi_Temperature_imposee", Cond_lim_utilisateur_base);
// XD paroi_temperature_imposee dirichlet paroi_temperature_imposee 0 Imposed temperature condition at the wall called bord (edge).
// XD attr ch front_field_base ch 0 Boundary field type.

Sortie& Paroi_Temperature_imposee::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Paroi_Temperature_imposee::readOn(Entree& s ) { return s; }

void Paroi_Temperature_imposee::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  Nom Nrayo;
  if (rayo == 1)
    Nrayo = "_rayo_semi_transp";
  if (rayo == 2)
    Nrayo = "_rayo_transp";
  if (mon_equation->discretisation().is_vdf())
    {
      ajout = "paroi_echange_externe_impose";
      if (rayo)
        ajout += Nrayo;
      Nom cl(" h_imp champ_front_uniforme ");
      const int N = mon_equation->inconnue().valeurs().dimension(1);
      cl += N;
      for (int n = 0; n < N; n++)
        cl += " 1e10";
      cl += " t_ext";
      ajout += cl;
    }
  else
    {
      ajout = "temperature_imposee_paroi";
      if (rayo)
        {
          ajout = "paroi_temperature_imposee";
          ajout += Nrayo;
        }

    }
}
