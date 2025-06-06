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

#include <Champ_Face_PolyMAC_P0.h>
#include <Domaine.h>
#include <Domaine_VF.h>
#include <Champ_Uniforme.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <EChaine.h>
#include <TRUSTTab_parts.h>
#include <Linear_algebra_tools_impl.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Fonc_reprise.h>
#include <array>
#include <cmath>
#include <Pb_Multiphase.h>

Implemente_instanciable(Champ_Face_PolyMAC_P0,"Champ_Face_PolyMAC_P0",Champ_Face_PolyMAC_P0P1NC) ;

Sortie& Champ_Face_PolyMAC_P0::printOn(Sortie& os) const { return os << que_suis_je() << " " << le_nom(); }

Entree& Champ_Face_PolyMAC_P0::readOn(Entree& is) { return is; }

int Champ_Face_PolyMAC_P0::fixer_nb_valeurs_nodales(int n)
{
  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base sauf que je recupere le nombre de faces au lieu du nombre d'elements
  // je suis tout de meme etonne du code utilise dans Champ_Fonc_P0_base::fixer_nb_valeurs_nodales() pour recuperer le domaine discrete...

  assert(n == domaine_PolyMAC_P0().nb_faces() || n < 0);

  // Probleme: nb_comp vaut dimension mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  if(nb_compo_ != 1) nb_compo_ /= dimension;

  /* variables : valeurs normales aux faces, puis valeurs aux elements par blocs -> pour que line_size() marche */
  creer_tableau_distribue(domaine_PolyMAC_P0().md_vector_faces());
  nb_compo_ = old_nb_compo;
  return n;
}

void Champ_Face_PolyMAC_P0::init_auxiliary_variables()
{
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    {
      DoubleTab& vals = futur(n);
      vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
      vals.resize_dim0(domaine_PolyMAC_P0().mdv_ch_face->get_nb_items_tot()); //...on dimensionne a la bonne taille...
      vals.set_md_vector(domaine_PolyMAC_P0().mdv_ch_face); //...et on remet le bon MD_Vector
      update_ve(vals);
    }
}

int Champ_Face_PolyMAC_P0::reprendre(Entree& fich)
{
  if (! via_ch_fonc_reprise()) return Champ_Inc_base::reprendre(fich); /* ie: resume last time ! */

  const Pb_Multiphase * pbm = mon_equation_non_nul() ? (sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr) : nullptr;
  if (pbm) return Champ_Inc_base::reprendre(fich);

  // sinon on fait ca ...
  const Domaine_PolyMAC_P0* domaine = le_dom_VF.non_nul() ? &ref_cast( Domaine_PolyMAC_P0,le_dom_VF.valeur()) : nullptr;
  valeurs().set_md_vector(MD_Vector()); //on enleve le MD_Vector...
  valeurs().resize(0);
  int ret = Champ_Inc_base::reprendre(fich);
  //et on remet le bon si on peut
  if (domaine) valeurs().set_md_vector(valeurs().dimension_tot(0) > domaine->nb_faces_tot() ? domaine->mdv_faces_aretes : domaine->md_vector_faces());
  return ret;
}

Champ_base& Champ_Face_PolyMAC_P0::affecter_(const Champ_base& ch)
{
  const DoubleTab& ch_val = ch.valeurs();
  DoubleTab& val = valeurs();
  const int N = val.line_size(), D = Objet_U::dimension;

  if (sub_type(Champ_Fonc_reprise, ch))
    {
      if (val.dimension_tot(0) == ch_val.dimension_tot(0) && val.line_size() == ch_val.line_size())
        val = ch_val;
      else
        {
          init_auxiliary_variables();
          for (int i = 0; i < ch_val.dimension_tot(0); i++)
            for (int j = 0; j < ch_val.line_size(); j++)
              val(i,j) = ch_val(i,j);
        }
    }
  else
    {
      const Domaine_PolyMAC_P0& domaine = domaine_PolyMAC_P0();
      const DoubleVect& fs = domaine.face_surfaces();
      const DoubleTab& nf = domaine.face_normales(), &xv = domaine.xv();
      const int unif = sub_type(Champ_Uniforme, ch);
      DoubleTab eval;

      if (unif)
        eval = ch_val;
      else
        eval.resize(val.dimension_tot(0), N * D), ch.valeur_aux(xv, eval);

      for (int f = 0; f < domaine.nb_faces_tot(); f++)
        for (int d = 0; d < D; d++)
          for (int n = 0; n < N; n++)
            val(f, n) += eval(unif ? 0 : f, N * d + n) * nf(f, d) / fs(f);

      update_ve(val);
      //copie dans toutes les cases
      val.echange_espace_virtuel();
      for (int i = 1; i < les_valeurs->nb_cases(); i++)
        les_valeurs[i].valeurs() = val;
    }
  return *this;
}


