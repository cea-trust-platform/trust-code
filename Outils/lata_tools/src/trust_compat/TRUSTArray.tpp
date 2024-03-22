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

#ifndef TRUSTArray_TPP_included
#define TRUSTArray_TPP_included

#include <Array_base.h>
#include <assert.h>
#include <arch.h>
#include <Objet_U.h>
#include <VTRUSTdata.h>

#if ! defined(DMAXFLOAT)
#define DMAXFLOAT 1e40
#endif

template <typename _TYPE_>
class TRUSTArray : public Array_base
{
public:
  using value_type = _TYPE_;

  virtual ~TRUSTArray()
  {
    detach_array();
    size_array_ = -1; // Paranoia: si size_array_==-1, c'est un zombie
  }

  TRUSTArray() : mem_(0), span_(0), size_array_(0), memory_size_(0), smart_resize_(0), storage_type_(STANDARD) { }

  TRUSTArray(entier n) :
      mem_(new VTRUSTdata<_TYPE_>(n, STANDARD)), span_(mem_->get_data()), size_array_(n), memory_size_(n), smart_resize_(0), storage_type_(STANDARD)
  {
    if (n > 0)
      fill_default_value(0, n);
  }

  TRUSTArray(entier n, _TYPE_ x) :
      mem_(new VTRUSTdata<_TYPE_>(n, STANDARD)), span_(mem_->get_data()), size_array_(n), memory_size_(n), smart_resize_(0), storage_type_(STANDARD)
  {
    *this = x;
  }

  TRUSTArray(const TRUSTArray<_TYPE_>& A)
  {
    const entier size = A.size_array();
    if (size > 0)
      {
        // Creation d'un tableau "normal"
        storage_type_ = STANDARD;
        mem_ = new VTRUSTdata<_TYPE_>(size, STANDARD);
        span_ = mem_->get_data();
        size_array_ = size;
        memory_size_ = size;
        smart_resize_ = A.smart_resize_;
        inject_array(A);
      }
    else
      {
        // Creation d'un tableau "detache"
        mem_ = 0;
        span_ = 0;
        size_array_ = 0;
        memory_size_ = 0;
        smart_resize_ = 0;
        storage_type_ = STANDARD;
      }
  }

  inline TRUSTArray& resize_array(entier new_size)
  {
    assert(new_size >= 0);
    assert(new_size == size_array_ || span_ == 0 || (mem_ != 0 && ref_count() == 1));

    if ((smart_resize_ == 0) || (new_size > memory_size_))
      memory_resize(new_size, COPY_OLD + INITIALIZE_NEW);
    size_array_ = new_size;
    return *this;
  }

  void set_smart_resize(entier flag)
  {
    assert(flag == 0 || flag == 1);
    smart_resize_ = flag;
  }

  void set_mem_storage(const Storage storage) { storage_type_ = storage; }
  Storage get_mem_storage() const { return storage_type_; }

  TRUSTArray& ref_data(_TYPE_ *ptr, entier size)
  {
    assert(ptr != 0 || size == 0);
    assert(size >= 0);
    detach_array();
    span_ = ptr;
    size_array_ = size;
    return *this;
  }

  TRUSTArray& ref_array(const TRUSTArray<_TYPE_>& m)
  {
    assert(&m != this);
    detach_array();
    attach_array(m);
    return *this;
  }

  TRUSTArray& operator=(const TRUSTArray<_TYPE_>& m)
  {
    if (&m != this)
      {
        const entier new_size = m.size_array();
        if (new_size != size_array())
          {
            if ((smart_resize_ == 0) || (new_size > memory_size_))
              memory_resize(new_size, 0); // Pas d'initialisation
            size_array_ = new_size;
          }
        inject_array(m);
      }
    return *this;
  }

  virtual void reset() { detach_array(); }

  TRUSTArray& operator=(_TYPE_ x)
  {
    const entier n = size_array();
    _TYPE_ *data = addr();
    for (entier i = 0; i < n; i++)
      {
        data[i] = x;
      }
    return *this;
  }

