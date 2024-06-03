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

#include <Champ_Face_PolyMAC.h>
#include <Masse_PolyMAC_Face.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Operateur.h>
#include <Dirichlet.h>

Implemente_instanciable(Masse_PolyMAC_Face, "Masse_PolyMAC_Face", Masse_PolyMAC_base);

Sortie& Masse_PolyMAC_Face::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_PolyMAC_Face::readOn(Entree& s) { return s; }

DoubleTab& Masse_PolyMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  //hors faces de bord, on ne fait rien et on passe secmem a corriger_derivee_* (car PolyMAC a une matrice de masse)
  assert(le_dom_PolyMAC.non_nul());
  assert(le_dom_Cl_PolyMAC.non_nul());
  const Domaine_PolyMAC& domaine_PolyMAC = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  ch.fcl();

  assert(sm.nb_dim() <= 2); // sinon on ne fait pas ce qu'il faut
  int nb_faces_tot = domaine_PolyMAC.nb_faces_tot();
  int nb_aretes_tot = (dimension < 3 ? domaine_PolyMAC.nb_som_tot() : domaine_PolyMAC.domaine().nb_aretes_tot()), nc = sm.line_size();

  if (sm.dimension_tot(0) != nb_faces_tot && sm.dimension_tot(0) != nb_faces_tot + nb_aretes_tot)
    {
      Cerr << "Masse_PolyMAC_Face::appliquer :  erreur dans la taille de sm" << finl;
      Process::exit();
    }

  //mise a zero de la partie vitesse de sm sur les faces a vitesse imposee
  for (int f = 0; f < domaine_PolyMAC.nb_faces(); f++)
    if (ch.fcl()(f, 0) > 1)
      for (int k = 0; k < nc; k++)
        sm(f, k) = 0;

  return sm;
}

void Masse_PolyMAC_Face::dimensionner(Matrice_Morse& matrix) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC;
  const IntTab& e_f = domaine.elem_faces();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  int i, j, k, e, a, f, fb, nf_tot = domaine.nb_faces_tot(), na_tot = dimension < 3 ? domaine.domaine().nb_som_tot() : domaine.domaine().nb_aretes_tot();
  const bool only_m2 = (matrix.nb_lignes() == nf_tot);

  domaine.init_m1(), domaine.init_m2(), ch.init_ra();
  IntTab indice(0, 2);
  indice.set_smart_resize(1);
  //partie vitesses : matrice de masse des vitesses si la face n'est pas a vitesse imposee, diagonale sinon
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = 0, j = domaine.m2d(e); j < domaine.m2d(e + 1); i++, j++)
      if (ch.fcl()(f = e_f(e, i), 0) > 1 && f < domaine.nb_faces())
        indice.append_line(f, f);
      else
        for (k = domaine.m2i(j); f < domaine.nb_faces() && k < domaine.m2i(j + 1); k++)
          if (ch.fcl()(fb = e_f(e, domaine.m2j(k)), 0) < 2)
            indice.append_line(f, fb);

  //partie vorticites : diagonale si pas de diffusion
  if (!only_m2)
    for (a = 0; no_diff_ && a < (dimension < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()); a++)
      indice.append_line(nf_tot + a, nf_tot + a);

  tableau_trier_retirer_doublons(indice);
  Matrix_tools::allocate_morse_matrix(nf_tot + !only_m2 * na_tot, nf_tot + !only_m2 * na_tot, indice, matrix);
}

DoubleTab& Masse_PolyMAC_Face::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const DoubleTab& nf = domaine.face_normales();
  const DoubleVect& fs = domaine.face_surfaces(), &pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();
  DoubleVect coef(equation().milieu().porosite_face());
  coef = 1.;
  int i, j, k, l, e, f, fb;

  if (has_coefficient_temporel_) appliquer_coef(coef);

  domaine.init_m1(), domaine.init_m2(), ch.init_ra();

  //partie vitesses : vitesses imposees par CLs
  for (f = 0; f < domaine.premiere_face_int(); f++)
    if (ch.fcl()(f, 0) == 3)
      for (k = 0, secmem(f) = 0; k < dimension; k++) //valeur imposee par une CL de type Dirichlet
        secmem(f) += nf(f, k) * ref_cast(Dirichlet, cls[ch.fcl()(f, 1)].valeur()).val_imp(ch.fcl()(f, 2), k) / fs(f);
    else if (ch.fcl()(f, 0) > 1)
      secmem(f) = 0; //Dirichlet homogene ou Symetrie

  //partie vitesses : m2 / dt
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = 0, j = domaine.m2d(e); j < domaine.m2d(e + 1); i++, j++)
      for (f = e_f(e, i), k = domaine.m2i(j); ch.fcl()(f, 0) < 2 && f < domaine.nb_faces() && k < domaine.m2i(j + 1); k++)
        if (ch.fcl()(fb = e_f(e, domaine.m2j(k)), 0) < 2) //vfb calcule
          secmem(f) += ve(e) * pe(e) * domaine.m2c(k) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * coef(f) * inco(fb) / dt;
        else if (ch.fcl()(fb, 0) == 3)
          for (l = 0; l < dimension; l++) //vfb impose par Dirichlet
            secmem(f) += ve(e) * pe(e) * domaine.m2c(k) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * coef(f)
                         * ref_cast(Dirichlet, cls[ch.fcl()(fb, 1)].valeur()).val_imp(ch.fcl()(fb, 2), l) * nf(fb, l) / (fs(fb) * dt);

  return secmem;
}

Matrice_Base& Masse_PolyMAC_Face::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();
  DoubleVect coef(equation().milieu().porosite_face());
  coef = 1.;
  int i, j, k, e, a, f, fb, nf_tot = domaine.nb_faces_tot();
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);

  if (has_coefficient_temporel_) appliquer_coef(coef);

  domaine.init_m1(), domaine.init_m2(), ch.init_ra();

  //partie vitesses : vitesses imposees par CLs
  for (f = 0; f < domaine.premiere_face_int(); f++)
    if (ch.fcl()(f, 0) > 1)
      mat(f, f) = 1;

  //partie vitesses : m2 / dt
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = 0, j = domaine.m2d(e); j < domaine.m2d(e + 1); i++, j++)
      for (f = e_f(e, i), k = domaine.m2i(j); ch.fcl()(f, 0) < 2 && f < domaine.nb_faces() && k < domaine.m2i(j + 1); k++)
        if (ch.fcl()(fb = e_f(e, domaine.m2j(k)), 0) < 2) //vfb calcule
          mat(f, fb) += ve(e) * pe(e) * domaine.m2c(k) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * coef(f) / dt;

  //partie vorticites : diagonale si Op_Diff_negligeable
  if (mat.nb_lignes() > nf_tot)
    for (a = 0; no_diff_ && a < (dimension < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()); a++)
      mat(nf_tot + a, nf_tot + a) = 1;

  return matrice;
}
