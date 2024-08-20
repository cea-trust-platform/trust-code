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

#include <Op_Conv_Amont_VPoly_VDF_Face.h>
#include <Masse_ajoutee_base.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Amont_VPoly_VDF_Face,"Op_Conv_Amont_VPoly_VDF_Face",Op_Conv_Amont_VDF_Face);

Sortie& Op_Conv_Amont_VPoly_VDF_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Amont_VPoly_VDF_Face::readOn(Entree& s ) { return s ; }

void Op_Conv_Amont_VPoly_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine = iter_->domaine();
  // okok je sais ... tgv
  DoubleTab& tab_flux_bords = flux_bords();
  tab_flux_bords.resize(domaine.nb_faces_bord(), dimension);
  tab_flux_bords = 0.;

  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue().valeur());
  const Conds_lim& cls = iter_->domaine_Cl().les_conditions_limites();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& vit = ch.passe(), &vfd = domaine.volumes_entrelaces_dir();
  const DoubleVect& fs = domaine.face_surfaces(), &pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();

  /* a_r : produit alpha_rho si Pb_Multiphase -> par semi_implicite, ou en recuperant le champ_conserve de l'equation de masse */
  const std::string& nom_inco = ch.le_nom().getString();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : nullptr;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs(), *a_r =
                            !pbm ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") : &pbm->equation_masse().champ_conserve().valeurs(), *alp = pbm ? &pbm->equation_masse().inconnue()->passe() : nullptr, &rho =
                                  equation().milieu().masse_volumique().passe();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  int i, j, k, e = -100, eb, f, fb, fd, m, n, N = inco.line_size(), d, D = dimension, comp = !incompressible_;

  DoubleTrav dfac(2, N, N), masse(N, N);
  for (f = 0; f < domaine.nb_faces_tot(); f++)
    {
      const bool is_interne = (f_e(f, 0) >= 0 && f_e(f, 1) >= 0);
      const bool is_bord = ((f_e(f, 0) >= 0 && f_e(f, 1) < 0) || (f_e(f, 0) < 0 && f_e(f, 1) >= 0));
      const bool is_neum_or_diric = (fcl(f, 0) == 1 || fcl(f, 0) == 3);

      if (is_interne || (is_bord && is_neum_or_diric))
        {
          // etape 1 : masse + dfac
          if (f_e(f, 0) < 0)
            {
              for (i = 1, dfac = 0; i >= 0; i--)
                {
                  //masse : diagonale
                  for (masse = 0, e = f_e(f, f_e(f, i) >= 0 ? i : !i), n = 0; n < N; n++)
                    masse(n, n) = a_r ? (*a_r)(e, n) : 1;

                  // masse ajoutee si correlation
                  if (corr)
                    corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);

                  //contribution a dfac
                  for (n = 0; n < N; n++)
                    {
                      e = f_e(f, i);
                      eb = e;
                      for (m = 0; m < N; m++)
                        {
                          const int ind = fcl(f, 0) == 1 ? 0 : i;
                          const int ind_por = eb >= 0 ? eb : f_e(f, f_e(f, i) >= 0 ? i : !i);
                          double dd = (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : vit(f, m) ? -1. : 0.);

                          dfac(ind, n, m) += fs(f) * vit(f, m) * pe(ind_por) * masse(n, m) * (1. + dd) / 2;
                        }
                    }
                }
            }
          else
            {
              for (i = 0, dfac = 0; i < 2; i++)
                {
                  //masse : diagonale
                  for (masse = 0, e = f_e(f, f_e(f, i) >= 0 ? i : !i), n = 0; n < N; n++)
                    masse(n, n) = a_r ? (*a_r)(e, n) : 1;

                  // masse ajoutee si correlation
                  if (corr)
                    corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);

                  //contribution a dfac
                  for (n = 0; n < N; n++)
                    {
                      e = f_e(f, i);
                      eb = e;
                      for (m = 0; m < N; m++)
                        {
                          const int ind = fcl(f, 0) == 1 ? (f_e(f, 0) >= 0 ? 0 : 1) : i;
                          const int ind_por = eb >= 0 ? eb : f_e(f, f_e(f, i) >= 0 ? i : !i);
                          double dd = (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : vit(f, m) ? -1. : 0.);

                          dfac(ind, n, m) += fs(f) * vit(f, m) * pe(ind_por) * masse(n, m) * (1. + dd) / 2;
                        }
                    }
                }
            }

          // etape 2 : secmem + derivee
          for (i = 0; i < 2; i++)
            if ((e = f_e(f, i)) >= 0)
              {
                for (k = 0; k < e_f.dimension(1); k++)
                  {
                    fb = e_f(e, k);
                    if (fb >= 0 && (domaine.orientation(fb) == domaine.orientation(f)))
                      if (fb < domaine.nb_faces())
                        if (f_e(f, i == 0 ? 1 : 0) < 0 || (f_e(f, 0) >= 0 && f_e(f, 1) >= 0))
                          {
                            if (f_e(f, 0) < 0)
                              for (j = 1; j >= 0; j--) //equivalence : face fd -> face fb
                                {
                                  eb = f_e(f, j), fd = (j == i ? fb : domaine.face_amont_princ(fb, j) /* face  */); //element/face sources

                                  for (n = 0; n < N; n++)
                                    for (m = 0; m < N; m++)
                                      if (dfac(!j, n, m))
                                        {
                                          // secmem
                                          double fac = (i ? -1 : 1) * vfd(fb, e != f_e(fb, 0)) * dfac(!j, n, m) / ve(e);
                                          if (f_e(fb, 0) < 0)
                                            fac *= 0.5;
                                          if (fd >= 0)
                                            secmem(fb, n) -= fac * inco(fd, m); //autre face calculee
                                          else
                                            {
                                              for (d = 0; d < D; d++)  //CL de Dirichlet
                                                {
                                                  if (sub_type(Dirichlet, cls[fcl(f, 1)].valeur()))
                                                    throw;
                                                  secmem(fb, n) -= 0;  //fac * domaine.face_normales(fb, d) / fs(fb) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + m);
                                                }
                                            }

                                          // si compressible :partie v div(alpha rho v)
                                          if (comp)
                                            secmem(fb, n) += fac * inco(fb, m);

                                          // derivee
                                          if (mat)
                                            {
                                              if (fd >= 0)
                                                (*mat)(N * fb + n, N * fd + m) += fac;
                                              if (comp)
                                                (*mat)(N * fb + n, N * fb + m) -= fac;
                                            }
                                        }
                                }
                            else
                              for (j = 0; j < 2; j++) //equivalence : face fd -> face fb
                                {
                                  eb = f_e(f, j), fd = (j == i ? fb : domaine.face_amont_princ(fb, j) /* face  */); //element/face sources

                                  for (n = 0; n < N; n++)
                                    for (m = 0; m < N; m++)
                                      if (dfac(j, n, m))
                                        {
                                          // secmem
                                          double fac = (i ? -1 : 1) * vfd(fb, e != f_e(fb, 0)) * dfac(j, n, m) / ve(e);
                                          if (f_e(fb, 0) < 0)
                                            fac *= 0.5;
                                          if (fd >= 0)
                                            secmem(fb, n) -= fac * inco(fd, m); //autre face calculee
                                          else
                                            {
                                              for (d = 0; d < D; d++)  //CL de Dirichlet
                                                {
                                                  if (sub_type(Dirichlet, cls[fcl(f, 1)].valeur()))
                                                    throw;
                                                  secmem(fb, n) -= 0;  //fac * domaine.face_normales(fb, d) / fs(fb) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + m);
                                                }
                                            }

                                          // si compressible :partie v div(alpha rho v)
                                          if (comp)
                                            secmem(fb, n) += fac * inco(fb, m);

                                          // derivee
                                          if (mat)
                                            {
                                              if (fd >= 0)
                                                (*mat)(N * fb + n, N * fd + m) += fac;
                                              if (comp)
                                                (*mat)(N * fb + n, N * fb + m) -= fac;
                                            }
                                        }
                                }
                          }

                  }
              }
        }
    }
}

