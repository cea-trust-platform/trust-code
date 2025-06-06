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

#include <Op_Conv_EF_Stab_PolyMAC_Face.h>
#include <Discretisation_base.h>
#include <Dirichlet_homogene.h>
#include <Champ_Face_PolyMAC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Domaine_PolyMAC.h>
#include <Probleme_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Param.h>
#include <cmath>

Implemente_instanciable( Op_Conv_EF_Stab_PolyMAC_Face, "Op_Conv_EF_Stab_PolyMAC_Face_PolyMAC", Op_Conv_PolyMAC_base );
Implemente_instanciable( Op_Conv_Amont_PolyMAC_Face, "Op_Conv_Amont_PolyMAC_Face_PolyMAC", Op_Conv_EF_Stab_PolyMAC_Face );
Implemente_instanciable( Op_Conv_Centre_PolyMAC_Face, "Op_Conv_Centre_PolyMAC_Face_PolyMAC", Op_Conv_EF_Stab_PolyMAC_Face );

// XD Op_Conv_EF_Stab_PolyMAC_Face interprete Op_Conv_EF_Stab_PolyMAC_Face 1 Class Op_Conv_EF_Stab_PolyMAC_Face_PolyMAC

Sortie& Op_Conv_EF_Stab_PolyMAC_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Amont_PolyMAC_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Centre_PolyMAC_Face::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }

Entree& Op_Conv_EF_Stab_PolyMAC_Face::readOn(Entree& is)
{
  Op_Conv_PolyMAC_base::readOn(is);
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha_);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

Entree& Op_Conv_Amont_PolyMAC_Face::readOn(Entree& is)
{
  alpha_ = 1.0;
  return Op_Conv_PolyMAC_base::readOn(is);
}

Entree& Op_Conv_Centre_PolyMAC_Face::readOn(Entree& is)
{
  alpha_ = 0.0;
  return Op_Conv_PolyMAC_base::readOn(is);
}

void Op_Conv_EF_Stab_PolyMAC_Face::completer()
{
  Op_Conv_PolyMAC_base::completer();

  /* au cas ou... */
  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 2)
    {
      Cerr << "Op_Conv_EF_Stab_PolyMAC_Face : largeur de joint insuffisante (minimum 2)!" << finl;
      Process::exit();
    }
  porosite_f.ref(mon_equation->milieu().porosite_face());
  porosite_e.ref(mon_equation->milieu().porosite_elem());
}

