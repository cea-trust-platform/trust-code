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

#include <Echange_contact_PolyMAC_P0.h>
#include <Linear_algebra_tools_impl.h>
#include <Frottement_impose_base.h>
#include <Champ_implementation_P1.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Dirichlet_homogene.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Option_PolyMAC_P0.h>
#include <MD_Vector_tools.h>
#include <TRUSTTab_parts.h>
#include <Comm_Group_MPI.h>
#include <Quadrangle_VEF.h>
#include <communications.h>
#include <Statistiques.h>
#include <Hexaedre_VEF.h>
#include <Matrix_tools.h>
#include <unordered_map>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Tetraedre.h>
#include <Rectangle.h>
#include <PE_Groups.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <LireMED.h>
#include <Ecrire_MED.h>
#include <unistd.h>
#include <Lapack.h>
#include <numeric>
#include <vector>
#include <cfloat>
#include <tuple>
#include <cmath>
#include <cfenv>
#include <set>
#include <map>

Implemente_instanciable(Domaine_PolyMAC_P0, "Domaine_PolyMAC_P0", Domaine_PolyMAC_P0P1NC);

Sortie& Domaine_PolyMAC_P0::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_PolyMAC_P0::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_PolyMAC_P0::discretiser()
{
  /* on saut le discretiser() de Domaine_PolyMAC_P0P1NC pour eviter d'initialiser les variables de PolyMAC_P0P1NC_V1 */
  Domaine_Poly_base::discretiser();

  //MD_vector pour Champ_Face_PolyMAC_P0 (faces + dimension * champ_p0)
  MD_Vector_composite mdc_ch_face;
  mdc_ch_face.add_part(md_vector_faces());
  mdc_ch_face.add_part(domaine().md_vector_elements(), dimension);
  mdv_ch_face.copy(mdc_ch_face);
}

//stencil face/face : fsten_f([fsten_d(f, 0), fsten_d(f + 1, 0)[)
void Domaine_PolyMAC_P0::init_stencils() const
{
  if (fsten_d.size()) return;
  const IntTab& f_s = face_sommets(), &f_e = face_voisins(), &e_s = domaine().les_elems();
  int i, e, f, s, ne_tot = nb_elem_tot(), ns_tot = domaine().nb_som_tot();
  fsten_d.resize(1);

  /* connectivite sommets -> elems / faces de bord */
  std::vector<std::set<int>> som_eb(ns_tot);
  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++) som_eb[s].insert(e);
  for (f = 0; f < nb_faces_tot(); f++)
    if (fbord(f) >= 0)
      for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) som_eb[s].insert(ne_tot + f);

  std::set<int> f_eb; //sommets de la face f, elems connectes a e par soms, sommets / faces connectes par une face commune
  for (f = 0; f < nb_faces_tot(); fsten_d.append_line(fsten_eb.size()), f++)
    if (f_e(f, 0) >= 0 && (fbord(f) >= 0 || f_e(f, 1) >= 0))
      {
        /* connectivite par un sommet de f */
        for (f_eb.clear(), i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          for (auto &&el : som_eb[s]) f_eb.insert(el);

        /* remplissage */
        for (auto && el : f_eb) fsten_eb.append_line(el);
      }

  CRIMP(fsten_d), CRIMP(fsten_eb);
}

