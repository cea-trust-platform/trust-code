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

#include <Force_Tchen_PolyMAC_P0.h>
#include <Champ_Face_base.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Force_Tchen_PolyMAC_P0, "Force_Tchen_Face_PolyMAC_P0|Tchen_force_Face_PolyMAC_P0", Source_Force_Tchen_base);

Sortie& Force_Tchen_PolyMAC_P0::printOn(Sortie& os) const { return os; }
Entree& Force_Tchen_PolyMAC_P0::readOn(Entree& is) { return Source_Force_Tchen_base::readOn(is); }

void Force_Tchen_PolyMAC_P0::dimensionner_blocs_aux(IntTrav& stencil) const
{
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& inco = ref_cast(Champ_Face_base, equation().inconnue()).valeurs();
  int N = inco.line_size(), D = dimension, nf_tot = domaine.nb_faces_tot();

  /* elements */
  for (int e = 0; e < domaine.nb_elem_tot(); e++)
    for (int d = 0 ; d <D ; d++)
      for (int k = 0 ; k<N ; k++)
        if (k != n_l) //phase gazeuse
          stencil.append_line(  N *(nf_tot + D * e + d) + k  , N *(nf_tot + D * e + d) + n_l) ;
}

void Force_Tchen_PolyMAC_P0::ajouter_blocs_aux(matrices_t matrices, DoubleTab& secmem) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe(),
                   &rho   = equation().milieu().masse_volumique()->passe();

  double pas_tps = equation().probleme().schema_temps().pas_de_temps();
  int N = inco.line_size(), D = dimension, nf_tot = domaine.nb_faces_tot();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();

  /* elements */
  for (int e = 0; e < domaine.nb_elem_tot(); e++)
    for (int d = 0 ; d <D ; d++)
      for (int k = 0 ; k<N ; k++)
        if (k != n_l) //phase gazeuse
          {
            double fac = pe(e) * ve(e) * alpha(e, k) * rho(e, n_l) ;

            secmem(nf_tot + D * e + d, k ) += fac * (inco(nf_tot + D * e + d, n_l)-pvit(nf_tot + D * e + d, n_l))/pas_tps;
            secmem(nf_tot + D * e + d,n_l) -= fac * (inco(nf_tot + D * e + d, n_l)-pvit(nf_tot + D * e + d, n_l))/pas_tps;
            if (mat)
              {
                (*mat)( N *(nf_tot + D * e + d) + k  , N *(nf_tot + D * e + d) + n_l) -= fac/pas_tps ;
                (*mat)( N *(nf_tot + D * e + d) + n_l, N *(nf_tot + D * e + d) + n_l) += fac/pas_tps ;
              }
          }
}
