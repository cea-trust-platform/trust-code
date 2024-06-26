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

#include <Op_Conv_EF_Stab_PolyVEF_Face.h>
#include <Pb_Multiphase.h>
#include <Schema_Temps_base.h>
#include <Domaine_Poly_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_Face_PolyVEF.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Param.h>
#include <cmath>
#include <Masse_ajoutee_base.h>
#include <Synonyme_info.h>
#include <Option_PolyVEF.h>

Implemente_instanciable( Op_Conv_EF_Stab_PolyVEF_Face, "Op_Conv_EF_Stab_PolyVEF_P0_Face|Op_Conv_EF_Stab_PolyVEF_P0P1_Face", Op_Conv_EF_Stab_PolyMAC_P0_Face );
Implemente_instanciable_sans_constructeur(Op_Conv_Amont_PolyVEF_Face, "Op_Conv_Amont_PolyVEF_P0_Face|Op_Conv_Amont_PolyVEF_P0P1_Face", Op_Conv_EF_Stab_PolyVEF_Face);
Implemente_instanciable_sans_constructeur(Op_Conv_Centre_PolyVEF_Face, "Op_Conv_Centre_PolyVEF_P0_Face|Op_Conv_Centre_PolyVEF_P0P1_Face", Op_Conv_EF_Stab_PolyVEF_Face);
Add_synonym(Op_Conv_EF_Stab_PolyVEF_Face, "Op_Conv_EF_Stab_PolyVEF_P0P1NC_Face");
Add_synonym(Op_Conv_Amont_PolyVEF_Face, "Op_Conv_Amont_PolyVEF_P0P1NC_Face");
Add_synonym(Op_Conv_Centre_PolyVEF_Face, "Op_Conv_Centre_PolyVEF_P0P1NC_Face");

Op_Conv_Amont_PolyVEF_Face::Op_Conv_Amont_PolyVEF_Face() { alpha = 1.0; }
Op_Conv_Centre_PolyVEF_Face::Op_Conv_Centre_PolyVEF_Face() { alpha = 0.0; }

// XD Op_Conv_EF_Stab_PolyVEF_Face interprete Op_Conv_EF_Stab_PolyVEF_Face 1 Class Op_Conv_EF_Stab_PolyVEF_Face
Sortie& Op_Conv_EF_Stab_PolyVEF_Face::printOn(Sortie& os) const { return Op_Conv_EF_Stab_PolyMAC_P0_Face::printOn(os); }
Sortie& Op_Conv_Amont_PolyVEF_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Centre_PolyVEF_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Entree& Op_Conv_Amont_PolyVEF_Face::readOn(Entree& is) { return Op_Conv_PolyMAC_base::readOn(is); }
Entree& Op_Conv_Centre_PolyVEF_Face::readOn(Entree& is) { return Op_Conv_PolyMAC_base::readOn(is); }

