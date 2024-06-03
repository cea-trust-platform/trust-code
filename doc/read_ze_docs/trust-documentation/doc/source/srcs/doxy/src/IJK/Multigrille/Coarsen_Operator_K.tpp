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
#ifndef Coarsen_Operator_K_TPP_H
#define Coarsen_Operator_K_TPP_H

#include <IJK_Grid_Geometry.h>
#include <EFichier.h>
#include <communications.h>
#include <TRUSTTab.h>
#include <stat_counters.h>

template<typename _TYPE_>
void Coarsen_Operator_K::initialize_grid_data_(const Grid_Level_Data_template<_TYPE_>& fine, Grid_Level_Data_template<_TYPE_>& coarse,
                                               int additional_k_layers)
{
  //IntTab src_dest_index;
  src_dest_index_.set_smart_resize(1);
  src_dest_index_.resize(0,2);
  coarsen_coefficients_.set_smart_resize(1);
  coarsen_coefficients_.resize_array(0);
  avg_coefficients_.set_smart_resize(1);
  avg_coefficients_.resize_array(0);

  const IJK_Grid_Geometry& src_grid_geom = fine.get_grid_geometry();
  const ArrOfDouble& coord_z_fine = src_grid_geom.get_node_coordinates(2 /* k direction */);
  const ArrOfDouble& coord_z_coarse = z_coord_all_;

  const double epsilon = precision_geom;
  const int n_coord_fine = coord_z_fine.size_array();
  const int n_coord_coarse = coord_z_coarse.size_array();
  if (std::fabs(coord_z_fine[0] - coord_z_coarse[0]) > epsilon
      || std::fabs(coord_z_fine[n_coord_fine-1] - coord_z_coarse[n_coord_coarse-1]) > epsilon)
    {
      Cerr << "Error in Coarsen_Operator_K::initialize_grid_data: coarse and fine grids have wrong size" << finl;
      Cerr << " fine grid:    " << coord_z_fine[0] << " .. " << coord_z_fine[n_coord_fine-1] << finl;
      Cerr << " coarse grid:  " << coord_z_coarse[0] << " .. " << coord_z_coarse[n_coord_coarse-1] << finl;
      Process::exit();
    }

  ArrOfDouble coarse_delta_k(n_coord_coarse - 1);

  for (int i = 0; i < n_coord_coarse - 1; i++)
    coarse_delta_k[i] = coord_z_coarse[i+1] - coord_z_coarse[i];

  int current_coarse_cell = 0;
  int current_fine_cell = 0;
  Journal() << "Coarsen_Operator_K: global coarsening coefficients:\nfine_k coarse_k coarsen_coeff avg_coeff:" << endl;
  while (1)
    {
      const int nlines = src_dest_index_.dimension(0);
      src_dest_index_.resize(nlines + 1, 2);
      src_dest_index_(nlines, 0) = current_fine_cell;
      src_dest_index_(nlines, 1) = current_coarse_cell;
      double fine_cell_size = coord_z_fine[current_fine_cell+1] - coord_z_fine[current_fine_cell];
      double coarse_cell_size = coarse_delta_k[current_coarse_cell];
      const double zmin = std::max(coord_z_fine[current_fine_cell], coord_z_coarse[current_coarse_cell]);
      const double zmax = std::min(coord_z_fine[current_fine_cell+1], coord_z_coarse[current_coarse_cell+1]);
      double intersection = zmax - zmin;
      if (fine_cell_size <= 0. || coarse_cell_size <= 0. || intersection <= 0.)
        {
          Cerr << "Error in Coarsen_Operator_K::initialize_grid_data: wrong cell sizes" << finl
               << " fine cell " << current_fine_cell << " zmin=" << coord_z_fine[current_fine_cell] ;
          Cerr << " zmax=" << coord_z_fine[current_fine_cell+1] << finl;
          Cerr << " coarse cell " << current_coarse_cell << " zmin=" << coord_z_coarse[current_coarse_cell]
               << " zmax=" << coord_z_coarse[current_coarse_cell+1] << finl;
          Process::exit();
        }
      // Compute contribution of this cell/cell intersection
      coarsen_coefficients_.append_array(intersection / fine_cell_size);
      avg_coefficients_.append_array(intersection / coarse_cell_size);
      Journal() << current_fine_cell << " " << current_coarse_cell << " "
                << coarsen_coefficients_[nlines] << " " << avg_coefficients_[nlines] << endl;

      // Advance to next cell/cell intersection:
      if (std::fabs(coord_z_fine[current_fine_cell + 1] - coord_z_coarse[current_coarse_cell+1]) < epsilon)
        {
          // advance in fine mesh and coarse mesh
          current_fine_cell++;
          current_coarse_cell++;
        }
      else if (coord_z_fine[current_fine_cell + 1] < coord_z_coarse[current_coarse_cell+1])
        {
          // advance in the fine mesh
          current_fine_cell++;
        }
      else
        {
          // advance in the coarse mesh
          current_coarse_cell++;
        }
      // If beyond last cell, exit:
      if (current_coarse_cell == n_coord_coarse-1)
        {
          assert(current_fine_cell == n_coord_fine-1);
          break;
        }
      assert(current_fine_cell < n_coord_fine - 1);
    }

  IJK_Grid_Geometry grid_geom;
  grid_geom.initialize_origin_deltas(src_grid_geom.get_origin(0),
                                     src_grid_geom.get_origin(1),
                                     src_grid_geom.get_origin(2),
                                     src_grid_geom.get_delta(0),
                                     src_grid_geom.get_delta(1),
                                     coarse_delta_k,
                                     src_grid_geom.get_periodic_flag(0),
                                     src_grid_geom.get_periodic_flag(1),
                                     src_grid_geom.get_periodic_flag(2));

  // For the moment, the algorithm cannot interpolate data across processors so the mesh boundaries on each processor
  // on the coarse and on the fine meshes must coincide (message "cannot merge")
  // Compute the splitting of the coarse mesh: coarsened cells are on the same processor than the fine cells
  // they come from:
  IJK_Splitting coarse_splitting;
  // Same processor mapping as fine mesh
  IntTab processor_mapping;
  fine.get_splitting().get_processor_mapping(processor_mapping);
  // Same splitting in i and j directions
  ArrOfInt slice_size_i, slice_size_j, fine_slice_size_k, coarse_slice_size_k;
  fine.get_splitting().get_slice_size(0, IJK_Splitting::ELEM, slice_size_i);
  fine.get_splitting().get_slice_size(1, IJK_Splitting::ELEM, slice_size_j);
  fine.get_splitting().get_slice_size(2, IJK_Splitting::ELEM, fine_slice_size_k);
  coarse_slice_size_k.resize_array(fine_slice_size_k.size_array());
  // compute sizes of slices in the k direction:
  {
    int slice_num = 0;
    bool error = false;
    int end_of_fine_slice = fine_slice_size_k[0];
    int previous_coarse_cell = -1;
    for (int i = 0; i < src_dest_index_.dimension(0); i++)
      {
        // Index of the first cell of the next slice in the fine mesh:
        int next_coarse_cell = src_dest_index_(i,1);
        if (src_dest_index_(i,0) >= end_of_fine_slice)
          {
            // Finished a slice in the fine mesh: go to next slice
            // Check if we go to a new cell in the coarse mesh right here:
            if (previous_coarse_cell == next_coarse_cell)
              {
                // The current and the next slice have an intersection with the same coarse cell.
                // This is currently not supported
                Cerr << "Error in Coarsen_Operator_K::initialize_grid_data: "
                     << " cannot merge cells across processors, you must put a coarse node at z=";
                Cerr << coord_z_fine[src_dest_index_(i,0)] << " (or modify processor splitting)" << finl;
                error = true;
              }
            slice_num++;
            end_of_fine_slice += fine_slice_size_k[slice_num];
          }
        if (previous_coarse_cell != next_coarse_cell)
          coarse_slice_size_k[slice_num]++;
        previous_coarse_cell = next_coarse_cell;
      }
    // Display all errors before exiting:
    if (error)
      Process::exit();
  }
  coarse_splitting.initialize(grid_geom, slice_size_i, slice_size_j, coarse_slice_size_k, processor_mapping);
  const int ghost_domaine_size = fine.get_ghost_size();
  coarse.initialize(coarse_splitting, ghost_domaine_size, additional_k_layers);

  // Build "local" intersection data:
  {
    src_dest_index_local_.reset();
    src_dest_index_local_.set_smart_resize(1);
    coarsen_coefficients_local_.reset();
    coarsen_coefficients_local_.set_smart_resize(1);
    avg_coefficients_local_.reset();
    avg_coefficients_local_.set_smart_resize(1);

    const int fine_k_offset = fine.get_splitting().get_offset_local(DIRECTION_K);
    const int fine_start = fine_k_offset;
    const int fine_nlocal = fine.get_splitting().get_nb_elem_local(DIRECTION_K);
    const int coarse_k_offset = coarse.get_splitting().get_offset_local(DIRECTION_K);
    const int coarse_start = coarse_k_offset;
    const int coarse_nlocal = coarse.get_splitting().get_nb_elem_local(DIRECTION_K);


    const int n = src_dest_index_.dimension(0);
    Journal() << "Coarsen_Operator_K: local coarsening coefficients:\nfine_k coarse_k coarsen_coeff avg_coeff:" << endl;
    for (int i = 0; i < n; i++)
      {
        const int fine_k = src_dest_index_(i,0);
        const int coarse_k = src_dest_index_(i,1);
        // Find coefficients that affect the local values on this processor,
        // either at interpolation step or at coarsening step
        if ((fine_k >= fine_start && fine_k < fine_start + fine_nlocal)
            || (coarse_k >= coarse_start && coarse_k < coarse_start + coarse_nlocal))
          {
            const int sz = src_dest_index_local_.dimension(0);
            src_dest_index_local_.resize(sz+1, 2);
            src_dest_index_local_(sz, 0) = fine_k - fine_k_offset;
            src_dest_index_local_(sz, 1) = coarse_k - coarse_k_offset;
            coarsen_coefficients_local_.append_array(coarsen_coefficients_[i]);
            avg_coefficients_local_.append_array(avg_coefficients_[i]);
            Journal() << fine_k - fine_k_offset << " " << coarse_k - coarse_k_offset << " "
                      << coarsen_coefficients_[i] << " " << avg_coefficients_[i] << endl;
          }
      }
  }
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Coarsen_Operator_K::coarsen_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, int compute_weighted_average) const
{
  static Stat_Counter_Id coarsen_counter_ = statistiques().new_counter(2, "multigrille : K coarsen ");
  statistiques().begin_count(coarsen_counter_);

  const int index_start = 0;
  const int index_end = src_dest_index_local_.dimension(0);
  const int delta_index = 1;

  const int ni = coarse.ni();
  const int nj = coarse.nj();

  const ArrOfDouble& coef_array = compute_weighted_average ? avg_coefficients_local_ : coarsen_coefficients_local_;

  int previous_coarse_k = -10;

  for (int index = index_start; index != index_end; index += delta_index)
    {
      // Source layer and destination layer (src_dest_index_ contains an index in the entire mesh)
      const int fine_k = src_dest_index_local_(index, 0);
      const int coarse_k = src_dest_index_local_(index, 1);

      const _TYPE_ coef = (_TYPE_)coef_array[index];
      if (coarse_k != previous_coarse_k)
        {
          // Start a new layer, overwrite value
          for (int j = 0; j < nj; j++)
            for (int i = 0; i < ni; i++)
              coarse(i, j, coarse_k) = coef * fine(i, j, fine_k);
          previous_coarse_k = coarse_k;
        }
      else
        {
          // Contribution to the same layer, add to previous value:
          for (int j = 0; j < nj; j++)
            for (int i = 0; i < ni; i++)
              coarse(i, j, coarse_k) += coef * fine(i, j, fine_k);
        }
    }
  statistiques().end_count(coarsen_counter_);

}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Coarsen_Operator_K::interpolate_sub_shiftk_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, const int kshift) const
{
  static Stat_Counter_Id interpolate_counter_ = statistiques().new_counter(2, "multigrille : interpolate (K)");
  statistiques().begin_count(interpolate_counter_);

  int index_start, index_end, delta_index;
  if (kshift <= 0)
    {
      index_start = 0;
      index_end = src_dest_index_local_.dimension(0);
      delta_index = 1;
    }
  else
    {
      index_start = src_dest_index_local_.dimension(0) - 1;
      index_end = -1;
      delta_index = -1;
    }

  const int ni = coarse.ni();
  const int nj = coarse.nj();

  const ArrOfDouble& coef_array = coarsen_coefficients_local_;
  int previous_fine_k = -10;

  for (int index = index_start; index != index_end; index += delta_index)
    {
      const int fine_k = src_dest_index_local_(index, 0);
      const int coarse_k = src_dest_index_local_(index, 1);
      const _TYPE_ coef = (_TYPE_)coef_array[index];
      if (fine_k != previous_fine_k)
        {
          previous_fine_k = fine_k;
          // Start a new layer from the fine mesh, take data from shifted position:
          for (int J = 0; J < nj; J++)
            {
              for (int I = 0; I < ni; I++)
                {
                  fine.get_in_allocated_area(I,J,fine_k + kshift) = fine(I,J,fine_k) - coef * coarse(I,J,coarse_k);
                }
            }
        }
      else
        {
          // Continue on the same layer, data already shifted:
          for (int J = 0; J < nj; J++)
            {
              for (int I = 0; I < ni; I++)
                {
                  fine.get_in_allocated_area(I,J,fine_k + kshift) -= coef * coarse(I,J,coarse_k);
                }
            }
        }
    }
  fine.shift_k_origin(kshift);
  //flop_count += ni*nj*src_dest_index_.dimension(0) * 2;

  statistiques().end_count(interpolate_counter_);

}

#endif
