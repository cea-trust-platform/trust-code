/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Zone_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Linear_algebra_tools_impl.h>
#include <Champ_implementation_P1.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Zone_Cl_PolyMAC.h>
#include <MD_Vector_tools.h>
#include <Comm_Group_MPI.h>
#include <communications.h>
#include <Poly_geom_base.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Zone_PolyMAC.h>
#include <unordered_map>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <PE_Groups.h>
#include <Rectangle.h>
#include <Tetraedre.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <Lapack.h>
#include <unistd.h>
#include <numeric>
#include <cfloat>
#include <vector>
#include <tuple>
#include <cmath>
#include <cfenv>
#include <set>
#include <map>

Implemente_instanciable(Zone_PolyMAC,"Zone_PolyMAC",Zone_Poly_base);

//// printOn
//

Sortie& Zone_PolyMAC::printOn(Sortie& os) const
{
  return Zone_Poly_base::printOn(os);
}

//// readOn
//

Entree& Zone_PolyMAC::readOn(Entree& is)
{
  return Zone_Poly_base::readOn(is);
}

void Zone_PolyMAC::discretiser()
{

  Zone_Poly_base::discretiser();

  // orthocentrer();

  //diverses quantites liees aux aretes
  if (dimension > 2)
    {
      zone().creer_aretes();
      md_vector_aretes_ = zone().aretes_som().get_md_vector();

      /* ordre canonique dans aretes_som */
      IntTab& a_s = zone().set_aretes_som(), &e_a = zone().set_elem_aretes();
      const DoubleTab& xs = zone().domaine().coord_sommets();
      std::map<std::array<double, 3>, int> xv_fsa;
      for (int a = 0, i, j, s; a < a_s.dimension_tot(0); a++)
        {
          for (i = 0, j = 0, xv_fsa.clear(); i < a_s.dimension(1) && (s = a_s(a, i)) >= 0; i++) xv_fsa[ {{ xs(s, 0), xs(s, 1), xs(s, 2) }}] = s;
          for (auto &&c_s : xv_fsa) a_s(a, j) = c_s.second, j++;
        }

      //remplissage de som_aretes
      som_arete.resize(zone().nb_som_tot());
      for (int i = 0; i < a_s.dimension_tot(0); i++) for (int j = 0; j < 2; j++) som_arete[a_s(i, j)][a_s(i, !j)] = i;

      //remplissage de xa (CGs des aretes), de ta_ (vecteur tangent aux aretes) et de longueur_arete_ (longueurs des aretes)
      xa_.resize(0, 3), ta_.resize(0, 3);
      creer_tableau_aretes(xa_), creer_tableau_aretes(ta_), creer_tableau_aretes(longueur_aretes_);
      for (int i = 0, k; i < zone().nb_aretes_tot(); i++)
        {
          int s1 = a_s(i, 0), s2 = a_s(i, 1);
          longueur_aretes_(i) = sqrt( dot(&xs(s2, 0), &xs(s2, 0), &xs(s1, 0), &xs(s1, 0)));
          for (k = 0; k < 3; k++) xa_(i, k) = (xs(s1, k) + xs(s2, k)) / 2, ta_(i, k) = (xs(s2, k) - xs(s1, k)) / longueur_aretes_(i);
        }

      /* ordre canonique dans elem_aretes_ */
      for (int e = 0, i, j, a; e < nb_elem_tot(); e++)
        {
          for (i = 0, j = 0, xv_fsa.clear(); i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++) xv_fsa[ {{ xa_(a, 0), xa_(a, 1), xa_(a, 2) }}] = a;
          for (auto &&c_a : xv_fsa) e_a(e, j) = c_a.second, j++;
        }
    }

  //MD_vector pour Champ_Elem_PolyMAC (elems + faces)
  MD_Vector_composite mdc_ef;
  mdc_ef.add_part(zone().md_vector_elements()), mdc_ef.add_part(md_vector_faces());
  mdv_elems_faces.copy(mdc_ef);

  //MD_vector pour Champ_Face_PolyMAC (faces + aretes)
  MD_Vector_composite mdc_fa;
  mdc_fa.add_part(md_vector_faces()), mdc_fa.add_part(dimension < 3 ? zone().domaine().md_vector_sommets() : md_vector_aretes());
  mdv_faces_aretes.copy(mdc_fa);
}

