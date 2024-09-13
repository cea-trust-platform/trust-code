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

  if (equation().discretisation().is_polymac())
    {
      const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
      const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();
      int i, j, k, e1, e2, f, f1, f2, ok;

      Cerr << domaine.domaine().le_nom() << " : initialisation de la convection aux faces... ";

      IntTrav ntot, nequiv;
      domaine.creer_tableau_faces(ntot), domaine.creer_tableau_faces(nequiv);
      equiv_.resize(domaine.nb_faces_tot(), 2, e_f.dimension(1));
      for (f = 0, equiv_ = -1; f < domaine.nb_faces_tot(); f++)
        if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
          for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
            for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
              {
                for (k = 0, ok = 1; ok && k < dimension; k++)
                  ok &= std::fabs((xv(f1, k) - xp(e1, k)) - (xv(f2, k) - xp(e2, k))) < 1e-6;
                if (ok)
                  equiv_(f, 0, i) = f2, equiv_(f, 1, j) = f1, nequiv(f)++;
              }
      if (mp_somme_vect(ntot) != 0)
        Cerr << mp_somme_vect_as_double(nequiv) * 100. / mp_somme_vect_as_double(ntot) << "% de convection directe!" << finl;
    }
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
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();
  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces();
  int i, j, k, l, m, e, eb, f, fb, fc, idx;

  ch.fcl(), domaine.init_ve();

  IntTab stencil(0, 2);


  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      for (j = 0; f < domaine.nb_faces() && ch.fcl()(f, 0) < 2 && j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
        {
          if (ch.fcl()(fb, 0) < 2)
            stencil.append_line(f, fb);
          if ((fc = equiv_(fb, e != f_e(fb, 0), i)) >= 0 || f_e(fb, 1) < 0) //equivalence ou bord -> convection de m2
            {
              int fa[2] = { f, fc }, ea[2] = { e, f_e(fb, e == f_e(fb, 0)) };
              for (k = 0; k < 2 && ea[k] >= 0; k++)
                for (l = domaine.m2d(ea[k]), idx = 0; l < domaine.m2d(ea[k] + 1); l++, idx++)
                  for (m = domaine.m2i(l); e_f(ea[k], idx) == fa[k] && m < domaine.m2i(l + 1); m++)
                    if (ch.fcl()(fc = e_f(ea[k], domaine.m2j(m)), 0) < 2)
                      stencil.append_line(f, fc);
            }
          else
            for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++)
              for (l = domaine.vedeb(eb); l < domaine.vedeb(eb + 1); l++) //sinon -> convection de ve.(xv-xp)
                if (std::fabs(domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0))) > 1e-8 * vf(f) / fs(f) && ch.fcl()(fc = domaine.veji(l), 0) < 2)
                  stencil.append_line(f, fc);
        }

  tableau_trier_retirer_doublons(stencil);
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

  /* if (0)
    {
      // ToDo Kokkos: Ne fonctionne pas... Difficulte avec les appels virtuels pour les CLs. Separer faces de bord et faces internes ?
      CIntTabView f_e = domaine.face_voisins().view_ro();
      CIntTabView e_f = domaine.elem_faces().view_ro();
      CDoubleTabView xp = domaine.xp().view_ro();
      CDoubleTabView xv = domaine.xv().view_ro();
      CDoubleTabView vfd = domaine.volumes_entrelaces_dir().view_ro();
      CDoubleArrView vit = static_cast<const DoubleVect&>(vitesse_->valeurs()).view_ro();
      CDoubleArrView fs = domaine.face_surfaces().view_ro();
      CDoubleArrView ve = domaine.volumes().view_ro();
      CDoubleArrView pf = porosite_f.view_ro();
      CDoubleArrView pe = porosite_e.view_ro();
      CDoubleArrView m2c = static_cast<const DoubleVect&>(domaine.m2c).view_ro();
      CIntArrView m2d = static_cast<const IntVect&>(domaine.m2d).view_ro();
      CIntArrView m2i = static_cast<const IntVect&>(domaine.m2i).view_ro();
      CIntArrView m2j = static_cast<const IntVect&>(domaine.m2j).view_ro();
      CIntArrView vedeb = static_cast<const IntVect&>(domaine.vedeb).view_ro();
      CIntArrView veji = static_cast<const IntVect&>(domaine.veji).view_ro();
      CIntTabView3 equiv = equiv_.view3_ro();
      CDoubleTabView veci = domaine.veci.view_ro();
      CIntTabView fcl = ch.fcl().view_ro();
      CDoubleArrView inco = static_cast<const DoubleVect&>(tab_inco).view_ro();
      DoubleArrView resu = static_cast<DoubleVect&>(tab_resu).view_rw();
      int nb_faces = domaine.nb_faces();
      int nfe = domaine.elem_faces().dimension(1);
      int dim = Objet_U::dimension;
      const double alpha = alpha_;
      const int incompressible = incompressible_;
      //element e -> contribution de la face fb a l'equation a la face f
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(0, domaine.nb_elem_tot()),
                           KOKKOS_LAMBDA(
                             const int e)
      {
        int i, j, k, l, m, eb, f, fb, fc, fd, fam, idx;
        double div;
        for (i = 0, div = 0;
             i < nfe && (f = e_f(e, i)) >= 0; div += fs(f) * pf(f) * (e == f_e(f, 0) ? 1 : -1) * vit(f), i++)
          {
            for (j = 0; f < nb_faces && fcl(f, 0) < 2 && j < nfe && (fb = e_f(e, j)) >= 0; j++)
              for (k = 0; k < 2; k++) //amont/aval de fb
                {
                  eb = f_e(fb, k); //element amont/aval de fb (toujours l'amont si Neumann)
                  double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) *
                               (1. + (vit(fb) * (k ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
                  if ((fc = equiv(fb, (int) (e != f_e(fb, 0)), i)) >= 0 || f_e(fb, 0) < 0 ||
                      f_e(fb, 1) < 0) //equivalence ou bord -> on convecte m2
                    {
                      if (eb >= 0)
                        for (fam = (eb == e ? f : fc), l = m2d(eb), idx = 0; l < m2d(eb + 1); l++, idx++)
                          for (m = m2i(l); fam == e_f(eb, idx) && m < m2i(l + 1); m++) //convection de m2
                            {
                              fd = e_f(eb, m2j(m));
                              Kokkos::atomic_sub(&resu(f), fac * (eb == f_e(fd, 0) ? 1 : -1) * ve(eb) * m2c(m) *
                                                 vfd(f, e != f_e(f, 0)) /
                                                 vfd(fam, eb != f_e(fam, 0)) * pe(eb) * inco(fd));
                            }
                      else if (fcl(fb, 0) == 3)
                        {
                          for (l = 0; l < dim; l++) //face de Dirichlet -> on convecte la vitesse au bord
                            {
                              // ToDo Kokkos: comment gerer l'appel virtuel ici:
                              const Dirichlet& cl = static_cast<const Dirichlet&>(cls[fcl(fb, 1)].valeur());
                              Kokkos::atomic_sub(&resu(f), fac * fs(f) * (xv(f, l) - xp(e, l)) * cl.val_imp_view(fcl(fb, 2), l));
                            }
                        }
                    }
                  else
                    for (l = vedeb(eb);
                         l < vedeb(eb + 1); l++) //face interne sans equivalence -> convection de ve
                      {
                        fc = veji(l);
                        Kokkos::atomic_sub(&resu(f),
                                           fac * fs(f) * domaine.dot(dim, &xv(f, 0), &veci(l, 0), &xp(e, 0)) *
                                           pe(eb) * inco(fc));
                      }
                }
          }

        //partie - (div v) v
        if (!incompressible)
          for (i = 0; i < nfe && (f = e_f(e, i)) >= 0; i++)
            for (j = m2i(m2d(e) + i); f < nb_faces && fcl(f, 0) < 2 && j < m2i(m2d(e) + i); j++)
              {
                fb = e_f(e, m2j(j));
                Kokkos::atomic_add(&resu(f),
                                   (f == f_e(e, 0) ? 1 : -1) * (fb == f_e(e, 0) ? 1 : -1) * ve(e) * div *
                                   inco(fb));
              }
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  else */
  {
    const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
    const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vfd = domaine.volumes_entrelaces_dir(), &vit = vitesse_->valeurs();
    const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes(), &pf = porosite_f, &pe = porosite_e;

    int i, j, k, l, m, e, eb, f, fb, fc, fd, fam, idx;
    double div;
    //element e -> contribution de la face fb a l'equation a la face f
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      {
        for (i = 0, div = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; div += fs(f) * pf(f) * (e == f_e(f, 0) ? 1 : -1) * vit(f), i++)
          for (j = 0; f < domaine.nb_faces() && ch.fcl()(f, 0) < 2 && j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
            for (k = 0; k < 2; k++) //amont/aval de fb
              {
                eb = f_e(fb, k); //element amont/aval de fb (toujours l'amont si Neumann)
                double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) * (1. + (vit(fb) * (k ? -1 : 1) >= 0 ? 1. : -1.) * alpha_) / 2;
                if ((fc = equiv_(fb, e != f_e(fb, 0), i)) >= 0 || f_e(fb, 0) < 0 || f_e(fb, 1) < 0) //equivalence ou bord -> on convecte m2
                  {
                    if (eb >= 0)
                      for (fam = (eb == e ? f : fc), l = domaine.m2d(eb), idx = 0; l < domaine.m2d(eb + 1); l++, idx++)
                        for (m = domaine.m2i(l); fam == e_f(eb, idx) && m < domaine.m2i(l + 1); m++) //convection de m2
                          fd = e_f(eb, domaine.m2j(m)), tab_resu(f) -= fac * (eb == f_e(fd, 0) ? 1 : -1) * ve(eb) * domaine.m2c(m) * vfd(f, e != f_e(f, 0)) / vfd(fam, eb != f_e(fam, 0)) * pe(eb) * tab_inco(fd);
                    else if (ch.fcl()(fb, 0) == 3)
                      for (l = 0; l < dimension; l++) //face de Dirichlet -> on convecte la vitesse au bord
                        tab_resu(f) -= fac * fs(f) * (xv(f, l) - xp(e, l)) * ref_cast(Dirichlet, cls[ch.fcl()(fb, 1)].valeur()).val_imp(ch.fcl()(fb, 2), l);
                  }
                else
                  for (l = domaine.vedeb(eb); l < domaine.vedeb(eb + 1); l++) //face interne sans equivalence -> convection de ve
                    fc = domaine.veji(l), tab_resu(f) -= fac * fs(f) * domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0)) * pe(eb) * tab_inco(fc);
              }

        //partie - (div v) v
        if (!incompressible_)
          for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
            for (j = domaine.m2i(domaine.m2d(e) + i); f < domaine.nb_faces() && ch.fcl()(f, 0) < 2 && j < domaine.m2i(domaine.m2d(e) + i); j++)
              fb = e_f(e, domaine.m2j(j)), tab_resu(f) += (f == f_e(e, 0) ? 1 : -1) * (fb == f_e(e, 0) ? 1 : -1) * ve(e) * div * tab_inco(fb);
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
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vfd = domaine.volumes_entrelaces_dir(), &vit = vitesse_->valeurs();
  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes(), &pe = porosite_e, &pf = porosite_f;
  int i, j, k, l, m, e, eb, f, fb, fc, fd, fam, idx;
  double div;

  //element e -> contribution de la face fb a l'equation a la face f
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    {
      for (i = 0, div = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; div += fs(f) * pf(f) * (e == f_e(f, 0) ? 1 : -1) * vit(f), i++)
        for (j = 0; f < domaine.nb_faces() && ch.fcl()(f, 0) < 2 && j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
          for (k = 0; (ch.fcl()(fb, 0) < 2 || ch.fcl()(fb, 0) == 3) && k < 2; k++) //amont/aval de fb
            {
              eb = f_e(fb, k); //element amont/aval de fb (toujours l'amont si Neumann)
              double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) * (1. + (vit(fb) * (k ? -1. : 1) >= 0 ? 1. : -1.) * alpha_) / 2;
              if ((fc = equiv_(fb, e != f_e(fb, 0), i)) >= 0 || f_e(fb, 0) < 0 || f_e(fb, 1) < 0) //equivalence ou bord -> on convecte m2
                {
                  if (eb >= 0)
                    {
                      for (fam = (eb == e ? f : fc), l = domaine.m2d(eb), idx = 0; l < domaine.m2d(eb + 1); l++, idx++)
                        for (m = domaine.m2i(l); fam == e_f(eb, idx) && m < domaine.m2i(l + 1); m++)
                          if (ch.fcl()(fd = e_f(eb, domaine.m2j(m)), 0) < 2) //convection de m2
                            matrice(f, fd) += fac * (eb == f_e(fd, 0) ? 1 : -1) * ve(eb) * domaine.m2c(m) * vfd(f, e != f_e(f, 0)) / vfd(fam, eb != f_e(fam, 0)) * pe(eb);
                    }
                }
              else
                for (l = domaine.vedeb(eb); l < domaine.vedeb(eb + 1); l++) //face interne sans equivalence -> convection de ve
                  if (ch.fcl()(fc = domaine.veji(l), 0) < 2 && std::fabs(domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0))) > 1e-8 * vf(f) / fs(f))
                    matrice(f, fc) += fac * fs(f) * domaine.dot(&xv(f, 0), &domaine.veci(l, 0), &xp(e, 0)) * pe(eb);
            }

      //partie - (div v) v
      if (!incompressible_)
        for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
          for (j = domaine.m2i(domaine.m2d(e) + i); f < domaine.nb_faces() && ch.fcl()(f, 0) < 2 && j < domaine.m2i(domaine.m2d(e) + i); j++)
            if (ch.fcl()(fb = e_f(e, domaine.m2j(j)), 0) < 2)
              matrice(f, fb) -= (f == f_e(e, 0) ? 1 : -1) * (fb == f_e(e, 0) ? 1 : -1) * ve(e) * div;
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
