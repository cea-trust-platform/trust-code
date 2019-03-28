/****************************************************************************
* Copyright (c) 2015, CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Op_Conv_EF_Stab_PolyMAC_Face.cpp
// Directory : $POLYMAC_ROOT/src/Operateurs
//
/////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_Stab_PolyMAC_Face.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <cmath>
#include <Champ_Face_PolyMAC.h>
#include <Param.h>

Implemente_instanciable( Op_Conv_EF_Stab_PolyMAC_Face, "Op_Conv_EF_Stab_PolyMAC_Face_PolyMAC", Op_Conv_PolyMAC_base ) ;
// XD Op_Conv_EF_Stab_PolyMAC_Face interprete Op_Conv_EF_Stab_PolyMAC_Face 1 Class Op_Conv_EF_Stab_PolyMAC_Face_PolyMAC
Sortie& Op_Conv_EF_Stab_PolyMAC_Face::printOn( Sortie& os ) const
{
  Op_Conv_PolyMAC_base::printOn( os );
  return os;
}

Entree& Op_Conv_EF_Stab_PolyMAC_Face::readOn( Entree& is )
{
  Op_Conv_PolyMAC_base::readOn( is );
  alpha = 1; //par defaut, on fait de l'amont
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Conv_EF_Stab_PolyMAC_Face::completer()
{
  Op_Conv_PolyMAC_base::completer();
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv();
  int i, j, k, e1, e2, f, f1, f2, ok;

  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Conv_EF_Stab_PolyMAC_Face : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();

  Cerr << zone.zone().domaine().le_nom() << " : initialisation de la convection aux faces... ";

  IntTrav ntot, nequiv;
  zone.creer_tableau_faces(ntot), zone.creer_tableau_faces(nequiv);
  equiv.resize(zone.nb_faces_tot(), 2, e_f.dimension(1));
  for (f = 0, equiv = -1; f < zone.nb_faces_tot(); f++) if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
      for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
        for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
          {
            std::array<double, 3> vec = zone.cross(dimension, dimension, &xv(f2, 0), &xv(f1, 0), &xp(e2, 0), &xp(e1, 0));
            for (k = 0, ok = (zone.dot(&xv(f2, 0), &xv(f1, 0), &xp(e2, 0), &xp(e1, 0)) > 0); ok && k < 3; k++)
              ok &= dabs(vec[k]) < 1e-8 * zone.dot(&xv(f1, 0), &xv(f1, 0), &xp(e1, 0), &xp(e1, 0));
            if (ok) equiv(f, 0, i) = f2, equiv(f, 1, j) = f1, nequiv(f)++;
          }
  Cerr << mp_somme_vect(nequiv) * 100. / mp_somme_vect(ntot) << "% de convection directe!" << finl;
}

void Op_Conv_EF_Stab_PolyMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv();
  const DoubleVect& fs = zone.face_surfaces_const(), &vf = zone.volumes_entrelaces();
  int i, j, k, l, e, eb, f, fb, fc;

  ch.init_cl(), zone.init_ve();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  for (e = 0; e < zone.nb_elem_tot(); e++)  for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      for (j = 0; f < zone.nb_faces() && ch.icl(f, 0) < 2 && j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
        {
          if (ch.icl(fb, 0) < 2) stencil.append_line(f, fb);
          if ((fc = equiv(fb, e != f_e(fb, 0), i)) >= 0 || f_e(fb, 1) < 0) //equivalence ou bord -> convection de m2
            {
              int fa[2] = { f, fc }, ea[2] = { e, f_e(fb, e == f_e(fb, 0)) };
              for (k = 0; k < 2 && ea[k] >= 0; k++) for (l = zone.m2deb(fa[k]); l < zone.m2deb(fa[k] + 1); l++)
                  if (zone.m2ji(l, 1) == ea[k] && ch.icl(fc = zone.m2ji(l, 0), 0) < 2) stencil.append_line(f, fc);
            }
          else for (k = 0; k < 2 && (eb = f_e(fb, k)) >= 0; k++) for (l = zone.vedeb(eb); l < zone.vedeb(eb + 1); l++) //sinon -> convection de ve.(xv-xp)
                if (dabs(zone.dot(&xv(f, 0), &zone.veci(l, 0), &xp(e, 0))) > 1e-8 * vf(f) / fs(f) && ch.icl(fc = zone.veji(l), 0) < 2)
                  stencil.append_line(f, fc);
        }

  tableau_trier_retirer_doublons(stencil);
  int taille = zone.nb_faces_tot() + (dimension < 3 ? zone.zone().domaine().nb_som_tot() : zone.zone().nb_aretes_tot());
  Matrix_tools::allocate_morse_matrix(taille, taille, stencil, mat);
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
inline DoubleTab& Op_Conv_EF_Stab_PolyMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &vfd = zone.volumes_entrelaces_dir(), &vit = vitesse_->valeurs();
  const DoubleVect& fs = zone.face_surfaces_const(), &ve = zone.volumes(), &pf = zone.porosite_face();

  int i, j, k, l, e, eb, f, fb, fc, fd, fam;

  //element e -> contribution de la face fb a l'equation a la face f
  for (e = 0; e < zone.nb_elem_tot(); e++)  for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      for (j = 0; f < zone.nb_faces() && ch.icl(f, 0) < 2 && j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
        for (k = 0; k < 2; k++) //amont/aval de fb
          {
            eb = f_e(fb, k); //element amont/aval de fb (toujours l'amont si Neumann)
            double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) * (1. + (vit(fb) * (k ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
            if ((fc = equiv(fb, e != f_e(fb, 0), i)) >= 0 || f_e(fb, 0) < 0 || f_e(fb, 1) < 0) //equivalence ou bord -> on convecte m2
              {
                if (eb >= 0)
                  {
                    for (fam = eb == e ? f : fc, l = zone.m2deb(fam); l < zone.m2deb(fam + 1); l++) if (zone.m2ji(l, 1) == eb) //convection de m2
                        fd = zone.m2ji(l, 0), resu(f) -= fac * (eb == f_e(fam, 0) ? 1 : -1) * zone.m2ci(l) * vfd(f, e != f_e(f, 0)) / vfd(fam, eb != f_e(fam, 0)) * pf(fd) * inco(fd);
                  }
                else if (ch.icl(fb, 0) == 3) for (l = 0; l < dimension; l++) //face de Dirichlet -> on convecte la vitesse au bord
                    resu(f) -= fac * fs(f) * (xv(f, l) - xp(e, l)) * ref_cast(Dirichlet, cls[ch.icl(fb, 1)].valeur()).val_imp(ch.icl(fb, 2), l);
              }
            else for (l = zone.vedeb(eb); l < zone.vedeb(eb + 1); l++) //face interne sans equivalence -> convection de ve
                fc = zone.veji(l), resu(f) -= fac * fs(f) * zone.dot(&xv(f, 0), &zone.veci(l, 0), &xp(e, 0)) * pf(fc) * inco(fc);
          }
  return resu;
}

//Description:
//on assemble la matrice.

inline void Op_Conv_EF_Stab_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &vfd = zone.volumes_entrelaces_dir(), &vit = vitesse_->valeurs();
  const DoubleVect& fs = zone.face_surfaces_const(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &pf = zone.porosite_face();
  int i, j, k, l, e, eb, f, fb, fc, fd, fam;

  //element e -> contribution de la face fb a l'equation a la face f
  for (e = 0; e < zone.nb_elem_tot(); e++)  for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      for (j = 0; f < zone.nb_faces() && ch.icl(f, 0) < 2 && j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
        for (k = 0; (ch.icl(fb, 0) < 2 || ch.icl(fb, 0) == 3) && k < 2; k++) //amont/aval de fb
          {
            eb = f_e(fb, k); //element amont/aval de fb (toujours l'amont si Neumann)
            double fac = (e == f_e(f, 0) ? 1 : -1) * vit(fb) * (e == f_e(fb, 0) ? 1 : -1) * fs(fb) / ve(e) * (1. + (vit(fb) * (k ? -1. : 1) >= 0 ? 1. : -1.) * alpha) / 2;
            if ((fc = equiv(fb, e != f_e(fb, 0), i)) >= 0 || f_e(fb, 0) < 0 || f_e(fb, 1) < 0) //equivalence ou bord -> on convecte m2
              {
                if (eb >= 0) for (fam = eb == e ? f : fc, l = zone.m2deb(fam); l < zone.m2deb(fam + 1); l++) if (zone.m2ji(l, 1) == eb && ch.icl(fd = zone.m2ji(l, 0), 0) < 2) //convection de m2
                      matrice(f, fd) += fac * (eb == f_e(fam, 0) ? 1 : -1) * zone.m2ci(l) * vfd(f, e != f_e(f, 0)) / vfd(fam, eb != f_e(fam, 0)) * pf(fd);
              }
            else for (l = zone.vedeb(eb); l < zone.vedeb(eb + 1); l++) //face interne sans equivalence -> convection de ve
                if (ch.icl(fc = zone.veji(l), 0) < 2 && dabs(zone.dot(&xv(f, 0), &zone.veci(l, 0), &xp(e, 0))) > 1e-8 * vf(f) / fs(f))
                  matrice(f, fc) += fac * fs(f) * zone.dot(&xv(f, 0), &zone.veci(l, 0), &xp(e, 0)) * pf(fc);
          }
}

//Description:
//on ajoute la contribution du second membre.

void Op_Conv_EF_Stab_PolyMAC_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();

}
