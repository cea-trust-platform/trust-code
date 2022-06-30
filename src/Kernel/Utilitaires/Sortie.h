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

#ifndef Sortie_included
#define Sortie_included

#include <iostream>
#include <assert.h>
#include <arch.h>
#include <string>

using std::ios;
using std::ostream;
using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::flush;
using std::ends;

class Objet_U;
class Separateur;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Classe de base des flux de sortie. Elle sait ecrire des types simples
//   (entiers, flottants) et des Objet_U (via printOn de l'objet_U)
//   Attention, certains classes derivees sont paralleles: dans ce cas, il faut appeler
//   syncfile() periodiquement sur tous les processeurs. Voir class EcrFicPartage
//   Attention: pour ecrire correctement un flux a la fois en ASCII et BINAIRE,
//    il faut utiliser un Separateur (finl ou space) pour separer les objets ecrits.
// .SECTION voir aussi
//    Entree
//////////////////////////////////////////////////////////////////////////////

class Sortie
{
public:
  Sortie();
  Sortie(ostream& os);
  Sortie(const Sortie& os);

  Sortie& operator=(ostream& os);
  Sortie& operator=(Sortie& os);

  ostream& get_ostream();
  const ostream& get_ostream() const;
  void set_ostream(ostream* os);
  void set_col_width(const int );

  Sortie& operator <<(ostream& (*f)(ostream&));
  Sortie& operator <<(Sortie& (*f)(Sortie&));
  Sortie& operator <<(ios& (*f)(ios&));

  virtual Sortie& flush();
  virtual Sortie& lockfile();
  virtual Sortie& unlockfile();
  virtual Sortie& syncfile();
  virtual void setf(IOS_FORMAT);
  virtual void precision(int);

  virtual Sortie& operator<<(const Separateur& );
  virtual Sortie& operator<<(const Objet_U&    ob);
  virtual Sortie& operator<<(const int    ob);
  virtual Sortie& operator<<(const unsigned    ob);
#ifndef INT_is_64_
  virtual Sortie& operator<<(const long      ob);
  virtual Sortie& operator<<(const unsigned long      ob);
#endif
  virtual Sortie& operator<<(const float     ob);
  virtual Sortie& operator<<(const double    ob);
  virtual Sortie& operator<<(const char      * ob);
  virtual Sortie& operator<<(const std::string& str);


  virtual int add_col(const double ob);
  virtual int add_col(const char * ob);

  virtual int put(const unsigned* ob, int n, int nb_colonnes=1);
  virtual int put(const int* ob, int n, int nb_colonnes=1);
  virtual int put(const float * ob, int n, int nb_colonnes=1);
  virtual int put(const double* ob, int n, int nb_colonnes=1);
#ifndef INT_is_64_
  virtual int put(const long  * ob, int n, int nb_colonnes=1);
#endif

  virtual ~Sortie();
  virtual int set_bin(int bin);
  int is_bin();
  bool has_ostream() const;
protected:
  int bin_;
  int col_width_;
private:
  ostream * ostream_;

  template <typename _TYPE_>
  int put_template(const _TYPE_* ob, int n, int nb_col);

  template <typename _TYPE_>
  Sortie& operator_template(const _TYPE_& );
};

// Description:
//  Methode de bas niveau pour ecrire un tableau d'ints ou reels dans le stream.
//  Dans l'implementation de la classe de base, on ecrit dans ostream_.
//  En binaire on utilise ostream::write(), en ascii ostream::operato<<()
//  En ascii, on revient a la ligne chaque fois qu'on a ecrit "nb_col" valeurs et a la fin du tableau.
//  Valeur de retour : ostream_->good()
template<typename _TYPE_>
int Sortie::put_template(const _TYPE_ *ob, int n, int nb_col)
{
  assert(n >= 0);
  if (bin_)
    {
      std::streamsize sz = sizeof(_TYPE_);
      sz *= n;
      // Overflow checking :
      assert(sz / (std::streamsize) sizeof(_TYPE_) == (std::streamsize) n);
      ostream_->write((const char*) ob, sz);
    }
  else
    {
      int j = nb_col;
      for (int i = 0; i < n; i++)
        {
          (*ostream_) << (ob[i]) << (' ');
          j--;
          if (j <= 0)
            {
              (*ostream_) << (endl);
              j = nb_col;
            }
        }
      // Si on n'a pas fini pas un retour a la ligne, en ajouter un
      if (j != nb_col && n > 0) (*ostream_) << (endl);

      ostream_->flush();
    }
  return ostream_->good();
}

// Description:
//  Methode de bas niveau pour ecrire un int ou flottant dans le stream.
//  Dans l'implementation de la classe de base, on ecrit dans ostream_.
//  En binaire on utilise ostream::write(), en ascii ostream::operato<<()
template<typename _TYPE_>
Sortie& Sortie::operator_template(const _TYPE_ &ob)
{
  if (bin_)
    ostream_->write((char*) &ob, sizeof(_TYPE_)); /* Ecriture en mode binaire */
  else
    (*ostream_) << ob; /* Ecriture avec conversion en ascii */
  return *this;
}

#endif /* Sortie_included */
