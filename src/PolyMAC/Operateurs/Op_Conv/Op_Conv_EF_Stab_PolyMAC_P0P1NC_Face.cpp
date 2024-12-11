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

#include <Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Dirichlet_homogene.h>
#include <Masse_ajoutee_base.h>
#include <Schema_Temps_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_Poly_base.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Param.h>
#include <cmath>

extern Stat_Counter_Id convection_counter_;

Implemente_instanciable( Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face, "Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face", Op_Conv_EF_Stab_PolyMAC_Face );
Implemente_instanciable( Op_Conv_Amont_PolyMAC_P0P1NC_Face, "Op_Conv_Amont_PolyMAC_P0P1NC_Face", Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face );
Implemente_instanciable( Op_Conv_Centre_PolyMAC_P0P1NC_Face, "Op_Conv_Centre_PolyMAC_P0P1NC_Face", Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face );

// XD Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face interprete Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face 1 Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face

Sortie& Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Amont_PolyMAC_P0P1NC_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Centre_PolyMAC_P0P1NC_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }

Entree& Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face::readOn(Entree& is) { return Op_Conv_EF_Stab_PolyMAC_Face::readOn(is); }

Entree& Op_Conv_Amont_PolyMAC_P0P1NC_Face::readOn(Entree& is)
{
  alpha_ = 1.0;
  return Op_Conv_PolyMAC_base::readOn(is);
}

Entree& Op_Conv_Centre_PolyMAC_P0P1NC_Face::readOn(Entree& is)
{
  alpha_ = 0.0;
  return Op_Conv_PolyMAC_base::readOn(is);
}

double Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face::calculer_dt_stab() const
{
  double dt = 1e10;
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
//  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue());
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face(), &ve = domaine.volumes(), &pe = equation().milieu().porosite_elem(); // &vf = domaine.volumes_entrelaces();
  const DoubleTab& vit = vitesse_->valeurs(), // &vfd = domaine.volumes_entrelaces_dir(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(); //, &fcl = ch.fcl();
  int i, e, f, n, N = vit.line_size();
  DoubleTrav flux(N), vol(N); //somme des flux pf * |f| * vf, volume minimal des mailles d'elements/faces affectes par ce flux

  for (e = 0; e < domaine.nb_elem(); e++)
    {
      // Calcul du volume effectif de l'élément
      vol = pe(e) * ve(e);
      flux = 0.;

      // Parcourt des faces associées à l'élément
      for (i = 0; i < e_f.dimension(1); i++)
        {
          f = e_f(e, i);
          if (f >= 0)
            for (n = 0; n < N; n++)
              {
                // Ajout du flux entrant pour la composante n
                double flux_f = pf(f) * fs(f) * (e == f_e(f, 1) ? 1 : -1) * vit(f, n);
                flux(n) += flux_f;  // Seuls les flux entrants comptent

//                  if (false && fcl(f, 0) < 2)
//                    vol(n) = std::min(vol(n), pf(f) * vf(f) * vf(f) / vfd(f, e != f_e(f, 0))); //prise en compte de la contribution aux faces
              }
        }

      // Calcul du pas de temps pour chaque composante n
      for (n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 1e-3) && std::abs(flux(n)) > 1e-12)
          dt = std::min(dt, vol(n) / flux(n));
    }

  return Process::mp_min(dt);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl(), &equiv = domaine.equiv();
  const DoubleTab& nf = domaine.face_normales(), &inco = ch.valeurs(), &xp = domaine.xp(), &xv = domaine.xv();
  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes();

  const std::string& nom_inco = ch.le_nom().getString();

  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco))
    return; //pas de bloc diagonal ou semi-implicite -> rien a faire

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, k, l, e, eb, f, fb, fc, m, n, N = equation().inconnue().valeurs().line_size();

  IntTab stencil(0, 2);

  /* Ce bloc agit uniquement aux éléments; la diagonale de la matrice est omise. */
  for (f = 0; f < domaine.nb_faces_tot(); f++)
    {
      // Vérifie si la face est interne ou satisfait les conditions aux limites
      if (f_e(f, 0) >= 0 && (f_e(f, 1) >= 0 || fcl(f, 0) == 3))
        {
          // Parcourt les éléments associés à cette face
          for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
              {
                // Parcourt les faces connectées à l'élément courant
                for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++)
                  {
                    if (fb < domaine.nb_faces())
                      {
                        // Cas où une équivalence entre faces existe
                        if ((fc = equiv(f, i, k)) >= 0)
                          {
                            for (n = 0; n < N; n++)
                              for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)
                                stencil.append_line(N * fb + n, N * fc + m);
                          }
                        // Cas sans équivalence : contributions entre faces de l'élément
                        else if (f_e(f, 1) >= 0)
                          {
                            for (l = 0; l < e_f.dimension(1) && (fc = e_f(eb, l)) >= 0; l++)
                              {
                                double critère = fs(fc) * domaine.dot(&xv(fc, 0), &nf(fb, 0), &xp(eb, 0));
                                if (std::abs(critère) > 1e-6 * ve(eb) * fs(fb))
                                  for (n = 0; n < N; n++)
                                    for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)
                                      stencil.append_line(N * fb + n, N * fc + m);
                              }
                          }
                      }
                  }
              }
        }
    }

  // Trie et retire les doublons dans le stencil
  tableau_trier_retirer_doublons(stencil);

  // Alloue une matrice clairsemée basée sur le stencil
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);

  // Ajoute mat2 à la matrice existante ou initialise `mat`
  if (mat.nb_colonnes())
    mat += mat2;
  else
    mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(convection_counter_);
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue());
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl(), &equiv = domaine.equiv();
  const DoubleTab& vit = ch.passe(), &nf = domaine.face_normales(), &vfd = domaine.volumes_entrelaces_dir(), &xp = domaine.xp(), &xv = domaine.xv();
  const DoubleVect& fs = domaine.face_surfaces(), &pe = porosite_e, &pf = porosite_f, &ve = domaine.volumes();

  /* a_r : produit alpha_rho si Pb_Multiphase -> par semi_implicite, ou en recuperant le champ_conserve de l'equation de masse */
  const std::string& nom_inco = ch.le_nom().getString();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs(),
                   *a_r = !pbm ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") : &pbm->equation_masse().champ_conserve().valeurs(),
                    *alp = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr,
                     &rho = equation().milieu().masse_volumique().passe();

  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  int i, j, k, l, e, eb, f, fb, fc, fd, m, n, N = inco.line_size(), d, D = dimension, comp = !incompressible_;

  DoubleTrav dfac(2, N, N), masse(N, N);
  // Parcourt toutes les faces du domaine
  for (f = 0; f < domaine.nb_faces_tot(); f++)
    {
      if (f_e(f, 0) >= 0 && (f_e(f, 1) >= 0 || fcl(f, 0) == 1 || fcl(f, 0) == 3))
        {
          // Calcul des contributions des faces
          for (i = 0, dfac = 0; i < 2; i++)
            {
              // Masse diagonale avec correction si nécessaire
              for (masse = 0, e = f_e(f, (f_e(f, i) >= 0) ? i : 0), n = 0; n < N; n++)
                masse(n, n) = a_r ? (*a_r)(e, n) : 1;

              if (corr)
                corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);

              // Contribution à dfac
              for (e = f_e(f, i), eb = f_e(f, i), n = 0; n < N; n++)
                {
                  for (m = 0; m < N; m++)
                    {
                      double signe = (vit(f, m) * (i ? -1 : 1) >= 0) ? 1. : (vit(f, m) ? -1. : 0.);
                      dfac((fcl(f, 0) == 1) ? 0 : i, n, m) += fs(f) * vit(f, m) * pe((eb >= 0) ? eb : f_e(f, 0)) * masse(n, m) * (1. + signe * alpha_) / 2;
                    }
                }
            }

          // Contributions aux matrices et au second membre
          for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            {
              for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++)
                {
                  if (fb < domaine.nb_faces())
                    {
                      // Cas d'équivalence : face source -> face cible
                      if ((fc = equiv(f, i, k)) >= 0 || f_e(f, 1) < 0)
                        {
                          for (j = 0; j < 2; j++)
                            {
                              eb = f_e(f, j);
                              fd = (j == i) ? fb : fc; // Face ou élément source

                              //multiplicateur pour passer de vf a ve
                              double mult = (fd < 0 || domaine.dot(&nf(fb, 0), &nf(fd, 0)) > 0) ? 1 : -1;
                              mult *= (fd >= 0) ? pf(fd) / pe(eb) : 1;

                              for (n = 0; n < N; n++)
                                for (m = 0; m < N; m++)
                                  {
                                    if (dfac(j, n, m))
                                      {
                                        double fac = (i ? -1 : 1) * vfd(fb, e != f_e(fb, 0)) * dfac(j, n, m) / ve(e);

                                        // Mise à jour du second membre
                                        if (fd >= 0)
                                          secmem(fb, n) -= fac * mult * inco(fd, m);
                                        else
                                          {
                                            // CL de Dirichlet
                                            for (d = 0; d < D; d++)
                                              secmem(fb, n) -= fac * nf(fb, d) / fs(fb) *
                                                               ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + m);
                                          }
                                        if (comp)
                                          secmem(fb, n) += fac * inco(fb, m);

                                        // Mise à jour de la matrice
                                        if (mat)
                                          {
                                            if (fd >= 0)
                                              (*mat)(N * fb + n, N * fd + m) += fac * mult;

                                            if (comp)
                                              (*mat)(N * fb + n, N * fb + m) -= fac;
                                          }
                                      }
                                  }
                            }
                        }
                      // Cas sans équivalence : n_f * opérateur élémentaire
                      else
                        {
                          for (j = 0; j < 2; j++)
                            {
                              for (eb = f_e(f, j), l = 0; l < e_f.dimension(1) && (fc = e_f(eb, l)) >= 0; l++)
                                {
                                  double critère = fs(fc) * domaine.dot(&xv(fc, 0), &nf(fb, 0), &xp(eb, 0));
                                  if (std::abs(critère) > 1e-6 * ve(eb) * fs(fb))
                                    {
                                      for (n = 0; n < N; n++)
                                        for (m = 0; m < N; m++)
                                          if (dfac(j, n, m))
                                            {
                                              double fac = (i ? -1 : 1) * vfd(fb, e != f_e(fb, 0)) * dfac(j, n, m) * ((eb == f_e(fc, 0)) ? 1 : -1) * critère / (ve(e) * ve(eb) * fs(fb));
                                              secmem(fb, n) -= fac * inco(fc, m);
                                              if (mat && fac)
                                                {
                                                  (*mat)(N * fb + n, N * fc + m) += fac;
                                                }
                                            }
                                    }
                                }
                              // Partie correction si `comp`
                              if (comp)
                                {
                                  for (l = 0; l < e_f.dimension(1) && (fc = e_f(e, l)) >= 0; l++)
                                    {
                                      double critère_comp = fs(fc) * domaine.dot(&xv(fc, 0), &nf(fb, 0), &xp(e, 0));
                                      if (std::abs(critère_comp) > 1e-6 * ve(e) * fs(fb))
                                        {
                                          for (n = 0; n < N; n++)
                                            for (m = 0; m < N; m++)
                                              {
                                                if (dfac(j, n, m))
                                                  {
                                                    double fac = (i ? -1 : 1) * vfd(fb, e != f_e(fb, 0)) * dfac(j, n, m) * ((e == f_e(fc, 0)) ? 1 : -1) * critère_comp / (ve(e) * ve(e) * fs(fb));
                                                    secmem(fb, n) += fac * inco(fc, m);
                                                    if (mat && fac)
                                                      (*mat)(N * fb + n, N * fc + m) -= fac;
                                                  }
                                              }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  statistiques().end_count(convection_counter_);
}
