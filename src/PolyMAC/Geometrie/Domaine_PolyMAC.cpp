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

#include <Linear_algebra_tools_impl.h>
#include <MD_Vector_composite.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Option_PolyMAC_P0.h>
#include <Domaine_PolyMAC.h>
#include <Option_PolyMAC.h>
#include <Quadrangle_VEF.h>
#include <Option_PolyMAC.h>
#include <Poly_geom_base.h>
#include <communications.h>
#include <TRUSTTab_parts.h>
#include <Segment_poly.h>
#include <Matrix_tools.h>
#include <Hexaedre_VEF.h>
#include <Statistiques.h>
#include <Quadri_poly.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Tetra_poly.h>
#include <Rectangle.h>
#include <Tetraedre.h>
#include <Hexa_poly.h>
#include <TRUSTList.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <EFichier.h>
#include <Tri_poly.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <Lapack.h>
#include <unistd.h>
#include <numeric>
#include <cmath>
#include <cfloat>
#include <tuple>
#include <cfenv>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <osqp/osqp.h>
#pragma GCC diagnostic pop

Implemente_instanciable(Domaine_PolyMAC, "Domaine_PolyMAC", Domaine_Poly_base);

Sortie& Domaine_PolyMAC::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_PolyMAC::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_PolyMAC::discretiser()
{
  Domaine_Poly_base::discretiser();
  calculer_h_carre();
  discretiser_aretes();
}

void Domaine_PolyMAC::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  Elem_geom_base& elem_geom = domaine().type_elem().valeur();
  int is_polyedre = sub_type(Poly_geom_base, elem_geom) ? 1 : 0;
  const ArrOfInt PolyIndex = is_polyedre ? ref_cast(Poly_geom_base, domaine().type_elem().valeur()).getElemIndex() : ArrOfInt(0);
  const DoubleVect& surfaces=face_surfaces();
  const int nbe=nb_elem();
  for (int num_elem=0; num_elem<nbe; num_elem++)
    {
      double surf_max = 0;
      const int nb_faces_elem = is_polyedre ? PolyIndex[num_elem+1] - PolyIndex[num_elem] : domaine().nb_faces_elem();
      for (int i=0; i<nb_faces_elem; i++)
        {
          double surf = surfaces(elem_faces(num_elem,i));
          surf_max = (surf > surf_max)? surf : surf_max;
        }
      double vol = volumes(num_elem)/surf_max;
      vol *= vol;
      h_carre_(num_elem) = vol;
      h_carre = ( vol < h_carre )? vol : h_carre;
    }
}

void Domaine_PolyMAC::calculer_volumes_entrelaces()
{
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(nb_faces(), 2);
  creer_tableau_faces(volumes_entrelaces_dir_);
  const DoubleVect& fs = face_surfaces();

  for (int num_face=0; num_face<nb_faces(); num_face++)
    for (int dir=0; dir<2; dir++)
      {
        int elem = face_voisins_(num_face,dir);
        if (elem!=-1)
          {
            volumes_entrelaces_dir_(num_face, dir) = sqrt(dot(&xp_(elem, 0), &xp_(elem, 0), &xv_(num_face, 0), &xv_(num_face, 0))) * fs[num_face];
            volumes_entrelaces_[num_face] += volumes_entrelaces_dir_(num_face, dir);
          }
      }
  volumes_entrelaces_.echange_espace_virtuel();
  volumes_entrelaces_dir_.echange_espace_virtuel();
}

void Domaine_PolyMAC::init_equiv() const
{
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces(); //, &vf = volumes_entrelaces();
  int i, j, e1, e2, f1, f2, d, D = dimension, ok;

  IntTrav ntot, nequiv;
  creer_tableau_faces(ntot);
  creer_tableau_faces(nequiv);
  equiv_.resize(nb_faces_tot(), 2, e_f.dimension(1));
  equiv_ = -1;

  Cerr << domaine().le_nom() << " : intializing equiv... " ;

  for (int f = 0; f < nb_faces_tot(); f++)
    if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
      for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
        for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
          {
            if (std::fabs(std::fabs(dot(&nf(f1, 0), &nf(f2, 0)) / (fs(f1) * fs(f2))) - 1) > 1e-6)
              continue; //normales colineaires?

            // XXX Elie Saikali
            // Options pour forcer le calcul du tableau equiv
            // car le test ne marche pas si le maillage est hexa, conforme et non-uniforme
            if (Option_PolyMAC::MAILLAGE_VDF || Option_PolyMAC_P0::MAILLAGE_VDF)
              {
                bool aligned = true;
                const int orn_f1 = orientation(f1), orn_f2 = orientation(f2);
                const bool same_orn = (orn_f1 == orn_f2);

                if (same_orn)
                  for (d = 0; d < D; d++)
                    if (d != orn_f1)
                      if ((xv_(f1, d) != xv_(f2, d)) )
                        {
                          aligned = false;
                          break;
                        }

                if (same_orn && aligned && ( f1 != f2 ))
                  {
                    // on cherche si f1 et f2 trouve le meme elem
                    int ee1 = f_e(f1, 0), ee2 = f_e(f1, 1);

                    for (int ii = 0; ii < e_f.dimension(1); ii++)
                      {
                        int ff1 = e_f(ee1, ii), ff2 = e_f(ee2,ii);
                        if (f2 == ff1 || f2 == ff2)
                          {
                            ok = 1;
                            break;
                          }
                        else
                          ok = 0;
                      }
                  }
                else
                  for (ok = 1, d = 0; d < D; d++)
                    ok &= std::fabs((xv_(f1, d) - xp_(e1, d)) - (xv_(f2, d) - xp_(e2, d))) < 1e-12; //xv - xp identiques?
              }
            else
              for (ok = 1, d = 0; d < D; d++)
                ok &= std::fabs((xv_(f1, d) - xp_(e1, d)) - (xv_(f2, d) - xp_(e2, d))) < 1e-12; //xv - xp identiques?

            if (!ok)
              continue;

            equiv_(f, 0, i) = f2;
            equiv_(f, 1, j) = f1;
            nequiv(f)++; //si oui, on a equivalence
          }

  Cerr << mp_somme_vect_as_double(nequiv) * 100. / mp_somme_vect_as_double(ntot) << "% equivalent faces!" << finl;
}

