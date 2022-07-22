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

#include <Char_ptr.h>

#include <Motcle.h>
#include <string>



/*! @brief Constructeur par defaut.
 *
 * Cree la chaine "??"
 *
 */
Char_ptr::Char_ptr()
{
  nom_ = new char[3];
  nom_[0] = '?';
  nom_[1] = '?';
  nom_[2] = 0;
}



/*! @brief Construction d'un nom a partir d'une chaine de caracteres La chaine est copiee
 *
 * @param (const char* nom) la chaine de caracteres a utiliser
 */
Char_ptr::Char_ptr(const char* nom)
{
  nom_ = 0;
  operator=(nom);
}

/*! @brief Constructeur par copie d'un nom
 *
 * @param (const Char_ptr& nom) le nom a utiliser
 */
Char_ptr::Char_ptr(const Char_ptr& nom)
{
  nom_ = 0;
  operator=(nom);
}

/*! @brief Destructeur
 *
 */
Char_ptr::~Char_ptr()
{
  if(nom_)
    delete[] nom_;
}


/*! @brief Renvoie le nombre de caracteres de la chaine du Char_ptr y compris le caractere zero de fin de chaine.
 *
 *     Exemple : Char_ptr("hello").longueur() == 6
 *
 */
int Char_ptr::longueur() const
{
  return ((int)strlen(nom_)+1);
}

/*! @brief Copie la chaine nom.
 *
 * Modif BM pour que nom puisse pointer sur une sous-partie de nom_
 *
 */
Char_ptr& Char_ptr::operator=(const char* const nom)
{
  if (nom_ == nom)
    return *this;
  char *old = nom_;
  const char *n = nom;
  if (!n)
    n = "??";
  nom_ = new char[strlen(n)+1];
  strcpy(nom_, n);
  // On efface l'ancien apres avoir copie le nouveau au cas ou nom est une partie de nom_
  delete [] old;
  return *this;
}

/*! @brief Copie le Char_ptr nom
 *
 * @param (const Char_ptr& nom) le nom a copier
 * @return (Char_ptr&) reference sur this qui represente la chaine du Char_ptr nom
 */
Char_ptr& Char_ptr::operator=(const Char_ptr& nom)
{
  operator=(nom.nom_);
  return *this;
}

/*! @brief Retourne un pointeur sur la chaine de caractere du nom
 *
 * @return (char*) pointeur sur la chaine de caractere du nom
 */
Char_ptr::operator char*() const
{
  return nom_;
}


void Char_ptr::allocate(int n)
{
  if (nom_)
    delete [] nom_;
  nom_=new char[n+1];
  for (int i=0; i<n; i++)
    nom_[i]=' ';
  nom_[n]='\0';
}
