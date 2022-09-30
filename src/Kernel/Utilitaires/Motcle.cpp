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

#include <Motcle.h>
#include <Noms.h>

Implemente_instanciable_sans_constructeur(Motcle,"Motcle",Nom);
Implemente_instanciable(Motcles,"Motcles",VECT(Motcle));


/*! @brief Ecriture d'un Motcle sur un flot de sortie Utilise l'implementation de la classe Nom
 *
 * @param (Sortie& s) le flot de sortie a utiliser
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Motcle::printOn(Sortie& s) const
{
  return Nom::printOn(s);
}


/*! @brief Ecriture d'un tableau Motcles sur un flot de sortie
 *
 * @param (Sortie& s) le flot de sortie a utiliser
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Motcles::printOn(Sortie& s) const
{
  return VECT(Motcle)::printOn(s);
}


/*! @brief Lecture d'un Motcle dans un flot d'entree Utilise l'implementation de la classe Nom
 *
 *     Passe ensuite le Nom en majuscules
 *
 * @param (Entree& s) le flux d'entree a utiliser
 * @return (Entree&) le flux d'entree modifie
 */
Entree& Motcle::readOn(Entree& s)
{
  Nom::readOn(s);
  majuscule();
  return s;
}


/*! @brief Constructeur par defaut Construit un Nom puis le passe en majuscule
 *
 */
Motcle::Motcle() : Nom()
{
  majuscule();
}


/*! @brief Construction d'un Motcle a partie d'une chaine de caracteres Construit un Nom puis le passe en majuscules
 *
 * @param (const char* nom) la chaine de caracteres du Motcle
 */
Motcle::Motcle(const char* const nom) : Nom(nom)
{
  majuscule();
}

Motcle::Motcle(const std::string& nom) : Nom(nom)
{
  majuscule();
}

Motcle::Motcle(const Nom& nom) : Nom(nom)
{
  majuscule();
}

/*! @brief Construction d'un Motcle par copie
 *
 * @param (const Motcle& nom) le Motcle a copier
 */
Motcle::Motcle(const Motcle& nom) : Nom(nom)
{
}

/*! @brief Construction a partir d'une chaine de caracteres Utilise l'implementation de la classe Nom
 *
 * @param (const char* const mot) la chaine de caracteres du mot a construire
 * @return (Motcle&) reference sur le Motcle cree
 */
Motcle& Motcle::operator=(const char* const mot)
{
  Nom::operator=(mot);
  majuscule();
  return *this;
}

Motcle& Motcle::operator=(const Nom& mot)
{
  Nom::operator=(mot);
  majuscule();
  return *this;
}

/*! @brief Construction par copie Utilise l'implementation de la classe Nom
 *
 * @param (const Motcle& mot) le Motcle a copier
 * @return (Motcle&) reference sur le Motcle modifie
 */
Motcle& Motcle::operator=(const Motcle& mot)
{
  Nom::operator=(mot);
  return *this;
}

/*! @brief Lecture d'un tableau Motcles sur un flot d'entree
 *
 * @param (Entree& s) le flux d'entree a utiliser
 * @return (Entree&) le flux d'entree modifie
 */
Entree& Motcles::readOn(Entree& s)
{
  return VECT(Motcle)::readOn(s);
}


/*! @brief Autotest de la classe Motcle Effectue des affectations de controle
 *
 *     Retourne toujours 1
 *
 * @return (int) code de retour; retourne toujours 1
 */
int Motcle::selftest()
{
  Nom un_nom;
  Motcle un_motcle("PasOk");
  Motcle un_autre(un_nom);
  un_nom="Ok";
  un_motcle=un_nom;
  un_motcle="Ok";
  un_motcle=un_autre;
  return 1;
}



/*! @brief Constructeur Cree un tableau de i elements
 *
 * @param (int i) nombre de mots cles
 */
Motcles::Motcles(int i):
  VECT(Motcle)(i)
{
}

