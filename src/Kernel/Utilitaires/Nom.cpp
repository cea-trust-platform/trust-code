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
// File:        Nom.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/43
//
//////////////////////////////////////////////////////////////////////////////

#include <Nom.h>
#include <stdio.h>
#include <Motcle.h>
#include <string>
int Nom::nb_noms=0;

Implemente_instanciable_sans_constructeur_ni_destructeur(Nom,"Nom",Objet_U);

int sz = 0;

// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Ecriture d'un Nom sur un flot de sortie
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
Sortie& Nom::printOn(Sortie& s) const
{
  const char* nom=getChar();
  if(nom)
    return s << nom;
  else
    return s;
}
#define BUFLEN 10000

// Description: Lecture d'un nom.
//  En cas d'echec, le nom vaut "??" a la sortie.
Entree& Nom::readOn(Entree& s)
{
  char buffer[BUFLEN];
  const int ok = s.get(buffer, BUFLEN);
  if (ok)
    operator=(buffer);
  else
    operator=("??");
  return s;
}

//// Nom()
//

// Description:
//    Constructeur par defaut.
//    Cree la chaine "??"
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
Nom::Nom()
{
  nb_noms++;
  nom_ = "??";
}

// Description:
//    Construction d'un nom a partir d'un caractere
// Precondition:
// Parametre: char c
//    Signification: le caractere du nom
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::Nom(char c)
{
  nb_noms++;
  /* nom_=new char[2];
  nom_[0]=c;
  nom_[1]='\0';*/
  nom_=c;
}


// Description:
//    Construction d'un nom a partir d'un entier
//    La chaine cree est la representation de l'entier
//    Exemple : Nom(128) cree la chaine "128"
// Precondition:
// Parametre: int i
//    Signification: l'entier a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::Nom(int i)
{
  nb_noms++;
  nom_ = "";
  // 20 caracteres suffisent pour stocker n'importe quel entier

  char chaine[22];
#ifdef INT_is_64_
  sprintf(chaine, "%ld", i);
#else
  sprintf(chaine, "%d", i);
#endif
  operator=(chaine);
}
// Description:
//    Construction d'un nom a partir d'une chaine de caracteres
//    La chaine est copiee
// Precondition:
// Parametre: const char* nom
//    Signification: la chaine de caracteres a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::Nom(const char* nom)
{
  nb_noms++;
  nom_ = "";
  operator=(nom);
}

// Description:
//    Constructeur par copie d'un nom
// Precondition:
// Parametre: const Nom& nom
//    Signification: le nom a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::Nom(const Nom& nom) : Objet_U(nom)
{
  nb_noms++;
  nom_ = "";
  operator=(nom);
}

// Description:
//    Construction d'un nom a partir d'un flottant
//    La chaine cree est la representation du nombre reel (sprintf)
// Precondition:
// Parametre: double le_reel
//    Signification: le reel a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::Nom(double le_reel)
{
  nb_noms++;
  nom_ = "";
  char la_chaine[80];
  sprintf(la_chaine,"%f",le_reel);
  operator=(la_chaine);
}

// Description:
//    Construction d'un nom a partir d'un flottant
//    La chaine cree est la representation du nombre reel (sprintf)
//    Le format du nombre represente par la chaine est donne par format
// Precondition:
// Parametre: double le_reel
//    Signification: le reel a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::Nom(double le_reel, const char* format)
{
  char la_chaine[80];
  sprintf(la_chaine,format,le_reel);
#ifdef MICROSOFT
  // sous windows les chiffres s'ecrivent 1.0000e+000 (avec 3 chiffres pour les puissances)
  // on retire le premier zero
  unsigned int length=strlen(la_chaine);
  if (la_chaine[length-5]=='e')
    {
      if (la_chaine[length-3]!='0') exit();
      for (unsigned int i=length-3; i<=length; i++)
        la_chaine[i]=la_chaine[i+1];
    }
#endif
  nom_ =  la_chaine;
  //delete[] la_chaine;
  nb_noms++;
}


// Description:
//    Destructeur
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
Nom::~Nom()
{
  nb_noms--;

}

// Description:
//    Transforme le nom en majuscules
//    Seules les lettres 'a'-'z' sont modifiees
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
Nom& Nom::majuscule()
{
  /*
    char * es=strdup(nom_.c_str()); -> valgrind error with $exec_opt binary
    convertit_en_majuscule(es);
    nom_=es;
    free(es); */
  int length = nom_.size();
  for(int p = 0; p < length; p++)
    nom_[p] = toupper(nom_[p]);
  return *this;
}


// Description:
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
char intochar(int i)
{
  assert( (0<=i) && (i<=9) );
  return ('0'+i);
}

