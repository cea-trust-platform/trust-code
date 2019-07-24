/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Motcle.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Motcle.h>
#include <Noms.h>

Implemente_vect(Motcle);
Implemente_instanciable_sans_constructeur(Motcle,"Motcle",Nom);
Implemente_instanciable(Motcles,"Motcles",VECT(Motcle));


// Description:
//    Ecriture d'un Motcle sur un flot de sortie
//    Utilise l'implementation de la classe Nom
// Precondition:
// Parametre: Sortie& s
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Motcle::printOn(Sortie& s) const
{
  return Nom::printOn(s);
}


// Description:
//    Ecriture d'un tableau Motcles sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Motcles::printOn(Sortie& s) const
{
  return VECT(Motcle)::printOn(s);
}


// Description:
//    Lecture d'un Motcle dans un flot d'entree
//    Utilise l'implementation de la classe Nom
//    Passe ensuite le Nom en majuscules
// Precondition:
// Parametre: Entree& s
//    Signification: le flux d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flux d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Motcle::readOn(Entree& s)
{
  Nom::readOn(s);
  majuscule();
  return s;
}


// Description:
//    Constructeur par defaut
//    Construit un Nom puis le passe en majuscule
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Motcle::Motcle() : Nom()
{
  majuscule();
}


// Description:
//    Construction d'un Motcle a partie d'une chaine de caracteres
//    Construit un Nom puis le passe en majuscules
// Precondition:
// Parametre: const char* nom
//    Signification: la chaine de caracteres du Motcle
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Motcle::Motcle(const char* const nom) : Nom(nom)
{
  majuscule();
}

Motcle::Motcle(const Nom& nom) : Nom(nom)
{
  majuscule();
}

// Description:
//    Construction d'un Motcle par copie
// Precondition:
// Parametre: const Motcle& nom
//    Signification: le Motcle a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Motcle::Motcle(const Motcle& nom) : Nom(nom)
{
}

// Description:
//    Construction a partir d'une chaine de caracteres
//    Utilise l'implementation de la classe Nom
// Precondition:
// Parametre: const char* const mot
//    Signification: la chaine de caracteres du mot a construire
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Motcle&
//    Signification: reference sur le Motcle cree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description:
//    Construction par copie
//    Utilise l'implementation de la classe Nom
// Precondition:
// Parametre: const Motcle& mot
//    Signification: le Motcle a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: reference sur le Motcle modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Motcle& Motcle::operator=(const Motcle& mot)
{
  Nom::operator=(mot);
  return *this;
}

// Description:
//    Lecture d'un tableau Motcles sur un flot d'entree
// Precondition:
// Parametre: Entree& s
//    Signification: le flux d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flux d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Motcles::readOn(Entree& s)
{
  return VECT(Motcle)::readOn(s);
}


// Description:
//    Autotest de la classe Motcle
//    Effectue des affectations de controle
//    Retourne toujours 1
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour; retourne toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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



// Description:
//    Constructeur
//    Cree un tableau de i elements
// Precondition:
// Parametre: int i
//    Signification: nombre de mots cles
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Motcles::Motcles(int i):
  VECT(Motcle)(i)
{
}

static inline char char_uppercase(char c)
{
  if (c >= 'a' && c <= 'z')
    c += 'A' - 'a';
  return c;
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
      c1 = (unsigned char) char_uppercase(n1[i]);
      i++;
      length_i++;
      if (c1==0 || c1==124) // Keyword found in n1
        {
          // loop on c2
          int j=0;
          int length_j=0;
          do
            {
              c2 = (unsigned char) char_uppercase(n2[j]);
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
                          C1 = (unsigned char) char_uppercase(n1[i-l]);
                          C2 = (unsigned char) char_uppercase(n2[j-l]);
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

// Description:
//    Comparaison d'un mot cle avec une chaine de caracteres
// Precondition:
// Parametre: const Motcle& un_mot
//    Signification: le mot cle a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const char* const nom
//    Signification: la chaine de caractere avec laquelle comparer le mot cle
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si le Nom du mot est egal a la chaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description:
//    Comparaison d'un mot cle avec une chaine de caracteres
// Precondition:
// Parametre: const Motcle& un_mot
//    Signification: le mot cle a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const char* const nom
//    Signification: la chaine de caractere avec laquelle comparer le mot cle
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si le Nom du mot est different de la chaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description: Identique a try_map_to_int, mais en cas d'echec
//  affiche un message d'erreur et quitte le programme.
int Motcle::map_to_int(const char * keywords_list) const
{
  int resu = try_map_to_int(keywords_list);
  if (resu < 0)
    {
      Cerr << "Error in Motcle::map_to_int : the keyword " << nom_.c_str()
           << "\n has not been found in the following list :\n"
           << keywords_list << finl;
      exit();
    }
  return resu;
}

// Description: cherche si le motcle se trouve dans la liste keywords_list
//  et si oui renvoie le numero associe au nom.
//  keywords_list est une chaine de caracteres terminee par '\0' de la forme suivante:
//   farine:5,oeuf:1,beurre:2,levure:3,lait:4,kougelopf:15
//  Les entiers associes aux noms doivent etre positifs ou nuls.
//  Si le motcle contient "beurre", try_map_to_int renvoie 2.
//  Si le motcle n'est pas trouve, renvoie -1;
int Motcle::try_map_to_int(const char * keywords_list) const
{
  // Pointeur dans la chaine keywords_list
  const char * map_ptr = keywords_list;
  const char * src = nom_.c_str();
  int resultat = -1;

  while (*map_ptr)   // Tant qu'on n'est pas a la fin de la chaine
    {
      // Combien de caracteres identiques ?
      const int count = strcmp_uppercase(src, map_ptr);
      int ok = 0;
      if (map_ptr[count] == ':' && src[count] == 0)
        ok = 1;
      // On avance jusqu'au prochain ":"
      while (*map_ptr != ':')
        {
          if (*map_ptr == 0 || *map_ptr == ',')
            {
              Cerr << "Error in Motcle::try_map_to_int(\"" << keywords_list << "\")\n"
                   << " invalid string, we expected a ':'" << finl;
              exit();
            }
          map_ptr++;
        }
      map_ptr++;
      // Lecture de l'entier correspondant
      int intval = 0;
      while (*map_ptr != ',' || *map_ptr != 0)
        {
          const int c = (int) (*map_ptr);
          int i = c - '0';
          if (i < 0 || i > 9)
            {
              Cerr << "Error in Motcle::try_map(\"" << keywords_list << "\")\n"
                   << " invalid string, we expect a positive integer or null between : and ," << finl;
              exit();
            }
          intval = intval * 10 + i;
        }
      if (ok)
        {
          resultat = intval;
          break;
        }
    }
  return resultat;
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
