/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Nom.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <algorithm>
#include <cmath>

int Nom::nb_noms=0;

Implemente_instanciable_sans_constructeur_ni_destructeur(Nom,"Nom",Objet_U);
// XD nom objet_u nom 0 Class to name the TRUST objects.
// XD attr mot chaine mot 1 Chain of characters.


int Nom::check_case_non_sensitive_=1;


void Nom::sed_check_case_non_sensitive(int i)
{
  check_case_non_sensitive_=i;
}

/*! @brief Surcharge Objet_U::printOn(Sortie&) Ecriture d'un Nom sur un flot de sortie
 *
 * @param (Sortie& s) le flot de sortie a utiliser
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Nom::printOn(Sortie& s) const
{
  const char* nom=getChar();
  if(nom)
    return s << nom;
  else
    return s;
}
#define BUFLEN 100000

/*! @brief Lecture d'un nom.
 *
 * En cas d'echec, le nom vaut "??" a la sortie.
 *
 */
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

/*! @brief Constructeur par defaut.
 *
 * Cree la chaine "??"
 *
 */
Nom::Nom()
{
  nb_noms++;
  nom_ = "??";
}

/*! @brief Construction d'un nom a partir d'un caractere
 *
 * @param (char c) le caractere du nom
 */
Nom::Nom(char c)
{
  nb_noms++;
  nom_=c;
}


/*! @brief Construction d'un nom a partir d'un entier La chaine cree est la representation de l'entier
 *
 *     Exemple : Nom(128) cree la chaine "128"
 *
 * @param (int i) l'entier a utiliser
 */
Nom::Nom(True_int i)
{
  nb_noms++;
  nom_ = "";
  // 22 caracteres suffisent pour stocker n'importe quel entier
  char chaine[22];
  snprintf(chaine, 22, "%d", i);
  operator=(chaine);
}

Nom::Nom(long i)
{
  nb_noms++;
  nom_ = "";
  // 22 caracteres suffisent pour stocker n'importe quel entier
  char chaine[22];
  snprintf(chaine, 22, "%ld", i);
  operator=(chaine);
}


/*! @brief Construction d'un nom a partir d'une chaine de caracteres La chaine est copiee
 *
 * @param (const char* nom) la chaine de caracteres a utiliser
 */
Nom::Nom(const char* nom) : nom_(nom)
{
  nb_noms++;
}

Nom::Nom(const std::string& nom) : nom_(nom)
{
  nb_noms++;
}


/*! @brief Constructeur par copie d'un nom
 *
 * @param (const Nom& nom) le nom a utiliser
 */
Nom::Nom(const Nom& nom) : Objet_U(nom), nom_(nom.nom_)
{
  nb_noms++;
}

/*! @brief Construction d'un nom a partir d'un flottant La chaine cree est la representation du nombre reel (snprintf)
 *
 * @param (double le_reel) le reel a utiliser
 */
Nom::Nom(double le_reel)
{
  nb_noms++;
  nom_ = "";
  char la_chaine[80];
  snprintf(la_chaine,80,"%f",le_reel);
  operator=(la_chaine);
}

/*! @brief Construction d'un nom a partir d'un flottant La chaine cree est la representation du nombre reel (snprintf)
 *
 *     Le format du nombre represente par la chaine est donne par format
 *
 * @param (double le_reel) le reel a utiliser
 */
