/****************************************************************************
* Copyright (c) 2020, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Conv_EF_Stab_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_Stab_CoviMAC_Face.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_Face_CoviMAC.h>
#include <Masse_CoviMAC_Face.h>
#include <Param.h>
#include <cmath>

Implemente_instanciable_sans_constructeur( Op_Conv_EF_Stab_CoviMAC_Face, "Op_Conv_EF_Stab_CoviMAC_Face_CoviMAC", Op_Conv_CoviMAC_base ) ;

Op_Conv_EF_Stab_CoviMAC_Face::Op_Conv_EF_Stab_CoviMAC_Face()
{
  alpha = 1; //par defaut, on fait de l'amont
}

// XD Op_Conv_EF_Stab_CoviMAC_Face interprete Op_Conv_EF_Stab_CoviMAC_Face 1 Class Op_Conv_EF_Stab_CoviMAC_Face_CoviMAC
Sortie& Op_Conv_EF_Stab_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Entree& Op_Conv_EF_Stab_CoviMAC_Face::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Conv_EF_Stab_CoviMAC_Face::completer()
{
  Op_Conv_CoviMAC_base::completer();
  /* au cas ou... */
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Equation_base& eq = equation();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, eq.inconnue().valeur());
  ch.init_cl(), zone.init_equiv();

  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Conv_EF_Stab_CoviMAC_Face : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();
  porosite_f.ref(zone.porosite_face());
  porosite_e.ref(zone.porosite_elem());
}

void Op_Conv_EF_Stab_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();
  int i, j, k, e, eb, f, fb, fc, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* agit uniquement aux elements; diagonale omise */
  for (f = 0; f < zone.nb_faces_tot(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
        {
          for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2)
              {
                if ((fc = zone.equiv(f, i, k)) >= 0) //equivalence : face -> face
                  for (n = 0; ch.fcl(fc, 0) < 2 && n < N; n++) stencil.append_line(N * fb + n, N * fc + n);
                else if (f_e(f, 1) >= 0) for (d = 0; d < D; d++) //pas d'equivalence : elem -> face
                    if(dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) stencil.append_line(N * fb + n, N * (nf_tot + D * eb + d) + n); //elem -> face
              }
          for (d = 0; d < D; d++) for (n = 0; n < N; n++) //elem->elem
              stencil.append_line(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * eb + d) + n);
        }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + D * ne_tot), N * (nf_tot + D * ne_tot), stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Conv_EF_Stab_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& vit = vitesse_->valeurs(), &nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  const DoubleVect& fs = zone.face_surfaces(), &pe = porosite_e, &pf = porosite_f, &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int i, j, k, e, eb, f, fb, fc, fd, nf_tot = zone.nb_faces_tot(), n, N = inco.line_size(), d, D = dimension;
  double mult;

  DoubleTrav dfac(2, N), pdfac(2, N);
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      for (i = 0, dfac = 0, pdfac = 0; i < 2; i++, dfac += pdfac, pdfac = 0)
        {
          if ((e = f_e(f, i)) >= 0 || ch.fcl(f, 0) == 3) for (n = 0; n < N; n++) pdfac(i, n) = pe(e >= 0 ? e : f_e(f, 0));
          else if (ch.fcl(f, 0) == 1) for (n = 0; n < N; n++) pdfac(0, n) = pe(f_e(f, 0));
          for (j = 0; j < 2; j++) for (n = 0; n < N; n++) pdfac(j, n) *= (1. + (vit.addr()[N * f + n] * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
        }
      for (j = 0; j < 2; j++) for (n = 0; n < N; n++) dfac(j, n) *= fs(f) * vit.addr()[N * f + n];
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2) //partie "faces"
              {
                if ((fc = zone.equiv(f, i, k)) >= 0 || f_e(f, 1) < 0) for (j = 0; j < 2; j++) //equivalence : face fd -> face fb
                    {
                      for (eb = f_e(f, j), fd = (j == i ? fb : fc), mult = pf(fd >= 0 ? fd : fb) / pe(eb >= 0 ? eb : f_e(f, 0)) * (fd < 0 || zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1), n = 0; n < N; n++)
                        if (dfac(j, n) && fd >= 0) resu.addr()[N * fb + n] -= (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e) * mult * inco.addr()[N * fd + n];
                        else if (dfac(j, n) && ch.fcl(f, 0) == 3) for (d = 0; d < D; d++) //convection avec une CL de Dirichlet
                            resu.addr()[N * fb + n] -= (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e) * mult
                                                       * nf(fb, d) / fs(fb) * ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), d);
                    }
                else for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++) //pas d'equivalence : elem eb -> face fb
                    {
                      /*if (eb == e) for (n = 0; n < N; n++) if (dfac(j, n))
                            resu.addr()[N * fb + n] -= (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e) * inco.addr()[N * fb + n];
                      if (eb != e)*/ for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) if (dfac(j, n))
                              resu.addr()[N * fb + n] -= (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e) * nf(fb, d) / fs(fb) * inco.addr()[N * (nf_tot + D * eb + d) + n];
                    }
              }
          for (j = 0; j < 2; j++) for (eb = f_e(f, j), d = 0; d < D; d++) for (n = 0; n < N; n++) if (dfac(j, n)) //partie "elem"
                  resu.addr()[N * (nf_tot + D * e + d) + n] -= (i ? -1 : 1) * dfac(j, n)
                                                               * (eb >= 0 ? inco.addr()[N * (nf_tot + D * eb + d) + n] : ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), N * d + n));
        }
    }

  return resu;
}

