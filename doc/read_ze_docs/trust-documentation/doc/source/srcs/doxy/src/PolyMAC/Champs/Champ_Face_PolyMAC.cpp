/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Linear_algebra_tools_impl.h>
#include <Connectivite_som_elem.h>
#include <Dirichlet_homogene.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Fonc_reprise.h>
#include <Champ_Face_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Domaine_PolyMAC.h>
#include <Domaine_Cl_dis.h>
#include <TRUSTTab_parts.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Domaine_VF.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <EChaine.h>
#include <Domaine.h>
#include <array>
#include <cmath>

Implemente_instanciable(Champ_Face_PolyMAC, "Champ_Face_PolyMAC", Champ_Face_base);

Sortie& Champ_Face_PolyMAC::printOn(Sortie& os) const { return os << que_suis_je() << " " << le_nom(); }

Entree& Champ_Face_PolyMAC::readOn(Entree& is) { return is; }

int Champ_Face_PolyMAC::fixer_nb_valeurs_nodales(int n)
{
  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base sauf que je recupere le nombre de faces au lieu du nombre d'elements
  // je suis tout de meme etonne du code utilise dans Champ_Fonc_P0_base::fixer_nb_valeurs_nodales() pour recuperer le domaine discrete...

  const Champ_Inc_base& self = ref_cast(Champ_Inc_base, *this);
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,self.domaine_dis_base());

  assert(n == domaine.nb_faces() + (dimension < 3 ? domaine.nb_som() : domaine.domaine().nb_aretes()));

  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ = 1;
  creer_tableau_distribue(domaine.mdv_faces_aretes);
  nb_compo_ = old_nb_compo;
  return n;
}

Champ_base& Champ_Face_PolyMAC::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab_parts parts(valeurs());
  DoubleTab& val = parts[0]; //partie vitesses
  const Domaine_VF& domaine_PolyMAC = ref_cast( Domaine_VF,le_dom_VF.valeur());
  int nb_faces = domaine_PolyMAC.nb_faces();
  const DoubleVect& surface = domaine_PolyMAC.face_surfaces();
  const DoubleTab& normales = domaine_PolyMAC.face_normales();

  if (sub_type(Champ_Uniforme,ch))
    {
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double vn=0;
          for (int dir=0; dir<dimension; dir++)
            vn+=v(0,dir)*normales(num_face,dir);

          vn/=surface(num_face);
          val(num_face) = vn;
        }
    }
  else if (sub_type(Champ_Fonc_reprise, ch))
    for (int f = 0; f < nb_faces; f++)
      val(f) = ch.valeurs()(f);
  else
    {
      //      int ndeb_int = domaine_PolyMAC.premiere_face_int();
      //      const IntTab& face_voisins = domaine_PolyMAC.face_voisins();
      const DoubleTab& xv=domaine_PolyMAC.xv();
      DoubleTab eval(val.dimension_tot(0),dimension);
      ch.valeur_aux(xv,eval);
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double vn=0;
          for (int dir=0; dir<dimension; dir++)
            vn+=eval(num_face,dir)*normales(num_face,dir);

          vn/=surface(num_face);
          val(num_face) = vn;
        }
    }
  return *this;
}

DoubleVect& Champ_Face_PolyMAC::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  Cerr << "Champ_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

double Champ_Face_PolyMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  Cerr << "Champ_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

