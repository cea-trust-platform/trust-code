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

#ifndef Grid_Level_Data_TPP_H
#define Grid_Level_Data_TPP_H

#ifdef DUMP_FACES_COEFFICIENTS
#include <IJK_lata_dump.h>
#endif

template<typename _TYPE_>
Grid_Level_Data_template<_TYPE_>::Grid_Level_Data_template()
{
  local_delta_xyz_.dimensionner(3);
  ghost_size_ = 0;
}


// Initialize the data structures (allocate memory and setup temporary storage structures)
template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::initialize(const IJK_Splitting& splitting, int ghost, int additional_k_layers)
{
  grid_splitting_ = splitting;
  perio_k_= splitting.get_grid_geometry().get_periodic_flag(DIRECTION_K);
  ghost_size_ = ghost;

  if (IJK_Splitting::rho_vap_ref_for_poisson_!=0. )
    {
      // shear periodicity
      //ijk_rho_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost);
      //std :: cout <<  " ICIIIII "<< IJK_Splitting::rho_vap_ref_for_poisson_<< IJK_Splitting::rho_liq_ref_for_poisson_ << std::endl;
      ijk_rho_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost, 0 ,1, false, 2, IJK_Splitting::rho_vap_ref_for_poisson_, IJK_Splitting::rho_liq_ref_for_poisson_);
    }
  else
    {
      ijk_rho_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost);
    }


  ijk_rho_.data() = 1.;
  // Allocate the array of coefficients at faces with size "elements".
  // Therefore, if the domain is not periodic, at the right end of the domain,
  //  the wall coefficient is stored in a "ghost" cell.
  // This trick allows to have the same stride in j and k for all arrays.
  ijk_faces_coefficients_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost, 0 /* add.k layers */, 4 /* components */);
  ijk_faces_coefficients_.data() = 1.;
  ijk_x_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost, additional_k_layers);
  ijk_x_.data() = 0.;
  ijk_rhs_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost);
  ijk_rhs_.data() = 0.;
  ijk_residue_.allocate(grid_splitting_, IJK_Splitting::ELEM, ghost);
  ijk_residue_.data() = 0.;

  const IJK_Grid_Geometry& geometry = grid_splitting_.get_grid_geometry();
  for (int dir = 0; dir < 3; dir++)
    grid_splitting_.get_local_mesh_delta(dir, ghost, local_delta_xyz_[dir]);
  for (int i = 0; i < 3; i++)
    uniform_[i] = geometry.is_uniform(i);

}

// Fill the coeffs_faces field (see ijk_faces_coefficients_) for the requested grid_level
// nb_ghost_layers is the number of valid ghost layers provided in ijk_rho_levels_ and as input to the jacobi
// smoother (at least 1).
template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_rho()
{
  int flag = uniform_[0] ? 1 : 0;
  flag += uniform_[1] ? 2 : 0;
  flag += uniform_[2] ? 4 : 0;
  switch(flag)
    {
    case 3: // variable in k
      compute_faces_coefficients_from_rho_cst_i_cst_j_var_k();
      break;
    case 7: // completely uniform
      compute_faces_coefficients_from_rho_cst_i_cst_j_cst_k();
      break;
    default:
      Cerr << "Grid_Level_Data::compute_faces_coefficients_from_rho() flag=" << flag << " not implemented" << finl;
      Process::exit();
    }
#ifdef DUMP_FACES_COEFFICIENTS
  {
    IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>> c[3];
    for (int dir = 0; dir < 3; dir++)
      {
        IJK_Splitting::Localisation loc = (dir==0)?IJK_Splitting::FACES_I:((dir==1)?IJK_Splitting::FACES_J:IJK_Splitting::FACES_K);
        IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>>& f = c[dir];
        f.allocate(grid_splitting_, loc, 1);
        for (int k = 0; k < f.nk(); k++)
          for (int j = 0; j < f.nj(); j++)
            for (int i = 0; i < f.ni(); i++)
              f(i,j,k)=ijk_faces_coefficients_(i,j,k,dir);
        f.echange_espace_virtuel(1); // to update periodic faces
      }
    IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>> e;
    e.allocate(grid_splitting_, IJK_Splitting::ELEM, 0);
    for (int k = 0; k < e.nk(); k++)
      for (int j = 0; j < e.nj(); j++)
        for (int i = 0; i < e.ni(); i++)
          e(i,j,k)=ijk_faces_coefficients_(i,j,k,3);
    static int step = 0;
    Nom prefix = Nom("Grid_coefficients_")+Nom(typeid(_TYPE_).name())
                 +Nom(grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_I))+Nom("_")
                 + Nom(grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_J))+Nom("_")
                 + Nom(grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_K));

    dumplata_vector(prefix+Nom("_faces.lata"), "val", c[0],c[1],c[2],step);
    dumplata_scalar(prefix+Nom("_elem.lata"), "val", e,step);
    step++;
  }
