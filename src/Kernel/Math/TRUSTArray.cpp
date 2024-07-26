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

#include <TRUSTArray.h>
#include <string.h>

// Ajout d'un flag par appel a end_gpu_timer peut etre couteux (creation d'une string)
#ifdef _OPENMP
static bool timer=true;
#else
static bool timer=false;
#endif

// TRUSTArray kernels for device moved in .cpp file to avoid multiple definition during link
template <typename _TYPE_>
Sortie&  TRUSTArray<_TYPE_>::printOn(Sortie& os) const
{
#ifndef LATATOOLS
  this->checkDataOnHost();
  int sz = size_array();
  os << sz << finl;
  if (sz > 0)
    {
      const _TYPE_* v = span_.data();
      os.put(v,sz,sz);
    }
#endif
  return os;
}

template <typename _TYPE_>
Entree&  TRUSTArray<_TYPE_>::readOn(Entree& is)
{
#ifndef LATATOOLS
  int sz;
  is >> sz;
  if (sz >= 0)
    {
      // Appel a la methode sans precondition sur le type derive (car readOn est virtuelle, les autres proprietes seront initialisees correctement)
      resize_array_(sz);
      if (sz > 0)
        {
          _TYPE_* v = span_.data();
          is.get(v,sz);
        }
    }
  else
    {
      Cerr << "Error in TRUSTArray:readOn : size = " << sz << finl;
      Process::exit();
    }
#endif
  return is;
}


/** Protected method for resize. Used by derived classes.
 * Same as resize_array() with less checks.
 *
 * This is also where we deal with the STORAGE::TEMP_STORAGE capability, i.e. the Trav arrays.
 * There memory is taken from a shared pool (TRUSTTravPool)
 */
template <typename _TYPE_>
void TRUSTArray<_TYPE_>::resize_array_(int new_size, RESIZE_OPTIONS opt)
{
  assert(new_size >= 0);

  if (mem_ == nullptr)
    {
      if (!span_.empty()) // ref_data! We may pass here if just changing the shape of a tab
        {
          assert(size_array() == new_size);
          return;  // Nothing to do ...
        }
      // We avoid allocating for empty arrays ... those are typically situations where we will resize (with a non
      // null size) just after, so the real allocation will be made at that point.
      if(new_size == 0) return;

      // First allocation - memory space should really be malloc'd:
      if(storage_type_ == STORAGE::TEMP_STORAGE)
        mem_ = TRUSTTravPool<_TYPE_>::GetFreeBlock(new_size);
      else
        mem_ = std::make_shared<Vector_>(Vector_(new_size));

      span_ = Span_(*mem_);

      // We should never have to worry about device allocation here:
      if (isAllocatedOnDevice(mem_->data()))
        {
          if (opt == RESIZE_OPTIONS::COPY_INIT)
            data_location_ = std::make_shared<DataLocation>(DataLocation::Device);
          else
            data_location_ = std::make_shared<DataLocation>(DataLocation::Host);
        }
      else
        data_location_ = std::make_shared<DataLocation>(DataLocation::HostOnly);

      if(opt == RESIZE_OPTIONS::COPY_INIT)
        operator=((_TYPE_)0); // To initialize on device or host
      //std::fill(mem_->begin(), mem_->end(), (_TYPE_) 0);
    }
  else
    {
      // Array is already allocated, we want to resize:
      // array must not be shared! (also checked in resize_array()) ... but, still, we allow passing here (i.e. no assert)
      // only if we keep the same size_array(). This is for example invoked by TRUSTTab when just changing the overall shape of
      // the array without modifying the total number of elems ...
      int sz_arr = size_array();
      if(new_size != sz_arr) // Yes, we compare to the span's size
        {
          assert(ref_count() == 1);  // from here on, we *really* should not be shared

          if (storage_type_ == STORAGE::TEMP_STORAGE)
            {
              // Resize of a Trav: if the underlying mem_ is already big enough, just update the span, and possibly fill with 0
              // else, really increase memory allocation using the TRUSTTravPool.
              int mem_sz = (int)mem_->size();
              if (new_size <= mem_sz)
                {
                  // Cheat, simply update the span (up or down)
                  span_ = Span_(span_.begin(), span_.begin()+new_size);
                  // Possibly set to 0 extended part:
                  if (new_size > sz_arr && opt == RESIZE_OPTIONS::COPY_INIT)
                    {
                      checkDataOnHost();
                      std::fill(span_.begin() + sz_arr, span_.end(), (_TYPE_) 0);
                    }
                }
              else  // Real size increase of the underlying std::vector
                {
                  // ResizeBlock
                  mem_ = TRUSTTravPool<_TYPE_>::ResizeBlock(mem_, new_size);
                  span_ = Span_(*mem_);
                  if (opt == RESIZE_OPTIONS::COPY_INIT)
                    {
                      checkDataOnHost();
                      std::fill(span_.begin() + sz_arr, span_.end(), (_TYPE_) 0);
                    }
                }
            }
          else  // Normal (non Trav) arrays
            {
              _TYPE_ * prev_ad = span_.data(); // before resize!
              Span_ prev_span = span_;
              mem_->resize(new_size);
              span_ = Span_(*mem_);
              // Possibly set to 0 extended part, since we have a custom Vector allocator not doing it by default (TVAlloc):
              if (new_size > sz_arr && opt == RESIZE_OPTIONS::COPY_INIT && get_data_location() == DataLocation::HostOnly)
                std::fill(span_.begin()+sz_arr, span_.end(), (_TYPE_) 0);
              if(get_data_location() != DataLocation::HostOnly && new_size > sz_arr)
                if (prev_ad != span_.begin())
                  // hiiiic, when sizing up and down an array we might end up not changing the block at all!
                  // TODO find a nicer way to spot this ... TODO device allocation should be based on real underlying block
                  // size, not size_array() as it is now ...
                  {
                    // Delete former block before allocating a new one !
                    deleteOnDevice(prev_ad, sz_arr);
                    set_data_location(DataLocation::HostOnly);
                    // Allocate new (bigger) block on device:
                    allocateOnDevice(*this);
                    // Copy data (use a dummy TRUSTArray just because of inject_array API)
                    TRUSTArray<_TYPE_> dummy_src;
                    dummy_src.span_ = prev_span;
                    inject_array(dummy_src, sz_arr);
                  }
            }
        }
    }
}

