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

#ifndef IJK_Field_internal_included
#define IJK_Field_internal_included

#include <Vect.h>
#include <Static_Int_Lists.h>
#include <TRUSTLists.h>
#include <TRUSTVect.h>
#include <TRUSTArray.h>
#include <IJK_Splitting.h>

#define DIRECTION_I 0
#define DIRECTION_J 1
#define DIRECTION_K 2

// .Description
// Useless class but for some reason, verifie_pere only seems to work if it's here ??
class IJK_Field_internal : public Objet_U
{
  Declare_instanciable(IJK_Field_internal);
};

void envoyer_recevoir(const void * send_buf, int send_buf_size, int send_proc,
                      void * recv_buf, int recv_buf_size, int recv_proc);

// .Description. This is an array with [] operator. Allocate array with resize(n, ghost). tab[i] is valid for "-ghost <= i < n + ghost"
template<typename _TYPE_, typename _TYPE_ARRAY_>
class IJKArray_with_ghost : public Objet_U
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    IJKArray_with_ghost *xxx = new IJKArray_with_ghost(*this);
    if (!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJKArray_with_ghost() : ghost_(0) { }
  void resize(int n, int new_ghost)
  {
    tab_.resize_array(n + 2 * new_ghost);
    ghost_ = new_ghost;
  }
  _TYPE_ operator[](int i) const { return tab_[i + ghost_]; }
  _TYPE_& operator[](int i) { return tab_[i + ghost_]; }
  const _TYPE_ARRAY_& data() const { return tab_; }
  int size() const { return tab_.size_array() - 2 * ghost_; }
  int ghost() const { return ghost_; }
  void echange_espace_virtuel(int pe_min, int pe_max);

protected:
  _TYPE_ARRAY_ tab_;
  int ghost_;
};

using ArrOfFloat_with_ghost = IJKArray_with_ghost<float,ArrOfFloat>;
Declare_vect(ArrOfFloat_with_ghost);
using ArrOfDouble_with_ghost = IJKArray_with_ghost<double,ArrOfDouble>;
Declare_vect(ArrOfDouble_with_ghost);


