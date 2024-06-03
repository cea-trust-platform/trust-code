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

#ifndef VTRUSTdata_included
#define VTRUSTdata_included

#include <Array_base.h>
#include <Memoire.h>
#include <assert.h>
#include <typeinfo>

class Memoire;
template<typename _TYPE_> class TRUSTArray;
template<typename _TYPE_> class TRUST_ptr_trav;

/*! @brief VTRUSTdata alloue une zone de memoire de la taille specifiee au constructeur, et libere la zone de memoire a la destruction.
 *
 * La memoire peut etre allouee sur le tas (avec new) ou par le mecanisme Memoire::add_trav_double.
 *  "ref_count" compte le nombre de pointeurs qui font reference a "this". (permet au dernier utilisateur de l'objet de le detruire)
 *
 * @sa TRUSTArray
 */
enum DataLocation { HostOnly, Host, Device, HostDevice, PartialHostDevice };

template<typename _TYPE_>
class VTRUSTdata
{
public:
  typedef _TYPE_ value_type;

  VTRUSTdata(int size, Array_base::Storage storage);

  // Detruit la zone de memoire allouee.
  // Precondition : ref_count == 0 (la zone de memoire ne doit etre referencee nulle part)
  ~VTRUSTdata()
  {
    assert(ref_count_ == 0);
    if (d_ptr_trav_ == 0) delete[] data_; // Stockage STANDARD
    else // Stockage TEMP_STORAGE
      {
        d_ptr_trav_->unlock();
        d_ptr_trav_ = 0;
      }
    data_ = 0;  // paranoia: si size_==-1 c'est qu'on pointe sur un zombie
    size_ = -1; //  (pointeur vers un objet qui a ete detruit)
  }

  inline _TYPE_ * get_data() { return data_; }
  inline const _TYPE_ * get_data() const { return data_; }
  inline int add_one_ref() { return ++ref_count_; } // Un nouveau tableau utilise cette zone memoire : incremente ref_count
  inline int suppr_one_ref() { assert(ref_count_ > 0); return (--ref_count_); } // Un tableau de moins utilise cette zone memoire : decremente ref_count
  inline int ref_count() const { return ref_count_; }
  inline int get_size() const { return size_; }

  // Options de localisation de data_ (Host ou Device ou les 2):
  // HostOnly   : data_ alloue sur host seulement (pas alloue sur device)
  // Host       : data_ est a jour sur host (pas sur device)
  // Device     : data_ est a jour sur device (pas sur host)
  // HostDevice : data_ est a jour sur host et device
  inline DataLocation get_dataLocation() { return dataLocation_; }
  inline DataLocation get_dataLocation() const { return dataLocation_; }
  inline void set_dataLocation(DataLocation flag) { dataLocation_ = flag; }
  //inline void set_dataLocation(DataLocation flag) const { dataLocation_ = flag; }

private:
  // Le constructeur par copie et l'operateur= sont interdits.
  VTRUSTdata(const VTRUSTdata& v) = delete;
  VTRUSTdata& operator=(const VTRUSTdata& v) = delete;

  // "data" est un pointeur sur une zone de memoire de taille sz * sizeof(_TYPE_), allouee par le constructeur et liberee par le destructeur.
  // Ce pointeur n'est jamais nul meme si size_==0
  _TYPE_ * data_;

  // Compteur incremente par add_one_ref et decremente par suppr_one_ref. Contient le nombre d'objets TRUSTArray dont le membre "p" pointe vers "this". On a ref_count_ >= 0.
  int ref_count_;

  // "sz" est la taille du tableau "data_" alloue. On a sz >= 0.
  int size_;

  // Si storage est de type TEMP_STORAGE, d_ptr_trav porte la reference au zone allouee, sinon le pointeur est nul.
  TRUST_ptr_trav<_TYPE_> * d_ptr_trav_;

  // Drapeau du statut du data sur le Device:
  // HostOnly  : Non alloue sur le device encore
  // Host      : A jour sur le host pas sur le device
  // Device    : A jour sur le device pas sur le host
  // HostDevice: A jour sur le host et le device
  // PartialHostDevice : Etat temporaire: certaines valeurs sont plus a jour sur le host que le device (ex: faces frontieres ou items distants)
  DataLocation dataLocation_ = HostOnly;
};

using VDoubledata = VTRUSTdata<double>;
using VFloatdata = VTRUSTdata<float>;
using VIntdata = VTRUSTdata<int>;

/*! @brief Construit un VTRUSTdata de taille size >= 0
 *
 * @param (int s) taille du VTRUSTdata, il faut size >= 0
 * @param (Storage storage) indique si la memoire doit etre allouee avec "new" ou avec "memoire.add_trav_double()"
 */
template<typename _TYPE_>
VTRUSTdata<_TYPE_>::VTRUSTdata(int size, Array_base::Storage storage)
{
  //const int size_warning = 100000000;
  if (size<0)
    {
      Cerr << "Error, trying to allocate an array with a size>2^31. Switch to TRUST int64 version." << finl;
      Process::exit();
    }
  assert(size >= 0);
  if (size == 0) storage = Array_base::STANDARD;

  switch (storage)
    {
    case Array_base::STANDARD:
      {
        d_ptr_trav_ = 0;
        //if (size>size_warning) Cerr << "Warning: Allocating an array of " << size << " " << typeid(_TYPE_).name() << finl;
#ifdef _EXCEPTION_
        // Allocation de la memoire sur le tas
        try
          {
            data_ = new _TYPE_[size];
          }
        catch(...)
          {
            Cerr << "unable to allocate " << size << " _TYPE_ " << finl;
            Process::exit();
          }
#else
        data_ = new _TYPE_[size];
        if(!data_)
          {
            Cerr << "unable to allocate " << size << "_TYPE_ " << finl;
            Process::exit();
          }
#endif
        //if (size>size_warning) Cerr << " OK" << finl;
        break;
      }
    case Array_base::TEMP_STORAGE:
      {
        // Allocation de la memoire sur un tas special. La memoire ne sera pas rendue au systeme mais conservee pour une reutilisation ulterieure.
        Memoire& memoire = Memoire::Instance();
        //if (size>size_warning) Cerr << "Warning: Allocating or reusing a " << typeid(_TYPE_).name() << " Trav of " << size << " elements ...";
        d_ptr_trav_ = memoire.template add_trav<_TYPE_>(size);
        assert(d_ptr_trav_ != 0);
        data_ = d_ptr_trav_->ptr_();
        //if (size>size_warning) Cerr << " OK" << finl;
        break;

      }
    default:
      Process::exit();
    }
  ref_count_ = 1;
  size_ = size;
  assert(data_ != 0);
}

#endif /* VTRUSTdata_included */
