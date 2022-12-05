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

#include <Flux_interfacial_PolyMAC.h>
#include <Champ_Inc_P0_base.h>
#include <Zone_PolyMAC_P0.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Zone_VF.h>
#include <Domaine.h>

Implemente_instanciable(Flux_interfacial_PolyMAC,"Flux_interfacial_Elem_PolyMAC|Flux_interfacial_Elem_PolyMAC_P0", Source_Flux_interfacial_base);

Sortie& Flux_interfacial_PolyMAC::printOn(Sortie& os) const { return os; }
Entree& Flux_interfacial_PolyMAC::readOn(Entree& is) { return Source_Flux_interfacial_base::readOn(is); }

void Flux_interfacial_PolyMAC::fill_vit_elem_tab(DoubleTab& pvit_elem) const
{
  const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
  const Champ_Inc_base& ch_vit = ref_cast(Pb_Multiphase, equation().probleme()).eq_qdm.inconnue().valeur();
  const int N = ref_cast(Champ_Inc_P0_base, equation().inconnue().valeur()).valeurs().line_size(), D = dimension, nf_tot = zone.nb_faces_tot();

  if (sub_type(Zone_PolyMAC_P0,zone))
    {
      for (int e = 0; e < zone.nb_elem_tot(); e++)
        for (int n = 0; n < N; n++)
          for (int d = 0; d < D; d++)
            pvit_elem(e, N * d + n) = ch_vit.passe()(nf_tot + D * e + d, n);
    }
  else
    {
      const IntTab& f_s = zone.face_sommets(), &e_f = zone.elem_faces();
      const Domaine& dom = zone.zone().domaine();
      pvit_elem = 0.0;
      for (int e = 0; e < zone.nb_elem_tot(); e++)
        for (int n = 0; n < N; n++)
          for (int d = 0; d < D; d++)
            {
              const double val1 = ch_vit.passe()(e_f(e, d), n), val2 = ch_vit.passe()(e_f(e, d + D), n);
              const int som0 = f_s(e_f(e, d), 0), som1 = f_s(e_f(e, d + D), 0);
              const double psi = (zone.xp(e, d) - dom.coord(som0, d)) / (dom.coord(som1, d) - dom.coord(som0, d));

              pvit_elem(e, N * d + n) = (std::fabs(psi) < 1e-12) ? val1 : (std::fabs(1. - psi) < 1e-12 ? val2 : val1 + psi * (val2-val1));
            }
    }

}
