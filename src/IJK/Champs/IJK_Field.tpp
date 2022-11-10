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

#ifndef IJK_Field_TPP_H
#define IJK_Field_TPP_H

#include <stat_counters.h>
#include <Statistiques.h>
#include <simd_tools.h>

template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJKArray_with_ghost<_TYPE_,_TYPE_ARRAY_>::echange_espace_virtuel(int pe_min, int pe_max)
{
  statistiques().begin_count(echange_vect_counter_);
  // envoi a pe_max et reception de pe_min
  const int n = tab_.size_array() - ghost_ * 2;
  _TYPE_ *pdata = tab_.addr();
  ::envoyer_recevoir(pdata + n, ghost_ * sizeof(_TYPE_), pe_max, pdata, ghost_ * sizeof(_TYPE_), pe_min);
  // l'autre
  ::envoyer_recevoir(pdata + ghost_, ghost_ * sizeof(_TYPE_), pe_min, pdata + n + ghost_, ghost_ * sizeof(_TYPE_), pe_max);
  statistiques().end_count(echange_vect_counter_);
}


// inline void set_imin_nitot(int rank_dir, int nproc_dir, int pe_min, int pe_max, int n_local, int& i_min, int& n_tot)
// {
//   int canal = 53;

//   if (rank_dir == 0) i_min = 0;
//   else recevoir(i_min, pe_min, canal);

//   if (rank_dir < nproc_dir - 1) envoyer(i_min + n_local, pe_max, canal);

//   if (rank_dir == nproc_dir - 1) n_tot = i_min + n_local;
//   else recevoir(n_tot, pe_max, canal);

//   if (rank_dir > 0) envoyer(n_tot, pe_min, canal);
// }

// External storage: false=>normal behaviour, true=>the field will not allocate memory in data_, the user must
//  provide the storage via set_external_storage() method (used to map memory from elsewhere).
//  Example: field.allocate(... , true); int size=field.get_allocated_size(); Array T(size); field.set_external_storage(T);
// IMPORTANT: when "external_storage==false", an appropriate offset will be set in order to align the data on cache lines,
//  (this offset varies from field to field even if the field shape (ni,nj,nk,compo,ghost) is exactly the same
//  plus extra padding will be added to align all i=0,j,k on SIMD aligned blocks...
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::allocate(int Ni, int Nj, int Nk, int ghosts, int additional_k_layers, int nb_compos, bool external_storage)
{
  // WARNING !!!
  // for the sympy_to_trust system, mem alignment must be disabled otherwise we have serious trouble
  // with fields of same "kind" that have not the same storage pattern (offset, stride, etc...), and
  // we heavily rely on "same pattern" to optimize fetch operations in the fields...
  bool disable_memalign = true;
  if (ghosts > Ni || ghosts > Nj || ghosts > Nk)
    {
      Cerr << "Error in IJK_Field_local_template::allocate: ghostsize=" << ghosts << " is larger than the local mesh size " << Ni << " " << Nj << " " << Nk << finl;
      Process::exit();
    }
  ni_ = Ni;
  nj_ = Nj;
  nk_ = Nk;
  ghost_size_ = ghosts;
  nb_compo_ = nb_compos;
  j_stride_ = ni_ + 2 * ghosts;
  if (!external_storage || disable_memalign)
    // Align beginning of line on SIMD type...
    while (j_stride_ % Simd_template<_TYPE_>::size() != 0)
      j_stride_++;
  compo_stride_ = j_stride_ * (nj_ + 2 * ghosts);
  offset_ = ghost_size_ * (1 + j_stride_ + compo_stride_ * nb_compo_);
  k_layer_shift_ = 0;
  additional_k_layers_ = additional_k_layers;
  int sz = (nk_ + 2 * ghosts + additional_k_layers) * compo_stride_ * nb_compo_ + 1;
  allocated_size_ = sz;

  if (!external_storage || disable_memalign)
    {
      // Ensure that there is some padding data at beginning:
      offset_ += 8;

      // Align origin on cache line boundary
      const int CacheLineSizeBytes = 64;
      sz += (int)(CacheLineSizeBytes/sizeof(_TYPE_));
      // Add supplemental data at end for the last SIMD instruction of the loop that might go
      // beyond the end of the usefull data
      sz += (int)(CacheLineSizeBytes/sizeof(_TYPE_));

      allocated_size_ = sz;
      _TYPE_ *ptr = data_.addr() + offset_;
      char *cptr = (char *) ptr;
      long long iptr = (long long) cptr;
      long long decal = iptr & (CacheLineSizeBytes - 1);
      offset_ += (int)((CacheLineSizeBytes - decal) / sizeof(_TYPE_));
    }
  if (!external_storage)
    data_.resize_array(sz);

}

