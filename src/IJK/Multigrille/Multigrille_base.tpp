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

#ifndef Multigrille_base_TPP_H
#define Multigrille_base_TPP_H

#include <IJK_discretization.h>
#include <Statistiques.h>
#include <stat_counters.h>
#include <Matrice_Morse_Sym.h>
#include <IJK_Vector.h>
#include <Interprete_bloc.h>

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::resoudre_systeme_IJK(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rhs, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x)
{
  if (solver_precision_ == precision_float_)
    resoudre_systeme_IJK_template<float, _TYPE_, _TYPE_ARRAY_>(rhs, x);
  else
    resoudre_systeme_IJK_template<double, _TYPE_, _TYPE_ARRAY_>(rhs, x);

}

template <typename _TYPE_FUNC_, typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::resoudre_systeme_IJK_template(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rhs, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x)
{
  IJK_Field_template<_TYPE_FUNC_,TRUSTArray<_TYPE_FUNC_>>& ijk_b =  get_storage_template<_TYPE_FUNC_>(STORAGE_RHS, 0);
  IJK_Field_template<_TYPE_FUNC_,TRUSTArray<_TYPE_FUNC_>>& ijk_x =  get_storage_template<_TYPE_FUNC_>(STORAGE_X, 0);
  ijk_x.shift_k_origin(needed_kshift_for_jacobi(0) - ijk_x.k_shift());
  int i, j, k;
  const int imax = x.ni();
  const int jmax = x.nj();
  const int kmax = x.nk();
  for (k = 0; k < kmax; k++)
    for (j = 0; j < jmax; j++)
      for (i = 0; i < imax; i++)
        {
          ijk_b(i,j,k) = (_TYPE_FUNC_)rhs(i,j,k);
        }

  solve_ijk_in_storage_template<_TYPE_FUNC_>();

  for (k = 0; k < kmax; k++)
    for (j = 0; j < jmax; j++)
      for (i = 0; i < imax; i++)
        {
          x(i,j,k) = (_TYPE_)ijk_x(i,j,k);
        }
}


// Can be further optimized for float (extend Schema_Comm_Vecteurs to float)
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::convert_from_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x, DoubleVect& x)
{
  // fetch the vdf_to_ijk translator (assume there is one unique object, with conventional name)
  const Nom& ijkdis_name = IJK_discretization::get_conventional_name();
  const IJK_discretization& ijkdis = ref_cast(IJK_discretization, Interprete_bloc::objet_global(ijkdis_name));
  ijkdis.get_vdf_to_ijk(IJK_Splitting::ELEM).convert_from_ijk(ijk_x, x);
}

// Can be further optimized for float (extend Schema_Comm_Vecteurs to float)
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::convert_to_ijk(const DoubleVect& x, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x)
{
  // fetch the vdf_to_ijk translator (assume there is one unique object, with conventional name)
  const Nom& ijkdis_name = IJK_discretization::get_conventional_name();
  const IJK_discretization& ijkdis = ref_cast(IJK_discretization, Interprete_bloc::objet_global(ijkdis_name));
  ijkdis.get_vdf_to_ijk(IJK_Splitting::ELEM).convert_to_ijk(x, ijk_x);
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
double Multigrille_base::multigrille(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& b, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu)
{
  return multigrille_(x, b, residu, 0, 0);
}

// Methode recursive qui resout A*x = b et calcule residu = A*x-b
//  pour le niveau de grille grid_level.
//  Si grid_level = niveau grossier, appel a coarse_solver,
//  sinon iterations sur
//    pre_smoothing, coarsening, appel recursif, interpolation, post_smoothing
// input: b
// output: x, residu
// return value = L2 norm of the residue
template <typename _TYPE_, typename _TYPE_ARRAY_>
double Multigrille_base::multigrille_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& b, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu,
                                      int grid_level, int iter_number)
{

  static Stat_Counter_Id multigrille_counter_ = statistiques().new_counter(2, "projection: multigrille");

  double norme_residu_final = 0.;
  const int needed_kshift = needed_kshift_for_jacobi(grid_level + 1);
#ifdef DUMP_LATA_ALL_LEVELS
  IJK_Field_template<_TYPE_,_TYPE_ARRAY_> copy_residu_for_post(residu);
  copy_residu_for_post.data() = 0.;
  IJK_Field_template<_TYPE_,_TYPE_ARRAY_> copy_x_for_post(x);
  IJK_Field_template<_TYPE_,_TYPE_ARRAY_> copy_xini_for_post(x);
  copy_xini_for_post.data() = x.data();
#endif
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
  dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("avt pre-smooth / recursive call / coarse solver"));
