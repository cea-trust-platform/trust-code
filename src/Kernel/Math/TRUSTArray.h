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

#ifndef TRUSTArray_included
#define TRUSTArray_included

#include <Array_base.h>
#include <Double.h>
#include <span.hpp>
#include <memory>
#include <climits>
#include <vector>
#include <Device.h>

#include <View_Types.h>  // Kokkos stuff

/*! @brief Represente un tableau d'elements de type int/double/float.
 *
 * L'etat du tableau est caracterise par la valeur de p_ et de data_ :
 *   * detache : on a alors p_==0, data_==0, size_array_==0 (etat obtenu par le constructeur par defaut, detach_array() et resize_array(0) dans certains cas)
 *   * normal : alors p pointe vers une structure VTRUSTdata, data_==p->data_ et size_array_ <= p->size_.
 *     A la destruction du tableau, si p->ref_count_ est nul on libere la memoire. data_ n'est pas nul.
 *     L'espace memoire du tableau peut etre partage entre plusieurs TRUSTArray (le pointeur "p" pointe alors sur une meme structure).
 *     Ce type de tableau est produit par ref_array().
 *     Le dernier TRUSTArray a utiliser une zone de memoire la detruit lorsqu'il a fini (voir detach_array())
 *     Si smart_resize_==0, alors on a toujours size_array_==p->size_.
 *   * ref_data : p_==0, data_ pointe vers un tableau existant, la memoire n'est pas liberee a la destruction du TRUSTArray.
 *     Ces tableaux sont obtenus par appel a ref_data(...) et peuvent servir pour importer un tableau fourni par une routine exterieure (C ou fortran)
 *
 *   Valeur initiale par defaut : voir "fill_default_value". Priere de supposer desormais que les tableaux ne sont pas initialises par defaut.
 *
 */
template <typename _TYPE_>
class TRUSTArray : public Array_base
{
protected:
  unsigned taille_memoire() const override { return sizeof(TRUSTArray<_TYPE_>); }

