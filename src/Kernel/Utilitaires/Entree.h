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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Entree.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Entree_included
#define Entree_included



#include <stdio.h>
#include <cstdio> // Pour EOF sur GNU >= 4.4
#include <iostream>
#include <assert.h>
#include <Process.h>
using std::ios;
using std::istream;
class Objet_U;
class Nom;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe de definition des operateurs et methodes pour toute lecture dans un flot d'entree
//    (fichier, clavier, tampon de communication, etc)
// .SECTION voir aussi
//    Sortie
//////////////////////////////////////////////////////////////////////////////

class Entree
{
public:
  // Constructeurs
  Entree();
  Entree(istream& is);
  Entree(const Entree& is);

  // Operateurs d'affectation
  Entree& operator=(istream& is);
  Entree& operator=(Entree& is);

  virtual istream& get_istream();
  virtual const istream& get_istream() const;
  void set_istream(istream *is);
  void set_different_int_size(bool is_different_int_size);

  Entree& operator >>(Entree& (*f)(Entree&));
  Entree& operator >>(istream& (*f)(istream&));
  Entree& operator >>(ios& (*f)(ios&));

  virtual Entree& operator>>(int& ob);
#ifndef INT_is_64_
  virtual Entree& operator>>(long& ob);
#endif
  virtual Entree& operator>>(float& ob);
  virtual Entree& operator>>(double& ob);

  virtual int get(int *ob, int n);
#ifndef INT_is_64_
  virtual int get(long *ob, int n);
#endif
  virtual int get(float *ob, int n);
  virtual int get(double *ob, int n);

  virtual int get(char *buf, int bufsize);

  virtual int eof();
  virtual int fail();
  virtual int good();
  virtual ~Entree();
  virtual int set_bin(int bin);
  int is_bin() const;
  virtual void set_check_types(int flag);
  int check_types() const;
  enum Error_Action { ERROR_EXIT, ERROR_CONTINUE, ERROR_EXCEPTION };
  virtual void set_error_action(Error_Action);
  Error_Action get_error_action();

  inline operator istream& () { return get_istream(); }
  inline istream& putback(char ch) { return get_istream().putback(ch); }
  inline istream& ignore(int n = 1, int delim = EOF) { return get_istream().ignore(n, delim); }
  inline int get_diffuse() { return diffuse_; }

  virtual void set_diffuse(int diffuse);

protected:
  // methode inline pour traiter rapidement le cas trivial. Sinon, appel a la methode virtuelle error_handle_()
  inline int error_handle(int fail_flag)
  {
    if (!fail_flag) return 1;
    else return error_handle_(fail_flag);
  }

  virtual int error_handle_(int fail_flag);
  int bin_;
  bool is_different_int_size_; // File with int32 (or int64) whereas version is int64 (or int32)
  int check_types_;
  Error_Action error_action_;
  int diffuse_; // By default 1, but some child classes (eg: LecFicDiffuse) could set temporary to 0 to not diffuse to other processes

private:
  istream *istream_;

  template <typename _TYPE_>
  int get_template(_TYPE_ *ob, int n);

  template <typename _TYPE_>
  Entree& operator_template(_TYPE_& ob);
};

int is_a_binary_file(Nom&);
Entree& operator>>(Entree&, Objet_U& ob);

void convert_to(const char *s, int& ob);
#ifndef INT_is_64_
void convert_to(const char *s, long& ob);
#endif
void convert_to(const char *s, float& ob);
void convert_to(const char *s, double& ob);

template<typename _TYPE_>
int Entree::get_template(_TYPE_ *ob, int n)
{
  static constexpr bool IS_INT = std::is_same<_TYPE_,int>::value;

  assert(istream_!=0);
  assert(n >= 0);
  if (bin_)
    {
      if (is_different_int_size_ && IS_INT)
        {
          for (int i = 0; i < n; i++) (*this) >> ob[i];
        }
      else
        {
          char *ptr = (char*) ob;
          // En binaire: lecture optimisee en bloc:
          std::streamsize sz = sizeof(_TYPE_);
          sz *= n;
          // Overflow checking :
          assert(sz / (std::streamsize )sizeof(_TYPE_) == (std::streamsize )n);
          istream_->read(ptr, sz);
          error_handle(istream_->fail());
        }
    }
  else
    {
      // En ascii : on passe par operator>> pour verifier les conversions
      // Attention : on appelle celui de cette classe, pas de la classe derivee
      for (int i = 0; i < n; i++) Entree::operator>>(ob[i]);
    }
  return (!istream_->fail());
}

template <typename _TYPE_>
Entree& Entree::operator_template(_TYPE_& ob)
{
  static constexpr bool IS_INT = std::is_same<_TYPE_,int>::value;

  assert(istream_!=0);
  if (bin_)
    {
      if (is_different_int_size_ && IS_INT)
        {
#ifdef INT_is_64_
          True_int pr;
          char * ptr = (char*) &pr;
          istream_->read(ptr, sizeof(True_int));
          ob=pr;
#else
          long pr;
          char *ptr = (char*) &pr;
          istream_->read(ptr, sizeof(long));
          if (pr > INT32_MAX)
            {
              std::cerr << "Can't read this int64 binary file with an int32 binary: overflow." << std::endl;
              Process::exit();
            }
          ob = pr;
#endif
        }
      else
        {
          char *ptr = (char*) &ob;
          istream_->read(ptr, sizeof(_TYPE_));
          error_handle(istream_->fail());
        }
    }
  else
    {
      if (check_types_)
        {
          char buffer[100];
          int ok = Entree::get(buffer, 100); // Bien appeler get de cette classe
          if (ok)
            convert_to(buffer, ob);
        }
      else
        {
          (*istream_) >> ob;
          error_handle(istream_->fail());
        }
    }
  return *this;
}

// Classe renvoyee par Entree si une exception est levee lors d'une erreur
class Entree_Sortie_Error
{
};

#endif
