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

#include <Op_Conv_Classic_PolyVEF_Face.h>
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

Implemente_instanciable( Op_Conv_Classic_PolyVEF_Face, "Op_Conv_Classic_PolyVEF_P0_Face|Op_Conv_Classic_PolyVEF_P0P1_Face", Op_Conv_EF_Stab_PolyMAC_P0_Face );
Implemente_instanciable_sans_constructeur(Op_Conv_Classic_Amont_PolyVEF_Face, "Op_Conv_Classic_Amont_PolyVEF_P0_Face|Op_Conv_Classic_Amont_PolyVEF_P0P1_Face", Op_Conv_Classic_PolyVEF_Face);
Implemente_instanciable_sans_constructeur(Op_Conv_Classic_Centre_PolyVEF_Face, "Op_Conv_Classic_Centre_PolyVEF_P0_Face|Op_Conv_Classic_Centre_PolyVEF_P0P1_Face", Op_Conv_Classic_PolyVEF_Face);
Add_synonym(Op_Conv_Classic_PolyVEF_Face, "Op_Conv_Classic_PolyVEF_P0P1NC_Face");
Add_synonym(Op_Conv_Classic_Amont_PolyVEF_Face, "Op_Conv_Classic_Amont_PolyVEF_P0P1NC_Face");
Add_synonym(Op_Conv_Classic_Centre_PolyVEF_Face, "Op_Conv_Classic_Centre_PolyVEF_P0P1NC_Face");

Op_Conv_Classic_Amont_PolyVEF_Face::Op_Conv_Classic_Amont_PolyVEF_Face() { alpha = 1.0; }
Op_Conv_Classic_Centre_PolyVEF_Face::Op_Conv_Classic_Centre_PolyVEF_Face() { alpha = 0.0; }

// XD Op_Conv_Classic_PolyVEF_Face interprete Op_Conv_Classic_PolyVEF_Face 1 Class Op_Conv_Classic_PolyVEF_Face
Sortie& Op_Conv_Classic_PolyVEF_Face::printOn(Sortie& os) const { return Op_Conv_EF_Stab_PolyMAC_P0_Face::printOn(os); }
Sortie& Op_Conv_Classic_Amont_PolyVEF_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Classic_Centre_PolyVEF_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Entree& Op_Conv_Classic_Amont_PolyVEF_Face::readOn(Entree& is) { return Op_Conv_PolyMAC_base::readOn(is); }
Entree& Op_Conv_Classic_Centre_PolyVEF_Face::readOn(Entree& is) { return Op_Conv_PolyMAC_base::readOn(is); }

Entree& Op_Conv_Classic_PolyVEF_Face::readOn( Entree& is )
{
  Op_Conv_PolyMAC_base::readOn( is );
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema Classic_Centre) a 1 (schema Classic_Amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Conv_Classic_PolyVEF_Face::completer()
{
  Op_Conv_PolyMAC_base::completer();
  /* au cas ou... */
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  if (dom.nb_joints() && dom.joint(0).epaisseur() < 2)
    {
      Cerr << "Op_Conv_Classic_PolyVEF_Face : largeur de joint insuffisante (minimum 2)!" << finl;
      Process::exit();
    }
  porosite_f.ref(equation().milieu().porosite_face());
  porosite_e.ref(equation().milieu().porosite_elem());

  /* construction de e_fa_d / e_fa_f / e_fa_c */
  const IntTab& e_f = dom.elem_faces(), &f_s = dom.face_sommets();
  const DoubleTab& xp = dom.xp(), &xv = dom.xv(), &xs = dom.domaine().coord_sommets();
  int i, j, e, f, s, n_f, D = dimension, skip;
  double vz[3] = { 0, 0, 1 };
  e_fa_d.set_smart_resize(1), e_fa_f.set_smart_resize(1), e_fa_s.set_smart_resize(1);
  std::map<std::array<int, 2>, std::array<int, 2>> a_f; //faces connectees a chaque arete : on stocke les indices de face + 1
  for (e = 0, e_fa_d.resize(1), e_fa_f.resize(0, 2), e_fa_s.resize(0, D); e < dom.nb_elem_tot(); e_fa_d.append_line(e_fa_f.dimension(0)), e++)
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
        {
          int s1 = kv.first[0], s2 = kv.first[1], f1 = kv.second[0] - 1, f2 = kv.second[1] - 1;
          e_fa_f.append_line(f1, f2);
          auto v = dom.cross(D, 3, &xs(s2, 0), D < 3 ? vz : &xs(s1, 0), &xp(e, 0), D < 3 ? NULL : &xp(e, 0));
          double fac = (D < 3 ? 1 : 0.5) * (dom.dot(&xv(f2, 0), &v[0], &xv(f1, 0)) > 0 ? 1 : -1);
          D < 3 ? e_fa_s.append_line(fac * v[0], fac * v[1]) : e_fa_s.append_line(fac * v[0], fac * v[1], fac * v[2]);
        }
    }
  CRIMP(e_fa_d), CRIMP(e_fa_f), CRIMP(e_fa_s);
}