void Domaine_PolyMAC::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  Cerr << "Le Domaine_PolyMAC a ete rempli avec succes" << finl;
  //      calculer_h_carre();

  Journal() << "Domaine_PolyMAC::Modifier_pour_Cl" << finl;
  int nb_cond_lim=conds_lim.size();
  int num_cond_lim=0;
  for (; num_cond_lim<nb_cond_lim; num_cond_lim++)
    {
      //for cl
      const Cond_lim_base& cl = conds_lim[num_cond_lim].valeur();
      if (sub_type(Periodique, cl))
        {
          //if Perio
          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          int nb_faces_elem = domaine().nb_faces_elem();
          const Front_VF& la_front_dis = ref_cast(Front_VF,cl.frontiere_dis());
          int ndeb = 0;
          int nfin = la_front_dis.nb_faces_tot();
#ifndef NDEBUG
          int num_premiere_face = la_front_dis.num_premiere_face();
          int num_derniere_face = num_premiere_face+nfin;
#endif
          int nbr_faces_bord = la_front_dis.nb_faces();
          assert((nb_faces()==0)||(ndeb<nb_faces()));
          assert(nfin>=ndeb);
          int elem1,elem2,k;
          int face;
          // Modification des tableaux face_voisins_ , face_normales_ , volumes_entrelaces_
          // On change l'orientation de certaines normales
          // de sorte que les normales aux faces de periodicite soient orientees
          // de face_voisins(la_face_en_question,0) vers face_voisins(la_face_en_question,1)
          // comme le sont les faces internes d'ailleurs

          DoubleVect C1C2(dimension);
          double vol,psc=0;

          for (int ind_face=ndeb; ind_face<nfin; ind_face++)
            {
              //for ind_face
              face = la_front_dis.num_face(ind_face);
              if  ( (face_voisins_(face,0) == -1) || (face_voisins_(face,1) == -1) )
                {
                  int faassociee = la_front_dis.num_face(la_cl_period.face_associee(ind_face));
                  if (ind_face<nbr_faces_bord)
                    {
                      assert(faassociee>=num_premiere_face);
                      assert(faassociee<num_derniere_face);
                    }

                  elem1 = face_voisins_(face,0);
                  elem2 = face_voisins_(faassociee,0);
                  vol = (volumes_[elem1] + volumes_[elem2])/nb_faces_elem;
                  volumes_entrelaces_[face] = vol;
                  volumes_entrelaces_[faassociee] = vol;
                  face_voisins_(face,1) = elem2;
                  face_voisins_(faassociee,0) = elem1;
                  face_voisins_(faassociee,1) = elem2;
                  psc = 0;
                  for (k=0; k<dimension; k++)
                    {
                      C1C2[k] = xv_(face,k) - xp_(face_voisins_(face,0),k);
                      psc += face_normales_(face,k)*C1C2[k];
                    }

                  if (psc < 0)
                    for (k=0; k<dimension; k++)
                      face_normales_(face,k) *= -1;

                  for (k=0; k<dimension; k++)
                    face_normales_(faassociee,k) = face_normales_(face,k);
                }
            }
        }
    }

  // PQ : 10/10/05 : les faces periodiques etant a double contribution
  //		      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //		      afin de pouvoir beneficier de conds_lim.
  Domaine_VF::marquer_faces_double_contrib(conds_lim);
}

//stabilisation des matrices m1 et m2 de PolyMAC
inline void Domaine_PolyMAC::ajouter_stabilisation(DoubleTab& M, DoubleTab& N) const
{
  int i, j, k, i1, i2, j1, j2, n_f = M.dimension(0), lwork = -1, infoo = 0;
  DoubleTab A, S, b(n_f, 1), D(1, 1), x(1, 1), work(1), U(n_f - dimension, n_f - dimension), V;

  /* spectre de M */
  kersol(M, b, 1e-12, nullptr, x, S);
  double l_max = S(0), l_min = S(dimension - 1); //vp la plus petite sans stabilisation

  /* D : noyau de N (N.D = 0), de taille n_f * (n_f - dimension) */
  b.resize(dimension, 1), kersol(N, b, 1e-12, &D, x, S);
  assert(D.dimension(1) == n_f - dimension); //M doit etre de rang d

  /* matrice U telle que M + D.U.Dt mimimise les termes hors diagonale */
  //une ligne par coeff M(i, j > i), une colonne par terme U(i, j >= i)
  int n_k = D.dimension(1), n_l = n_f * (n_f - 1) / 2, n_c = n_k * (n_k + 1) / 2, un = 1;
  A.resize(n_l, n_c), b.resize(n_l, 1);
  for (i1 = i = 0; i1 < n_f; i1++)
    for (i2 = i1 + 1; i2 < n_f; i2++, i++) //(i1, i2, i) -> numero de ligne
      for (j1 = j = 0, b(i, 0) = -M(i1, i2); j1 < n_k; j1++)
        for (j2 = j1; j2 < n_k; j2++, j++) //(j1, j2, j) -> numero de colonne
          A(i, j) = D(i1, j1) * D(i2, j2) + (j1 != j2) * D(i1, j2) * D(i2, j1);
  char trans = 'T';
  //minimise la somme des carres des termes hors diag de M
  F77NAME(dgels)(&trans, &n_c, &n_l, &un, &A(0, 0), &n_c, &b(0, 0), &n_l, &work(0), &lwork, &infoo); //"workspace query"
  work.resize(lwork = (int)work(0));
  F77NAME(dgels)(&trans, &n_c, &n_l, &un, &A(0, 0), &n_c, &b(0, 0), &n_l, &work(0), &lwork, &infoo); //le vrai appel
  assert(infoo == 0);
  //reconstruction de U
  for (j1 = j = 0; j1 < n_k; j1++)
    for (j2 = j1; j2 < n_k; j2++, j++) U(j1, j2) = U(j2, j1) = b(j, 0);

  /* ajustement de U pour que la vp minimale depasse eps */
  //decomposition de Schur U = Vt.S.V
  char jobz = 'V', uplo = 'U';
  V = U, S.resize(n_k);
  F77NAME(dsyev)(&jobz, &uplo, &n_k, &V(0, 0), &n_k, &S(0), &work(0), &lwork, &infoo);//"workspace query"
  work.resize(lwork = (int)work(0));
  F77NAME(dsyev)(&jobz, &uplo, &n_k, &V(0, 0), &n_k, &S(0), &work(0), &lwork, &infoo);
  assert(infoo == 0);
  //pour garantir des vp plus grandes que eps : S(k) -> std::max(S(k), eps)
  for (i = 0, U = 0; i < n_k; i++)
    for (j = 0; j < n_k; j++)
      for (k = 0; k < n_k; k++) U(i, j) += V(k, i) * std::min(std::max(S(k), l_min), l_max) * V(k, j);

  /* ajout a M */
  for (i1 = 0; i1 < n_f; i1++)
    for (i2 = 0; i2 < n_f; i2++)
      for (j1 = 0; j1 < n_k; j1++)
        for (j2 = 0; j2 < n_k; j2++)
          M(i1, i2) += D(i1, j1) * U(j1, j2) * D(i2, j2);
}

