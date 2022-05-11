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
// File:        Lec_Diffuse_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Lec_Diffuse_base_included
#define Lec_Diffuse_base_included

#include <communications.h>
#include <EFichier.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Classe de base des entrees diffusees: le processeur maitre lit
//   les donnees dans la classe get_entree_master() et les diffuse
//   sur tous les processeurs.
//   Attention, les methodes operator>>(), get(), eof(), good() et bad()
//   doivent etre appelees simultanement sur tous les processeurs.
//   Les classes derivees doivent reimplementer get_entree_master().
//   La methode get_entree_master() doit renvoyer une reference a l'entree
//   qui sert de source sur le processeur maitre, elle n'est jamais appellee
//   sur les autres processeurs.
//   Attention: l'entree source doit avoir set_error_action(ERROR_CONTINUE)
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

//    Since 1.7.0 version, Lec_Diffuse_base class inherits EFichier class not Entree class
class Lec_Diffuse_base: public EFichier
{
  Declare_base_sans_constructeur(Lec_Diffuse_base);
public:
  Entree& operator>>(int& ob) override;
#ifndef INT_is_64_
  Entree& operator>>(long& ob) override;
#endif
  Entree& operator>>(float& ob) override;
  Entree& operator>>(double& ob) override;

  int get(int *ob, int n) override;
#ifndef INT_is_64_
  int get(long *ob, int n) override;
#endif
  int get(float *ob, int n) override;
  int get(double *ob, int n) override;
  int get(char *buf, int bufsize) override;

  int eof() override;
  int good() override;
  int fail() override;

  int set_bin(int bin) override;
  void set_check_types(int flag) override;

  void set_diffuse(int diffuse) override;

protected:
  Lec_Diffuse_base();
  Lec_Diffuse_base(const Lec_Diffuse_base&) = default;
  Lec_Diffuse_base& operator=(const Lec_Diffuse_base&);
  virtual Entree& get_entree_master() = 0;

private:
  template <typename _TYPE_>
  int get_template(_TYPE_ *ob, int n);

  template <typename _TYPE_>
  Entree& operator_template(_TYPE_&ob);
};

template <typename _TYPE_>
int Lec_Diffuse_base::get_template(_TYPE_ *ob, int n)
{
  int ok = 0;
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      assert(is.get_error_action() == ERROR_CONTINUE);
      ok = is.get(ob, n);
    }
  else if (!diffuse_)
    {
      Cerr << "Lec_Diffuse_base::get(...) can't be used with diffuse_=0 on non master process." << finl;
      Process::exit();
    }
  if (diffuse_)
    {
      envoyer_broadcast(ok, 0);
      if (ok)
        envoyer_broadcast_array(ob, n, 0);
    }
  return error_handle(!ok);
}

template <typename _TYPE_>
Entree& Lec_Diffuse_base::operator_template(_TYPE_& ob)
{
  int ok = 0;
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      assert(is.get_error_action() == ERROR_CONTINUE);
      is >> ob;
      ok = is.good();
    }
  else if (!diffuse_)
    {
      Cerr << "Lec_Diffuse_base::operator>> can't be used with diffuse_=0 on non master process." << finl;
      Process::exit();
    }
  if (diffuse_)
    {
      envoyer_broadcast(ok, 0);
      if (ok)
        envoyer_broadcast(ob, 0);
    }
  error_handle(!ok);
  return *this;
}

#endif
