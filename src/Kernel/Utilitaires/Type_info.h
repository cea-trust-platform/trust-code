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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Type_info.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Type_info_included
#define Type_info_included
#include <cstring>
#include <arch.h>
#include <assert.h>
class Objet_U;
class Nom;
class Noms;
class Sortie;
class Synonyme_info;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     modelise une information de type pour les Objet_U
// .SECTION voir aussi
//     Objet_U Nom
//////////////////////////////////////////////////////////////////////////////
class Type_info
{
public:
  ~Type_info();
  Type_info(const char* name, Objet_U* (*f)(), int nb_bases=0, const Type_info** bases=0);
  Type_info(const char* name,                  int nb_bases=0, const Type_info** bases=0);

  inline const Nom& name() const
  {
    return *name_;
  };
  inline int      can_cast(const Type_info* p) const;

  int     same(const Type_info* p) const;
  int     same(const Nom&) const;
  int     has_base(const Type_info* p, int direct=0) const;
  int     has_base(const Nom& , int direct=0) const;
  Sortie&    bases(Sortie&) const;
  Objet_U*   instance() const;
  int     instanciable() const;

  // Methodes statiques :
  static Sortie&           hierarchie(Sortie&) ;
  static int            est_un_type(const char*) ;
  static int            les_sous_types(const Nom&, Noms& sous_types);
  static int            les_sous_types(const Type_info&, Noms& sous_types);
  static const Type_info * type_info_from_name(const char * type_name);
  static Objet_U*          instance(const char* typ);

protected:

private:
  Type_info(Type_info&) {}; // Constructeur par copie invalide
  Type_info&    operator=(Type_info&); // Operateur= invalide
  void   ajouter_type(const Type_info& type_info);
  static int search_type_info_name(const char *nom, int& index);

  // Possible names (eg: A|B)
  const char* names_;
  // Name and its synonym
  mutable Nom * name_; // (eg: A)
  mutable Nom * synonym_name_; // (eg: B)
  // Object synonym:
  Synonyme_info* synonym_; // Synonym

  // Nombre de classes de base de cette classe
  int nb_bases_;
  // Liste des Type_info des classes de base de cette classe
  const Type_info** b;
  // Pointeur vers la methode statique "cree_instance" de la classe
  // (nul si la classe n'est pas instanciable)
  Objet_U* (*cree_instance)();

  // Liste des Type_info des classes declarees par declare_base/declare_instanciable
  // La liste est triee par ordre alphabetique (minuscules/majuscules indifferentes)
  static const Type_info** les_types;
  // Pour chaque type enregistre dans "les_types", s'il existe plusieurs classes
  // ayant le meme "nom", alors types_homonymes != 0.
  static int * types_homonymes;
  // Nombre de classes enregistrees dans "les_types" et "types_homonymes"
  static int nb_classes;
  // Taille memoire du tableau "les_types" et "types_homonymes"
  // (tableau redimensionne par blocs)
  static int les_types_memsize;
};

// Description:
//     Exploration de la hierarchie des types
//     retourne 1 si p pointe sur un sous type du type considere
// Precondition:
// Parametre: const Type_info* p
//    Signification: pointeur sur le type a tester
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si p pointe sur un sous type du type considere, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Type_info::can_cast(const Type_info* p) const
{
  return ( (same(p)) || (p->has_base(this)) );
}

#endif