/* recherche d'une matrice W verifiant W.R = N avec sum_{i!=j} w_{ij}^2 minimal et un spectre acceptable */
/* en entree,W contient le stencil admissible  */
int Domaine_PolyMAC::W_stabiliser(DoubleTab& W, DoubleTab& R, DoubleTab& N, int *ctr, double *spectre) const
{
  int i, j, k, l, n_f = R.dimension(0), nv = 0, d = R.dimension(1), infoo = 0, lwork = -1, sym, diag, ret, it, cv;

  /* idx : numero de l'inconnue W(i, j) dans le probleme d'optimisation */
  //si NtR est symetrique, alors on cherche a avoir W symetrique
  Matrice33 NtR(0, 0, 0, 0, d < 2, 0, 0, 0, d < 3), iNtR;
  for (i = 0; i < d; i++)
    for (j = 0; j < d; j++)
      for (k = 0; k < n_f; k++) NtR(i, j) += N(k, i) * R(k, j);
  for (i = 0, sym = 1; i < d; i++)
    for (j = i + 1; j < d; j++) sym &= (std::fabs(NtR(i, j) - NtR(j, i)) < 1e-8);
  //remplissage de idx(i, j) : numero de W_{ij} dans le pb d'optimisation
  IntTrav idx(n_f, n_f);
  if (sym)
    for (i = 0; i < n_f; i++)
      for (j = i; j < n_f; j++) idx(i, j) = idx(j, i) = (W(i, j) ? nv++ : -1);
  else for (i = 0; i < n_f; i++)
      for (j = 0; j < n_f; j++) idx(i, j) = (W(i, j) ? nv++ : -1);

  /* version non stabilisee : W0 = N.(NtR)^-1.Nt */
  Matrice33::inverse(NtR, iNtR); //crash si NtR non inversible
  for (i = 0, W = 0; i < n_f; i++)
    for (j = 0; j < d; j++)
      for (k = 0; k < d; k++)
        for (l = 0; l < n_f; l++) W(i, l) += N(i, j) * iNtR(j, k) * N(l, k);
  //spectre de Ws (partie symetrique de W)
  DoubleTrav Ws(n_f, n_f), S(n_f), work(1);
  for (i = 0; i < n_f; i++)
    for (j = 0; j < n_f; j++) Ws(i, j) = (W(i, j) + W(j, i)) / 2;
  char jobz = 'N', uplo = 'U';
  F77NAME(dsyev)(&jobz, &uplo, &n_f, &Ws(0, 0), &n_f, &S(0), &work(0), &lwork, &infoo);//"workspace query"
  work.resize(lwork = (int)work(0));
  F77NAME(dsyev)(&jobz, &uplo, &n_f, &Ws(0, 0), &n_f, &S(0), &work(0), &lwork, &infoo);//vrai appel
  assert(S(0) > -1e-8 && S(n_f - dimension) > 0);
  if (spectre) spectre[0] = std::min(spectre[0], S(n_f - dimension)), spectre[2] = std::max(spectre[2], S(n_f - 1));
  //bornes sur le spectre de la matrice finale
  double l_min = S(n_f - dimension) / 100, l_max = S(n_f - 1) * 100;

  /* probleme d'optimisation : sur les W_{ij} autorises */
  OSQPData data;
  OSQPSettings settings;
  osqp_set_default_settings(&settings);
  settings.scaled_termination = 1, settings.polish = 1, settings.eps_abs = settings.eps_rel = 1e-8, settings.max_iter = 1e3;

  /* contrainte : W.R = N */
  std::vector<std::map<int, double>> C(nv); //stockage CSC : C[j][i] = M_{ij}
  std::vector<double> lb, ub; //bornes inf/sup
  int il = 0; //suivi de la ligne qu'on est en train de creer
  for (i = 0; i < n_f; i++)
    for (j = 0; j < d; j++, il++)
      for (k = 0, lb.push_back(N(i, j)), ub.push_back(N(i, j)); k < n_f; k++)
        if (idx(i, k) >= 0) C[idx(i, k)][il] += R(k, j);

  /* objectif: minimiser la norme l2 des termes hors diagonale */
  std::vector<int> P_c(nv + 1), P_l(nv), A_c, A_l; //coeffs de la colonne c : indices [P_c(c), P_c(c + 1)[ dans P_l, P_v
  std::vector<double> P_v(nv), A_v, Q(nv, 0.);
  for (i = 0; i < nv; i++) P_l[i] = i, P_c[i + 1] = i + 1;
  for (i = 0; i < n_f; i++)
    for (j = 0; j < n_f; j++)
      if (idx(i, j) >= 0) P_v[idx(i, j)] += (i == j ? 0 : 1);
  data.n = nv, data.P = csc_matrix(nv, nv, (int)P_v.size(), P_v.data(), P_l.data(), P_c.data()), data.q = Q.data();

  //iterations : on resout et on ajoute des contraintes tant que W a un spectre hors de [l_min, l_max]
  std::fenv_t fenv;
  std::feholdexcept(&fenv); //suspend les exceptions FP
  std::vector<double> sol(nv);
  for (cv = 0, it = 0; !cv && it < 100; it++)
    {
      /* assemblage de A : matrice des contraintes */
      for (j = 0, A_c.resize(1), A_l.resize(0), A_v.resize(0); j < nv; j++, A_c.push_back((int)A_l.size()))
        for (auto && l_v : C[j]) A_l.push_back(l_v.first), A_v.push_back(l_v.second);
      data.A = csc_matrix((int)lb.size(), nv, (int)A_v.size(), A_v.data(), A_l.data(), A_c.data());
      data.l = lb.data(), data.u = ub.data(), data.m = (int)lb.size();

      /* resolution  */
      OSQPWorkspace *osqp = nullptr;
      if (osqp_setup(&osqp, &data, &settings)) Cerr << "Domaine_PolyMAC::W_stabiliser : osqp_setup error" << finl, Process::exit();
      if (it) osqp_warm_start_x(osqp, sol.data()); //repart de l'iteration precedente
      osqp_solve(osqp);
      ret = osqp->info->status_val, sol.assign(osqp->solution->x, osqp->solution->x + nv);
      if (ret == OSQP_PRIMAL_INFEASIBLE || ret == OSQP_PRIMAL_INFEASIBLE_INACCURATE)
        {
          osqp_cleanup(osqp), free(data.A);
          break;
        }
      for (i = 0; i < n_f; i++)
        for (j = 0; j < n_f; j++) W(i, j) = (idx(i, j) >= 0 ? sol[idx(i, j)] : 0);

      /* calcul du spectre : Ws recupere les vecteurs propres */
      for (i = 0; i < n_f; i++)
        for (j = 0; j < n_f; j++) Ws(i, j) = (W(i, j) + W(j, i)) / 2;
      jobz = 'V', F77NAME(dsyev)(&jobz, &uplo, &n_f, &Ws(0, 0), &n_f, &S(0), &work(0), &lwork, &infoo);

      /* pour chaque vp sortant de [ l_min, l_max ], on ajoute une contrainte pour la restreindre a [l_min * 1.1, l_max / 1.1 ] */
      for (i = 0, cv = (osqp->info->status_val == OSQP_SOLVED); i < n_f; i++)
        if (S(i) < l_min || S(i) > l_max)
          {
            for (j = 0; j < n_f; j++)
              for (k = 0; k < n_f; k++)
                if(idx(j, k) >= 0) C[idx(j, k)][il]  += Ws(i, j) * Ws(i, k);
            lb.push_back(l_min * (S(i) < l_min ? 1.1 : 1)), ub.push_back(l_max / (S(i) > l_max ? 1.1 : 1));
            cv = 0, il++; //on repart avec une ligne en plus
          }
      osqp_cleanup(osqp), free(data.A);
    }
  free(data.P);
  std::fesetenv(&fenv);     //remet les exceptions FP

  if (!cv)
    {
      Cerr << "Matrice non stabilisee : attention le maillage est trop deforme!!" << finl;
      return 0; //ca passe ou ca casse
    }
  if (spectre)
    for (i = 0; i < n_f; i++) spectre[1] = std::min(spectre[1], S(i)), spectre[3] = std::max(spectre[3], S(i));

  //statistiques
  //ctr[0] : diagonale, ctr[1] : symetrique
  for (i = 0, diag = 1; i < n_f; i++)
    for (j = 0; j < n_f; j++) diag &= (i == j || std::fabs(W(i, j)) < 1e-6);
  ctr[0] += diag, ctr[1] += sym;
  return 1;
}