void Zone_PolyMAC::orthocentrer()
{
  const IntTab& f_s = face_sommets(), &e_s = zone().les_elems(), &e_f = elem_faces();
  const DoubleTab& xs = zone().domaine().coord_sommets(), &nf = face_normales_;
  const DoubleVect& fs = face_surfaces();
  int i, j, e, f, s, np;
  DoubleTab M(0, dimension + 1), X(dimension + 1, 1), S(0, 1), vp; //pour les systemes lineaires
  M.set_smart_resize(1), S.set_smart_resize(1), vp.set_smart_resize(1);
  IntTrav b_f_ortho, b_e_ortho; // b_{f,e}_ortho(f/e) = 1 si la face / l'element est orthocentre
  creer_tableau_faces(b_f_ortho), zone().creer_tableau_elements(b_e_ortho);

  /* 1. orthocentrage des faces (en dimension 3) */
  Cerr << zone().domaine().le_nom() << " : ";
  if (dimension < 3) b_f_ortho = 1; //les faces (segments) sont deja orthcentrees!
  else for (f = 0; f < nb_faces_tot(); f++)
      {
        //la face est-elle deja orthocentree?
        double d2min = DBL_MAX, d2max = 0, d2;
        for (i = 0, np = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++, np++)
          d2min = std::min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))), d2max = std::max(d2max, d2);
        if ((b_f_ortho(f) = (d2max / d2min - 1 < 1e-8))) continue;

        //peut-on l'orthocentrer?
        M.resize(np + 1, 4), S.resize(np + 1, 1);
        for (i = 0; i < np; i++) for (j = 0, S(i, 0) = 0, M(i, 3) = 1; j < 3; j++)
            S(i, 0) += 0.5 * std::pow(M(i, j) = xs(f_s(f, i), j) - xv_(f, j), 2);
        for (j = 0, S(np, 0) = M(np, 3) = 0; j < 3; j++) M(np, j) = nf(f, j) / fs(f);
        if (kersol(M, S, 1e-12, NULL, X, vp) > 1e-8) continue; //la face n'a pas d'orthocentre

        //contrainte : ne pas diminuer la distance entre xv et chaque arete de plus de 50%
        double r2min = DBL_MAX;
        for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          {
            int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0),
                a = som_arete[s].at(s2);
            std::array<double, 3> vec = cross(3, 3, &xv_(f, 0), &ta_(a, 0), &xs(s, 0));
            r2min = std::min(r2min, dot(&vec[0], &vec[0]));
          }
        if (dot(&X(0, 0), &X(0, 0)) > 0.25 * r2min) continue; //on s'eloigne trop du CG

        for (i = 0, b_f_ortho(f) = 1; i < dimension; i++) if (std::fabs(X(i, 0)) > 1e-8) xv_(f, i) += X(i, 0);
      }
  Cerr << 100. * mp_somme_vect(b_f_ortho) / Process::mp_sum(nb_faces()) << "% de faces orthocentrees" << finl;

  /* 2. orthocentrage des elements */
  Cerr << zone().domaine().le_nom() << " : ";
  for (e = 0; e < nb_elem_tot(); e++)
    {
      //l'element est-il deja orthocentre?
      double d2min = DBL_MAX, d2max = 0, d2;
      for (i = 0, np = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++, np++)
        d2min = std::min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xp_(e, 0), &xp_(e, 0))), d2max = std::max(d2max, d2);
      if ((b_e_ortho(e) = (d2max / d2min - 1 < 1e-8))) continue;

      //pour qu'on puisse l'orthocentrer, il faut que ses faces le soient
      for (i = 0, j = 1; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) j = j && b_f_ortho(f);
      if (!j) continue;

      //existe-il un orthocentre?
      M.resize(np, dimension + 1), S.resize(np, 1);
      for (i = 0; i < np; i++) for (j = 0, S(i, 0) = 0, M(i, dimension) = 1; j < dimension; j++)
          S(i, 0) += 0.5 * std::pow(M(i, j) = xs(e_s(e, i), j) - xp_(e, j), 2);
      if (kersol(M, S, 1e-12, NULL, X, vp) > 1e-8) continue; //l'element n'a pas d'orthocentre

      //contrainte : ne pas diminuer la distance entre xp et chaque face de plus de 50%
      double rmin = DBL_MAX;
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        rmin = std::min(rmin, std::fabs(dot(&xp_(e, 0), &nf(f, 0), &xv_(f, 0)) / fs(f)));
      if (dot(&X(0, 0), &X(0, 0)) > 0.25 * rmin * rmin) continue; //on s'eloigne trop du CG

      for (i = 0, b_e_ortho(e) = 1; i < dimension; i++) if (std::fabs(X(i, 0)) > 1e-8) xp_(e, i) += X(i, 0);
    }
  Cerr << 100. * mp_somme_vect(b_e_ortho) / Process::mp_sum(nb_elem()) << "% d'elements orthocentres" << finl;
}