// Adds n * compo_stride_ * nb_compo_ to the offset (shifts the data by n layers in the k direction without moving memory)
// Used by the jacobi "in place" algorithm: the result replaces the source data but is shifted in k.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::shift_k_origin(int n)
{
  k_layer_shift_ += n;
  // Check that the data still fits into the allocated memory block
  assert(k_layer_shift_ >= 0 && k_layer_shift_ <= additional_k_layers_);

  offset_ += compo_stride_ * nb_compo_ * n;
}

// Creates a field with nk=1, that points to the data than src(...,...,k_layer) (uses ref_array() to create the reference).
// ghostsize is identical to source array.
template<>
inline void IJK_Field_local_template<float,ArrOfFloat>::ref_ij(IJK_Field_local_float& src, int k_lay)
{
  Cerr << "Error: must implement ArrOfFloat::ref_array() and reset()" << endl;
  Process::exit();
}

template<>
inline void IJK_Field_local_template<double, ArrOfDouble>::ref_ij(IJK_Field_local_double& src, int k_lay)
{
  data_.reset();
  ni_ = src.ni_;
  nj_ = src.nj_;
  nk_ = 1;
  ghost_size_ = src.ghost_size_;
  j_stride_ = src.j_stride_;
  nb_compo_ = 1;
  compo_stride_ = 0;
  k_layer_shift_ = 0;
  additional_k_layers_ = 0;

  const int i_first = src.linear_index(-ghost_size_, -ghost_size_, k_lay);
  const int i_last = src.linear_index(ni_ + ghost_size_ - 1, nj_ + ghost_size_ - 1, k_lay);
  offset_ = src.linear_index(0, 0, k_lay) - i_first;
  data_.ref_array(src.data_, i_first, i_last - i_first + 1);
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::exchange_data(int pe_send_, /* processor to send to */
                                                             int is, int js, int ks, /* ijk coordinates of first data to send */
                                                             int pe_recv_, /* processor to recv from */
                                                             int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                                                             int isz, int jsz, int ksz) /* size of block data to send/recv */
{
  if (pe_send_ == Process::me() && pe_recv_ == Process::me())
    {

      // Self (periodicity on same processor)
      _TYPE_ *dest = (this)->template data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++)
            dest[(this)->template linear_index(ir + i, jr + j, kr + k)] = (this)->template operator()(is + i, js + j, ks + k);
      return;
    }
  const int data_size = isz * jsz * ksz;
  const int type_size = sizeof(_TYPE_);
  _TYPE_ *send_buffer = 0;
  _TYPE_ *recv_buffer = 0;

  if (pe_send_ >= 0)
    {
      send_buffer = new _TYPE_[data_size];
      // Pack send data
      _TYPE_ *buf = send_buffer;
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
            *buf = (this)->template operator()(is + i, js + j, ks + k);
    }
  if (pe_recv_ >= 0)
    recv_buffer = new _TYPE_[data_size];

  ::envoyer_recevoir(send_buffer, data_size * type_size, pe_send_, recv_buffer, data_size * type_size, pe_recv_);
  if (pe_recv_ >= 0)
    {
      // Unpack recv data
      _TYPE_ *buf = recv_buffer;
      _TYPE_ *dest = (this)->template data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
            dest[(this)->template linear_index(ir + i, jr + j, kr + k)] = *buf;
    }

  delete[] send_buffer;
  delete[] recv_buffer;
}


/*! @brief Exchange data over "ghost" number of cells.
 *
 */
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::echange_espace_virtuel(int le_ghost)
{
  statistiques().begin_count(echange_vect_counter_);
  assert(le_ghost <= (this)->template ghost());
  // Exchange in i direction real cells,
  // then in j direction with ghost cells in i,
  // then in k direction, with ghost cells in i and j
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int pe_imin_ = splitting.get_neighbour_processor(0, 0);
  int pe_imax_ = splitting.get_neighbour_processor(1, 0);
  int pe_jmin_ = splitting.get_neighbour_processor(0, 1);
  int pe_jmax_ = splitting.get_neighbour_processor(1, 1);
  int pe_kmin_ = splitting.get_neighbour_processor(0, 2);
  int pe_kmax_ = splitting.get_neighbour_processor(1, 2);
  const int nii = (this)->template ni(), njj = (this)->template nj(), nkk = (this)->template nk();

  // send left layer of real cells to right layer of virtual cells
  exchange_data(pe_imin_, 0, 0, 0, pe_imax_, nii, 0, 0, le_ghost, njj, nkk); /* size of block data to send */

  // send right real cells to left virtual cells
  exchange_data(pe_imax_, nii - le_ghost, 0, 0, pe_imin_, -le_ghost, 0, 0, le_ghost, njj, nkk);

  exchange_data(pe_jmin_, -le_ghost, 0, 0, pe_jmax_, -le_ghost, njj, 0, nii + 2 * le_ghost, le_ghost, nkk);

  exchange_data(pe_jmax_, -le_ghost, njj - le_ghost, 0, pe_jmin_, -le_ghost, -le_ghost, 0, nii + 2 * le_ghost, le_ghost, nkk);

  exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);

  exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk - le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);

  statistiques().end_count(echange_vect_counter_);
}