  inline _TYPE_& operator[](entier i);
  inline const _TYPE_& operator[](entier i) const;

  const _TYPE_* addr() const { return span_; }
  _TYPE_* addr() { return span_; }

  inline entier size_array() const { return size_array_; }

  entier ref_count() const
  {
    if (mem_)
      return mem_->ref_count();
    else
      return -1;
  }

  inline void append_array(_TYPE_ valeur)
  {
    assert(smart_resize_);
    assert(span_ == 0 || (mem_ != 0 && ref_count() == 1));

    if (size_array_+1 > memory_size_)
      memory_resize(size_array_+1, COPY_OLD);
    span_[size_array_] = valeur;
    size_array_++;
  }

  TRUSTArray& operator+=(const TRUSTArray<_TYPE_> &y)
  {
    assert(size_array() == y.size_array());
    _TYPE_ *dx = addr();
    const _TYPE_ *dy = y.addr();
    const entier n = size_array();
    for (entier i = 0; i < n; i++)
      dx[i] += dy[i];
    return *this;
  }

  TRUSTArray& operator+=(const _TYPE_ dy)
  {
    _TYPE_ *data = addr();
    const entier n = size_array();
    for (entier i = 0; i < n; i++)
      data[i] += dy;
    return *this;
  }

  TRUSTArray& operator-=(const TRUSTArray<_TYPE_> &y)
  {
    const entier size = size_array();
    assert(size == y.size_array());
    _TYPE_ *data = addr();
    const _TYPE_ *data_y = y.addr();
    for (entier i = 0; i < size; i++)
      data[i] -= data_y[i];
    return *this;
  }

  TRUSTArray& operator-=(const _TYPE_ dy)
  {
    _TYPE_ *data = addr();
    const entier n = size_array();
    for (entier i = 0; i < n; i++)
      data[i] -= dy;
    return *this;
  }

  TRUSTArray& inject_array(const TRUSTArray<_TYPE_> &source, entier nb_elements = -1, entier first_element_dest = 0, entier first_element_source = 0)
  {
    assert(&source != this);
    assert(nb_elements >= -1);
    assert(first_element_dest >= 0);
    assert(first_element_source >= 0);

    if (nb_elements < 0)
      nb_elements = source.size_array();

    assert(first_element_source + nb_elements <= source.size_array());
    assert(first_element_dest + nb_elements <= size_array());

    if (nb_elements > 0)
      {
        _TYPE_ * addr_dest = addr() + first_element_dest;
        const _TYPE_ * addr_source = source.addr() + first_element_source;
        entier i;
        for (i = 0; i < nb_elements; i++)
          {
            addr_dest[i] = addr_source[i];
          }
      }
    return *this;
  }

  TRUSTArray& copy_array(const TRUSTArray<_TYPE_>& a)
  {
    operator=(a);
    return *this;
  }

  TRUSTArray& operator*=(const _TYPE_ dy)
  {
    if (std::is_same<_TYPE_,entier>::value) throw;
    _TYPE_ *data = addr();
    const entier n = size_array();
    for (entier i = 0; i < n; i++)
      data[i] *= dy;
    return *this;
  }

  TRUSTArray& operator/=(const _TYPE_ dy)
  {
    if (std::is_same<_TYPE_,entier>::value) throw;
    _TYPE_ *data = addr();
    const entier n = size_array();
    for (entier i = 0; i < n; i++)
      data[i] /= dy;
    return *this;
  }

  void ordonne_array();

protected:
  void attach_array(const TRUSTArray<_TYPE_>& m)
  {
    assert(span_ == 0 && mem_ == 0);
    assert(&m != this);

    if (m.size_array() > 0)
      {
        mem_ = m.mem_;
        if (mem_)
          mem_->add_one_ref();
        span_ = m.span_;
        size_array_ = m.size_array_;
        memory_size_ = m.memory_size_;
        smart_resize_ = m.smart_resize_;
      }
    else { }
  }

