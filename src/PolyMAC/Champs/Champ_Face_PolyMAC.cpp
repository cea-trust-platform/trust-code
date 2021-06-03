/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Champ_Face_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Face_PolyMAC.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_reprise.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Zone_Cl_PolyMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Connectivite_som_elem.h>
#include <Matrix_tools.h>
#include <EChaine.h>
#include <ConstDoubleTab_parts.h>
#include <Linear_algebra_tools_impl.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <array>
#include <cmath>

Implemente_instanciable(Champ_Face_PolyMAC,"Champ_Face_PolyMAC",Champ_Inc_base) ;

Sortie& Champ_Face_PolyMAC::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Face_PolyMAC::readOn(Entree& is)
{
  return is;
}

Champ_base& Champ_Face_PolyMAC::le_champ(void)
{
  return *this;
}

const Champ_base& Champ_Face_PolyMAC::le_champ(void) const
{
  return *this;
}

void Champ_Face_PolyMAC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  ref_zone_vf_=ref_cast(Zone_VF, z_dis);
}


int Champ_Face_PolyMAC::fixer_nb_valeurs_nodales(int n)
{

  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base
  // sauf que je recupere le nombre de faces au lieu du nombre d'elements
  //
  // je suis tout de meme etonne du code utilise dans
  // Champ_Fonc_P0_base::fixer_nb_valeurs_nodales()
  // pour recuperer la zone discrete...

  const Champ_Inc_base& self = ref_cast(Champ_Inc_base, *this);
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,self.zone_dis_base());

  assert(n == zone.nb_faces() + (dimension < 3 ? zone.nb_som() : zone.zone().nb_aretes()));

  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ = 1;
  creer_tableau_distribue(zone.mdv_faces_aretes);
  nb_compo_ = old_nb_compo;
  return n;

}
Champ_base& Champ_Face_PolyMAC::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab_parts parts(valeurs());
  DoubleTab& val = parts[0]; //partie vitesses
  const Zone_PolyMAC& zone_PolyMAC = ref_cast( Zone_PolyMAC,ref_zone_vf_.valeur());
  int nb_faces = zone_PolyMAC.nb_faces();
  const DoubleVect& surface = zone_PolyMAC.face_surfaces();
  const DoubleTab& normales = zone_PolyMAC.face_normales();


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
      //      int ndeb_int = zone_PolyMAC.premiere_face_int();
      //      const IntTab& face_voisins = zone_PolyMAC.face_voisins();
      const DoubleTab& xv=zone_PolyMAC.xv();
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
  throw;
  // return Champ_implementation_RT0::valeur_a_elem(position,result,poly);
}

double Champ_Face_PolyMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  throw;
  //return Champ_implementation_RT0::valeur_a_elem_compo(position,poly,ncomp);
}

//tableaux de correspondance pour les CLs
void Champ_Face_PolyMAC::init_cl() const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  int i, f, n;

  if (icl.dimension(0)) return;
  icl.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Neumann, cls[n].valeur()) + sub_type(Neumann_homogene, cls[n].valeur())
                + 2 * sub_type(Symetrie, cls[n].valeur())
                + 3 * sub_type(Dirichlet, cls[n].valeur()) + 4 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx) Cerr << "Champ_Face_PolyMAC : CL non codee rencontree!" << finl, Process::exit();
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), icl(f, 0) = idx, icl(f, 1) = n, icl(f, 2) = i;
    }
  CRIMP(icl);
}

