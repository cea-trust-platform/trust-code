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

#ifndef ptrParam_included
#define ptrParam_included

#include <TRUST_Vector.h>
#include <Objet_U.h>

class Param;
/*! @brief class Objet_a_lire : contient un nom, et une reference vers un int,double,flag,un Objet_U a lire,ou un Objet_u sur lequel il faut appeler lire_motcle_non_standard
 *
 */
class ptrParam: public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(ptrParam);
public:
  ptrParam();
  ~ptrParam() override;
  ptrParam(const ptrParam&);
  const ptrParam& operator=(const ptrParam&);
  inline int non_nul() const { return (param_!=0); }
  void create(const char*);
  Param& valeur();
  const Param& valeur() const;

  inline const Param* operator ->() const { assert(param_); return param_; }
  inline Param* operator ->() { assert(param_); return param_; }

protected:
  Param* param_;
};

#endif /* ptrParam_included */
