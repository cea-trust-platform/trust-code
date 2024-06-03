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

#include <Entree.h>
#include <Objet_U.h>
#include <Nom.h>
#include <errno.h>
#include <fstream>

using std::ifstream;

istream& Entree::get_istream()
{
  return *istream_;
}

const istream& Entree::get_istream() const
{
  return *istream_;
}
void Entree::set_istream(istream* is)
{
  istream_ = is;
}

void Entree::set_different_int_size(bool is_different_int_size)
{
  is_different_int_size_ = is_different_int_size;
}

Entree& Entree::operator >>(Entree& (*f)(Entree&))
{
  (*f)(*this);
  return *this;
}
Entree& Entree::operator >>(istream& (*f)(istream&))
{
  (*f)(*istream_);
  return *this;
}
Entree& Entree::operator >>(ios& (*f)(ios&))
{
  (*f)(*istream_);
  return *this;
}

Entree::Entree()
{
  bin_=0;
  is_different_int_size_=false;
  istream_ = 0;
  check_types_ = 0;
  error_action_ = ERROR_CONTINUE;
  diffuse_ = 1;
}

Entree::Entree(istream& is)
{
  bin_=0;
  is_different_int_size_=false;
  istream_ = new istream(is.rdbuf());
  check_types_ = 0;
  error_action_ = ERROR_CONTINUE;
  diffuse_ = 1;
}

Entree::Entree(const Entree& is)
{
  bin_=0;
  is_different_int_size_=false;
  istream_ = new istream(is.get_istream().rdbuf());
  check_types_ = 0;
  error_action_ = ERROR_CONTINUE;
  diffuse_ = 1;
}

// Operateurs d'affectation
Entree& Entree::operator=(istream& is)
{
  if(istream_)
    delete istream_;
  istream_ = new istream(is.rdbuf());
  return *this;
}

Entree& Entree::operator=(Entree& is)
{
  if(istream_)
    delete istream_;
  istream_ = new istream(is.get_istream().rdbuf());
  return *this;
}

/*! @brief Lecture d'une chaine dans ostream_ bufsize est la taille du buffer alloue pour ob (y compris
 *
 *   le caractere 0 final).
 *   La chaine contient toujours un 0 meme en cas d'echec.
 *   La methode renvoie 1 si la lecture est bonne, 0 sinon.
 *   Si le buffer est trop petit, pour l'instant on fait exit() mais
 *   par la suite on pourra tester: si strlen(ob)==bufsize-1, alors
 *   refaire lire() jusqu'a arriver au bout. Si le lire() suivant
 *   renvoie une chaine de longueur nulle, cela signifie que la taille de la
 *   chaine etait exactement bufsize-1.
 *   Attention: le comportement est different en binaire et en ascii.
 *    En binaire, on lit la chaine jusqu'au prochain '\0'.
 *    En ascii, on lit la chaine jusqu'au prochain separateur (espace, tab, fin ligne)
 *
 */
int Entree::get(char* ob, int bufsize)
{
  assert(istream_!=0);
  assert(bufsize > 0);
  ob[bufsize-1] = 1;
  if(bin_)
    {
      // En binaire, on lit jusqu'au prochain caractere 0
      // (y compris espaces, retours a la ligne etc)
      int i;
      for (i = 0; i < bufsize-1; i++)
        {
          (*istream_).read(ob+i, sizeof(char));
          if (!error_handle(istream_->fail()))
            ob[i] = 0;
          if (ob[i] == 0)
            break;
        }
      ob[i] = 0;
    }
  else
    {
      // L'appel suivant permet a l'operateur>> de limiter le nombre
      // de caracteres lus. On lira au maximum bufsize-1 caracteres.
      istream_->width(bufsize);
      (*istream_) >> ob;
      if (!error_handle(istream_->fail()))
        ob[0] = 0;
    }
  if (ob[bufsize-1] == 0)
    {
      // Note Benoit Mathieu:
      // Si on a rempli le buffer jusqu'au bout, c'est qu'il est probablement
      // trop petit. Lire la suite est dangereux car en ascii on ne sait pas
      // si on a pu lire pile poil la chaine (donc c'est ok), ou si la chaine
      // n'a pas ete lue en int. Il faut tester tres serieusement la stl et
      // ca depend sans doute de l'implementation. Donc, si le buffer est plein,
      // on plante le code et tant pis.
      Cerr << "Error in Entree::lire(char* ob, int bufsize) : buffer too small" << finl;
      Process::exit();
    }
  return (!istream_->fail());
}