//Description:
//on assemble la matrice.

inline void Op_Conv_EF_Stab_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& vit = vitesse_->valeurs(), &nf = zone.face_normales(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f;
  const DoubleVect& fs = zone.face_surfaces(), &pe = porosite_e, &pf = porosite_f, &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int i, j, k, e, eb, f, fb, fc, fd, nf_tot = zone.nb_faces_tot(), n, N = inco.line_size(), d, D = dimension;
  double mult;

  DoubleTrav dfac(2, N), pdfac(2, N);
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      for (i = 0, dfac = 0, pdfac = 0; i < 2; i++, dfac += pdfac, pdfac = 0)
        {
          if ((e = f_e(f, i)) >= 0) for (n = 0; n < N; n++) pdfac(i, n) = pe(e);
          else if (ch.fcl(f, 0) == 1) for (n = 0; n < N; n++) pdfac(0, n) = pe(f_e(f, 0));
          for (j = 0; j < 2; j++) for (n = 0; n < N; n++) pdfac(j, n) *= (1. + (vit.addr()[N * f + n] * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
        }
      for (j = 0; j < 2; j++) for (n = 0; n < N; n++) dfac(j, n) *= fs(f) * vit.addr()[N * f + n];
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2) //partie "faces"
              {
                if ((fc = zone.equiv(f, i, k)) >= 0 || f_e(f, 1) < 0) for (j = 0; j < 2; j++) //equivalence : face fd -> face fb
                    {
                      for (eb = f_e(f, j), fd = (i == j ? fb : fc), mult = pf(fd >= 0 ? fd : fb) / pe(eb >= 0 ? eb : f_e(f, 0)) * (fd < 0 || zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1), n = 0; n < N; n++)
                        if (dfac(j, n) && fd >= 0 && ch.fcl(fd, 0) < 2) matrice(N * fb + n, N * fd + n) += (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e) * mult;
                    }
                else for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++) //pas d'equivalence : elem eb -> face fb
                    {
                      /*if (eb == e) for (n = 0; n < N; n++) if (dfac(j, n))
                            matrice(N * fb + n, N * fb + n) += (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e);
                      if (eb != e)*/ for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) if (dfac(j, n)) //pas d'equivalence : elem eb -> face fb
                              matrice(N * fb + n, N * (nf_tot + D * eb + d) + n) += (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n) / ve(e) * nf(fb, d) / fs(fb);
                    }
              }
          for (j = 0; j < 2; j++) for (eb = f_e(f, j), d = 0; d < D; d++) for (n = 0; n < N; n++) if (dfac(j, n)) //partie "elem"
                  matrice(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * eb + d) + n) += (i ? -1 : 1) * dfac(j, n);
        }
    }
}
//Description:
//on ajoute la contribution du second membre.

void Op_Conv_EF_Stab_CoviMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}

void Op_Conv_EF_Stab_CoviMAC_Face::set_incompressible(const int flag)
{
  if (flag == 0)
    {
      Cerr << "Compressible form of operator \"" << que_suis_je() << "\" :" << finl;
      Cerr << "Discretization of \u2207(inco \u2297 v) - v \u2207.(inco)" << finl;
    }
  incompressible_ = flag;
}