void Op_Conv_EF_Stab_PolyMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  if (has_interface_blocs())
    {
      Operateur_base::dimensionner(mat);
      return;
    }

  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue());
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &equiv = domaine.equiv();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();
  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces();

  ch.fcl(), domaine.init_ve();

  IntTab stencil(0, 2);

  for (int e = 0; e < domaine.nb_elem_tot(); e++)
    for (int i = 0; i < e_f.dimension(1); i++)
      {
        int f = e_f(e, i);
        if (f < 0) continue;

        for (int j = 0; j < e_f.dimension(1); j++)
          {
            int fb = e_f(e, j);
            if (fb < 0) continue;

            if (f < domaine.nb_faces() && ch.fcl()(f, 0) < 2)
              {
                // Ajout de la ligne pour le stencil si fb est interne
                if (ch.fcl()(fb, 0) < 2)
                  stencil.append_line(f, fb);

                // Cas d'equivalence ou bord : convection de m2
                int fc = equiv(fb, e != f_e(fb, 0), i);
                if (fc  >= 0 || f_e(fb, 1) < 0)
                  {
                    int fa[2] = { f, fc };
                    int ea[2] = { e, f_e(fb, e == f_e(fb, 0)) };
                    for (int k = 0; k < 2; k++)
                      {
                        if (ea[k] < 0) continue;

                        int idx = 0;
                        for (int l = domaine.m2d(ea[k]); l < domaine.m2d(ea[k] + 1); l++, idx++)
                          {
                            if (e_f(ea[k], idx) == fa[k])
                              {
                                for (int m = domaine.m2i(l); m < domaine.m2i(l + 1); m++)
                                  {
                                    fc = e_f(ea[k], domaine.m2j(m));
                                    if (ch.fcl()(fc, 0) < 2)
                                      stencil.append_line(f, fc);
                                  }
                              }
                          }
                      }
                  }
                else
                  {
                    // Cas general : convection de ve.(xv - xp)
                    for (int k = 0; k < 2; k++)
                      {
                        int eb = f_e(fb, k);
                        if (eb < 0) continue;

                        for (int l = domaine.vedeb(eb); l < domaine.vedeb(eb + 1); l++)
                          {
                            fc = domaine.veji(l);
                            if (ch.fcl()(fc, 0) < 2 && std::fabs(domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0))) > 1e-8 * vf(f) / fs(f))
                              stencil.append_line(f, fc);
                          }
                      }
                  }
              }
          }
      }

  // Tri et suppression des doublons
  tableau_trier_retirer_doublons(stencil);

  // Allocation de la matrice
  int taille = domaine.nb_faces_tot() + (dimension < 3 ? domaine.domaine().nb_som_tot() : domaine.domaine().nb_aretes_tot());
  Matrix_tools::allocate_morse_matrix(taille, taille, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Conv_EF_Stab_PolyMAC_Face::ajouter(const DoubleTab& tab_inco, DoubleTab& tab_resu) const
{
  if (has_interface_blocs())
    return Operateur_base::ajouter(tab_inco, tab_resu);

  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue());
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();

  domaine.init_ve();

  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &equiv = domaine.equiv();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vfd = domaine.volumes_entrelaces_dir(), &vit = vitesse_->valeurs();
  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes(), &pf = porosite_f, &pe = porosite_e;

  for (int e = 0; e < domaine.nb_elem_tot(); e++)
    {
      double div = 0.;

      for (int i = 0; i < e_f.dimension(1); i++)
        {
          int f = e_f(e, i);
          if (f < 0) continue;

          // Calcul de la divergence
          div += fs(f) * pf(f) * (e == f_e(f, 0) ? 1 : -1) * vit(f);

          // Traitement de la convection pour cette face
          if (f < domaine.nb_faces() && ch.fcl()(f, 0) < 2)
            for (int j = 0; j < e_f.dimension(1); j++)
              {
                int fb = e_f(e, j);
                if (fb < 0) continue;

                for (int k = 0; k < 2; k++)
                  {
                    int eb = f_e(fb, k);
                    double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) * (1. + (vit(fb) * (k ? -1 : 1) >= 0 ? 1. : -1.) * alpha_) / 2;

                    int fc = equiv(fb, e != f_e(fb, 0), i);
                    if (fc  >= 0 || f_e(fb, 0) < 0 || f_e(fb, 1) < 0)
                      {
                        if (eb >= 0)
                          {
                            int fam = (eb == e ? f : fc);
                            int idx = 0;
                            for (int l = domaine.m2d(eb); l < domaine.m2d(eb + 1); l++, idx++)
                              for (int m = domaine.m2i(l); m < domaine.m2i(l + 1); m++)
                                if (fam == e_f(eb, idx))
                                  {
                                    int fd = e_f(eb, domaine.m2j(m));
                                    tab_resu(f) -= fac * (eb == f_e(fd, 0) ? 1 : -1) * ve(eb) * domaine.m2c(m) * vfd(f, e != f_e(f, 0)) / vfd(fam, eb != f_e(fam, 0)) * pe(eb) * tab_inco(fd);
                                  }
                          }
                        else if (ch.fcl()(fb, 0) == 3)
                          {
                            for (int l = 0; l < dimension; l++)
                              tab_resu(f) -= fac * fs(f) * (xv(f, l) - xp(e, l)) * ref_cast(Dirichlet, cls[ch.fcl()(fb, 1)].valeur()).val_imp(ch.fcl()(fb, 2), l);
                          }
                      }
                    else
                      {
                        for (int l = domaine.vedeb(eb); l < domaine.vedeb(eb + 1); l++)
                          {
                            fc = domaine.veji(l);
                            tab_resu(f) -= fac * fs(f) * domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0)) * pe(eb) * tab_inco(fc);
                          }
                      }
                  }
              }
        }

      // Partie - (div v) v
      if (!incompressible_)
        {
          for (int i = 0; i < e_f.dimension(1); i++)
            {
              int f = e_f(e, i);
              if (f < 0) continue;

              for (int j = domaine.m2i(domaine.m2d(e) + i); j < domaine.m2i(domaine.m2d(e) + i + 1); j++)
                {
                  int fb = e_f(e, domaine.m2j(j));
                  tab_resu(f) += (f == f_e(e, 0) ? 1 : -1) * (fb == f_e(e, 0) ? 1 : -1) * ve(e) * div * tab_inco(fb);
                }
            }
        }
    }
  return tab_resu;
}