//interpolation de l'integrale de v autour d'une arete duale, multipliee par la longueur de l'arete
void Champ_Face_PolyMAC::init_ra() const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const IntTab& a_f = zone.arete_faces(), &e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &ta = zone.ta(), &xs = zone.zone().domaine().coord_sommets(), &xa = dimension < 3 ? xs : zone.xa(), &xv = zone.xv();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes();
  int i, j, k, l, r, e, f, fb, a, idx;

  if (radeb.dimension(0)) return;
  init_cl(), zone.init_m2(), init_va();
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
          vec = zone.cross(3, dimension, dimension < 3 ? &taz[0] : &ta(a, 0), &xv(f, 0), NULL, dimension < 3 ? &xs(a, 0): &xa(a, 0));
          int sgn = zone.dot(&vec[0], &nf(f, 0)) > 0 ? 1 : -1;

          //partie m2
          for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++) for (k = zone.m2d(e), idx = 0; k < zone.m2d(e + 1); k++, idx++) for (l = zone.m2i(k); f == e_f(e, idx) && l < zone.m2i(k + 1); l++)
                if (icl(fb = e_f(e, zone.m2j(l)), 0) < 2) rami[fb] += sgn * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * ve(e) * zone.m2c(l) / fs(f);
                else if (icl(fb, 0) == 3) for (r = 0; r < dimension; r++)
                    ramf[fb][r] += sgn * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * ve(e) * zone.m2c(l) / fs(f) * nf(fb, r) / fs(fb);
          //partie "bord" -> avec va si Neumann ou Symetrie, avec val_imp si Dirichlet_homogene
          if (icl(f, 0) == 1 || icl(f, 0) == 2)
            {
              for (k = vadeb(a, 0); k < vadeb(a + 1, 0); k++) rami[vaji(k)] += sgn * zone.dot(&xa(a, 0), &vaci(k, 0), &xv(f, 0));
              for (k = vadeb(a, 1); k < vadeb(a + 1, 1); k++) ramf[vajf(k, 0)][vajf(k, 1)] += sgn * zone.dot(&xa(a, 0), &vacf(k, 0), &xv(f, 0));
            }
          else if (icl(f, 0) == 3) for (k = 0; k < dimension; k++)
              ramf[f][k] += sgn * (xa(a, k) - xv(f, k));
        }
      //remplissage
      double la = dimension < 3 ? 1 : zone.longueur_aretes()(a);
      for (auto &&kv : rami) if (dabs(kv.second) > 1e-8 * la)
          raji.append_line(kv.first), raci.append_line(kv.second * la);
      for (auto &&kv : ramf) if (zone.dot(&kv.second[0], &kv.second[0]) > 1e-16 * la * la)
          rajf.append_line(kv.first), racf.append_line(kv.second[0] * la, kv.second[1] * la, kv.second[2] * la);
    }
  CRIMP(radeb), CRIMP(raji), CRIMP(rajf), CRIMP(raci), CRIMP(racf);
}