// .Description : This class describes a scalar field in an ijk box without any parallel information.
// The scalar field can be accessed by
//  - field(i,j,k) with "-ghost() <= i < ni() + ghost()", same for j and k
//  - field.data()[linear_index(i,j,k)]
template<typename _TYPE_, typename _TYPE_ARRAY_>
class IJK_Field_local_template : public Objet_U
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    IJK_Field_local_template *xxx = new IJK_Field_local_template(*this);
    if (!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJK_Field_local_template() : ni_(0), nj_(0), nk_(0), ghost_size_(0), nb_compo_(1), j_stride_(0), compo_stride_(0), offset_(0), k_layer_shift_(0), additional_k_layers_(0), allocated_size_(0) { }

  void allocate(int ni, int nj, int nk, int ghosts, int additional_k_layers = 0, int nb_compo = 1, bool external_storage = false);
  void shift_k_origin(int n);
  void ref_ij(IJK_Field_local_template& src, int k_layer);

  int linear_index(int i, int j, int k) const
  {
    assert(nb_compo_ == 1); // otherwise, must specify component
    assert(i >= -ghost_size_ && i < ni_ + ghost_size_ && j >= -ghost_size_ && j < nj_ + ghost_size_ && k >= -ghost_size_ && k < nk_ + ghost_size_);
    return offset_ + k * compo_stride_ + j * j_stride_ + i;
  }

  int linear_index(int i, int j, int k, int compo) const
  {
    assert(i >= -ghost_size_ && i < ni_ + ghost_size_ && j >= -ghost_size_ && j < nj_ + ghost_size_ && k >= -ghost_size_ && k < nk_ + ghost_size_ && compo >= 0 && compo < nb_compo_);
    return offset_ + (k * nb_compo_ + compo) * compo_stride_ + j * j_stride_ + i;
  }

  // Operator() checks if the requested i,j,k index lies within the valid range [-ghost,n+ghost-1]
  _TYPE_& operator()(int i, int j, int k)
  {
    int idx = linear_index(i, j, k);
    return data_[idx];
  }

  const _TYPE_& operator()(int i, int j, int k) const
  {
    int idx = linear_index(i, j, k);
    return data_[idx];
  }

  // Operator() checks if the requested i,j,k index lies within the valid range [-ghost,n+ghost-1]
  _TYPE_& operator()(int i, int j, int k, int compo)
  {
    int idx = linear_index(i, j, k, compo);
    return data_[idx];
  }

  const _TYPE_& operator()(int i, int j, int k, int compo) const
  {
    int idx = linear_index(i, j, k, compo);
    return data_[idx];
  }

  int linear_index_relaxed_test(int i, int j, int k) const
  {
    assert(nb_compo_ == 1); // otherwise, must specify component
    assert(k >= -ghost_size_-k_layer_shift_ && k < nk_ + ghost_size_ + additional_k_layers_ - k_layer_shift_);
    int x = offset_ + k * compo_stride_ + j * j_stride_ + i;
    assert(x >= 0 && x < data_.size_array());
    return x;
  }

  int linear_index_relaxed_test(int i, int j, int k, int compo) const
  {
    assert(compo >= 0 && compo < nb_compo_);
    assert(k >= -ghost_size_-k_layer_shift_ && k < nk_ + ghost_size_ + additional_k_layers_ - k_layer_shift_);
    int x = offset_ + (k * nb_compo_ + compo) * compo_stride_ + j * j_stride_ + i;
    assert(x >= 0 && x < data_.size_array());
    return x;
  }

  // This method allows to access padding cells outside of the valid data range but inside the allocated data block.
  _TYPE_& get_in_allocated_area(int i, int j, int k)
  {
    int idx = linear_index_relaxed_test(i, j, k);
    return data_[idx];
  }

  const _TYPE_& get_in_allocated_area(int i, int j, int k) const
  {
    int idx = linear_index_relaxed_test(i, j, k);
    return data_[idx];
  }

  // This method allows to access padding cells outside of the valid data range but inside the allocated data block.
  _TYPE_& get_in_allocated_area(int i, int j, int k, int compo)
  {
    int idx = linear_index_relaxed_test(i, j, k, compo);
    return data_[idx];
  }

  const _TYPE_& get_in_allocated_area(int i, int j, int k, int compo) const
  {
    int idx = linear_index_relaxed_test(i, j, k, compo);
    return data_[idx];
  }

  _TYPE_* k_layer(int k)
  {
    assert(nb_compo_ == 1);
    assert(k >= -ghost_size_ && k < nk_ + ghost_size_);
    return data_.addr() + offset_ + k * compo_stride_;
  }
  const _TYPE_* k_layer(int k) const
  {
    assert(nb_compo_ == 1);
    assert(k >= -ghost_size_ && k < nk_ + ghost_size_);
    return data_.addr() + offset_ + k * compo_stride_;
  }
  _TYPE_* k_layer(int k, int compo)
  {
    assert(compo >= 0 && compo < nb_compo_);
    assert(k >= -ghost_size_ && k < nk_ + ghost_size_);
    return data_.addr() + offset_ + (k * nb_compo_ + compo) * compo_stride_;
  }
  const _TYPE_* k_layer(int k, int compo) const
  {
    assert(compo >= 0 && compo < nb_compo_);
    assert(k >= -ghost_size_ && k < nk_ + ghost_size_);
    return data_.addr() + offset_ + (k * nb_compo_ + compo) * compo_stride_;
  }

  int ni() const { return ni_; }
  int nj() const { return nj_; }
  int nk() const { return nk_; }
  int nb_elem_local(int dir) const { return (dir==0)?ni_:((dir==1)?nj_:nk_); }
  int nb_compo() const { return nb_compo_; }
  int j_stride() const { return j_stride_; }
  int compo_stride() const { return compo_stride_; }
  int k_stride() const { return compo_stride_ * nb_compo_; }
  int ghost() const { return ghost_size_; }
  int k_shift() const { return k_layer_shift_; }
  int k_shift_max() const { return additional_k_layers_; }
  int get_allocated_size() const { return allocated_size_; }
  _TYPE_ARRAY_& data() { return data_; }
  const _TYPE_ARRAY_& data() const { return data_; }
protected:
  // local size on this proc: (real items) : ni_ nj_ nk_ do not include the ghost size
  int ni_, nj_, nk_, ghost_size_, nb_compo_;
  int j_stride_; // how to jump to next j
  int compo_stride_; // how to jump to next component, k_stride is compo_stride_ * nb_compo_
  int offset_; // offset to first non ghost cell
  int k_layer_shift_; // current shift value of the origin in the k direction
  int additional_k_layers_, allocated_size_;
  _TYPE_ARRAY_ data_;
};

using IJK_Field_local_float = IJK_Field_local_template<float,ArrOfFloat>;
using IJK_Field_local_double = IJK_Field_local_template<double,ArrOfDouble>;
using IJK_Field_local_int = IJK_Field_local_template<int,ArrOfInt>;