/**  Copie les elements source[first_element_source + i] dans les elements  (*this)[first_element_dest + i] pour 0 <= i < nb_elements
*    Les autres elements de (*this) sont inchanges.

* @param  const ArrOfDouble& m: le tableau a utiliser, doit etre different de *this !
* @param int nb_elements: nombre d'elements a copier, nb_elements >= -1. Si nb_elements==-1, on copie tout le tableau m. Valeurs par defaut: -1
* @param int first_element_dest. Valeurs par defaut: 0
* @param int first_element_source. Valeurs par defaut: 0
* @return ArrOfDouble& : *this
* @throw Sort en erreur si la taille du tableau m est plus grande que la taille de tableau this.
*/
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::inject_array(const TRUSTArray& source, int nb_elements, int first_element_dest, int first_element_source)
{
  assert(&source != this && nb_elements >= -1);
  assert(first_element_dest >= 0 && first_element_source >= 0);

  if (nb_elements < 0) nb_elements = source.size_array();

  assert(first_element_source + nb_elements <= source.size_array());
  assert(first_element_dest + nb_elements <= size_array());

  if (nb_elements > 0)
    {
      bool kernelOnDevice = checkDataOnDevice(source);
      start_gpu_timer();
      if (kernelOnDevice)
        {
#ifndef LATATOOLS
          const auto addr_source = source.view_ro();
          auto addr_dest = view_rw();
          Kokkos::parallel_for(__KERNEL_NAME__, nb_elements, KOKKOS_LAMBDA(const int i) { addr_dest[first_element_dest+i] = addr_source[first_element_source+i]; });
#endif
        }
      else
        {
          // PL: On utilise le memcpy car c'est VRAIMENT plus rapide (10% +vite sur RNR_G20)
          const _TYPE_ * addr_source = source.span_.data() + first_element_source;
          _TYPE_ * addr_dest = span_.data() + first_element_dest;
          memcpy(addr_dest, addr_source, nb_elements * sizeof(_TYPE_));
        }
      if (timer) end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
    }
  return *this;
}

