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

#include <Champ_Face_PolyVEF.h>
#include <Domaine.h>
#include <Domaine_VF.h>
#include <Champ_Uniforme.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <EChaine.h>
#include <TRUSTTab_parts.h>
#include <Linear_algebra_tools_impl.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyVEF.h>
#include <Dirichlet_homogene.h>
#include <Champ_Fonc_reprise.h>
#include <Option_PolyVEF_P0.h>
#include <Schema_Temps_base.h>
#include <TRUSTTab_parts.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <TRUSTLists.h>
#include <Domaine_VF.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <EChaine.h>
#include <Domaine.h>
#include <array>
#include <cmath>

Implemente_instanciable(Champ_Face_PolyVEF, "Champ_Face_PolyVEF_P0|Champ_Face_PolyVEF_P0P1", Champ_Face_PolyMAC_P0);

Sortie& Champ_Face_PolyVEF::printOn(Sortie& os) const { return os << que_suis_je() << " " << le_nom(); }

Entree& Champ_Face_PolyVEF::readOn(Entree& is) { return is; }

int Champ_Face_PolyVEF::fixer_nb_valeurs_nodales(int n)
{
  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base sauf que je recupere le nombre de faces au lieu du nombre d'elements
  // je suis tout de meme etonne du code utilise dans Champ_Fonc_P0_base::fixer_nb_valeurs_nodales() pour recuperer le domaine discrete...
  assert(n == domaine_PolyVEF().nb_faces() || n < 0);
  creer_tableau_distribue(domaine_PolyVEF().md_vector_faces());
  return n;
}

Champ_base& Champ_Face_PolyVEF::affecter_(const Champ_base& ch)
{
  const Domaine_PolyVEF& dom = domaine_PolyVEF();
  const DoubleTab& xv = dom.xv();
  DoubleTab& val = valeurs(), eval;

  if (sub_type(Champ_Fonc_reprise, ch))
    {
      for (int num_face = 0; num_face < dom.nb_faces(); num_face++)
        for (int i = 0; i < val.dimension(1); i++)
          val(num_face, i) = ch.valeurs()(num_face, i);
    }
  else
    {
      int f, i, ND = val.line_size(), unif = sub_type(Champ_Uniforme, ch);

      if (unif)
        eval = ch.valeurs();
      else
        eval.resize(val.dimension_tot(0), ND), ch.valeur_aux(xv, eval);
      for (f = 0; f < dom.nb_faces_tot(); f++)
        for (i = 0; i < ND; i++)
          val(f, i) = eval(unif ? 0 : f, i);
      //copie dans toutes les cases
      valeurs().echange_espace_virtuel();
      for (i = 1; i < les_valeurs->nb_cases(); i++)
        les_valeurs[i].valeurs() = valeurs();
    }
  return *this;
}

DoubleTab& Champ_Face_PolyVEF::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  const Domaine_PolyVEF& dom = domaine_PolyVEF();
  const DoubleTab& src = valeurs(), &nf = dom.face_normales(), &xp = dom.xp(), &xv = dom.xv(), &vf_dir = dom.volumes_entrelaces_dir();
  const DoubleVect& ve = dom.volumes(), *pe = mon_equation_non_nul() ? &equation().milieu().porosite_elem() : nullptr, *pf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : nullptr;
  const IntTab& e_f = dom.elem_faces(), &f_e = dom.face_voisins();
  int i, j, e, f, d, db, D = dimension, n, N = valeurs().line_size() / D;

  if (Option_PolyVEF_P0::interp_postraitement_no_poro)  // Cas sans porosite : on fait une moyenne volumique
    for (val = 0, i = 0; i < les_polys.size(); i++) //element
      for (e = les_polys(i), j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) //face de l'element
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            val(i, N * d + n) += src(f, N * d + n) * vf_dir(f, e == f_e(f, 0) ? 0 : 1) / ve(e);

  else
    // Cas general : on utilise que les normales de vitesse aux faces
    for (val = 0, i = 0; i < les_polys.size(); i++) //element
      for (e = les_polys(i), j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) //face de l'element
        for (d = 0; d < D; d++)
          for (db = 0; db < D; db++)
            for (n = 0; n < N; n++)
              val(i, N * d + n) += src(f, N * db + n) * (pf ? (*pf)(f) : 1) * (e == f_e(f, 0) ? 1 : -1) * nf(f, db) * (xv(f, d) - xp(e, d)) / (ve(e) * (pe ? (*pe)(e) : 1));

  return val;
}

