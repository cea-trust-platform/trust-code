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
#include <Champ_implementation_P1.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Domaine_Cl_PolyMAC.h>
#include <MD_Vector_tools.h>
#include <Comm_Group_MPI.h>
#include <communications.h>
#include <Poly_geom_base.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Domaine_PolyMAC_P0P1NC.h>
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

Implemente_instanciable(Domaine_PolyMAC_P0P1NC, "Domaine_PolyMAC_P0P1NC", Domaine_PolyMAC);

Sortie& Domaine_PolyMAC_P0P1NC::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_PolyMAC_P0P1NC::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_PolyMAC_P0P1NC::discretiser()
{
  Domaine_Poly_base::discretiser();
  // orthocentrer();
  discretiser_aretes();
}

void Domaine_PolyMAC_P0P1NC::calculer_volumes_entrelaces()
{
  //volumes_entrelaces_ et volumes_entrelaces_dir : par projection de l'amont/aval sur la normale a la face
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(0, 2), creer_tableau_faces(volumes_entrelaces_dir_);
  for (int f = 0, i, e; f < nb_faces(); f++)
    for (i = 0; i < 2 && (e = face_voisins_(f, i)) >= 0; volumes_entrelaces_(f) += volumes_entrelaces_dir_(f, i), i++)
      volumes_entrelaces_dir_(f, i) = std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0)));
  volumes_entrelaces_.echange_espace_virtuel(), volumes_entrelaces_dir_.echange_espace_virtuel();
}

const DoubleTab& Domaine_PolyMAC_P0P1NC::surf_elem_arete() const
{
  if (surf_elem_arete_.nb_dim() == 2) return surf_elem_arete_; //deja fait
  int e, f, a, s, sb, i, j, k, d, D = dimension, sgn;
  const IntTab& ea_d = elem_arete_d(), e_f = elem_faces(), &f_s = face_sommets(), &e_a = D < 3 ? e_f : domaine().elem_aretes();//en 2D, les aretes sont les faces!
  const DoubleTab& xe = xp(), &xf = xv(), &xs = domaine().coord_sommets();
  surf_elem_arete_.resize(ea_d(nb_elem_tot()), D);
  double vecz[3] = { 0, 0, 1 };
  for (e = 0; e < nb_elem_tot(); e++)
    if (D < 3)
      for (i = 0, j = ea_d(e); j < ea_d(e + 1); i++, j++) //2D : arete <-> face, avec orientation du premier au second sommet de f_s
        {
          auto vec = cross(D, 3, &xf(f = e_f(e, i), 0), vecz, &xe(e, 0)); //rotation de (xf - xe)
          for (sgn = dot(&xs(f_s(f, 1), 0), &vec[0], &xs(f_s(f, 0), 0)) > 0 ? 1 : -1, d = 0; d < D; d++) surf_elem_arete_(j, d) = sgn * vec[d]; //avec la bonne orientation
        }
    else for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //3D: une contribution par couple (f, a)
          {
            sb = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0); //autre sommet
            a = som_arete[s].at(sb), k = (int)(std::find(&e_a(e, 0), &e_a(e, 0) + ea_d(e + 1) - ea_d(e), a) - &e_a(e, 0) + ea_d(e)); //arete, son indice dans e_a
            auto vec = cross(D, D, &xf(f, 0), &xa_(a, 0), &xe(e, 0), &xe(e, 0)); //non oriente
            for (sgn = dot(&vec[0], &ta_(a, 0)) >= 0 ? 1 : -1, d = 0; d < D; d++) surf_elem_arete_(k, d) += sgn * vec[d] / 2;
          }
  return surf_elem_arete_;
}
/* "clamping" a 0 des coeffs petits dans M1/W1/M2/W2 */
inline void clamp(DoubleTab& m)
{
  for (int i = 0; i < m.dimension(0); i++)
    for (int j = 0; j < m.dimension(1); j++)
      for (int n = 0; n < m.dimension(2); n++)
        if (1e6 * std::abs(m(i, j, n)) < std::abs(m(i, i, n)) + std::abs(m(j, j, n))) m(i, j, n) = 0;
}

