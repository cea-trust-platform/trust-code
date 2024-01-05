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

#include <Linear_algebra_tools_impl.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Frottement_impose_base.h>
#include <Masse_PolyVEF_Face.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Masse_ajoutee_base.h>
#include <Option_PolyVEF_P0.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Dirichlet.h>
#include <Matrice33.h>
#include <Vecteur3.h>
#include <TRUSTTab.h>
#include <Piso.h>

Implemente_instanciable(Masse_PolyVEF_Face, "Masse_PolyVEF_P0_Face|Masse_PolyVEF_P0P1_Face", Masse_PolyMAC_P0_Face);

Sortie& Masse_PolyVEF_Face::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_PolyVEF_Face::readOn(Entree& s) { return s ; }

DoubleTab& Masse_PolyVEF_Face::appliquer_impl(DoubleTab& sm) const
{
  const Domaine_PolyVEF& dom =  ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const IntTab& f_e = dom.face_voisins();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces();
  int i, e, f, d, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D;
  const DoubleTab *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().champ_conserve().passe() : nullptr, &vfd = dom.volumes_entrelaces_dir();
  double fac;

  //vitesses aux faces
  for (f = 0; f < dom.nb_faces(); f++)
    for (d = 0; d < D; d++)
      for (n = 0; n < N; n++)
        {
          for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += vfd(f, i) / vf(f) * (a_r ? (*a_r)(e, n) : 1);
          if (fac > 1e-10) sm(f, N * d + n) /= pf(f) * vf(f) * fac; //vitesse calculee
          else sm(f, N * d + n) = 0; //cas d'une evanescence
        }

  sm.echange_espace_virtuel();
  return sm;
}

void Masse_PolyVEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inc), mat2;
  const Domaine_PolyVEF& dom =  ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const DoubleTab& inco = equation().inconnue().valeurs();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue()).fcl();
  int i, j, f, d, D = dimension, n, N = inco.line_size() / D, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);

  IntTrav sten(0, 2);
  for (f = 0, i = 0; f < dom.nb_faces(); f++)
    for (d = 0; d < D; d++)
      for (n = 0; n < N; n++, i++)
        if (p0p1 && fcl(f, 0) > 2) //Dirichlet en P0P1 : diagonal
          sten.append_line(i, i);
        else if (corr && fcl(f, 0) > 2) //Dirichlet + masse ajoutee : melange tout
          for (j = N * D * f; j < N * D * (f + 1); j++) sten.append_line(i, j);
        else if (corr) //juste masse ajoutee : melange les phases
          for (j = N * (D * f + d); j < N * (D * f + d + 1); j++) sten.append_line(i, j);
        else if (fcl(f, 0) > 2) //juste Dirichlet : melange les directions
          for (j = N * D * f + n; j < N * D * (f + 1) + n; j += N) sten.append_line(i, j);
        else sten.append_line(i, i); //sinon : diagonale!

  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// XXX : a voir si on peut utiliser Solveur_Masse_Face_proto::ajouter_blocs_proto ...
void Masse_PolyVEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
  Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
  const Domaine_PolyVEF& dom =  ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const IntTab& f_e = dom.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue()).fcl();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces(), &fs = dom.face_surfaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const DoubleTab& rho = equation().milieu().masse_volumique().passe(), &nf = dom.face_normales(),
                   *alpha = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr, *a_r = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr, &vfd = dom.volumes_entrelaces_dir();
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  const Conds_lim& cls = equation().inconnue().domaine_Cl_dis().les_conditions_limites();
  int i, e, f, m, d, db, D = dimension, n, N = inco.line_size() / D, cR = rho.dimension_tot(0) == 1, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);

  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  DoubleTrav masse(N, N), masse_e(N, N); //masse alpha * rho, contribution
  double force = 1e12; //forcage des CLs de Dirichlet
  for (f = 0; f < dom.nb_faces(); f++) //faces reelles
    if (p0p1 && fcl(f, 0) > 2) //Dirichlet en P0P1 -> trivial
      for (d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          {
            secmem(f, N * d + n) += (fcl(f, 0) == 3 ? ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n) : 0) - resoudre_en_increments * inco(f, N * d + n);
            if (mat) (*mat)(N * (D * f + d) + n, N * (D * f + d) + n) = 1;
          }
    else
      {
        if (!pbm)
          for (masse = 0, n = 0; n < N; n++) masse(n, n) = 1; //pas Pb_Multiphase -> pas de alpha * rho
        else for (masse = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            {
              for (masse_e = 0, n = 0; n < N; n++) masse_e(n, n) = (*a_r)(e, n); //partie diagonale
              if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse_e); //partie masse ajoutee
              for (n = 0; n < N; n++)
                for (m = 0; m < N; m++) masse(n, m) += vfd(f, i) / vf(f) * masse_e(n, m); //contribution au alpha * rho de la face
            }
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            {
              double fac = pf(f) * vf(f) / dt;
              for (m = 0; m < N; m++) secmem(f, N * d + n) -= fac * resoudre_en_increments * masse(n, m) * inco(f, N * d + m);
              for (m = 0; m < N; m++) secmem(f, N * d + n) += fac * masse(n, m) * passe(f, N * d + m);
              if (mat)
                for (m = 0; m < N; m++)
                  if (masse(n, m))
                    (*mat)(N * (D * f + d) + n, N * (D * f + d) + m) += fac * masse(n, m);

              if (fcl(f, 0) > 2)
                {
                  for (db = 0; db < D; db++) //Dirichlet : forcage des directions tangentielles
                    secmem(f, N * d + n) -= fac * force * ((d == db) - nf(f, d) * nf(f, db) / (fs(f) * fs(f))) * (resoudre_en_increments * inco(f, N * db + n) - (fcl(f, 0) < 4 ? ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * db + n) : 0));
                  if (mat)
                    for (db = 0; db < D; db++) //Dirichlet : forcage des directions tangentielles
                      (*mat)(N * (D * f + d) + n, N * (D * f + db) + n) += fac * force * ((d == db) - nf(f, d) * nf(f, db) / (fs(f) * fs(f)));
                }
              else if (fcl(f, 0) == 2 && sub_type(Frottement_impose_base, cls[fcl(f, 1)].valeur())) //Navier
                {
                  // Pour l'instant on fait un truc sale : on appelle un frottement global comme un frottement externe pour avoir une seule classe paroi_frottante
                  const Frottement_impose_base *cl = sub_type(Frottement_impose_base, cls[fcl(f, 1)].valeur()) ? &ref_cast(Frottement_impose_base, cls[fcl(f, 1)].valeur()) : nullptr;
                  if (!cl || cl->is_externe()) Process::exit("Masse_PolyVEF_Face: only Frottement_impose_base global is supported!");
                  secmem(f, N * d + n) -= fs(f) * cl->coefficient_frottement(fcl(f, 2), n) * inco(f, N * d + n);
                  if (mat) (*mat)(N * (D * f + d) + n, N * (D * f + d) + n) += fs(f) * cl->coefficient_frottement(fcl(f, 2), n);
                }
            }
      }
}

//recalcule les vitesses tangentielles aux faces a partir des vitesses normales
DoubleTab& Masse_PolyVEF_Face::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  ref_cast(Champ_Face_PolyVEF, equation().inconnue()).update_vf2(x, incr);
  return x;
}
