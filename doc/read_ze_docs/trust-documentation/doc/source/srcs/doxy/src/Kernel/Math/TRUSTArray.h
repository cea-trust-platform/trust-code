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

#ifndef TRUSTArray_included
#define TRUSTArray_included

#include <TRUSTIterator.h>
#include <VTRUSTdata.h>
#include <Array_base.h>
#include <Double.h>
#include <span.hpp>
#include <climits>
#include <Device.h>

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
  using Iterator = TRUSTIterator<TRUSTArray<_TYPE_>>;
  using Span_ = tcb::span<_TYPE_>;

  // Iterators
  inline Iterator begin() { return Iterator(data_); }
  inline Iterator end() { return Iterator(data_ + size_array_); }

  virtual ~TRUSTArray()
  {
    detach_array();
    size_array_ = -1; // Paranoia: si size_array_==-1, c'est un zombie
  }

  TRUSTArray() : p_(0), data_(0), size_array_(0), memory_size_(0), smart_resize_(0), storage_type_(STANDARD) { }

  TRUSTArray(int n): p_(0), data_(0), size_array_(n), memory_size_(n), smart_resize_(0), storage_type_(STANDARD)
  {
    if (n)
      {
        p_ = new VTRUSTdata<_TYPE_>(n, STANDARD);
        data_ = p_->get_data();
        fill_default_value(COPY_INIT, 0, n);
      }
  }

  // Constructeur par copie : deep copy (on duplique les donnees)
  TRUSTArray(const TRUSTArray& A) : Array_base()
  {
    const int size = A.size_array();
    if (size > 0)
      {
        // Creation d'un tableau "normal"
        storage_type_ = STANDARD;
        p_ = new VTRUSTdata<_TYPE_>(size, STANDARD);
        data_ = p_->get_data();
        size_array_ = size;
        memory_size_ = size;
        smart_resize_ = A.smart_resize_;
        if (A.isDataOnDevice()) allocateOnDevice(*this);
        inject_array(A);
      }
    else
      {
        // Creation d'un tableau "detache"
        p_ = 0;
        data_ = 0;
        size_array_ = 0;
        memory_size_ = 0;
        smart_resize_ = 0;
        storage_type_ = STANDARD;
      }
  }

  // Methodes de construction tardive (on cree un tableau vide avec TRUSTArray() puis on appelle ces methodes
  // pour modifier les caracteristiques du tableau : Change le nombre d'elements du tableau
  inline void resize(int new_size, Array_base::Resize_Options opt = COPY_INIT) { resize_array(new_size, opt); }
  inline void resize_array(int new_size, Array_base::Resize_Options opt = COPY_INIT);

  // Methodes de gestion de l'allocation memoire :
  // Assigne une valeur au drapeau "smart_resize" (reallocation uniquement si la taille augmente)
  inline void set_smart_resize(int flag);

  // Gestion du type de memoire alouee (standard ou pool de memoire Trio-U)
  inline void set_mem_storage(const Storage storage);
  inline Storage get_mem_storage() const { return storage_type_; }

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
  inline _TYPE_ * addrForDevice();
  inline const _TYPE_ * addrForDevice() const;

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
  inline virtual void resize_tab(int n, Array_base::Resize_Options opt = COPY_INIT);

  // Host/Device methods:
  inline DataLocation get_dataLocation() { return p_==NULL ? HostOnly : p_->get_dataLocation(); }
  inline DataLocation get_dataLocation() const { return p_==NULL ? HostOnly : p_->get_dataLocation(); }
  inline void set_dataLocation(DataLocation flag) { if (p_!=NULL) p_->set_dataLocation(flag); }
  inline void set_dataLocation(DataLocation flag) const { if (p_!=NULL) p_->set_dataLocation(flag); }
  inline void checkDataOnHost() { checkDataOnHost(*this); }
  inline void checkDataOnHost() const { checkDataOnHost(*this); }
  inline bool isDataOnDevice() const { return isDataOnDevice(*this); }
  inline bool checkDataOnDevice(std::string kernel_name="??") { return checkDataOnDevice(*this, kernel_name); }
  inline bool checkDataOnDevice(std::string kernel_name="??") const { return checkDataOnDevice(*this, kernel_name); }
  inline bool checkDataOnDevice(const TRUSTArray& arr, std::string kernel_name="??") { return checkDataOnDevice(*this, arr, kernel_name); }

  inline virtual Span_ get_span() { return Span_(data_,size_array_); }
  inline virtual Span_ get_span_tot() { return Span_(data_,size_array_); }
  inline virtual const Span_ get_span() const { return Span_((_TYPE_*)data_, size_array_); }
  inline virtual const Span_ get_span_tot() const { return Span_((_TYPE_*)data_, size_array_); }
