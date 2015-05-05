/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Objet_a_lire.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Objet_a_lire_included
#define Objet_a_lire_included
#include <Objet_U.h>
#include <List_Nom.h>
#include <ArrOfInt.h>
#include <ptrParam.h>

class ArrOfDouble;
class Param;
class Objet_a_lire :public Objet_U
{
  Declare_instanciable(Objet_a_lire);
public:
  enum Type { INTEGER = 0,
              DOUBLE  = 1,
              OBJECT  = 2,
              FLAG    = 3,
              NON_STD = 4,
              DERIV   = 5,
              ArrOfInt_size_imp = 6,
              ArrOfDouble_size_imp = 7,
              PARAM = 8
            };

  enum Nature { OPTIONAL = 0,
                REQUIRED = 1
              };

  void set_entier(int*);
  void set_double(double*);
  void set_objet(Objet_U*);
  void set_arrofint(ArrOfInt*);
  void set_arrofdouble(ArrOfDouble*);
  void set_deriv(Deriv_*, const char *prefixe);
  Param& create_param(const char*);
  void set_flag(int*);
  void set_non_std(Objet_U*);
  ptrParam& add_dict(const char*,int, const char* =0);
  void set_nature(Objet_a_lire::Nature n);
  void read(const Motcle& keyword,Entree& is);
  void print(Sortie& s) const;
  const Nom& get_name(void) const;
  int comprend_name(Motcle& mot) const;
  Nom get_names_message(void) const;
  void set_name(const LIST(Nom)& n);
  bool is_optional(void) const;
  bool is_type_simple() const ;
  double get_value() const;

protected:
  Nom name;
  LIST(Nom) names;
  Objet_a_lire::Type type;
  Objet_a_lire::Nature nature;
  int * int_a_lire;
  double* double_a_lire;
  Objet_U *obj_a_lire,*objet_lu ;
  ArrOfInt* arrofint_a_lire;
  ArrOfDouble* arrofdouble_a_lire;
  int * flag_a_lire;
  Motcles dictionnaire_noms;
  ArrOfInt dictionnaire_valeurs;
  VECT(ptrParam) dictionnaire_params;
  Motcle prefixe_deriv;
  ptrParam param_interne;
};
Declare_liste(Objet_a_lire);
#endif
