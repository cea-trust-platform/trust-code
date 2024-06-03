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

#include <Entree_complete.h>
#include <Process.h>
#include <communications.h>
#include <communications_array.h>
#include <Nom.h>

Entree_complete::Entree_complete(const char* str_entree1, Entree& entree2)
{
  num_entree_ = 0;
  str_size_ = (int)strlen(str_entree1);
  chaine_str_.init(str_entree1);
  Process::Journal() << "Construction of a Entree_complete with : " << str_entree1 << finl;
  entree2_ = &entree2;
  num_entree_ = 0;
  Error_Action action = entree2.get_error_action();
  Entree::set_error_action(action);
  chaine_str_.set_error_action(action);
  int check_types_sa = entree2.check_types();
  Entree::set_check_types(check_types_sa);
  chaine_str_.set_check_types(check_types_sa);
}

Entree_complete::~Entree_complete() { entree2_ = 0; }

Entree& Entree_complete::get_input()
{
  if (num_entree_ == 0)
    {
      int pos = (int)chaine_str_.get_istream().tellg();
      if (pos < str_size_ && pos >= 0)
        return chaine_str_;
      num_entree_ = 1;
    }
  return *entree2_;
}

int Entree_complete::get(char* ob, int bufsize)
{
  Entree& is = get_input();
  return is.get(ob, bufsize);
}

Entree& Entree_complete::operator>>(int& ob) { return operator_template<int>(ob); }
int Entree_complete::get(int * ob, int n) { return get_template<int>(ob,n); }

#ifndef INT_is_64_
Entree& Entree_complete::operator>>(long& ob) { return operator_template<long>(ob); }
int Entree_complete::get(long * ob, int n) { return get_template<long>(ob,n); }
#endif

Entree& Entree_complete::operator>>(float& ob) { return operator_template<float>(ob); }
int Entree_complete::get(float * ob, int n) { return get_template<float>(ob,n); }

Entree& Entree_complete::operator>>(double& ob) { return operator_template<double>(ob); }
int Entree_complete::get(double * ob, int n) { return get_template<double>(ob,n); }

int Entree_complete::eof()
{
  Entree& is = get_input();
  return is.eof();
}

int Entree_complete::fail()
{
  Entree& is = get_input();
  return is.fail();
}

int Entree_complete::good()
{
  Entree& is = get_input();
  return is.good();
}

/*! @brief Il est interdit de changer de type d'entree.
 *
 * exit()
 *
 */
int Entree_complete::set_bin(int bin)
{
  if (bin)
    {
      Cerr << "Error in Entree_complete::set_bin(int bin) : not supported" << finl;
      Process::exit();
    }
  return bin;
}

/*! @brief affecte le drapeau aux deux entrees sources Attention, le drapeau de entree2 est modifie !
 *
 *   Voir Entree::set_check_types()
 *
 */
void Entree_complete::set_check_types(int flag)
{
  Entree::set_check_types(flag);
  chaine_str_.set_check_types(flag);
  entree2_->set_check_types(flag);
}

/*! @brief affecte la valeur aux deux entrees sources Attention, la valeur de entree2 est modifiee !
 *
 *   Voir Entree::set_error_action()
 *
 */
void Entree_complete::set_error_action(Error_Action action)
{
  Entree::set_error_action(action);
  chaine_str_.set_error_action(action);
  entree2_->set_error_action(action);
}