//interpolation aux aretes de la vitesse (dans le plan normal a chaque arete)
void Champ_Face_PolyMAC::init_va() const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const IntTab& f_e = zone.face_voisins(), &a_f = zone.arete_faces();
  const DoubleTab& xv = zone.xv(), &ta = zone.ta(), &xs = zone.zone().domaine().coord_sommets(),
                   &xa = dimension < 3 ? xs : zone.xa(), &nf = zone.face_normales(), &xp = zone.xp();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int i, j, k, l, m, e, f, fb, a;

  if (vadeb.dimension(0)) return;
  zone.init_m1(), zone.init_ve();
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
      std::array<double, 3> taz = {{ 0, 0, 1 }}, v0 = zone.cross(3, dimension, dimension < 3 ? &taz[0] : &ta(a, 0), &xv(a_f(a, 0), 0), NULL, &xa(a, 0)),
      v1 = zone.cross(3, 3, dimension < 3 ? &taz[0] : &ta(a, 0), &v0[0]);
      // liste des faces, ordonnee en tournant dans la base locale
      std::map<double, int> fmap;
      for (i = 0; i < a_f.dimension(1) && (f = a_f(a, i)) >= 0; i++) fmap[atan2(zone.dot(&xv(f, 0), &v1[0], &xa(a, 0)), zone.dot(&xv(f, 0), &v0[0], &xa(a, 0)))] = f;
      std::vector<int> fas, sgn;
      for (auto && kv : fmap) fas.push_back(kv.second);
      //orientation des faces
      /* calcul de l'interpolation */
      DoubleTrav xe(fas.size(), dimension), surf(fas.size(), 2), nsurf(fas.size(), 2, 3); //points intermediaires entre les xv, surfaces et normales des triangles de l'arete duale
      double surf_tot = 0, xg[3] = { 0, 0, 0 }; //surface totale, centre de gravite, normale a l'arete
      //orientations des faces par rapport au contour
      for (i = 0; i < (int) fas.size(); i++)
        {
          std::array <double, 3> vec = zone.cross(3, dimension, dimension < 3 ? &taz[0] : &ta(a, 0), &xv(fas[i], 0), NULL, &xa(a, 0));
          sgn.push_back(zone.dot(&vec[0], &nf(fas[i], 0)) > 0 ? 1 : -1);
        }
      for (i = 0; i < (int) fas.size(); i++)
        {
          //points intermediaires entre faces (pas forcement xp)
          int fa[2] = { f = fas[i], fb = fas[(i + 1) * (i + 1 < (int) fas.size())] }, sgfa[2] = { sgn[i], sgn[(i + 1) * (i + 1 < (int) fas.size())] };
          if (zone.dot(&xv(fb, 0), &nf(f, 0), &xv(f, 0)) * sgn[i] < 0) //angle > 180 -> on passe par xa
            for (k = 0; k < dimension; k++) xe(i, k) = xa(a, k);
          else if (zone.dot(&xv(f, 0), &xv(fb, 0), &xa(a, 0), &xa(a, 0)) - (dimension < 3 ? 0 : zone.dot(&xv(f, 0), &ta(a, 0), &xa(a, 0)) * zone.dot(&xv(fb, 0), &ta(a, 0), &xa(a, 0))) <= 0) //angle obtus -> parallelogramme
            for (k = 0; k < dimension; k++) xe(i, k) = xv(f, k) + xv(fb, k) - xa(a, k);
          else //angle aigu -> intersection des normales aux faces. On resout (xe - xv).(xv - xa) = 0 pour les deux faces, et xe.ta = ta.(xf + xfb) / 2 en 3D
            {
              double s[3];
              for (j = 0; j < 2; j++) for (k = 0, s[j] = zone.dot(&xv(fa[j], 0), &xv(fa[j], 0), &xa(a, 0), &xa(a, 0)); k < 3; k++)
                  M(j, k) = k < dimension ? xv(fa[j], k) - xa(a, k) : 0;
              for (k = 0, s[2] = dimension < 3 ? 0 : (zone.dot(&xv(f, 0), &ta(a, 0), &xa(a, 0)) + zone.dot(&xv(fb, 0), &ta(a, 0), &xa(a, 0))) / 2; k < 3; k++)
                M(2, k) = dimension < 3 ? (k == 2) : ta(a, k);
              double eps = Matrice33::inverse(M, iM, 0);
              for (j = 0; eps != 0 && j < dimension; j++) for (k = 0, xe(i, j) = xa(a, j); k < dimension; k++) xe(i, j) += iM(j, k) * s[k];
              //si xe se retrouve du mauvais cote d'une des faces, on prend xp (si l'element existe) ou xf + xfb / 2 (sinon)
              if (eps == 0 || zone.dot(&xe(i, 0), &nf(f, 0), &xv(f, 0)) * sgfa[0] < 0 || zone.dot(&xe(i, 0), &nf(fb, 0), &xv(fb, 0)) * sgfa[1] > 0)
                {
                  if ((e = f_e(f, sgfa[0] > 0)) >= 0) for (k = 0; k < dimension; k++) xe(i, k) = xp(e, k);
                  else for (k = 0; k < dimension; k++) xe(i, k) = (xv(f, k) + xv(fb, k)) / 2;
                }
            }

          //surfaces et centre de gravite
          for (j = 0; j < 2; j++)
            {
              std::array<double, 3> vsurf3 = zone.cross(dimension, dimension, &xe(i, 0), &xv(fa[j], 0), &xa(a, 0), &xa(a, 0));
              //orientation par rapport a ta
              for (k = 0, l = (dimension < 3 ? vsurf3[2] < 0 : zone.dot(&ta(a, 0), &vsurf3[0]) < 0); k < 3; k++) vsurf3[k] *=  (l ? -1. : 1.) / 2;
              surf(i, j) = dimension < 3 ? dabs(vsurf3[2]) : sqrt(zone.dot(&vsurf3[0], &vsurf3[0]));
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
      for (i = 0; i < (int) fas.size(); i++) for (j = 0; j < 2; j++) if (surf(i, j) > 1e-16)
            {
              f = fas[(i + j) * (i + j < (int) fas.size())], e = f_e(f, sgn[i] > 0); //face, element (si il existe)
              double x[2][3] = { { 0, }, }; //points sur le contour : xv(fa[i]) -> xe(i) (j = 0) ou xe(i) -> xv (fa[i + 1]) (j = 1)
              for (k = 0; k < 2; k++) for (l = 0; l < dimension; l++) x[k][l] = j == k ? xv(f, l) : xe(i, l);

              //segments arete-points : avec va
              for (k = 0; k < 2; k++)
                {
                  for (l = 0; l < dimension; l++) vec[l] = (xa(a, l) + x[k][l]) / 2 - xg[l];
                  vec = zone.cross(3, dimension, &nsurf(i, j, 0), &vec[0]);
                  for (l = 0; l < 3; l++) for (m = 0; m < dimension; m++) M(l, m) -= (k ? -1 : 1) * (x[k][m] - xa(a, m)) * vec[l];
                }

              //segment x[0] -> x[1]
              for (k = 0; k < dimension; k++) vec[k] = (x[0][k] + x[1][k]) / 2 - xg[k];
              vec = zone.cross(3, dimension, &nsurf(i, j, 0), &vec[0]);
              //partie vf
              if (icl(f, 0) < 2) for (k = 0; k < dimension; k++)
                  vami[f][k] += zone.dot(x[1], &nf(f, 0), x[0]) / fs(f) * vec[k];
              else if (icl(f, 0) == 3) for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++)
                    vamf[ {{ f, l }}][l] += zone.dot(x[1], &nf(f, 0), x[0]) / fs(f) * vec[k] * nf(f, l) / fs(f);
              //complement : avec ve (e >= 0) / val_imp (Dirichlet) / va (Neumann/Symetrie)
              if (e >= 0) for (k = zone.vedeb(e); k < zone.vedeb(e + 1); k++) //ve
                  {
                    if (icl(fb = zone.veji(k), 0) < 2) for (l = 0; l < dimension; l++)
                        vami[fb][l] += (zone.dot(x[1], &zone.veci(k, 0), x[0]) - zone.dot(x[1], &nf(f, 0), x[0]) * zone.dot(&zone.veci(k, 0), &nf(f, 0)) / (fs(f) * fs(f))) * vec[l] * pf(fb) / pe(e);
                    else if (icl(fb, 0) == 3) for (l = 0; l < dimension; l++) for (m = 0; m < dimension; m++)
                          vamf[ {{ fb, m }}][l] += (zone.dot(x[1], &zone.veci(k, 0), x[0]) - zone.dot(x[1], &nf(f, 0), x[0]) * zone.dot(&zone.veci(k, 0), &nf(f, 0)) / (fs(f) * fs(f)))
                    * vec[l] * nf(fb, m) / fs(f) * pf(fb) / pe(e);
                  }
              else if (icl(f, 0) == 3) for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) //val_imp
                    vamf[ {{ f, k }}][l] += (x[1][k] - x[0][k] - zone.dot(x[1], &nf(f, 0), x[0]) * nf(f, k) / (fs(f) * fs(f))) * vec[l];
              else if (icl(f, 0) == 1 || icl(f, 0) == 2) for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) //va
                    M(k, l) -= (x[1][l] - x[0][l] - zone.dot(x[1], &nf(f, 0), x[0]) * nf(f, l) / (fs(f) * fs(f))) * vec[k];

              //partie normale au triangle (3D seulement): avec vf/ve/val_imp egalement
              if (dimension < 3) continue;
              else if (e >= 0) for (k = zone.vedeb(e); k < zone.vedeb(e + 1); k++) //ve
                  {
                    if (icl(fb = zone.veji(k), 0) < 2) for (l = 0; l < dimension; l++)
                        vami[fb][l] += surf(i, j) * zone.dot(&nsurf(i, j, 0), &zone.veci(k, 0)) * nsurf(i, j, l) * pf(fb) / pe(e);
                    else if (icl(fb, 0) == 3) for (l = 0; l < dimension; l++) for (m = 0; m < dimension; m++)
                          vamf[ {{ fb, m }}][l] += surf(i, j) * zone.dot(&nsurf(i, j, 0), &zone.veci(k, 0)) * nsurf(i, j, l) * nf(fb, m) / fs(f) * pf(fb) / pe(e);
                  }
              else if (icl(f, 0) == 3) for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) //val_imp
                    vamf[ {{ f, k }}][l] += surf(i, j) * nsurf(i, j, k) * nsurf(i, j, l);
              else if (icl(f, 0) == 1 || icl(f, 0) == 2) for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) //va
                    M(k, l) -= surf(i, j) * nsurf(i, j, k) * nsurf(i, j, l);
            }

      //inversion de M et remplissage
      double eps = Matrice33::inverse(M, iM, 0);
      if (dabs(eps) < 1e-24) continue;
      for (auto && kv : vami)
        {
          double inc[3] = { 0, 0, 0 };
          for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) inc[k] += iM(k, l) * kv.second[l];
          if (zone.dot(inc, inc) > 1e-16) vaji.append_line(kv.first), vaci.append_line(inc[0], inc[1], inc[2]);
        }
      for (auto && kv : vamf)
        {
          double inc[3] = { 0, 0, 0 };
          for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) inc[k] += iM(k, l) * kv.second[l];
          if (zone.dot(inc, inc) > 1e-16) vajf.append_line(kv.first[0], kv.first[1]), vacf.append_line(inc[0], inc[1], inc[2]);
        }
    }
  CRIMP(vadeb), CRIMP(vaji), CRIMP(vajf), CRIMP(vaci), CRIMP(vacf);
}