//normales aux aretes duales -> tangentes aux aretes : (nu|a|t_a.v)   = m1 (S_ea.v)
void Domaine_PolyMAC_P0P1NC::M1(const DoubleTab *nu, int e, DoubleTab& m1) const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets(), &e_a = dimension < 3 ? e_f : domaine().elem_aretes(), &a_s = dimension < 3 ? f_s : domaine().aretes_som(), &ea_d = elem_arete_d(); //en 2D, les aretes sont les faces!
  const DoubleTab& xs = domaine().coord_sommets(), &S_ea = surf_elem_arete();
  const DoubleVect& ve = volumes();
  int i, j, k, a, s, sb, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_a = ea_d(e + 1) - ea_d(e), d, D = dimension, idx;
  double prefac, fac, beta = 1 || n_a == 3 * D - 3 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  m1.resize(n_a, n_a, N), m1 = 0;
  DoubleTrav v_e(n_a, D), v_ea(n_a, n_a, D); //interpolations non stabilisees / stabilisees
  //v_e (interpolation non stabilisee)
  for (i = 0; i < n_a; i++)
    for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), d = 0; d < D; d++) v_e(i, d) = (xs(sb, d) - xs(s, d)) / ve(e);
  //v_ea (interpolation stabilisee)
  for (i = 0, idx = ea_d(e); i < n_a; i++, idx++)
    for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), prefac = D * beta / dot(&xs(sb, 0), &S_ea(idx, 0), &xs(s, 0)), j = 0; j < n_a; j++)
      for (fac = prefac * ((j == i) - dot(&S_ea(idx, 0), &v_e(j, 0))), d = 0; d < D; d++)
        v_ea(i, j, d) = v_e(j, d) + fac * (xs(sb, d) - xs(s, d));
  //matrice!
  for (i = 0; i < n_a; i++)
    for (j = 0; j < n_a; j++)
      if (j < i)
        for (n = 0; n < N; n++) m1(i, j, n) = m1(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0, idx = ea_d(e); k < n_a; k++, idx++)
          for (a = e_a(e, k), s = a_s(a, 0), sb = a_s(a, 1), fac = dot(&xs(sb, 0), &S_ea(idx, 0), &xs(s, 0)) / D, n = 0; n < N; n++)
            m1(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ea(k, i, 0), &v_ea(k, j, 0));
  clamp(m1);
}

//tangentes aux aretes -> normales aux aretes duales : (nuS_ea.v) = w1 (|a|t_a.v)
void Domaine_PolyMAC_P0P1NC::W1(const DoubleTab *nu, int e, DoubleTab& w1, DoubleTab& v_e, DoubleTab& v_ea) const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets(), &e_a = dimension < 3 ? e_f : domaine().elem_aretes(), &a_s = dimension < 3 ? f_s : domaine().aretes_som(), &ea_d = elem_arete_d(); //en 2D, les aretes sont les faces!
  const DoubleTab& xs = domaine().coord_sommets(), &S_ea = surf_elem_arete();
  const DoubleVect& ve = volumes();
  int i, j, k, a, s, sb, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_a = ea_d(e + 1) - ea_d(e), d, D = dimension, idx;
  double prefac, fac, beta = n_a == 3 * D - 3 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  w1.resize(n_a, n_a, N), w1 = 0, v_e.resize(n_a, D), v_ea.resize(n_a, n_a, D); //interpolations non stabilisees / stabilisees
  //v_e (interpolation non stabilisee)
  for (i = 0, idx = ea_d(e); i < n_a; i++, idx++)
    for (d = 0; d < D; d++) v_e(i, d) = S_ea(idx, d) / ve(e);
  //v_ea (interpolation stabilisee)
  for (i = 0, idx = ea_d(e); i < n_a; i++, idx++)
    for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), prefac = D * beta / dot(&xs(sb, 0), &S_ea(idx, 0), &xs(s, 0)), j = 0; j < n_a; j++)
      for (fac = prefac * ((j == i) - dot(&xs(sb, 0), &v_e(j, 0), &xs(s, 0))), d = 0; d < D; d++)
        v_ea(i, j, d) = v_e(j, d) + fac * S_ea(idx, d);
  //matrice!
  for (i = 0; i < n_a; i++)
    for (j = 0; j < n_a; j++)
      if (j < i)
        for (n = 0; n < N; n++) w1(i, j, n) = w1(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0, idx = ea_d(e); k < n_a; k++, idx++)
          for (a = e_a(e, k), s = a_s(a, 0), sb = a_s(a, 1), fac = dot(&xs(sb, 0), &S_ea(idx, 0), &xs(s, 0)) / D, n = 0; n < N; n++)
            w1(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ea(k, i, 0), &v_ea(k, j, 0));
  clamp(w1);
}
