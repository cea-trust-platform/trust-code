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

#include <grad_Champ_Face_PolyVEF.h>
#include <Frottement_impose_base.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_Cl_dis_base.h>
#include <Dirichlet.h>
#include <EChaine.h>
#include <Equation_base.h>
#include <Milieu_base.h>

Implemente_instanciable(grad_Champ_Face_PolyVEF, "grad_Champ_Face_PolyVEF", Champ_Fonc_Elem_PolyMAC);

Sortie& grad_Champ_Face_PolyVEF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& grad_Champ_Face_PolyVEF::readOn(Entree& s) { return s; }

void grad_Champ_Face_PolyVEF::mettre_a_jour(double tps)
{
  if (temps() != tps)
    me_calculer();
  Champ_Fonc_base::mettre_a_jour(tps);
}

void grad_Champ_Face_PolyVEF::me_calculer()
{
  const Domaine_PolyVEF& domaine = ref_cast(Domaine_PolyVEF, domaine_vf());
  const DoubleTab& n_f = domaine.face_normales();
  const DoubleVect& ve = domaine.volumes(), &pf = champ_a_deriver().equation().milieu().porosite_face();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const int D = dimension, N = valeurs().line_size()/(D*D), ne = domaine.nb_elem();
  int e, f, i, c, n, d_U, d_X;

  valeurs() = 0;

  for (e=0; e<ne ; e++)
    for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0 && (c = e == f_e(f,0) ? 0 : 1) > -1; i++)
      for (n = 0 ; n<N ; n++)
        for (d_U = 0; d_U < D; d_U++)
          for (d_X = 0; d_X < D; d_X++)
            valeurs()(e, N * ( D*d_U+d_X ) + n) += 1/ve(e) * (c == 0 ? 1 : -1) * n_f(f, d_X) * pf(f) * champ_a_deriver().valeurs()(f, N*d_U + n);

  valeurs().echange_espace_virtuel();
}