// Description:
//    Renvoie le nombre de caracteres de la chaine du Nom
//    y compris le caractere zero de fin de chaine.
//    Exemple : Nom("hello").longueur() == 6
int Nom::longueur() const
{
  return (strlen(nom_.c_str())+1);
}

// Description:
//  Copie la chaine nom.
//  Modif BM pour que nom puisse pointer sur une sous-partie de nom_
Nom& Nom::operator=(const char* const nom)
{
  nom_=nom;
  return *this;
}

// Description:
//    Copie le Nom nom
// Precondition:
// Parametre: const Nom& nom
//    Signification: le nom a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: reference sur this qui represente la chaine du Nom nom
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom& Nom::operator=(const Nom& nom)
{
  operator=(nom.nom_.c_str());
  return *this;
}

// Description:
//    Concatenation avec un Nom
// Precondition:
// Parametre: const Nom& x
//    Signification: la nom a concatener
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: reference sur this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom& Nom::operator +=(const Nom& x)
{
  operator+=(x.nom_.c_str());
  return *this;
}

Nom& Nom::operator+=(const char *x)
{
  nom_ += x;
  return *this;
}

// Description:
// concatenation de chaine
Nom& Nom::operator +=(char x)
{
  char n[2];
  n[0] = x;
  n[1] = 0;
  operator+=(n);
  return *this;
}

// Description:
// extraction de suffix :
// Nom x("azerty");
// x.suffix("aze")
// x contient "rty".

// Description:
//     Extraction de suffixe :
//     Nom x("azerty");
//     x.suffix("aze")
//     x contient "rty".
// Precondition:
// Parametre: const char* const ch
//    Signification: chaine de caractere a utiliser comme prefixe
//    Valeurs par defaut:
//    Contraintes: ch doit etre un prefixe de x
//    Acces:
// Retour: Nom&
//    Signification: reference sur this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom& Nom::suffix(const char* const s)
{

  if (debute_par(s))
    {
      //int n = strlen(s_.c_str());
      int n2 = strlen(s);
      nom_.erase(0,n2);
    }
  return *this;
}

const Nom Nom::getSuffix(const char* const s) const
{
  if (debute_par(s))
    {
      const int n1 = strlen(s);
      const int n2 = nom_.size();
      const std::string str1 = nom_.substr(n1,n2);
      return Nom(str1.c_str());

    }
  return *this;
}

int Nom::debute_par(const char* const ch) const
{
  int result = 1;
  const char * const s = nom_.c_str();
  int i;
  for (i = 0; ch[i] != 0; i++)
    {
      // (note: ca marche aussi si nom_ est plus court que ch)
      if (ch[i] != s[i])
        {
          result = 0;
          break;
        }
    }
  return result;
}

int Nom::finit_par(const char* const s) const
{
  const int l1 = longueur()-1;
  const int l2 = strlen(s);
  return (l1>=l2) ? (strncmp(nom_.c_str()+(l1-l2), s, l2) == 0) : 0;

}
int Nom::find(const char * n) const
{
  std::string s_(nom_);
  std::size_t x = s_.find(n);
  return (x != std::string::npos) ? 1 : -1;
}

Nom& Nom::prefix(const char* const s)
{
  if (finit_par(s))
    {
      int n = strlen(nom_.c_str());
      int n2 = strlen(s);
      nom_.erase(n-n2,n2);
    }

  return *this;
}

const Nom Nom::getPrefix(const char* const s) const
{
  if (finit_par(s))
    {
      //int n = strlen(s_.c_str());

      const int n1 = nom_.size();
      const int n2 = strlen(s);
      const std::string str1 = nom_.substr(0,n1-n2);
      return Nom(str1.c_str());

    }
  return *this;
}
// Description:
//    Concatenation avec un Nom
// Precondition:
// Parametre: const Nom& x
//    Signification: la nom a concatener
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom
//    Signification: le nouveau Nom cree avec la concatenation de this et de x
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom Nom::operator +(const Nom& x) const
{
  Nom nouveau(*this);
  nouveau += x;
  return nouveau;
}

// Description:
//    Comparaison avec un Objet_U
//    l'Objet_U est caste en Nom pour la comparaison
// Precondition:
// Parametre: const Objet_U& x
//    Signification: l'Objet_U a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si egalite
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Nom::est_egal_a(const Objet_U& x) const
{
  if (!(sub_type(Nom, x))) return 0;
  return (*this == ref_cast( Nom, x));
}

