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

#ifndef TRUSTArray_included
#define TRUSTArray_included

#include <Array_base.h>
#include <TVAlloc.h>     // Custom allocator
#include <Double.h>
#include <span.hpp>
#include <memory>
#include <climits>
#include <vector>
#include <TRUSTTabs_forward.h>

#include <Device.h>
#include <View_Types.h>  // Kokkos stuff

//Booleans for checking if an execution space is host or device
#ifndef LATATOOLS

//Checking if ExecSpace is Device (Kokkos::DefaultExecutionSpace) or Host (DefaultHostExecutionSpace)
//When compiled on CPU, DefaultExecutionSpace=DefaultHostExecutionSpace = serial or OpenMP
//When compiled on GPU, DefaultExecutionSpace=Cuda, DefaultHostExecutionSpace=serial or OpenMP

//Checking if ExecSpace is Kokkos::DefaultExecutionSpace
//CPU or GPU, true if ExecSpace=Device
template <typename EXEC_SPACE >
constexpr bool is_default_exec_space = std::is_same<EXEC_SPACE, Kokkos::DefaultExecutionSpace>::value;

//Checking if ExecSpace is Kokkos::DefaultHostExecutionSpace
//GPU: !std::is_same<Kokkos::DefaultHostExecutionSpace, Kokkos::DefaultExecutionSpace>::value; true is Exec=Host
//CPU: !std::is_same<Kokkos::DefaultHostExecutionSpace, Kokkos::DefaultExecutionSpace>::value; false, always false
template <typename EXEC_SPACE >
constexpr bool is_host_exec_space = std::is_same<EXEC_SPACE, Kokkos::DefaultHostExecutionSpace>::value &&
                                    !std::is_same<Kokkos::DefaultHostExecutionSpace, Kokkos::DefaultExecutionSpace>::value;
#endif

/*! @brief Represents a an array of int/int64/double/... values.
 *
 * The two main members are mem_ and span_:
 * - 'mem_' is the (shared) pointer to the actual underlying data. The block of data itself is a std::vector. This
 *    block of data can be shared among several arrays (see below) or can be null if we reference external data (ref_data)
 * - 'span_' is a view on the actual data, and can point to a sub-part of *mem_.
 *
 * We can have 3 states for the array:
 *   - "detached": meaning mem_==nullptr, span_.empty() == true (state obtained with the default ctor and detach_array())
 *   - "normal" : in this case mem_ is a non-null shared pointer to a std::vector holding the data. 'span_' then typically represents the entire span of the vector.
 *    The array is always initialised with 0.
 *    When the array is destroyed, the shared_ptr 'mem_' is destroyed too, and if this was the last reference to the underlying data, the std::vector itself
 *    is freed.
 *    The memory space of the array can be shared among several TRUSTArray ('mem_' has the same underlying value in several instances).
 *    This is typically produced by ref_array().
 *    Note that when this happens, we have two instances pointing to the same underlying block of data, but *none* of them has precedence over the other one (none
 *    of them is 'the' owner of the data). Ownership is shared, and when the last owner is destroyed, memory is released.
 *   - "ref_data" : this is used to point to an exterior existing memory zone (not managed by TRUSTArray - for example data provided by an external Fortran func)
 *    In this case, mem_ remains nullptr, just the span_ is correctly filled, and no memory is released when the array is destroyed.
 *
 * Finally, in case of a "XXTrav" array, the memory allocation is special:
 *  - a Trav array is allocated once, but when released, memory is kept in a pool so that another request for a new Trav might re-use it without having
 *  to perform the allocation again.
 *  - a Trav may not be ref_data or ref_array.
 *  - see implementation details of this mechanism in the TRUSTTravPool class.
 *
 *  In the template parameters below, _TYPE_ is the value type (what it contains), _SIZE_ is the extent type (how many items).
 */
template <typename _TYPE_, typename _SIZE_>
class TRUSTArray : public Array_base
{
protected:
  unsigned taille_memoire() const override { return sizeof(TRUSTArray<_TYPE_, _SIZE_>); }