//interpolation de l'integrale de v autour d'une arete duale, multipliee par la longueur de l'arete
void Champ_Face_PolyMAC::init_ra() const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const IntTab& a_f = domaine.arete_faces(), &e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const DoubleTab& nf = domaine.face_normales(), &ta = domaine.ta(), &xs = domaine.domaine().coord_sommets(), &xa = dimension < 3 ? xs : domaine.xa(), &xv = domaine.xv();
  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes();
  int i, j, k, l, r, e, f, fb, a, idx;

  if (radeb.dimension(0)) return;
  init_fcl(), domaine.init_m2(), init_va();
  radeb.resize(1, 2), racf.resize(0, 3);
  radeb.set_smart_resize(1), raji.set_smart_resize(1), rajf.set_smart_resize(1);
  raci.set_smart_resize(1), racf.set_smart_resize(1);
  std::map<int, double> rami;
  std::map<int, std::array<double, 3> > ramf;
  for (a = 0; a < xa.dimension_tot(0); radeb.append_line(raji.dimension(0), rajf.dimension(0)), rami.clear(), ramf.clear(), a++)
    {
      //contribution de chaque face entourant l'arete
      for (i = 0; i < a_f.dimension(1) && (f = a_f(a, i)) >= 0; i++)
        {
          //sens par rapport a l'arete
          std::array<double, 3> taz = {{ 0, 0, 1 }}, vec; //vecteur tangent a l'arete et produit vectoriel de celui-ci avec xv - xa
          vec = domaine.cross(3, dimension, dimension < 3 ? &taz[0] : &ta(a, 0), &xv(f, 0), NULL, dimension < 3 ? &xs(a, 0): &xa(a, 0));
          int sgn = domaine.dot(&vec[0], &nf(f, 0)) > 0 ? 1 : -1;

          //partie m2
          for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
            for (k = domaine.m2d(e), idx = 0; k < domaine.m2d(e + 1); k++, idx++)
              for (l = domaine.m2i(k); f == e_f(e, idx) && l < domaine.m2i(k + 1); l++)
                if (fcl_(fb = e_f(e, domaine.m2j(l)), 0) < 2) rami[fb] += sgn * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * ve(e) * domaine.m2c(l) / fs(f);
                else if (fcl_(fb, 0) == 3)
                  for (r = 0; r < dimension; r++)
                    ramf[fb][r] += sgn * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * ve(e) * domaine.m2c(l) / fs(f) * nf(fb, r) / fs(fb);
          //partie "bord" -> avec va si Neumann ou Symetrie, avec val_imp si Dirichlet_homogene
          if (fcl_(f, 0) == 1 || fcl_(f, 0) == 2)
            {
              for (k = vadeb(a, 0); k < vadeb(a + 1, 0); k++) rami[vaji(k)] += sgn * domaine.dot(&xa(a, 0), &vaci(k, 0), &xv(f, 0));
              for (k = vadeb(a, 1); k < vadeb(a + 1, 1); k++) ramf[vajf(k, 0)][vajf(k, 1)] += sgn * domaine.dot(&xa(a, 0), &vacf(k, 0), &xv(f, 0));
            }
          else if (fcl_(f, 0) == 3)
            for (k = 0; k < dimension; k++)
              ramf[f][k] += sgn * (xa(a, k) - xv(f, k));
        }
      //remplissage
      double la = dimension < 3 ? 1 : domaine.longueur_aretes()(a);
      for (auto &&kv : rami)
        if (std::fabs(kv.second) > 1e-8 * la)
          raji.append_line(kv.first), raci.append_line(kv.second * la);
      for (auto &&kv : ramf)
        if (domaine.dot(&kv.second[0], &kv.second[0]) > 1e-16 * la * la)
          rajf.append_line(kv.first), racf.append_line(kv.second[0] * la, kv.second[1] * la, kv.second[2] * la);
    }
  CRIMP(radeb), CRIMP(raji), CRIMP(rajf), CRIMP(raci), CRIMP(racf);
}

