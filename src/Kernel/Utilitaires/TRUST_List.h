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

#include <Motcle.h>
#include <liste.h>

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

// MACROs to replace LIST(THECLASS), LIST_CURSEUR(THECLASS), CONST_LIST_CURSEUR(THECLASS) by
// List_THECLASS, List_Curseur_THECLASS, Const_List_Curseur_THECLASS & keep previous syntax for some developers
#define LIST(_TYPE_) TRUST_List<_TYPE_>
#define LIST_CURSEUR(_TYPE_) TRUST_List_Curseur<_TYPE_>
#define CONST_LIST_CURSEUR(_TYPE_) TRUST_List_Curseur<_TYPE_>
#define STLLIST(_TYPE_) TRUST_STLList<_TYPE_>

template<typename _CLASSE_>
class TRUST_List: public liste
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    TRUST_List *xxx = new TRUST_List(*this);
    if (!xxx) Cerr << "Not enough memory " << finl, Process::exit();
    return xxx->numero();
  }

  static Objet_U* cree_instance()
  {
    TRUST_List *instan = new TRUST_List();
    if (!instan) Cerr << "Not enough memory " << finl, Process::exit();
    return instan;
  }

  Sortie& printOn(Sortie& os) const override { return liste::printOn(os); }
  Entree& readOn(Entree& is) override;

public:
  ~TRUST_List() { }
  TRUST_List() { }
  TRUST_List(const _CLASSE_ &t) : liste(t) { }
  TRUST_List(const TRUST_List& t) : liste(t) { }

  _CLASSE_& operator[](int i) { return static_cast<_CLASSE_&>(liste::operator[](i)); }
  const _CLASSE_& operator[](int i) const { return static_cast<const _CLASSE_&>(liste::operator[](i)); }
  _CLASSE_& operator[](const Nom& nom) { return static_cast<_CLASSE_&>(liste::operator[](nom)); }
  const _CLASSE_& operator[](const Nom& nom) const { return static_cast<const _CLASSE_&>(liste::operator[](nom)); }

  _CLASSE_& operator()(int i) { return operator[](i); }
  const _CLASSE_& operator()(int i) const { return operator[](i); }
  _CLASSE_& operator()(const Nom& n) { return operator[](n); }
  const _CLASSE_& operator()(const Nom& n) const { return operator[](n); }

  _CLASSE_& valeur() { return static_cast<_CLASSE_&>(liste::valeur()); }
  const _CLASSE_& valeur() const { return static_cast<const _CLASSE_&>(liste::valeur()); }

  TRUST_List& operator=(const _CLASSE_ &t) { return static_cast<TRUST_List&>(liste::operator=(t)); }
  _CLASSE_* operator ->() { return static_cast<_CLASSE_*>(liste::operator->()); }

  TRUST_List& operator=(const TRUST_List& a_list) { return static_cast<TRUST_List&>(liste::operator=(a_list)); }
  TRUST_List& add(const TRUST_List& a_list) {  return static_cast<TRUST_List&>(liste::add(a_list)); }
  _CLASSE_& add(const _CLASSE_& t) { return static_cast<_CLASSE_&>(liste::add((const Objet_U&) t)); }

  TRUST_List& inserer(const TRUST_List& a_list) { return static_cast<TRUST_List&>(liste::inserer(a_list)); }
  _CLASSE_& inserer(const _CLASSE_& t) { return static_cast<_CLASSE_&>(liste::inserer(t)); }

  void supprimer() { liste::supprimer(); }

  TRUST_List& operator +=(const TRUST_List& a_list) { return static_cast<TRUST_List&>(liste::operator+=(a_list)); }
  TRUST_List& operator +=(const _CLASSE_& t) { return static_cast<TRUST_List&>(liste::operator+=(t)); }

  _CLASSE_& add_if_not(const _CLASSE_& t) { return static_cast<_CLASSE_&>(liste::add_if_not(t)); }
  _CLASSE_& add_if_not(const char* const);

  TRUST_List& search(const _CLASSE_& t) const { return static_cast<TRUST_List&>(liste::search(t)); }
  TRUST_List& search(const char* const c) const { return static_cast<TRUST_List&>(liste::search(c)); }

  TRUST_List& suivant() { return static_cast<TRUST_List&>(liste::suivant()); }
  const TRUST_List& suivant() const { return static_cast<const TRUST_List&>(liste::suivant()); }

  TRUST_List& dernier() { return static_cast<TRUST_List&>(liste::dernier()); }
  const TRUST_List& dernier() const { return static_cast<const TRUST_List&>(liste::dernier()); }

  TRUST_List& operator -=(const _CLASSE_& t) { return static_cast<TRUST_List&>(liste::operator-=(t)); }

  int est_egal_a(const Objet_U&) const override { return 0; }

  int contient(const _CLASSE_& t) const { return liste::contient(t); }
  int contient(const char* const c) const { return liste::contient(c); }

  int rang(const _CLASSE_& t) const { return liste::rang(t); }
  void suppr(const _CLASSE_& t) { liste::suppr(t); }
};

template<typename _CLASSE_>
Entree& TRUST_List<_CLASSE_>::readOn(Entree& is)
{
  if (!est_vide())
    if (0) Cerr << "list not empty, append to it" << finl;

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

template<typename _CLASSE_>
_CLASSE_& TRUST_List<_CLASSE_>::add_if_not(const char *const t)
{
  Nom nom(t);
  if (est_vide())
    {
      _CLASSE_ to_add;
      to_add.nommer(nom);
      return add(to_add);
    }
  liste_curseur curseur = *this;
  while (curseur)
    {
      if (curseur.valeur().le_nom() == nom)
        return static_cast<_CLASSE_&>(curseur.valeur());
      if (curseur.list().est_dernier())
        {
          _CLASSE_ to_add;
          to_add.nommer(nom);
          return add(to_add);
        }
      ++curseur;
    }
  Cerr << "Error in a list for add_if_not " << finl;
  return static_cast<_CLASSE_&>(valeur());
}

/* ======================================================= *
 * ======================================================= *
 * ======================================================= */

template<typename _CLASSE_>
class TRUST_List_Curseur : public liste_curseur
{
public:
  TRUST_List_Curseur(const TRUST_List<_CLASSE_>& a_list) : liste_curseur(a_list) { }

  void operator=(const TRUST_List<_CLASSE_>& a_list) { liste_curseur::operator=(a_list); }

  _CLASSE_& valeur() { return static_cast<_CLASSE_&>(liste_curseur::valeur()); }
  const _CLASSE_& valeur() const { return static_cast<const _CLASSE_&>(liste_curseur::valeur()); }

  _CLASSE_* operator ->() { return static_cast<_CLASSE_*>(liste_curseur::operator ->()); }
  const _CLASSE_* operator ->() const { return static_cast<const _CLASSE_*>(liste_curseur::operator ->()); }

  TRUST_List<_CLASSE_>& list() { return static_cast<TRUST_List<_CLASSE_>&>(liste_curseur::list()); }
  const TRUST_List<_CLASSE_>& list() const { return static_cast<const TRUST_List<_CLASSE_>&>(liste_curseur::list()); }
};

#endif /* TRUST_List_included */
