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
// File:        Champ_Face_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Face_CoviMAC.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Champ_Uniforme.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_dis.h>
#include <Zone_Cl_CoviMAC.h>
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

Implemente_instanciable(Champ_Face_CoviMAC,"Champ_Face_CoviMAC",Champ_Inc_base) ;

Sortie& Champ_Face_CoviMAC::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Face_CoviMAC::readOn(Entree& is)
{
  return is;
}

Champ_base& Champ_Face_CoviMAC::le_champ(void)
{
  return *this;
}

const Champ_base& Champ_Face_CoviMAC::le_champ(void) const
{
  return *this;
}

void Champ_Face_CoviMAC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  ref_zone_vf_=ref_cast(Zone_VF, z_dis);
}

int Champ_Face_CoviMAC::fixer_nb_valeurs_nodales(int n)
{

  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base
  // sauf que je recupere le nombre de faces au lieu du nombre d'elements
  //
  // je suis tout de meme etonne du code utilise dans
  // Champ_Fonc_P0_base::fixer_nb_valeurs_nodales()
  // pour recuperer la zone discrete...

  const Champ_Inc_base& self = ref_cast(Champ_Inc_base, *this);
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,self.zone_dis_base());

  assert(n == zone.nb_faces() + nb_compo_ * zone.nb_elem());

  // Probleme: nb_comp vaut dimension mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ /= dimension;

  /* variables : valeurs normales aux faces, puis valeurs aux elements par blocs -> pour que line_size() marche */
  creer_tableau_distribue(zone.mdv_ch_face);
  nb_compo_ = old_nb_compo;
  return n;
}


Champ_base& Champ_Face_CoviMAC::affecter_(const Champ_base& ch)
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,ref_zone_vf_.valeur());
  const DoubleVect& fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales(), &xv = zone.xv();
  DoubleTab& val = valeurs(), eval;
  int f, n, N = val.line_size(), d, D = dimension, unif = sub_type(Champ_Uniforme, ch);

  if (unif) eval = ch.valeurs();
  else eval.resize(val.dimension_tot(0), N * D), ch.valeur_aux(xv,eval);

  for (f = 0; f < zone.nb_faces_tot(); f++) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
        val.addr()[N * f + n] += eval(unif ? 0 : f, N * d + n) * nf(f, d) / fs(f);
  update_ve(val);
  return *this;
}

DoubleVect& Champ_Face_CoviMAC::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  throw;
  // return Champ_implementation_RT0::valeur_a_elem(position,result,poly);
}

double Champ_Face_CoviMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  throw;
  //return Champ_implementation_RT0::valeur_a_elem_compo(position,poly,ncomp);
}

//tableaux de correspondance pour les CLs
void Champ_Face_CoviMAC::init_cl() const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  int i, f, n;

  if (fcl.dimension(0)) return;
  fcl.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Neumann, cls[n].valeur()) + sub_type(Neumann_homogene, cls[n].valeur())+ sub_type(Neumann_val_ext, cls[n].valeur())
                + 2 * sub_type(Symetrie, cls[n].valeur())
                + 3 * sub_type(Dirichlet, cls[n].valeur()) + 4 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx) Cerr << "Champ_Face_CoviMAC : CL non codee rencontree!" << finl, Process::exit();
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), fcl(f, 0) = idx, fcl(f, 1) = n, fcl(f, 2) = i;
    }
  CRIMP(fcl);
}

void Champ_Face_CoviMAC::update_ve(DoubleTab& val) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int e, f, j, d, D = dimension, n, N = val.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  zone.init_ve();
  for (e = 0; e < ne_tot; e++)
    {
      for (d = 0; d < D; d++) for (n = 0; n < N; n++) val.addr()[N * (nf_tot + D * e + d) + n] = 0;
      for (j = zone.ved(e); j < zone.ved(e + 1); j++) for (f = zone.vej(j), d = 0; d < D; d++) for (n = 0; n < N; n++)
            val.addr()[N * (nf_tot + D * e + d) + n] += zone.vec(j, d) * val.addr()[N * f + n] * pf(f) / pe(e);
    }
}