protected:
  inline void attach_array(const TRUSTArray& a, int start = 0, int size = -1);
  inline void fill_default_value(Array_base::Resize_Options opt, int first, int nb);
  inline void resize_array_(int n, Array_base::Resize_Options opt = COPY_INIT);
  inline int detach_array();

private:
  // B. Mathieu 22/06/2004 : je mets ces membres "private" pour forcer le passage par les accesseurs dans les classes derivees,
  // au cas ou on voudrait modifier l'implementation.

  inline void memory_resize(int new_size, Array_base::Resize_Options opt = COPY_INIT);

  // Zone de memoire contenant les valeurs du tableau. Pointeur nul => le tableau est "detache" ou "ref_data", Pointeur non nul => le tableau est "normal"
  VTRUSTdata<_TYPE_>* p_;

  // Pointeur vers le premier element du tableau= Pointeur nul => le tableau est "detache". Pointeur non nul => le tableau est "normal" ou "ref_data"
  // Si p_ est non nul, data_ pointe quelque part a l'interieur de la zone allouee (en general au debut, sauf si le tableau a ete initialise avec ref() ou attach_array() avec start > 0)
  _TYPE_* data_;

  // Nombre d'elements du tableau (inferieur ou egal a memory_size_). Si le tableau est "detache", alors size_array_=0
  int size_array_;

  // Taille memoire reellement allouee pour le tableau (pour le mecanisme smart_resize_). memory_size_ est nul si le tableau est de type "ref_data". Sinon memory_size_ est egal a p_->size_.
  int memory_size_;

  // Drapeau indiquant si on applique une strategie d'allocation preventive (la memoire alouee augmente d'un facteur constant si la taille devient insuffisante).
  // Si smart_resize_ == 0, alors on a toujours p_->size_ == size
  int smart_resize_;

  // Drapeau indiquant si l'allocation memoire a lieu avec un new classique ou dans le pool de memoire temporaire de Trio
  Storage storage_type_;

  // Methodes de verification que le tableau est a jour sur le host:
  // ToDo OpenMP :Appels couteux (car non inlines?) depuis operator()[int] mais comment faire mieux ?
  inline void checkDataOnHost(const TRUSTArray& tab) const
  {
#ifdef _OPENMP
    if (tab.get_dataLocation()==Device)
      {
        copyFromDevice(tab, "const detected with checkDataOnHost()");
        exit_on_copy_condition(tab.size_array());
      }
#endif
  }
  inline void checkDataOnHost(TRUSTArray& tab)
  {
#ifdef _OPENMP
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
    if (kernel_name!="??" && tab.size_array()>100 && getenv ("TRUST_CLOCK_ON")!=NULL)
      {
        std::string clock(Process::nproc()>1 ? "[clock]#"+std::to_string(Process::me()) : "[clock]  ");
        std::cout << clock << "            [" << (flag ? "Kernel] " : "Host]   ") << kernel_name
                  << " with a loop on array [" << toString(tab.addrForDevice()).c_str() << "] of " << tab.size_array()
                  << " elements" << std::endl ;
      }
  }
  // Fonctions checkDataOnDevice pour lancement conditionnel de kernels sur le device:
  // -Si les tableaux passes en parametre sont sur a jour sur le device
  // -Si ce n'est pas le cas, les tableaux sont copies sur le host via checkDataOnHost
  inline bool checkDataOnDevice(const TRUSTArray& tab, std::string kernel_name) const
  {
    bool flag = tab.isDataOnDevice() && computeOnDevice;
    if (!flag)
      checkDataOnHost(tab);
    //else
    //  tab.set_dataLocation(Device); // non const array will be computed on device
    printKernel(flag, tab, kernel_name);
    return flag;
  }
  inline bool checkDataOnDevice(TRUSTArray& tab, std::string kernel_name)
  {
    bool flag = tab.isDataOnDevice() && computeOnDevice;
    if (!flag)
      checkDataOnHost(tab);
    else
      tab.set_dataLocation(Device); // non const array will be computed on device
    printKernel(flag, tab, kernel_name);
    return flag;
  }
  inline bool checkDataOnDevice(TRUSTArray& tab, const TRUSTArray& tab_const, std::string kernel_name="??")
  {
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