#endif
  // Recurse
  if (grid_level < nb_grid_levels() - 1)
    {
      // Pre-smooting
      {
        statistiques().begin_count(multigrille_counter_);
        const int pss = (grid_level >= pre_smooth_steps_.size_array())
                        ? pre_smooth_steps_[pre_smooth_steps_.size_array()-1]
                        : pre_smooth_steps_[grid_level];
        jacobi_residu(x, &b, grid_level, pss /* jacobi iterations */, &residu);
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
        dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("apres pre-smooth"));
#endif
      }
#ifdef DUMP_LATA_ALL_LEVELS
      copy_x_for_post.data() = x.data();
#endif

      norme_residu_final = norme_ijk(residu);

      if (impr_)
        Cout << "level=" << grid_level << " residu(pre)=" << norme_residu_final << finl;

      IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse_b = get_storage_template<_TYPE_>(STORAGE_RHS, grid_level + 1);
      IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse_x = get_storage_template<_TYPE_>(STORAGE_X, grid_level + 1);
      IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse_residu = get_storage_template<_TYPE_>(STORAGE_RESIDUE, grid_level + 1);

      const int nmax = nb_full_mg_steps_.size_array() - 1;
      int nb_full = 1;
      if (iter_number != 0 || grid_level <= 1)
        {
          if (grid_level > nmax)
            nb_full = nb_full_mg_steps_[nmax];
          else
            nb_full = nb_full_mg_steps_[grid_level];
        }

      for (int fmg_step = 0; fmg_step < nb_full; fmg_step++)
        {
#ifdef DUMP_LATA_ALL_LEVELS
          copy_residu_for_post = residu;
#endif
          // Coarsen residual
          coarse_b.data() = 0.;
          coarsen(residu, coarse_b, grid_level);

          // It seems that the residue has non zero sum due to accumulation of
          // roundoff errors when computing A*x. At the coarse level, we get
          // a wrong rhs...
          prepare_secmem(coarse_b);

          // We need one less layer on b than on x to compute jacobi or residue
          if (IJK_Shear_Periodic_helpler::defilement_==0)
            {
              //pas sur de devoir echanger espace virtuel pour le second membre dans le cas du shear_perio...
              coarse_b.echange_espace_virtuel(b.ghost()-1);
            }
          // Solve for coarse_x
          coarse_x.shift_k_origin(needed_kshift - coarse_x.k_shift());
          coarse_x.data() = 0.;
          //coarse_x.shift_k_origin(coarse_x.k_shift_max() - coarse_x.k_shift());
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
          // inutile : idem avt avt-presmooth pour grid-level+1
          //  dump_x_b_residue_in_file(coarse_x,coarse_b,coarse_residu, grid_level+1, global_count_dump_in_file, Nom("avt recursive-call"));
#endif
          statistiques().end_count(multigrille_counter_);

          multigrille_(coarse_x, coarse_b, coarse_residu, grid_level+1, fmg_step);
          statistiques().begin_count(multigrille_counter_);

#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
          dump_x_b_residue_in_file(coarse_x,coarse_b,coarse_residu, grid_level+1, global_count_dump_in_file, Nom("avt interpol / apres recursive-call/jacobi-residu/ou/coarse-solver "));
#endif

          // Interpolate and substract to x
          // Shift by n layers in k for the next jacobi_residu call
          interpolate_sub_shiftk(coarse_x, x, grid_level);
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
          IJK_Field_template<_TYPE_,_TYPE_ARRAY_> copy_x_to_compute_res(x);
          copy_x_to_compute_res.data() = x.data();
          const int g = x.ghost();
          const int imin = x.linear_index(-g,-g,-g);
          const int imax = x.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1;
          assert(imin == copy_x_to_compute_res.linear_index(-g,-g,-g));
          assert(imax == copy_x_to_compute_res.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1);
          _TYPE_ *ptr = x.data().addr();
          const _TYPE_ *ptr2 = copy_x_to_compute_res.data().addr();
          for (int i = imin; i < imax; i++)
            ptr[i] -= ptr2[i];

          jacobi_residu<_TYPE_, _TYPE_ARRAY_>(copy_x_to_compute_res, &b, grid_level, 0 /* no jacobi iterations, just compute residu */, &residu);
          dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("apres interpol"));