#endif
}

template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_inv_rho()
{
  int flag = uniform_[0] ? 1 : 0;
  flag += uniform_[1] ? 2 : 0;
  flag += uniform_[2] ? 4 : 0;
  switch(flag)
    {
    case 3: // variable in k
      compute_faces_coefficients_from_inv_rho_cst_i_cst_j_var_k();
      break;
    case 7: // completely uniform
      compute_faces_coefficients_from_inv_rho_cst_i_cst_j_cst_k();
      break;
    default:
      Cerr << "Grid_Level_Data::compute_faces_coefficients_from_inv_rho() flag=" << flag << " not implemented" << finl;
      Process::exit();
    }
#ifdef DUMP_FACES_COEFFICIENTS
  {
    IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>> c[3];
    for (int dir = 0; dir < 3; dir++)
      {
        IJK_Splitting::Localisation loc = (dir==0)?IJK_Splitting::FACES_I:((dir==1)?IJK_Splitting::FACES_J:IJK_Splitting::FACES_K);
        IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>>& f = c[dir];
        f.allocate(grid_splitting_, loc, 1);
        for (int k = 0; k < f.nk(); k++)
          for (int j = 0; j < f.nj(); j++)
            for (int i = 0; i < f.ni(); i++)
              f(i,j,k)=ijk_faces_coefficients_(i,j,k,dir);
        f.echange_espace_virtuel(1); // to update periodic faces
      }
    IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>> e;
    e.allocate(grid_splitting_, IJK_Splitting::ELEM, 0);
    for (int k = 0; k < e.nk(); k++)
      for (int j = 0; j < e.nj(); j++)
        for (int i = 0; i < e.ni(); i++)
          e(i,j,k)=ijk_faces_coefficients_(i,j,k,3);
    static int step = 0;
    Nom prefix = Nom("Grid_coefficients_")++Nom(typeid(_TYPE_).name())
                 +Nom(grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_I))+Nom("_")
                 + Nom(grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_J))+Nom("_")
                 + Nom(grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_K));

    dumplata_vector(prefix+Nom("_faces.lata"), "val", c[0],c[1],c[2],step);
    dumplata_scalar(prefix+Nom("_elem.lata"), "val", e,step);
    step++;
  }
#endif
}

