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

#include <Source_Force_Tchen_base.h>
#include <Champ_Face_base.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <math.h>

Implemente_base(Source_Force_Tchen_base, "Source_Force_Tchen_base", Sources_Multiphase_base);

Sortie& Source_Force_Tchen_base::printOn(Sortie& os) const { return os; }

Entree& Source_Force_Tchen_base::readOn(Entree& is)
{
  //identification des phases
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  return is;
}

void Source_Force_Tchen_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire

  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);

  int N = inco.line_size(), nf = domaine.nb_faces();

  /* faces */
  for (int f = 0; f < nf; f++)
    if (fcl(f, 0) < 2)
      for (int k = 0 ; k<N ; k++)
        if (k != n_l) //phase gazeuse
          stencil.append_line( N * f +  k , N * f + n_l) ;

  /* elements si aux */
  dimensionner_blocs_aux(stencil);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Source_Force_Tchen_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe(),
                   &rho   = equation().milieu().masse_volumique().passe(), &vf_dir = domaine.volumes_entrelaces_dir();

  double pas_tps = equation().probleme().schema_temps().pas_de_temps();
  int N = inco.line_size(), nf = domaine.nb_faces();

  /* elements si aux */
  ajouter_blocs_aux(matrices, secmem);

  /* faces */
  for (int f = 0; f < nf; f++)
    if (fcl(f, 0) < 2)
      for (int k = 0 ; k<N ; k++)
        if (k != n_l) //phase gazeuse
          {
            double alpha_loc = f_e(f, 1)>=0 ? alpha(f_e(f, 0), k) * vf_dir(f, 0)/vf(f) +  alpha(f_e(f, 1), k) * vf_dir(f, 1)/vf(f) : alpha(f_e(f, 0), k) ;
            double rho_loc   = f_e(f, 1)>=0 ? rho(f_e(f, 0), n_l) * vf_dir(f, 0)/vf(f) + rho(f_e(f, 1), n_l)  * vf_dir(f, 1)/vf(f) : rho(f_e(f, 0), n_l) ;
            double fac = pf(f) * vf(f) * alpha_loc * rho_loc ;

            secmem(f, k ) += fac * (inco(f, n_l)-pvit(f, n_l))/pas_tps;
            secmem(f,n_l) -= fac * (inco(f, n_l)-pvit(f, n_l))/pas_tps;
            if (mat)
              {
                (*mat)( N * f +  k , N * f + n_l) -= fac/pas_tps ;
                (*mat)( N * f + n_l, N * f + n_l) += fac/pas_tps ;
              }
          }
}