#endif
          // Post smoothing iterations
          {
            const int postss = (grid_level >= smooth_steps_.size_array())
                               ? smooth_steps_[smooth_steps_.size_array()-1]
                               : smooth_steps_[grid_level];
            jacobi_residu(x, &b, grid_level, postss /* jacobi iterations */, &residu);
          }
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
          dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("apres post-smooth"));
#endif
#ifdef DUMP_LATA_ALL_LEVELS
          {
            static ArrOfInt step;
            if (step.size_array() < grid_level + 1)
              step.resize_array(grid_level + 1);

            const Nom lata_name = Nom("Multigrid_level") + Nom(grid_level) + Nom(".lata");
            if (step[grid_level] == 0)
              {
                Nom dom = Nom("DOM")+Nom(grid_level);
                IJK_Grid_Geometry& geom = ref_cast_non_const(IJK_Grid_Geometry,x.get_splitting().get_grid_geometry() );
                geom.nommer(dom); // On nomme la geom pour pouvoir l'ecrire.
                dumplata_header(lata_name, x /* on passe un champ pour ecrire la geometrie */);
              }
            dumplata_newtime(lata_name,step[grid_level]);
            dumplata_scalar(lata_name, "xini", copy_x_for_post, step[grid_level]);
            dumplata_scalar(lata_name, "x1", copy_x_for_post, step[grid_level]);
            dumplata_scalar(lata_name, "xf", x, step[grid_level]);
            dumplata_scalar(lata_name, "b", b, step[grid_level]);
            dumplata_scalar(lata_name, "residu1", copy_residu_for_post, step[grid_level]);
            dumplata_scalar(lata_name, "residue", residu, step[grid_level]);
            step[grid_level]++;
          }