/** Remplit le tableau avec la x en parametre (x est affecte a toutes les cases du tableau)
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator=(_TYPE_ x)
{
  const int size = size_array();
  bool kernelOnDevice = checkDataOnDevice();
  start_gpu_timer(size>100 ? __KERNEL_NAME__ : "");
  if (kernelOnDevice)
    {
#ifndef LATATOOLS
      auto data = view_rw();
      Kokkos::parallel_for(__KERNEL_NAME__, size, KOKKOS_LAMBDA(const int i) { data[i] = x; });
#endif
    }
  else
    {
      _TYPE_ *data = span_.data();
      for (int i = 0; i < size; i++) data[i] = x;
    }
  if (timer) end_gpu_timer(kernelOnDevice, size>100 ? __KERNEL_NAME__ : "");
  return *this;
}

/** Addition case a case sur toutes les cases du tableau : la taille de y doit etre au moins egale a la taille de this
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator+=(const TRUSTArray& y)
{
  assert(size_array()==y.size_array());
  int size = size_array();
  bool kernelOnDevice = checkDataOnDevice(y);
  start_gpu_timer();
  if (kernelOnDevice)
    {
#ifndef LATATOOLS
      const auto dy = y.view_ro();
      auto dx = view_rw();
      Kokkos::parallel_for(__KERNEL_NAME__, size, KOKKOS_LAMBDA(const int i) { dx[i] += dy[i]; });
#endif
    }
  else
    {
      const _TYPE_* dy = y.span_.data();
      _TYPE_* dx = span_.data();
      for (int i = 0; i < size; i++) dx[i] += dy[i];
    }
  if (timer) end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
  return *this;
}

/** Ajoute la meme valeur a toutes les cases du tableau
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator+=(const _TYPE_ dy)
{
  int size = size_array();
  bool kernelOnDevice = checkDataOnDevice();
  start_gpu_timer();
  if (kernelOnDevice)
    {
#ifndef LATATOOLS
      auto data = view_rw();
      Kokkos::parallel_for(__KERNEL_NAME__, size, KOKKOS_LAMBDA(const int i) { data[i] += dy; });
#endif
    }
  else
    {
      _TYPE_ *data = span_.data();
      for(int i = 0; i < size; i++) data[i] += dy;
    }
  if (timer) end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
  return *this;
}

/** Soustraction case a case sur toutes les cases du tableau : tableau de meme taille que *this
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator-=(const TRUSTArray& y)
{
  assert(size_array() == y.size_array());
  int size = size_array();
  bool kernelOnDevice = checkDataOnDevice(y);
  start_gpu_timer();
  if (kernelOnDevice)
    {
#ifndef LATATOOLS
      auto data = view_rw();
      const auto data_y = y.view_ro();
      Kokkos::parallel_for(__KERNEL_NAME__, size, KOKKOS_LAMBDA(const int i) { data[i] -= data_y[i]; });
#endif
    }
  else
    {
      _TYPE_ * data = span_.data();
      const _TYPE_ * data_y = y.span_.data();
      for (int i = 0; i < size; i++) data[i] -= data_y[i];
    }
  if (timer) end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
  return *this;
}

/** soustrait la meme valeur a toutes les cases
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator-=(const _TYPE_ dy)
{
  operator+=(-dy);
  return *this;
}

/** muliplie toutes les cases par dy
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator*= (const _TYPE_ dy)
{
  int size = size_array();
  bool kernelOnDevice = checkDataOnDevice();
  start_gpu_timer();
  if (kernelOnDevice)
    {
#ifndef LATATOOLS
      auto data = view_rw();
      Kokkos::parallel_for(__KERNEL_NAME__, size, KOKKOS_LAMBDA(const int i) { data[i] *= dy; });
#endif
    }
  else
    {
      _TYPE_ *data = span_.data();
      for(int i=0; i < size; i++) data[i] *= dy;
    }
  if (timer) end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
  return *this;
}

/** divise toutes les cases par dy (pas pour TRUSTArray<int>)
 */
template <typename _TYPE_>
TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator/= (const _TYPE_ dy)
{
  if (std::is_same<_TYPE_,int>::value) throw;
  operator*=(1/dy);
  return *this;
}

// Pour instancier les methodes templates dans un .cpp
template class TRUSTArray<double>;
template class TRUSTArray<int>;
template class TRUSTArray<float>;