DoubleVect& Champ_Face_PolyVEF::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  const Domaine_PolyVEF& dom = domaine_PolyVEF();
  const DoubleTab& src = valeurs(), &nf = dom.face_normales(), &xp = dom.xp(), &xv = dom.xv(), &vf_dir = dom.volumes_entrelaces_dir();
  const DoubleVect& ve = dom.volumes(), *pe = mon_equation_non_nul() ? &equation().milieu().porosite_elem() : nullptr, *pf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : nullptr;
  const IntTab& e_f = dom.elem_faces(), &f_e = dom.face_voisins();
  int i, j, e, f, db, D = dimension, N = src.line_size() / D, d = ncomp / N, n = ncomp % N;

  if (Option_PolyVEF_P0::interp_postraitement_no_poro) // Cas sans porosite : on fait une moyenne volumique
    for (val = 0, i = 0; i < les_polys.size(); i++) //element
      for (e = les_polys(i), j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) //face de l'element
        for (n = 0; n < N; n++)
          val(i) += src(f, N * d + n) * vf_dir(f, e == f_e(f, 0) ? 0 : 1) / ve(e);

  else
    // Cas general : on utilise que les normales de vitesse aux faces
    for (val = 0, i = 0; i < les_polys.size(); i++) //element
      for (e = les_polys(i), j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) //face de l'element
        for (db = 0; db < D; db++)
          for (n = 0; n < N; n++)
            val(i) += src(f, N * db + n) * (pf ? (*pf)(f) : 1) * (e == f_e(f, 0) ? 1 : -1) * nf(f, db) * (xv(f, d) - xp(e, d)) / (ve(e) * (pe ? (*pe)(e) : 1));

  return val;
}

DoubleTab& Champ_Face_PolyVEF::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant == 0);
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const DoubleTab& val = valeurs(t);

  for (int i = 0, N = val.dimension(1); i < fr_vf.nb_faces(); i++)
    {
      const int f = fr_vf.num_face(i);
      for (int n = 0; n < N; n++)
        x(i, n) = val(f, n);
    }

  return x;
}

