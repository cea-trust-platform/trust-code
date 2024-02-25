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

#include <Op_Diff_PolyVEF_Face.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Champ_Face_PolyVEF.h>
#include <Champ_Uniforme.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Synonyme_info.h>
#include <MD_Vector_base.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Pb_Multiphase.h>
#include <Option_PolyVEF.h>

Implemente_instanciable( Op_Diff_PolyVEF_Face, "Op_Diff_PolyVEF_P0_Face|Op_Dift_PolyVEF_Face_PolyVEF_P0", Op_Diff_PolyVEF_base ) ;
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Diff_PolyVEF_P0_var_Face");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Dift_PolyVEF_P0_var_Face_PolyVEF_P0");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Diff_PolyVEF_P0P1_Face");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Dift_PolyVEF_Face_PolyVEF_P0P1");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Diff_PolyVEF_P0P1_var_Face");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Dift_PolyVEF_P0P1_var_Face_PolyVEF_P0P1");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Diff_PolyVEF_P0P1NC_Face");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Dift_PolyVEF_Face_PolyVEF_P0P1NC");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Diff_PolyVEF_P0P1NC_var_Face");
Add_synonym(Op_Diff_PolyVEF_Face, "Op_Dift_PolyVEF_P0P1NC_var_Face_PolyVEF_P0P1NC");

Sortie& Op_Diff_PolyVEF_Face::printOn( Sortie& os ) const
{
  Op_Diff_PolyVEF_base::printOn( os );
  return os;
}

Entree& Op_Diff_PolyVEF_Face::readOn( Entree& is )
{
  Op_Diff_PolyVEF_base::readOn( is );
  return is;
}

void Op_Diff_PolyVEF_Face::completer()
{
  Op_Diff_PolyVEF_base::completer();
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue());
  if (le_champ_inco.non_nul()) ch.init_auxiliary_variables(); // cas flica5 : ce n'est pas l'inconnue qui est utilisee, donc on cree les variables auxiliaires ici
  flux_bords_.resize(dom.premiere_face_int(), ch.valeurs().line_size());
  if (dom.nb_joints() && dom.joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyVEF_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  porosite_e.ref(equation().milieu().porosite_elem());
  porosite_f.ref(equation().milieu().porosite_face());
}

double Op_Diff_PolyVEF_Face::calculer_dt_stab() const
{
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  const IntTab& e_f = dom.elem_faces(), &f_e = dom.face_voisins(), &fcl = ref_cast(Champ_Face_PolyVEF, equation().inconnue()).fcl();
  const DoubleTab& vfd = dom.volumes_entrelaces_dir(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr,
                    *a_r = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().champ_conserve().passe() : (has_champ_masse_volumique() ? &get_champ_masse_volumique().valeurs() : nullptr); /* produit alpha * rho */
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces();
  update_nu();

  int i, j, e, f, fb, n, N = equation().inconnue().valeurs().line_size() / dimension, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom), skip;
  double dt = 1e10;
  DoubleTrav flux(dom.nb_faces(), N), vol(N), a_f(N), w2, lw2, tw2(N); //matrice w2, sommes par ligne et totale

  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      //si e n'a aucune face reelle, rien a faire
      for (skip = (e >= dom.nb_elem()), i = 0; skip && i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) skip &= f >= dom.nb_faces();
      if (skip) continue;

      /* matrice w2, sommes de ses lignes et somme totale */
      for (dom.W2(&nu_, e, w2), lw2.resize(w2.dimension(0), N), lw2 = 0, tw2 = 0, i = 0; i < w2.dimension(0); i++)
        for (j = 0; j < w2.dimension(0); j++)
          for (n = 0; n < N; n++)
            lw2(i, n) += w2(i, j, n), tw2(n) += w2(i, j, n);

      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        if (f < dom.nb_faces() && (!p0p1 || fcl(f, 0) < 2))
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
            if (fcl(fb, 0) != 2)
              for (n = 0; n < N; n++)
                flux(f, n) += std::abs(w2(i, j, n) - lw2(i, n) * lw2(j, n) / tw2(n));
    }
  for (f = 0; f < dom.nb_faces(); f++)
    if (fcl(f, 0) < (Option_PolyVEF::sym_as_diri ? 2 : 3))
      {
        for (vol = 0, a_f = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          for (n = 0; n < N; n++)
            vol(n) += pf(f) * vfd(f, i) * (a_r ? (*a_r)(e, n) : 1), a_f(n) += vfd(f, i) / vf(f) * (alp ? (*alp)(e, n) : 1);
        for (n = 0; n < N; n++)
          if (a_f(n) > 0.25 && flux(f, n)) /* sous 0.25, on suppose que les forces stabilisent */
            dt = std::min(dt, vol(n) / flux(f, n));
      }
  return Process::mp_min(dt);
}

void Op_Diff_PolyVEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  const IntTab& f_e = dom.face_voisins(), &e_f = dom.elem_faces();

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //semi-implicite ou pas de bloc diagonal -> rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, e, f, fb, nd, ND = equation().inconnue().valeurs().line_size(), nf_tot = dom.nb_faces_tot();

  IntTrav stencil(0, 2);

  /* stencil : tous les voisins de la face par un element, sans melanger les dimensions ou les composantes */
  for (f = 0; f < dom.nb_faces(); f++)
    for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
        for (nd = 0; nd < ND; nd++)
          stencil.append_line(ND * f + nd, ND * fb + nd);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(ND * nf_tot, ND * nf_tot, stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Diff_PolyVEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices[nom_inco] : nullptr; //facultatif
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : le_champ_inco.non_nul() ? le_champ_inco->valeurs() : equation().inconnue().valeurs();
  const Domaine_PolyMAC& dom = le_dom_poly_.valeur();
  const IntTab& e_f = dom.elem_faces(), &fcl = ref_cast(Champ_Face_PolyVEF, equation().inconnue()).fcl();
  int i, j, e, f, fb, d, D = dimension, nd, ND = inco.line_size(), n, N = ND / D, skip;

  DoubleTrav w2, lw2, tw2(N); //matrice w2, sommes par ligne et totale

  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      //si e n'a aucune face reelle, rien a faire
      for (skip = (e >= dom.nb_elem()), i = 0; skip && i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) skip &= f >= dom.nb_faces();
      if (skip) continue;

      /* matrice w2, sommes de ses lignes et somme totale */
      for (dom.W2(&nu_, e, w2), lw2.resize(w2.dimension(0), N), lw2 = 0, tw2 = 0, i = 0; i < w2.dimension(0); i++)
        for (j = 0; j < w2.dimension(0); j++)
          for (n = 0; n < N; n++)
            lw2(i, n) += w2(i, j, n), tw2(n) += w2(i, j, n);

      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        if (f < dom.nb_faces())
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
            if (!Option_PolyVEF::sym_as_diri || fcl(fb, 0) != 2)
              for (d = 0, nd = 0; d < D; d++)
                for (n = 0; n < N; n++, nd++)
                  {
                    double fac = w2(i, j, n) - lw2(i, n) * lw2(j, n) / tw2(n);
                    secmem(f, nd) -= fac * inco(fb, nd);
                    if (mat)
                      (*mat)(ND * f + nd, ND * fb + nd) += fac;
                  }
    }
  i++;
}