  entier detach_array()
  {
    entier retour = 0;
    if (mem_)
      {
        if ((mem_->suppr_one_ref()) == 0)
          {
            delete mem_;
            retour = 1;
          }
        mem_ = 0;
      }
    span_ = 0;
    size_array_ = 0;
    memory_size_ = 0;
    return retour;
  }

  void fill_default_value(entier first, entier nb)
  {
    assert((nb == 0) || (first >= 0 && first < memory_size_));
    assert((nb == 0) || (nb > 0 && nb <= memory_size_ - first));
    _TYPE_ * data = addr();
    assert(data!=0 || nb==0);
    data += first;
    if (smart_resize_) { }
    else
      {
        for (entier i = 0; i < nb; i++)
          data[i] = (_TYPE_) 0;
      }
  }

private:
  VTRUSTdata<_TYPE_> *mem_;
  _TYPE_ *span_;

  entier size_array_;
  entier memory_size_;
  entier smart_resize_;

  Storage storage_type_;
  static constexpr entier COPY_OLD = 1;
  static constexpr entier INITIALIZE_NEW = 2;

  void memory_resize(entier new_size, entier options)
  {
    assert(new_size >= 0);
    entier old_mem_size = 0;
    if (mem_)
      old_mem_size = mem_->get_size();

    entier new_mem_size = new_size;
    if (smart_resize_ && (old_mem_size * 2 > new_size))
      new_mem_size = old_mem_size * 2;

    if (new_mem_size != old_mem_size)
      {
        const entier old_size_array = size_array_;
        assert(span_ == 0 || (mem_ != 0 && ref_count() == 1));
        if (new_mem_size == 0)
          {
            detach_array();
          }
        else
          {
            VTRUSTdata<_TYPE_> * new_p = new VTRUSTdata<_TYPE_>(new_mem_size, storage_type_);
            _TYPE_ * new_data = new_p->get_data();
            entier copy_size = 0;
            if (span_ != 0)
              {
                if (options & COPY_OLD)
                  {
                    copy_size = size_array_;
                    if (new_size < copy_size)
                      copy_size = new_size;
                    for (entier i = 0; i < copy_size; i++)
                      new_data[i] = span_[i];
                  }
                detach_array();
              }
            mem_ = new_p;
            span_ = new_data;
            memory_size_ = new_mem_size;
            if (options & INITIALIZE_NEW)
              fill_default_value(copy_size, new_mem_size - copy_size);
            size_array_ = old_size_array;
          }
      }
  }
};

/* *********************************** *
 * FONCTIONS NON MEMBRES DE TRUSTArray *
 * *********************************** */

template <typename _TYPE_>
inline entier operator==(const TRUSTArray<_TYPE_>& v, const TRUSTArray<_TYPE_>& a)
{
  const entier n = v.size_array();
  const entier na = a.size_array();
  entier resu = 1;
  if (n != na)
    {
      resu = 0;
    }
  else
    {
      const _TYPE_* vv = v.addr();
      const _TYPE_* av = a.addr();
      entier i;
      for (i = 0; i < n; i++)
        {
          if (av[i] != vv[i])
            {
              resu = 0;
              break;
            }
        }
    }
  return resu;
}

template <typename _TYPE_>
inline entier imin_array(const TRUSTArray<_TYPE_>& dx)
{
  entier indice_min = -1;
  const entier size = dx.size_array();
  if (size > 0)
    {
      indice_min = 0;
      _TYPE_ valeur_min = dx[0];
      for(entier i = 1; i < size; i++)
        {
          const _TYPE_ val = dx[i];
          if(val < valeur_min)
            {
              indice_min = i;
              valeur_min = val;
            }
        }
    }
  return indice_min;
}

template <typename _TYPE_>
inline entier imax_array(const TRUSTArray<_TYPE_>& dx)
{
  entier indice_max = -1;
  const entier size = dx.size_array();
  if (size > 0)
    {
      indice_max = 0;
      _TYPE_ valeur_max = dx[0];
      for(entier i = 1; i < size; i++)
        {
          const _TYPE_ val = dx[i];
          if(val > valeur_max)
            {
              indice_max = i;
              valeur_max = val;
            }
        }
    }
  return indice_max;
}

