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

#ifndef TRUST_Deriv_included
#define TRUST_Deriv_included

#include <Deriv_.h>
#include <Nom.h>

/*! @brief classe TRUST_Deriv
 *
 *   Soit une classe Type_base qui derive de Objet_U.
 *
 *   TRUST_Deriv<Type_base> est une petite classe template contenant un pointeur vers une instance de Type_base,
 *   ou de n'importe quelle classe derivee de Type_base qui est creee dynamiquement lors a l'execution.
 *
 *   Definition de la DERIV:
 *
 *   #include <TRUST_Deriv.h>
 *   #include <Type_base.h>
 *   TRUST_Deriv<Type_base>
 *
 *   Creation d'un objet de type DERIV(Type_base) :
 *    DERIV(Type_base) deriv_type; // deriv_type est encore un pointeur nul
 *
 *   On suppose que la classe Type_Derive derive de Type_base et est instanciable:
 *    class Type_Derive : Type_base
 *    {
 *        Declare_instanciable(Type_Derive);;
 *      ...
 *    };
 *
 *   Creation d'une instance de la classe Type_Derive qui derive de Type_base:
 *    deriv_type.typer("Type_Derive");
 *
 *   Acces a l'instance de Type_Derive:
 *    Type_Derive & objet_derive = deriv_type.valeur();
 *    const Type_Derive & objet_derive = deriv_type.valeur();
 *
 *   L'instance de Type_Derive est detruite si on appelle a nouveau "typer()"
 *   ou si l'objet deriv_type est detruit.
 *
 *   Exemples :
 *    Equation_base     Operateur_Diff_base  ... ( <=> Type_base )
 *    Navier_Stokes_std Op_Diff_VDF_var_Face     ( <=> Type_derive )
 *    Equation          Operateur_Diff           ( <=> DERIV(Type_base) )
 *
 */

// MACRO to replace DERIV(THECLASS) by Deriv_THECLASS & keep previous syntax for some developers
#define DERIV(_TYPE_) TRUST_Deriv<_TYPE_>

template<typename _CLASSE_>
class TRUST_Deriv: public Deriv_
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    TRUST_Deriv *xxx = new TRUST_Deriv(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

  static Objet_U* cree_instance()
  {
    TRUST_Deriv *instan = new TRUST_Deriv();
    if (!instan) Process::exit("Not enough memory !");
    return instan;
  }

  Sortie& printOn(Sortie& os) const override { return Deriv_::printOn(os); }
  Entree& readOn(Entree& is) override  { return Deriv_::readOn(is); }

  void set_O_U_Ptr(Objet_U *objet) override
  {
    O_U_Ptr::set_O_U_Ptr(objet);
    /* Attention: cette conversion de type est non triviale. si le _TYPE_ est issu d'un heritage multiple. */
    if (objet) pointeur_ = (_CLASSE_*) objet;
    else pointeur_ = (_CLASSE_*) 0;
  }

private:
  _CLASSE_ *pointeur_;

public:
  ~TRUST_Deriv() { detach(); }
  TRUST_Deriv() :  Deriv_(), pointeur_(0) { }
  TRUST_Deriv(const TRUST_Deriv& t) : Deriv_(), pointeur_(0)
  {
    if (t.non_nul()) recopie(t.valeur());
  }

  inline const _CLASSE_& valeur() const
  {
    assert(pointeur_ != 0);
    assert(get_O_U_Ptr_check() || 1);
    return *pointeur_;
  }

  inline _CLASSE_& valeur()
  {
    assert(pointeur_ != 0);
    assert(get_O_U_Ptr_check() || 1);
    return *pointeur_;
  }

  inline const _CLASSE_* operator ->() const
  {
    assert(pointeur_ != 0);
    assert(get_O_U_Ptr_check() || 1);
    return pointeur_;
  }

  inline _CLASSE_* operator ->()
  {
    assert(pointeur_ != 0);
    assert(get_O_U_Ptr_check() || 1);
    return pointeur_;
  }

  const TRUST_Deriv& operator=(const _CLASSE_& t)
  {
    if (pointeur_ != (&t))
      {
        detach();
        recopie(t);
      }
    return *this;
  }

  const TRUST_Deriv& operator=(const TRUST_Deriv& t)
  {
    if (pointeur_ != t.pointeur_)
      {
        detach();
        if (t.non_nul()) recopie(t.valeur());
      }
    return *this;
  }

  operator const _CLASSE_& () const { return valeur(); }
  operator _CLASSE_& () { return valeur(); }

  const Type_info& get_info_ptr() const override
  {
    const Type_info * type_info = _CLASSE_::info();
    return *type_info; /* type de base accepte par la ref */
  }
};

/* ======================================================= *
 * ======================================================= *
 * ======================================================= */

/*! @brief classe TRUST_Deriv_Objet_U est quasiment identique a TRUST_Deriv<Objet_U>
 *  sauf qu'elle ne contient pas les operateurs de conversion de DERIV(Objet_U) en Objet_U.
 *
 *  Il existe 3 methodes supplementaires :
 *
 *   - deplace(TRUST_Deriv_Objet_U& )
 *   - reprendre
 *   - sauvegarder
 *
 *   Utilisation :
 *   - TRUST_Deriv_Objet_U
 *   ou
 *   - DerObjU
 */
class TRUST_Deriv_Objet_U : public TRUST_Deriv<Objet_U>
{
protected:
  int duplique() const override
  {
    TRUST_Deriv_Objet_U *xxx = new TRUST_Deriv_Objet_U(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

  static Objet_U* cree_instance()
  {
    TRUST_Deriv_Objet_U *instan = new TRUST_Deriv_Objet_U();
    if (!instan) Process::exit("Not enough memory !");
    return instan;
  }

public:
  TRUST_Deriv_Objet_U() : TRUST_Deriv<Objet_U>() { }
  TRUST_Deriv_Objet_U(const TRUST_Deriv_Objet_U& t) : TRUST_Deriv<Objet_U>(t) { }
  TRUST_Deriv_Objet_U(const Objet_U& t) : TRUST_Deriv<Objet_U>() { recopie(t); }

  const TRUST_Deriv_Objet_U& operator=(const Objet_U& t)
  {
    detach();
    recopie(t);
    return *this;
  }

  const TRUST_Deriv_Objet_U& operator=(const TRUST_Deriv_Objet_U& t)
  {
    detach();
    if (t.non_nul()) recopie(t.valeur());
    else Deriv_::set_O_U_Ptr((Objet_U *) 0); /* Elie Saikali : Attention pas la classe mere ! */
    return *this;
  }

  int reprendre(Entree& is) override { return valeur().reprendre(is); }
  int sauvegarder(Sortie& os) const override { return valeur().sauvegarder(os); }

  /* Ici on a retire les operateurs de conversion implicite
   * Cette fonction detruit l'objet en reference s'il existe et le remplace par l'objet deriv_obj.valeur()
   * deriv_obj devient une reference nulle
   */
  void deplace(TRUST_Deriv_Objet_U& deriv_obj)
  {
    detach();
    Objet_U& objet = deriv_obj.valeur();
    set_O_U_Ptr(&objet);
    deriv_obj.set_O_U_Ptr((Objet_U*) 0);
  }
};

using DerObjU = TRUST_Deriv_Objet_U;

#endif /* TRUST_Deriv_included */