void Champ_Face_PolyVEF::init_vf2() const
{
#ifdef _COMPILE_AVEC_PGCC_AVANT_22_7
  Cerr << "Internal error with nvc++: Internal error: read_memory_region: not all expected entries were read." << finl;
  Process::exit();
#else
  const Domaine_PolyVEF& dom = domaine_PolyVEF();
  if (vf2d.dimension(0))
    return; //deja initialise
  const DoubleVect& pf = equation().milieu().porosite_face(), &fs = dom.face_surfaces(), &ve = dom.volumes(), &vf = dom.volumes_entrelaces();
  const DoubleTab& xp = dom.xp(), &xv = dom.xv(), &nf = dom.face_normales();
  const IntTab& f_e = dom.face_voisins(), &e_f = dom.elem_faces(), &f_s = dom.face_sommets();
  int i, j, jb, k, e, f, fb, s, sb, d, db, D = dimension, nc, nl = D * (D + 1), nw, infoo = 0, un = 1, rank;
  double eps = 1e-8, fac, vol;
  init_fcl();

  DoubleTrav vf2, vf2i, A, B, P, W(1), x_cg(D);
  IntTrav pvt;

  /* connectivite arete-face */
  std::map<std::array<int, 2>, std::vector<int>> a_f;
  for (f = 0; f < dom.nb_faces_tot(); f++)
    for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
      sb = D < 3 ? -1 : f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0), a_f[ { { std::min(s, sb), std::max(s, sb) } }].push_back(f);

  vf2d.resize(1, 2), vf2bj.resize(0, 2), vf2c.resize(0, D), vf2bc.resize(0, D);
  std::map<std::array<int, 2>, int> v_i; // v_i[{f, -1 (interne) ou composante }] = indice
  std::vector<std::array<int, 2>> i_v; // v_i[i_v[f]] = f
  for (f = 0; f < dom.nb_faces(); f++, v_i.clear(), i_v.clear(), vf2d.append_line(vf2c.dimension(0), vf2bc.dimension(0)))
    {
      if (fcl_(f, 0) > 2) //Dirichlet -> rien a faire
        continue;
      /* stencil : faces des elems voisins + faces voisines par un sommet (2D) / arete (3D) */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
          for (k = fcl_(fb, 0) < 3 ? -1 : 0; k < (fcl_(fb, 0) < 3 ? 0 : D); k++) //toutes les composantes si face de bord de Dirichlet, composante normale sinon
            if (!v_i.count( { { fb, k } }))
      v_i[ { { fb, k } }] = (int) i_v.size(), i_v.push_back( { { fb, k } });
      for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
        {
          sb = D < 3 ? -1 : f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0);
          for (auto &&fa : a_f.at( { { std::min(s, sb), std::max(s, sb) } }))
          for (k = fcl_(fa, 0) < 3 ? -1 : 0; k < (fcl_(fa, 0) < 3 ? 0 : D); k++) //toutes les composantes si face de bord de Dirichlet, composante normale sinon
            if (!v_i.count( { { fa, k } }))
          v_i[ { { fa, k } }] = (int) i_v.size(), i_v.push_back( { { fa, k } });
        }
      vf2.resize(nc = (int) i_v.size(), D), A.resize(nc, nl), B.resize(nc), P.resize(nc), pvt.resize(nc);

      /* coeffs de l'interpolation d'ordre 1 */
      for (x_cg = 0, vol = ve(f_e(f, 0)) + (f_e(f, 1) >= 0 ? ve(f_e(f, 1)) : 0), i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) //CG du polyedre "elem amont + elem aval"
        for (d = 0; d < D; d++)
          x_cg(d) += ve(e) / vol * xp(e, d);
      for (vf2 = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
          if (fb == f && f_e(f, 1) >= 0)
            continue;
          else if (v_i.count( { { fb, -1 } }))
      for (k = v_i[ { { fb, -1 } }], fac = (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / vol, d = 0; d < D; d++)
      vf2(k, d) += fac * (xv(fb, d) - x_cg(d));
      else
        for (db = 0; db < D; db++)
          for (k = v_i[ { { fb, db } }], fac = (e == f_e(fb, 0) ? 1 : -1) * nf(fb, db) / vol, d = 0; d < D; d++)
      vf2(k, d) += fac * (xv(fb, d) - x_cg(d));
      /* ponderations (comme dans Domaine_PolyVEF::{e,f}grad) */
      for (i = 0; i < nc; i++)
        fb = i_v[i][0], P(i) = 1. / std::max(sqrt(dom.dot(&xv(fb, 0), &xv(fb, 0), &xv(f, 0), &xv(f, 0))), 1e-8 * vf(f) / fs(f));

      /* par composante, si on a assez de voisins : correction pour etre d'ordre 2 */
      if (nc >= nl)
        for (d = 0; d < D; d++)
          {
            /* systeme A.x = b */
            for (B = 0, pvt = 0, i = 0; i < nc; i++)
              for (fb = i_v[i][0], db = i_v[i][1], j = 0, jb = 0; j < D; j++)
                for (k = 0; k <= D; k++, jb++)
                  {
                    fac = (db < 0 ? nf(fb, j) / fs(fb) : (db == j)) * (k < D ? xv(fb, k) - xv(f, k) : 1);
                    A(i, jb) = fac * P(i);
                    if (k < D)
                      B(jb) -= fac * vf2(i, d); //erreur de l'interp d'ordre 1 a corriger
                  }

            /* x de norme L2 minimale par dgels */
            nw = -1, F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
            W.resize(nw = (int) std::lrint(W(0))), F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
            assert(infoo == 0);
            /* ajout dans ve2 */
            for (i = 0; i < nc; i++)
              vf2(i, d) += P(i) * B(i);
          }

      /* preservation de la composante normale */
      for (i = 0; i < nc; i++) //on la retire partout...
        for (fac = dom.dot(&vf2(i, 0), &nf(f, 0)) / (fs(f) * fs(f)), d = 0; d < D; d++)
          vf2(i, d) -= fac * nf(f, d);
      for (i = v_i.at( { { f, -1 } }), d = 0; d < D; d++)
      vf2(i, d) += nf(f, d) / fs(f); //et on la remet sur f

      /* stockage */
      for (i = 0; i < nc; i++)
        if (dom.dot(&vf2(i, 0), &vf2(i, 0)) > 1e-16)
          {
            i_v[i][1] < 0 ? vf2j.append_line(i_v[i][0]) : vf2bj.append_line(i_v[i][0], i_v[i][1]);
            fac = pf(i_v[i][0]) / pf(f);
            DoubleTab& tab = i_v[i][1] < 0 ? vf2c : vf2bc;
            D < 3 ? tab.append_line(fac * vf2(i, 0), fac * vf2(i, 1)) : tab.append_line(fac * vf2(i, 0), fac * vf2(i, 1), fac * vf2(i, 2));
          }
    }
  CRIMP(vf2d), CRIMP(vf2j), CRIMP(vf2bj), CRIMP(vf2c), CRIMP(vf2bc);
#endif
}

