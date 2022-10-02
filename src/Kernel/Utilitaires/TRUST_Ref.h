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

#ifndef TRUST_Ref_included
#define TRUST_Ref_included

#include <Ref_.h>

/*! @brief Soit une classe _CLASSE_ qui derive de Objet_U.
 *
 *   TRUST_Ref<_CLASSE_> est une petite classe template contenant un pointeur vers une instance de _CLASSE_.
 *
 *   Definition de la REF:
 *
 *   #include <TRUST_Ref.h>
 *   #include <_CLASSE_.h>
 *   TRUST_Ref<_CLASSE_>
 *
 */
// MACRO to replace REF(THECLASS) by Ref_THECLASS & keep previous syntax for some developers
#define REF2(_TYPE_) TRUST_Ref<_TYPE_>

template<typename _CLASSE_>
class TRUST_Ref: public Ref_
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    TRUST_Ref *xxx = new TRUST_Ref(*this);
    if (!xxx) Cerr << "Not enough memory " << finl, Process::exit();
    return xxx->numero();
  }

  static Objet_U* cree_instance()
  {
    TRUST_Ref *instan = new TRUST_Ref();
    if (!instan) Cerr << "Not enough memory " << finl, Process::exit();
    return instan;
  }

  Sortie& printOn(Sortie& os) const override { return Ref_::printOn(os); }
  Entree& readOn(Entree& is) override  { return Ref_::readOn(is); }

public:
  ~TRUST_Ref() { }

  TRUST_Ref() : Ref_() , pointeur_(0) { }
  TRUST_Ref(const _CLASSE_ &t) : Ref_(), pointeur_(0) { set_Objet_U_ptr((_CLASSE_*) &t); }
  TRUST_Ref(const TRUST_Ref& t) : Ref_(), pointeur_(0) { set_Objet_U_ptr(t.pointeur_); }

  const TRUST_Ref& operator=(const _CLASSE_& t)
  {
    set_Objet_U_ptr((_CLASSE_ *) &t);
    return *this;
  }

  const TRUST_Ref& operator=(const TRUST_Ref& t)
  {
    set_Objet_U_ptr(t.pointeur_);
    return *this;
  }

  operator const _CLASSE_& () const { return valeur(); }
  operator _CLASSE_& () { return valeur(); }

  inline const _CLASSE_& valeur() const
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return *pointeur_;
  }

  inline _CLASSE_& valeur()
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return *pointeur_;
  }

  inline const _CLASSE_* operator ->() const
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return pointeur_;
  }

  inline _CLASSE_* operator ->()
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return pointeur_;
  }

  const Type_info& get_info_ptr() const override
  {
    const Type_info *type_info = _CLASSE_::info();
    return *type_info; /* type de base accepte par la ref */
  }

protected:
  void set_Objet_U_ptr(Objet_U* objet) override
  {
    Objet_U_ptr::set_Objet_U_ptr(objet);
    /* Attention: cette conversion de type est non triviale. si le _TYPE_ est issu d'un heritage multiple. */
    if (objet) pointeur_ = (_CLASSE_*) objet;
    else pointeur_ = (_CLASSE_*) 0;
  }

private:
  _CLASSE_ *pointeur_;
};

// Le resultat de == est positif si r1 et r2 pointent sur le meme objet (meme cle), ou si les deux references sont nulles
template<typename _CLASSE_>
inline int operator ==(const TRUST_Ref<_CLASSE_>& r1, const TRUST_Ref<_CLASSE_>& r2)
{
  if ((!r1.non_nul()) && (!r2.non_nul()))
    return 1;
  if (r1.valeur().numero() == r2.valeur().numero())
    return 1;
  return 0;
}

#endif /* TRUST_Ref_included */