void Domaine_PolyMAC::init_m2_new() const
{
  const IntTab& e_f = elem_faces();
  const DoubleVect& fs = face_surfaces(), &ve = volumes();
  int i, j, e, n_f, ctr[3] = {0, 0, 0 };
  double spectre[4] = { DBL_MAX, DBL_MAX, 0, 0 }; //vp min (partie consistante, partie stab), vp max (partie consistante, partie stab)

  if (is_init["w2"]) return;
  Cerr << domaine().le_nom() << " : initializing w2/m2 ... ";

  DoubleTab W, M;

  /* pour le partage entre procs */
  DoubleTrav m2e(0, e_f.dimension(1), e_f.dimension(1)), w2e(0, e_f.dimension(1), e_f.dimension(1));
  domaine().creer_tableau_elements(m2e), domaine().creer_tableau_elements(w2e);


  /* calcul sur les elements reels */
  //std::map<int, std::vector<int>> som_face; //som_face[s] : faces de l'element e touchant le sommet s
  IntTrav nnz, nef;//par elements : nombre de coeffs non nuls, nombre de faces (lignes)
  domaine().creer_tableau_elements(nnz), domaine().creer_tableau_elements(nef);
  for (e = 0; e < nb_elem(); e++)
    {
      W2(nullptr, e, W);
      M2(nullptr, e, M);

      for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++;
      for (i = 0; i < n_f; i++)
        for (j = 0, nef(e)++; j < n_f; j++) w2e(e, i, j) = W(i, j, 0), nnz(e) += (std::fabs(W(i, j, 0)) > 1e-6);
      for (i = 0; i < n_f; i++)
        for (j = 0; j < n_f; j++) m2e(e, i, j) = M(i, j, 0);
    }

  /* echange et remplissage */
  m2e.echange_espace_virtuel(), w2e.echange_espace_virtuel();
  for (e = 0, m2d.append_line(0), m2i.append_line(0), w2i.append_line(0); e < nb_elem_tot(); e++, m2d.append_line(m2i.size() - 1))
    {
      for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++;
      for (i = 0; i < n_f; i++, m2i.append_line(m2j.size()))
        for (j = 0, m2j.append_line(i), m2c.append_line(m2e(e, i, i)); j < n_f; j++)
          if (j != i && std::fabs(m2e(e, i, j)) > 1e-6) m2j.append_line(j), m2c.append_line(m2e(e, i, j));
      for (i = 0; i < n_f; i++, w2i.append_line(w2j.size()))
        for (j = 0, w2j.append_line(i), w2c.append_line(w2e(e, i, i)); j < n_f; j++)
          if (j != i && std::fabs(w2e(e, i, j)) > 1e-6) w2j.append_line(j), w2c.append_line(w2e(e, i, j));
    }
  int f, fb;
  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0; i < m2d(e + 1) - m2d(e); i++)
      for (f = e_f(e, i), j = w2i(m2d(e) + i); j < w2i(m2d(e) + i + 1); j++)
        {
          fb = e_f(e, w2j(j));
          w2c(j) /= fs(f) * fs(fb) / ve(e);
        }

  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0; i < m2d(e + 1) - m2d(e); i++)
      for (f = e_f(e, i), j = m2i(m2d(e) + i); j < m2i(m2d(e) + i + 1); j++)
        {
          fb = e_f(e, m2j(j));
          m2c(j) *= fs(f) * fs(fb) / ve(e);
        }

  CRIMP(m2d), CRIMP(m2i), CRIMP(m2j), CRIMP(m2c), CRIMP(w2i), CRIMP(w2j), CRIMP(w2c);
  double n_tot = Process::mp_sum_as_double(nb_elem());
  Cerr << 100. * Process::mp_sum_as_double(ctr[0]) / n_tot << "% diag " << 100. * Process::mp_sum_as_double(ctr[1]) / n_tot << "% sym "
       << 100. * Process::mp_sum_as_double(ctr[2]) / n_tot << "% sparse lambda : " << Process::mp_min(spectre[0]) << " / "
       << Process::mp_min(spectre[1]) << " -> " << Process::mp_max(spectre[2])
       << " / " << Process::mp_max(spectre[3]) << " width : " << mp_somme_vect_as_double(nnz) * 1. / mp_somme_vect_as_double(nef) << finl;
  is_init["w2"] = 1;
}