/* "clamping" a 0 des coeffs petits dans M1/W1/M2/W2 */
inline void clamp(DoubleTab& m)
{
  for (int i = 0; i < m.dimension(0); i++) for (int j = 0; j < m.dimension(1); j++) for (int n = 0; n < m.dimension(2); n++)
        if (1e6 * std::abs(m(i, j, n)) < std::abs(m(i, i, n)) + std::abs(m(j, j, n))) m(i, j, n) = 0;
}

//matrices locales par elements (operateurs de Hodge) permettant de faire des interpolations :
//normales aux faces -> tangentes aux faces duales : (nu x_ef.v) = m2 (|f|n_ef.v)
void Zone_PolyMAC::M2(const DoubleTab *nu, int e, DoubleTab& m2) const
{
  int i, j, k, f, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_f, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& xe = xp(), &xf = xv(), &nf = face_normales();
  const DoubleVect& ve = volumes();
  for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++; //nombre de faces de e
  double prefac, fac, beta = n_f == D + 1 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  m2.resize(n_f, n_f, N), m2 = 0;
  DoubleTrav v_e(n_f, D), v_ef(n_f, n_f, D); //interpolations du vecteur complet : non stabilisee en e, stabilisee en (e, f)
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), d = 0; d < D; d++) v_e(i, d) = (xf(f, d) - xe(e, d)) / ve(e);
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), prefac = D * beta / std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))), j = 0; j < n_f; j++)
      for (fac = prefac * ((j == i) - (e == f_e(f, 0) ? 1 : -1) * dot(&nf(f, 0), &v_e(j, 0))), d = 0; d < D; d++)
        v_ef(i, j, d) = v_e(j, d) + fac * (xf(f, d) - xe(e, d));
  //matrice!
  for (m2 = 0, i = 0; i < n_f; i++) for (j = 0; j < n_f; j++)
      if (j < i) for (n = 0; n < N; n++) m2(i, j, n) = m2(j, i, n); //sous la diagonale -> avec l'autre cote
      else for (k = 0; k < n_f; k++) for (f = e_f(e, k), fac = std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))) / D, n = 0; n < N; n++)
            m2(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ef(k, i, 0), &v_ef(k, j, 0));
  clamp(m2);
}

