/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Op_Evanescence_Homogene_VDF_Face.h>
#include <Vitesse_relative_base.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Op_Evanescence_Homogene_VDF_Face, "Op_Evanescence_HOMOGENE_VDF_Face", Op_Evanescence_Homogene_Face_base);

Sortie& Op_Evanescence_Homogene_VDF_Face::printOn(Sortie& os) const { return os; }
Entree& Op_Evanescence_Homogene_VDF_Face::readOn(Entree& is) { return Op_Evanescence_Homogene_Face_base::readOn(is); }

void Op_Evanescence_Homogene_VDF_Face::preparer_calcul()
{
  Op_Evanescence_Homogene_Face_base::preparer_calcul();

  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Vitesse_relative_base* correlation_vd = pbm.has_correlation("vitesse_relative") ? &ref_cast(Vitesse_relative_base, pbm.get_correlation("vitesse_relative")) : nullptr;

  if (correlation_vd && correlation_vd->needs_grad_alpha())
    {
      Cerr << "Typing gradient operator useful for the correlation " << correlation_vd->que_suis_je() << finl;
      const Equation_base& eq_masse = pbm.equation_masse();
      grad_vdf_faces_.associer_eqn(eq_masse);
      grad_vdf_faces_.typer();
      grad_vdf_faces_.l_op_base().associer_eqn(eq_masse);
      const Domaine_dis_base& zdis = eq_masse.domaine_dis();
      const Domaine_Cl_dis_base& zcl = eq_masse.domaine_Cl_dis();
      const Champ_Inc_base& inco = eq_masse.inconnue();
      grad_vdf_faces_->associer(zdis, zcl, inco);
    }
}

void Op_Evanescence_Homogene_VDF_Face::calc_grad_alpha_faces(DoubleTab& gradAlphaFaces) const
{
  assert (grad_vdf_faces_.non_nul());

  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const DoubleTab& alpha = pbm.equation_masse().inconnue().passe();
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();
  const DoubleTab& n_f = domaine.face_normales(), &vf_dir = domaine.volumes_entrelaces_dir();
  const DoubleVect& vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const int N = alpha.line_size(), nf_tot = domaine.nb_faces_tot(), D = dimension, ne_tot = domaine.nb_elem_tot(), nf = domaine.nb_faces();

  /* calculaiton of the gradient of alpha at the face */
  DoubleTrav grad_f_a(nf_tot, N);
  grad_vdf_faces_->calculer(alpha, grad_f_a);

  DoubleTrav gradAlphaElem(ne_tot, D, N);
  /* Calcul du grad aux elems */
  for (int n = 0; n < N; n++)
    for (int e = 0; e < ne_tot; e++)
      for (int d = 0; d < D; d++)
        for (int j = 0 ; j < e_f.dimension(1); j++)
          {
            const int f = e_f(e, j);

            if (f < 0 ) continue;

            gradAlphaElem(e, d, n) += (e == f_e(f, 0) ? 1 : -1) * fs(f) * (xv(f, d) - xp(e, d)) / ve(e) * grad_f_a(f, n);
          }

  /* Calcul du grad vectoriel aux faces */
  double scalGradElem = 0.;
  for (int n = 0; n < N; n++)
    for (int f = 0; f < nf; f++)
      {
        for (int c = 0; c < 2; c++)
          for (int d = 0; d < D; d++)
            {
              const int e = f_e(f, c);

              if (e < 0) continue;

              gradAlphaFaces(f, d, n) += vf_dir(f, c) / vf(f) * gradAlphaElem(e, d, n);
            }

        scalGradElem = 0.;
        for (int d = 0; d < D; d++)
          scalGradElem += gradAlphaFaces(f, d, n) * n_f(f, d) / fs(f);

        for (int d = 0; d < D; d++)
          gradAlphaFaces(f, d, n) += (grad_f_a(f, n) - scalGradElem) * n_f(f, d) / fs(f);
      }
}