  int duplique() const override
  {
    TRUSTArray* xxx = new  TRUSTArray(*this);
    if(!xxx) Process::exit("Not enough memory ");
    return xxx->numero();
  }
  Sortie& printOn(Sortie& os) const override;
  Entree& readOn(Entree& is) override;

public:
  using Value_type_ = _TYPE_;
  using int_t = _SIZE_;
  using Iterator_ = typename tcb::span<_TYPE_>::iterator;
  using Vector_ = std::vector<_TYPE_, TVAlloc<_TYPE_> >;
  using Span_ = tcb::span<_TYPE_>;

  // Tests can inspect whatever they want:
  friend class TestTRUSTArray;

  // One instanciation with given template parameter may see all other template versions (useful in ref_as_big())
  template<typename _TYPE2_, typename _SIZE2_> friend class TRUSTArray;

  // Iterators
  inline Iterator_ begin() { return span_.begin(); }
  inline Iterator_ end() { return span_.end(); }

  inline virtual ~TRUSTArray();

  TRUSTArray() : TRUSTArray(0) { }

  TRUSTArray(_SIZE_ n) : storage_type_(STORAGE::STANDARD)
  {
    if (n)
      {
        // Initialize underlying std::vector<> with 0:
        mem_ = std::make_shared<Vector_>(Vector_(n, (_TYPE_)0));
        span_ = Span_(*mem_);
        data_location_ = std::make_shared<DataLocation>(DataLocation::HostOnly);
      }
  }

  /**
   * Copy ctor. Performs a deep copy.
   *
   * It is forbidden to deep copy a ref_data.
   */
  TRUSTArray(const TRUSTArray& A) :
    Array_base(), storage_type_(A.storage_type_)
  {
    assert(A.mem_ != nullptr || A.span_.empty());
    const _SIZE_ size = A.size_array();
    if (size > 0)
      {
        // storage_type_ must be set properly before invoking this! So that Trav mechanism works:
        resize_array_(size, RESIZE_OPTIONS::NOCOPY_NOINIT);
        if (A.get_data_location() != DataLocation::HostOnly)   // Only allocate *this on device if A is on the device
          // Not a Trav already allocated on device (avoid double alloc on device for Trav)
          if(!(storage_type_ == STORAGE::TEMP_STORAGE && isAllocatedOnDevice(mem_->data())))
            allocateOnDevice(*this);
        // Set new location AFTER possible allocation
        data_location_ = std::make_shared<DataLocation>(A.get_data_location());
        inject_array(A);
      }
  }