void Champ_Face_PolyMAC_P0::update_ve(DoubleTab& val) const
{
  const Domaine_PolyMAC_P0& domaine = domaine_PolyMAC_P0();
  if (valeurs().get_md_vector() != domaine.mdv_ch_face)
    return; //pas de variables auxiliaires -> rien a faire

  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes(),
                    *pf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : nullptr,
                     *pe = pf ? &equation().milieu().porosite_elem() : nullptr;

  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const int D = dimension, N = val.line_size(), ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot();

  double fac;
  for (int e = 0; e < ne_tot; e++)
    {
      for (int d = 0; d < D; d++)
        for (int n = 0; n < N; n++)
          val(nf_tot + D * e + d, n) = 0;
      for (int j = 0; j < e_f.dimension(1); j++)
        {
          const int f = e_f(e, j);
          if (f < 0) continue;

          fac = (e == f_e(f, 0) ? 1 : -1) * (pf ? (*pf)(f) : 1.0) * fs(f) / ((pe ? (*pe)(e) : 1.0) * ve(e));
          for (int d = 0; d < D; d++)
            for (int n = 0; n < N; n++)
              val(nf_tot + D * e + d, n) += fac * (xv(f, d) - xp(e, d)) * val(f, n);
        }
    }
}

void Champ_Face_PolyMAC_P0::init_ve2() const
{
  const Domaine_PolyMAC_P0& domaine = domaine_PolyMAC_P0();

  if (ve2d.dimension(0) || valeurs().get_md_vector() != domaine.mdv_ch_face)
    return; //deja initialise ou pas de variables auxiliaires

  const DoubleVect& pf = equation().milieu().porosite_face(),
                    &pe = equation().milieu().porosite_elem(),
                     &fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &nf = domaine.face_normales();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(),
                &e_s = domaine.domaine().les_elems(), &f_s = domaine.face_sommets();

  int D = dimension, nl = D * (D + 1), infoo = 0, un = 1;
  double eps = 1e-8, fac;
  const double *xf;

  init_fcl();

  //position des points aux faces de bord : CG si interne ou Dirichlet, projection si Neumann
  DoubleTrav xfb(domaine.nb_faces_tot(), D), ve2, ve2i, A, B, P, W(1);
  IntTrav pvt;

  for (int f = 0; f < domaine.nb_faces_tot(); f++)
    if (fcl_(f, 0) == 1 || fcl_(f, 0) == 2) //Neumann / Symetrie
      {
        const int e = f_e(f, 0);
        double scal = domaine.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) / (fs(f) * fs(f));
        for (int d = 0; d < D; d++)
          xfb(f, d) = xp(e, d) + scal * nf(f, d);
      }
    else if (fcl_(f, 0))
      {
        for (int d = 0; d < D; d++)
          xfb(f, d) = xv(f, d); //Dirichlet
      }

  /* connectivites som-elem et elem-elem */
  std::vector<std::set<int>> s_f(domaine.domaine().nb_som()), e_s_f(domaine.nb_elem());
  for (int f = 0; f < domaine.nb_faces_tot(); f++)
    for (int i = 0; i < f_s.dimension(1); i++)
      {
        const int s = f_s(f, i);
        if (s < 0) continue;

        if (s < domaine.domaine().nb_som())
          s_f[s].insert(f);
      }

  for (int e = 0; e < domaine.nb_elem(); e++)
    for (int i = 0; i < e_s.dimension(1); i++)
      {
        const int s = e_s(e, i);
        if (s < 0) continue;

        for (auto &&fa : s_f[s])
          e_s_f[e].insert(fa);
      }

  ve2d.resize(1, 2);
  ve2bj.resize(0, 2);
  std::map<std::array<int, 2>, int> v_i; // v_i[{f, -1 (interne) ou composante }] = indice
  std::vector<std::array<int, 2>> i_v; // v_i[i_v[f]] = f

  for (int e = 0; e < domaine.nb_elem(); e++, v_i.clear(), i_v.clear())
    {
      /* stencil : faces de l'element et de ses voisins par som-elem + toutes composantes a ses faces de bord */
      for (auto &&fa : e_s_f[e])
        if (!v_i.count( { { fa, -1 } }))
      v_i[ { { fa, -1 } }] = (int) i_v.size(), i_v.push_back( { { fa, -1 } });
      for (int i = 0; i < e_f.dimension(1) ; i++)
        {
          const int f = e_f(e, i);
          if (f < 0) continue;

          if (fcl_(f, 0))
            for (int d = 0; d < D; d++)
              v_i[ { { f, d } }] = (int) i_v.size(), i_v.push_back( { { f, d } });
        }

      /* coeffs de l'interpolation d'ordre 1, ponderations (comme dans Domaine_PolyMAC_P0::{e,f}grad)  */
      const int nc = (int) i_v.size();

      ve2.resize(nc, D);
      A.resize(nc, nl);
      B.resize(nc);
      P.resize(nc);
      pvt.resize(nc);
      ve2 = 0.;

      for (int i = 0; i < e_f.dimension(1); i++)
        {
          const int f = e_f(e, i);
          if (f < 0) continue;

          fac = (e == f_e(f, 0) ? 1 : -1) * fs(f) / ve(e);
          int j = v_i.at( { { f, -1 } });

          for (int d = 0; d < D; d++)
            ve2(j, d) += fac * (xv(f, d) - xp(e, d));
        }

      for (int i = 0; i < nc; i++)
        {
          int f = i_v[i][0];
          xf = i_v[i][1] < 0 ? &xv(f, 0) : &xfb(f, 0);
          P(i) = 1. / sqrt(domaine.dot(xf, xf, &xp(e, 0), &xp(e, 0)));
        }

      /* par composante : correction pour etre d'ordre 2 */
      for (int d = 0; d < D; d++)
        {
          /* systeme A.x = b */
          B = 0;
          pvt = 0;

          for (int i = 0; i < nc; i++)
            {
              int f = i_v[i][0];
              int db = i_v[i][1];
              xf = db < 0 ? &xv(f, 0) : &xfb(f, 0);

              int jb = 0;
              for (int j = 0; j < D; j++)
                for (int k = 0; k <= D; k++, jb++)
                  {
                    fac = (db < 0 ? nf(f, j) / fs(f) : (db == j)) * (k < D ? xf[k] - xp(e, k) : 1);
                    A(i, jb) = fac * P(i);
                    if (k < D)
                      B(jb) -= fac * ve2(i, d); //erreur de l'interp d'ordre 1 a corriger
                  }
            }

          /* x de norme L2 minimale par dgels */
          int nw = -1, rank;

          F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);

          W.resize(nw = (int) std::lrint(W(0)));

          F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
          assert(infoo == 0);

          /* ajout dans ve2 */
          for (int i = 0; i < nc; i++)
            ve2(i, d) += P(i) * B(i);
        }

      /* implicitation des CLs de Neumann / Symetrie */
      Matrice33 M(1, 0, 0, 0, 1, 0, 0, 0, 1), iM;

      for (int i = 0; i < nc; i++)
        if (i_v[i][1] >= 0 && fcl_(i_v[i][0], 0) < 2)
          for (int j = 0; j < D; j++)
            M(j, i_v[i][1]) -= ve2(i, j);

      Matrice33::inverse(M, iM);
      ve2i.resize(nc, D);

      for (int i = 0; i < nc; i++)
        for (int j = 0; j < D; j++)
          {
            ve2i(i, j) = 0;
            for (int k = 0; k < D; k++)
              ve2i(i, j) += iM(j, k) * ve2(i, k);
          }

      /* stockage */
      for (int d = 0; d < D; d++, ve2d.append_line(ve2c.size(), ve2bc.size()))
        for (int i = 0; i < nc; i++)
          if (std::fabs(ve2i(i, d)) > 1e-6 && (i_v[i][1] < 0 || fcl_(i_v[i][0], 0) == 3))
            {
              i_v[i][1] < 0 ? ve2j.append_line(i_v[i][0]) : ve2bj.append_line(i_v[i][0], i_v[i][1]);
              (i_v[i][1] < 0 ? &ve2c : &ve2bc)->append_line(ve2i(i, d) * pf(i_v[i][0]) / pe(e));
            }
    }

  CRIMP(ve2d);
  CRIMP(ve2j);
  CRIMP(ve2bj);
  CRIMP(ve2c);
  CRIMP(ve2bc);
}