/* vitesse aux elements */
void Champ_Face_PolyMAC::interp_ve(const DoubleTab& inco, DoubleTab& val, bool is_vit) const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int e, f, j, k, r;

  zone.init_ve();
  val = 0;
  for (e = 0; e < val.dimension(0); e++) for (j = zone.vedeb(e); j < zone.vedeb(e + 1); j++)
      if (icl(f = zone.veji(j), 0) < 2) //vitesse calculee
        {
          const double coef = is_vit ? pf(f) / pe(e) : 1.0;
          for (r = 0; r < dimension; r++) val(e, r) += zone.veci(j, r) * inco(f) * coef;
        }
      else if (icl(f, 0) == 3) for (k = 0; k < dimension; k++) for (r = 0; r < dimension; r++) //Dirichlet
            {
              const double coef = is_vit ? pf(f) / pe(e) : 1.0;
              val(e, r) += zone.veci(j, r) * ref_cast(Dirichlet, cls[icl(f, 1)].valeur()).val_imp(icl(f, 2), k) * nf(f, k) / fs(f) * coef;
            }
}

/* gradient d_j v_i aux elements */
void Champ_Face_PolyMAC::interp_gve(const DoubleTab& inco, DoubleTab& vals) const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();
  int i, j, k, f, e;
  double scal;

  zone.init_m2solv();
  //vitesses aux elements, gradient de chaque composante aux faces
  DoubleTrav ve1(0, dimension), gve(zone.nb_faces_tot());
  zone.zone().domaine().creer_tableau_elements(ve1);
  interp_ve(inco, ve1);

  //gradient aux faces duales, champ (nf.grad)v (obtenu en inversant m2)
  DoubleTrav gv(zone.nb_faces_tot(), dimension), cgv, ngv;
  zone.creer_tableau_faces(cgv), zone.creer_tableau_faces(ngv);
  for (f = 0; f < zone.nb_faces_tot(); f++) if (icl(f, 0) != 1) //gve = 0 si Neumann
      {
        for (i = 0; i < 2; i++) if ((e = f_e(f, i)) >= 0) for (k = 0; k < dimension; k++) gv(f, k) += (i ? 1 : -1) * fs(f) * ve1(e, k); //element interne -> avec ve1
          else if (icl(f, 0) == 3) for (k = 0; k < dimension; k++) //bord de Dirichlet -> avec val_imp
              gv(f, k) += (i ? 1 : -1) * fs(f) * ref_cast(Dirichlet, cls[icl(f, 1)].valeur()).val_imp(icl(f, 2), k);
        //si Symetrie -> on ne garde que la composante normale a la face
        if (icl(f, 0) == 2) for (k = 0, scal = zone.dot(&nf(f, 0), &gv(f, 0)) / fs(f); k < dimension; k++) gv(f, k) = scal * nf(f, k) / fs(f);
      }

  //pour chaque composante :
  for (k = 0; k < dimension; k++)
    {
      /* inversion de m2 -> pour obtenir (nf.grad) v_k */
      for (f = 0; f < zone.nb_faces_tot(); f++) cgv(f) = gv(f, k);
      zone.m2solv.resoudre_systeme(zone.m2mat, cgv, ngv);
      /* reinterpolation aux elements -> grad v_k */
      for (e = 0; e < zone.nb_elem(); e++) for (i = zone.vedeb(e); i < zone.vedeb(e + 1); i++) for (j = 0; j < dimension; j++)
            vals.addr()[(e * dimension + k) * dimension + j] += zone.veci(i, j) * ngv(zone.veji(i));
    }

  vals.echange_espace_virtuel();
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();
  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_Face_implementation::valeur_aux_elems()" << finl;
      Cerr << "Le DoubleTab val a plus de 2 entrees" << finl;
      Process::exit();
    }

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_aux_elems"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  if (!ma_zone_cl_dis.non_nul()) return val;//on ne peut rien faire tant qu'on ne connait pas les CLs

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  DoubleTrav ve(0, dimension);
  zone.zone().domaine().creer_tableau_elements(ve);
  bool is_vit = cha.le_nom().debute_par("vitesse");
  interp_ve(cha.valeurs(), ve, is_vit);
  for (int p = 0; p < les_polys.size(); p++) for (int r = 0, e = les_polys(p); e < zone.nb_elem() && r < dimension; r++) val(p, r) = ve(e, r);
  return val;
}