void Domaine_PolyMAC::init_m2_osqp() const
{
  const IntTab& f_e = face_voisins(), &e_f = elem_faces(), &f_s = face_sommets();
  const DoubleVect& fs = face_surfaces(), &ve = volumes();
  const DoubleTab& nf = face_normales();
  int i, j, e, f, s, n_f, ctr[3] = {0, 0, 0 }, infoo = 0;
  double spectre[4] = { DBL_MAX, DBL_MAX, 0, 0 }; //vp min (partie consistante, partie stab), vp max (partie consistante, partie stab)
  char uplo = 'U';

  if (is_init["w2"]) return;
  Cerr << domaine().le_nom() << " : initializing w2/m2 ... ";

  DoubleTab W, M, R, N;

  /* pour le partage entre procs */
  DoubleTrav m2e(0, e_f.dimension(1), e_f.dimension(1)), w2e(0, e_f.dimension(1), e_f.dimension(1));
  domaine().creer_tableau_elements(m2e), domaine().creer_tableau_elements(w2e);


  /* calcul sur les elements reels */
  std::map<int, std::vector<int>> som_face; //som_face[s] : faces de l'element e touchant le sommet s
  IntTrav nnz, nef;//par elements : nombre de coeffs non nuls, nombre de faces (lignes)
  domaine().creer_tableau_elements(nnz), domaine().creer_tableau_elements(nef);
  for (e = 0; e < nb_elem(); e++)
    {
      for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++;
      W.resize(n_f, n_f), R.resize(n_f, dimension), N.resize(n_f, dimension);

      /* matrices R (lignes elements -> faces) et N (normales sortantes aux faces) */
      for (i = 0; i < n_f; i++)
        for (j = 0, f = e_f(e, i); j < dimension; j++)
          N(i, j) = nf(f, j) / fs(f) * (e == f_e(f, 0) ? 1 : -1), R(i, j) = (xv_(f, j) - xp_(e, j)) * fs(f) / ve(e);

      /* faces connectees a chaque sommet, puis stencil admissible */
      for (i = 0, W = 0, som_face.clear(); i < n_f; i++)
        for (j = 0, f = e_f(e, i); j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) som_face[s].push_back(i);
      for (auto &s_fs : som_face)
        for (auto i1 : s_fs.second)
          for (auto i2 : s_fs.second) W(i1, i2) = 1;

      /* matrice W2 stabilisee */
      W = 1, W_stabiliser(W, R, N, ctr, spectre); /* il faut une matrice complete */

      /* matrice M2 : W2^-1 */
      M = W;
      F77NAME(dpotrf)(&uplo, &n_f, M.addr(), &n_f, &infoo);
      F77NAME(dpotri)(&uplo, &n_f, M.addr(), &n_f, &infoo);
      assert(infoo == 0);
      for (i = 0; i < n_f; i++)
        for (j = i + 1; j < n_f; j++) M(i, j) = M(j, i);

      for (i = 0; i < n_f; i++)
        for (j = 0, nef(e)++; j < n_f; j++) w2e(e, i, j) = W(i, j), nnz(e) += (std::fabs(W(i, j)) > 1e-6);
      for (i = 0; i < n_f; i++)
        for (j = 0; j < n_f; j++) m2e(e, i, j) = M(i, j);
    }

  /* echange et remplissage */
  m2e.echange_espace_virtuel(), w2e.echange_espace_virtuel();
  for (e = 0, m2d.append_line(0), m2i.append_line(0), w2i.append_line(0); e < nb_elem_tot(); e++, m2d.append_line(m2i.size() - 1))
    {
      for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++;
      for (i = 0; i < n_f; i++, m2i.append_line(m2j.size()))
        for (j = 0, m2j.append_line(i), m2c.append_line(m2e(e, i, i)); j < n_f; j++)
          if (j != i && std::fabs(m2e(e, i, j)) > 1e-6) m2j.append_line(j), m2c.append_line(m2e(e, i, j));
      for (i = 0; i < n_f; i++, w2i.append_line(w2j.size()))
        for (j = 0, w2j.append_line(i), w2c.append_line(w2e(e, i, i)); j < n_f; j++)
          if (j != i && std::fabs(w2e(e, i, j)) > 1e-6) w2j.append_line(j), w2c.append_line(w2e(e, i, j));
    }

  CRIMP(m2d), CRIMP(m2i), CRIMP(m2j), CRIMP(m2c), CRIMP(w2i), CRIMP(w2j), CRIMP(w2c);
  double n_tot = Process::mp_sum_as_double(nb_elem());
  Cerr << 100. * Process::mp_sum_as_double(ctr[0]) / n_tot << "% diag " << 100. * Process::mp_sum_as_double(ctr[1]) / n_tot << "% sym "
       << 100. * Process::mp_sum_as_double(ctr[2]) / n_tot << "% sparse lambda : " << Process::mp_min(spectre[0]) << " / "
       << Process::mp_min(spectre[1]) << " -> " << Process::mp_max(spectre[2])
       << " / " << Process::mp_max(spectre[3]) << " width : " << mp_somme_vect_as_double(nnz) * 1. / mp_somme_vect_as_double(nef) << finl;
  is_init["w2"] = 1;
}