//interpolation aux aretes de la vitesse (dans le plan normal a chaque arete)
void Champ_Face_PolyMAC::init_va() const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const IntTab& f_e = domaine.face_voisins(), &a_f = domaine.arete_faces();
  const DoubleTab& xv = domaine.xv(), &ta = domaine.ta(), &xs = domaine.domaine().coord_sommets(),
                   &xa = dimension < 3 ? xs : domaine.xa(), &nf = domaine.face_normales(), &xp = domaine.xp();
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face(), &pe = equation().milieu().porosite_elem();

  int i, j, k, l, m, e, f, fb, a;

  if (vadeb.dimension(0)) return;
  domaine.init_m1(), domaine.init_ve();
  std::map<int, std::array<double, 3>> vami;
  std::map<std::array<int, 2>, std::array<double, 3>> vamf;
  vadeb.resize(1, 2), vajf.resize(0, 2), vaci.resize(0, 3), vacf.resize(0, 3);
  vadeb.set_smart_resize(1), vaji.set_smart_resize(1), vajf.set_smart_resize(1);
  vaci.set_smart_resize(1), vacf.set_smart_resize(1);
  Matrice33 M, iM;
  for (a = 0; a < xa.dimension_tot(0); vadeb.append_line(vaji.dimension(0), vajf.dimension(0)), vami.clear(), vamf.clear(), a++)
    {
      /* liste des faces ordonnee dans le sens trigo */
      //base locale
      std::array<double, 3> taz = {{ 0, 0, 1 }}, v0 = domaine.cross(3, dimension, dimension < 3 ? &taz[0] : &ta(a, 0), &xv(a_f(a, 0), 0), NULL, &xa(a, 0)),
      v1 = domaine.cross(3, 3, dimension < 3 ? &taz[0] : &ta(a, 0), &v0[0]);
      // liste des faces, ordonnee en tournant dans la base locale
      std::map<double, int> fmap;
      for (i = 0; i < a_f.dimension(1) && (f = a_f(a, i)) >= 0; i++) fmap[atan2(domaine.dot(&xv(f, 0), &v1[0], &xa(a, 0)), domaine.dot(&xv(f, 0), &v0[0], &xa(a, 0)))] = f;
      std::vector<int> fas, sgn;
      for (auto && kv : fmap) fas.push_back(kv.second);
      //orientation des faces
      /* calcul de l'interpolation */
      DoubleTrav xe((int)fas.size(), dimension), surf((int)fas.size(), 2), nsurf((int)fas.size(), 2, 3); //points intermediaires entre les xv, surfaces et normales des triangles de l'arete duale
      double surf_tot = 0, xg[3] = { 0, 0, 0 }; //surface totale, centre de gravite, normale a l'arete
      //orientations des faces par rapport au contour
      for (i = 0; i < (int) fas.size(); i++)
        {
          std::array <double, 3> vec = domaine.cross(3, dimension, dimension < 3 ? &taz[0] : &ta(a, 0), &xv(fas[i], 0), NULL, &xa(a, 0));
          sgn.push_back(domaine.dot(&vec[0], &nf(fas[i], 0)) > 0 ? 1 : -1);
        }
      for (i = 0; i < (int) fas.size(); i++)
        {
          //points intermediaires entre faces (pas forcement xp)
          int fa[2] = { f = fas[i], fb = fas[(i + 1) * (i + 1 < (int) fas.size())] }, sgfa[2] = { sgn[i], sgn[(i + 1) * (i + 1 < (int) fas.size())] };
          if (domaine.dot(&xv(fb, 0), &nf(f, 0), &xv(f, 0)) * sgn[i] < 0) //angle > 180 -> on passe par xa
            for (k = 0; k < dimension; k++) xe(i, k) = xa(a, k);
          else if (domaine.dot(&xv(f, 0), &xv(fb, 0), &xa(a, 0), &xa(a, 0)) - (dimension < 3 ? 0 : domaine.dot(&xv(f, 0), &ta(a, 0), &xa(a, 0)) * domaine.dot(&xv(fb, 0), &ta(a, 0), &xa(a, 0))) <= 0) //angle obtus -> parallelogramme
            for (k = 0; k < dimension; k++) xe(i, k) = xv(f, k) + xv(fb, k) - xa(a, k);
          else //angle aigu -> intersection des normales aux faces. On resout (xe - xv).(xv - xa) = 0 pour les deux faces, et xe.ta = ta.(xf + xfb) / 2 en 3D
            {
              double s[3];
              for (j = 0; j < 2; j++)
                for (k = 0, s[j] = domaine.dot(&xv(fa[j], 0), &xv(fa[j], 0), &xa(a, 0), &xa(a, 0)); k < 3; k++)
                  M(j, k) = k < dimension ? xv(fa[j], k) - xa(a, k) : 0;
              for (k = 0, s[2] = dimension < 3 ? 0 : (domaine.dot(&xv(f, 0), &ta(a, 0), &xa(a, 0)) + domaine.dot(&xv(fb, 0), &ta(a, 0), &xa(a, 0))) / 2; k < 3; k++)
                M(2, k) = dimension < 3 ? (k == 2) : ta(a, k);
              double eps = Matrice33::inverse(M, iM, 0);
              for (j = 0; eps != 0 && j < dimension; j++)
                for (k = 0, xe(i, j) = xa(a, j); k < dimension; k++) xe(i, j) += iM(j, k) * s[k];
              //si xe se retrouve du mauvais cote d'une des faces, on prend xp (si l'element existe) ou xf + xfb / 2 (sinon)
              if (eps == 0 || domaine.dot(&xe(i, 0), &nf(f, 0), &xv(f, 0)) * sgfa[0] < 0 || domaine.dot(&xe(i, 0), &nf(fb, 0), &xv(fb, 0)) * sgfa[1] > 0)
                {
                  if ((e = f_e(f, sgfa[0] > 0)) >= 0)
                    for (k = 0; k < dimension; k++) xe(i, k) = xp(e, k);
                  else for (k = 0; k < dimension; k++) xe(i, k) = (xv(f, k) + xv(fb, k)) / 2;
                }
            }

          //surfaces et centre de gravite
          for (j = 0; j < 2; j++)
            {
              std::array<double, 3> vsurf3 = domaine.cross(dimension, dimension, &xe(i, 0), &xv(fa[j], 0), &xa(a, 0), &xa(a, 0));
              //orientation par rapport a ta
              for (k = 0, l = (dimension < 3 ? vsurf3[2] < 0 : domaine.dot(&ta(a, 0), &vsurf3[0]) < 0); k < 3; k++) vsurf3[k] *=  (l ? -1. : 1.) / 2;
              surf(i, j) = dimension < 3 ? std::fabs(vsurf3[2]) : sqrt(domaine.dot(&vsurf3[0], &vsurf3[0]));
              surf_tot += surf(i, j);
              for (k = 0; surf(i, j) > 1e-16 && k < 3; k++) nsurf(i, j, k) = vsurf3[k] / surf(i, j);
              for (k = 0; k < dimension; k++) xg[k] += surf(i, j) * (xv(fa[j], k) + xe(i, k) - 2 * xa(a, k)) / 3;
            }
        }
      for (k = 0; k < dimension; k++) xg[k] = xa(a, k) + (surf_tot ==0 ? 0. : xg[k] / surf_tot);

      if (!surf_tot) continue;
      M = Matrice33(surf_tot, 0, 0, 0, surf_tot, 0, 0, 0, surf_tot);
      /* boucles sur les triangles de l'arete duale */
      std::array<double, 3> vec;
      for (i = 0; i < (int) fas.size(); i++)
        for (j = 0; j < 2; j++)
          if (surf(i, j) > 1e-16)
            {
              f = fas[(i + j) * (i + j < (int) fas.size())], e = f_e(f, sgn[i] > 0); //face, element (si il existe)
              double x[2][3] = { { 0, }, }; //points sur le contour : xv(fa[i]) -> xe(i) (j = 0) ou xe(i) -> xv (fa[i + 1]) (j = 1)
              for (k = 0; k < 2; k++)
                for (l = 0; l < dimension; l++) x[k][l] = j == k ? xv(f, l) : xe(i, l);

              //segments arete-points : avec va
              for (k = 0; k < 2; k++)
                {
                  for (l = 0; l < dimension; l++) vec[l] = (xa(a, l) + x[k][l]) / 2 - xg[l];
                  vec = domaine.cross(3, dimension, &nsurf(i, j, 0), &vec[0]);
                  for (l = 0; l < 3; l++)
                    for (m = 0; m < dimension; m++) M(l, m) -= (k ? -1 : 1) * (x[k][m] - xa(a, m)) * vec[l];
                }

              //segment x[0] -> x[1]
              for (k = 0; k < dimension; k++) vec[k] = (x[0][k] + x[1][k]) / 2 - xg[k];
              vec = domaine.cross(3, dimension, &nsurf(i, j, 0), &vec[0]);
              //partie vf
              if (fcl_(f, 0) < 2)
                for (k = 0; k < dimension; k++)
                  vami[f][k] += domaine.dot(x[1], &nf(f, 0), x[0]) / fs(f) * vec[k];
              else if (fcl_(f, 0) == 3)
                for (k = 0; k < dimension; k++)
                  for (l = 0; l < dimension; l++)
                    vamf[ {{ f, l }}][l] += domaine.dot(x[1], &nf(f, 0), x[0]) / fs(f) * vec[k] * nf(f, l) / fs(f);
              //complement : avec ve (e >= 0) / val_imp (Dirichlet) / va (Neumann/Symetrie)
              if (e >= 0)
                for (k = domaine.vedeb(e); k < domaine.vedeb(e + 1); k++) //ve
                  {
                    if (fcl_(fb = domaine.veji(k), 0) < 2)
                      for (l = 0; l < dimension; l++)
                        vami[fb][l] += (domaine.dot(x[1], &domaine.veci(k, 0), x[0]) - domaine.dot(x[1], &nf(f, 0), x[0]) * domaine.dot(&domaine.veci(k, 0), &nf(f, 0)) / (fs(f) * fs(f))) * vec[l] * pf(fb) / pe(e);
                    else if (fcl_(fb, 0) == 3)
                      for (l = 0; l < dimension; l++)
                        for (m = 0; m < dimension; m++)
                          vamf[ {{ fb, m }}][l] += (domaine.dot(x[1], &domaine.veci(k, 0), x[0]) - domaine.dot(x[1], &nf(f, 0), x[0]) * domaine.dot(&domaine.veci(k, 0), &nf(f, 0)) / (fs(f) * fs(f)))
                    * vec[l] * nf(fb, m) / fs(f) * pf(fb) / pe(e);
                  }
              else if (fcl_(f, 0) == 3)
                for (k = 0; k < dimension; k++)
                  for (l = 0; l < dimension; l++) //val_imp
                    vamf[ {{ f, k }}][l] += (x[1][k] - x[0][k] - domaine.dot(x[1], &nf(f, 0), x[0]) * nf(f, k) / (fs(f) * fs(f))) * vec[l];
              else if (fcl_(f, 0) == 1 || fcl_(f, 0) == 2)
                for (k = 0; k < dimension; k++)
                  for (l = 0; l < dimension; l++) //va
                    M(k, l) -= (x[1][l] - x[0][l] - domaine.dot(x[1], &nf(f, 0), x[0]) * nf(f, l) / (fs(f) * fs(f))) * vec[k];

              //partie normale au triangle (3D seulement): avec vf/ve/val_imp egalement
              if (dimension < 3) continue;
              else if (e >= 0)
                for (k = domaine.vedeb(e); k < domaine.vedeb(e + 1); k++) //ve
                  {
                    if (fcl_(fb = domaine.veji(k), 0) < 2)
                      for (l = 0; l < dimension; l++)
                        vami[fb][l] += surf(i, j) * domaine.dot(&nsurf(i, j, 0), &domaine.veci(k, 0)) * nsurf(i, j, l) * pf(fb) / pe(e);
                    else if (fcl_(fb, 0) == 3)
                      for (l = 0; l < dimension; l++)
                        for (m = 0; m < dimension; m++)
                          vamf[ {{ fb, m }}][l] += surf(i, j) * domaine.dot(&nsurf(i, j, 0), &domaine.veci(k, 0)) * nsurf(i, j, l) * nf(fb, m) / fs(f) * pf(fb) / pe(e);
                  }
              else if (fcl_(f, 0) == 3)
                for (k = 0; k < dimension; k++)
                  for (l = 0; l < dimension; l++) //val_imp
                    vamf[ {{ f, k }}][l] += surf(i, j) * nsurf(i, j, k) * nsurf(i, j, l);
              else if (fcl_(f, 0) == 1 || fcl_(f, 0) == 2)
                for (k = 0; k < dimension; k++)
                  for (l = 0; l < dimension; l++) //va
                    M(k, l) -= surf(i, j) * nsurf(i, j, k) * nsurf(i, j, l);
            }

      //inversion de M et remplissage
      double eps = Matrice33::inverse(M, iM, 0);
      if (std::fabs(eps) < 1e-24) continue;
      for (auto && kv : vami)
        {
          double inc[3] = { 0, 0, 0 };
          for (k = 0; k < dimension; k++)
            for (l = 0; l < dimension; l++) inc[k] += iM(k, l) * kv.second[l];
          if (domaine.dot(inc, inc) > 1e-16) vaji.append_line(kv.first), vaci.append_line(inc[0], inc[1], inc[2]);
        }
      for (auto && kv : vamf)
        {
          double inc[3] = { 0, 0, 0 };
          for (k = 0; k < dimension; k++)
            for (l = 0; l < dimension; l++) inc[k] += iM(k, l) * kv.second[l];
          if (domaine.dot(inc, inc) > 1e-16) vajf.append_line(kv.first[0], kv.first[1]), vacf.append_line(inc[0], inc[1], inc[2]);
        }
    }
  CRIMP(vadeb), CRIMP(vaji), CRIMP(vajf), CRIMP(vaci), CRIMP(vacf);
}