void Champ_Face_CoviMAC::init_ve2() const
{
#ifdef _COMPILE_AVEC_PGCC
  // PL ToDo: rewrite code
  Cerr << "Internal error with nvc++: Internal error: read_memory_region: not all expected entries were read." << finl;
  Process::exit();
#else
  if (ve2d.dimension(0)) return; //deja initialise
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &nf = zone.face_normales();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces(), &e_s = zone.zone().les_elems(), &f_s = zone.face_sommets();
  int i, j, jb, k, e, f, s, d, db, D = dimension, nc, nl = D * (D + 1), nw, infoo=0, un = 1, rank;
  double eps = 1e-8;
  const double *xf;
  zone.init_ve();

  //position des points aux faces de bord : CG si interne ou Dirichlet, projection si Neumann
  DoubleTrav xfb(zone.nb_faces_tot(), D), ve2, ve2i, A, B, P, W(1);
  IntTrav pvt;
  ve2.set_smart_resize(1), A.set_smart_resize(1), B.set_smart_resize(1), P.set_smart_resize(1), W.set_smart_resize(1), pvt.set_smart_resize(1), init_cl();
  for (f = 0; f < zone.nb_faces_tot(); f++) if (fcl(f, 0) == 1 || fcl(f, 0) == 2) //Neumann / Symetrie
      {
        double scal = zone.dot(&xv(f, 0), &nf(f, 0), &xp(e = f_e(f, 0), 0)) / (fs(f) * fs(f));
        for (d = 0; d < D; d++) xfb(f, d) = xp(e, d) + scal * nf(f, d);
      }
    else if (fcl(f, 0)) for (d = 0; d < D; d++) xfb(f, d) = xv(f, d); //Dirichlet

  /* connectivites som-elem et elem-elem */
  std::vector<std::set<int>> s_f(zone.zone().nb_som()), e_s_f(zone.nb_elem());
  for (f = 0; f < zone.nb_faces_tot(); f++) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
      if (s < zone.zone().nb_som()) s_f[s].insert(f);
  for (e = 0; e < zone.nb_elem(); e++) for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
      for (auto &&fa : s_f[s]) e_s_f[e].insert(fa);

  ve2d.resize(1, 2), ve2d.set_smart_resize(1), ve2j.set_smart_resize(1), ve2bj.resize(0, 2), ve2bj.set_smart_resize(1);
  ve2c.set_smart_resize(1), ve2bc.set_smart_resize(1);
  std::map<std::array<int, 2>, int> v_i; // v_i[{f, -1 (interne) ou composante }] = indice
  std::vector<std::array<int, 2>> i_v; // v_i[i_v[f]] = f
  for (e = 0; e < zone.nb_elem(); e++, v_i.clear(), i_v.clear())
    {
      /* stencil : faces de l'element et de ses voisins par som-elem + toutes composantes a ses faces de bord */
      for (auto &&fa : e_s_f[e]) if (!v_i.count({{ fa, -1 }})) v_i[ {{fa, -1}}] = i_v.size(), i_v.push_back({{fa, -1}});
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) if (fcl(f, 0)) for (d = 0; d < D; d++)
            v_i[ {{f, d }}] = i_v.size(), i_v.push_back({{f, d}});

      /* coeffs de l'interpolation d'ordre 1, ponderations (comme dans Zone_CoviMAC::{e,f}grad)  */
      ve2.resize(nc = i_v.size(), D), A.resize(nc, nl), B.resize(nc), P.resize(nc), pvt.resize(nc);
      for (ve2 = 0, i = zone.ved(e); i < zone.ved(e + 1); i++) for (j = v_i.at({{zone.vej(i), -1}}), d = 0; d < D; d++)
      ve2(j, d) += zone.vec(i, d);
      for (i = 0; i < nc; i++) f = i_v[i][0], xf = i_v[i][1] < 0 ? &xv(f, 0) : &xfb(f, 0), P(i) = 1. / sqrt(zone.dot(xf, xf, &xp(e, 0), &xp(e, 0)));

      /* par composante : correction pour etre d'ordre 2 */
      for (d = 0; d < D; d++)
        {
          /* systeme A.x = b */
          for (B = 0, pvt = 0, i = 0; i < nc; i++) for (f = i_v[i][0], db = i_v[i][1], xf = db < 0 ? &xv(f, 0) : &xfb(f, 0), j = 0, jb = 0; j < D; j++) for (k = 0; k <= D; k++, jb++)
                {
                  double fac = (db < 0 ? nf(f, j) / fs(f) : (db == j)) * (k < D ? xf[k] - xp(e, k) : 1);
                  A(i, jb) = fac * P(i);
                  if (k < D) B(jb) -= fac * ve2(i, d); //erreur de l'interp d'ordre 1 a corriger
                }

          /* x de norme L2 minimale par dgels */
          nw = -1,             F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
          W.resize(nw = W(0)), F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
          assert(infoo == 0);
          /* ajout dans ve2 */
          for (i = 0; i < nc; i++) ve2(i, d) += P(i) * B(i);
        }

      /* implicitation des CLs de Neumann / Symetrie */
      Matrice33 M(1, 0, 0, 0, 1, 0, 0, 0, 1), iM;
      for (i = 0; i < nc; i++) if (i_v[i][1] >= 0 && fcl(i_v[i][0], 0) < 2) for (j = 0; j < D; j++)
            M(j, i_v[i][1]) -= ve2(i, j);
      Matrice33::inverse(M, iM);
      for (ve2i.resize(nc, D), i = 0; i < nc; i++) for (j = 0; j < D; j++) for (ve2i(i, j) = 0, k = 0; k < D; k++)
            ve2i(i, j) += iM(j, k) * ve2(i, k);

      /* stockage */
      for (d = 0; d < D; d++, ve2d.append_line(ve2c.size(), ve2bc.size())) for (i = 0; i < nc; i++) if (dabs(ve2i(i, d)) > 1e-6 && (i_v[i][1] < 0 || fcl(i_v[i][0], 0) == 3))
            {
              i_v[i][1] < 0 ? ve2j.append_line(i_v[i][0])  : ve2bj.append_line(i_v[i][0], i_v[i][1]);
              (i_v[i][1] < 0 ? &ve2c : &ve2bc)->append_line(ve2i(i, d) * pf(i_v[i][0]) / pe(e));
            }
    }
  CRIMP(ve2d), CRIMP(ve2j), CRIMP(ve2bj), CRIMP(ve2c), CRIMP(ve2bc);