#endif

          norme_residu_final = norme_ijk(residu);
          if (impr_)
            Cout << "level=" << grid_level << " residu=" << norme_residu_final << finl;
          // use threshold criteria only if pure multigrid (not gcp with mg preconditionning)
          if (grid_level == 0 && norme_residu_final < seuil_ && max_iter_gcp_ == 0)
            break;
        }
      statistiques().end_count(multigrille_counter_);
    }
  else
    {
      statistiques().begin_count(multigrille_counter_);
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
      // inutile : idem avt avt-presmooth pour grid-level
      // dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("avt coarse-solver"));
#endif
      coarse_solver(x, b);
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
      // inutile : idem avt interpol
      // dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("apres coarse-solver"));
#endif
      jacobi_residu(x, &b, grid_level, 0 /* not jacobi */, &residu);
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
      // inutile : idem avt interpol
      // dump_x_b_residue_in_file(x,b,residu, grid_level, global_count_dump_in_file, Nom("apres jacobi-residu"));
#endif


      norme_residu_final = norme_ijk(residu);

      if (impr_)
        Cout << finl << "level=" << grid_level << " residu=" << norme_residu_final
             << " (coarse solver)" << finl;
#ifdef DUMP_LATA_ALL_LEVELS
      {
        static ArrOfInt step;
        if (step.size_array() < grid_level + 1)
          step.resize_array(grid_level + 1);

        const Nom lata_name = Nom("Multigrid_level") + Nom(grid_level) + Nom(".lata");
        if (step[grid_level] == 0)
          {
            Nom dom = Nom("DOM")+Nom(grid_level);
            x.get_splitting_non_const().get_grid_geometry_non_const().nommer(dom); // On nomme la geom pour pouvoir l'ecrire.
            dumplata_header(lata_name, x /* on passe un champ pour ecrire la geometrie */);
          }
        dumplata_newtime(lata_name,step[grid_level]);
        dumplata_scalar(lata_name, "x", x, step[grid_level]);
        dumplata_scalar(lata_name, "b", b, step[grid_level]);
        dumplata_scalar(lata_name, "residue", residu, step[grid_level]);
        step[grid_level]++;
      }
#endif
      statistiques().end_count(multigrille_counter_);

    }
  return norme_residu_final;
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::coarse_solver(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& b)
{
  const Matrice_Grossiere& mat = coarse_matrix_;

  DoubleVect inco;
  DoubleVect secmem;
  const MD_Vector& md = mat.md_vector();
  inco.resize(md->get_nb_items_tot());
  inco.set_md_vector(md);
  secmem = inco;

  int ni = x.ni();
  int nj = x.nj();
  int nk = x.nk();
  int i, j, k;

  for (k = 0; k < nk; k++)
    for (j = 0; j < nj; j++)
      for (i = 0; i < ni; i++)
        secmem[mat.renum(i,j,k)] = b(i,j,k);

  //pas sur de devoir echanger espace virtuel pour le second membre dans le cas du shear_perio...
  if (IJK_Shear_Periodic_helpler::defilement_==0)
    {
      secmem.echange_espace_virtuel();
    }
  solveur_grossier_.resoudre_systeme(mat.matrice(), secmem, inco);

  for (k = 0; k < nk; k++)
    for (j = 0; j < nj; j++)
      for (i = 0; i < ni; i++)
        x(i,j,k) = (_TYPE_)inco[mat.renum(i,j,k)]; //!!!!!!!!!!!!!!!!!! ToDo WARNING: potentiellement conversion de double en float!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void op_negate(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x)
{
  const int g = x.ghost();
  const int imin = x.linear_index(-g,-g,-g);
  const int imax = x.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1;
  _TYPE_ *ptr = x.data().addr();
  for (int i = imin; i < imax; i++)
    ptr[i] = -ptr[i];
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void op_multiply(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, _TYPE_ a)
{
  const int g = x.ghost();
  const int imin = x.linear_index(-g,-g,-g);
  const int imax = x.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1;
  _TYPE_ *ptr = x.data().addr();
  for (int i = imin; i < imax; i++)
    ptr[i] *= a;
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void op_sub(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& y)
{
  const int g = x.ghost();
  const int imin = x.linear_index(-g,-g,-g);
  const int imax = x.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1;
  assert(imin == y.linear_index(-g,-g,-g));
  assert(imax == y.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1);
  _TYPE_ *ptr = x.data().addr();
  const _TYPE_ *ptr2 = y.data().addr();
  for (int i = imin; i < imax; i++)
    ptr[i] -= ptr2[i];
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void ajoute_alpha_v(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, _TYPE_ alpha, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& v)
{
  const int g = x.ghost();
  const int imin = x.linear_index(-g,-g,-g);
  const int imax = x.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1;
  assert(imin == v.linear_index(-g,-g,-g));
  assert(imax == v.linear_index(x.ni()-1+g, x.nj()-1+g, x.nk()-1+g) + 1);
  _TYPE_ *ptr = x.data().addr();
  const _TYPE_ *ptr2 = v.data().addr();
  for (int i = imin; i < imax; i++)
    ptr[i] += alpha * ptr2[i];
}

// Ne semble pas fonctionner (manque juste echange espace virtuel sur second membre ?)
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::resoudre_avec_gcp(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& b, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu)
{
  IJK_Field_template<_TYPE_,_TYPE_ARRAY_> Z, P, R;
  alloc_field(Z, 0);
  alloc_field(P, 0);
  alloc_field(R, 0);
  x.data() = 0.;
  R.data() = b.data();
  op_negate(R);

  multigrille(P, R, residu);

  _TYPE_ dold = prod_scal_ijk(R, P);
  op_negate(P);

  for (int iter = 0; iter < max_iter_gcp_; iter++)
    {
      // calcul de AP = A * P
      jacobi_residu(P, 0 /* secmem */, 0 /* grid_level */, 0 /* no jacobi */, &residu);

      _TYPE_ alpha = dold / prod_scal_ijk(P, residu);

      Cout << "alpha=" << alpha << " normeP=" << norme_ijk(P) << " dold=" << dold << finl;

      ajoute_alpha_v(x, alpha, P);

      ajoute_alpha_v(R, alpha, residu);

      _TYPE_ nr = (_TYPE_)norme_ijk(R);
      Cout << "GCP+MG iteration " << iter << " residu " << nr << finl;
      if (nr < seuil_)
        break;

      if (iter == max_iter_gcp_)
        {
          Cerr << "Non convergence de Multigrille_poreux::resoudre_avec_gcp en " << max_iter_gcp_
               << " iterations" << finl;
          break;
        }

      // preconditionner
      multigrille(Z, R, residu);

      _TYPE_ prodscal_RZ = prod_scal_ijk(R, Z);
      _TYPE_ beta = prodscal_RZ / dold;
      dold = prodscal_RZ;

      op_multiply(P, beta);
      op_sub(P, Z);
    }
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
inline double ajouter_alpha_v_multiple_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, ArrOfDouble& coeff, IJK_Vector<IJK_Field_template, _TYPE_, _TYPE_ARRAY_>& y, int n, int nfirst)
{
  if (n > 4)
    {
      Cerr << "Erreur ajouter_alpha_v_multiple" << finl;
      Process::exit();
    }
  const int ni = x.ni();
  const int nj = x.nj();
  const int nk = x.nk();
  _TYPE_ * x_ptr = x.data().addr();
  _TYPE_ * y_ptr[4];
  _TYPE_ c[4];
  int nn;
  double somme = 0.;
  for (nn = 0; nn < n; nn++)
    {
      y_ptr[nn] = y[nfirst + nn].data().addr();
      c[nn] = (_TYPE_)coeff[nfirst + nn];
    }
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          int index = x.linear_index(0,j,k);
          for (int i = 0; i < ni; i++)
            {
              _TYPE_ val = x_ptr[index+i];
              for (nn = 0; nn < n; nn++)
                val += c[nn] * y_ptr[nn][index+i];
              x_ptr[index+i] = val;
              somme += val * val;
            }
        }
    }
  return somme;
}

// Calcule ceci
//   x += coeff[0] * y[0] + ... + coeff[n-1] * y[n-1]
// Renvoie la somme des x^2 du resultat sur le processeur local
template <typename _TYPE_, typename _TYPE_ARRAY_>
double ajouter_alpha_v_multiple(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, ArrOfDouble& coeff, IJK_Vector<IJK_Field_template, _TYPE_, _TYPE_ARRAY_>& y, int n)
{
  int i = 0;
  double somme = 0.;
  while (i < n)
    {
      int nn = n - i;
      if (nn > 4)
        nn = 4;
      if (nn == 1)
        somme += ajouter_alpha_v_multiple_(x, coeff, y, 1, i);
      else if (nn == 2)
        somme += ajouter_alpha_v_multiple_(x, coeff, y, 2, i);
      else if (nn == 3)
        somme += ajouter_alpha_v_multiple_(x, coeff, y, 3, i);
      else if (nn == 4)
        somme += ajouter_alpha_v_multiple_(x, coeff, y, 4, i);

      i += nn;
    }
  return somme;
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
inline void calcul_produits_scalaires_(IJK_Vector<IJK_Field_template, _TYPE_, _TYPE_ARRAY_>& x, int n, int nfirst, int m, ArrOfDouble& resu)
{
  if (n > 4)
    {
      Cerr << "Erreur " << finl;
      Process::exit();
    }
  const int ni = x[0].ni();
  const int nj = x[0].nj();
  const int nk = x[0].nk();
  _TYPE_ * x_ptr = x[m].data().addr();
  _TYPE_ * y_ptr[4];
  _TYPE_ c[4] = { 0., 0., 0., 0. };
  int nn;
  for (nn = 0; nn < n; nn++)
    {
      y_ptr[nn] = x[nfirst + nn].data().addr();
      c[nn] = 0;
    }
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          int index = x[0].linear_index(0,j,k);
          for (int i = 0; i < ni; i++)
            {
              _TYPE_ val = x_ptr[index+i];
              for (nn = 0; nn < n; nn++)
                c[nn] += val * y_ptr[nn][index+i];
            }
        }
    }
  for (nn = 0; nn < n; nn++)
    resu[nfirst + nn] += c[nn];
}

// Calcule ceci, pour i=0..n avec n = resu.size_array()-1
//   resu[i] = x[i] scalaire x[n]
// Renvoie la somme des x^2 du resultat sur le processeur local
template <typename _TYPE_, typename _TYPE_ARRAY_>
void calcul_produits_scalaires(IJK_Vector<IJK_Field_template, _TYPE_, _TYPE_ARRAY_>& x, ArrOfDouble& resu)
{
  int i = 0;
  resu = 0.;
  const int n = resu.size_array() - 1;
  while (i < n)
    {
      int nn = n - i;
      if (nn > 4)
        nn = 4;
      if (nn == 1)
        calcul_produits_scalaires_(x, 1, i, n, resu);
      else if (nn == 2)
        calcul_produits_scalaires_(x, 2, i, n, resu);
      else if (nn == 3)
        calcul_produits_scalaires_(x, 3, i, n, resu);
      else if (nn == 4)
        calcul_produits_scalaires_(x, 4, i, n, resu);
      i += nn;
    }
  mp_sum_for_each_item(resu);
}

static inline void triangularise(const DoubleTab& hessenberg, const double norme_b, DoubleTab& resu, ArrOfDouble& r)
{
  const int n = hessenberg.dimension(1);
  assert(hessenberg.dimension(0) == n+1);
  assert(r.size_array() == n + 1);

  r[0] = norme_b;

  // Triangularisation
  int i;
  for(i = 0; i < n; i++)
    {
      double ccos, ssin;
      {
        const double h_ii = hessenberg(i, i);
        const double h_i1i = hessenberg(i + 1, i);
        const double tmp_val = 1. / sqrt(h_ii * h_ii + h_i1i * h_i1i);
        ccos = h_ii * tmp_val;
        ssin = - h_i1i * tmp_val;
      }
      for (int j = i; j < n; j++)
        {
          const double h_ij = hessenberg(i, j);
          const double h_i1j = hessenberg(i + 1, j);
          resu(i, j)     = ccos * h_ij - ssin * h_i1j;
          resu(i + 1, j) = ssin * h_ij + ccos * h_i1j;
        }
      const double ri = r[i];
      r[i] = ccos * ri;
      r[i + 1] = ssin * ri;
    }
}

// si plus de 3 vecteurs de base, semble atteindre la limite de precision numerique...
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::resoudre_avec_gmres(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& b, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu)
{
  int n_krilov = n_krilov_;
  DoubleTab hessenberg(n_krilov + 1, n_krilov);

  // Espace de Krilov
  IJK_Vector<IJK_Field_template, _TYPE_, _TYPE_ARRAY_> krilov_space;
  // Vecteurs des M^(-1) * krilov_space[i] ou M^-1 est le preconditionneur:
  IJK_Vector<IJK_Field_template, _TYPE_, _TYPE_ARRAY_> m_krilov_space;
  {
    IJK_Field_template<_TYPE_,_TYPE_ARRAY_> titi;
    for (int ii=0; ii < n_krilov+1; ii++)
      krilov_space.add(titi);
    for (int ii=0; ii < n_krilov; ii++)
      m_krilov_space.add(titi);
  }

  krilov_space[0] = b;
  for (int ii=0; ii < n_krilov; ii++)
    {
      alloc_field(krilov_space[ii+1], 0);
      alloc_field(m_krilov_space[ii], 0, true /* with additional layers for jacobi sweeps */);
    }

  x.data() = 0.;

  for (int iter = 0; ; iter++)
    {

      // Hypothese de depart: krilov_space[0] = -residu
      const double norme_b = norme_ijk(krilov_space[0]);

      if (impr_gmres_)
        Cout << "gmres iteration " << iter << " norme(residu) " << norme_b << finl;
      if (norme_b < seuil_ || norme_b == 0.)
        {
          if (impr_gmres_)
            Cout << "gmres: norme(residu) < seuil=" << seuil_ << " => return" << finl;
          return;
        }
      if (iter >= max_iter_gmres_)
        {
          Cerr << "Error in Multigrille_base::resoudre_avec_gmres: did not converge in " << max_iter_gmres_ << " restarts."
               << "\n Residue= " << norme_b << finl;
          Process::exit();
        }
      krilov_space[0].data() *= (_TYPE_)(1. / norme_b);

      hessenberg = 0.;

      for (int i = 0; i < n_krilov; i++)
        {
          // Produit matrice vecteur : v1 = A * M^(-1) * v0  (ou M^(-1) est une approx de A^(-1) par multigrille)
          m_krilov_space[i].data() = 0.;
          m_krilov_space[i].shift_k_origin(needed_kshift_for_jacobi(0) - m_krilov_space[i].k_shift());
          krilov_space[i].echange_espace_virtuel(krilov_space[i].ghost());
          // Stockage de M^-1 * v0 dans m_krilov_space[i]
          // et on met le residu dans krilov_space[i+1]
          multigrille(m_krilov_space[i], krilov_space[i], krilov_space[i+1]);

          // remarque Titi23
          // On obtient krilov_space[i+1] = A * m_krilov_space[i] - krilov_space[i]
          // normalement, on devrait faire
          //  krilov_space[i+1] += krilov_space[i]
          // L'orthogonalisation va retirer la composante krilov_space[i],
          // mais va calculer "prod_scal_ijk(krilov_space[i+1], krilov_space[i]) - 1"

          // Orthogonalisation:
          //  v0 = somme pour 0 <= j <= i des (krilov_space[j] * hessenberg(j, i)
          ArrOfDouble produit_scal(i+2);
          ArrOfDouble coeff(i+1);
          calcul_produits_scalaires(krilov_space, produit_scal);
          for (int j = 0; j <= i; j++)
            {
              double h = produit_scal[j];
              coeff[j] = -h;
              //norme2_prevue -= h*h;
              if (j == i)
                {
                  // voir Titi23 ci-dessus:
                  // on n'a pas rajoute krilov_space[i] toute a l'heure a krilov_space[i+1],
                  // le produit scalaire a stocker dans hessenberg vaut donc 1 de plus:
                  h += 1.;
                }
              hessenberg(j, i) = h;
            }
          double norme_v0 = ajouter_alpha_v_multiple(krilov_space[i+1], coeff, krilov_space, i+1);
          norme_v0 = sqrt(mp_sum(norme_v0));
          hessenberg(i + 1, i) = norme_v0;
          krilov_space[i+1].data() *= (_TYPE_)(1. / norme_v0);
        }

      DoubleTab mat(n_krilov + 1, n_krilov);
      ArrOfDouble r__(n_krilov + 1);
      triangularise(hessenberg, norme_b, mat, r__);

      //...Solution of linear system
      for (int i = n_krilov - 1; i >= 0; i--)
        {
          r__[i] /= mat(i, i);
          for (int j = i-1; j >= 0; j--)
            r__[j] -= mat(j, i) * r__[i];
        }
      ajouter_alpha_v_multiple(x, r__, m_krilov_space, n_krilov);
      if (impr_gmres_)
        {
          Cout << "facteurs r : ";
          for (int i = 0; i < r__.size_array(); i++)
            Cout << r__[i] << " ";
          Cout << finl;
          if (impr_gmres_ > 1)
            {
              Cout << "matrice de hessenberg " << hessenberg << finl;
            }
        }
      // On stocke le residu dans krilov_space[0] pour l'iteration suivante:
      jacobi_residu(x, &b, 0 /* grid_level */, 0 /* no jacobi */, &krilov_space[0]);
      prepare_secmem(krilov_space[0]);
      op_negate(krilov_space[0]);
    }
}


template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_base::resoudre_systeme_template(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_b = get_storage_template<_TYPE_>(STORAGE_RHS, 0);
  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x = get_storage_template<_TYPE_>(STORAGE_X, 0) ;
  ijk_x.shift_k_origin(needed_kshift_for_jacobi(0) - ijk_x.k_shift());

  convert_to_ijk(b, ijk_b);

  solve_ijk_in_storage_template<_TYPE_>();

  convert_from_ijk(ijk_x, x);

  x.echange_espace_virtuel();

  if (check_residu_)
    {
      // Calcul du residu
      DoubleVect residu(b);
      a.multvect(x, residu);

      const Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc,a);
      const Matrice_Morse_Sym& la_matrice =ref_cast(Matrice_Morse_Sym,matrice_bloc.get_bloc(0,0).valeur());
      // Terme diagonal multiplie par 2 dans Assembleur_P_VDF.cpp: assembler_QC()
      if (Process::je_suis_maitre())
        residu[0] -= x[0] * la_matrice(0,0) * 0.5;
      Cout << "Norme de Ax et residu: " << mp_norme_vect(residu);
      residu -= b;
      Cout << " " << mp_norme_vect(residu) << finl;
    }

}


#endif