void error_convert(const char * s, const char * type)
{
  Cerr << "Error converting a string to type " <<  type << " : string = " << s << finl;
  Process::exit();
}

/*! @brief methode de conversion
 *
 */
void convert_to(const char *s, int& ob)
{
  errno = 0;
  char * errorptr = 0;
  ob = (int)strtol(s, &errorptr, 0 /* base 10 par defaut */);
  if (errno || *errorptr != 0) error_convert(s,"int");
}

#ifndef INT_is_64_
void convert_to(const char *s, long& ob)
{
  errno = 0;
  char * errorptr = 0;
  ob = strtol(s, &errorptr, 0 /* base 10 par defaut */);
  if (errno || *errorptr != 0)  error_convert(s,"long");
}
#endif

void convert_to(const char *s, long long& ob)
{
  errno = 0;
  char * errorptr = 0;
#ifdef HPPA_11 /* NO_PROCESS */
  ob = strtol(s, &errorptr, 0 /* base 10 par defaut */);
#else /* NO_PROCESS */
#ifdef MICROSOFT
  ob = _strtoi64(s, &errorptr, 0 /* base 10 par defaut */);
#else
  ob = strtoll(s, &errorptr, 0 /* base 10 par defaut */);
#endif
#endif /* NO_PROCESS */
  if (errno || *errorptr != 0)  error_convert(s,"long long");
}

void convert_to(const char *s, float& ob)
{
  errno = 0;
  char * errorptr = 0;
#ifndef _COMPILE_AVEC_GCC_ /* NO_PROCESS */
  // PL: strtof n'existe pas semble t'il qu'avec gcc. pour le reste on utiliser strtod
  // BM: le risque, c'est qu'on ait un overflow qu'on ne detectera pas avec stdtod
  ob = strtod(s, &errorptr);
#else /* NO_PROCESS */
  ob = strtof(s, &errorptr);
#endif /* NO_PROCESS */
  if (errno || *errorptr != 0)  error_convert(s,"float");
}

void convert_to(const char *s, double& ob)
{
  errno = 0;
  char * errorptr = 0;
  ob = strtod(s, &errorptr);
  if (errno || *errorptr != 0)  error_convert(s,"double");
}

// methode virtuelle pour lire un int ou un reel. Dans cette classe de base, on lit dans le istream avec read() (si is_bin() == 1)
//   ou avec operator>>() (si is_bin() == 0). Si le drapeau check_types est mis, on appelle convert_to() pour verifier les types des objets lus.
//   Dans ce cas, pour les ints on comprend les formats 123 (decimal), 0xa345 (hexa) et autres.
//  Si une erreur se produit, on appelle error_handle_()
//  Note pour les programmeurs des classes derivees:  L'implementation de cette methode doit toujours passer par hande_error()
Entree& Entree::operator>>(double& ob) { return operator_template<double>(ob); }

// methode virtuelle pour lire un tableau d'ints ou reels (le tableau doit avoir la bonne dimension: attention pas de verification possible)
int Entree::get(double * ob, int n) { return get_template<double>(ob,n); }

Entree& Entree::operator>>(int& ob) { return operator_template<int>(ob); }
int Entree::get(int * ob, int n) { return get_template<int>(ob,n); }

Entree& Entree::operator>>(float& ob) { return operator_template<float>(ob); }
int Entree::get(float * ob, int n) { return get_template<float>(ob,n); }

#ifndef INT_is_64_
Entree& Entree::operator>>(long& ob) { return operator_template<long>(ob); }
int Entree::get(long * ob, int n) { return get_template<long>(ob,n); }
#endif

Entree& Entree::operator >>(Objet_U& ob) { return ob.readOn(*this); }

int Entree::jumpOfLines()
{
  if(istream_)
    {
      int jol = 0;
      char tmp=(char)istream_->peek();
      while(isspace(tmp))  //tmp=='\n')
        {
          if(tmp=='\n')
            jol++;
          istream_->get(tmp);
          tmp=(char)istream_->peek();
        }
      return jol;
    }
  return -1;
}

int Entree::eof()
{
  if(istream_)
    return (istream_->eof());
  else
    return -1;
}
int Entree::fail()
{
  if(istream_)
    return (istream_->fail());
  else
    return -1;
}
int Entree::good()
{
  if(istream_)
    return (istream_->good());
  else
    return -1;
}
Entree::~Entree()
{
  if(istream_)
    delete istream_;
  istream_=0;
}