#endif
}

/* met en coherence les composantes aux elements avec les vitesses aux faces : interpole sur phi * v */
void Champ_Face_CoviMAC::update_ve2(DoubleTab& val) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  int i, j, e, ed, d, D = dimension, n, N = val.line_size(), nf_tot = zone.nb_faces_tot();
  init_ve2();

  for (e = 0, ed = 0, i = N * nf_tot; e < zone.nb_elem(); e++) for (d = 0; d < D; d++, ed++) for (n = 0; n < N; n++, i++)
        {
          /* partie "interne" */
          for (val.addr()[i] = 0, j = ve2d(ed, 0); j < ve2d(ed + 1, 0); j++)
            val.addr()[i] += ve2c(j) * val.addr()[N * ve2j(j) + n];

          /* partie "faces de bord" (Dirichlet seulement) */
          for (j = ve2d(ed, 1); j < ve2d(ed + 1, 1); j++)
            val.addr()[i] += ve2bc(j) * ref_cast(Dirichlet, cls[fcl(ve2bj(j, 0), 1)].valeur()).val_imp(fcl(ve2bj(j, 0), 2), N * ve2bj(j, 1) + n);
        }

  val.echange_espace_virtuel();
}

/* gradient d_j v_i aux elements : interp_ve -> grad -> interp_ve */
void Champ_Face_CoviMAC::interp_gve(const DoubleTab& inco, DoubleTab& vals) const
{
  abort();
}

DoubleTab& Champ_Face_CoviMAC::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = valeurs().line_size();
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

  for (int p = 0, e; p < les_polys.size(); p++) for (e = les_polys(p), d = 0; e < zone.nb_elem() && d < D; d++) for (n = 0; n < N; n++)
        val(p, N * d + n) = valeurs().addr()[N * (nf_tot + D * e + d) + n];
  return val;
}

DoubleVect& Champ_Face_CoviMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& val, int ncomp) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  int nf_tot = zone.nb_faces_tot(), D = dimension, N = valeurs().line_size();
  assert(val.size() == polys.size());

  for (int p = 0, e; p < polys.size(); p++) val(p) = (e = polys(p)) < 0 ? 0. : valeurs().addr()[N * (nf_tot + D * e) + ncomp];

  return val;
}

DoubleTab& Champ_Face_CoviMAC::remplir_coord_noeuds(DoubleTab& positions) const
{

  throw;
  // return Champ_implementation_RT0::remplir_coord_noeuds(positions);
}

int Champ_Face_CoviMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{

  throw;
  //  return Champ_implementation_RT0::remplir_coord_noeuds_et_polys(positions,polys);
}

DoubleTab& Champ_Face_CoviMAC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_CoviMAC::valeur_aux_faces"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,ref_zone_vf_.valeur());
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();

  val.resize(zone.nb_faces(), dimension), val = 0;

  for (int f = 0; f < zone.nb_faces(); f++)
    for (int r = 0; r < dimension; r++)
      val(f, r) = cha.valeurs()(f) * nf(f, r) / fs(f);
  return val;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleVect& Champ_Face_CoviMAC::calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  // avec contrib au bord pour l'instant...
  abort();
  return calcul_S_barre(vitesse, SMA_barre);
}

DoubleVect& Champ_Face_CoviMAC::calcul_S_barre(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  abort();
  return SMA_barre;
}

DoubleTab& Champ_Face_CoviMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant==0);
  init_cl();
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const IntTab& face_voisins = zone.face_voisins();
  const DoubleTab& val = valeurs(t);

  for (int i = 0; i < fr_vf.nb_faces(); i++)
    {
      const int face = fr_vf.num_premiere_face() + i, elem = face_voisins(face, 0);
      for (int d = 0; d < dim; d++) x(i, d) = val(vectoriel ? nf_tot + ne_tot * d + elem : face);
    }

  return x;
}