Nom::Nom(double le_reel, const char* format)
{
  char la_chaine[80];
  snprintf(la_chaine,80,format,le_reel);
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


/*! @brief Destructeur
 *
 */
Nom::~Nom()
{
  nb_noms--;
}

/*! @brief Transforme le nom en majuscules Seules les lettres 'a'-'z' sont modifiees
 *
 */
Nom& Nom::majuscule()
{
  std::transform(nom_.begin(), nom_.end(), nom_.begin(), ::toupper);
  return *this;
}

/*! @brief Renvoie le nombre de caracteres de la chaine du Nom y compris le caractere zero de fin de chaine.
 *
 *     Exemple : Nom("hello").longueur() == 6
 *
 */
int Nom::longueur() const
{
  return (int)nom_.size()+1;
}

/*! @brief Copie la chaine nom.
 *
 * Modif BM pour que nom puisse pointer sur une sous-partie de nom_
 *
 */
Nom& Nom::operator=(const char* const nom)
{
  nom_=nom;
  return *this;
}

/*! @brief Copie le Nom nom
 *
 * @param (const Nom& nom) le nom a copier
 * @return (Nom&) reference sur this qui represente la chaine du Nom nom
 */
Nom& Nom::operator=(const Nom& nom)
{
  nom_ = nom.nom_;
  return *this;
}

/*! @brief Concatenation avec un Nom
 *
 * @param (const Nom& x) la nom a concatener
 * @return (Nom&) reference sur this
 */
Nom& Nom::operator +=(const Nom& x)
{
  nom_ += x.nom_;
  return *this;
}

Nom& Nom::operator+=(const char *x)
{
  nom_ += x;
  return *this;
}

/*! @brief concatenation de chaine
 *
 */
Nom& Nom::operator +=(char x)
{
  char n[2];
  n[0] = x;
  n[1] = 0;
  operator+=(n);
  return *this;
}

Nom& Nom::operator +=(unsigned char x)
{
  char n[2];
  n[0] = (char)x;
  n[1] = 0;
  operator+=(n);
  return *this;
}


Nom& Nom::operator +=(int x)
{
  nom_ += Nom(x);
  return *this;
}

/*! @brief Extraction de suffixe : Nom x("azerty");
 *
 *      x.suffix("aze")
 *      x contient "rty".
 *
 * @param (const char* const ch) chaine de caractere a utiliser comme prefixe
 * @return (Nom&) reference sur this
 */
Nom& Nom::suffix(const char* const s)
{
  if (debute_par(s))
    {
      int n2 = (int)strlen(s);
      nom_.erase(0,n2);
    }
  return *this;
}

const Nom Nom::getSuffix(const char* const s) const
{
  if (debute_par(s))
    {
      const int n1 = (int)strlen(s);
      const int n2 = (int)nom_.size();
      const std::string str1 = nom_.substr(n1,n2);
      return Nom(str1);

    }
  return *this;
}

int Nom::debute_par(const std::string& ch) const
{
  return (nom_.rfind(ch, 0) == 0);
}

int Nom::finit_par(const std::string& s) const
{
  auto l = nom_.size(), e = s.size();
  if (l >= e)
    return (0 == nom_.compare(l - e, e, s));
  else
    return 0;
}

int Nom::find(const std::string& n) const
{
  std::size_t x = nom_.find(n);
  return (x != std::string::npos) ? (int)x : -1;
}

int Nom::find(const char* const n ) const
{
  return find(std::string(n));
}

int Nom::debute_par(const char* const n) const
{
  return debute_par(std::string(n));
}

int Nom::finit_par(const char* const n) const
{
  return finit_par(std::string(n));
}

Nom& Nom::prefix(const char* const s)
{
  if (finit_par(s))
    {
      int n = (int)nom_.size();
      int n2 = (int)strlen(s);
      nom_.erase(n-n2,n2);
    }
  return *this;
}

const Nom Nom::getPrefix(const char* const s) const
{
  if (finit_par(s))
    {
      const int n1 = (int)nom_.size();
      const int n2 = (int)strlen(s);
      const std::string str1 = nom_.substr(0,n1-n2);
      return Nom(str1);

    }
  return *this;
}

/*! @brief Concatenation avec un Nom
 *
 * @param (const Nom& x) la nom a concatener
 * @return (Nom) le nouveau Nom cree avec la concatenation de this et de x
 */
Nom Nom::operator +(const Nom& x) const
{
  Nom nouveau(*this);
  nouveau += x;
  return nouveau;
}

/*! @brief Comparaison avec un Objet_U l'Objet_U est caste en Nom pour la comparaison
 *
 * @param (const Objet_U& x) l'Objet_U a utiliser pour la comparaison
 * @return (int) 1 si egalite
 */
int Nom::est_egal_a(const Objet_U& x) const
{
#ifndef LATATOOLS
  if (!(sub_type(Nom, x))) return 0;
  return (*this == ref_cast( Nom, x));
#else
  const Nom& n2 = dynamic_cast<const Nom&>(x);
  return (*this == n2);
#endif
}

/*! @brief Insere _prefix000n (n=me() ou nproc()) dans un nom de fichier (par ex:toto.
 *
 * titi) pour donner toto_prefix000n.titi
 *
 * @param (without_padding) flag indiquant que l'on ne souhaite pas ajouter de zeros devant n
 */
Nom Nom::nom_me(int n, const char* prefixe, int without_padding) const
{
  int compteur=(int)nom_.size();
  const char* ptr=nom_.c_str()+compteur;
  while((*ptr!='.') && (*ptr!='/')&&(compteur>0))  // backward loop
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
      compteur=(int)nom_.size();
      pas_de_point=1 ;
    }
  std::string newname=nom_.substr(0,compteur);

  //searching for the number of digits we want to write
  int digits=0,diviseur=0;
  if(without_padding)
    {
      digits = (n==0) ? 1 : (int)std::lrint(std::truncl(log10(n)+1.0));
      diviseur = (int)std::lrint(std::truncl(pow(10, digits-1)));
    }
  else
    {
      if (Process::nproc()<=10000)
        {
          //the underscore will be taken into account later
          //digits=5;
          digits=4;
          diviseur=1000;
        }
      else if (Process::nproc()<=100000)
        {
          //the underscore will be taken into account later
          //digits=6;
          digits=5;
          diviseur=10000;
        }
      else if (Process::nproc()<=1000000)
        {
          //the underscore will be taken into account later
          //digits=7;
          digits=6;
          diviseur=100000;
        }
      else
        {
          Cerr << "Error in Nom::nom_me. Contact TRUST support." << finl;
          exit();
        }

    }

  int prefix_len = 1; //for the underscore
  if(prefixe) prefix_len+=(int)strlen(prefixe);

  char *c_numero=new char[prefix_len+digits+1];
  int resultat;
  c_numero[0]='_';
  if(prefixe) strcpy(c_numero+1, prefixe);
  for (int i=prefix_len; i<prefix_len+digits; i++)
    {
      resultat=n/diviseur;
      char c= (char)((int)'0' + resultat); // on old compilos, '+' is not for char, always int ...
      c_numero[i]=c;
      n-=resultat*diviseur;
      diviseur/=10;
    }
  c_numero[prefix_len+digits]='\0';
  newname+=c_numero;
  if (pas_de_point==0)
    newname+=ptr;
  Nom new_name(newname);
  delete[] c_numero;
  return new_name;
}

