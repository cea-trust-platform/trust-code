/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Type_info.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Type_info.h>
#include <Noms.h>
#include <Synonyme_info.h>

// B.Mathieu, 08/2004
//  Le processus d'initialisation de ces membres statiques est tres
//  important : il faut absolument qu'ils soient initialises AVANT
//  le premier appel au constructeur Type_info::Type_info(...).
//  Or ce constructeur est appele pour initialiser le membre statique info_obj
//  de tous les Objet_U.
//  Risque de "static initialization ordre fiasco"
//    (voir http://www.parashift.com/c++-faq-lite/ctors.html   [10.11])
//  Pour l'instant c'est ok parce qu'on initialise avec une constante...

// Tableau de pointeurs sur les types enregistres lors de la construction
// des Type_info. Si plusieurs types on le meme nom (Type_info::n), alors
// on n'en enregistre qu'un seul dans Type_info::les_types.
const Type_info** Type_info::les_types=0;
// Pour chaque elements du tableau "les_types", ce nombre vaut 1 si le
// nom du type est commun a plusieurs types, 0 sinon.
// Voir "ajouter_type"
int * Type_info::types_homonymes=0;

int Type_info::nb_classes=0;
int Type_info::les_types_memsize=0;

static inline char char_uppercase(char c)
{
  if (c >= 'a' && c <= 'z')
    c += 'A' - 'a';
  return c;
}

static inline int strcmp_uppercase(const char *n1, const char *n2)
{
  int i = 0;
  unsigned char c1, c2;
  int delta;
  do
    {
      c1 = (unsigned char) char_uppercase(n1[i]);
      c2 = (unsigned char) char_uppercase(n2[i]);
      delta = c1 - c2;
      i++;
    }
  while ((delta == 0) && (c1 != 0) && (c2 != 0));
  return delta;
}

// GF pour liberer correctement la memoire il faut au moins detruire
// le Nom
Type_info::~Type_info()
{
  // On cherche ou retirer le type dans le tableau :
  int index;
  int existe_deja = search_type_info_name(name(), index);
  if (existe_deja)
    {
      for (int i = index; i<nb_classes-1; i++)
        {
          les_types[i] = les_types[i+1];
          types_homonymes[i] = types_homonymes[i+1];
        }
      nb_classes--;
    }
  if (name_)
    {
      delete name_;
      name_=0;
      delete synonym_;
      synonym_=0;
      delete synonym_name_;
      synonym_name_=0;
    }
  if ((nb_classes==0)&& (les_types_memsize!=0))
    {
      delete [] les_types;
      delete [] types_homonymes;
      les_types_memsize=0;
    }
}

// Description:
//  Recherche le type de nom "nom" dans la liste de types enregistres
//  par une recherche binaire.
//  On compare les chaines converties en majuscules.
//  On range dans "index" l'indice du type, s'il a ete trouve
//  et sinon l'indice du type juste apres (dans ce cas, on a
//   les_types[index-1]->n < nom < les_types[index]->n )
//  Si le type a ete trouve on renvoie 1, sinon 0.
int Type_info::search_type_info_name(const char *nom, int& index)
{
  assert(nom != 0);
  // [imin..imax] est l'intervalle ou se trouve l'index recherche
  int imin = 0;
  int imax = nb_classes;
  while (imax > imin)
    {
      // On a toujours milieu < imax
      int milieu = (imin + imax) / 2;
      int comparaison = strcmp_uppercase(nom, les_types[milieu]->name());
      if (comparaison == 0)
        {
          index = milieu;
          return 1;
        }
      if (comparaison < 0)
        {
          // nom < les_types[milieu]
          // l'index recherche est donc inferieur ou egal a "milieu"
          imax = milieu;
        }
      else
        {
          // nom > les_types[milieu]
          // l'index recherche est donc strictement superieur a "milieu"
          imin = milieu + 1;
        }
    }
  index = imax;
  return 0;
}
// Description:
//     Constructeur par un nom, un tableau de meres.
// Precondition:
// Parametre: const char* nom
//    Signification: le nom du type a creer
//    Valeurs par defaut:
//    Contraintes: nom != null
//    Acces: entree
// Parametre: int nb_base
//    Signification: le nombre de meres dans le tableau bases
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const Type_info* bases[]
//    Signification: le tableau specifiant les types de bases (meres) du type a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
//    Sort en erreur si le nom n'est pas defini (null)
// Effets de bord:
// Postcondition:
Type_info::Type_info(const char* un_nom, int nb_base, const Type_info** the_bases) :
  names_(un_nom),
  name_((Nom*)0),
  synonym_name_((Nom*) 0),
  nb_bases_(nb_base),
  b(the_bases),
  cree_instance(0)
{
  if(un_nom == 0)
    {
      cerr << "Type_info::Type_info(const char* nom,Objet_U* (*f)()...)\n";
      cerr << " Error : name == 0" << endl;
      assert(0);
      Process::exit();
    }
  ajouter_type(*this);
}