//tangentes aux faces duales -> normales aux faces : nu|f|n_ef.v = w2.(x_ef.v)
void Zone_PolyMAC::W2(const DoubleTab *nu, int e, DoubleTab& w2) const
{
  int i, j, k, f, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_f, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& xe = xp(), &xf = xv(), &nf = face_normales();
  const DoubleVect& ve = volumes();
  for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++; //nombre de faces de e
  double prefac, fac, beta = n_f == D + 1 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  w2.resize(n_f, n_f, N), w2 = 0;
  DoubleTrav v_e(n_f, D), v_ef(n_f, n_f, D); //interpolations du vecteur complet : non stabilisee en e, stabilisee en (e, f)
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), d = 0; d < D; d++) v_e(i, d) = (e == f_e(f, 0) ? 1 : -1) * nf(f, d) / ve(e);
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), prefac = D * beta * (e == f_e(f, 0) ? 1 : -1) / std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))), j = 0; j < n_f; j++)
      for (fac = prefac * ((j == i) - dot(&xf(f, 0), &v_e(j, 0), &xe(e, 0))), d = 0; d < D; d++)
        v_ef(i, j, d) = v_e(j, d) + fac * nf(f, d);
  //matrice!
  for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++)
      if (j < i) for (n = 0; n < N; n++) w2(i, j, n) = w2(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_f; k++) for (f = e_f(e, k), fac = std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))) / D, n = 0; n < N; n++)
            w2(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ef(k, i, 0), &v_ef(k, j, 0));
  clamp(w2);
}

//normales aux aretes duales -> tangentes aux aretes : (nu|a|t_a.v)   = m1 (S_ea.v)
void Zone_PolyMAC::M1(const DoubleTab *nu, int e, DoubleTab& m1) const
{
  int i, j, k, f, a, s, sb, sgn, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_a, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_s = face_sommets(), &e_a = D < 3 ? e_f : zone().elem_aretes(), &a_s = D < 3 ? f_s : zone().aretes_som(); //en 2D, les aretes sont les faces!
  const DoubleTab& xe = xp(), &xf = xv(), &xs = zone().domaine().coord_sommets();
  const DoubleVect& ve = volumes();
  for (n_a = 0; n_a < e_a.dimension(1) && e_a(e, n_a) >= 0;) n_a++; //nombre d'aretes autour de e
  double prefac, fac, vecz[3] = { 0, 0, 1 }, beta = n_a == 3 * D - 3 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  m1.resize(n_a, n_a, N), m1 = 0;
  DoubleTrav S_ea(n_a, D), v_e(n_a, D), v_ea(n_a, n_a, D); //vecteur "surface duale x normale" (oriente comme a), interpolations non stabilisees / stabilisees
  //construction de S_ea
  if (D < 3) for (i = 0; i < n_a; i++) //2D : arete <-> face, avec orientation du premier au second sommet de f_s
      {
        auto vec = cross(D, 3, &xf(f = e_f(e, i), 0), vecz, &xe(e, 0)); //rotation de (xf - xe)
        for (sgn = dot(&xs(f_s(f, 1), 0), &vec[0], &xs(f_s(f, 0), 0)) > 0 ? 1 : -1, d = 0; d < D; d++) S_ea(i, d) = sgn * vec[d]; //avec la bonne orientation
      }
  else for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //3D: une contribution par couple (f, a)
        {
          sb = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0); //autre sommet
          a = som_arete[s].at(sb), k = std::find(&e_a(e, 0), &e_a(e, 0) + n_a, a) - &e_a(e, 0); //arete, son indice dans e_a
          auto vec = cross(D, D, &xf(f, 0), &xa_(a, 0), &xe(e, 0), &xe(e, 0)); //non oriente
          for (sgn = dot(&vec[0], &ta_(a, 0)) >= 0 ? 1 : -1, d = 0; d < D; d++) S_ea(k, d) += sgn * vec[d] / 2;
        }
  //v_e (interpolation non stabilisee)
  for (i = 0; i < n_a; i++) for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), d = 0; d < D; d++) v_e(i, d) = (xs(sb, d) - xs(s, d)) / ve(e);
  //v_ea (interpolation stabilisee)
  for (i = 0; i < n_a; i++) for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), prefac = D * beta / dot(&xs(sb, 0), &S_ea(i, 0), &xs(s, 0)), j = 0; j < n_a; j++)
      for (fac = prefac * ((j == i) - dot(&S_ea(i, 0), &v_e(j, 0))), d = 0; d < D; d++)
        v_ea(i, j, d) = v_e(j, d) + fac * (xs(sb, d) - xs(s, d));
  //matrice!
  for (i = 0; i < n_a; i++) for (j = 0; j < n_a; j++)
      if (j < i) for (n = 0; n < N; n++) m1(i, j, n) = m1(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_a; k++) for (a = e_a(e, k), s = a_s(a, 0), sb = a_s(a, 1), fac = dot(&xs(sb, 0), &S_ea(k, 0), &xs(s, 0)) / D, n = 0; n < N; n++)
            m1(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ea(k, i, 0), &v_ea(k, j, 0));
  clamp(m1);
}