// "a"  == "a|b" returns 0
// "b"  == "a|b" returns 0
// "a|b" == "a|c" return 0
static inline int strcmp_uppercase(const char *n1, const char *n2)
{
  // loop on n1
  unsigned char c1,c2;
  int i=0;
  int length_i=0;
  do
    {
      c1 = (unsigned char) ::toupper(n1[i]);
      i++;
      length_i++;
      if (c1==0 || c1==124) // Keyword found in n1
        {
          // loop on c2
          int j=0;
          int length_j=0;
          do
            {
              c2 = (unsigned char) ::toupper(n2[j]);
              j++;
              length_j++;
              if (c2==0 || c2==124) // Keyword found in n2
                {
                  if (length_i==length_j)
                    {
                      // Same length, we compare each caracter:
                      int l=length_i;
                      int delta=0;
                      unsigned char C1,C2;
                      do
                        {
                          C1 = (unsigned char) ::toupper(n1[i-l]);
                          C2 = (unsigned char) ::toupper(n2[j-l]);
                          delta = C2 - C1;
                          l--;
                        }
                      while (l>1 && delta==0);
                      if (delta==0)
                        return 0; // Found the same keyword
                    }
                  length_j=0;
                }
            }
          while(c2!=0); // End of n2
          length_i=0;
        }
    }
  while (c1!=0); // End of n1
  return 1;
}

/*! @brief Comparaison d'un mot cle avec une chaine de caracteres
 *
 * @param (const Motcle& un_mot) le mot cle a utiliser pour la comparaison
 * @param (const char* const nom) la chaine de caractere avec laquelle comparer le mot cle
 * @return (int) 1 si le Nom du mot est egal a la chaine
 */
int operator ==(const Motcle& nom, const Motcle& un_mot)
{
  const int resu = strcmp_uppercase((const char *) nom, (const char *)un_mot);
  return (resu == 0);
}

int operator ==(const Motcle& un_mot, const char* const nom)
{
  const int resu = strcmp_uppercase((const char *)un_mot, nom);
  return (resu == 0);
}

int operator ==(const char* const nom, const Motcle& un_mot)
{
  return (un_mot == nom);
}

int operator ==(const Motcle& un_mot, const Nom& nom)
{
  const int resu = strcmp_uppercase((const char *)un_mot, (const char *) nom);
  return (resu == 0);
}

int operator ==(const Nom& nom, const Motcle& un_mot)
{
  return (un_mot == nom);
}

/*! @brief Comparaison d'un mot cle avec une chaine de caracteres
 *
 * @param (const Motcle& un_mot) le mot cle a utiliser pour la comparaison
 * @param (const char* const nom) la chaine de caractere avec laquelle comparer le mot cle
 * @return (int) 1 si le Nom du mot est different de la chaine
 */
int operator !=(const Motcle& nom, const Motcle& un_mot)
{
  return ! (nom == un_mot);
}

int operator !=(const Motcle& un_mot, const char* const nom)
{
  return ! (un_mot == nom);
}

int operator !=(const char* const nom, const Motcle& un_mot)
{
  return ! (nom == un_mot);
}

int operator !=(const Motcle& un_mot, const Nom& nom)
{
  return ! (un_mot == nom);
}

int operator !=(const Nom& nom, const Motcle& un_mot)
{
  return ! (nom == un_mot);
}

Motcle& Motcle::operator+=(const char * const mot)
{
  Nom::operator+=(mot);
  majuscule();
  return *this;
}

Motcles noms_to_motcles(const Noms& a)
{
  int n = a.size();

  Motcles b(n);
  for (int i = 0; i < n; i++)
    b[i] = a[i]; // ouais, ecriture bizarre mais la plus efficace...
  return b;
}

int Motcle::finit_par(const char* const mot) const
{
  Motcle mm(mot);
  return Nom::finit_par(mm);
}


int Motcle::debute_par(const char* const mot) const
{
  Motcle mm(mot);
  return Nom::debute_par(mm);
}

int Motcle::find(const char* const mot) const
{
  Motcle mm(mot);
  return Nom::find(mm);
}

int Motcles::search(const Motcle& t ) const
{
  assert(size()>=0);
  int i=size();
  while(i--)
    if (operator()(i)==t)
      {
        return i;
      }
  return -1;
}

int Motcles::contient_(const char* const ch) const
{
  return (rang(ch)!=-1);
}
/* Returns the VECT position number of a string (-1 if not found) */
int Motcles::rang(const char* const ch) const
{
  Nom nom(ch);
  assert(size()>=0);
  int i=size();
  while(i--)
    if (operator()(i)==nom)
      {
        return i;
      }
  return -1;
}