// .Description : This class is an IJK_Field_local with parallel informations.
// Each processor has a sub_box of the global box, and echange_espace_virtuel(n) exchanges n layers of ghost cells,
// echange_espace_virtuel handles periodicity by copying the first layer into the ghost layer on the opposite side.
template<typename _TYPE_, typename _TYPE_ARRAY_>
class IJK_Field_template : public IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    IJK_Field_template *xxx = new IJK_Field_template(*this);
    if (!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJK_Field_template() : IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>() { }
  void allocate(const IJK_Splitting&, IJK_Splitting::Localisation, int ghost_size, int additional_k_layers = 0, int nb_compo = 1, bool external_storage = false);

  const IJK_Splitting& get_splitting() const { return splitting_ref_.valeur(); }
  IJK_Splitting::Localisation get_localisation() const { return localisation_; }
  void echange_espace_virtuel(int ghost);

protected:
  REF(IJK_Splitting) splitting_ref_;
  IJK_Splitting::Localisation localisation_;

  void exchange_data(int pe_imin_, /* processor to send to */
                     int is, int js, int ks, /* ijk coordinates of first data to send */
                     int pe_imax_, /* processor to recv from */
                     int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                     int isz, int jsz, int ksz); /* size of block data to send/recv */
};

using IJK_Field_float = IJK_Field_template<float,ArrOfFloat>;
Declare_vect(IJK_Field_float);
using IJK_Field_double = IJK_Field_template<double,ArrOfDouble>;
Declare_vect(IJK_Field_double);
using IJK_Field_int = IJK_Field_template<int,ArrOfInt>;

template<typename _TYPE_, typename _TYPE_ARRAY_>
double norme_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x);

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ prod_scal_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& y);

template<typename _TYPE_, typename _TYPE_ARRAY_>
double somme_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu);

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ max_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu);

typedef IJK_Field_local_double IJK_Field_local;
typedef IJK_Field_double IJK_Field;
typedef VECT(IJK_Field_double) VECT(IJK_Field);

// .Description
//  This class describes an IJ plane of an IJK_Field_local.
//  It just encapsulates the offset computation:
//    j * j_stride_ + i
//  with bound checking in debug...
//  This class is usefull to safely optimize the code !
class IJ_layout
{
public:
  int ni() const { return ni_; }
  int nj() const { return nj_; }
  int ghost() const { return ghost_; }
  int j_stride() const { return j_stride_; }

  IJ_layout(const IJK_Field_local_double& f) :
    ni_(f.ni()),
    nj_(f.nj()),
    ghost_(f.ghost()),
    j_stride_(f.j_stride()) {}

  // origin points to x(0,0,k) for an IJK_Field
  double& operator()(double * origin, int i, int j) const
  {
    assert(i >= -ghost_ && i < ni_ + ghost_);
    assert(j >= -ghost_ && j < nj_ + ghost_);
    return origin[j * j_stride_ + i];
  }

  // origin points to x(0,0,k) for an IJK_Field
  const double& operator()(const double * origin, int i, int j) const
  {
    assert(i >= -ghost_ && i < ni_ + ghost_);
    assert(j >= -ghost_ && j < nj_ + ghost_);
    return origin[j * j_stride_ + i];
  }
  // linear index of cell (i=0,j=0) is zero
  // origin points to x(0,0,k) for an IJK_Field
  double& linear(double * origin, int linear_index) const
  {
    assert(linear_index >= -ghost_*(j_stride_+1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    return origin[linear_index];
  }

  const double& linear(const double * origin, int linear_index) const
  {
    assert(linear_index >= -ghost_*(j_stride_+1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    return origin[linear_index];
  }
  IJ_layout(const IJK_Field_local_float& f) :
    ni_(f.ni()),
    nj_(f.nj()),
    ghost_(f.ghost()),
    j_stride_(f.j_stride()) {}

  // origin points to x(0,0,k) for an IJK_Field
  float& operator()(float * origin, int i, int j) const
  {
    assert(i >= -ghost_ && i < ni_ + ghost_);
    assert(j >= -ghost_ && j < nj_ + ghost_);
    return origin[j * j_stride_ + i];
  }

  // origin points to x(0,0,k) for an IJK_Field
  const float& operator()(const float * origin, int i, int j) const
  {
    assert(i >= -ghost_ && i < ni_ + ghost_);
    assert(j >= -ghost_ && j < nj_ + ghost_);
    return origin[j * j_stride_ + i];
  }
  // linear index of cell (i=0,j=0) is zero
  // origin points to x(0,0,k) for an IJK_Field
  float& linear(float * origin, int linear_index) const
  {
    assert(linear_index >= -ghost_*(j_stride_+1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    return origin[linear_index];
  }

  const float& linear(const float * origin, int linear_index) const
  {
    assert(linear_index >= -ghost_*(j_stride_+1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    return origin[linear_index];
  }
  void linear_to_ij(int linear_index, int& i, int& j) const
  {
    assert(linear_index >= -ghost_*(j_stride_+1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    int l = linear_index + ghost_*(j_stride_+1);
    j = (l / j_stride_) - ghost_;
    i = (l % j_stride_) - ghost_;
  }

  // ghost_ is the number of ghost cells allocated in memory in I and J directions
  int ni_, nj_, ghost_, j_stride_;
};

#include <IJK_Field.tpp>

#endif