template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_rho_cst_i_cst_j_cst_k()
{
  // mesh is uniform in i direction: define a constant value
  const _TYPE_ delta_i = (_TYPE_)local_delta_xyz_[0][0];
  const _TYPE_ i_delta_i = (_TYPE_)(1. / local_delta_xyz_[0][0]);
#define DELTA_i delta_i
#define invDELTA_i i_delta_i
  // mesh is uniform in j direction: define a constant value
  const _TYPE_ delta_j = (_TYPE_)local_delta_xyz_[1][0];
  const _TYPE_ i_delta_j = (_TYPE_)(1. / local_delta_xyz_[1][0]);
#define DELTA_j delta_j
#define invDELTA_j i_delta_j
  // mesh is uniform in k direction: define a constant value
  const _TYPE_ delta_k = (_TYPE_)local_delta_xyz_[2][0];
  const _TYPE_ i_delta_k = (_TYPE_)(1. / local_delta_xyz_[2][0]);
#define DELTA_k delta_k
#define invDELTA_k i_delta_k

  IJ_layout layout(ijk_rho_);

  const int nb_ghost_layers_rho = ijk_rho_.ghost();
  const int ghost = nb_ghost_layers_rho - 1;
  const int imin = - ghost;
  const int imax = ijk_rho_.ni() + ghost;
  const int jmin = - ghost;
  const int jmax = ijk_rho_.nj() + ghost;
  const int kmin = - ghost;
  const int kmax = ijk_rho_.nk() + ghost;

  for (int k = kmin; k < kmax + 1; k++)
    {

      const bool compute_k_direction_only = (k == kmax);
      if (!compute_k_direction_only)
        {

          // Compute layer of i faces
          const _TYPE_ *src_rho = ijk_rho_.k_layer(k);
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 0);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax+1; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(2. / (layout(src_rho, i-1, j) + layout(src_rho, i, j)));
                    const _TYPE_ f = invDELTA_i * DELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
          // Compute layer of j faces
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 1);
            for (int j = jmin; j < jmax+1; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = _TYPE_(2. / (layout(src_rho, i, j-1) + layout(src_rho, i, j)));
                    const _TYPE_ f = DELTA_i * invDELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
        }

      {
        _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 2);
        // Wall boundary condition:
        const int global_k_pos = k + grid_splitting_.get_offset_local(DIRECTION_K);
        // Number of elements in the z direction (equal to index in k direction of the faces on the wall)
        const int global_k_size = grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_K);
        // the face that are located on the walls in k direction have position 0 and global_k_size:
        if (!perio_k_ && (global_k_pos <= 0 || global_k_pos >= global_k_size))
          {
            // layer below or equal to first layer in the mesh
            for (int i = imin; i < imax; i++)
              for (int j = jmin; j < jmax; j++)
                layout(coeff, i, j) = 0.;
          }
        else
          {
            const _TYPE_ *src_rho_left = ijk_rho_.k_layer(k-1);
            const _TYPE_ *src_rho_right = ijk_rho_.k_layer(k);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(2. / (layout(src_rho_left, i, j) + layout(src_rho_right, i, j)));
                    const _TYPE_ f = DELTA_i * DELTA_j * invDELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
      }
      if (k > kmin)
        {
          const _TYPE_ *coeff_i = ijk_faces_coefficients_.k_layer(k-1, 0);
          const _TYPE_ *coeff_j = ijk_faces_coefficients_.k_layer(k-1, 1);
          const _TYPE_ *coeff_k = ijk_faces_coefficients_.k_layer(k-1, 2);
          const _TYPE_ *coeff_kplus = ijk_faces_coefficients_.k_layer(k, 2);
          _TYPE_ *coeff_sum = ijk_faces_coefficients_.k_layer(k-1, 3);

          // Sum of coefficients for 6 faces of each element
          for (int i = imin; i < imax; i++)
            for (int j = jmin; j < jmax; j++)
              {
                _TYPE_ s =
                  layout(coeff_i, i, j) + layout(coeff_i, i+1, j)
                  + layout(coeff_j, i, j) + layout(coeff_j, i, j+1)
                  + layout(coeff_k, i, j) + layout(coeff_kplus, i, j);
                // ghost cells beyond wall boundaries will have zero sum, crashes the division in jacobi,
                // so replace zero with dummy non zero value:
                if (s == 0.)
                  s = 1.;
                layout(coeff_sum, i, j) = s;
              }
        }
    }
#undef DELTA_i
#undef DELTA_j
#undef DELTA_k
#undef invDELTA_i
#undef invDELTA_j
#undef invDELTA_k
}