//tangentes aux aretes -> normales aux aretes duales : (nuS_ea.v) = w1 (|a|t_a.v)
void Zone_PolyMAC::W1(const DoubleTab *nu, int e, DoubleTab& w1) const
{
  int i, j, k, f, a, s, sb, sgn, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_a, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_s = face_sommets(), &e_a = D < 3 ? e_f : zone().elem_aretes(), &a_s = D < 3 ? f_s : zone().aretes_som(); //en 2D, les aretes sont les faces!
  const DoubleTab& xe = xp(), &xf = xv(), &xs = zone().domaine().coord_sommets();
  const DoubleVect& ve = volumes();
  for (n_a = 0; n_a < e_a.dimension(1) && e_a(e, n_a) >= 0;) n_a++; //nombre d'aretes autour de e
  double prefac, fac, vecz[3] = { 0, 0, 1 }, beta = n_a == 3 * D - 3 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  w1.resize(n_a, n_a, N), w1 = 0;
  DoubleTrav S_ea(n_a, D), v_e(n_a, D), v_ea(n_a, n_a, D); //vecteur "surface duale x normale" (oriente comme a), interpolations non stabilisees / stabilisees
  //construction de S_ea
  if (D < 3) for (i = 0; i < n_a; i++) //2D : arete <-> face, avec orientation du premier au second sommet de f_s
      {
        auto vec = cross(D, 3, &xf(f = e_f(e, i), 0), vecz, &xe(e, 0)); //rotation de (xf - xe)
        for (sgn = dot(&xs(f_s(f, 1), 0), &vec[0], &xs(f_s(f, 0), 0)) > 0 ? 1 : -1, d = 0; d < D; d++) S_ea(i, d) = sgn * vec[d]; //avec la bonne orientation
      }
  else for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //3D: une contribution par couple (f, a)
        {
          sb = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0); //autre sommet
          a = som_arete[s].at(sb), k = std::find(&e_a(e, 0), &e_a(e, 0) + n_a, a) - &e_a(e, 0); //arete, son indice dans e_a
          auto vec = cross(D, D, &xf(f, 0), &xa_(a, 0), &xe(e, 0), &xe(e, 0)); //non oriente
          for (sgn = dot(&vec[0], &ta_(a, 0)) >= 0 ? 1 : -1, d = 0; d < D; d++) S_ea(k, d) += sgn * vec[d] / 2;
        }
  //v_e (interpolation non stabilisee)
  for (i = 0; i < n_a; i++) for (d = 0; d < D; d++) v_e(i, d) = S_ea(i, d) / ve(e);
  //v_ea (interpolation stabilisee)
  for (i = 0; i < n_a; i++) for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), prefac = D * beta / dot(&xs(sb, 0), &S_ea(i, 0), &xs(s, 0)), j = 0; j < n_a; j++)
      for (fac = prefac * ((j == i) - dot(&xs(sb, 0), &v_e(j, 0), &xs(s, 0))), d = 0; d < D; d++)
        v_ea(i, j, d) = v_e(j, d) + fac * S_ea(i, d);
  //matrice!
  for (i = 0; i < n_a; i++) for (j = 0; j < n_a; j++)
      if (j < i) for (n = 0; n < N; n++) w1(i, j, n) = w1(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_a; k++) for (a = e_a(e, k), s = a_s(a, 0), sb = a_s(a, 1), fac = dot(&xs(sb, 0), &S_ea(k, 0), &xs(s, 0)) / D, n = 0; n < N; n++)
            w1(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ea(k, i, 0), &v_ea(k, j, 0));
  clamp(w1);
}