/*! @brief on assemble la matrice.
 *
 */
inline void Op_Conv_EF_Stab_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (has_interface_blocs())
    {
      Operateur_base::contribuer_a_avec(inco, matrice);
      return;
    }

  const Domaine_PolyMAC& domaine = le_dom_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &equiv = domaine.equiv();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vfd = domaine.volumes_entrelaces_dir(), &vit = vitesse_->valeurs();
  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes(), &pe = porosite_e, &pf = porosite_f;

  for (int e = 0; e < domaine.nb_elem_tot(); e++)
    {
      double div = 0.;

      // Calcul de la divergence et contributions aux equations
      for (int i = 0; i < e_f.dimension(1); i++)
        {
          int f = e_f(e, i);
          if (f < 0) continue;

          div += fs(f) * pf(f) * (e == f_e(f, 0) ? 1 : -1) * vit(f);

          // Contributions des faces voisines
          if (f < domaine.nb_faces() && ch.fcl()(f, 0) < 2)
            {
              for (int j = 0; j < e_f.dimension(1); j++)
                {
                  int fb = e_f(e, j);
                  if (fb < 0) continue;

                  for (int k = 0; k < 2; k++)
                    {
                      if (ch.fcl()(fb, 0) < 2 || ch.fcl()(fb, 0) == 3)
                        {
                          int eb = f_e(fb, k);
                          double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) * (1. + (vit(fb) * (k ? -1. : 1) >= 0 ? 1. : -1.) * alpha_) / 2;
                          int fc = equiv(fb, e != f_e(fb, 0), i);
                          if (fc  >= 0 || f_e(fb, 0) < 0 || f_e(fb, 1) < 0)
                            {
                              // Convection pour les equivalences ou bords
                              if (eb < 0) continue;

                              int fam = (eb == e ? f : fc);
                              int idx = 0;
                              for (int l = domaine.m2d(eb); l < domaine.m2d(eb + 1); l++, idx++)
                                for (int m = domaine.m2i(l); fam == e_f(eb, idx) && m < domaine.m2i(l + 1); m++)
                                  {
                                    int fd = e_f(eb, domaine.m2j(m));
                                    if (ch.fcl()(fd, 0) < 2)
                                      matrice(f, fd) += fac * (eb == f_e(fd, 0) ? 1 : -1) * ve(eb) * domaine.m2c(m) * vfd(f, e != f_e(f, 0)) / vfd(fam, eb != f_e(fam, 0)) * pe(eb);
                                  }
                            }
                          else
                            {
                              // Convection pour les faces internes sans equivalence
                              for (int l = domaine.vedeb(eb); l < domaine.vedeb(eb + 1); l++)
                                {
                                  fc = domaine.veji(l);
                                  if (ch.fcl()(fc, 0) < 2 && std::fabs(domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0))) > 1e-8 * vf(f) / fs(f))
                                    matrice(f, fc) += fac * fs(f) * domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0)) * pe(eb);
                                }
                            }
                        }
                    }
                }
            }
        }

      // Partie - (div v) v
      if (!incompressible_)
        {
          for (int i = 0; i < e_f.dimension(1); i++)
            {
              int f = e_f(e, i);
              if (f < 0) continue;

              for (int j = domaine.m2i(domaine.m2d(e) + i); j < domaine.m2i(domaine.m2d(e) + i + 1); j++)
                {
                  int fb = e_f(e, domaine.m2j(j));
                  if (ch.fcl()(fb, 0) < 2)
                    matrice(f, fb) -= (f == f_e(e, 0) ? 1 : -1) * (fb == f_e(e, 0) ? 1 : -1) * ve(e) * div;
                }
            }
        }
    }
}

void Op_Conv_EF_Stab_PolyMAC_Face::set_incompressible(const int flag)
{
  if (flag == 0)
    {
      Cerr << "Compressible form of operator \"" << que_suis_je() << "\" :" << finl;
      Cerr << "Discretization of \u2207(inco \u2297 v) - v \u2207.(inco)" << finl;
    }
  incompressible_ = flag;
}
