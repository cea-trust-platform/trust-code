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

#ifndef TRUST_Vector_included
#define TRUST_Vector_included

#include <vector>
#include <Nom.h>

class MD_Vector;

// MACRO to replace VECT(THECLASS) by TRUST_Vector<THECLASS> & keep previous syntax for some developers
#define VECT(_TYPE_) TRUST_Vector<_TYPE_>

template<bool B, typename T> using enable_if_t = typename std::enable_if<B, T>::type;

/*! @brief classe TRUST_Vector
 *
 *  - La classe template TRUST_Vector est utilisable pour n'importe quelle classe
 *
 *      Utilisation (par exemple):
 *
 *        - TRUST_Vector<MD_Vector>
 *        - TRUST_Vector<Milieu_base>
 *        - TRUST_Vector<TRUSTArray<double>>
 */
template<typename _CLASSE_>
class TRUST_Vector: public Objet_U
{
  using value_type = _CLASSE_;
  using STLVect = std::vector<_CLASSE_>;
  using Iterator = typename STLVect::iterator;
  using Const_Iterator = typename STLVect::const_iterator;

protected:
  unsigned taille_memoire() const override { throw; }
  Sortie& printOn(Sortie& s) const override { return printOn_<_CLASSE_>(s); }
  Entree& readOn(Entree& s) override { return readOn_<_CLASSE_>(s); }

  int duplique() const override
  {
    TRUST_Vector *xxx = new TRUST_Vector(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

private:
  STLVect z_vect_;

  template<typename _TYPE_>
  enable_if_t< !(std::is_same<_TYPE_,MD_Vector>::value), Entree&>
  readOn_(Entree& s)
  {
    int i;
    s >> i;
    z_vect_.resize(i);
    for (auto &itr : z_vect_) s >> itr;
    return s;
  }

  template<typename _TYPE_>
  enable_if_t< !(std::is_same<_TYPE_,MD_Vector>::value), Sortie&>
  printOn_(Sortie& s) const
  {
    s << (int) z_vect_.size() << space;
    for (auto &itr : z_vect_) s << itr << space;
    return s << finl;
  }

  // MD_Vector class does not derive from Objet_U => no readOn & printOn
  template<typename _TYPE_>
  enable_if_t<(std::is_same<_TYPE_,MD_Vector>::value), Entree&>
  readOn_(Entree& s) { return s; }

  template<typename _TYPE_>
  enable_if_t<(std::is_same<_TYPE_,MD_Vector>::value), Sortie&>
  printOn_(Sortie& s) const { return s ; }

public:
  ~TRUST_Vector() { z_vect_.clear(); }
  TRUST_Vector() : z_vect_() { }
  TRUST_Vector(int i) : z_vect_(i) { }
  TRUST_Vector(const TRUST_Vector& avect) : Objet_U(avect) { z_vect_ = avect.z_vect_; }

  // get stl vector
  const STLVect& get_stl_vect() const { return z_vect_; }
  STLVect& get_stl_vect() { return z_vect_; }

  // iterators on TRUST_Vector
  inline Iterator begin() { return z_vect_.begin(); }
  inline Iterator end() { return z_vect_.end(); }
  inline Const_Iterator begin() const { return z_vect_.begin(); }
  inline Const_Iterator end() const { return z_vect_.end(); }

  const _CLASSE_& operator[](int i) const { return static_cast<const _CLASSE_&>(z_vect_[i]); }
  _CLASSE_& operator[](int i) { return static_cast<_CLASSE_&>(z_vect_[i]); }

  // Elie Saikali : Pas de negociation pour ca desole ...
  const _CLASSE_& operator()(int i) const = delete;
  _CLASSE_& operator()(int i) = delete;

  _CLASSE_& add() = delete; //{ return add(_CLASSE_()); }

  int size() const { return (int)z_vect_.size(); }
  void reset() { z_vect_.clear(); }
  void dimensionner(int i) { z_vect_.resize(i); }

  Entree& lit(Entree& s)
  {
    int i;
    s >> i;
    dimensionner_force(i);
    for (auto& itr : z_vect_) s >> itr;
    return s;
  }

  void dimensionner_force(int i)
  {
    z_vect_.clear();
    z_vect_.resize(i);
  }

  TRUST_Vector& operator=(const TRUST_Vector& avect)
  {
    if (this == &avect) return *this;

    z_vect_ = avect.z_vect_;
    return *this;
  }

  /* Add a new element to the vect */
  _CLASSE_& add(const _CLASSE_& data_to_add)
  {
    z_vect_.push_back(data_to_add);
    return z_vect_.back();
  }

  /* Append a vect to a vect */
  void add(const TRUST_Vector& v2) { z_vect_.insert(end(), v2.begin(), v2.end()); }
};

#endif /* TRUST_Vector_included */