/*! @brief Retourne un nom selon la commande usuelle substr ATTENTION : deb = 1 => premier caractere de la chaine !!!
 *
 */
Nom Nom::substr_old(const int deb, const int la_longueur) const
{

  assert(deb > 0);
  assert(deb - 1 + la_longueur <= (int) nom_.size());
  Nom nouveau(nom_.substr(deb-1,la_longueur));
  return nouveau;
}

/*! @brief Retourne fichier si le nom est sous la forme /toto/titi/fichier
 *
 * @param (Signification)
 */
Nom Nom::basename() const
{
  Nom dirname("");
  Nom the_basename(nom_);
  int iLength = (int)nom_.size();
  for (int i=0; i<iLength; i++)
    {
      dirname+=nom_[i];
      if (nom_[i]=='/' || nom_[i]=='\\')    // slash or backslash
        {
          the_basename.suffix(dirname);
          dirname="";
        }
    }
  return the_basename;
}

/*! @brief Retourne un pointeur sur la chaine de caractere du nom
 *
 * @return (char*) pointeur sur la chaine de caractere du nom
 */
Nom::operator const char*() const
{
  return nom_.c_str();
}

/*! @brief Comparaison d'un nom avec une chaine de caractere Utilise strcmp
 *
 * @param (const Nom& un_nom)
 * @param (const char* const un_autre)
 * @return (int) 1 si les noms sont egaux, 0 sinon
 */
int operator ==(const Nom& un_nom, const char* const un_autre)
{
  int res_actu=(un_nom.nom_.compare(un_autre)==0);
#ifndef NDEBUG
  if ((!res_actu) && (Nom::check_case_non_sensitive_==1))
    {
      Nom toto(un_autre);
      if (toto.majuscule().nom_.compare(un_nom.getChar()) == 0)
        {
          Cerr << "Warning: " << un_nom << " and " << un_autre << " are they really different ?" << finl;
          Cerr << "A test in the code on a string of characters does not seem to take account of the case." << finl;
          Cerr << "Contact TRUST support by sending your data file of this calculation." << finl;
          Cerr << finl;
        }
    }
#endif
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

/*! @brief Comparaison d'un nom avec une chaine de caractere
 *
 * @param (const Nom& un_nom)
 * @param (const char* const un_autre)
 * @return (int) 1 si les noms sont differents, 0 sinon
 */
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

/*! @brief Renvoie *this;
 *
 * @return (const Nom&) reference sur le Nom
 */
const Nom& Nom::le_nom() const
{
  return *this;
}