/* met en coherence les composantes aux elements avec les vitesses aux faces : interpole sur phi * v */
void Champ_Face_PolyVEF::update_vf2(DoubleTab& val, int incr) const
{
  if (!Option_PolyVEF_P0::interp_vf2 || 1)
    return;
  const Domaine_PolyVEF& dom = domaine_PolyVEF();
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites();
  const DoubleTab& nf = dom.face_normales();
  const DoubleVect& fs = dom.face_surfaces(); //, &pf = equation().milieu().porosite_face();
  int i, f, fb, d, db, D = dimension, N = val.line_size() / D, n;
  DoubleTrav vn(N), nv(D, N); //vitesse normale (a une face), nouvelle vitesse
  init_vf2(), init_fcl();

  for (f = 0; f < dom.nb_faces(); f++)
    if (vf2d(f, 0) < vf2d(f + 1, 0) || vf2d(f, 1) < vf2d(f + 1, 1))
      {
        for (nv = 0, i = vf2d(f, 0); i < vf2d(f + 1, 0); i++) //composantes normales
          {
            for (fb = vf2j(i), vn = 0, d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                vn(n) += val(fb, N * d + n) * nf(fb, d) / fs(fb);
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                nv(d, n) += vf2c(i, d) * vn(n);
          }
        if (!incr) //bords de Dirichlet (sauf si on traite en increments)
          for (i = vf2d(f, 1); i < vf2d(f + 1, 1); i++)
            if (sub_type(Dirichlet, cls[fcl_(fb = vf2bj(i, 0), 1)].valeur()))
              for (db = vf2bj(i, 1), d = 0; d < D; d++)
                for (n = 0; n < N; n++)
                  nv(d, n) += vf2bc(i, d) * ref_cast(Dirichlet, cls[fcl_(fb, 1)].valeur()).val_imp(fcl_(fb, 2), N * db + n);
        for (d = 0; d < D; d++) //stockage
          for (n = 0; n < N; n++)
            val(f, N * d + n) = nv(d, n);
      }
  val.echange_espace_virtuel();
}