/* met en coherence les composantes aux elements avec les vitesses aux faces : interpole sur phi * v */
void Champ_Face_PolyMAC_P0::update_ve2(DoubleTab& val, int incr) const
{
  const Domaine_PolyMAC_P0& domaine = domaine_PolyMAC_P0();
  if (valeurs().get_md_vector() != domaine.mdv_ch_face)
    return; //pas de variables auxiliaires -> on sort

  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites();
  const int D = dimension,  N = val.line_size(), nf_tot = domaine.nb_faces_tot();

  init_ve2();
  init_fcl();

  int ed = 0, i = nf_tot;

  for (int e = 0 ; e < domaine.nb_elem(); e++)
    for (int d = 0; d < D; d++, ed++, i++)
      for (int n = 0; n < N; n++)
        {
          /* partie "interne" */
          val(i, n) = 0;

          for (int j = ve2d(ed, 0); j < ve2d(ed + 1, 0); j++)
            val(i, n) += ve2c(j) * val(ve2j(j), n);

          /* partie "faces de bord de Dirichlet" (sauf si on fait des increments) */
          if (!incr)
            for (int j = ve2d(ed, 1); j < ve2d(ed + 1, 1); j++)
              if (sub_type(Dirichlet, cls[fcl_(ve2bj(j, 0), 1)].valeur()))
                val(i, n) += ve2bc(j) * ref_cast(Dirichlet, cls[fcl_(ve2bj(j, 0), 1)].valeur()).val_imp(fcl_(ve2bj(j, 0), 2), N * ve2bj(j, 1) + n);
        }

  val.echange_espace_virtuel();
}

