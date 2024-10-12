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

#include <Objet_U.h>
#include <Sortie.h>
#include <Separateur.h>
#include <iostream>
#include <iomanip>
#include <cstring>

const Separateur finl(Separateur::ENDL);
const Separateur tspace(Separateur::SPACE);

void Sortie::setf(IOS_FORMAT code)
{
  if(ostream_)
    ostream_->setf(code);
}

void Sortie::precision(int pre)
{
  if(ostream_)
    ostream_->precision(pre);
}

Sortie::Sortie(ostream& os)
{
  if(os.rdbuf())
    ostream_ = std::make_unique<ostream>(os.rdbuf());
  else
    Process::exit();
}

Sortie::Sortie(const Sortie& os)
{
  if (os.has_ostream())
    {
      Cerr<<"we try to copy a Sortie with a non-empty std::ostream!!!"<<finl;
      abort();
    }
}

// Operateurs d'affectation
Sortie& Sortie::operator=(ostream& os)
{
  // Make a new ostream_:
  ostream_ = std::make_unique<ostream>(os.rdbuf());
  return *this;
}

Sortie& Sortie::operator=(Sortie& os)
{
  if (os.has_ostream())
    {
      Cerr<<"we try to copy a Sortie with a non-empty std::stream!!!"<<finl;
      abort();
    }
  else  // We are copying from a void Sortie - make sure we clean our side:
    ostream_ = nullptr;
  return *this;
}

int Sortie::add_col(const double ob)
{
  if (bin_ or col_width_ == -1)
    abort();
  else
    (*ostream_) << std::right << std::setw(col_width_) << ob;
  return ostream_->good();
}

int Sortie::add_col(const char * ob)
{
  if (bin_ or col_width_ == -1)
    abort();
  else
    (*ostream_) << std::right << std::setw(col_width_) << ob;
  return ostream_->good();
}

int Sortie::put(const unsigned * ob, std::streamsize n, std::streamsize nb_col) { return put_template<unsigned>(ob,n,nb_col); }
int Sortie::put(const True_int * ob, std::streamsize n, std::streamsize nb_col) { return put_template<True_int>(ob,n,nb_col); }
int Sortie::put(const long * ob, std::streamsize n, std::streamsize nb_col) { return put_template<long>(ob,n,nb_col); }
int Sortie::put(const float * ob, std::streamsize n, std::streamsize nb_col) { return put_template<float>(ob,n,nb_col); }
int Sortie::put(const double * ob, std::streamsize n, std::streamsize nb_col) { return put_template<double>(ob,n,nb_col); }

Sortie& Sortie::operator<<(const True_uint ob) { return operator_template<True_uint>(ob); }
Sortie& Sortie::operator<<(const True_int ob) { return operator_template<True_int>(ob); }
Sortie& Sortie::operator<<(const float ob) { return operator_template<float>(ob); }
Sortie& Sortie::operator<<(const double ob) { return operator_template<double>(ob); }
Sortie& Sortie::operator<<(const long ob) { return operator_template<long>(ob); }
Sortie& Sortie::operator<<(const unsigned long ob) { return operator_template<unsigned long>(ob); }

Sortie& Sortie::operator <<(ostream& (*f)(ostream&))
{
  // Ca c'est pas genial, c'est pour permettre  "<< endl"
  // Probleme: ca ne marche pas si ostream_ == 0.
  if(ostream_ && !bin_)
    (*f)(*ostream_);
  return *this;
}

Sortie& Sortie::operator <<(Sortie& (*f)(Sortie&))
{
  (*f)(*this);
  return *this;
}

Sortie& Sortie::operator <<(ios& (*f)(ios&))
{
  if (ostream_)
    (*f)(*ostream_);
  return *this;
}

Sortie& Sortie::flush()
{
  ostream_->flush();
  return *this;
}

Sortie& Sortie::operator<<(const Separateur& ob)
{
  if (bin_)
    {
      // En binaire on n'ecrit pas les separateurs
    }
  else
    {
      switch (ob.get_type())
        {
        case Separateur::ENDL:
          // endl = '\n' + flush
#if defined(__CYGWIN__) || defined(MICROSOFT)
          // GF pb sous windows avec ancienne ligne
          (*ostream_)<<endl;
#else
          (*ostream_) << '\n';
#endif
          // Flush eventuel (surcharge possible par Sortie_Fichier_base::flush())
          flush();
          break;
        case Separateur::SPACE:
          (*ostream_) << ' ';
          break;
        }
    }
  return *this;
}

Sortie& Sortie::lockfile()
{
  Cerr<<"Sortie::lockfile() : we should not go through it !!"<<finl;
  Process::exit();
  return *this;
}

Sortie& Sortie::unlockfile()
{
  Cerr<<"Sortie::unlockfile() : we should not go through it !!"<<finl;
  Process::exit();
  return *this;
}

Sortie& Sortie::syncfile()
{
  Cerr<<"Sortie::syncfile() : we should not go through it !!"<<finl;
  Process::exit();
  return *this;
}

/*! @brief Ecriture d'un objet ou d'une variable.
 *
 * Dans cette implementation (et dans la plupart des classes derivees)
 *   on appelle simplement ob.printOn (a l'exception de Sortie_Nulle)
 *   Attention, si on veut que le flux puisse etre indifferemment ASCII ou BINAIRE,
 *   il faut inserer "<< space <<"  ou "<< finl <<" pour separer les objets.
 *
 */
Sortie& Sortie::operator<<(const Objet_U& ob)
{
  ob.printOn(*this);
  return *this;
}

/*! @brief Ecriture d'une chaine de caracteres.
 *
 * Attention, pour pouvoir relire correctement la chaine en mode ascii, celle-ci ne doit
 *  pas contenir de separateur (ni espace, ni retour a la ligne, ...)
 *
 */
Sortie& Sortie::operator <<(const char* ob)
{
  if(bin_)
    {
      // Ca c'est dommage : dans LIST, il y a "<< blanc <<"
      // qui oblige a mettre ce test :
      // Il faudrait mettre "<< space <<"  mais cela change les fichiers
      // .Zones binaires...
      if (strcmp(ob, " "))
        {
          const int n = (int)strlen(ob) + 1;
          ostream_->write((char *) ob, n * sizeof(char));
        }
    }
  else
    {
      (*ostream_) << ob;
    }
  // B.Mathieu, 7/10/2004 : je supprime l'espace apres ecrire(ob).
  // Attention, maintenant, il faut faire
  //  fichier << "chaine" << space << nombre;
  // pour pouvoir relire le fichier de facon transparente avec
  //  fichier >> motcle >> nombre;
  // Ancien code:
  //   if(!bin_)
  //     ecrire(" ");
  return *this;
}

Sortie& Sortie::operator <<(const std::string& str) { return (*this) << str.c_str(); }


/*! @brief Change le mode d'ecriture du fichier.
 *
 * Cette methode peut etre appelee n'importe quand. Attention
 *   cependant pour les fichiers Ecrire_Fichier_Partage :
 *   il faut faire le changement uniquement au debut de l'ecriture
 *   d'un bloc, juste apres syncfile() (sinon, mauvaise traduction
 *   des retours a la ligne lors du syncfile suivant).
 *
 */
int Sortie::set_bin(int bin)
{
  assert(bin==0 || bin==1);
  bin_ = bin;
  if (ostream_)
    {
      Cerr<<"Error you cant change binary format after open "<<finl;
      assert(0);
      Process::exit();
    }
  return bin_;
}