/* vitesse aux elements */
void Champ_Face_PolyMAC::interp_ve(const DoubleTab& inco, DoubleTab& val, bool is_vit) const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites();
  const DoubleTab& nf = domaine.face_normales();
  const DoubleVect& fs = domaine.face_surfaces(), *pf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : NULL, *pe = pf ? &equation().milieu().porosite_elem() : NULL;

  int e, f, j, k, r;

  domaine.init_ve();
  val = 0;
  for (e = 0; e < val.dimension(0); e++)
    for (j = domaine.vedeb(e); j < domaine.vedeb(e + 1); j++)
      if (fcl_(f = domaine.veji(j), 0) < 2) //vitesse calculee
        {
          const double coef = is_vit && pf ? (*pf)(f) / (*pe)(e) : 1.0;
          for (r = 0; r < dimension; r++) val(e, r) += domaine.veci(j, r) * inco(f) * coef;
        }
      else if (fcl_(f, 0) == 3)
        for (k = 0; k < dimension; k++)
          for (r = 0; r < dimension; r++) //Dirichlet
            {
              const double coef = is_vit && pf ? (*pf)(f) / (*pe)(e) : 1.0;
              val(e, r) += domaine.veci(j, r) * ref_cast(Dirichlet, cls[fcl_(f, 1)].valeur()).val_imp(fcl_(f, 2), k) * nf(f, k) / fs(f) * coef;
            }
}