// Description:
//     Constructeur par un nom, une fonction et un tableau de meres.
//     La fonction permet de creer une instance du bon type.
// Precondition:
// Parametre: const char* nom
//    Signification: le nom du type a creer
//    Valeurs par defaut:
//    Contraintes: nom != null
//    Acces: entree
// Parametre: Objet_U* (*f)()
//    Signification: fonction pour creer une instance du type considere
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int nb_base
//    Signification: le nombre de meres dans le tableau bases[]
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const Type_info* bases[]
//    Signification: le tableau specifiant les types de bases (meres) du type a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
//    Sort en erreur si le nom donne n'est pas defini (null)
// Effets de bord:
// Postcondition:
Type_info::Type_info(const char* un_nom,
                     Objet_U* (*f)(),
                     int nb_base,
                     const Type_info** the_bases) :
  names_(un_nom),
  name_((Nom*) 0),
  synonym_name_((Nom*) 0),
  nb_bases_(nb_base),
  b(the_bases),
  cree_instance(f)
{
  if(un_nom == 0)
    {
      cerr << "Type_info::Type_info(const char* nom,Objet_U* (*f)()...)\n";
      cerr << " Error : name == 0" << endl;
      assert(0);
      Process::exit();
    }
  ajouter_type(*this);
}

// Description:
//  Methode statique appelee par les constructeurs de Type_info pour
//  ajouter un nouveau type a la liste des types enregistres.
//  Verifie que le nom du type n'existe pas encore.
void Type_info::ajouter_type(const Type_info& type_info)
{
  // Split type_info.names_ into A if | is found (eg: n=A|B)
  // and add a synonym B
  Nom A("");
  Nom B("");
  int synonym_found=0;
  int i = 0;
  unsigned char c;
  do // Start loop
    {
      c = type_info.names_[i];
      // Find a |
      if (c==124)
        {
          if (synonym_found==1)
            {
              Cerr << "More than 1 synonym found in " << type_info.names_ << finl;
              Cerr << "Not supported yet !" << finl;
              Process::exit();
            }
          else
            {
              synonym_found=1;
              i++;
              c = type_info.names_[i];
              if (c==0)
                {
                  Cerr << "Error in a classname which can't finished by | : " << type_info.names_ << finl;
                  Process::exit();
                }
            }
        }
      // Build the synonym name:
      if (synonym_found)
        B+=c;
      else
        A+=c;
      i++;
    }
  while (c!=0); // End loop

  name_ = new Nom(A);
  if (synonym_found)
    {
      //Commented cause too verbose:
      //cerr << "Keyword " << A << " has a synonym: " << B << endl;
      synonym_name_ = new Nom(B);
      synonym_ = new Synonyme_info(synonym_name_->getChar(),name_->getChar());
    }
  // Verifie qu'il y a assez de place dans le tableau :
  if (les_types_memsize <= nb_classes + 1)
    {
      static const int INCREMENT = 512;
      // Plus assez de place dans le tableau: on redimensionne.
      les_types_memsize += INCREMENT;
      const Type_info** nouveau = new const Type_info*[les_types_memsize];
      for (int j = 0; j < nb_classes; j++)
        nouveau[j] = les_types[j];
      delete[] les_types;
      les_types = nouveau;

      int * temp = new int[les_types_memsize];
      for (int j = 0; j < nb_classes; j++)
        temp[j] = types_homonymes[j];
      delete[] types_homonymes;
      types_homonymes = temp;
    }

  // On cherche ou mettre le type dans le tableau :
  int existe_deja=Synonyme_info::est_un_synonyme(type_info.name());
  if (existe_deja)
    {
      cerr<<" class "<<type_info.name()<<" already exists as a synonym which is forbidden!!!!"<<endl;
      Process::exit();
    }
  int index;
  existe_deja = search_type_info_name(type_info.name(), index);
  if (existe_deja)
    {
      types_homonymes[index] = 1;
      // GF: si on a un homonyme que la macro string_macro_trio fonctionne
      // on a un pb sauf pour les iterateurs
      if (strcmp(string_macro_trio("VECT",titi),"VECT"))
        {
          if (strncmp(type_info.name(),"Iterateur_",10))
            {
              cerr<<" type "<<type_info.name()<<" is in double and it is not allowed!!!!"<<endl;
              Process::exit();
            }
        }
    }
  else
    {
      // Ajout du type dans le tableau a l'indice "index":
      for (int j = nb_classes; j > index; j--)
        {
          les_types[j] = les_types[j-1];
          types_homonymes[j] = types_homonymes[j-1];
        }
      les_types[index] = &type_info;
      types_homonymes[index] = 0;
      nb_classes++;
    }
}

