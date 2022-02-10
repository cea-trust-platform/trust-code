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
// File:        Sortie_Nulle.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sortie_Nulle_included
#define Sortie_Nulle_included

#include <Sortie.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Classe derivee de Sortie qui ne sort les donnees nulle part (c'est une poubelle)
//   Classe utilisee dans le Journal() si celui-ci est desactive.
//   Le pointeur ostream_ est nul.
//////////////////////////////////////////////////////////////////////////////

class Sortie_Nulle : public Sortie
{
public:
  Sortie_Nulle();
  ~Sortie_Nulle() override;
  Sortie& flush() override;
  void setf(IOS_FORMAT) override;
  void precision(int) override;

  Sortie& operator <<(const Separateur& ) override;
  Sortie& operator <<(const Objet_U& ob) override;
  Sortie& operator <<(const int& ob) override;
  Sortie& operator <<(const unsigned& ob) override;
#ifndef INT_is_64_
  Sortie& operator <<(const long& ob) override;
#endif
  Sortie& operator <<(const float& ob) override;
  Sortie& operator <<(const double& ob) override;
  Sortie& operator <<(const char* ob) override;
  int put(const unsigned* ob, int n, int pas=1) override;
  int put(const int* ob, int n, int pas=1) override;
#ifndef INT_is_64_
  int put(const long* ob, int n, int pas=1) override;
#endif
  int put(const float* ob, int n, int pas=1) override;
  int put(const double* ob, int n, int pas=1) override;
};
#endif
