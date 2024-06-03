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

#include <communications_array.h>
#include <Lec_Diffuse_base.h>
#include <Process.h>
#include <assert.h>

Implemente_base_sans_constructeur(Lec_Diffuse_base,"Lec_Diffuse_base",EFichier);

Entree& Lec_Diffuse_base::readOn(Entree& s)
{
  throw;
}

Sortie& Lec_Diffuse_base::printOn(Sortie& s) const
{
  throw;
}

/*! @brief ne fait rien (constructeur protege car c'est une classe de base)
 *
 */
Lec_Diffuse_base::Lec_Diffuse_base() { }

/*! @brief erreur
 *
 */
Lec_Diffuse_base& Lec_Diffuse_base::operator=(const Lec_Diffuse_base& )
{
  Cerr << "Error in Lec_Diffuse_base & operator=(const Lec_Diffuse_base & is)" << finl;
  Process::exit();
  return *this;
}

Entree& Lec_Diffuse_base::operator>>(int& ob) { return operator_template<int>(ob); }
int Lec_Diffuse_base::get(int* ob, int n) { return get_template<int>(ob, n); }

#ifndef INT_is_64_
Entree& Lec_Diffuse_base::operator>>(long& ob) { return operator_template<long>(ob); }
int Lec_Diffuse_base::get(long* ob, int n) { return get_template<long>(ob, n); }
#endif

Entree& Lec_Diffuse_base::operator>>(float& ob) { return operator_template<float>(ob); }
int Lec_Diffuse_base::get(float* ob, int n) { return get_template<float>(ob, n); }

Entree& Lec_Diffuse_base::operator>>(double& ob) { return operator_template<double>(ob); }
int Lec_Diffuse_base::get(double *ob, int n) { return get_template<double>(ob, n); }

int Lec_Diffuse_base::get(char *buf, int bufsize)
{
  int l = -1;
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      assert(is.get_error_action() == ERROR_CONTINUE);
      int ok = is.get(buf, bufsize);
      if (ok)
        l = (int)strlen(buf) + 1;
    }
  else if (!diffuse_)
    {
      Cerr << "Lec_Diffuse_base::get(...) can't be used with diffuse_=0 on non master process." << finl;
      Process::exit();
    }
  if (diffuse_)
    {
      envoyer_broadcast(l, 0);
      if (l > 0)
        envoyer_broadcast_array(buf, l, 0);
    }
  return error_handle(l < 0);
}

int Lec_Diffuse_base::eof()
{
  int flag = 0;
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      flag = is.eof();
    }
  if (diffuse_) envoyer_broadcast(flag, 0);
  return flag;
}

int Lec_Diffuse_base::good()
{
  int flag = 0;
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      flag = is.good();
    }
  if (diffuse_) envoyer_broadcast(flag, 0);
  return flag;
}

int Lec_Diffuse_base::fail()
{
  int flag = 0;
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      flag = is.fail();
    }
  if (diffuse_) envoyer_broadcast(flag, 0);
  return flag;
}

/*! @brief appelle get_entree_master().
 *
 * set_bin(bin)
 *
 */
int Lec_Diffuse_base::set_bin(int bin)
{
  if (Process::je_suis_maitre())
    {
      Entree& is = get_entree_master();
      bin = is.set_bin(bin);
    }
  if (diffuse_) envoyer_broadcast(bin, 0);
  Entree::set_bin(bin);
  return bin;
}

/*! @brief appelle get_entree_master().
 *
 * set_check_types(flag)
 *
 */
void Lec_Diffuse_base::set_check_types(int flag)
{
  Entree::set_check_types(flag);
  Entree& is = get_entree_master();
  is.set_check_types(flag);
}

void Lec_Diffuse_base::set_diffuse(int diffuse)
{
  assert(diffuse==0 || diffuse==1);
  diffuse_ = diffuse;
}