template <typename _TYPE_>
inline _TYPE_ min_array(const TRUSTArray<_TYPE_>& dx)
{
  const entier size = dx.size_array();
  assert(size > 0);
  _TYPE_ valeur_min = dx[0];
  for(entier i = 1; i < size; i++)
    {
      const _TYPE_ val = dx[i];
      if (val < valeur_min)
        valeur_min = val;
    }
  return valeur_min;
}

template <typename _TYPE_>
inline _TYPE_ max_array(const TRUSTArray<_TYPE_>& dx)
{
  const entier size = dx.size_array();
  assert(size > 0);
  _TYPE_ valeur_max = dx[0];
  for(entier i = 1; i < size; i++)
    {
      const _TYPE_ val = dx[i];
      if (val > valeur_max)
        valeur_max = val;
    }
  return valeur_max;
}

template <typename _TYPE_>
inline _TYPE_ max_abs_array(const TRUSTArray<_TYPE_>& dx)
{
  if (std::is_same<_TYPE_,entier>::value) throw;
  const entier size = dx.size_array();
  assert(size > 0);
  _TYPE_ valeur_max = fabs(dx[0]);
  for(entier i = 1; i < size; i++)
    {
      const _TYPE_ val = fabs(dx[i]);
      if (val > valeur_max)
        valeur_max = val;
    }
  return valeur_max;
}

static int fonction_compare_arroffloat_ordonner(const void * data1, const void * data2)
{
  const float x = *(const float*)data1;
  const float y = *(const float*)data2;
  if (x < y)
    return -1;
  else if (x > y)
    return 1;
  else
    return 0;
}

static int fonction_compare_arrofdouble_ordonner(const void * data1, const void * data2)
{
  const double x = *(const double*)data1;
  const double y = *(const double*)data2;
  if (x < y)
    return -1;
  else if (x > y)
    return 1;
  else
    return 0;
}

static int fonction_compare_arrofentier_ordonner(const void * data1, const void * data2)
{
  const entier x = *(const entier*)data1;
  const entier y = *(const entier*)data2;
  return x - y;
}

/* ***************************** *
 * FONCTIONS MEMBRES DE TRUSTTab *
 * ***************************** */

template <typename _TYPE_>
inline _TYPE_& TRUSTArray<_TYPE_>::operator[](entier i)
{
  assert(span_ != nullptr && i >= 0 && i < size_array_);
  assert((smart_resize_==1)|| (span_[i] > -DMAXFLOAT && span_[i] < DMAXFLOAT));
  return span_[i];
}

template <>
inline entier& TRUSTArray<entier>::operator[](entier i)
{
  assert(span_ != nullptr && i >= 0 && i < size_array_);
  return span_[i];
}

template <typename _TYPE_>
inline const _TYPE_& TRUSTArray<_TYPE_>::operator[](entier i) const
{
  assert(span_ != nullptr && i >= 0 && i < size_array_);
  assert(span_[i] > -DMAXFLOAT && span_[i] < DMAXFLOAT);
  return span_[i];
}

template <>
inline const entier& TRUSTArray<entier>::operator[](entier i) const
{
  assert(span_ != nullptr && i >= 0 && i < size_array_);
  return span_[i];
}

template <>
inline void TRUSTArray<double>::ordonne_array()
{
  const int size = size_array_;
  if (size > 1)
    {
      double * data = span_;
      qsort(data, size, sizeof(double), fonction_compare_arrofdouble_ordonner);
    }
}

template <>
inline void TRUSTArray<float>::ordonne_array()
{
  const entier size = size_array();
  if (size > 1)
    {
      float *data = addr();
      qsort(data, size, sizeof(float), fonction_compare_arroffloat_ordonner);
    }
}

template <>
inline void TRUSTArray<entier>::ordonne_array()
{
  const entier size = size_array();
  if (size > 1)
    {
      entier *data = addr();
      qsort(data, size, sizeof(entier), fonction_compare_arrofentier_ordonner);
    }
}

#endif /* TRUSTArray_TPP_included */