template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_inv_rho_cst_i_cst_j_cst_k()
{
  // mesh is uniform in i direction: define a constant value
  const _TYPE_ delta_i = (_TYPE_)local_delta_xyz_[0][0];
  const _TYPE_ i_delta_i = (_TYPE_)(1. / local_delta_xyz_[0][0]);
#define DELTA_i delta_i
#define invDELTA_i i_delta_i
  // mesh is uniform in j direction: define a constant value
  const _TYPE_ delta_j = (_TYPE_)local_delta_xyz_[1][0];
  const _TYPE_ i_delta_j = (_TYPE_)(1. / local_delta_xyz_[1][0]);
#define DELTA_j delta_j
#define invDELTA_j i_delta_j
  // mesh is uniform in k direction: define a constant value
  const _TYPE_ delta_k = (_TYPE_)local_delta_xyz_[2][0];
  const _TYPE_ i_delta_k = (_TYPE_)(1. / local_delta_xyz_[2][0]);
#define DELTA_k delta_k
#define invDELTA_k i_delta_k

  IJ_layout layout(ijk_rho_);

  const int nb_ghost_layers_rho = ijk_rho_.ghost();
  const int ghost = nb_ghost_layers_rho - 1;
  const int imin = - ghost;
  const int imax = ijk_rho_.ni() + ghost;
  const int jmin = - ghost;
  const int jmax = ijk_rho_.nj() + ghost;
  const int kmin = - ghost;
  const int kmax = ijk_rho_.nk() + ghost;

  for (int k = kmin; k < kmax + 1; k++)
    {

      const bool compute_k_direction_only = (k == kmax);
      if (!compute_k_direction_only)
        {

          // Compute layer of i faces
          const _TYPE_ *src_rho = ijk_rho_.k_layer(k);
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 0);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax+1; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(0.5 * (layout(src_rho, i-1, j) + layout(src_rho, i, j)));
                    const _TYPE_ f = invDELTA_i * DELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
          // Compute layer of j faces
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 1);
            for (int j = jmin; j < jmax+1; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(0.5 * (layout(src_rho, i, j-1) + layout(src_rho, i, j)));
                    const _TYPE_ f = DELTA_i * invDELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
        }

      {
        _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 2);
        // Wall boundary condition:
        const int global_k_pos = k + grid_splitting_.get_offset_local(DIRECTION_K);
        // Number of elements in the z direction (equal to index in k direction of the faces on the wall)
        const int global_k_size = grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_K);
        // the face that are located on the walls in k direction have position 0 and global_k_size:
        if (!perio_k_ && (global_k_pos <= 0 || global_k_pos >= global_k_size))
          {
            // layer below or equal to first layer in the mesh
            for (int i = imin; i < imax; i++)
              for (int j = jmin; j < jmax; j++)
                layout(coeff, i, j) = 0.;
          }
        else
          {
            const _TYPE_ *src_rho_left = ijk_rho_.k_layer(k-1);
            const _TYPE_ *src_rho_right = ijk_rho_.k_layer(k);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(0.5 * (layout(src_rho_left, i, j) + layout(src_rho_right, i, j)));
                    const _TYPE_ f = DELTA_i * DELTA_j * invDELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
      }
      if (k > kmin)
        {
          const _TYPE_ *coeff_i = ijk_faces_coefficients_.k_layer(k-1, 0);
          const _TYPE_ *coeff_j = ijk_faces_coefficients_.k_layer(k-1, 1);
          const _TYPE_ *coeff_k = ijk_faces_coefficients_.k_layer(k-1, 2);
          const _TYPE_ *coeff_kplus = ijk_faces_coefficients_.k_layer(k, 2);
          _TYPE_ *coeff_sum = ijk_faces_coefficients_.k_layer(k-1, 3);

          // Sum of coefficients for 6 faces of each element
          for (int i = imin; i < imax; i++)
            for (int j = jmin; j < jmax; j++)
              {
                _TYPE_ s =
                  layout(coeff_i, i, j) + layout(coeff_i, i+1, j)
                  + layout(coeff_j, i, j) + layout(coeff_j, i, j+1)
                  + layout(coeff_k, i, j) + layout(coeff_kplus, i, j);
                // ghost cells beyond wall boundaries will have zero sum, crashes the division in jacobi,
                // so replace zero with dummy non zero value:
                if (s == 0.)
                  s = 1.;
                layout(coeff_sum, i, j) = s;
              }
        }
    }
#undef DELTA_i
#undef DELTA_j
#undef DELTA_k
#undef invDELTA_i
#undef invDELTA_j
#undef invDELTA_k
}