DoubleTab& Champ_Face_PolyMAC_P0::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  if (valeurs().get_md_vector() != domaine_PolyMAC_P0().mdv_ch_face)
    return Champ_Face_PolyMAC_P0P1NC::valeur_aux_elems_(valeurs(), positions, les_polys, val_elem);
  else
    return valeur_aux_elems_(le_champ().valeurs(), positions, les_polys, val_elem);
}

DoubleTab& Champ_Face_PolyMAC_P0::valeur_aux_elems_passe(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  if (valeurs().get_md_vector() != domaine_PolyMAC_P0().mdv_ch_face)
    return Champ_Face_PolyMAC_P0P1NC::valeur_aux_elems_(passe(), positions, les_polys, val_elem);
  else
    return valeur_aux_elems_(le_champ().passe(), positions, les_polys, val_elem);
}

DoubleTab& Champ_Face_PolyMAC_P0::valeur_aux_elems_(const DoubleTab& val_face, const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  const Domaine_PolyMAC_P0& domaine = domaine_PolyMAC_P0();
  int nb_compo = le_champ().nb_comp(), nf_tot = domaine.nb_faces_tot(), d, D = dimension, n, N = val_face.line_size();

  assert((positions.dimension(0) == les_polys.size()) || (positions.dimension_tot(0) == les_polys.size()));

  if (val_elem.nb_dim() > 2)
    Process::exit("TRUST error in Champ_Face_PolyMAC_P0::valeur_aux_elems_ : The DoubleTab val has more than 2 entries !");

  if (nb_compo == 1)
    Process::exit("TRUST error in Champ_Face_PolyMAC_P0::valeur_aux_elems_ : A scalar field cannot be of Champ_Face type !");

  for (int p = 0, e; p < les_polys.size(); p++)
    for (e = les_polys(p), d = 0; e < domaine.nb_elem() && d < D; d++)
      for (n = 0; n < N; n++)
        val_elem(p, N * d + n) = val_face(nf_tot + D * e + d, n);
  return val_elem;
}

DoubleVect& Champ_Face_PolyMAC_P0::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& val, int ncomp) const
{
  if (valeurs().get_md_vector() != domaine_PolyMAC_P0().mdv_ch_face)
    return Champ_Face_PolyMAC_P0P1NC::valeur_aux_elems_compo(positions, polys, val, ncomp);
  int nf_tot = domaine_PolyMAC_P0().nb_faces_tot(), D = dimension, N = valeurs().line_size();
  assert(val.size_totale() >= polys.size());

  DoubleTab vfe(valeurs());
  update_ve(vfe);

  for (int p = 0, e; p < polys.size(); p++)
    val(p) = (e = polys(p)) < 0 ? 0. : vfe.addr()[N * (nf_tot + D * e) + ncomp];

  return val;
}

DoubleTab& Champ_Face_PolyMAC_P0::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant == 0);
  const Domaine_PolyMAC_P0& domaine = domaine_PolyMAC_P0();
  if (valeurs().get_md_vector() != domaine.mdv_ch_face)
    return Champ_Face_PolyMAC_P0P1NC::trace(fr, x, t, distant);

  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1, ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot();
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const IntTab& face_voisins = domaine.face_voisins();
  const DoubleTab& val = valeurs(t);

  for (int i = 0; i < fr_vf.nb_faces(); i++)
    {
      const int face = fr_vf.num_premiere_face() + i, elem = face_voisins(face, 0);
      for (int d = 0; d < dim; d++)
        x(i, d) = val(vectoriel ? nf_tot + ne_tot * d + elem : face);
    }

  return x;
}