// Description:
//     Ecriture des bases du type considere sur un flot de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Type_info::bases(Sortie& os) const
{
  int i= nb_bases_;
  while(i--)
    os << b[i]->name() << " ";
  return os << finl;
}

// Description:
//     Ecriture de toute la hierarchie du type considere sur un flix de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Type_info::hierarchie(Sortie& os)
{
  os << "There is " << nb_classes << " classes:" << finl;
  int i= nb_classes;
  while(i--)
    {
      os << les_types[i]->name() << " inherits from ";
      les_types[i]->bases(os);
    }
  return os << flush;
}

// Description:
//     Instanciation d'un Objet_U du type indique
//     S'il existe une classe T dont le Type_info a
//     le nom typ, alors instance renvoie un pointeur
//     sur une nouvelle instance de T.
//     renvoie le pointeur nul sinon.
// Precondition:
// Parametre: const char* typ
//    Signification: chaine de caractere associee a un type
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Objet_U*
//    Signification: pointeur sur un nouvel Objet_U du type typ
//    Contraintes:
// Exception:
// Effets de bord:
//      Creation d'un nouvel Objet_U s'il existe une classe de type typ
// Postcondition:
Objet_U* Type_info::instance(const char* typ)
{
  const Type_info * le_type = type_info_from_name(typ);
  Objet_U * instance;
  if (le_type)
    instance = le_type->instance();
  else
    instance = 0;
  return instance;
}

// Description:
//  Cree une instance de la classe associee au type_info.
// Precondition:
//  La classe doit etre instanciable (sinon arret du programme).
// Effets de bord:
//  Creation d'un nouvel Objet_U
Objet_U* Type_info::instance() const
{
  if (cree_instance == 0)
    {
      Cerr << "Error in Type_info::instance()\n";
      Cerr << " The type " << name() << " is not instantiable" << finl;
      assert(0);
      Process::exit();
    }
  Objet_U * ainstance = (*cree_instance)();
  return ainstance;
}

// Description:
//     Test d'existence d'une classe du type indique
//     si il existe une classe T dont le Type_info a
//     le nom nom, alors est_un_type renvoie 1
//     renvoie le pointeur nul sinon.
// Precondition:
// Parametre: const char* nom
//    Signification: chaine de caractere associee a un type
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour (0 ou  1)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Type_info::est_un_type(const char* nom)
{
  const Type_info * type = type_info_from_name(nom);
  return (type != 0);
}

// Description:
//     Test d'appartenance d'un type dans les types de bases du type considere
//     si direct == 0
//     renvoie 1 si (*p) fait partie des bases de (*this)
//     renvoie 0 sinon.
//     si direct != 0
//     renvoie 1 si (*p) fait partie des bases de (*this)
//     ou des meres directes ou non de (*this)
//     renvoie 0 sinon.
// Precondition:
// Parametre: const Type_info* p
//    Signification: le pointeur sur le type a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int direct
//    Signification: 0 pour une recherche dans toute la hierarchie des bases, non nul pour une recherche directe
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: code de retour (0 ou 1)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Type_info::has_base(const Type_info* p, int direct) const
{
  //recherche de p->name() dans b
  // si trouve return 1
  // sinon si pas direct return 0;
  //        sinon recherche dans les bases de b
  // Modif de B. Mathieu: test uniquement sur l'adresse du type_info,
  // pas sur le nom du type...
  if (p == 0)
    {
      return 0;
    }
  else
    {
      if (p == this)
        {
          return 1;
        }
      else
        {
          for (int i = 0; i < nb_bases_; i++)
            if (b[i] == p)
              return 1;
          if (!direct)
            {
              // Verifier les ancetres de niveau superieur
              for (int i = 0; i < nb_bases_; i++)
                if (b[i]->has_base(p, direct)) return 1;
            }
        }
    }
  return 0;
}

