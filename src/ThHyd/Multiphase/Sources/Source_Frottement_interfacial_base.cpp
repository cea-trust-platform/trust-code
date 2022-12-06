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

#include <Source_Frottement_interfacial_base.h>
#include <Champ_Face_base.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Zone_VF.h>

Implemente_base(Source_Frottement_interfacial_base, "Source_Frottement_interfacial_base", Sources_Multiphase_base);

Sortie& Source_Frottement_interfacial_base::printOn(Sortie& os) const { return os; }

Entree& Source_Frottement_interfacial_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("a_res", &a_res_);
  param.ajouter("dv_min", &dv_min);
  param.ajouter("exp_res", &exp_res);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  if (pbm.has_correlation("frottement_interfacial")) correlation_ = pbm.get_correlation("frottement_interfacial"); //correlation fournie par le bloc correlation
  else correlation_.typer_lire(pbm, "frottement_interfacial", is); //sinon -> on la lit
  return is;
}

void Source_Frottement_interfacial_base::completer()
{
  if (a_res_ < 1.e-12)
    {
      a_res_ = ref_cast(QDM_Multiphase, equation()).alpha_res();
      a_res_ = std::max(1.e-4, a_res_*100.);
    }
}

void Source_Frottement_interfacial_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  int f, k, l, N = inco.line_size();
  /* faces */
  for (f = 0; f < zone.nb_faces(); f++)
    if (fcl(f, 0) < 2)
      {
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++) stencil.append_line(N * f + k, N * f + l);
        // for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (d = 0, j = nf_tot + D * e; d < D; d++, j++)
        //     for (k = 0; k < N; k++) for (l = 0; l < N; l++) stencil.append_line(N * f + k, N * j + l);
      }
  /* elements si aux */
  dimensionner_blocs_aux(stencil);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}