//matrice mimetique W_2/M_2 : valeurs tangentes aux lignes element-faces <-> valeurs normales aux faces
void Domaine_PolyMAC::init_m2() const
{
  if (Option_PolyMAC::USE_NEW_M2) init_m2_new();
  else init_m2_osqp();
}


//interpolation normales aux faces -> elements d'ordre 1
void Domaine_PolyMAC::init_ve() const
{
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleVect& ve = volumes_, &fs = face_surfaces();
  int i, k, e, f;

  if (is_init["ve"]) return;
  Cerr << domaine().le_nom() << " : initialisation de ve... ";
  vedeb.resize(1);
  veci.resize(0, 3);
  //formule (1) de Basumatary et al. (2014) https://doi.org/10.1016/j.jcp.2014.04.033 d'apres Perot
  for (e = 0; e < nb_elem_tot(); vedeb.append_line(veji.dimension(0)), e++)
    for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      {
        double x[3] = { 0, };
        for (k = 0; k < dimension; k++) x[k] = fs(f) * (xv(f, k) - xp(e, k)) * (e == f_e(f, 0) ? 1 : -1) / ve(e);
        veji.append_line(f), veci.append_line(x[0], x[1], x[2]);
      }
  CRIMP(vedeb), CRIMP(veji), CRIMP(veci);
  is_init["ve"] = 1, Cerr << "OK" << finl;
}

//rotationnel aux faces d'un champ tangent aux aretes
void Domaine_PolyMAC::init_rf() const
{
  const IntTab& f_s = face_sommets();
  const DoubleTab& xs = domaine().coord_sommets(), &nf = face_normales();
  const DoubleVect& la = longueur_aretes(), &fs = face_surfaces();

  if (is_init["rf"]) return;
  int i, s, f;
  rfdeb.resize(1);
  for (f = 0; f < nb_faces_tot(); rfdeb.append_line(rfji.dimension(0)), f++)
    for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
      {
        int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0),
            a = dimension < 3 ? s : som_arete[s].at(s2);
        std::array<double, 3> taz = {{ 0, 0, 1 }}, vec; //vecteur tangent a l'arete et produit vectoriel de celui-ci avec xa - xv
        vec = cross(dimension, 3, dimension < 3 ? &xs(a, 0) : &xa_(a, 0), dimension < 3 ? &taz[0] : &ta_(a, 0), &xv_(f, 0));
        int sgn = dot(&vec[0], &nf(f, 0)) > 0 ? 1 : -1;
        rfji.append_line(a), rfci.append_line(sgn * (dimension < 3 ? 1 : la(a)) / fs(f));
      }
  CRIMP(rfdeb), CRIMP(rfji), CRIMP(rfci);
  is_init["rf"] = 1;
}

//interpolation aux elements d'un champ dont on connait la composante tangente aux aretes (en 3D ), ou la composante verticale aux sommets en 2D
void Domaine_PolyMAC::init_we() const
{
  if (is_init["we"]) return;

  //remplissage de arete_faces_ (liste des faces touchant chaque arete en 3D, chaque sommet en 2D)
  std::vector<std::vector<int> > a_f_vect(dimension < 3 ? nb_som_tot() : domaine().nb_aretes_tot());
  const IntTab& f_s = face_sommets_;
  for (int f = 0, i, s1; f < nb_faces_tot(); f++)
    for (i = 0; i < f_s.dimension(1) && (s1 = f_s(f, i)) >= 0; i++)
      {
        int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0);
        a_f_vect[dimension < 3 ? s1 : som_arete[s1].at(s2)].push_back(f);
      }
  int a_f_max = 0;
  for (int i = 0; i < (int) a_f_vect.size(); i++) a_f_max = std::max(a_f_max, (int) a_f_vect[i].size());
  arete_faces_.resize((int)a_f_vect.size(), a_f_max), arete_faces_ = -1;
  for (int i = 0, j; i < arete_faces_.dimension(0); i++)
    for (j = 0; j < (int) a_f_vect[i].size(); j++) arete_faces_(i, j) = a_f_vect[i][j];

  dimension < 3 ? init_we_2d() : init_we_3d();
  is_init["we"] = 1;
}

void Domaine_PolyMAC::init_we_2d() const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets_;
  const DoubleTab& xs = domaine().coord_sommets();
  const DoubleVect& ve = volumes();
  int i, j, e, f, s;

  wedeb.resize(1);
  std::map<int, double> wemi;
  for (e = 0; e < nb_elem_tot(); wedeb.append_line(weji.dimension(0)), wemi.clear(), e++)
    {
      //une contribution par "facette" (triangle entre le sommet, le CG de la face et celui de l'element)
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //contrib. de chaque sommet de chaque face
          wemi[s] += std::fabs(cross(2, 2, &xp_(e, 0), &xv_(f, 0), &xs(s, 0), &xs(s, 0))[2]) / (2 * ve(e));
      for (auto &&kv : wemi) weji.append_line(kv.first), weci.append_line(kv.second);
    }
  CRIMP(wedeb), CRIMP(weji), CRIMP(weci);
}