/* vitesse aux elements sur une liste d'elements */
void Champ_Face_PolyMAC::interp_ve(const DoubleTab& inco, const IntVect& les_polys, DoubleTab& val, bool is_vit) const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const DoubleVect *pf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : NULL, *pe = pf ? &equation().milieu().porosite_elem() : NULL;
  int e, f, j, r;

  domaine.init_ve();
  for (int poly = 0; poly < les_polys.size(); poly++)
    {
      e = les_polys(poly);
      if (e!=-1)
        {
          for (r = 0; r < dimension; r++)
            val(e, r) = 0;
          for (j = domaine.vedeb(e); j < domaine.vedeb(e + 1); j++)
            {
              f = domaine.veji(j);
              const double coef = is_vit && pf ? (*pf)(f) / (*pe)(e) : 1.0;
              for (r = 0; r < dimension; r++) val(e, r) += domaine.veci(j, r) * inco(f) * coef;
            }
        }
    }
}

/* gradient d_j v_i aux elements */
void Champ_Face_PolyMAC::interp_gve(const DoubleTab& inco, DoubleTab& vals) const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites();
  const IntTab& f_e = domaine.face_voisins();
  const DoubleTab& nf = domaine.face_normales();
  const DoubleVect& fs = domaine.face_surfaces();
  int i, j, k, f, e;
  double scal;

  domaine.init_m2solv();
  //vitesses aux elements, gradient de chaque composante aux faces
  DoubleTrav ve1(0, dimension);
  domaine.domaine().creer_tableau_elements(ve1);
  interp_ve(inco, ve1);

  //gradient aux faces duales, champ (nf.grad)v (obtenu en inversant m2)
  DoubleTrav gv(domaine.nb_faces_tot(), dimension), cgv, ngv;
  domaine.creer_tableau_faces(cgv), domaine.creer_tableau_faces(ngv);
  for (f = 0; f < domaine.nb_faces_tot(); f++)
    if (fcl_(f, 0) != 1) //gve = 0 si Neumann
      {
        for (i = 0; i < 2; i++)
          if ((e = f_e(f, i)) >= 0)
            for (k = 0; k < dimension; k++) gv(f, k) += (i ? 1 : -1) * fs(f) * ve1(e, k); //element interne -> avec ve1
          else if (fcl_(f, 0) == 3)
            for (k = 0; k < dimension; k++) //bord de Dirichlet -> avec val_imp
              gv(f, k) += (i ? 1 : -1) * fs(f) * ref_cast(Dirichlet, cls[fcl_(f, 1)].valeur()).val_imp(fcl_(f, 2), k);
        //si Symetrie -> on ne garde que la composante normale a la face
        if (fcl_(f, 0) == 2)
          for (k = 0, scal = domaine.dot(&nf(f, 0), &gv(f, 0)) / fs(f); k < dimension; k++) gv(f, k) = scal * nf(f, k) / fs(f);
      }

  //pour chaque composante :
  for (k = 0; k < dimension; k++)
    {
      /* inversion de m2 -> pour obtenir (nf.grad) v_k */
      for (f = 0; f < domaine.nb_faces_tot(); f++) cgv(f) = gv(f, k);
      domaine.m2solv.resoudre_systeme(domaine.m2mat, cgv, ngv);
      /* reinterpolation aux elements -> grad v_k */
      for (e = 0; e < domaine.nb_elem(); e++)
        for (i = domaine.vedeb(e); i < domaine.vedeb(e + 1); i++)
          for (j = 0; j < dimension; j++)
            vals.addr()[(e * dimension + k) * dimension + j] += domaine.veci(i, j) * ngv(domaine.veji(i));
    }

  vals.echange_espace_virtuel();
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_elems_(const DoubleTab& val_face, const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  const Champ_base& cha = le_champ();
  int nb_compo = cha.nb_comp(), N = val_face.line_size(), D = dimension;
  assert(val_elem.line_size() == nb_compo * N);
  assert((positions.dimension(0) == les_polys.size()) || (positions.dimension_tot(0) == les_polys.size()));

  if (val_elem.nb_dim() > 2)
    Process::exit("TRUST error in Champ_Face_PolyMAC::valeur_aux_elems_ : The DoubleTab val has more than 2 entries !");

  if (nb_compo == 1)
    Process::exit("TRUST error in Champ_Face_PolyMAC::valeur_aux_elems_ : A scalar field cannot be of Champ_Face type !");

  // seulement si Champ_Face_PolyMAC car interp_ve est besoin de mon_dom_cl_dis ...
  if (mon_dom_cl_dis.est_nul() && que_suis_je() == "Champ_Face_PolyMAC")
    return val_elem; //on ne peut rien faire tant qu'on ne connait pas les CLs

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  DoubleTrav ve(0, N * D);
  const Domaine_VF& domdom = ref_cast(Domaine_VF, domaine_vf());
  domdom.domaine().creer_tableau_elements(ve);

  bool is_vit = cha.le_nom().debute_par("vitesse") && !cha.le_nom().debute_par("vitesse_debitante");
  interp_ve(val_face, ve, is_vit);

  for (int p = 0; p < les_polys.size(); p++)
    for (int r = 0, e = les_polys(p); e < domdom.nb_elem() && r < N * D; r++)
      val_elem(p, r) = (e == -1) ? 0. : ve(e, r);
  return val_elem;
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  return valeur_aux_elems_(le_champ().valeurs(), positions, les_polys, val_elem);
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_elems_passe(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  return valeur_aux_elems_(le_champ().passe(), positions, les_polys, val_elem);
}

DoubleVect& Champ_Face_PolyMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  fcl();
  const Champ_base& cha=le_champ();
  assert(val.size() == les_polys.size());

  // seulement si Champ_Face_PolyMAC car interp_ve est besoin de mon_dom_cl_dis ...
  if (mon_dom_cl_dis.est_nul() && que_suis_je() ==  "Champ_Face_PolyMAC")
    return val; //on ne peut rien faire tant qu'on ne connait pas les CLs

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  DoubleTrav ve(0, dimension * cha.valeurs().line_size());
  ref_cast(Domaine_VF, domaine_vf()).domaine().creer_tableau_elements(ve);
  interp_ve(cha.valeurs(), ve);

  for (int p = 0; p < les_polys.size(); p++) val(p) = (les_polys(p) == -1) ? 0. : ve(les_polys(p), ncomp);

  return val;
}

