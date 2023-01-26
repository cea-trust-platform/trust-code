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

#include <Synonyme_info.h>
#include <Objet_U_ptr.h>
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
class TRUST_Deriv: public Objet_U_ptr
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

  Sortie& printOn(Sortie& os) const override
  {
    const Objet_U * objet = get_Objet_U_ptr();
    if (objet)
      {
        os << objet->le_type() << finl;
        os << (*objet);
      }
    else os << "vide" << finl;

    return os;
  }

  Entree& readOn(Entree& is) override
  {
    detach(); // Efface l'objet existant
    static Nom nom_type; // static pour le pas creer un Objet_U a chaque fois
    is >> nom_type;
    Objet_U * objet = (Objet_U*) 0;
    if (nom_type != "vide")
      {
        objet = typer(nom_type);
        if (! objet) Process::exit();
      }

    set_Objet_U_ptr(objet);
    if (objet) is >> (*objet); // On lit

    return is;
  }

  void set_Objet_U_ptr(Objet_U *objet) override
  {
    Objet_U_ptr::set_Objet_U_ptr(objet);
    /* Attention: cette conversion de type est non triviale. si le _TYPE_ est issu d'un heritage multiple. */
    if (objet) pointeur_ = (_CLASSE_*) objet;
    else pointeur_ = (_CLASSE_*) 0;
  }

private:
  _CLASSE_ *pointeur_;

public:
  ~TRUST_Deriv() { detach(); }
  TRUST_Deriv() :  Objet_U_ptr(), pointeur_(0) { }
  TRUST_Deriv(const TRUST_Deriv& t) : Objet_U_ptr(), pointeur_(0)
  {
    if (t.non_nul()) recopie(t.valeur());
  }

  void detach()
  {
    Objet_U * ptr = get_Objet_U_ptr();
    if (ptr) delete ptr;
    set_Objet_U_ptr((Objet_U *) 0);
  }

  Objet_U * typer(const char * nom_type);

  int associer_(Objet_U& objet) override
  {
    Objet_U * ptr = get_Objet_U_ptr_check();
    assert(ptr != 0);
    int resu = ptr->associer_(objet);
    return resu;
  }

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

/*! @brief Essaie de creer une instance du type "type".
 *
 * si type n'est pas un type ou type n'est pas instanciable=>arret
 *    si type n'est pas un sous-type du type du pointeur=>retour 0
 *    si ok, renvoie l'adresse de l'objet cree.
 */
template<typename _CLASSE_>
Objet_U * TRUST_Deriv<_CLASSE_>::typer(const char * type)
{
  const Type_info * type_info = Type_info::type_info_from_name(type); // Type_info du type demande
  const Type_info& type_base = get_info_ptr(); // Type de base du DERIV

  if ( get_Objet_U_ptr()) detach();

  Objet_U * instance = 0;

  if (type_info == 0)
    {
      const Synonyme_info* syn_info= Synonyme_info::synonyme_info_from_name(type);

      if (syn_info!=0) return typer(syn_info->org_name_());
      else
        {
          Cerr << "Error in Deriv_::typer_(const char* const type)" << finl << type << " is not a type." << finl;
          Cerr << "Type required : derived from " << type_base.name() << finl << finl;
          Cerr << type << " is not a recognized keyword." << finl << "Check your data set." << finl;
          Nom nompb = type;
          if (nompb.find("TURBULENT")!=-1 )
            Cerr << "Since TRUST V1.8.0, turbulence models are in TrioCFD and not anymore in TRUST.\nTry using TrioCFD executable or contact trust support." << finl;
          Process::exit();
        }
    }
  if (! type_info->instanciable())
    {
      Cerr << "Error in Deriv_::typer_(const char* const type).\n" << type << " is not instanciable." << finl;
      Process::exit();
    }

  if (! type_info->has_base(&type_base))
    Cerr << "Error in Deriv_::typer_(const char* const type).\n " << type << " is not a subtype of " << type_base.name() << finl;
  else
    instance = type_info->instance(); // Cree une instance du type decrit dans type_info

  set_Objet_U_ptr(instance);
  return instance;
}

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
class TRUST_Deriv_Objet_U: public Objet_U_ptr
{
  Declare_instanciable_sans_constructeur(TRUST_Deriv_Objet_U);
protected:
  void set_Objet_U_ptr(Objet_U *objet) override;

private:
  Objet_U *pointeur_;

public:
  TRUST_Deriv_Objet_U();
  TRUST_Deriv_Objet_U(const TRUST_Deriv_Objet_U& t);
  TRUST_Deriv_Objet_U(const Objet_U& t);
  const Type_info& get_info_ptr() const override;
  const TRUST_Deriv_Objet_U& operator=(const Objet_U& t);
  const TRUST_Deriv_Objet_U& operator=(const TRUST_Deriv_Objet_U& t);
  int reprendre(Entree& is) override;
  int sauvegarder(Sortie& os) const override;
  void deplace(TRUST_Deriv_Objet_U& deriv_obj);
  void detach();

  Objet_U* typer(const char *nom_type);
  int associer_(Objet_U& objet) override;

  inline const Objet_U& valeur() const
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return *pointeur_;
  }

  inline Objet_U& valeur()
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return *pointeur_;
  }

  inline Objet_U* operator ->() const
  {
    assert(pointeur_ != 0);
    assert(get_Objet_U_ptr_check() || 1);
    return pointeur_;
  }
};

using DerObjU = TRUST_Deriv_Objet_U;

#endif /* TRUST_Deriv_included */