template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_rho_cst_i_cst_j_var_k()
{
  // mesh is uniform in i direction: define a constant value
  const _TYPE_ delta_i = (_TYPE_)local_delta_xyz_[0][0];
  const _TYPE_ i_delta_i = (_TYPE_)(1. / local_delta_xyz_[0][0]);
#define DELTA_i delta_i
#define invDELTA_i i_delta_i
  // mesh is uniform in j direction: define a constant value
  const _TYPE_ delta_j = (_TYPE_)local_delta_xyz_[1][0];
  const _TYPE_ i_delta_j = (_TYPE_)(1. / local_delta_xyz_[1][0]);
#define DELTA_j delta_j
#define invDELTA_j i_delta_j
  // mesh has variable size in k direction: compute the inverse of
  // space between cell centers and create macro to point to appropriate
  // value in the arrays
  IJKArray_with_ghost<_TYPE_,TRUSTArray<_TYPE_>> delta_k_array;
  IJKArray_with_ghost<_TYPE_,TRUSTArray<_TYPE_>> i_delta_k_array;
  {
    const int n = ijk_rho_.nk();
    const int ghost = ijk_rho_.ghost();
    delta_k_array.resize(n, ghost);
    i_delta_k_array.resize(n, ghost);
    const int imin = -ghost + 1;
    const int imax = n + ghost;
    delta_k_array[-ghost] = (_TYPE_)local_delta_xyz_[2][-ghost];
    for (int i = imin; i < imax; i++)
      {
        delta_k_array[i] = (_TYPE_)local_delta_xyz_[2][i];
        _TYPE_ x = delta_k_array[i-1];
        _TYPE_ y = delta_k_array[i];
        i_delta_k_array[i] = (_TYPE_)(2. / (x+y));
      }
  }
#define DELTA_k delta_k_array[k]
#define invDELTA_k i_delta_k_array[k]

  IJ_layout layout(ijk_rho_);

  const int nb_ghost_layers_rho = ijk_rho_.ghost();
  const int ghost = nb_ghost_layers_rho - 1;
  const int imin = - ghost;
  const int imax = ijk_rho_.ni() + ghost;
  const int jmin = - ghost;
  const int jmax = ijk_rho_.nj() + ghost;
  const int kmin = - ghost;
  const int kmax = ijk_rho_.nk() + ghost;

  for (int k = kmin; k < kmax + 1; k++)
    {

      const bool compute_k_direction_only = (k == kmax);
      if (!compute_k_direction_only)
        {

          // Compute layer of i faces
          const _TYPE_ *src_rho = ijk_rho_.k_layer(k);
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 0);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax+1; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(2. / (layout(src_rho, i-1, j) + layout(src_rho, i, j)));
                    const _TYPE_ f = invDELTA_i * DELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
          // Compute layer of j faces
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 1);
            for (int j = jmin; j < jmax+1; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = _TYPE_(2. / (layout(src_rho, i, j-1) + layout(src_rho, i, j)));
                    const _TYPE_ f = DELTA_i * invDELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
        }

      {
        _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 2);
        // Wall boundary condition:
        const int global_k_pos = k + grid_splitting_.get_offset_local(DIRECTION_K);
        // Number of elements in the z direction (equal to index in k direction of the faces on the wall)
        const int global_k_size = grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_K);
        // the face that are located on the walls in k direction have position 0 and global_k_size:
        if (!perio_k_ && (global_k_pos <= 0 || global_k_pos >= global_k_size))
          {
            // layer below or equal to first layer in the mesh
            for (int i = imin; i < imax; i++)
              for (int j = jmin; j < jmax; j++)
                layout(coeff, i, j) = 0.;
          }
        else
          {
            const _TYPE_ *src_rho_left = ijk_rho_.k_layer(k-1);
            const _TYPE_ *src_rho_right = ijk_rho_.k_layer(k);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(2. / (layout(src_rho_left, i, j) + layout(src_rho_right, i, j)));
                    const _TYPE_ f = DELTA_i * DELTA_j * invDELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
      }
      if (k > kmin)
        {
          const _TYPE_ *coeff_i = ijk_faces_coefficients_.k_layer(k-1, 0);
          const _TYPE_ *coeff_j = ijk_faces_coefficients_.k_layer(k-1, 1);
          const _TYPE_ *coeff_k = ijk_faces_coefficients_.k_layer(k-1, 2);
          const _TYPE_ *coeff_kplus = ijk_faces_coefficients_.k_layer(k, 2);
          _TYPE_ *coeff_sum = ijk_faces_coefficients_.k_layer(k-1, 3);

          // Sum of coefficients for 6 faces of each element
          for (int i = imin; i < imax; i++)
            for (int j = jmin; j < jmax; j++)
              {
                _TYPE_ s =
                  layout(coeff_i, i, j) + layout(coeff_i, i+1, j)
                  + layout(coeff_j, i, j) + layout(coeff_j, i, j+1)
                  + layout(coeff_k, i, j) + layout(coeff_kplus, i, j);
                // ghost cells beyond wall boundaries will have zero sum, crashes the division in jacobi,
                // so replace zero with dummy non zero value:
                if (s == 0.)
                  s = 1.;
                layout(coeff_sum, i, j) = s;
              }
        }
    }
