/*****************************************************************************
*
* Copyright (c) 2011 - 2013, CEA
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CEA, nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <ArrOf__Scalar__.h>
__DoubleOnlyBegin__
#include <math.h>
__DoubleOnlyEnd__
__IntOnlyBegin__
// limits.h definit INT_MIN, SHRT_MIN, ...
#include <limits.h>
__IntOnlyEnd__
#include <stdlib.h>
#include <Objet_U.h>
#include <iostream>
#include <stdlib.h>

// ******************************************************************
//
//             Implementation des methodes de V__Scalar__data
//
// ******************************************************************
////////////////////////////////////////////////////////////////////
// .NOM        ArrOf__Scalar__
// .ENTETE     TRUST Math
// .LIBRAIRIE  libtmath
// .FILE       ArrOf__Scalar__.h
// .FILE       ArrOf__Scalar__.cpp
//
// .DESCRIPTION
// V__Scalar__data alloue une zone de memoire de la taille specifiee au
// constructeur, et libere la zone de memoire a la destruction.
// La memoire peut etre allouee sur le tas (avec new) ou par le
// mecanisme Memoire::add_trav___scalar__.
//
// "ref_count" compte le nombre de pointeurs qui font reference a "this".
// (permet au dernier utilisateur de l'objet de le detruire), voir
// ArrOf__Scalar__.
//
// .SECTION voir aussi
// .CONTRAINTES
// .INVARIANTS
// .HTML
// .EPS
///////////////////////////////////////////////////////////////////

class V__Scalar__data
{
public:
  V__Scalar__data(entier size, ArrOf__Scalar__::Storage storage);
  ~V__Scalar__data();
  entier          add_one_ref();
  entier          suppr_one_ref();
  __scalar__ *        get_data();
  const __scalar__ *  get_data() const;
  inline entier   ref_count() const;
  inline entier   get_size() const;
private:
  // Le constructeur par copie et l'operateur= sont interdits.
  V__Scalar__data(const V__Scalar__data& v);
  V__Scalar__data& operator=(const V__Scalar__data& v);

  // "data" est un pointeur sur une zone de memoire de taille
  // sz * sizeof(__scalar__), allouee par le
  // constructeur et liberee par le destructeur.
  // Ce pointeur n'est jamais nul meme si size_==0
  __scalar__ * data_;
  // Compteur incremente par add_one_ref et decremente par suppr_one_ref.
  // Contient le nombre d'objets ArrOf__Scalar__ dont le membre "p" pointe
  // vers "this". On a ref_count_ >= 0.
  entier ref_count_;
  // "sz" est la taille du tableau "data_" alloue
  // On a sz >= 0.
  entier size_;
  // Si storage est de type TEMP_STORAGE, d_ptr_trav porte la reference
  // a la zone allouee, sinon le pointeur est nul.
  //__Scalar___ptr_trav * d_ptr_trav_;
};


// Description:
//    Construit un V__Scalar__data de taille size >= 0
// Parametre: entier s
//    Signification: taille du V__Scalar__data, il faut size >= 0
// Parametre: Storage storage
//    Signification: indique si la memoire doit etre allouee
//                   avec "new" ou avec "memoire.add_trav___scalar__()"
//    Valeurs par defaut: STANDARD (allocation avec "new")
// Postcondition:
//    data_ n'est jamais nul, meme si size==0
V__Scalar__data::V__Scalar__data(entier size, ArrOf__Scalar__::Storage storage)
{
  if (size == 0)
    storage = ArrOf__Scalar__::STANDARD;

  switch (storage)
    {
    case ArrOf__Scalar__::STANDARD:
      {
#ifdef _EXCEPTION_
        // Allocation de la memoire sur le tas
        try
          {
            data_ = new __scalar__[size];
          }
        catch(...)
          {
            Cerr << "impossible d'allouer " << size << " __scalar__ " << finl;
            throw;
          }
#else
        data_ = new __scalar__[size];
        if(!data_)
          {
            Cerr << "impossible d'allouer " << size << "__scalar__ " << finl;
            throw ;
          }
#endif
        break;
      }
    default:
      throw;
    }
  ref_count_ = 1;
  size_ = size;

  assert(data_ != 0);
}

// Description:
//  Detruit la zone de memoire allouee.
// Precondition:
//  ref_count == 0 (la zone de memoire ne doit etre referencee nulle part)
V__Scalar__data::~V__Scalar__data()
{
  assert(ref_count_ == 0);

  // Stockage STANDARD
  delete[] data_;

  data_ = 0;  // paranoia: si size_==-1 c'est qu'on pointe sur un zombie
  size_ = -1; //  (pointeur vers un objet qui a ete detruit)
}

// Description: renvoie ref_count_
inline entier V__Scalar__data::ref_count() const
{
  return ref_count_;
}

// Description: renvoie size_
inline entier V__Scalar__data::get_size() const
{
  return size_;
}

// Description:
//     Un nouveau tableau utilise cette zone memoire :
//     incremente ref_count
// Retour: int
//    Signification: ref_count
inline entier V__Scalar__data::add_one_ref()
{
  return ++ref_count_;
}

// Description:
//     Un tableau de moins utilise cette zone memoire
//     decremente ref_count
// Precondition:
//     ref_count_ > 0
// Retour: int
//    Signification: ref_count
inline entier V__Scalar__data::suppr_one_ref()
{
  assert(ref_count_ > 0);
  return (--ref_count_);
}

// Description: renvoie data_
inline __scalar__ * V__Scalar__data::get_data()
{
  return data_;
}

// Description: renvoie data_
inline const __scalar__ * V__Scalar__data::get_data() const
{
  return data_;
}

// Description: Constructeur par copie. Interdit : genere une erreur !
V__Scalar__data::V__Scalar__data(const V__Scalar__data& v)
{
  Cerr << "Erreur dans V__Scalar__data::V__Scalar__data(const V__Scalar__data & v)" << finl;
  throw;
}

// Description: Operateur= interdit. Genere une erreur !
V__Scalar__data& V__Scalar__data::operator=(const V__Scalar__data& v)
{
  Cerr << "Erreur dans V__Scalar__data::operator=(const V__Scalar__data & v)" << finl;
  throw;
  return *this;
}

// ******************************************************************
//
//             Implementation des methodes de ArrOf__Scalar__
//
// ******************************************************************


// Definition des constantes pour les options de memory_resize
const entier ArrOf__Scalar__::COPY_OLD = 1;
const entier ArrOf__Scalar__::INITIALIZE_NEW = 2;

// Description:
//  Destructeur : appelle detach_array()
ArrOf__Scalar__::~ArrOf__Scalar__()
{
  detach_array();
  size_array_ = -1; // Paranoia: si size_array_==-1, c'est un zombie
}

// Description:
//  Constructeur par defaut: cree un tableau "detache",
//  soit p_==0, data_==0, size_array_==0, smart_resize_==0
ArrOf__Scalar__::ArrOf__Scalar__() :
  p_(0),
  data_(0),
  size_array_(0),
  memory_size_(0),
  smart_resize_(0),
  storage_type_(STANDARD)
{
}

// Description:
//     Cree un tableau de taille n avec allocation standard
//     (voir set_mem_storage).
//     Valeur de remplissage par defaut: voir fill_default_value
// Precondition:
// Parametre: entier n
//    Signification: taille du tableau
ArrOf__Scalar__::ArrOf__Scalar__(entier n) :
  p_(new V__Scalar__data(n, STANDARD)),
  data_(p_->get_data()),
  size_array_(n),
  memory_size_(n),
  smart_resize_(0),
  storage_type_(STANDARD)
{
  if (n > 0)
    fill_default_value(0, n);
}

// Description:
//     Cree un tableau de taille n
//     toutes les cases sont initialisees a x
// Precondition:
// Parametre: entier n
//    Signification: taille du tableau
// Parametre: __scalar__ x
//    Signification: valeur pour initialiser le tableau
ArrOf__Scalar__::ArrOf__Scalar__(entier n, __scalar__ x) :
  p_(new V__Scalar__data(n, STANDARD)),
  data_(p_->get_data()),
  size_array_(n),
  memory_size_(n),
  smart_resize_(0),
  storage_type_(STANDARD)
{
  *this = x;
}

// Description:
//     Constructeur par copie. On alloue une nouvelle zone de memoire
//     et on copie le contenu du tableau. L'attribut smart_resize_ est
//     copie aussi.
//     Si le tableau A est de taille nulle, on cree un tableau "detache",
//     sinon on cree un tableau "normal".
// Parametre: const ArrOf__Scalar__& A
//    Signification: le tableau a copier
ArrOf__Scalar__::ArrOf__Scalar__(const ArrOf__Scalar__& A)
{
  const entier size = A.size_array();
  if (size > 0)
    {
      // Creation d'un tableau "normal"
      storage_type_ = STANDARD;
      p_ = new V__Scalar__data(size, STANDARD);
      data_ = p_->get_data();
      size_array_ = size;
      memory_size_ = size;
      smart_resize_ = A.smart_resize_;
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

// Description:
//   Change le mode d'allocation memoire lors des resize
//   (voir V__Scalar__data et __Scalar___ptr_trav)
//   Exemple pour creer un tableau avec allocation temporaire:
//    DoubleTab tab; // Creation d'un tableau vide
//    tab.set_mem_storage(TEMP_STORAGE); // Changement de mode d'allocation
//    tab.resize(n); // Allocation memoire
void ArrOf__Scalar__::set_mem_storage(const Storage storage)
{
  storage_type_ = storage;
}

// Description:
//   Renvoie le mode d'allocation du tableau (qui sera utilise
//   lors du prochain resize si changement de taille).
//   (voir V__Scalar__data et __Scalar___ptr_trav)
enum ArrOf__Scalar__::Storage ArrOf__Scalar__::get_mem_storage() const
{
  return storage_type_;
}

// Description:
//   Change le mode l'allocation memoire: reallocation d'un tableau
//   a chaque changement de taille (flag = 0) ou reallocation
//   uniquement si la taille augmente et par doublement de la taille
//   du tableau (flag = 1).
void ArrOf__Scalar__::set_smart_resize(entier flag)
{
  assert(flag == 0 || flag == 1);
  smart_resize_ = flag;
}

// Description:
//    Remet le tableau dans l'etat obtenu avec le constructeur par defaut
//    (libere la memoire mais conserve le mode d'allocation memoire actuel)
void ArrOf__Scalar__::reset()
{
  detach_array();
}

// Description:
//    Copie les donnees du tableau m.
//    Si "m" n'a pas la meme taille que "*this", on fait un resize_array.
//    Ensuite, on copie les valeurs de "m" dans "*this".
//    Le type de tableau (methode d'allocation) n'est pas copie.
// Precondition:
//    Si le tableau n'a pas la meme taille que "m", alors *this doit
//    etre "resizable" (ne pas etre de type "ref_data" et "ref_count == 1")
// Parametre: const ArrOf__Scalar__& m
//    Signification: la tableau a copier
// Retour:  ArrOf__Scalar__&
//    Signification: *this
ArrOf__Scalar__& ArrOf__Scalar__::operator=(const ArrOf__Scalar__& m)
{
  if (&m != this)
    {
      const entier new_size = m.size_array();
      // Le code suivant est quasiment une copie de ArrOf__Scalar__::resize()
      // SAUF: memory_resize est appele avec NO_INITIALIZE (la zone de memoire
      //  n'est pas initialisee)
      if (new_size != size_array())
        {
          if ((smart_resize_ == 0) || (new_size > memory_size_))
            memory_resize(new_size, 0); // Pas d'initialisation
          size_array_ = new_size;
        }
      inject_array(m);
    }
  return *this;
}


// Description:
//     x est affecte a toutes les cases
// Precondition:
// Parametre: __scalar__ x
//    Signification: la valeur a affecter a toutes les cases du tableau
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: ArrOf__Scalar__&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
ArrOf__Scalar__& ArrOf__Scalar__::operator=(__scalar__ x)
{
  const entier n = size_array();
  __scalar__ *data = addr();
  for (entier i = 0; i < n; i++)
    {
      data[i] = x;
    }
  return *this;
}

// Description: appelle operator=(a)
ArrOf__Scalar__& ArrOf__Scalar__::copy_array(const ArrOf__Scalar__ & a)
{
  operator=(a);
  return *this;
}

// Description:
//  Si besoin, alloue une nouvelle zone de memoire,
//  copie les donnees et efface l'ancienne zone de memoire.
//  Attention, on suppose que cette methode est appelee par
//  resize_array().
//  Attention: si ref_count_>1, l'appel a resize_array() est
//  autorise uniquement si la nouvelle taille est identique
//  a la precedente.
// Precondition:
//  Le tableau doit etre de type "detache" ou "normal" avec
//  ref_count==1, et il faut new_size >= 0
//  On suppose que size_array contient encore le nombre d'elements
//  valides avant changement de taille.
// Parametre: new_size
//  Signification: nouvelle taille demandee pour le tableau.
// Parametre: options
//  Signification: COPY_OLD => on recopie les anciennes donnees dans le nouveau
//                  tableau (jusqu'au max de l'ancienne et de la nouvelle taille).
//                 INITIALIZE_NEW => initialisation des cases non copiees
// Postcondition:
//  p_ et data_ sont mis a jour, mais pas size_array_ !!!
//  (on suppose que c'est fait dans resize_array()).
//  Si la nouvelle taille est nulle, on detache le tableau.
void  ArrOf__Scalar__::memory_resize(entier new_size, entier options)
{
  assert(new_size >= 0);

  // Occupation memoire de l'ancien tableau:
  entier old_mem_size = 0;
  if (p_)
    old_mem_size = p_->get_size();

  // Occupation memoire du nouveau tableau :
  // Si smart_resize, on prend au moins deux fois la taille
  // precedente, ou new_size
  entier new_mem_size = new_size;
  if (smart_resize_ && (old_mem_size * 2 > new_size))
    new_mem_size = old_mem_size * 2;

  if (new_mem_size != old_mem_size)
    {
      // detach_array() efface le contenu de size_array_. On le met de cote:
      const entier old_size_array = size_array_;
      // On va reellement changer l'adresse du tableau.
      // Il ne faut pas qu'il existe d'autre reference a ce tableau.
      assert(data_ == 0 || (p_ != 0 && ref_count() == 1));
      if (new_mem_size == 0)
        {
          // La nouvelle taille est nulle, on cree un tableau "detache"
          detach_array();
        }
      else
        {
          // Allocation d'une nouvelle zone
          V__Scalar__data * new_p = new V__Scalar__data(new_mem_size, storage_type_);
          __scalar__ * new_data = new_p->get_data();
          // Raccourci si le tableau etait "detache", inutile de copier
          // les anciennes donnees. On copie si COPY_OLD est demande
          entier copy_size = 0;
          if (data_ != 0)
            {
              // Calcul du nombre d'elements a copier vers la nouvelle
              // zone de memoire : c'est le min de l'ancienne et de
              // la nouvelle taille.
              if (options & COPY_OLD)
                {
                  copy_size = size_array_;
                  if (new_size < copy_size)
                    copy_size = new_size;
                  // Copie des valeurs dans le nouveau tableau
                  for (entier i = 0; i < copy_size; i++)
                    new_data[i] = data_[i];
                }
              // Destruction de l'ancienne zone (si plus aucune reference)
              detach_array();
            }
          // On attache la nouvelle zone de memoire
          p_ = new_p;
          data_ = new_data;
          memory_size_ = new_mem_size;
          // Initialisation des cases supplementaires avec une valeur par defaut
          if (options & INITIALIZE_NEW)
            fill_default_value(copy_size, new_mem_size - copy_size);
          // Restaure l'ancienne valeur de size_array_
          size_array_ = old_size_array;
        }
    }
}

// Description:
//  Remplit "nb" cases consecutives du tableau a partir de la case "first"
//  avec une valeur par defaut.
//  Cette fonction est appelee lors d'un resize pour initialiser les
//  cases nouvellement creees.
//  Le comportement depend actuellement du type de tableau :
//  * Tableau de type "smart_resize":
//    * en mode debug (macro NDEBUG non definie) le tableau est initialise
//      avec une valeur invalide.
//    * en optimise, le tableau n'est pas initialise
//  * Tableau normal :
//    Le tableau est initialise avec la valeur 0. Ce comportement est choisi
//    pour des raisons de compatibilite avec l'implementation precedente.
//    Cette specification pourrait etre modifiee prochainement pour des raisons
//    de performances (pour ne pas avoir a initialiser inutilement les tableaux).
//    DONC: il faut supposer desormais que les nouvelles cases ne sont pas
//    initialisees lors d'un resize.
// Parametre: first
//  Signification: premiere case a initialiser.
//  Contrainte:    (nb==0) ou (0 <= first < memory_size_)
// Parametre: nb
//  Signification: nombre de cases a initialiser.
//  Contrainte:    (nb==0) ou (0 < nb <= memory_size_ - first)
void  ArrOf__Scalar__::fill_default_value(entier first, entier nb)
{
  assert((nb == 0) || (first >= 0 && first < memory_size_));
  assert((nb == 0) || (nb > 0 && nb <= memory_size_ - first));
  __scalar__ * data = addr();
  assert(data!=0 || nb==0);
  data += first;
  if (smart_resize_)
    {
      // On initialise uniquement en mode debug
#ifndef NDEBUG
      __IntOnlyBegin__
      static const entier ENTIER_INVALIDE = INT_MIN;
      for (entier i = 0; i < nb; i++)
        data[i] = ENTIER_INVALIDE;
      __IntOnlyEnd__
      __DoubleOnlyBegin__
      // Ceci represente un NAN. N'importe quelle operation avec ca fait encore un NAN.
      // Si c'est pas portable, on peut remplacer par DMAX_FLOAT sur les autres machines.
      static const unsigned long long  VALEUR_INVALIDE =
        0x7ff7ffffffffffffULL;

      // On utilise "memcpy" et non "=" car "=" peut provoquer une exception
      // si la copie passe par le fpu.
      for (entier i = 0; i < nb; i++)
        memcpy(data + i, & VALEUR_INVALIDE, sizeof(__scalar__));
      __DoubleOnlyEnd__
#endif
    }
  else
    {
      // Comportement pour les tableaux normaux : compatibilite avec la
      // version precedente : on initialise avec 0.
      for (entier i = 0; i < nb; i++)
        data[i] = (__scalar__) 0;
    }
}

// ****************************************************************
//
//         Fonctions non membres de la classe ArrOf__Scalar__
//
// ****************************************************************

// Description:
//  Renvoie 1 si les tableaux "v" et "a" sont de la meme taille
//  et contiennent les memes valeurs au sens strict, sinon renvoie 0.
//  Le test est !(v[i]!=a[i])
entier operator==(const ArrOf__Scalar__& v, const ArrOf__Scalar__& a)
{
  const entier n = v.size_array();
  const entier na = a.size_array();
  entier resu = 1;
  if (n != na)
    {
      resu = 0;
    }
  else
    {
      const __scalar__* vv = v.addr();
      const __scalar__* av = a.addr();
      entier i;
      for (i = 0; i < n; i++)
        {
          if (av[i] != vv[i])
            {
              resu = 0;
              break;
            }
        }
    }
  return resu;
}

// Description:
//    Retourne l'indice du min ou -1 si le tableau est vide
// Precondition:
// Parametre: const ArrOf__Scalar__& dx
//    Signification: tableau a utiliser
// Retour: int
//    Signification: indice du min
entier imin_array(const ArrOf__Scalar__& dx)
{
  entier indice_min = -1;
  const entier size = dx.size_array();
  if (size > 0)
    {
      indice_min = 0;
      __scalar__ valeur_min = dx[0];
      for(entier i = 1; i < size; i++)
        {
          const __scalar__ val = dx[i];
          if(val < valeur_min)
            {
              indice_min = i;
              valeur_min = val;
            }
        }
    }
  return indice_min;
}

// Description:
//    Retourne l'indice du max ou -1 si le tableau est vide
// Precondition:
// Parametre: const ArrOf__Scalar__& dx
//    Signification: tableau a utiliser
// Retour: int
//    Signification: indice du max
entier imax_array(const ArrOf__Scalar__& dx)
{
  entier indice_max = -1;
  const entier size = dx.size_array();
  if (size > 0)
    {
      indice_max = 0;
      __scalar__ valeur_max = dx[0];
      for(entier i = 1; i < size; i++)
        {
          const __scalar__ val = dx[i];
          if(val > valeur_max)
            {
              indice_max = i;
              valeur_max = val;
            }
        }
    }
  return indice_max;
}

// Description:
//    Retourne la valeur minimale
// Precondition:
//    Le tableau doit contenir au moins une valeur
// Parametre: const ArrOf__Scalar__& dx
//    Signification: tableau a utiliser
// Retour: __scalar__
//    Signification: valeur du min
__scalar__ min_array(const ArrOf__Scalar__& dx)
{
  const entier size = dx.size_array();
  assert(size > 0);
  __scalar__ valeur_min = dx[0];
  for(entier i = 1; i < size; i++)
    {
      const __scalar__ val = dx[i];
      if (val < valeur_min)
        valeur_min = val;
    }
  return valeur_min;
}

// Description:
//    Retourne la valeur maximale
// Precondition:
//    Le tableau doit contenir au moins une valeur
// Parametre: const ArrOf__Scalar__& dx
//    Signification: tableau a utiliser
// Retour: __scalar__
//    Signification: valeur du max
__scalar__ max_array(const ArrOf__Scalar__& dx)
{
  const entier size = dx.size_array();
  assert(size > 0);
  __scalar__ valeur_max = dx[0];
  for(entier i = 1; i < size; i++)
    {
      const __scalar__ val = dx[i];
      if (val > valeur_max)
        valeur_max = val;
    }
  return valeur_max;
}

// Description:
//   Fonction de comparaison utilisee pour trier le tableau
//   dans ArrOf__Scalar__::trier(). Voir man qsort
static int fonction_compare_arrof__scalar___ordonner(const void * data1, const void * data2)
{
  const __scalar__ x = *(const __scalar__*)data1;
  const __scalar__ y = *(const __scalar__*)data2;
  __DoubleOnlyBegin__
  if (x < y)
    return -1;
  else if (x > y)
    return 1;
  else
    return 0;
  __DoubleOnlyEnd__
  __IntOnlyBegin__
  return x - y;
  __IntOnlyEnd__
}

// Description:
//   Tri des valeurs du tableau dans l'ordre croissant.
//   La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
void ArrOf__Scalar__::ordonne_array()
{
  const entier size = size_array();
  if (size > 1)
    {
      __scalar__ * data = addr();
      qsort(data, size, sizeof(__scalar__),
            fonction_compare_arrof__scalar___ordonner);
    }
}

// Description:
//    Fait pointer le tableau vers les memes donnees qu'un tableau
//    existant. Le tableau sera du meme type que le tableau m ("detache",
//    "normal"). Le tableau m ne doit pas etre de type "ref_data"
//    Les donnes existantes sont perdues si elles
//    ne sont pas referencees ailleurs.
// Precondition:
// Parametre: const ArrOf__Scalar__& m
//    Signification: le tableau a referencer (pas de type "ref_data"
//                   et different de *this !!!)
// Retour: ArrOf__Scalar__&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
ArrOf__Scalar__& ArrOf__Scalar__::ref_array(const ArrOf__Scalar__& m)
{
  assert(&m != this);
  // La condition 'm n'est pas de type "ref_data"' est necessaire pour
  // attach_array().
  detach_array();
  attach_array(m);
  return *this;
}

// Description:
//    Fait pointer le tableau vers la zone de memoire "data_".
//    On detache la zone de memoire existante. Le tableau devient
//    de type "ref_data". Attention : ptr doit etre non nul.
//    La taille est initialisee avec size.
//    Cette methode est appelee notamment par __Scalar__Vect::adopter.
// Parametre: __scalar__*
//    Signification: le tableau a recuperer. Si pointeur nul alors size
//                   doit etre nulle aussi et le tableau reste detache
// Parametre: entier size
//    Signification: le nombre d'elements du tableau.
// Retour: ArrOf__Scalar__&
//    Signification: *this
ArrOf__Scalar__& ArrOf__Scalar__::ref_data(__scalar__* ptr, entier size)
{
  assert(ptr != 0 || size == 0);
  assert(size >= 0);
  detach_array();
  data_ = ptr;
  size_array_ = size;
  return *this;
}

// Description:
//  Amene le tableau dans l'etat "detache". C'est a dire:
//  Si le tableau est "detache" :
//   * ne rien faire
//  Si le tableau est "normal" :
//   * decremente le nombre de references a *p
//   * detruit *p si p->ref_count==0
//   * annule p_, data_ et size_array_
//  Si le tableau est "ref_data" :
//   * annule data_ et size_array_
// Retour: int
//    Signification: 1 si les donnees du tableau ont ete supprimees
// Precondition:
// Postcondition:
//  On a p_==0, data_==0 et size_array_==0, memory_size_ = 0
//  L'attribut smart_resize_ est conserve.
entier ArrOf__Scalar__::detach_array()
{
  entier retour = 0;
  if (p_)
    {
      // Le tableau est de type "normal"
      // Si la zone de memoire n'est plus utilisee par personne,
      // on la detruit.
      if ((p_->suppr_one_ref()) == 0)
        {
          delete p_;
          retour = 1;
        }
      p_ = 0;
    }
  data_ = 0;
  size_array_ = 0;
  memory_size_ = 0;
  return retour;
}

// Description:
//    Amene le tableau dans l'etat "normal", "detache" ou "ref_array"
//    en associant la meme zone de memoire que le tableau m.
// Precondition:
//    Le tableau doit etre "detache"
// Parametre: const ArrOf__Scalar__& m
//    Signification: tableau a utiliser
//                   le tableau doit etre different de *this !!!
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//    Si m est detache, le tableau reste detache,
//    si m est "ref_array", le tableau devient "ref_array",
//    sinon le tableau est "normal", avec ref_count > 1
//    Si m est de taille nulle, le tableau reste detache + Warning dans fichier .log
void ArrOf__Scalar__::attach_array(const ArrOf__Scalar__& m)
{
  // Le tableau doit etre detache
  assert(data_ == 0 && p_ == 0);
  // Le tableau doit etre different de *this
  assert(&m != this);

  if (m.size_array() > 0)
    {
      p_ = m.p_;
      if (p_)
        p_->add_one_ref();
      data_ = m.data_;
      size_array_ = m.size_array_;
      memory_size_ = m.memory_size_;
      smart_resize_ = m.smart_resize_;
    }
  else
    {
      // Cas particulier ou on attache un tableau de taille nulle:
      //  en theorie, c'est pareil qu'un tableau de taille non nulle, MAIS
      //  dans les operateurs (ex:Op_Dift_VDF_Face_Axi), une ref est construite
      //  avant que le tableau ne prenne sa taille definitive. Donc, pour ne pas
      //  empecher le resize, il ne faut pas attacher le tableau s'il n'a pas
      //  encore la bonne taille. Solution propre: reecrire les operateurs pour
      //  qu'ils ne prennent pas une ref avant que le tableau ne soit valide
      //  et faire p_ = m.p_ dans tous les cas.
    }
}

// Description:
//    Copie les elements source[first_element_source + i]
//    dans les elements  (*this)[first_element_dest + i] pour 0 <= i < nb_elements
//    Les autres elements de (*this) sont inchanges.
// Precondition:
// Parametre:       const ArrOf__Scalar__& m
//  Signification:   le tableau a utiliser, doit etre different de *this !
// Parametre:       entier nb_elements
//  Signification:   nombre d'elements a copier, nb_elements >= -1.
//                   Si nb_elements==-1, on copie tout le tableau m.
//  Valeurs par defaut: -1
// Parametre:       entier first_element_dest
//  Valeurs par defaut: 0
// Parametre:       entier first_element_source
//  Valeurs par defaut: 0
// Retour: ArrOf__Scalar__&
//    Signification: *this
//    Contraintes:
// Exception:
//    Sort en erreur si la taille du tableau m est plus grande que la
//    taille de tableau this.
// Effets de bord:
// Postcondition:
ArrOf__Scalar__& ArrOf__Scalar__::inject_array(const ArrOf__Scalar__& source,
    entier nb_elements,
    entier first_element_dest,
    entier first_element_source)
{
  assert(&source != this);
  assert(nb_elements >= -1);
  assert(first_element_dest >= 0);
  assert(first_element_source >= 0);

  if (nb_elements < 0)
    nb_elements = source.size_array();

  assert(first_element_source + nb_elements <= source.size_array());
  assert(first_element_dest + nb_elements <= size_array());

  if (nb_elements > 0)
    {
      __scalar__ * addr_dest = addr() + first_element_dest;
      const __scalar__ * addr_source = source.addr() + first_element_source;
      // memcpy(addr_dest , addr_source, nb_elements * sizeof(__scalar__));
      entier i;
      for (i = 0; i < nb_elements; i++)
        {
          addr_dest[i] = addr_source[i];
        }
    }
  return *this;
}

// Description:
//    Retourne le nombre de references des donnees du tableau
//    si le tableau est "normal", -1 s'il est "detache" ou "ref_data"
// Retour: int
//    Signification: ref_count_
entier ArrOf__Scalar__::ref_count() const
{
  if (p_)
    return p_->ref_count();
  else
    return -1;
}

// Description:
//    Addition case a case sur toutes les cases du tableau
// Precondition:
//    la taille de y doit etre au moins egale a la taille de this
// Parametre: const ArrOf__Scalar__& y
//    Signification: tableau a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: ArrOf__Scalar__&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
ArrOf__Scalar__& ArrOf__Scalar__::operator+=(const ArrOf__Scalar__& y)
{
  assert(size_array()==y.size_array());
  __scalar__* dx = addr();
  const __scalar__* dy = y.addr();
  const entier n = size_array();
  for (entier i=0; i<n; i++)
    dx[i] += dy[i];
  return *this;
}

// Description:
//     ajoute la meme valeur a toutes les cases du tableau
// Precondition:
// Parametre: const __scalar__ dy
//    Signification: valeur a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: ArrOf__Scalar__
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
ArrOf__Scalar__& ArrOf__Scalar__::operator+=(const __scalar__ dy)
{
  __scalar__ * data = addr();
  const entier n = size_array();
  for(entier i=0; i < n; i++)
    data[i] += dy;
  return *this;
}
// Description:
//    Soustraction case a case sur toutes les cases du tableau
// Parametre: const ArrOf__Scalar__& y
//    Signification: tableau de meme taille que *this
// Retour: ArrOf__Scalar__&
//    Signification: *this
ArrOf__Scalar__& ArrOf__Scalar__::operator-=(const ArrOf__Scalar__& y)
{
  const entier size = size_array();
  assert(size == y.size_array());
  __scalar__ * data = addr();
  const __scalar__ * data_y = y.addr();
  for (entier i=0; i < size; i++)
    data[i] -= data_y[i];
  return *this;
}


// Description:
//     soustrait la meme valeur a toutes les cases
// Retour: ArrOf__Scalar__ &
//    Signification: *this
ArrOf__Scalar__& ArrOf__Scalar__::operator-=(const __scalar__ dy)
{
  __scalar__ * data = addr();
  const entier n = size_array();
  for(entier i=0; i < n; i++)
    data[i] -= dy;
  return *this;
}

// Description:
//   Renvoie un pointeur sur le premier element du tableau.
//   Le pointeur est nul si le tableau est "detache".
//   Attention, l'adresse peut changer apres un appel
//   a resize_array(), ref_data, ref_array, ...
// Precondition:
// Retour: const __scalar__*
//   Signification: pointeur sur le premier element du tableau
const __scalar__* ArrOf__Scalar__::addr() const
{
  return data_;
}

// Description:
//   Renvoie un pointeur sur le premier element du tableau.
//   Le pointeur est nul si le tableau est "detache".
// Precondition:
// Retour: const __scalar__*
//    Signification: la zone memoire du tableau
__scalar__* ArrOf__Scalar__::addr()
{
  return data_;
}

__DoubleOnlyBegin__

// Description:
//    Retourne le max des abs(i)
// Precondition:
//    Le tableau doit contenir au moins une valeur
// Parametre: const ArrOf__Scalar__& dx
//    Signification: tableau a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: __scalar__
//    Signification: valeur du max des valeurs absolues
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
__scalar__ max_abs_array(const ArrOf__Scalar__& dx)
{
  const entier size = dx.size_array();
  assert(size > 0);
  __scalar__ valeur_max = fabs(dx[0]);
  for(entier i = 1; i < size; i++)
    {
      const __scalar__ val = fabs(dx[i]);
      if (val > valeur_max)
        valeur_max = val;
    }
  return valeur_max;
}

// Description:
//     muliplie toutes les cases par dy
// Retour: ArrOf__Scalar__ &
//    Signification: *this
ArrOf__Scalar__& ArrOf__Scalar__::operator*= (const __scalar__ dy)
{
  __scalar__ * data = addr();
  const entier n = size_array();
  for(entier i=0; i < n; i++)
    data[i] *= dy;
  return *this;
}


// Description:
//     divise toutes les cases par dy
// Retour: ArrOf__Scalar__ &
//    Signification: *this
ArrOf__Scalar__& ArrOf__Scalar__::operator/= (const __scalar__ dy)
{
  // En theorie: les deux lignes suivantes sont plus efficaces, mais
  //  cela produit des differences sur certains cas tests
  //  (Hyd_C_VEF_Smago et Lambda_var_VEF_turb). Ca veut dire qu'il y
  //  a un probleme autre part mais pour l'instant on laisse l'ancien
  //  codage.
  // const __scalar__ i_dy = 1. / dy;
  // operator*=(i_dy);

  __scalar__ * data = addr();
  const entier n = size_array();
  for(entier i=0; i < n; i++)
    data[i] /= dy;

  return *this;
}
__DoubleOnlyEnd__

__Scalar__Tab::__Scalar__Tab()
{
  nb_dim_ = 2;
  dimensions_[0] = 0;
  dimensions_[1] = 0;
}

__Scalar__Tab::__Scalar__Tab(const __Scalar__Tab& tab) :
  ArrOf__Scalar__(tab)
{
  nb_dim_ = tab.nb_dim_;
  dimensions_[0] = tab.dimensions_[0];
  dimensions_[1] = tab.dimensions_[1];
}
__Scalar__Tab::__Scalar__Tab(const entier i, const entier j) :
  ArrOf__Scalar__(i*j)
{
  nb_dim_ = 2;
  dimensions_[0] = i;
  dimensions_[1] = j;
}

__Scalar__Tab& __Scalar__Tab::operator=(const __Scalar__Tab& tab)
{
  ArrOf__Scalar__::operator=(tab);
  nb_dim_ = tab.nb_dim_;
  dimensions_[0] = tab.dimensions_[0];
  dimensions_[1] = tab.dimensions_[1];
  return *this;
}

void __Scalar__Tab::reset()
{
  ArrOf__Scalar__::reset();
  nb_dim_ = 2;
  dimensions_[0] = 0;
  dimensions_[1] = 0;
}
