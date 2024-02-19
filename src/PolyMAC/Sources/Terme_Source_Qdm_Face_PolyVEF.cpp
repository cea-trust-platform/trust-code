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

#include <Terme_Source_Qdm_Face_PolyVEF.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_PolyVEF.h>
#include <Pb_Multiphase.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Synonyme_info.h>

Implemente_instanciable(Terme_Source_Qdm_Face_PolyVEF, "Source_Qdm_Face_PolyVEF_P0|Source_Qdm_Face_PolyVEF_P0P1", Terme_Source_Qdm_Face_PolyMAC_P0P1NC);
Add_synonym(Terme_Source_Qdm_Face_PolyVEF, "Source_Qdm_Face_PolyVEF_P0P1NC");

Sortie& Terme_Source_Qdm_Face_PolyVEF::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Terme_Source_Qdm_Face_PolyVEF::readOn(Entree& s) { return Terme_Source_Qdm_Face_PolyMAC_P0P1NC::readOn(s);; }

void Terme_Source_Qdm_Face_PolyVEF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& dom = ref_cast(Domaine_Poly_base, equation().domaine_dis());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const DoubleTab& vals = la_source->valeurs(), &vfd = dom.volumes_entrelaces_dir(), &rho = equation().milieu().masse_volumique().passe(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const DoubleVect& pf = equation().milieu().porosite_face();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl();
  int e, f, i, cS = (vals.dimension_tot(0) == 1), cR = (rho.dimension_tot(0) == 1), p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom),
               d, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D;

  /* contributions aux faces (par chaque voisin), aux elems */
  DoubleTrav a_f(N), rho_f(N), val_f(N), rho_m(2);

  for (a_f = 1, f = 0; f < dom.nb_faces(); f++)
    if (!p0p1 || fcl(f, 0) < 2)
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (e = f_e(f, i), n = 0; n < N; n++)
          for (d = 0; d < D; d++)
            secmem(f, N * d + n) += vfd(f, i) * pf(f) * (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * vals(!cS * e, N * d + n);
}