//pour u.n champ T aux elements, interpole [n_f.grad T]_f (si nu = nullptr) ou [n_f.nu.grad T]_f ou (grad p)_f (si is_p = 1)
//en preservant exactement les champs verifiant [nu grad T]_e = cte.
//Entrees : N             : nombre de composantes
//          is_p          : 1 si on traite le champ de pression (inversion Neumann / Dirichlet)
//          vec           : vecteur complet
//          cls           : conditions aux limites
//          fcl(f, 0/1/2) : donnes sur les CLs (type de CL, indice de CL, indice dans la CL) (cf. Champ_{P0,Face}_PolyVEF_P0)
//          nu(e, n, ..)  : diffusivite aux elements (optionnel)
//          som_ext       : liste de sommets a ne pas traiter (ex. : traitement direct des Echange_Contact dans Op_Diff_PolyVEF_P0_Elem)
//          virt          : 1 si on veut aussi le flux aux faces virtuelles
//          full_stencil  : 1 si on veut le stencil complet (pour dimensionner())
//Sorties : phif_d(f, 0/1)                       : indices dans phif_{e,c} / phif_{pe,pc} du flux a f dans [phif_d(f, 0/1), phif_d(f + 1, 0/1)[
//          phif_e(i), phif_c(i, n, c)           : indices/coefficients de [nf.nu.grad T]_f ou de [grad p]_f
void Domaine_PolyMAC_P0::fgrad(int N, int is_p, int vec, const Conds_lim& cls, const IntTab& fcl, const DoubleTab *nu, const IntTab *som_ext,
                               int virt, int full_stencil, IntTab& phif_d, IntTab& phif_e, DoubleTab& phif_c) const
{
#ifdef _COMPILE_AVEC_PGCC_AVANT_22_7
  Cerr << "Internal error with nvc++: Internal error: read_memory_region: not all expected entries were read." << finl;
  Process::exit();
#else
  const IntTab& f_e = face_voisins(), &e_f = elem_faces(), &f_s = face_sommets();
  const DoubleTab& nf = face_normales(), &xs = domaine().coord_sommets(), &vfd = volumes_entrelaces_dir();
  const DoubleVect& fs = face_surfaces(), &vf = volumes_entrelaces();
  const Static_Int_Lists& s_e = som_elem();
  int i, i_s, j, k, l, e, f, s, sb, n_f, n_m, n_ef, n_e, n_eb, m, n, ne_tot = nb_elem_tot(), sgn, nw, infoo, d, db, D = dimension, rk, nl, nc, un = 1, il, ok, essai;
  unsigned long ll;
  double x, eps_g = 1e-6, eps = 1e-10, i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }}, fac[3], vol_s;
  init_stencils(), phif_e.resize(0), vec ? phif_c.resize(fsten_eb.dimension(0), D, N) : phif_c.resize(fsten_eb.dimension(0), N), phif_c = 0;

  std::vector<int> s_eb, s_f; //listes d'elements/bord, de faces autour du sommet
  std::vector<double> surf_fs, vol_es; //surfaces partielles des faces connectees au sommet (meme ordre que s_f)
  std::vector<std::array<std::array<double, 3>,2>> vec_fs;//pour chaque facette, base de (D-1) vecteurs permettant de la parcourir
  std::vector<std::vector<int>> se_f; /* se_f[i][.] : faces connectees au i-eme element connecte au sommet s */
  DoubleTrav M, B, X, Ff, Feb, Gf, Geb, Mf, Meb, W(1), x_fs, A, S; //systeme M.(grad u) = B dans chaque element, flux/gradient a la face {F,G}f.u_fs + {F,G}eb.u_eb, equations Mf.u_fs = Meb.u_eb
  IntTrav piv, ctr[3];
  for (i = 0; first_fgrad_ && i < 3; i++) domaine().creer_tableau_sommets(ctr[i]);

  /* contributions aux sommets : en evitant ceux de som_ext */
  for (i_s = 0; i_s <= (som_ext ? som_ext->size() : 0); i_s++)
    for (s = (som_ext && i_s ? (*som_ext)(i_s - 1) + 1 : 0); s < (som_ext && i_s < som_ext->size() ? (*som_ext)(i_s) : (virt ? nb_som_tot() : nb_som())); s++)
      {
        /* elements connectes a s : a partir de som_elem (deja classes) */
        for (s_eb.clear(), n_e = 0; n_e < s_e.get_list_size(s); n_e++) s_eb.push_back(s_e(s, n_e));
        /* faces et leurs surfaces partielles */
        for (s_f.clear(), surf_fs.clear(), vec_fs.clear(), se_f.resize(std::max(int(se_f.size()), n_e)), i = 0, ok = 1; i < n_e; i++)
          for (se_f[i].clear(), e = s_eb[i], j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
            {
              for (k = 0, sb = 0; k < f_s.dimension(1) && (sb = f_s(f, k)) >= 0; k++)
                if (sb == s) break;
              if (sb != s) continue; /* face de e non connectee a s -> on saute */
              if (fbord(f) >= 0) s_eb.insert(std::lower_bound(s_eb.begin(), s_eb.end(), ne_tot + f), ne_tot + f); //si f est de bord, on ajoute l'indice correspondant a s_eb
              else ok &= (f_e(f, 0) >= 0 && f_e(f, 1) >= 0); //si f est interne, alors l'amont/aval doivent etre presents
              se_f[i].push_back(f); //faces connectees a e et s
              if ((ll = std::lower_bound(s_f.begin(), s_f.end(), f) - s_f.begin()) == s_f.size() || s_f[ll] != f) /* si f n'est pas dans s_f, on l'ajoute */
                {
                  s_f.insert(s_f.begin() + ll, f); //face -> dans s_f
                  if (D < 3) surf_fs.insert(surf_fs.begin() + ll, fs(f) / 2), vec_fs.insert(vec_fs.begin() + ll, {{{ xs(s, 0) - xv_(f, 0), xs(s, 1) - xv_(f, 1), 0}, { 0, 0, 0 }}}); //2D -> facile
                  else for (surf_fs.insert(surf_fs.begin() + ll, 0), vec_fs.insert(vec_fs.begin() + ll, {{{ 0, 0, 0}, {0, 0, 0 }}}), m = 0; m < 2; m++) //3D -> deux sous-triangles
                  {
                    if (m == 1 || k > 0) sb = f_s(f, m ? (k + 1 < f_s.dimension(1) && f_s(f, k + 1) >= 0 ? k + 1 : 0) : k - 1); //sommet suivant (m = 1) ou precedent avec k > 0 -> facile
                    else for (n = f_s.dimension(1) - 1; (sb = f_s(f, n)) == -1; ) n--; //sommet precedent avec k = 0 -> on cherche a partir de la fin
                    auto v = cross(D, D, &xs(s, 0), &xs(sb, 0), &xv_(f, 0), &xv_(f, 0));//produit vectoriel (xs - xf)x(xsb - xf)
                    surf_fs[ll] += std::fabs(dot(&v[0], &nf(f, 0))) / fs(f) / 4; //surface a ajouter
                    for (d = 0; d < D; d++) vec_fs[ll][m][d] = (xs(s, d) + xs(sb, d)) / 2 - xv_(f, d); //vecteur face -> arete
                  }
                }
            }
        if (!ok) continue; //au moins un voisin manquant
        n_eb = (int)s_eb.size(), n_f = (int)s_f.size();

        /* conversion de se_f en indices dans s_f */
        for (i = 0; i < n_e; i++)
          for (j = 0; j < (int) se_f[i].size(); j++) se_f[i][j] = (int)(std::lower_bound(s_f.begin(), s_f.end(), se_f[i][j]) - s_f.begin());
        for (vol_es.resize(n_e), vol_s = 0, i = 0; i < n_e; vol_s += vol_es[i], i++)
          for (e = s_eb[i], vol_es[i] = 0, j = 0; j < (int) se_f[i].size(); j++)
            f = s_f[k = se_f[i][j]], vol_es[i] += surf_fs[k] * vfd(f, e != f_e(f, 0)) / fs(f) / D;

        for (essai = 0; essai < 3; essai++) /* essai 0 : MPFA O -> essai 1 : MPFA O avec x_fs mobiles -> essai 2 : MPFA symetrique (corecive, mais pas tres consistante) */
          {
            if (essai == 1) /* essai 1 : choix des points x_fs de continuite aux facettes pour obtenir un schema symetrique */
              {
                /* systeme lineaire */
                for (M.resize(N, nc = (D - 1) * n_f, nl = D * (D - 1) / 2 * n_e), B.resize(N, n_m = std::max(nc, nl)), M = 0, B = 0, i = 0, il = 0; i < n_e; i++)
                  for (d = 0; d < D; d++)
                    for (db = 0; db < d; db++, il++)
                      for (e = s_eb[i], j = 0; j < (int) se_f[i].size(); j++)
                        for (sgn = e == f_e(f = s_f[k = se_f[i][j]], 0) ? 1 : -1, n = 0; n < N; n++)
                          {
                            for (l = 0; l < D; l++) fac[l] = sgn * nu_dot(nu, e, n, &nf(f, 0), i3[l]) * surf_fs[k] / fs(f) / vol_es[i]; //vecteur lambda_e nf sortant * facteur commun
                            B(n, il) += fac[d] * (xv_(f, db) - xp_(e, db)) - fac[db] * (xv_(f, d) - xp_(e, d)); //second membre
                            for (l = 0; l < D - 1; l++) M(n, (D - 1) * k + l, il) += fac[db] * vec_fs[k][l][d] - fac[d] * vec_fs[k][l][db]; //matrice
                          }

                /* resolution -> DEGLSY */
                nw = -1, piv.resize(nc), F77NAME(dgelsy)(&nl, &nc, &un, &M(0, 0, 0), &nl, &B(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                for (W.resize(nw = (int)std::lrint(W(0))), n = 0; n < N; n++) piv = 0, F77NAME(dgelsy)(&nl, &nc, &un, &M(n, 0, 0), &nl, &B(n, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                /* x_fs = xf + corrections */
                for (x_fs.resize(N, n_f, D), n = 0; n < N; n++)
                  for (i = 0; i < n_f; i++)
                    for (f = s_f[i], d = 0; d < D; d++)
                      for (x_fs(n, i, d) = xv_(f, d), k = 0; k < D - 1; k++) x_fs(n, i, d) += std::min(std::max(B(n, (D - 1) * i + k), 0.), 0.5) * vec_fs[i][k][d];
              }

            /* gradients par maille en fonctions des (u_eb, u_fs), flux F = Ff.u_fs + Feb.u_eb, et systeme Mf.u_fs = Feb.u_eb */
            Ff.resize(n_f, n_f, N), Feb.resize(n_f, n_eb, N), Mf.resize(N, n_f, n_f), Meb.resize(N, n_eb, n_f);
            if (vec) Gf.resize(n_f, D, n_f, N), Geb.resize(n_f, D, n_eb, N);
            for (Ff = 0, Feb = 0, Gf = 0, Geb = 0, Mf = 0, Meb = 0, i = 0; i < n_e; i++)
              for (e = s_eb[i], M.resize(n_ef = (int)se_f[i].size(), D), B.resize(D, n_m = std::max(D, n_ef)), X.resize(n_ef, D), piv.resize(n_ef), n = 0; n < N; n++)
                {
                  if (essai < 2) /* essais 0 et 1 : gradient consistant donne par (u_e, (u_fs)_{f v e, s})*/
                    {
                      /* gradient dans (e, s) -> matrice / second membre M.x = B du systeme (grad u)_i = sum_f b_{fi} (x_fs_i - x_e), avec x_fs le pt de continuite de u_fs */
                      for (j = 0; j < n_ef; j++)
                        for (f = s_f[k = se_f[i][j]], d = 0; d < D; d++) M(j, d) = (essai ? x_fs(n, k, d) : xv_(f, d)) - xp_(e, d);
                      for (B = 0, d = 0; d < D; d++) B(d, d) = 1;
                      nw = -1, piv = 0, F77NAME(dgelsy)(&D, &n_ef, &D, &M(0, 0), &D, &B(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                      W.resize(nw = (int)std::lrint(W(0))), F77NAME(dgelsy)(&D, &n_ef, &D, &M(0, 0), &D, &B(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                      for (j = 0; j < n_ef; j++)
                        for (d = 0; d < D; d++) X(j, d) = B(d, j); /* pour pouvoir utiliser nu_dot */
                    }
                  else for (j = 0; j < n_ef; j++)
                      for (sgn = e == f_e(f = s_f[k = se_f[i][j]], 0) ? 1 : -1, d = 0; d < D; d++) /* essai 2 : gradient non consistant */
                        X(j, d) = surf_fs[k] / vol_es[i] * sgn * nf(f, d) / fs(f);

                  /* flux et equation. Remarque : les CLs complexes des equations scalaires sont gerees directement dans Op_Diff_PolyVEF_P0_Elem */
                  for (j = 0; j < n_ef; j++)
                    {
                      k = se_f[i][j], f = s_f[k], sgn = e == f_e(f, 0) ? 1 : -1; //face et son indice
                      const Cond_lim_base *cl = fcl(f, 0) ? &cls[fcl(f, 1)].valeur() : nullptr; //si on est sur une CL, pointeur vers celle-ci
                      int is_dir = cl && (is_p || sub_type(Dirichlet, *cl) || sub_type(Dirichlet_homogene, *cl)); //dans le cas de la pression, toutes les CL sont de Dirichlet
                      for (l = 0; l < n_ef; l++)
                        {
                          x = sgn * nu_dot(nu, e, n, &nf(f, 0), &X(l, 0)) * surf_fs[k] / fs(f); //contribution au flux
                          if (sgn > 0) Ff(k, se_f[i][l], n) += x, Feb(k, i, n) -= x; //flux amont->aval
                          if (vec)
                            for (d = 0; d < D; d++) //pression : gradient complet
                              {
                                double y = nu_dot(nu, e, n, i3[d], &X(l, 0)) * surf_fs[k] * vfd(f, e != f_e(f, 0)) / vf(f);
                                Gf(k, d, se_f[i][l], n) += y, Geb(k, d, i, n) -= y;
                              }
                          if (!is_dir) Mf(n, se_f[i][l], k) += x, Meb(n, i, k) += x; //equation sur u_fs (sauf si CL Dirichlet)
                        }
                      if (!cl) continue; //rien de l'autre cote
                      else if (is_dir) Mf(n, k, k) = Meb(n, (int)(std::find(s_eb.begin(), s_eb.end(), ne_tot + f) - s_eb.begin()), k) = 1; //Dirichlet -> equation u_fs = u_b
                      else if (sub_type(Neumann, *cl)) //Neumann -> ajout du flux au bord
                        Meb(n, (int)(std::find(s_eb.begin(), s_eb.end(), ne_tot + f) - s_eb.begin()), k) += surf_fs[k];
                      else if (sub_type(Frottement_impose_base, *cl) && !ref_cast(Frottement_impose_base, *cl).is_externe()) //Frottement_impose_base global -> flux =  - coeff * v_e
                        Meb(n, i, k) -= surf_fs[k] * ((nu) ? ref_cast(Frottement_impose_base, *cl).coefficient_frottement(fcl(f, 2), n) : ref_cast(Frottement_impose_base, *cl).coefficient_frottement_grad(fcl(f, 2), n) ) ;
                      else if (sub_type(Frottement_impose_base, *cl) && ref_cast(Frottement_impose_base, *cl).is_externe()) //Frottement_impose_base externe -> flux =  - coeff * v_f
                        Mf(n, k, k)  += surf_fs[k] * ((nu) ? ref_cast(Frottement_impose_base, *cl).coefficient_frottement(fcl(f, 2), n) : ref_cast(Frottement_impose_base, *cl).coefficient_frottement_grad(fcl(f, 2), n) );
                      else if (sub_type(Echange_impose_base, *cl)) //Echange_impose_base -> flux =  - h * (T_{e,f} - T_ext)
                        {
                          double h = (nu) ? ref_cast(Echange_impose_base, *cl).h_imp(fcl(f, 2), n) : ref_cast(Echange_impose_base, *cl).h_imp_grad(fcl(f, 2), n) ;
                          Meb(n, (int)(std::find(s_eb.begin(), s_eb.end(), ne_tot + f) - s_eb.begin()), k) += surf_fs[k] * h; //partie h * T_ext
                          if (sub_type(Echange_externe_impose, *cl)) Mf(n, k, k) += surf_fs[k] * h; //Echange_externe_impose : partie h * T_f
                          else Meb(n, i, k) -= surf_fs[k] * h; //Echange_global_impose : partie h * T_e
                        }
                    }
                }
            /* resolution de Mf.u_fs = Meb.u_eb : DGELSY, au cas ou */
            nw = -1, piv.resize(n_f), F77NAME(dgelsy)(&n_f, &n_f, &n_eb, &Mf(0, 0, 0), &n_f, &Meb(0, 0, 0), &n_f, &piv(0), &eps, &rk, &W(0), &nw, &infoo);
            for (W.resize(nw = (int)std::lrint(W(0))), n = 0; n < N; n++)
              piv = 0, F77NAME(dgelsy)(&n_f, &n_f, &n_eb, &Mf(n, 0, 0), &n_f, &Meb(n, 0, 0), &n_f, &piv(0), &eps, &rk, &W(0), &nw, &infoo);

            /* substitution dans Feb et Geb */
            for (i = 0; i < n_f; i++)
              for (j = 0; j < n_eb; j++)
                for (n = 0; n < N; n++)
                  for (k = 0; k < n_f; k++)
                    Feb(i, j, n) += Ff(i, k, n) * Meb(n, j, k);
            if (vec)
              for (i = 0; i < n_f; i++)
                for (d = 0; d < D; d++)
                  for (j = 0; j < n_eb; j++)
                    for (n = 0; n < N; n++)
                      for (k = 0; k < n_f; k++)
                        Geb(i, d, j, n) += Gf(i, d, k, n) * Meb(n, j, k);

            /* A : forme bilineaire */
            if (essai == 2) break;//pas la peine pour VFSYM
            for (A.resize(N, n_e, n_e), A = 0, i = 0; i < n_e; i++)
              for (e = s_eb[i], j = 0; j < (int) se_f[i].size(); j++)
                for (sgn = e == f_e(f = s_f[k = se_f[i][j]], 0) ? 1 : -1, l = 0; l < n_e; l++)
                  for (n = 0; n < N; n++)
                    A(n, i, l) -= sgn * Feb(k, l, n);
            /* symmetrisation */
            for (n = 0; n < N; n++)
              for (i = 0; i < n_e; i++)
                for (j = 0; j <= i; j++) A(n, i, j) = A(n, j, i) = (A(n, i, j) + A(n, j, i)) / 2;
            /* v.p. la plus petite : DSYEV */
            nw = -1, F77NAME(DSYEV)("N", "U", &n_e, &A(0, 0, 0), &n_e, S.addr(), &W(0), &nw, &infoo);
            for (W.resize(nw = (int)std::lrint(W(0))), S.resize(n_e), n = 0, ok = 1; n < N; n++)
              F77NAME(DSYEV)("N", "U", &n_e, &A(n, 0, 0), &n_e, &S(0), &W(0), &nw, &infoo), ok &= S(0) > -1e-8 * vol_s;
            if (ok) break; //pour qu' "essai" ait la bonne valeur en sortie
          }
        if (first_fgrad_) ctr[essai](s) = 1;

        /* stockage dans phif_c */
        if (vec)
          for (i = 0; i < n_f; i++)
            for (f = s_f[i], j = 0; j < n_eb; j++)
              for (k = (int)(std::lower_bound(fsten_eb.addr() + fsten_d(f), fsten_eb.addr() + fsten_d(f + 1), s_eb[j]) - fsten_eb.addr()), d = 0; d < D; d++)
                for (n = 0; n < N; n++)
                  phif_c(k, d, n) += Geb(i, d, j, n) / fs(f);
        else for (i = 0; i < n_f; i++)
            for (f = s_f[i], j = 0; j < n_eb; j++)
              for (k = (int)(std::lower_bound(fsten_eb.addr() + fsten_d(f), fsten_eb.addr() + fsten_d(f + 1), s_eb[j]) - fsten_eb.addr()), n = 0; n < N; n++)
                phif_c(k, n) += Feb(i, j, n) / fs(f);
      }


  /* simplification du stencil */
  int skip;
  DoubleTrav scale(N);
  for (phif_d.resize(1), phif_d = 0, phif_e.resize(0), f = 0, i = 0; f < nb_faces_tot(); f++, phif_d.append_line(i))
    if (fbord(f) >= 0 || (f_e(f, 0) >= 0 && f_e(f, 1) >= 0))
      {
        for (n = 0; n < N; n++) scale(n) = nu_dot(nu, f_e(f, 0), n, &nf(f, 0), &nf(f, 0)) / (fs(f) * vf(f)); //ordre de grandeur des coefficients
        for (j = fsten_d(f); j < fsten_d(f + 1); j++)
          {
            if (!vec)
              for (skip = !full_stencil && fsten_eb(j) != f_e(f, 0), n = 0; n < N; n++) skip &= std::fabs(phif_c(j, n)) < 1e-8 * scale(n); //que mettre ici?
            else for (skip = !full_stencil && fsten_eb(j) != f_e(f, 0), d = 0; d < D; d++)
                for (n = 0; n < N; n++) skip &= std::fabs(phif_c(j, d, n)) < 1e-8 * scale(n);
            if (skip) continue;
            if (!vec)
              for (n = 0; n < N; n++) phif_c(i, n) = phif_c(j, n);
            else for (d = 0; d < D; d++)
                for (n = 0; n < N; n++) phif_c(i, d, n) = phif_c(j, d, n);
            phif_e.append_line(fsten_eb(j)), i++;
          }
      }
  /* comptage */
  if (!first_fgrad_) return;
  int count[3] = { mp_somme_vect(ctr[0]), mp_somme_vect(ctr[1]), mp_somme_vect(ctr[2]) }, tot = count[0] + count[1] + count[2];
  if (tot)
    Cerr << domaine().le_nom() << "::fgrad(): " << 100. * count[0] / tot << "% MPFA-O "
         << 100. * count[1] / tot << "% MPFA-O(h) " << 100. * count[2] / tot << "% MPFA-SYM" << finl;
  first_fgrad_ = 0;
#endif
}