DoubleTab& Champ_Face_PolyMAC::remplir_coord_noeuds(DoubleTab& positions) const
{
  Cerr << "Champ_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

int Champ_Face_PolyMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{
  Cerr << "Champ_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();

  if (nb_compo == 1)
    Process::exit("Champ_Face_PolyMAC::valeur_aux_faces : A scalar field cannot be of Champ_Face type !");

  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  val.resize(domaine.nb_faces(), dimension), val = 0;

  for (int f = 0; f < domaine.nb_faces(); f++)
    for (int r = 0; r < dimension; r++) val(f, r) = cha.valeurs()(f) * domaine.face_normales(f, r) / domaine.face_surfaces(f);
  return val;
}

DoubleVect& Champ_Face_PolyMAC::calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  // avec contrib au bord pour l'instant...
  abort();
  return calcul_S_barre(vitesse, SMA_barre);
}

DoubleVect& Champ_Face_PolyMAC::calcul_S_barre(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC, domaine_vf());
  DoubleTab duidxj(0, dimension, dimension);
  domaine.domaine().creer_tableau_elements(duidxj);
  interp_gve(vitesse, duidxj);

  for (int elem = 0; elem < domaine.domaine().nb_elem(); elem++)
    {
      double temp = 0.;
      for (int i = 0; i < dimension; i++)
        for (int j = 0; j < dimension; j++)
          {
            double Sij = 0.5 * (duidxj(elem, i, j) + duidxj(elem, j, i));
            temp += Sij * Sij;
          }
      SMA_barre(elem) = 2. * temp;
    }

  return SMA_barre;

}

DoubleTab& Champ_Face_PolyMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant==0);
  init_fcl();
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1;
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const IntTab& face_voisins = domaine.face_voisins();
  const DoubleTab& val = valeurs(t);
  DoubleTrav ve(0, dimension);
  if (vectoriel)
    {
      domaine.domaine().creer_tableau_elements(ve);
      interp_ve(val, ve);
    }

  for (int i = 0; i < fr_vf.nb_faces(); i++)
    {
      const int face = fr_vf.num_premiere_face() + i;
      for (int dir = 0; dir < 2; dir++)
        {
          const int elem = face_voisins(face, dir);
          if (elem != -1)
            {
              for (int d = 0; d < dim; d++) x(i, d) = vectoriel ? ve(elem, d) : val[face];
            }
        }
    }

  return x;
}

int Champ_Face_PolyMAC::nb_valeurs_nodales() const
{
  ConstDoubleTab_parts val_part(valeurs());
  return val_part[0].dimension(0) + val_part[1].dimension(0);
}