void Domaine_PolyMAC::init_we_3d() const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets_;
  const DoubleVect& la = longueur_aretes_, &ve = volumes_;
  int i, j, k, e, f, s;

  wedeb.resize(1);
  weci.resize(0, 3);
  std::map<int, std::array<double, 3> > wemi;
  for (e = 0; e < nb_elem_tot(); wedeb.append_line(weji.dimension(0)), wemi.clear(), e++)
    {
      //une contribution par "facette" (triangle entre CG de l'arete, le CG de la face et celui de l'element)
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //contrib. de chaque sommet de chaque face
          {
            int s2 = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0),
                a = som_arete[s].at(s2);
            std::array<double, 3> vec = cross(3, 3, &xp_(e, 0), &xv_(f, 0), &xa_(a, 0), &xa_(a, 0));
            //on tourne la facette dans la bonne direction
            int sgn = dot(&ta_(a, 0), &vec[0]) > 0 ? 1 : -1;
            for (k = 0; k < 3; k++) wemi[a][k] += sgn * vec[k] * la(a) / (2 * ve(e));
          }
      for (auto &&kv : wemi) weji.append_line(kv.first), weci.append_line(kv.second[0], kv.second[1], kv.second[2]);
    }
  CRIMP(wedeb), CRIMP(weji), CRIMP(weci);
}

//matrice mimetique d'un champ aux aretes : (valeur tangente aux aretes) -> (flux a travers l'union des facettes touchant l'arete)
//en 2D, m1 est toujours diagonale! Il suffit de calculer la surface de l'arete duale...
void Domaine_PolyMAC::init_m1_2d() const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets_;
  const DoubleTab& xs = domaine().coord_sommets();
  int i, j, e, f, s;

  std::vector<std::map<std::array<int, 2>, double>> m1(domaine().nb_som_tot()); //m1[a][{ ab, e }] : contribution de (arete ab, element e)
  for (e = 0; e < nb_elem_tot(); e++)
    {
      //une contribution par "facette" (triangle entre le sommet, le CG de la face et celui de l'element)
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //contrib. de chaque sommet de chaque face
          m1[s][ {{ s, e }}] += std::fabs(cross(2, 2, &xp_(e, 0), &xv_(f, 0), &xs(s, 0), &xs(s, 0))[2]) / 2;
    }

  //remplissage
  m1deb.resize(1);
  m1ji.resize(0, 2);
  for (i = 0; i < domaine().nb_som_tot(); m1deb.append_line(m1ji.dimension(0)), i++)
    for (auto &&kv : m1[i])
      m1ji.append_line(kv.first[0], kv.first[1]), m1ci.append_line(kv.second);
  CRIMP(m1deb), CRIMP(m1ji), CRIMP(m1ci);
}

//en 3D, c'est moins trivial...
void Domaine_PolyMAC::init_m1_3d() const
{
  const IntTab& f_s = face_sommets_, &e_a = domaine().elem_aretes(), &e_f = elem_faces_;
  const DoubleVect& la = longueur_aretes_, &ve = volumes();
  int a, ab, i, j, k, e, f, s, s2, n_a;

  Cerr << domaine().le_nom() << " : initialisation de m1... ";
  DoubleTab M, N;

  /* tableau parallele */
  DoubleTrav m1e(0, e_a.dimension(1), e_a.dimension(1));
  domaine().creer_tableau_elements(m1e);
  std::map<int, int> idxa;
  for (e = 0; e < nb_elem(); e++)
    {
      /* matrice non stabilisee : contribution par facette (couple face/arete) */
      for (i = 0, idxa.clear(), n_a = 0; i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++) idxa[a] = i, n_a++;
      M.resize(n_a, n_a), N.resize(dimension, n_a);
      for (i = 0, M = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
          {
            s2 = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0), a = som_arete[s].at(s2);
            std::array<double, 3> vec = cross(3, 3, &xp_(e, 0), &xv_(f, 0), &xa_(a, 0), &xa_(a, 0));
            //on tourne la facette dans la bonne direction
            int sgn = dot(&ta_(a, 0), &vec[0]) > 0 ? 1 : -1;
            for (k = wedeb(e); k < wedeb(e + 1); k++) M(idxa[a], idxa[weji(k)]) += sgn * la(a) * dot(&vec[0], &weci(k, 0)) / 2 / ve(e);
          }
      for (i = 0; i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++)
        for (j = 0; j < dimension; j++) N(j, i) = ta_(a, j);

      /* stabilisation et stockage */
      ajouter_stabilisation(M, N);
      for (i = 0; i < n_a; i++)
        for (j = 0; j < n_a; j++) m1e(e, i, j) = M(i, j);
    }

  /* echange et remplissage d'un map global */
  m1e.echange_espace_virtuel();
  std::vector<std::map<std::array<int, 2>, double>> m1(domaine().nb_aretes_tot()); //m1[a][{ ab, e }] : contribution de (arete ab, element e)
  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0; i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++)
      for (j = 0; j < e_a.dimension(1) && (ab = e_a(e, j)) >= 0; j++)
        if (std::fabs(m1e(e, i, j)) > 1e-8) m1[a][ {{ ab, e }}] += ve(e) * m1e(e, i, j);

  /* remplissage final */
  m1deb.resize(1);
  m1ji.resize(0, 2);
  for (a = 0; a < domaine().nb_aretes_tot(); m1deb.append_line(m1ji.dimension(0)), a++)
    for (auto &&kv : m1[a])
      m1ji.append_line(kv.first[0], kv.first[1]), m1ci.append_line(kv.second);
  CRIMP(m1deb), CRIMP(m1ji), CRIMP(m1ci);
  Process::barrier();
  Cerr << "OK" << finl;
}

void Domaine_PolyMAC::init_m1() const
{
  if (is_init["m1"]) return;
  init_we();
  dimension < 3 ? init_m1_2d() : init_m1_3d();
  is_init["m1"] = 1;
}

