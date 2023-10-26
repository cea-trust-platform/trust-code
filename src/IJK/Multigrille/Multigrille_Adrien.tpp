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

#ifndef Multigrille_Adrien_TPP_H
#define Multigrille_Adrien_TPP_H

#include <SSE_kernels.h>
using namespace SSE_Kernels;

static long long flop_count = 0;

// Substract the average of the x field to get a zero average field.
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::prepare_secmem_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x) const
{
  double moyenne = somme_ijk(x);
  double nb_elem_tot = (double) x.get_splitting().get_nb_items_global(IJK_Splitting::ELEM, DIRECTION_I)
                       * (double) x.get_splitting().get_nb_items_global(IJK_Splitting::ELEM, DIRECTION_J)
                       * (double) x.get_splitting().get_nb_items_global(IJK_Splitting::ELEM, DIRECTION_K);
  double val = moyenne / nb_elem_tot;
  const int m = x.data().size_array();
  for (int i = 0; i < m; i++)
    x.data()[i] -= (_TYPE_)val;
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::dump_lata_(const Nom& field, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& data, int tstep) const
{
  /* const IJK_Grid_Geometry & g = grids_data_float_[0].get_grid_geometry();
  data.dumplata(field,
   g.get_node_coordinates(0),
   g.get_node_coordinates(1),
   g.get_node_coordinates(2),
   tstep); */
  Process::exit();
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::set_rho(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rho)
{
  if (solver_precision_ == precision_double_)
    set_rho_template<double,_TYPE_,_TYPE_ARRAY_>(rho, true, false);
  else if (solver_precision_ == precision_float_)
    set_rho_template<float,_TYPE_,_TYPE_ARRAY_>(rho, true, false);
  else
    {
      set_rho_template<double,_TYPE_,_TYPE_ARRAY_>(rho, false, false);
      set_rho_template<float,_TYPE_,_TYPE_ARRAY_>(rho, true, true);
    }
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::set_inv_rho(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& inv_rho)
{
  if (solver_precision_ == precision_double_)
    set_inv_rho_template<double,_TYPE_,_TYPE_ARRAY_>(inv_rho, true, false);
  else if (solver_precision_ == precision_float_)
    set_inv_rho_template<float,_TYPE_,_TYPE_ARRAY_>(inv_rho, true, false);
  else
    {
      set_inv_rho_template<double,_TYPE_,_TYPE_ARRAY_>(inv_rho, false, false);
      set_inv_rho_template<float,_TYPE_,_TYPE_ARRAY_>(inv_rho, true, true);
    }
}

template <typename _TYPE_FUNC_, typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::set_rho_template(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rho, bool set_coarse_matrix_flag, bool use_coeffs_from_double)
{
  constexpr bool IS_DOUBLE = std::is_same<_TYPE_FUNC_, double>::value;

  // size might be 1 for mixed precision solver if updating the double precision part:
  const int nlevels = get_grid_data_size<_TYPE_FUNC_>();
  const int ghost = get_grid_data<_TYPE_FUNC_>(0).get_rho().ghost();

  for (int l = 0; l < nlevels; l++)
    {
      // Fill the "rho" field
      if (l == 0)
        {
          IJK_Field_template<_TYPE_FUNC_,TRUSTArray<_TYPE_FUNC_>>& r = set_grid_data<_TYPE_FUNC_>(l).get_update_rho();
          int ni = r.ni();
          int nj = r.nj();
          int nk = r.nk();
          int i, j, k;
          for (k = 0; k < nk; k++)
            for (j = 0; j < nj; j++)
              for (i = 0; i < ni; i++)
                r(i,j,k) = (_TYPE_FUNC_)rho(i,j,k);

          // echange espace virtuel sur rho sans passer par IJK_Field --> mauvais remplissage des coeffs de la matrice pour le shear periodique
          // modif pour shear-periodicite, que lechange_espace_virtuel soit bien fait pour rho ou inv_rho dans le solveur de Pousson
          //set_grid_data<_TYPE_FUNC_>(l).get_update_rho().set_indicatrice_ghost_zmin_(rho.get_indicatrice_ghost_zmin_());
          //set_grid_data<_TYPE_FUNC_>(l).get_update_rho().set_indicatrice_ghost_zmax_(rho.get_indicatrice_ghost_zmax_());
        }
      else
        coarsen_operators_[l-1].valeur().coarsen(set_grid_data<_TYPE_FUNC_>(l-1).get_rho(),
                                                 set_grid_data<_TYPE_FUNC_>(l).get_update_rho(),
                                                 1 /* compute average, not sum */);

      set_grid_data<_TYPE_FUNC_>(l).get_update_rho().echange_espace_virtuel(ghost);

      // Compute matrix coefficients at faces
      if(IS_DOUBLE)
        grids_data_double_[l].compute_faces_coefficients_from_rho();
      else
        {
          if (use_coeffs_from_double && l < grids_data_double_.size())
            grids_data_float_[l].compute_faces_coefficients_from_double_coeffs(grids_data_double_[l]);
          else
            grids_data_float_[l].compute_faces_coefficients_from_rho();
        }
    }

  // Update coarse problem matrix:
  if (set_coarse_matrix_flag)
    {
      const int coarse_level = nlevels - 1;
      set_coarse_matrix().build_matrix(set_grid_data<_TYPE_FUNC_>(coarse_level).get_faces_coefficients());
    }
}

template <typename _TYPE_FUNC_, typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::set_inv_rho_template(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rho, bool set_coarse_matrix_flag, bool use_coeffs_from_double)
{
  constexpr bool IS_DOUBLE = std::is_same<_TYPE_FUNC_, double>::value;

  // size might be 1 for mixed precision solver if updating the double precision part:
  const int nlevels = get_grid_data_size<_TYPE_FUNC_>();
  int i;

  const int ghost = get_grid_data<_TYPE_FUNC_>(0).get_rho().ghost();

  for (i = 0; i < nlevels; i++)
    {
      // Fill the "rho" field
      if (i == 0)
        {
          IJK_Field_template<_TYPE_FUNC_,TRUSTArray<_TYPE_FUNC_>>& r = set_grid_data<_TYPE_FUNC_>(i).get_update_rho();
          int ni = r.ni();
          int nj = r.nj();
          int nk = r.nk();
          int i2, j, k;
          for (k = 0; k < nk; k++)
            for (j = 0; j < nj; j++)
              for (i2 = 0; i2 < ni; i2++)
                r(i2,j,k) = (_TYPE_FUNC_)rho(i2,j,k);
          // modif pour shear-periodicite, que lechange_espace_virtuel soit bien fait pour rho ou inv_rho dans le solveur de Poisson
          // on ajoute ca pour le shear perio, uniquement sur le premier niveau de multigrille pour l'instant
          //set_grid_data<_TYPE_FUNC_>(i).get_update_rho().set_indicatrice_ghost_zmin_(rho.get_indicatrice_ghost_zmin_());
          //set_grid_data<_TYPE_FUNC_>(i).get_update_rho().set_indicatrice_ghost_zmax_(rho.get_indicatrice_ghost_zmax_());
        }
      else
        {
          coarsen_operators_[i-1].valeur().coarsen(set_grid_data<_TYPE_FUNC_>(i-1).get_rho(),
                                                   set_grid_data<_TYPE_FUNC_>(i).get_update_rho(),
                                                   1 /* compute average, not sum */);
        }

      set_grid_data<_TYPE_FUNC_>(i).get_update_rho().echange_espace_virtuel(ghost);

      if(IS_DOUBLE)
        {
          grids_data_double_[i].compute_faces_coefficients_from_inv_rho();
        }
      else
        {
          // Compute matrix coefficients at faces
          if (use_coeffs_from_double && i < grids_data_double_.size())
            grids_data_float_[i].compute_faces_coefficients_from_double_coeffs(grids_data_double_[i]);
          else
            grids_data_float_[i].compute_faces_coefficients_from_inv_rho();
        }
    }

  // Update coarse problem matrix:
  if (set_coarse_matrix_flag)
    {
      const int coarse_level = nlevels - 1;
      set_coarse_matrix().build_matrix(set_grid_data<_TYPE_FUNC_>(coarse_level).get_faces_coefficients());
    }
}



// Apply n_jacobi iterations of jacobi to x vector,
//  and, if the residu pointer is not zero, computes the residue.
// Precondition: ghost cells for secmem must be up to date.
//  The number of required ghost layers is n_jacobi + (1 if residu is required)
// Postcondition: ghost cells are not updated
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::jacobi_residu_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x,
                                        const IJK_Field_template<_TYPE_,_TYPE_ARRAY_> *secmem,
                                        const int grid_level,
                                        const int n_jacobi_tot,
                                        IJK_Field_template<_TYPE_,_TYPE_ARRAY_> *residu) const
{
  static Stat_Counter_Id jacobi_residu_counter_ = statistiques().new_counter(2, "multigrille : residu jacobi");

  statistiques().begin_count(jacobi_residu_counter_);

  const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& coeffs_face = get_grid_data<_TYPE_>(grid_level).get_faces_coefficients();

  //IJ_layout layout(x);
  const _TYPE_ relax = (_TYPE_)relax_jacobi(grid_level);
  // We can do at most this number of passes per sweep:
  const int nb_passes_max_per_sweep = x.ghost();
  // We have to do this number of passes in total:
  const int nb_passes_to_do_total = n_jacobi_tot + (residu ? 1 : 0);
  int nb_passes_done = 0;

  // Repeat as many sweeps as necessary:
  while (nb_passes_done < nb_passes_to_do_total)
    {

      int nb_passes_to_do = nb_passes_to_do_total - nb_passes_done;
      if (nb_passes_to_do > nb_passes_max_per_sweep)
        nb_passes_to_do = nb_passes_max_per_sweep;

      x.echange_espace_virtuel(nb_passes_to_do);

      if (grid_level == 0)
        {
          // Place counter here to avoid counting communication time:
          //statistiques().begin_count(counter);
          flop_count = 0;
        }

      const bool last_pass_is_residue = (nb_passes_done + nb_passes_to_do == n_jacobi_tot + 1);
      Multipass_Jacobi_template<_TYPE_, _TYPE_ARRAY_, SSE_Kernels::GENERIC_STRIDE, SSE_Kernels::GENERIC_STRIDE>(x, *residu, coeffs_face, *secmem, nb_passes_to_do, last_pass_is_residue, relax);

      nb_passes_done += nb_passes_to_do;

      if (grid_level == 0)
        {
          //statistiques().end_count(counter, flop_count);
          flop_count = 0;
        }
    }
  statistiques().end_count(jacobi_residu_counter_, (int)flop_count);
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::coarsen_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, int fine_level) const
{
  coarsen_operators_[fine_level].valeur().coarsen(fine, coarse);
}

// calcul de "fine -= interpolated(coarse)"
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::interpolate_sub_shiftk_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, int fine_level) const
{
  // Shift by maximum value:
  const int shift = needed_kshift_for_jacobi(fine_level) - fine.k_shift();
  coarsen_operators_[fine_level].valeur().interpolate_sub_shiftk(coarse, fine, shift);
}


template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::completer_template(const IJK_Splitting& split)
{
  Cerr << "Multigrille_Adrien::completer_template" << finl;

  const int nb_operators = coarsen_operators_.size();
  const int nb_grids = nb_operators + 1;
  constexpr bool IS_DOUBLE = std::is_same<_TYPE_, double>::value;
  if(IS_DOUBLE)
    grids_data_double_.dimensionner(nb_grids);
  else
    grids_data_float_.dimensionner(nb_grids);

  set_grid_data<_TYPE_>(0).initialize(split, ghost_size_, nsweeps_jacobi_residu(0));

  int i;
  for (i = 0; i < nb_operators; i++)
    {
      coarsen_operators_[i].valeur().initialize_grid_data(set_grid_data<_TYPE_>(i), set_grid_data<_TYPE_>(i+1),
                                                          nsweeps_jacobi_residu(i+1));
    }
  for (i = 0; i < nb_grids; i++)
    {
      const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& g = get_grid_data<_TYPE_>(i).get_rho();
      Journal() << "Grid level " << i << " local size: " << g.ni() << "x" << g.nj() << "x" << g.nk() << finl;
    }
}



// Allocate an array for the grid data
template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multigrille_Adrien::alloc_field_( IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& field, int level, bool with_additional_layers) const
{
  if (level < 0 || level > get_grid_data_size<_TYPE_>())
    {
      Cerr << "Fatal: wrong level in alloc_field" << finl;
      Process::exit();
    }
  int n = 0;
  if (with_additional_layers)
    n = ghost_size_;
  field.allocate(get_grid_data<_TYPE_>(level).get_splitting(), IJK_Splitting::ELEM, ghost_size_, n);
}




template <typename _TYPE_>
IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>>& Multigrille_Adrien::get_storage_template_(StorageId id, int level)
{
  switch (id)
    {
    case STORAGE_RHS:
      return set_grid_data<_TYPE_>(level).get_update_rhs();
    case STORAGE_X:
      return set_grid_data<_TYPE_>(level).get_update_x();
    case STORAGE_RESIDUE:
      return set_grid_data<_TYPE_>(level).get_update_residue();
    default:
      Cerr << "Error in Multigrille_Adrien::get_storage_float" << finl;
      Process::exit();
    }
  return set_grid_data<_TYPE_>(level).get_update_rhs(); // never arrive here...
}






#endif