// Description:
// Insere _000n (n=me()) dans un nom de fichier (par ex:toto.titi) pour donner toto_000n.titi
// Precondition:
// Parametre:
//    Signification
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom Nom::nom_me(int n) const
{
  // char* newname = new char[strlen(nom_.c_str())+6];
  int compteur=strlen(nom_.c_str());
  const char* ptr=nom_.c_str()+compteur;
  while((*ptr!='.') && (*ptr!='/')&&(compteur>0))
    {
      ptr--;
      compteur--;
      if (*ptr=='/')
        {
          compteur=0;
        }
    }
  int pas_de_point=0;
  if(compteur==0)
    {
      compteur=strlen(nom_.c_str());
      pas_de_point=1 ;
    }
  std::string newname=nom_.substr(0,compteur);

  int digits=-1,diviseur=-1;
  if (Process::nproc()<=10000)
    {
      digits=5;
      diviseur=1000;
    }
  else if (Process::nproc()<=100000)
    {
      digits=6;
      diviseur=10000;
    }
  else
    {
      Cerr << "Error in Nom::nom_me. Contact TRUST support." << finl;
      exit();
    }
  char *c_numero=new char[digits+1];
  int resultat;
  c_numero[0]='_';
  for (int i=1; i<digits; i++)
    {
      resultat=n/diviseur;
      char c=('0'+resultat);
      c_numero[i]=c;
      n-=resultat*diviseur;
      diviseur/=10;
    }
  c_numero[digits]='\0';
  newname+=c_numero;
  if (pas_de_point==0)
    newname+=ptr;
  Nom new_name=newname.c_str();
  delete[] c_numero;
  return new_name;
}

// Description:
//   Retourne un nom selon la commande usuelle substr
//   ATTENTION : deb = 1 => premier caractere de la chaine !!!
Nom Nom::substr_old(const int deb, const int la_longueur) const
{

  assert(deb > 0);
  assert(deb - 1 + la_longueur <= (int) strlen(nom_.c_str()));
  Nom nouveau(nom_.substr(deb-1,la_longueur).c_str());
  return nouveau;
}

// Description:
// Retourne fichier si le nom est sous la forme /toto/titi/fichier
// Precondition:
// Parametre:
//    Signification
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom Nom::basename() const
{
  const char* nom=nom_.c_str();
  Nom dirname("");
  Nom the_basename(nom);
  int iLength = strlen(nom);
  for (int i=0; i<iLength; i++)
    {
      dirname+=nom[i];
      if (nom[i]==47 || nom[i]==92) /* 47:/ 92:\ */
        {
          the_basename.suffix(dirname);
          dirname="";
        }
    }
  return the_basename;
}

int Nom::selftest()
{
  exit();
  return 0;
}
// Description:
//    Retourne un pointeur sur la chaine de caractere du nom
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: char*
//    Signification: pointeur sur la chaine de caractere du nom
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom::operator const char*() const
{
  return nom_.c_str();
}


// Description:
//     Comparaison d'un nom avec une chaine de caractere
//     Utilise strcmp
// Precondition:
// Parametre: const Nom& un_nom
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const char* const un_autre
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les noms sont egaux, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator ==(const Nom& un_nom, const char* const un_autre)
{
  //int res_actu=(strcmp(un_nom.nom_.c_str(), un_autre)==0);
  int res_actu=(un_nom.nom_.compare(un_autre)==0);
  if (!res_actu)
    {
      Motcle toto(un_autre);
      if (toto==un_nom)
        {
          if (Process::je_suis_maitre())
            {
              Cerr << "Warning: " << un_nom << " and " << un_autre << " are they really different ?" << finl;
              Cerr << "A test in the code on a string of characters does not seem to take account of the case." << finl;
              Cerr << "Contact TRUST support by sending your data file of this calculation." << finl;
              /*
                #ifndef NDEBUG
                Process::exit();
                #endif
              */
            }
        }
    }
  return res_actu;
}

int operator ==(const Nom& un_nom, const Nom& un_autre)
{
  return (un_nom==un_autre.getChar());
}


int operator ==(const char* const un_autre, const Nom& un_nom)
{
  return (un_nom == un_autre);
}

// Description:
//     Comparaison d'un nom avec une chaine de caractere
// Precondition:
// Parametre: const Nom& un_nom
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const char* const un_autre
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les noms sont differents, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator !=(const Nom& un_nom, const char* un_autre)
{
  return ! (un_nom == un_autre);
}

int operator !=(const Nom& un_nom, const Nom& un_autre)
{
  return ! (un_nom == un_autre);
}

int operator !=(const char* const un_autre, const Nom& un_nom)
{
  return ! (un_autre == un_nom);
}

// Description:
//     Renvoie *this;
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const Nom&
//    Signification: reference sur le Nom
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Nom& Nom::le_nom() const
{
  return *this;
}
