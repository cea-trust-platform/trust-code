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

#ifndef Entree_complete_included
#define Entree_complete_included

#include <EChaine.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//  Cette classe se comporte comme EChaine tant que l'on n'est pas
//  a la fin de la chaine. La suite est lue dans entree2 passee en parametre.
//  check_types() et error_action() sont identiques a ceux de entree2
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class Entree_complete: public Entree
{
public:
  Entree_complete(const char *str, Entree& entree2);
  ~Entree_complete() override;

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
  int fail() override;
  int good() override;

  int set_bin(int bin) override;
  void set_error_action(Error_Action) override;
  void set_check_types(int flag) override;

protected:
  Entree& get_input();
  // Si num_entree_ == 0, on est en train de lire dans chaine_str, sinon dans entree2_
  int num_entree_;
  int str_size_;
  EChaine chaine_str_;
  // Reference a la deuxieme entree (on n'est pas proprietaire de l'objet pointe)
  Entree *entree2_;

private:
  template <typename _TYPE_>
  int get_template(_TYPE_ *ob, int n);

  template <typename _TYPE_>
  Entree& operator_template(_TYPE_&ob);
};

template<typename _TYPE_>
int Entree_complete::get_template(_TYPE_ *ob, int n)
{
  // Je fais une boucle pour permettre au tableau de valeurs d'etre a cheval sur les deux entrees:
  for (int i = 0; i < n; i++)
    {
      Entree& is = get_input();
      if (! is.get(ob+i, 1))
        return 0;
    }
  return 1;
}

template<typename _TYPE_>
Entree& Entree_complete::operator_template(_TYPE_& ob)
{
  Entree& is = get_input();
  is >> ob;
  return *this;
}

#endif /* Entree_complete_included */