// Description:
//     Test d'appartenance d'un type dans les types de bases du type considere
//     Le type a rechercher est identifie par son nom
//     si direct == 0
//     renvoie 1 si le type de nom name fait partie des bases de (*this)
//     renvoie 0 sinon.
//     si direct != 0
//     renvoie 1 si le type de nom name fait partie des bases de (*this)
//     ou des meres directes ou non de (*this)
//     renvoie 0 sinon.
// Precondition:
// Parametre: const Nom& name
//    Signification: le nom du type a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int direct
//    Signification: 0 pour une recherche dans toute la hierarchie des bases, non nul pour une recherche directe
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: code de retour (0 ou 1)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Type_info::has_base(const Nom& aname, int direct) const
{
  //recherche de aname dans b
  // si trouve return 1
  // sinon si pas direct return 0;
  //        sinon recherche dans les bases de b
  // Modif B. Mathieu: comparaison des adresses du Type_info uniquement,
  //                   pas du nom du type.

  const Type_info * type = type_info_from_name(aname);
  int resultat = has_base(type, direct);
  return resultat;
}

// Description:
//     Comparaison sur le nom d'un type
//     Retourne 1 si les chaines de caracteres des noms du type considere et du nom indique sont identiques
//     Retour 0 sinon
int Type_info::same(const Nom& other_name) const
{
  return strcmp(name(),other_name)==0;
}

// Description:
//   Renvoie 1 si this==p, 0 sinon.
int Type_info::same(const Type_info* p) const
{
  return (this == p);
}

// Description:
//     Donne les noms des sous-types, un type mere etant donne
// Precondition:
// Parametre: const Type_info& mere
//    Signification: le type sur lequel rechercher les sous-types
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: Noms& les_sous_types
//    Signification: les noms des sous-types
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: int
//    Signification: nombre de sous-types retournes
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Type_info::les_sous_types(const Type_info& mere, Noms& les_sous_types)
{
  int compteur=0;
  int i= nb_classes;
  // Modif B. Mathieu: name() ne renvoie plus un static.
  const Nom& nom_mere = mere.name();
  cerr << "---------" << (const char*) nom_mere << endl;
  while(i--)
    {
      if( les_types[i]->has_base(nom_mere, 0) )
        if (!les_types[i]->same(nom_mere))
          {
            compteur++;
          }
    }
  cerr << compteur << endl ;
  if(compteur==0) return 0;
  les_sous_types.dimensionner(compteur);
  compteur=0;
  i= nb_classes;
  while(i--)
    {
      if( les_types[i]->has_base(nom_mere, 0) )
        if (!les_types[i]->same(nom_mere))
          {
            les_sous_types[compteur++]=les_types[i]->name();
          }
    }
  return compteur;
}

// Description:
//     Donne les noms des sous-types, un type mere etant donne par son nom
// Precondition:
// Parametre: const Nom& type
//    Signification: le nom du type sur lequel rechercher les sous-types
//    Valeurs par defaut:
//    Contraintes: le nom doit correspondre a un type existant
//    Acces: entree
// Parametre: Noms& les_sous_types
//    Signification: les noms des sous-types
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: int
//    Signification: nombre de sous-types retournes
//    Contraintes:
// Exception:
//     Sort en erreur si le nom indique ne correspond pas a un type existant dans TRUST
// Effets de bord:
// Postcondition:
int Type_info::les_sous_types(const Nom& type, Noms& sous_types)
{
  if (!est_un_type(type))
    {
      Cerr << type << "is not a type recognized by TRUST Unitaire " << finl;
      Process::exit();
    }
  int i= nb_classes;
  while(i--)
    {
      const Type_info& mere = *les_types[i];
      if( les_types[i]->same(type) )
        {
          return les_sous_types(mere, sous_types);
        }
    }
  return 0;
}

// Description:
//  Methode statique qui renvoie un pointeur vers le Type_info
//  dont le nom est "type_name". Si type_name n'est pas un type,
//  renvoie un pointeur nul.

const Type_info * Type_info::type_info_from_name(const char * type_name)
{
  const Type_info * type_info = 0;
  if (type_name != 0)
    {
      int index;
      if (search_type_info_name(type_name, index))
        {
          if (types_homonymes[index] == 0)
            {
              type_info = les_types[index];
            }
          else
            {
              // Le type est enregistre mais le nom correspond
              // a plusieurs types...
              cerr << "const Type_info * Type_info::type_info_from_name(const char * type_name)\n";
              cerr << " The type " << type_name << " has several homonymous\n";
              cerr << " We doing as if the type is unknown..." << endl;
            }
        }
    }
  return type_info;
}

// Description:
//  Renvoie 1 si le type associe est instanciable (cree_instance non nul)
//  renvoie 0 sinon.
int Type_info::instanciable() const
{
  return (cree_instance != 0);
}