/*! @brief Change le mode d'ecriture du fichier.
 *
 * Cette methode peut etre appelee n'importe quand.
 *
 */
int Entree::set_bin(int bin)
{
  assert(bin==0 || bin==1);
  bin_ = bin;
  if (istream_)
    {
      Cerr<<"Error you cant change binary format after open "<<finl;
      assert(0);
      Process::exit();
    }
  return bin_;
}

int Entree::is_bin() const
{
  return bin_;
}

/*! @brief indique si le stream doit verifier les types des objets lus (ints et nombres flottants).
 *
 * Exemple : l'entree contient 123.456 123.456
 *   int i;
 *   check_types(0);
 *   is >> i;   // i contient 123
 *   check_types(1);
 *   is >> i;   // Erreur : on lit la chaine 123.456 et on essaye de la convertir en int
 *   Voir operator>>(int &)
 *
 */
void Entree::set_check_types(int flag)
{
  check_types_ = flag;
}

/*! @brief renvoie la valeur actuelle du drapeau, voir set_check_types()
 *
 */
int Entree::check_types() const
{
  return check_types_;
}


/*! @brief Cette fonction est appellee par operateur>>, get, get_nom ouvrir, fermer, lire, etc.
 *
 * .. en cas d'echec (lorsque fail() est mis)
 *   Elle renvoie 0 s'il y a eu une erreur (passer par error_handle() qui
 *   traite en inline le cas ou il n'y a pas d'erreur), et 1 s'il n'y a pas
 *   d'erreur.
 *   (par commodite de codage des methodes qui l'utilisent, on
 *    ecrira "return error_handle(fail());"
 *   Elle peut etre configuree pour
 *    - renvoyer "0" en cas d'erreur et continuer l'execution du code
 *      (cas d'un ancien code qui ne gere pas les exceptions mais teste
 *       le drapeau fail() de temps en temps)
 *      Dans ce cas les methodes operator>> continuent l'execution du code
 *      meme en cas d'echec, le contenu des variables lues est indefini !
 *    - faire Process::exit() (cas d'une portion de code dans laquelle
 *      on ne veut pas faire de gestion d'erreur du tout et ou on suppose
 *      que tout va toujours bien)
 *    - lever une exception (permet une gestion rigoureuse des erreurs
 *      et une information utilisateur optimale en fonction du contexte)
 *   Voir aussi set_error_action()
 *
 */
int Entree::error_handle_(int fail_flag)
{
  if (!fail_flag)
    return 1;

  switch(error_action_)
    {
    case ERROR_CONTINUE:
      break;
    case ERROR_EXIT:
      Cerr << "Error while reading in Entree object. Exiting.\n";
      if (istream_)
        {
          // On n'utilise pas Entree::eof() car dans le cas d'un Lec_Fic_Dif
          // eof() est parallele et ca peut bloquer.
          if (get_istream().eof())
            Cerr << " End of file reached." << finl;
          else
            Cerr << " IO error (not an EOF error)." << finl;
        }
      Process::exit();
      break;
    case ERROR_EXCEPTION:
      Entree_Sortie_Error e;
      throw (e);
    }
  return 0;
}

/*! @brief renvoie error_action_ pour cette entree (permet de la modifier et de restaurer ensuite la valeur anterieure)
 *
 */
Entree::Error_Action Entree::get_error_action()
{
  return error_action_;
}

/*! @brief Change le comportement en cas d'erreur de l'entree, voir error_handle_() et get_error_action()
 *
 */
void Entree::set_error_action(Entree::Error_Action action)
{
  error_action_ = action;
}

// Detection sur un fichier de nom filename
// est de type binaire
int is_a_binary_file(Nom& filename)
{
  // On parcourt les 1000 premiers octets
  // Detection tres imparfaite donc limite
  // aux fichiers geom de TRUST
  int n=0;
  int c;
  std::ifstream fic(filename.getChar());
  // Si on rencontre un caractere ASCII>127
  // alors le fichier est de type binaire
  while((c = fic.get()) != EOF && n++<1000)
    if ((c>127)||(c<9))
      return 1;
  // GF sous windows les caracteres binaire sont surtout <9
  //  else printf("ici %d %c \n",c,c);
  return 0;
}

/*! @brief ToDo TMA : commenter
 *
 */
void Entree::set_diffuse(int diffuse)
{
  // virtual method does nothing ; cf Lec_Diffuse_base
  diffuse_ = 1;
}