#undef DELTA_i
#undef DELTA_j
#undef DELTA_k
#undef invDELTA_i
#undef invDELTA_j
#undef invDELTA_k
}

template<typename _TYPE_>
void Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_inv_rho_cst_i_cst_j_var_k()
{
  // mesh is uniform in i direction: define a constant value
  const _TYPE_ delta_i = (_TYPE_)local_delta_xyz_[0][0];
  const _TYPE_ i_delta_i = (_TYPE_)(1. / local_delta_xyz_[0][0]);
#define DELTA_i delta_i
#define invDELTA_i i_delta_i
  // mesh is uniform in j direction: define a constant value
  const _TYPE_ delta_j = (_TYPE_)local_delta_xyz_[1][0];
  const _TYPE_ i_delta_j = (_TYPE_)(1. / local_delta_xyz_[1][0]);
#define DELTA_j delta_j
#define invDELTA_j i_delta_j
  // mesh has variable size in k direction: compute the inverse of
  // space between cell centers and create macro to point to appropriate
  // value in the arrays
  IJKArray_with_ghost<_TYPE_,TRUSTArray<_TYPE_>> delta_k_array;
  IJKArray_with_ghost<_TYPE_,TRUSTArray<_TYPE_>> i_delta_k_array;
  {
    const int n = ijk_rho_.nk();
    const int ghost = ijk_rho_.ghost();
    delta_k_array.resize(n, ghost);
    i_delta_k_array.resize(n, ghost);
    const int imin = -ghost + 1;
    const int imax = n + ghost;
    delta_k_array[-ghost] = (_TYPE_)local_delta_xyz_[2][-ghost];
    for (int i = imin; i < imax; i++)
      {
        delta_k_array[i] = (_TYPE_)local_delta_xyz_[2][i];
        _TYPE_ x = delta_k_array[i-1];
        _TYPE_ y = delta_k_array[i];
        i_delta_k_array[i] = (_TYPE_)(2. / (x+y));
      }
  }
#define DELTA_k delta_k_array[k]
#define invDELTA_k i_delta_k_array[k]

  IJ_layout layout(ijk_rho_);

  const int nb_ghost_layers_rho = ijk_rho_.ghost();
  const int ghost = nb_ghost_layers_rho - 1;
  const int imin = - ghost;
  const int imax = ijk_rho_.ni() + ghost;
  const int jmin = - ghost;
  const int jmax = ijk_rho_.nj() + ghost;
  const int kmin = - ghost;
  const int kmax = ijk_rho_.nk() + ghost;

  for (int k = kmin; k < kmax + 1; k++)
    {

      const bool compute_k_direction_only = (k == kmax);
      if (!compute_k_direction_only)
        {

          // Compute layer of i faces
          const _TYPE_ *src_rho = ijk_rho_.k_layer(k);
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 0);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax+1; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(0.5 * (layout(src_rho, i-1, j) + layout(src_rho, i, j)));
                    const _TYPE_ f = invDELTA_i * DELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
          // Compute layer of j faces
          {
            _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 1);
            for (int j = jmin; j < jmax+1; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(0.5 * (layout(src_rho, i, j-1) + layout(src_rho, i, j)));
                    const _TYPE_ f = DELTA_i * invDELTA_j * DELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
        }

      {
        _TYPE_ *coeff = ijk_faces_coefficients_.k_layer(k, 2);
        // Wall boundary condition:
        const int global_k_pos = k + grid_splitting_.get_offset_local(DIRECTION_K);
        // Number of elements in the z direction (equal to index in k direction of the faces on the wall)
        const int global_k_size = grid_splitting_.get_grid_geometry().get_nb_elem_tot(DIRECTION_K);
        // the face that are located on the walls in k direction have position 0 and global_k_size:
        if (!perio_k_ && (global_k_pos <= 0 || global_k_pos >= global_k_size))
          {
            // layer below or equal to first layer in the mesh
            for (int i = imin; i < imax; i++)
              for (int j = jmin; j < jmax; j++)
                layout(coeff, i, j) = 0.;
          }
        else
          {
            const _TYPE_ *src_rho_left = ijk_rho_.k_layer(k-1);
            const _TYPE_ *src_rho_right = ijk_rho_.k_layer(k);
            for (int j = jmin; j < jmax; j++)
              {
                for (int i = imin; i < imax; i++)
                  {
                    const _TYPE_ rho = (_TYPE_)(0.5 * (layout(src_rho_left, i, j) + layout(src_rho_right, i, j)));
                    const _TYPE_ f = DELTA_i * DELTA_j * invDELTA_k;
                    layout(coeff, i, j) = rho * f;
                  }
              }
          }
      }
      if (k > kmin)
        {
          const _TYPE_ *coeff_i = ijk_faces_coefficients_.k_layer(k-1, 0);
          const _TYPE_ *coeff_j = ijk_faces_coefficients_.k_layer(k-1, 1);
          const _TYPE_ *coeff_k = ijk_faces_coefficients_.k_layer(k-1, 2);
          const _TYPE_ *coeff_kplus = ijk_faces_coefficients_.k_layer(k, 2);
          _TYPE_ *coeff_sum = ijk_faces_coefficients_.k_layer(k-1, 3);

          // Sum of coefficients for 6 faces of each element
          for (int i = imin; i < imax; i++)
            for (int j = jmin; j < jmax; j++)
              {
                _TYPE_ s =
                  layout(coeff_i, i, j) + layout(coeff_i, i+1, j)
                  + layout(coeff_j, i, j) + layout(coeff_j, i, j+1)
                  + layout(coeff_k, i, j) + layout(coeff_kplus, i, j);
                // ghost cells beyond wall boundaries will have zero sum, crashes the division in jacobi,
                // so replace zero with dummy non zero value:
                if (s == 0.)
                  s = 1.;
                layout(coeff_sum, i, j) = s;
              }
        }
    }
#undef DELTA_i
#undef DELTA_j
#undef DELTA_k
#undef invDELTA_i
#undef invDELTA_j
#undef invDELTA_k
}


