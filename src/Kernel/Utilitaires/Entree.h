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

#ifndef Entree_included
#define Entree_included

#include <Process.h>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <cstdio> // Pour EOF sur GNU >= 4.4
#include <arch.h> // pour LIBLATAFILTER int64
#include <cstdint>// Pour INT32_MAX sur GNU >= 13

using std::istream;
using std::ios;

template <typename T> class TRUST_Ref;
class TRUST_Ref_Objet_U;
class Objet_U;
class Nom;

/*! @brief Classe de definition des operateurs et methodes pour toute lecture dans un flot d'entree (fichier, clavier, tampon de communication, etc)
 *
 * @sa Sortie
 */

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

  template <typename T>
  Entree& operator>>(const TRUST_Ref<T>& ) { std::cerr << __func__ << " :: SHOULD NOT BE CALLED ! Use -> !! " << std::endl ; throw; }

  Entree& operator>>(const TRUST_Ref_Objet_U& ) { std::cerr << __func__ << " :: SHOULD NOT BE CALLED ! Use -> !! " << std::endl ; throw; }

  virtual Entree& operator>>(True_int& ob);
  virtual Entree& operator>>(long& ob);
  virtual Entree& operator>>(long long& ob);
  virtual Entree& operator>>(float& ob);
  virtual Entree& operator>>(double& ob);

  // final
  virtual Entree& operator>>(Objet_U& ob) final;

  virtual int get(True_int *ob, std::streamsize n);
  virtual int get(long *ob, std::streamsize n);
  virtual int get(long long *ob, std::streamsize n);
  virtual int get(float *ob, std::streamsize n);
  virtual int get(double *ob, std::streamsize n);

  virtual int get(char *buf, std::streamsize bufsize);

  virtual int eof();
  virtual int jumpOfLines();
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
  bool is_different_int_size_; // File with int32 (resp. int64) whereas binary version is int64 (resp. int32)
  int check_types_;
  Error_Action error_action_;
  int diffuse_; // By default 1, but some child classes (eg: LecFicDiffuse) could set temporary to 0 to not diffuse to other processes

private:
  istream *istream_;

  template <typename _TYPE_>
  int get_template(_TYPE_ *ob, std::streamsize n);

  template <typename _TYPE_>
  Entree& operator_template(_TYPE_& ob);
};

int is_a_binary_file(Nom&);

void convert_to(const char *s, True_int& ob);
void convert_to(const char *s, long& ob);
void convert_to(const char *s, long long& ob);
void convert_to(const char *s, float& ob);
void convert_to(const char *s, double& ob);

template<typename _TYPE_>
int Entree::get_template(_TYPE_ *ob, std::streamsize n)
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
  static constexpr bool IS_INT  = std::is_same<_TYPE_,True_int>::value,
                        IS_LONG = std::is_same<_TYPE_,std::int64_t>::value;

  assert(istream_!=0);
  if (bin_)
    {
      // Have the current binary and the file we are trying to read the same bit-ness (not worrying about double,float,etc.) ?
      if (is_different_int_size_ && (IS_INT || IS_LONG))
        {
          // No, then two cases:
          // 1. binary is 64b and file is 32b -> this is always OK, just need True_int to make sure we really read a 32b value
          // 2. binary is 32b and file is 64b -> only OK if read value is actually within the 32b range
#ifdef INT_is_64_   // Case 1
          if (IS_INT || IS_LONG)  // a 'if (constexpr ...)' really - just to make sure compiler doesn't see the following lines for _TYPE_=float or double,etc.
            {
              True_int pr;
              char * ptr = (char*) &pr;
              istream_->read(ptr, sizeof(True_int));
              ob=(_TYPE_)pr;
            }
#else              // Case 2
          long pr;
          char *ptr = (char*) &pr;
          istream_->read(ptr, sizeof(long));
          if (pr > INT32_MAX)
            {
              std::cerr << "Can't read this int64 binary file with an int32 binary: overflow." << std::endl;
              Process::exit();
            }
          // It's ok, we passed the check above, we can safely downcast:
          ob = (_TYPE_)pr;
#endif
        }
      else  // File has the same bit-ness as binary, or we are trying to read a non-problematic type - all OK.
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