Entree& Op_Conv_EF_Stab_PolyVEF_Face::readOn( Entree& is )
{
  Op_Conv_PolyMAC_base::readOn( is );
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Conv_EF_Stab_PolyVEF_Face::completer()
{
  Op_Conv_PolyMAC_base::completer();
  /* au cas ou... */
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  if (dom.nb_joints() && dom.joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Conv_EF_Stab_PolyVEF_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  porosite_f.ref(equation().milieu().porosite_face());
  porosite_e.ref(equation().milieu().porosite_elem());

  /* construction de e_fa_d / e_fa_f / e_fa_c */
  const IntTab& f_e = dom.face_voisins(), &e_f = dom.elem_faces(), &f_s = dom.face_sommets();
  const DoubleTab& vfd = dom.volumes_entrelaces_dir();
  const DoubleVect& ve = dom.volumes();
  int i, j, k, e, f, s, nw, infoo = 0, rk = 0, skip, n_f, D = dimension, sgn;
  DoubleTrav A, B, W;
  IntTrav piv;
  std::map<std::array<int, 2>, std::array<int, 2>> a_f; //faces connectees a chaque arete : on stocke les indices de face + 1
  for (e = 0, e_fa_d.resize(1, 2), e_fa_f.resize(0, 2); e < dom.nb_elem_tot(); e_fa_d.append_line(e_fa_f.dimension(0), e_fa_c.dimension(0)), e++)
    {
      /* on peut sauter les elements qui n'ont pas au moins une face reelle */
      for (skip = 1, i = 0, n_f = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) skip &= f >= dom.nb_faces(), n_f++;
      if (skip) continue;

      /* recherche des faces connectees et stockage dans e_fa_{d,f} */
      for (a_f.clear(), i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
          {
            int sb = D < 3 ? -1 : f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0);
            auto& p = a_f[ {{ std::min(s, sb), std::max(s, sb)}}];
            p[p[0] > 0] = f + 1;
          }
      for (auto &&kv : a_f)
        e_fa_f.append_line(kv.second[0] - 1, kv.second[1] - 1);

      /* calcul des flux aux facettes en fonction des flux aux faces de l'element : systeme A.X = B */
      int n_a = (int) a_f.size(), n_m = std::max(n_f, n_a);
      A.resize(n_a, n_f), B.resize(n_f, n_m);
      for (i = 0, j = e_fa_d(e, 0); i < n_a; i++, j++)
        for (k = 0; k < n_f; k++)
          f = e_f(e, k), A(i, k) = (f == e_fa_f(j, 0)) - (f == e_fa_f(j, 1));
      for (B = 0, i = 0; i < n_f; i++)
        for (f = e_f(e, i), sgn = e == f_e(f, 0) ? 1 : -1, j = 0; j < n_f; j++)
          B(i, j) = (vfd(f, e != f_e(f, 0)) / ve(e) - (i == j)) * sgn;
      //resolution : DGELSY (systeme sous-determine)
      double eps = 1e-8;
      nw = -1, piv.resize(n_a), F77NAME(dgelsy)(&n_f, &n_a, &n_f, &A(0, 0), &n_f, &B(0, 0), &n_m, &piv(0), &eps, &rk, &W(0), &nw, &infoo);
      W.resize(nw = (int)std::lrint(W(0))), piv = 0, F77NAME(dgelsy)(&n_f, &n_a, &n_f, &A(0, 0), &n_f, &B(0, 0), &n_m, &piv(0), &eps, &rk, &W(0), &nw, &infoo);
      //stockage des n_a x n_f coefficients trouves dans e_fa_c
      for (i = 0; i < n_a; i++)
        for (j = 0; j < n_f; j++)
          e_fa_c.append_line(B(j, i));
    }
  CRIMP(e_fa_d), CRIMP(e_fa_f), CRIMP(e_fa_c);
}

double Op_Conv_EF_Stab_PolyVEF_Face::calculer_dt_stab() const
{
  double dt = 1e10;
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces();
  const DoubleTab& vit = vitesse_->valeurs(), &vfd = dom.volumes_entrelaces_dir(), &nf = dom.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const IntTab& e_f = dom.elem_faces(), &f_e = dom.face_voisins(), &fcl = ch.fcl();
  int i, j, k, e, f, d, D = dimension, n, N = vit.line_size() / D;
  DoubleTrav flux(N), fsum(dom.nb_faces(), N), a_f(N); //flux, somme des flux par face, tx de vide par face

  /* on accumule la somme des flux entrants a chaque face */
  for (e = 0; e < dom.nb_elem_tot(); e++)
    for (i = e_fa_d(e, 0), j = e_fa_d(e, 1); i < e_fa_d(e + 1, 0); i++)
      {
        /* flux a la facette */
        for (flux = 0, k = 0; k < e_f.dimension(1) && (f = e_f(e, k)) >= 0; j++, k++)
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              flux(n) += e_fa_c(j) * pf(f) * vit(f, N * d + n) * nf(f, d);
        /* contribution a la face vers laquelle le flux va */
        for (n = 0; n < N; n++)
          if ((f = e_fa_f(i, flux(n) > 0)) < dom.nb_faces())
            fsum(f, n) += std::abs(flux(n));
      }
  /* dt sur ce proc */
  for (f = 0; f < dom.nb_faces(); f++)
    if (fcl(f, 0) < (Option_PolyVEF::sym_as_diri ? 2 : 3))
      {
        for (a_f = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          for (n = 0; n < N; n++)
            a_f(n) += vfd(f, i) * (alp ? (*alp)(e, n) : 1) / vf(f);
        for (n = 0; n < N; n++)
          if (a_f(n) > 1e-3 && fsum(f, n))
            dt = std::min(dt, pf(f) * vf(f) / fsum(f, n));
      }
  return Process::mp_min(dt);
}

void Op_Conv_EF_Stab_PolyVEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //pas de bloc diagonal ou semi-implicite -> rien a faire
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, f, fb, nf_tot = dom.nb_faces_tot(), m, d, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D;

  IntTab stencil(0, 2);
  /* stencil : toutes les faces connectees par une facette de e_fa_f, avec melange des phases si correlation de masse ajoutee */
  for (i = 0; i < e_fa_f.dimension(0); i++)
    for (j = 0; j < 2; j++)
      if ((f = e_fa_f(i, j)) < dom.nb_faces())
        for (fb = e_fa_f(i, !j), d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)
              stencil.append_line(N * (D * f + d) + n, N * (D * fb + d) + m);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * D * nf_tot, N * D * nf_tot, stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_PolyVEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const IntTab& f_e = dom.face_voisins(), &e_f = dom.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& vit = ch.passe(), &nf = dom.face_normales();
  const DoubleVect& pf = porosite_f;

  /* a_r : produit alpha_rho si Pb_Multiphase -> par semi_implicite, ou en recuperant le champ_conserve de l'equation de masse */
  const std::string& nom_inco = ch.le_nom().getString();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs(),
                   *a_r = !pbm ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") : &pbm->equation_masse().champ_conserve().valeurs(),
                    *alp = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr, &rho = equation().milieu().masse_volumique().passe(),
                     a_b = pbm ? pbm->equation_masse().inconnue().valeur_aux_bords() : DoubleTab(), r_b = pbm ? equation().milieu().masse_volumique().valeur_aux_bords() : DoubleTab();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  int i, j, k, l, e, f, fb, d, D = dimension, m, n, N = inco.line_size() / D;

  DoubleTrav F_f(0, N), F_fa(N), masse(N, N), fvf(N); //flux de masse a toutes les faces et a la facette (avec masse ajoutee)
  dom.creer_tableau_faces(F_f);
  for (f = 0; f < dom.nb_faces(); f++)
    {
      /* produit |f|n_f.v_f par composante */
      for (fvf = 0, d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          fvf(n) += nf(f, d) * vit(f, N * d + n);
      /* alpha*rho a la face avec decentrement */
      for (i = 0; i < 2; i++)
        for (e = f_e(f, i), n = 0; n < N; n++)
          F_f(f, n) += (!a_r ? 1 : e >= 0 ? (*a_r)(e, n) : a_b(f, n) * r_b(f, n)) *  (1 + (fvf(n) * (i ? -1 : 1) > 0 ? 1 : fvf(n) ? -1 : 0) * alpha) / 2;
      /* produit par pf * fvf */
      for (n = 0; n < N; n++)
        F_f(f, n) *= pf(f) * fvf(n);
      /* faces de bord: contrib a la convection (avec masse ajoutee) si Neumann */
      if (fcl(f, 0) == 1)
        {
          for (masse = 0, e = f_e(f, 0), n = 0; n < N; n++) masse(n, n) = a_r ? (*a_r)(e, n) : 1;
          if (corr) corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              for (m = corr ? 0 : n; m < (corr ? N : n + 1); m++)
                if (F_f(f, m) < 0)
                  {
                    secmem(f, N * d + n) += (masse(n, m) ? masse(n, m) / (a_r ? (*a_r)(e, m) : 1) : 0) * F_f(f, m) * inco(f, N * d + m);
                    if (mat) (*mat)(N * (D * f + d) + n, N * (D * f + d) + m) -= (masse(n, m) ? masse(n, m) / (a_r ? (*a_r)(e, m) : 1) : 0) * F_f(f, m);
                  }
        }
    }
  F_f.echange_espace_virtuel();

  /* contribution des facettes dans chaque element + contrib a la face si bord */
  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      for (masse = 0, n = 0; n < N; n++) masse(n, n) = a_r ? (*a_r)(e, n) : 1;
      if (corr) corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);

      /* boucle sur les facettes de e */
      for (i = e_fa_d(e, 0), j = e_fa_d(e, 1); i < e_fa_d(e + 1, 0); i++)
        {
          /* flux de masse a la facette */
          for (F_fa = 0, k = 0; k < e_f.dimension(1) && (f = e_f(e, k)) >= 0; j++, k++)
            for (n = 0; n < N; n++)
              F_fa(n) += e_fa_c(j) * F_f(f, n);
          for (k = 0; k < 2; k++)
            if ((f = e_fa_f(i, k)) < dom.nb_faces()) /* face d'arrivee */
              for (l = 0; l < 2; l++)
                for (fb = e_fa_f(i, l), d = 0; d < D; d++)
                  for (n = 0; n < N; n++)
                    for (m = corr ? 0 : n; m < (corr ? N : n + 1); m++)
                      {
                        double fac = (k ? -1 : 1) * (masse(n, m) ? masse(n, m) / (a_r ? (*a_r)(e, m) : 1) : 0) * F_fa(m) * ((1 + (F_fa(m) * (l ? -1 : 1) > 0 ? 1 : F_fa(m) ? -1 : 0) * alpha) / 2 - (fb == f));
                        secmem(f, N * d + n) -= fac * inco(fb, N * d + m);
                        if (mat)
                          (*mat)(N * (D * f + d) + n, N * (D * fb + d) + m) += fac;
                      }
        }
    }
}