  int duplique() const override
  {
    TRUSTArray* xxx = new  TRUSTArray(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override;
  Entree& readOn(Entree& is) override;

public:
  using value_type = _TYPE_; // return int, double ou float
  using Iterator = typename tcb::span<_TYPE_>::iterator;
  using Vector_ = std::vector<_TYPE_>;
  using Span_ = tcb::span<_TYPE_>;

  // Iterators
  inline Iterator begin() { return span_.begin(); }
  inline Iterator end() { return span_.end(); }

  virtual ~TRUSTArray()
  {
  }

  TRUSTArray() : TRUSTArray(0) { }

  TRUSTArray(int n): storage_type_(STORAGE::STANDARD)
  {
    if (n)
      {
        mem_ = std::make_shared<Vector_>(Vector_(n));
        span_ = Span_(*mem_);
//        fill_default_value(RESIZE_OPTIONS::COPY_INIT, 0, n);
      }
  }

  // Constructeur par copie : deep copy (on duplique les donnees)
  TRUSTArray(const TRUSTArray& A) : Array_base()
  {
    const int size = A.size_array();
    if (size > 0)
      {
        // Creation d'un tableau "normal"
        storage_type_ = STORAGE::STANDARD;
        // We deep copy *only* the data span of A, not the full underlying A.mem_:
        mem_ = std::make_shared<Vector_>(Vector_(A.span_.begin(), A.span_.end()));
        span_ = Span_(*mem_);
        if (A.isDataOnDevice()) allocateOnDevice(*this);
#ifdef OPENMP   // TODO review this later ... I think we need to keep it
        inject_array(A);
#endif
      }
    else
      {
        // Creation d'un tableau "detache", mem_ remains nullptr
        span_ = A.span_;
        storage_type_ = STORAGE::STANDARD;
      }
  }

  // Methodes de construction tardive (on cree un tableau vide avec TRUSTArray() puis on appelle ces methodes
  // pour modifier les caracteristiques du tableau : Change le nombre d'elements du tableau
  inline void resize(int new_size, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT) { resize_array(new_size, opt); }
  inline void resize_array(int new_size, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  // TODO: to delete
  inline void set_smart_resize(int flag) {}

  // Gestion du type de memoire alouee (standard ou pool de memoire Trio-U)
  inline void set_mem_storage(const STORAGE storage);
  inline STORAGE get_mem_storage() const { return storage_type_; }

  // Operateur copie
  inline TRUSTArray& operator=(const TRUSTArray&);

  inline _TYPE_& operator[](int i);
  inline const _TYPE_& operator[](int i) const;

  inline _TYPE_& operator()(int i) { return operator[](i); }
  inline const _TYPE_& operator()(int i) const { return operator[](i); }

  // Ces methodes renvoient un pointeur vers le premier element du tableau pour une utilisation sur le host
  inline _TYPE_ * addr();
  inline const _TYPE_ * addr() const;
  // Les memes methodes pour une utilisation sur le device
  inline _TYPE_ *data();

  inline const _TYPE_ *data() const;

  // Renvoie le nombre d'elements du tableau (et non la taille allouee)
  inline int size_array() const;

  // Renvoie le nombre de tableaux qui pointent vers la stucture "*p_"
  inline int ref_count() const;

  // Ajoute une case en fin de tableau et y stocke la "valeur"
  inline void append_array(_TYPE_ valeur);

  // Remplit le tableau avec la x en parametre (x est affecte a toutes les cases du tableau)
  TRUSTArray& operator=(_TYPE_ x);

  // Addition case a case sur toutes les cases du tableau : la taille de y doit etre au moins egale a la taille de this
  TRUSTArray& operator+=(const TRUSTArray& y);

  // ajoute la meme valeur a toutes les cases du tableau
  TRUSTArray& operator+=(const _TYPE_ dy);

  // Soustraction case a case sur toutes les cases du tableau : tableau de meme taille que *this
  TRUSTArray& operator-=(const TRUSTArray& y);

  // soustrait la meme valeur a toutes les cases
  TRUSTArray& operator-=(const _TYPE_ dy);

  // muliplie toutes les cases par dy
  TRUSTArray& operator*= (const _TYPE_ dy);

  // divise toutes les cases par dy (pas pour TRUSTArray<int>)
  TRUSTArray& operator/= (const _TYPE_ dy);

  TRUSTArray& inject_array(const TRUSTArray& source, int nb_elements = -1,  int first_element_dest = 0, int first_element_source = 0);

  inline TRUSTArray& copy_array(const TRUSTArray& a)
  {
    operator=(a);
    return *this;
  }

  inline void ordonne_array();
  inline void array_trier_retirer_doublons();

  // methodes virtuelles

  // Construction de tableaux qui pointent vers des donnees existantes !!! Utiliser ref_data avec precaution (attention a size_array_)
  inline virtual void ref_data(_TYPE_* ptr, int size);
  // Remet le tableau dans l'etat obtenu avec le constructeur par defaut (libere la memoire mais conserve le mode d'allocation memoire actuel)
  inline virtual void reset() { detach_array(); }
  inline virtual void ref_array(TRUSTArray&, int start = 0, int sz = -1);
  inline virtual void resize_tab(int n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  // Host/Device methods:
  inline DataLocation get_dataLocation()
  {
    return mem_ == nullptr ? HostOnly : get_dataLocation();
  }
  inline DataLocation get_dataLocation() const { return mem_ == nullptr ? HostOnly : dataLocation_; }
  inline void set_dataLocation(DataLocation flag) { if (mem_ != nullptr) dataLocation_ = flag; }
  inline void set_dataLocation(DataLocation flag) const { if (mem_ != nullptr) dataLocation_ = flag; }
  inline void checkDataOnHost() { checkDataOnHost(*this); }
  inline void checkDataOnHost() const { checkDataOnHost(*this); }
  inline bool isDataOnDevice() const { return isDataOnDevice(*this); }
  inline bool checkDataOnDevice(std::string kernel_name="??") { return checkDataOnDevice(*this, kernel_name); }
  inline bool checkDataOnDevice(std::string kernel_name="??") const { return checkDataOnDevice(*this, kernel_name); }
  inline bool checkDataOnDevice(const TRUSTArray& arr, std::string kernel_name="??") { return checkDataOnDevice(*this, arr, kernel_name); }


  inline virtual Span_ get_span() { return span_; }
  inline virtual Span_ get_span_tot() { return span_; }
  inline virtual const Span_ get_span() const { return span_; }
  inline virtual const Span_ get_span_tot() const { return span_; }

  // Kokkos accessors
protected:
  inline void init_view_arr() const;

public:
  // Kokkos view accessors:
  inline ConstViewArr<_TYPE_> view_ro() const;  // Read-only
  inline ViewArr<_TYPE_> view_wo();             // Write-only
  inline ViewArr<_TYPE_> view_rw();             // Read-write

  inline void sync_to_host() const;              // Synchronize back to host

  inline void modified_on_host() const;         // Mark data as being modified on host side

protected:
  // Kokkos members
  mutable bool dual_view_init_ = false;
  mutable DualViewArr<_TYPE_> dual_view_arr_;

protected:
  inline void attach_array(const TRUSTArray& a, int start=0, int size=-1);
  inline void fill_default_value(RESIZE_OPTIONS opt, int first, int nb);
  inline void resize_array_(int n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline bool detach_array();

private:
  // B. Mathieu 22/06/2004 : je mets ces membres "private" pour forcer le passage par les accesseurs dans les classes derivees,
  // au cas ou on voudrait modifier l'implementation.
  inline void memory_resize(int new_size, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  // Zone de memoire contenant les valeurs du tableau. Pointeur nul => le tableau est "detache" ou "ref_data", Pointeur non nul => le tableau est "normal"
  std::shared_ptr<Vector_> mem_;

  // Pointeur vers le premier element du tableau= Pointeur nul => le tableau est "detache". Pointeur non nul => le tableau est "normal" ou "ref_data"
  // Si p_ est non nul, data_ pointe quelque part a l'interieur de la zone allouee (en general au debut, sauf si le tableau a ete initialise avec ref() ou attach_array() avec start > 0)
  Span_ span_;

  // Drapeau indiquant si l'allocation memoire a lieu avec un new classique ou dans le pool de memoire temporaire de Trio
  STORAGE storage_type_;

  // Drapeau du statut du data sur le Device:
  // HostOnly  : Non alloue sur le device encore
  // Host      : A jour sur le host pas sur le device
  // Device    : A jour sur le device pas sur le host
  // HostDevice: A jour sur le host et le device
  // PartialHostDevice : Etat temporaire: certaines valeurs sont plus a jour sur le host que le device (ex: faces frontieres ou items distants)
  mutable DataLocation dataLocation_ = HostOnly;

  // Methodes de verification que le tableau est a jour sur le host:
  // ToDo OpenMP :Appels couteux (car non inlines?) depuis operator()[int] mais comment faire mieux ?
  inline void checkDataOnHost(const TRUSTArray& tab) const
  {
#if defined(_OPENMP) && !defined(TRUST_USE_UVM)
    if (tab.get_dataLocation()==Device)
      {
        copyFromDevice(tab, "const detected with checkDataOnHost()");
        exit_on_copy_condition(tab.size_array());
      }
#endif
  }
  inline void checkDataOnHost(TRUSTArray& tab)
  {
#if defined(_OPENMP) && !defined(TRUST_USE_UVM)
    const DataLocation& loc = tab.get_dataLocation();
    if (loc==Host || loc==HostOnly || loc==PartialHostDevice) return;
    else if (loc==Device)
      {
        copyFromDevice(tab, "non-const detected with checkDataOnHost()");
        exit_on_copy_condition(tab.size_array());
      }
    // On va modifier le tableau (non const) sur le host:
    tab.set_dataLocation(Host);
#endif
  }

  // Fonction pour connaitre la localisation du tableau
  inline bool isDataOnDevice(const TRUSTArray& tab) const
  {
    return tab.get_dataLocation() == Device || tab.get_dataLocation() == HostDevice;
  }
  inline void printKernel(bool flag, const TRUSTArray& tab, std::string kernel_name) const
  {
    if (kernel_name!="??" && tab.size_array()>100 && getenv ("TRUST_CLOCK_ON")!=nullptr)
      {
        std::string clock(Process::is_parallel() ? "[clock]#"+std::to_string(Process::me()) : "[clock]  ");
        std::cout << clock << "            [" << (flag ? "Kernel] " : "Host]   ") << kernel_name
                  << " with a loop on array [" << toString(tab.data()).c_str() << "] of " << tab.size_array()
                  << " elements" << std::endl ;
      }
  }
  // Fonctions checkDataOnDevice pour lancement conditionnel de kernels sur le device:
  // -Si les tableaux passes en parametre sont sur a jour sur le device
  // -Si ce n'est pas le cas, les tableaux sont copies sur le host via checkDataOnHost
  inline bool checkDataOnDevice(const TRUSTArray& tab, std::string kernel_name) const
  {
#ifdef _OPENMP
    bool flag = tab.isDataOnDevice() && computeOnDevice;
    if (!flag)
      checkDataOnHost(tab);
    //else
    //  tab.set_dataLocation(Device); // non const array will be computed on device
    printKernel(flag, tab, kernel_name);
    return flag;
#else
    return false;
#endif
  }
  inline bool checkDataOnDevice(TRUSTArray& tab, std::string kernel_name)
  {
#ifdef _OPENMP
    bool flag = tab.isDataOnDevice() && computeOnDevice;
    if (!flag)
      checkDataOnHost(tab);
    else
      tab.set_dataLocation(Device); // non const array will be computed on device
    printKernel(flag, tab, kernel_name);
    return flag;
#else
    return false;
#endif
  }
  inline bool checkDataOnDevice(TRUSTArray& tab, const TRUSTArray& tab_const, std::string kernel_name="??")
  {
#ifdef _OPENMP
    bool flag = tab.isDataOnDevice() && tab_const.isDataOnDevice() && computeOnDevice;
    // Si un des deux tableaux n'est pas a jour sur le device alors l'operation se fera sur le host:
    if (!flag)
      {
        checkDataOnHost(tab);
        checkDataOnHost(tab_const);
      }
    else
      tab.set_dataLocation(Device); // non const array will be computed on device
    printKernel(flag, tab, kernel_name);
    return flag;
#else
    return false;
#endif
  }
};

using ArrOfDouble = TRUSTArray<double>;
using ArrOfFloat = TRUSTArray<float>;
using ArrOfInt = TRUSTArray<int>;

/* *********************************** *
 * FONCTIONS NON MEMBRES DE TRUSTArray *
 * *********************************** */

#include <TRUSTArray_tools.tpp> // external templates function specializations ici ;)

/* ******************************* *
 * FONCTIONS MEMBRES DE TRUSTArray *
 * ******************************* */

#include <TRUSTArray.tpp> // templates specializations ici ;)

#endif /* TRUSTArray_included */