// Initializes the field and allocates memory
// splitting: reference to the geometry of the IJK mesh and how the mesh is split on processors.
//   The field stores a reference to this IJK_Splitting object so do not delete it.
// loc: localisation of the field (elements, nodes, faces in direction i, j, or k)
//   The number of "real" items in each direction (returned by the ni(), nj() or nk() method) is obtained from
//   the IJK_Splitting object. Warning: on a processor that is in the middle of the mesh, the nodes on the
//   right of the rightmost real element are not real, they are virtual, values are copied from the neigbour
//   processor.
// ghost_size: number of ghost layers to allocate. When an exchange of ghost cells data is requested, a smaller
//   number of layers can be requested if all layers are not needed by the following operations
// additional_k_layers: allocates more layers of cells in the k direction for use with the
//   shift_k_origin() method (optimization trick used by the temporally blocked algorithms in the multigrid
//   solver
// nb_compo: number of components of the field. Warning: you cannot allocate the velocity field at faces
//   with nb_compo=3: numbers of faces in each direction differ.
//   Also, components are not grouped by node but stored by layers in k. nb_compo>1 is essentially used
//   in the multigrid solver to optimize memory accesses to the components of the matrix.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::allocate(const IJK_Splitting& splitting, IJK_Splitting::Localisation loc, int ghost_size, int additional_k_layers, int ncompo, bool external_storage)
{
  const int ni_local = splitting.get_nb_items_local(loc, 0);
  const int nj_local = splitting.get_nb_items_local(loc, 1);
  const int nk_local = splitting.get_nb_items_local(loc, 2);
  IJK_Field_local_template<_TYPE_, _TYPE_ARRAY_>::allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);
  splitting_ref_ = splitting;
  localisation_ = loc;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
double norme_ijk(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& residu)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  double somme = 0.;
  for (int k = 0; k < nk; k++)
    {
      double partial1 = 0.;
      for (int j = 0; j < nj; j++)
        {
          double partial2 = 0.;
          for (int i = 0; i < ni; i++)
            {
              double x = residu(i, j, k);
              partial2 += x * x;
            }
          partial1 += partial2;
        }
      somme += partial1;
    }
  somme = Process::mp_sum(somme);
  return sqrt(somme);
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ prod_scal_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& y)
{
  const int ni = x.ni();
  const int nj = x.nj();
  const int nk = x.nk();
  _TYPE_ somme = 0.;
  const _TYPE_ARRAY_& xd = x.data();
  const _TYPE_ARRAY_& yd = y.data();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              int index = x.linear_index(i,j,k);
              assert(index == y.linear_index(i,j,k));
              somme += xd[index] * yd[index];
            }
        }
    }
  somme = (_TYPE_)Process::mp_sum(somme); //!!!! WARNING: possible conversion to double to float!!!!!!!!
  return somme;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
double somme_ijk(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& residu)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  double somme = 0.;
  for (int k = 0; k < nk; k++)
    {
      double partial1 = 0.;
      for (int j = 0; j < nj; j++)
        {
          double partial2 = 0.;
          for (int i = 0; i < ni; i++)
            {
              double x = residu(i, j, k);
              partial2 += x;
            }
          partial1 += partial2;
        }
      somme += partial1;
    }
  somme = Process::mp_sum(somme);
  return somme;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ max_ijk(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& residu)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  _TYPE_ res = (_TYPE_)-1.e30;
  for (int k = 0; k < nk; k++)
    for (int j = 0; j < nj; j++)
      for (int i = 0; i < ni; i++)
        {
          _TYPE_ x = residu(i,j,k);
          res = std::max(x,res);
        }
  res = (_TYPE_)Process::mp_max(res);
  return res;
}

#endif /* IJK_Field_TPP_H */
