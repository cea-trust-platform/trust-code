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

#ifndef TRUST_List_included
#define TRUST_List_included

#include <algorithm> // for std::find
#include <Motcle.h>
#include <list> // pour stl list

/*! @brief classe TRUST_List
 *
 *  - La classe template TRUST_List est utilisable pour n'importe quelle classe
 *      Utilisation (par exemple):
 *
 *        - TRUST_List<Milieu_base>
 *
 *  - Le curseur (iterateur) est utilise pour n'importe quelle classe
 *      Utilisation (par exemple):
 *
 *        - TRUST_List_Curseur<Milieu_base>
 */

#define STLLIST(_TYPE_) TRUST_STLList<_TYPE_>

class TRUST_List
{ };

template<typename _CLASSE_>
class TRUST_STLList : public Objet_U
{
  using value_type = _CLASSE_;
  using STLList = std::list<_CLASSE_>;

protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    TRUST_STLList *xxx = new TRUST_STLList(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override
  {
    Nom accouverte = "{", accfermee = "}", virgule = ",", blanc = " ";
    if (size() == 0)
      os << "vide" << finl;
    else
      {
        os << accouverte << finl;
        for (auto &itr : list_)
          {
            os << itr;
            if (&itr != &list_.back()) os << blanc << virgule << finl;
          }
        os << finl << accfermee << finl;
      }
    return os;
  }

  Entree& readOn(Entree& is) override
  {
    Nom accouverte = "{", accfermee = "}", virgule = ",";
    Motcle nom_;
    is >> nom_;
    if (nom_ == (const char*) "vide") return is;

    if (nom_ != accouverte) Cerr << "Error while reading a list. One expected an opened bracket { to start." << finl, Process::exit();
    _CLASSE_ t;

    while (1)
      {
        is >> add(t);
        is >> nom_;
        if (nom_ == accfermee) return is;
        if (nom_ != virgule) Cerr << nom_ << " one expected a ',' or a '}'" << finl, Process::exit();
      }
  }

private:
  STLList list_;

public:
  // Iterator class to enable iterating over the elements of TRUST_STLList
  struct Iterator
  {
  private:
    typename std::list<_CLASSE_>::iterator it;
  public:
    Iterator(typename std::list<_CLASSE_>::iterator i) : it(i) { }
    int operator*() { return *it; }
    Iterator& operator++() { ++it; return *this; }
    bool operator!=(const Iterator& other) { return it != other.it; }
  };

  inline Iterator begin() { return Iterator(list_.begin()); }
  inline Iterator end() { return Iterator(list_.end()); }
  inline Iterator cbegin() const { return Iterator(list_.cbegin()); }
  inline Iterator cend() const { return Iterator(list_.cend()); }
  const std::list<_CLASSE_>& get_stl_list() const { return list_; }
  std::list<_CLASSE_>& get_stl_list() { return list_; }

  TRUST_STLList() : list_() { }
  TRUST_STLList(const _CLASSE_& t) : list_(t) { }
  TRUST_STLList(const TRUST_STLList& t) : Objet_U(t) { list_ = t.list_; }

  int size() const { return (int)list_.size(); }
  void vide() { list_.clear(); }

  /* Add list to list */
  TRUST_STLList& add(const TRUST_STLList& a_list)
  {
    list_.insert(list_.end(), a_list.get_stl_list().begin(), a_list.get_stl_list().end());
    return *this;
  }

  /* Add element to list */
  _CLASSE_& add(const _CLASSE_ &t)
  {
    list_.push_back(t);
    return static_cast<_CLASSE_&>(list_.back()); /* attention pour retourner une reference de t dans la liste !!!! */
  }

  /* Add element to list if it is not already inside */
  _CLASSE_& add_if_not(const _CLASSE_& t)
  {
    auto it = std::find(list_.begin(), list_.end(), t); // Search for the element in the list

    // If the element is not found, add it to the list
    if (it == list_.end())
      {
        list_.push_back(t);
        it = std::prev(list_.end());
      }

    return *it;
  }

  const _CLASSE_& front() const { return list_.front(); }
  _CLASSE_& front() { return list_.front(); }
  const _CLASSE_& dernier() const { return list_.back(); }
  _CLASSE_& dernier() { return list_.back(); }

  int est_vide() const { return list_.empty(); }

  void suppr(const _CLASSE_& t) { list_.remove(t); }

  // XXX : Elie Saikali
  // j'ai tente de supprimer tout ce bordel mais ... bon courage je te laisse faire
  _CLASSE_& operator[](int i)
  {
    assert (size() > 0);
    int ind = 0;
    for (auto& itr : list_)
      {
        if (ind == i) return itr;
        ind++;
      }
    Cerr << "TRUST_STLList : overflow of list " << finl;
    throw;
  }

  const _CLASSE_& operator[](int i) const
  {
    assert (size() > 0);
    int ind = 0;
    for (auto& itr : list_)
      {
        if (ind == i) return itr;
        ind++;
      }
    Cerr << "TRUST_STLList : overflow of list " << finl;
    throw;
  }

  _CLASSE_& operator[](const Nom& nom)
  {
    assert (size() > 0);
    int ind = 0;
    for (auto& itr : list_)
      {
        if (itr.le_nom() == nom) return itr;
        ind++;
      }
    Cerr << "TRUST_STLList : We have not found an object with name " << nom << finl;
    throw;
  }

  const _CLASSE_& operator[](const Nom& nom) const
  {
    assert (size() > 0);
    int ind = 0;
    for (auto& itr : list_)
      {
        if (itr.le_nom() == nom) return itr;
        ind++;
      }
    Cerr << "TRUST_STLList : We have not found an object with name " << nom << finl;
    throw;
  }

  _CLASSE_& operator()(int i) { return operator[](i); }
  const _CLASSE_& operator()(int i) const { return operator[](i); }
  _CLASSE_& operator()(const Nom& n) { return operator[](n); }
  const _CLASSE_& operator()(const Nom& n) const { return operator[](n); }

//  _CLASSE_& operator[](int i) = delete;
//  const _CLASSE_& operator[](int i) const = delete;
//  _CLASSE_& operator[](const Nom& nom) = delete;
//  const _CLASSE_& operator[](const Nom& nom) const = delete;
//
//  _CLASSE_& operator()(int i) = delete;
//  const _CLASSE_& operator()(int i) const = delete;
//  _CLASSE_& operator()(const Nom& n) = delete;
//  const _CLASSE_& operator()(const Nom& n) const = delete;

  TRUST_STLList& operator=(const _CLASSE_ &t)
  {
    list_ = t.list_;
    return *this;
  }

  TRUST_STLList& operator=(const TRUST_STLList& t)
  {
    list_ = t.list_;
    return *this;
  }

  int rang(const char* const ch) const
  {
    Nom nom(ch);
    int ind = 0;
    for (auto& itr : list_)
      {
        if (itr.le_nom() == nom) return ind;
        ind++;
      }
    return -1;
  }

  int contient(const char* const ch) const
  {
    Nom nom(ch);
    for (auto& itr : list_)
      if (itr.le_nom() == nom) return 1;
    return -1;
  }

  int contient(const Objet_U& obj) const
  {
    for (auto &itr : list_)
      if (itr == obj) return 1;
    return 0;
  }
};

#endif /* TRUST_List_included */