// Copy double precision coefficients to single precision:
template<class _TYPE_> template<class MY_TYPE> std::enable_if_t<std::is_same<MY_TYPE, float>::value, void>
Grid_Level_Data_template<_TYPE_>::compute_faces_coefficients_from_double_coeffs(const Grid_Level_Data_double& dbl_coeffs)
{
  // Warn: strides might differ due to padding for different simd vector sizes.
  // Therefore, linear data indices might not match, must explicitely loop on i, j, k indices
  const IJK_Field_double& src = dbl_coeffs.get_faces_coefficients();

  const int nb_ghost_layers_rho = ijk_rho_.ghost();
  const int ghost = nb_ghost_layers_rho;
  const int ni = ijk_rho_.ni();
  const int imin = - ghost + 1;
  const int jmin = - ghost + 1;
  const int nj = ijk_rho_.nj();
  const int kmin = - ghost + 1;
  const int kmax = ijk_rho_.nk() + ghost;
  for (int k = kmin; k < kmax; k++)
    for (int compo = 0; compo < 4; compo++)
      {
        int imax = ni + ghost - 1;
        int jmax = nj + ghost - 1;
        if (k == (kmax-1) && compo != 2 )
          {
            continue; // this data is unused
          }
        if (compo == 0)
          imax++;
        else if (compo == 1)
          jmax++;

        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            ijk_faces_coefficients_(i, j, k, compo) = (float)src(i,j,k,compo);
      }
}


#endif
