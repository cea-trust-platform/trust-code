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

#include <Solveur_Masse_Face_proto.h>
#include <Masse_ajoutee_base.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <TRUSTTrav.h>
#include <Domaine_VF.h>

void Solveur_Masse_Face_proto::associer_masse_proto(const Solveur_Masse_base& sm, const Domaine_VF& zvf )
{
  solv_mass_ = sm;
  le_dom_ = zvf;
}

DoubleTab& Solveur_Masse_Face_proto::appliquer_impl_proto(DoubleTab& sm) const
{
  const Domaine_VF& domaine = le_dom_.valeur();
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = solv_mass_->equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  const DoubleTab *a_r = sub_type(QDM_Multiphase, solv_mass_->equation()) ? &ref_cast(Pb_Multiphase, solv_mass_->equation().probleme()).equation_masse().champ_conserve().passe() : nullptr,
                   &vfd = domaine.volumes_entrelaces_dir();
  int i, e, f, n, N = solv_mass_->equation().inconnue().valeurs().line_size();
  double fac;

  //vitesses aux faces
  for (f = 0; f < domaine.nb_faces(); f++)
    for (n = 0; n < N; n++)
      {
        for (fac = 0, i = 0; i < 2; i++)
          if ((e = f_e(f, i)) >= 0)
            fac += vfd(f, i) / vf(f) * (a_r ? (*a_r)(e, n) : 1);
        if (fac > 1e-10)
          sm(f, n) /= pf(f) * vf(f) * fac; //vitesse calculee
        else
          sm(f, n) = 0; //cas d'une evanescence
      }

  sm.echange_espace_virtuel();
  return sm;
}

void Solveur_Masse_Face_proto::dimensionner_blocs_proto(matrices_t matrices, const tabs_t& semi_impl, const bool allocate, IntTab& sten) const
{
  const std::string& nom_inc = solv_mass_->equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire

  Matrice_Morse& mat = *matrices.at(nom_inc), mat2;
  const Domaine_VF& domaine = le_dom_.valeur();
  const DoubleTab& inco = solv_mass_->equation().inconnue().valeurs();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, solv_mass_->equation().probleme()) ? &ref_cast(Pb_Multiphase, solv_mass_->equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  int i, f, m, n, N = inco.line_size();

  for (f = 0, i = 0; f < domaine.nb_faces(); f++)
    for (n = 0; n < N; n++, i++) //faces reelles
      if (corr)
        for (m = 0; m < N; m++) sten.append_line(i, N * f + m);
      else sten.append_line(i, i);

  if (allocate)
    {
      Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, mat2);
      mat.nb_colonnes() ? mat += mat2 : mat = mat2;
    }
}

void Solveur_Masse_Face_proto::ajouter_blocs_proto(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = solv_mass_->equation().inconnue().valeurs(), &passe = solv_mass_->equation().inconnue().passe();
  Matrice_Morse *mat = matrices[solv_mass_->equation().inconnue().le_nom().getString()]; //facultatif
  const Domaine_VF& domaine = le_dom_.valeur();
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = solv_mass_->equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, solv_mass_->equation().probleme()) ? &ref_cast(Pb_Multiphase, solv_mass_->equation().probleme()) : nullptr;
  const DoubleTab& vfd = domaine.volumes_entrelaces_dir(), &rho = solv_mass_->equation().milieu().masse_volumique().passe(),
                   *alpha = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr, *a_r = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  int i, e, f, m, n, N = inco.line_size(), cR = rho.dimension_tot(0) == 1;

  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  DoubleTrav masse(N, N), masse_e(N, N); //masse alpha * rho, contribution
  for (f = 0; f < domaine.nb_faces(); f++) //faces reelles
    {
      if (!pbm)
        for (masse = 0, n = 0; n < N; n++) masse(n, n) = vf(f); //pas Pb_Multiphase ou CL -> pas de alpha * rho
      else for (masse = 0, i = 0; i < 2; i++)
          if ((e = f_e(f, i)) >= 0)
            {
              for (masse_e = 0, n = 0; n < N; n++) masse_e(n, n) = (*a_r)(e, n); //partie diagonale
              if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse_e); //partie masse ajoutee
              for (n = 0; n < N; n++)
                for (m = 0; m < N; m++) masse(n, m) += vfd(f, i) * masse_e(n, m); //contribution au alpha * rho de la face
            }
      for (n = 0; n < N; n++)
        {
          for (m = 0; m < N; m++) secmem(f, n) += pf(f) / dt * masse(n, m) * (passe(f, m) - resoudre_en_increments * inco(f, m));
          if (mat)
            for (m = 0; m < N; m++)
              if (masse(n, m)) (*mat)(N * f + n, N * f + m) += pf(f) / dt * masse(n, m);
        }
    }
}
