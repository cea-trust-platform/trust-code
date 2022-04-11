/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Sortie_Nulle.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Sortie_Nulle.h>

// Description: Constructeur par defaut (le pointeur ostream_ est nul).
Sortie_Nulle::Sortie_Nulle()
{
}

Sortie_Nulle::~Sortie_Nulle()
{
}

Sortie& Sortie_Nulle::flush()
{
  return *this;
}
void Sortie_Nulle::setf(IOS_FORMAT) {}
void Sortie_Nulle::precision(int pre) {}

Sortie& Sortie_Nulle::operator<<(const Separateur& ob)
{
  return *this;
}
Sortie& Sortie_Nulle::operator<<(const Objet_U&    ob)
{
  return *this;
}
Sortie& Sortie_Nulle::operator<<(const int    ob)
{
  return *this;
}
Sortie& Sortie_Nulle::operator<<(const unsigned    ob)
{
  return *this;
}
#ifndef INT_is_64_
Sortie& Sortie_Nulle::operator<<(const long      ob)
{
  return *this;
}
#endif
Sortie& Sortie_Nulle::operator<<(const float     ob)
{
  return *this;
}
Sortie& Sortie_Nulle::operator<<(const double    ob)
{
  return *this;
}
Sortie& Sortie_Nulle::operator<<(const char      * ob)
{
  return *this;
}

int Sortie_Nulle::put(const unsigned* ob, int n, int pas)
{
  return 1;
}
#ifndef INT_is_64_
int Sortie_Nulle::put(const int* ob, int n, int pas)
{
  return 1;
}
#endif
int Sortie_Nulle::put(const long  * ob, int n, int pas)
{
  return 1;
}
int Sortie_Nulle::put(const float * ob, int n, int pas)
{
  return 1;
}
int Sortie_Nulle::put(const double* ob, int n, int pas)
{
  return 1;
}
