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
#ifndef Coarsen_Operator_Uniform_TPP_H
#define Coarsen_Operator_Uniform_TPP_H

#include <Domaine_IJK.h>
#include <stat_counters.h>

template<typename _TYPE_>
void Coarsen_Operator_Uniform::initialize_grid_data_(const Grid_Level_Data_template<_TYPE_>& fine,
                                                     Grid_Level_Data_template<_TYPE_>& coarse,
                                                     int additional_k_layers)
{
  const Domaine_IJK& src_grid_geom = fine.get_domaine();
  VECT(ArrOfDouble) coarse_delta(3);
  ArrOfInt nlocal(3);

  for (int dir = 0; dir < 3; dir++)
    {
      const int coarsen_factor = coarsen_factors_[dir];
      const ArrOfDouble& fine_delta = src_grid_geom.get_delta(dir);

      const int src_n = fine_delta.size_array();
      if (src_n % coarsen_factor != 0)
        {
          Cerr << "Coarsen_Operator_Uniform::initialize_grid_data: source grid has " << src_n
               << " elements in direction " << dir
               << " and cannot be refined by factor " << coarsen_factor << finl;
          Process::exit();
        }
      nlocal[dir] = fine.get_rho().nb_elem_local(dir);
      if (nlocal[dir] % coarsen_factor != 0)
        {
          Cerr << "Coarsen_Operator_Uniform::initialize_grid_data: local source grid processor has " << nlocal[dir]
               << " elements in direction " << dir
               << " and cannot be refined by factor " << coarsen_factor << finl;
          Process::exit();
        }
      nlocal[dir] /= coarsen_factor;
      const int n = src_n / coarsen_factor;
      ArrOfDouble& delta = coarse_delta[dir];
      delta.resize_array(n);

      int src_index = 0;
      delta.resize_array(n);
      for (int dest_index = 0; dest_index < n; dest_index++)
        {
          double d = 0;
          for (int j = 0; j < coarsen_factor; j++)
            {
              d += fine_delta[src_index];
              src_index++;
            }
          delta[dest_index] = d;
        }
    }

  Domaine_IJK grid_geom;
  grid_geom.initialize_origin_deltas(src_grid_geom.get_origin(0),
                                     src_grid_geom.get_origin(1),
                                     src_grid_geom.get_origin(2),
                                     coarse_delta[0],
                                     coarse_delta[1],
                                     coarse_delta[2],
                                     src_grid_geom.get_periodic_flag(0),
                                     src_grid_geom.get_periodic_flag(1),
                                     src_grid_geom.get_periodic_flag(2));

  Domaine_IJK coarse_splitting;
  // Same processor mapping as fine mesh
  IntTab processor_mapping;
  fine.get_domaine().get_processor_mapping(processor_mapping);
  // Splitting is identical, divide ncells by the coarsening factor
  VECT(ArrOfInt) slice_sizes(3);
  for (int dir = 0; dir < 3; dir++)
    {
      fine.get_domaine().get_slice_size(dir, Domaine_IJK::ELEM, slice_sizes[dir]);
      const int n = slice_sizes[dir].size_array();
      for (int i = 0; i < n; i++)
        slice_sizes[dir][i] /= coarsen_factors_[dir];
    }
  coarse_splitting.initialize_mapping(grid_geom, slice_sizes[0], slice_sizes[1], slice_sizes[2],
                                      processor_mapping);
  const int ghost_domaine_size = fine.get_ghost_size();
  coarse.initialize(coarse_splitting, ghost_domaine_size, additional_k_layers);
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Coarsen_Operator_Uniform::coarsen_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine,
                                        IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse,
                                        int compute_weighted_average) const
{
  static Stat_Counter_Id coarsen_counter_ = statistiques().new_counter(2, "multigrille : uniform coarsen ");
  statistiques().begin_count(coarsen_counter_);

  const int ni2 = coarse.ni();
  const int nj2 = coarse.nj();
  const int nk2 = coarse.nk();

  _TYPE_ coef = 1;
  if (compute_weighted_average)
    coef = (_TYPE_)(1. / (coarsen_factors_[0] * coarsen_factors_[1] * coarsen_factors_[2]));

  const int Kstart = 0;
  const int Kend = nk2;
  const int deltaK = 1;
  for (int K = Kstart; K != Kend; K += deltaK)
    {
      const int k = K*coarsen_factors_[2];
      for (int J = 0; J < nj2; J++)
        {
          const int j = J*coarsen_factors_[1];
          for (int I = 0; I < ni2; I++)
            {
              const int i = I*coarsen_factors_[0];
              _TYPE_ sum = 0.;
              for (int ii = 0; ii < coarsen_factors_[0]; ii++)
                for (int jj = 0; jj < coarsen_factors_[1]; jj++)
                  for (int kk = 0; kk < coarsen_factors_[2]; kk++)
                    sum += fine(i + ii, j + jj, k + kk);
              coarse(I,J,K) = sum * coef;
            }
        }
    }

  statistiques().end_count(coarsen_counter_);
  return;
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Coarsen_Operator_Uniform::interpolate_sub_shiftk_(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& coarse,
                                                       IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& fine,
                                                       const int kshift) const
{
  if (kshift == 1)
    {
      Cerr << "error Coarsen_Operator_Uniform: kshift=1 will not work" << finl;
      Process::exit();
    }

  static Stat_Counter_Id interpolate_counter_ = statistiques().new_counter(2, "multigrille : interpolate (uniform)");
  statistiques().begin_count(interpolate_counter_);

  const int ni2 = coarse.ni();
  const int nj2 = coarse.nj();
  const int nk2 = coarse.nk();

  const int Kstart = kshift <= 0 ? 0 : nk2 - 1;
  const int Kend = kshift <= 0 ? nk2 : -1;
  const int deltaK = kshift <= 0 ? 1 : -1;

  for (int K = Kstart; K != Kend; K += deltaK)
    {
      const int k = K*coarsen_factors_[2];
      for (int J = 0; J < nj2; ++J)
        {
          const int j = J*coarsen_factors_[1];
          for (int I = 0; I < ni2; ++I)
            {
              const int i = I*coarsen_factors_[0];
              const _TYPE_ val = coarse(I, J, K);
              for (int ii = 0; ii < coarsen_factors_[0]; ++ii)
                for (int jj = 0; jj < coarsen_factors_[1]; ++jj)
                  for (int kk = 0; kk < coarsen_factors_[2]; ++kk)
                    fine.get_in_allocated_area(i + ii, j + jj, k + kk + kshift) = fine(i + ii, j + jj, k + kk) - val;
            }
        }
    }

  fine.shift_k_origin(kshift);
  statistiques().end_count(interpolate_counter_);
  return;
}

#endif