  // Resizing methods
  inline void resize(_SIZE_ new_size, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT) { resize_array(new_size, opt); }
  inline void resize_array(_SIZE_ new_size, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  // Conversion method - from a small array (_SIZE_=int) of TID (_TYPE_=trustIdType), return a big one (_SIZE_=long).
  // No data copied! This behaves somewhat like a ref_array:
  void ref_as_big(TRUSTArray<_TYPE_, _TYPE_>& out) const;

  // The other way around compared to ref_as_big! From big to small. In debug, size limit is checked.
  void ref_as_small(TRUSTArray<_TYPE_, int>& out) const;

  // Conversion from a BigArrOfTID to an ArrOfInt - see also similar method in TRUSTTab
  void from_tid_to_int(TRUSTArray<int, int>& out) const;

  /*! Memory allocation type - TEMP arrays (i.e. Trav) have a different allocation mechanism - see TRUSTTravPool.h) */
  inline void set_mem_storage(const STORAGE storage);
  inline STORAGE get_mem_storage() const { return storage_type_; }

  inline std::shared_ptr<Vector_> get_mem() {return mem_;}

  inline TRUSTArray& operator=(const TRUSTArray&);

  inline _TYPE_& operator[](_SIZE_ i);
  inline const _TYPE_& operator[](_SIZE_ i) const;

  inline _TYPE_& operator()(_SIZE_ i) { return operator[](i); }
  inline const _TYPE_& operator()(_SIZE_ i) const { return operator[](i); }

  // Ces methodes renvoient un pointeur vers le premier element du tableau pour une utilisation sur le host
  inline _TYPE_ * addr();
  inline const _TYPE_ * addr() const;
  // Les memes methodes pour une utilisation sur le device
  inline _TYPE_ *data();

  inline const _TYPE_ *data() const;

  /*! Return the size of the span on the data (not the full underlying allocated size)   */
  inline _SIZE_ size_array() const;

  /*! Returns the number of owners of the data, i.e. the number of Arrays pointing to the same underlying data */
  inline int ref_count() const;

  /*! Add a slot at the end of the array and store it valeur -> similar to vector<>::push_back */
  inline void append_array(_TYPE_ valeur);

  /*! Assign 'x' to all slots in the array */
  TRUSTArray& operator=(_TYPE_ x);

  /*! Addition case a case sur toutes les cases du tableau : la taille de y doit etre au moins egale a la taille de this */
  TRUSTArray& operator+=(const TRUSTArray& y);

  /*! ajoute la meme valeur a toutes les cases du tableau */
  TRUSTArray& operator+=(const _TYPE_ dy);

  /*! Soustraction case a case sur toutes les cases du tableau : tableau de meme taille que *this */
  TRUSTArray& operator-=(const TRUSTArray& y);

  /*! soustrait la meme valeur a toutes les cases */
  TRUSTArray& operator-=(const _TYPE_ dy);

  /*! muliplie toutes les cases par dy */
  TRUSTArray& operator*= (const _TYPE_ dy);

  /*! divise toutes les cases par dy (pas pour TRUSTArray<int>) */
  TRUSTArray& operator/= (const _TYPE_ dy);

  TRUSTArray& inject_array(const TRUSTArray& source, _SIZE_ nb_elements=-1,  _SIZE_ first_element_dest=0, _SIZE_ first_element_source=0);

  inline TRUSTArray& copy_array(const TRUSTArray& a)
  {
    operator=(a);
    return *this;
  }

  inline void ordonne_array();
  inline void array_trier_retirer_doublons();

  // methodes virtuelles

  /*! Construction de tableaux qui pointent vers des donnees existantes !!! Utiliser ref_data avec precaution */
  inline virtual void ref_data(_TYPE_* ptr, _SIZE_ size);
  /*! Remet le tableau dans l'etat obtenu avec le constructeur par defaut (libere la memoire mais conserve le mode d'allocation memoire actuel) */
  inline virtual void reset() { detach_array(); }
  inline virtual void ref_array(TRUSTArray&, _SIZE_ start=0, _SIZE_ sz=-1);
  inline virtual void resize_tab(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  // Host/Device methods:
  inline DataLocation get_data_location() {  return data_location_ == nullptr ? DataLocation::HostOnly : *data_location_;   }
  inline DataLocation get_data_location() const { return data_location_ == nullptr ? DataLocation::HostOnly : *data_location_; }
  inline void set_data_location(DataLocation flag) { if (data_location_ != nullptr) *data_location_ = flag; }
  inline void set_data_location(DataLocation flag) const { if (data_location_ != nullptr) *data_location_ = flag; }
  inline int size_mem() { return mem_ == nullptr ? 0 : (int)mem_->size(); };

  inline void ensureDataOnHost();
  inline void ensureDataOnHost() const;
  inline bool isDataOnDevice() const;
  inline bool checkDataOnDevice();
  inline bool checkDataOnDevice() const;
  inline bool checkDataOnDevice(const TRUSTArray& arr);

  inline virtual Span_ get_span() { return span_; }
  inline virtual Span_ get_span_tot() { return span_; }
  inline virtual const Span_ get_span() const { return span_; }
  inline virtual const Span_ get_span_tot() const { return span_; }

  //From TRUSTArray.h, now also here and overridden in TRUSTTab
  inline virtual _SIZE_ dimension_tot(int) const;

#ifdef KOKKOS

  //Clean the internal view of the Trust Array in case it is needed, if the Array is static to avoid it's destruction after Kokkos::finalize
  inline void CleanMyDualView()
  {
    dual_view_1_=DualView<_TYPE_,1>();
    dual_view_2_=DualView<_TYPE_,2>();
    dual_view_3_=DualView<_TYPE_,3>();
    dual_view_4_=DualView<_TYPE_,4>();
  }

  // Kokkos accessors (brace yourself!)

  // Read-only
  template <int SHAPE, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstView<_TYPE_,SHAPE> >
  view_ro() const;

  template <int SHAPE, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostView<_TYPE_,SHAPE> >
  view_ro() const;

  // Write-only
  template <int SHAPE, typename EXEC_SPACE=Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_,SHAPE> >
  view_wo();

  template <int SHAPE, typename EXEC_SPACE=Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_,SHAPE> >
  view_wo();

  // Read-write
  template <int SHAPE, typename EXEC_SPACE=Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_,SHAPE> >
  view_rw();

  template <int SHAPE, typename EXEC_SPACE=Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_,SHAPE> >
  view_rw();

#endif

protected:
  inline void attach_array(const TRUSTArray& a, _SIZE_ start=0, _SIZE_ size=-1);
  inline bool detach_array();

  void resize_array_(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

#ifdef KOKKOS
  // Kokkos members
  template<int _SHAPE_> inline void init_view() const;

  mutable DualView<_TYPE_, 1> dual_view_1_;
  mutable DualView<_TYPE_, 2> dual_view_2_;
  mutable DualView<_TYPE_, 3> dual_view_3_;
  mutable DualView<_TYPE_, 4> dual_view_4_;

#endif

private:

  //From TRUSTArray.h, now also here
  static constexpr int MAXDIM_TAB = 4;
  int nb_dim_ = 1;

  /*! Shared pointer to the actual underlying memory block:
   *   - shared_ptr because data can be shared between several owners -> see ref_array()
   *   - std::vector<> because we want contiguous data, with a smart allocation mechanism
   * WARNING: allocation mechanism for a Trav array is special.
   */
  std::shared_ptr<Vector_> mem_;

  /*! Actual view on the data. See comments at the top of the class */
  Span_ span_;

  /*! Drapeau indiquant si l'allocation memoire a lieu avec un new classique ou dans le pool de memoire temporaire de TRUST */
  STORAGE storage_type_;

  // Drapeau du statut du data sur le Device:
  // HostOnly  : Non alloue sur le device encore
  // Host      : A jour sur le host pas sur le device
  // Device    : A jour sur le device pas sur le host
  // HostDevice: A jour sur le host et le device
  // PartialHostDevice : Etat temporaire: certaines valeurs sont plus a jour sur le host que le device (ex: faces frontieres ou items distants)
  // In a shared_ptr because this state has the same status as mem_ (same sharing properties)
  mutable std::shared_ptr<DataLocation> data_location_;

private:
  /*! Debug */
  inline void printKernel(bool flag, const TRUSTArray& tab, std::string kernel_name) const;
  template<typename _TAB_> void ref_conv_helper_(_TAB_& out) const;

#ifdef KOKKOS

  // get_dual_view for _SHAPE_ == 1
  template<int _SHAPE_>
  typename std::enable_if<_SHAPE_ == 1, DualView<_TYPE_, 1>>::type&
                                                          get_dual_view() const;

  // get_dual_view for _SHAPE_ == 2
  template<int _SHAPE_>
  typename std::enable_if<_SHAPE_ == 2, DualView<_TYPE_, 2>>::type&
                                                          get_dual_view() const;

  // get_dual_view for _SHAPE_ == 3
  template<int _SHAPE_>
  typename std::enable_if<_SHAPE_ == 3, DualView<_TYPE_, 3>>::type&
                                                          get_dual_view() const;

  // get_dual_view for _SHAPE_ == 4
  template<int _SHAPE_>
  typename std::enable_if<_SHAPE_ == 4, DualView<_TYPE_, 4>>::type&
                                                          get_dual_view() const;

#endif
};

using ArrOfDouble = TRUSTArray<double, int>;
using ArrOfFloat = TRUSTArray<float, int>;
using ArrOfInt = TRUSTArray<int, int>;
using ArrOfTID = TRUSTArray<trustIdType, int>;

template <typename _TYPE_>
using BigTRUSTArray = TRUSTArray<_TYPE_, trustIdType>;

using BigArrOfDouble = BigTRUSTArray<double>;
using BigArrOfInt = BigTRUSTArray<int>;
using BigArrOfTID = BigTRUSTArray<trustIdType>;


/* *********************************** *
 * FONCTIONS NON MEMBRES DE TRUSTArray *
 * *********************************** */

#include <TRUSTArray_tools.tpp> // external templates function specializations ici ;)

/* ******************************* *
 * FONCTIONS MEMBRES DE TRUSTArray *
 * ******************************* */

#include <TRUSTArray_device.tpp> // OMP stuff
#include <TRUSTArray_kokkos.tpp> // Kokkos stuff

#include <TRUSTArray.tpp> // The rest here!

#endif /* TRUSTArray_included */