DoubleVect& Champ_Face_PolyMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& val, int ncomp) const
{
  init_cl();
  const Champ_base& cha=le_champ();
  assert(val.size() == polys.size());

  if (!ma_zone_cl_dis.non_nul()) return val;//on ne peut rien faire tant qu'on ne connait pas les CLs

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  DoubleTrav ve(0, dimension);
  zone.zone().domaine().creer_tableau_elements(ve);
  interp_ve(cha.valeurs(), ve);

  for (int p = 0; p < polys.size(); p++) val(p) = (polys(p) == -1) ? 0. : ve(polys(p), ncomp);

  return val;
}

DoubleTab& Champ_Face_PolyMAC::remplir_coord_noeuds(DoubleTab& positions) const
{

  throw;
  // return Champ_implementation_RT0::remplir_coord_noeuds(positions);
}

int Champ_Face_PolyMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{

  throw;
  //  return Champ_implementation_RT0::remplir_coord_noeuds_et_polys(positions,polys);
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_PolyMAC::valeur_aux_faces"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  val.resize(zone.nb_faces(), dimension), val = 0;

  for (int f = 0; f < zone.nb_faces(); f++)
    for (int r = 0; r < dimension; r++) val(f, r) = cha.valeurs()(f) * zone.face_normales(f, r) / zone.face_surfaces(f);
  return val;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleVect& Champ_Face_PolyMAC::calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  // avec contrib au bord pour l'instant...
  abort();
  return calcul_S_barre(vitesse, SMA_barre);
}

DoubleVect& Champ_Face_PolyMAC::calcul_S_barre(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, zone_vf());
  DoubleTab duidxj(0, dimension, dimension);
  zone.zone().creer_tableau_elements(duidxj);
  interp_gve(vitesse, duidxj);

  for (int elem = 0; elem < zone.zone().nb_elem(); elem++)
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
  init_cl();
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1;
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const IntTab& face_voisins = zone.face_voisins();
  const DoubleTab& val = valeurs(t);
  DoubleTrav ve(0, dimension);
  if (vectoriel)
    {
      zone.zone().domaine().creer_tableau_elements(ve);
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