/* initisalisation de solveurs lineaires pour inverser m1 ou m2 */
void Domaine_PolyMAC::init_m2solv() const
{
  init_m2();
  if (is_init["m2solv"]) return;
  /* stencil et allocation */
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  IntTab stencil(0, 2);
  int e, i, j, k, f, fb;
  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0, j = m2d(e); j < m2d(e + 1); i++, j++)
      for (f = e_f(e, i), k = m2i(j); f < nb_faces() && k < m2i(j + 1); k++)
        if (f <= (fb = e_f(e, m2j(k))))
          stencil.append_line(f, fb);
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_symmetric_morse_matrix(nb_elem_tot() + nb_faces_tot(), stencil, m2mat);

  /* remplissage */
  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0, j = m2d(e); j < m2d(e + 1); i++, j++)
      for (f = e_f(e, i), k = m2i(j); f < nb_faces() && k < m2i(j + 1); k++)
        if (f <= (fb = e_f(e, m2j(k))))
          m2mat(f, fb) += (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * volumes(e) * m2c(k);
  m2mat.set_est_definie(1);

  char lu[] = "Petsc Cholesky { quiet }";
  EChaine ch(lu);
  ch >> m2solv;
  is_init["m2solv"] = 1;
}

void Domaine_PolyMAC::init_virt_ef_map() const
{
  if (is_init["virt_ef"]) return; //deja initialisa
  int e, f;
  IntTrav p_e(0, 2), p_f(0, 2);
  domaine().creer_tableau_elements(p_e), creer_tableau_faces(p_f);
  for (e = 0; e < nb_elem() ; e++) p_e(e, 0) = Process::me(), p_e(e, 1) = e;
  for (f = 0; f < nb_faces(); f++) p_f(f, 0) = Process::me(), p_f(f, 1) = nb_elem_tot() + f;
  p_e.echange_espace_virtuel(), p_f.echange_espace_virtuel();
  for (e = nb_elem() ; e < nb_elem_tot() ; e++) virt_ef_map[ {{ p_e(e, 0), p_e(e, 1) }}] = e;
  for (f = nb_faces(); f < nb_faces_tot(); f++) virt_ef_map[ {{ p_f(f, 0), p_f(f, 1) }}] = nb_elem_tot() + f;
  is_init["virt_ef"] = 1;
}


/* "clamping" a 0 des coeffs petits dans M1/W1/M2/W2 */
inline void clamp(DoubleTab& m)
{
  for (int i = 0; i < m.dimension(0); i++)
    for (int j = 0; j < m.dimension(1); j++)
      for (int n = 0; n < m.dimension(2); n++)
        if (1e6 * std::abs(m(i, j, n)) < std::abs(m(i, i, n)) + std::abs(m(j, j, n))) m(i, j, n) = 0;
}

//matrices locales par elements (operateurs de Hodge) permettant de faire des interpolations :
//normales aux faces -> tangentes aux faces duales : (nu x_ef.v) = m2 (|f|n_ef.v)
void Domaine_PolyMAC::M2(const DoubleTab *nu, int e, DoubleTab& m2) const
{
  int i, j, k, f, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_f, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& xe = xp(), &xf = xv(), &nf = face_normales();
  const DoubleVect& ve = volumes();
  for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++; //nombre de faces de e
  double prefac, fac, beta = n_f == D + 1 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  m2.resize(n_f, n_f, N), m2 = 0;
  DoubleTrav v_e(n_f, D), v_ef(n_f, n_f, D); //interpolations du vecteur complet : non stabilisee en e, stabilisee en (e, f)
  for (i = 0; i < n_f; i++)
    for (f = e_f(e, i), d = 0; d < D; d++) v_e(i, d) = (xf(f, d) - xe(e, d)) / ve(e);
  for (i = 0; i < n_f; i++)
    for (f = e_f(e, i), prefac = D * beta / std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))), j = 0; j < n_f; j++)
      for (fac = prefac * ((j == i) - (e == f_e(f, 0) ? 1 : -1) * dot(&nf(f, 0), &v_e(j, 0))), d = 0; d < D; d++)
        v_ef(i, j, d) = v_e(j, d) + fac * (xf(f, d) - xe(e, d));
  //matrice!
  for (m2 = 0, i = 0; i < n_f; i++)
    for (j = 0; j < n_f; j++)
      if (j < i)
        for (n = 0; n < N; n++) m2(i, j, n) = m2(j, i, n); //sous la diagonale -> avec l'autre cote
      else for (k = 0; k < n_f; k++)
          for (f = e_f(e, k), fac = std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))) / D, n = 0; n < N; n++)
            m2(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ef(k, i, 0), &v_ef(k, j, 0));
  clamp(m2);
}

//tangentes aux faces duales -> normales aux faces : nu|f|n_ef.v = w2.(x_ef.v)
void Domaine_PolyMAC::W2(const DoubleTab *nu, int e, DoubleTab& w2) const
{
  int i, j, k, f, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_f, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& xe = xp(), &xf = xv(), &nf = face_normales();
  const DoubleVect& ve = volumes();
  for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++; //nombre de faces de e
  double prefac, fac, beta = n_f == D + 1 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  w2.resize(n_f, n_f, N), w2 = 0;
  DoubleTrav v_e(n_f, D), v_ef(n_f, n_f, D); //interpolations du vecteur complet : non stabilisee en e, stabilisee en (e, f)
  for (i = 0; i < n_f; i++)
    for (f = e_f(e, i), d = 0; d < D; d++) v_e(i, d) = (e == f_e(f, 0) ? 1 : -1) * nf(f, d) / ve(e);
  for (i = 0; i < n_f; i++)
    for (f = e_f(e, i), prefac = D * beta * (e == f_e(f, 0) ? 1 : -1) / std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))), j = 0; j < n_f; j++)
      for (fac = prefac * ((j == i) - dot(&xf(f, 0), &v_e(j, 0), &xe(e, 0))), d = 0; d < D; d++)
        v_ef(i, j, d) = v_e(j, d) + fac * nf(f, d);
  //matrice!
  for (i = 0; i < n_f; i++)
    for (j = 0; j < n_f; j++)
      if (j < i)
        for (n = 0; n < N; n++) w2(i, j, n) = w2(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_f; k++)
          for (f = e_f(e, k), fac = std::abs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))) / D, n = 0; n < N; n++)
            w2(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ef(k, i, 0), &v_ef(k, j, 0));
  clamp(w2);
}