double Op_Conv_Classic_PolyVEF_Face::calculer_dt_stab() const
{
  double dt = 1e10;
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue().valeur());
  const DoubleVect& pe = porosite_e, &pf = porosite_f, &vf = dom.volumes_entrelaces();
  const DoubleTab& vit = vitesse_->valeurs(), &vfd = dom.volumes_entrelaces_dir(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : NULL;
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl();
  int i, j, e, f, d, D = dimension, n, N = vit.line_size() / D;
  DoubleTrav flux(N), fsum(dom.nb_faces(), N), a_f(N); //flux, somme des flux par face, tx de vide par face

  /* on accumule la somme des flux entrants a chaque face */
  for (e = 0; e < dom.nb_elem_tot(); e++)
    for (i = e_fa_d(e); i < e_fa_d(e + 1); i++)
      {
        /* flux a la facette */
        for (flux = 0, j = 0; j < 2; j++)
          for (f = e_fa_f(i, j), d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              flux(n) += 0.5 * vit(f, N * d + n) * pe(e) * e_fa_s(i, d);
        /* contribution a la face vers laquelle le flux va */
        for (n = 0; n < N; n++)
          if ((f = e_fa_f(i, flux(n) > 0)) < dom.nb_faces())
            fsum(f, n) += std::abs(flux(n));
      }
  /* dt sur ce proc */
  for (f = 0; f < dom.nb_faces(); f++)
    if (fcl(f, 0) < 2)
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

void Op_Conv_Classic_PolyVEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue().valeur());
  const IntTab& fcl = ch.fcl();
  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //pas de bloc diagonal ou semi-implicite -> rien a faire
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, f, fb, nf_tot = dom.nb_faces_tot(), m, d, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  /* stencil : toutes les faces connectees par une facette de e_fa_f, avec melange des phases si correlation de masse ajoutee */
  for (i = 0; i < e_fa_f.dimension(0); i++)
    for (j = 0; j < 2; j++)
      if ((f = e_fa_f(i, j)) < dom.nb_faces())
        for (fb = e_fa_f(i, !j), d = 0; d < D; d++)
          if (!p0p1 || (fcl(f, 0) < 2 && fcl(fb, 0) < 2))
            for (n = 0; n < N; n++)
              for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)
                stencil.append_line(N * (D * f + d) + n, N * (D * fb + d) + m);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * D * nf_tot, N * D * nf_tot, stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_Classic_PolyVEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& dom = le_dom_poly_.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue().valeur());
  const IntTab& fcl = ch.fcl();
  const DoubleVect& pe = porosite_e;

  /* a_r : produit alpha_rho si Pb_Multiphase -> par semi_implicite, ou en recuperant le champ_conserve de l'equation de masse */
  const std::string& nom_inco = ch.le_nom().getString();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs(), &vit = ch.passe(),
                   *a_r = !pbm ? NULL : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") : &pbm->equation_masse().champ_conserve().valeurs(),
                    *alp = pbm ? &pbm->equation_masse().inconnue().passe() : NULL, &rho = equation().milieu().masse_volumique().passe(),
                     a_b = pbm ? pbm->equation_masse().inconnue()->valeur_aux_bords() : DoubleTab(), r_b = pbm ? equation().milieu().masse_volumique()->valeur_aux_bords() : DoubleTab();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices.at(nom_inco) : NULL;

  int i, j, k, e, f, fb, d, D = dimension, m, n, N = inco.line_size() / D, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);
  DoubleTrav masse(N, N), flux(N); //flux de masse a toutes les faces et a la facette (avec masse ajoutee)

  /* contribution des facettes dans chaque element + contrib a la face si bord */
  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      for (masse = 0, n = 0; n < N; n++) masse(n, n) = a_r ? (*a_r)(e, n) : 1;
      if (corr) corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);

      /* boucle sur les facettes de e */
      for (i = e_fa_d(e); i < e_fa_d(e + 1); i++)
        {
          /* flux a la facette */
          for (flux = 0, j = 0; j < 2; j++)
            for (f = e_fa_f(i, j), d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                flux(n) += 0.5 * vit(f, N * d + n) * pe(e) * e_fa_s(i, d);

          /* contributions */
          for (j = 0; j < 2; j++)
            if ((f = e_fa_f(i, j)) < dom.nb_faces() && (!p0p1 || fcl(f, 0) < 2)) /* face d'arrivee */
              for (k = 0; k < 2; k++)
                for (fb = e_fa_f(i, k), d = 0; d < D; d++)
                  for (n = 0; n < N; n++)
                    for (m = corr ? 0 : n; m < (corr ? N : n + 1); m++)
                      {
                        double fac = (j ? -1 : 1) * masse(n, m) * flux(m) * ((1 + (flux(m) * (k ? -1 : 1) > 0 ? 1 : flux(m) ? -1 : 0) * alpha) / 2 - (fb == f));
                        secmem(f, N * d + n) -= fac * inco(fb, N * d + m);
                        if (mat && (!p0p1 || fcl(fb, 0) < 2))
                          (*mat)(N * (D * f + d) + n, N * (D * fb + d) + m) += fac;
                      }
        }
    }
}
