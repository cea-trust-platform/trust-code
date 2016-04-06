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
// File:        ArrOfFloat.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <ArrOfFloat.h>
#include <Double.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(ArrOfFloat,"ArrOfFloat",Objet_U);

const int ArrOfFloat::grow_factor_num_ = 2;
const int ArrOfFloat::grow_factor_denom_ = 1;

// Description: Construit un tableau vide
ArrOfFloat::ArrOfFloat() :
  data_(0), size_(0), memory_size_(0)
{
}

// Description: Constructeur d'un tableau vide de taille n.
// Le tableau n'est pas initialise.
ArrOfFloat::ArrOfFloat(int n) :
  data_(0), size_(0), memory_size_(0)
{
  resize_array(n);
}

// Description: Constructeur par copie
ArrOfFloat::ArrOfFloat(const ArrOfFloat& array) : Objet_U(array),
  data_(0), size_(0), memory_size_(0)
{
  operator=(array);
}


// Description: Operateur copie (deep copy)
const ArrOfFloat& ArrOfFloat::operator=(const ArrOfFloat& array)
{
  if (&array == this) // Self assignment, why not...
    return *this;
  if (array.size_ > memory_size_)
    {
      if (data_)
        delete [] data_;
      data_ = new float[array.size_];
      memory_size_ = array.size_;
    }
  if (array.size_ > 0)
    memcpy(data_, array.data_, array.size_ * sizeof(float));
  size_ = array.size_;
  return *this;
}

const ArrOfFloat& ArrOfFloat::operator=(float x)
{
  for (int i = 0; i < size_; i++)
    data_[i] = x;
  return *this;
}

// Description: Destructeur
ArrOfFloat::~ArrOfFloat()
{
  if (data_)
    delete [] data_;
  data_ = 0; // Paranoia... mais ca peut servir s'il reste une ref a ce tableau
  size_ = 0;
  memory_size_ = 0;
}

// Description:
// Augmentation de taille du tableau (appele uniquement si l'espace
// memoire est insuffisant et qu'il faut reallouer de la memoire).
// Les anciennes donnees sont recopiees dans le nouveau tableau,
// le contenu des elements nouveaux est indefini.
void ArrOfFloat::mem_resize(int size)
{
  assert(size > size_);
  int newsize = size * grow_factor_num_ / grow_factor_denom_;
  float * newdata = new float[newsize];

  // Copier les anciennes donnees dans le nouveau tableau
  if (size_ > 0)
    memcpy(newdata, data_, size_ * sizeof(float));

  // Effacer l'ancien tableau
  if (data_)
    delete [] data_;

  data_ = newdata;
  memory_size_ = newsize;
  size_ = size;
}

float * ArrOfFloat::addr()
{
  return data_;
}

const float * ArrOfFloat::addr() const
{
  return data_;
}

// Description:
// Lecture du tableau. Format :
// n                          (int)
// x[0] x[1] x[2] ... x[n-1]  (float)
Entree& ArrOfFloat::readOn(Entree& is)
{
  int mysize;
  is >> mysize;
  // Resize en deux fois pour ne pas faire de memcpy inutile
  size_ = 0;
  resize_array(mysize);
  if (mysize > 0)
    is.get(data_, mysize);
  return is;
}

// Description:
// Ecriture du tableau. Format :
// n                          (int)
// x[0] x[1] x[2] ... x[n-1]  (float)
Sortie& ArrOfFloat::printOn(Sortie& os) const
{
  os << size_ << finl;
  if (size_ > 0)
    os.put(data_, size_, size_);
  return os;
}

