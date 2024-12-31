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

#include <Champ_Fonc_Elem_PolyVEF_rot.h>
#include <grad_Champ_Face_PolyVEF.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Navier_Stokes_std.h>

Implemente_instanciable(Champ_Fonc_Elem_PolyVEF_rot, "Champ_Fonc_Elem_PolyVEF_rot", Champ_Fonc_Elem_PolyMAC_P0_rot);

Sortie& Champ_Fonc_Elem_PolyVEF_rot::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_Elem_PolyVEF_rot::readOn(Entree& s) { return s; }

void Champ_Fonc_Elem_PolyVEF_rot::me_calculer_2D()
{
  const Champ_Face_PolyVEF& vit = ref_cast(Champ_Face_PolyVEF, champ_a_deriver());
  const Domaine_PolyVEF& domaine = ref_cast(Domaine_PolyVEF, vit.domaine_vf());
  int e, n;
  int D = dimension, N = champ_a_deriver().valeurs().line_size() / D, ne = domaine.nb_elem();

  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, vit.equation());
  DoubleTab& tab_rot = valeurs();
  const grad_Champ_Face_PolyVEF& grad = ref_cast(grad_Champ_Face_PolyVEF, eq.get_champ("gradient_vitesse"));
  const DoubleTab& tab_grad = grad.passe();

  for (n = 0; n < N; n++)
    for (e = 0; e < ne; e++)
      tab_rot(e, n) = tab_grad(e, N * (D * 1 + 0) + n) - tab_grad(e, N * (D * 0 + 1) + n); // dUy/dx - dUx/dy

  tab_rot.echange_espace_virtuel();
}

void Champ_Fonc_Elem_PolyVEF_rot::me_calculer_3D()
{
  const Champ_Face_PolyVEF& vit = ref_cast(Champ_Face_PolyVEF, champ_a_deriver());
  const Domaine_PolyVEF& domaine = ref_cast(Domaine_PolyVEF, vit.domaine_vf());
  int e, n;
  int D = dimension, N = champ_a_deriver().valeurs().line_size() / D, ne = domaine.nb_elem();

  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, vit.equation());
  DoubleTab& tab_rot = valeurs();
  const grad_Champ_Face_PolyVEF& grad = ref_cast(grad_Champ_Face_PolyVEF, eq.get_champ("gradient_vitesse"));
  const DoubleTab& tab_grad = grad.passe();

  for (n = 0; n < N; n++)
    for (e = 0; e < ne; e++)
      {
        tab_rot(e, N * 0 + n) = tab_grad(e, N * (D * 2 + 1) + n) - tab_grad(e, N * (D * 1 + 2) + n); // dUz/dy - dUy/dz
        tab_rot(e, N * 1 + n) = tab_grad(e, N * (D * 0 + 2) + n) - tab_grad(e, N * (D * 2 + 0) + n); // dUx/dz - dUz/dx
        tab_rot(e, N * 2 + n) = tab_grad(e, N * (D * 1 + 0) + n) - tab_grad(e, N * (D * 0 + 1) + n); // dUy/dx - dUx/dy
      }

  tab_rot.echange_espace_virtuel();
}