// Description: remplit nb_elements consecutifs du tableau avec une valeur invalide
//  en commencant par l'indice first_element.
//  Il faut first_element >= 0 et first_element+nb_elements <= memory_size_.
void ArrOfFloat::invalidate(int first_element, int nb_elements)
{
  // Ceci represente un NAN. N'importe quelle operation avec ca fait encore un NAN.
  // Si c'est pas portable, on peut remplacer par DMAXFLOAT sur les autres machines.
  /* static const float VALEUR_INVALIDE =
     (__extension__ ((union {
     int __l __attribute__((__mode__(__DI__)));
     float __d; }) { __l: 0x7ff7ffffffffffffULL }).__d);; */
  static const float VALEUR_INVALIDE = 0x7ff7ffffffffffffULL ;

  const int n = first_element + nb_elements;
  assert(first_element >= 0 && n <= memory_size_);

  // On utilise "memcpy" et non "=" car "=" peut provoquer une exception
  // si la copie passe par le fpu.
  for (int i = first_element; i < n; i++)
    memcpy(data_+i, & VALEUR_INVALIDE, sizeof(float));
}
// Description:
//   Fonction de comparaison utilisee pour trier le tableau
//   dans ArrOfDouble::trier(). Voir man qsort
static True_int fonction_compare_arroffloat_ordonner(const void * data1, const void * data2)
{
  const float x = *(const float*)data1;
  const float y = *(const float*)data2;
  if (x < y)
    return -1;
  else if (x > y)
    return 1;
  else
    return 0;
}

void ArrOfFloat::ordonne_array()
{
  const int size = size_array();
  if (size > 1)
    {
      float * data = addr();
      qsort(data, size, sizeof(float),
            fonction_compare_arroffloat_ordonner);
    }
}


// Description:
//    Copie les elements source[first_element_source + i]
//    dans les elements  (*this)[first_element_dest + i] pour 0 <= i < nb_elements
//    Les autres elements de (*this) sont inchanges.
// Precondition:
// Parametre:       const ArrOfDouble& m
//  Signification:   le tableau a utiliser, doit etre different de *this !
// Parametre:       int nb_elements
//  Signification:   nombre d'elements a copier, nb_elements >= -1.
//                   Si nb_elements==-1, on copie tout le tableau m.
//  Valeurs par defaut: -1
// Parametre:       int first_element_dest
//  Valeurs par defaut: 0
// Parametre:       int first_element_source
//  Valeurs par defaut: 0
// Retour: ArrOfDouble&
//    Signification: *this
//    Contraintes:
// Exception:
//    Sort en erreur si la taille du tableau m est plus grande que la
//    taille de tableau this.
// Effets de bord:
// Postcondition:
ArrOfFloat& ArrOfFloat::inject_array(const ArrOfFloat& source,
                                     int nb_elements,
                                     int first_element_dest,
                                     int first_element_source)
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
      float * addr_dest = addr() + first_element_dest;
      const float * addr_source = source.addr() + first_element_source;
      // PL: On utilise le memcpy car c'est VRAIMENT plus rapide (10% +vite sur RNR_G20)
      memcpy(addr_dest , addr_source, nb_elements * sizeof(float));
      /*
        int i;
        for (i = 0; i < nb_elements; i++) {
        addr_dest[i] = addr_source[i];
        } */
    }
  return *this;
}

// Description:
//     muliplie toutes les cases par dy
// Retour: ArrOfFloat &
//    Signification: *this
const ArrOfFloat& ArrOfFloat::operator*= (const float dy)
{
  float * data = addr();
  const int n = size_array();
  for(int i=0; i < n; i++)
    data[i] *= dy;
  return *this;
}


// Description:
//     divise toutes les cases par dy
// Retour: ArrOfFloat &
//    Signification: *this
const ArrOfFloat& ArrOfFloat::operator/= (const float dy)
{
  // En theorie: les deux lignes suivantes sont plus efficaces, mais
  //  cela produit des differences sur certains cas tests
  //  (Hyd_C_VEF_Smago et Lambda_var_VEF_turb). Ca veut dire qu'il y
  //  a un probleme autre part mais pour l'instant on laisse l'ancien
  //  codage.
  // const float i_dy = 1. / dy;
  // operator*=(i_dy);

  float * data = addr();
  const int n = size_array();
  for(int i=0; i < n; i++)
    data[i] /= dy;

  return *this;
}
