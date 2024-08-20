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

#ifndef Cond_lim_included
#define Cond_lim_included

#include <Cond_lim_base.h>
#include <TRUST_Deriv.h>

/*! @brief classe Cond_lim Classe generique servant a representer n'importe quelle classe
 *
 *     derivee de la classe Cond_lim_base
 *     La plupart des methodes appellent les methodes de l'objet Probleme
 *     sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Cond_lim_base
 */
class Cond_lim : public DERIV(Cond_lim_base)
{
  Declare_instanciable(Cond_lim);
public:
  Cond_lim(const Cond_lim&) = default;
  inline Cond_lim& operator=(const Cond_lim_base& cl_base);
  inline Cond_lim& operator=(const Cond_lim& cl);
  void adopt(Cond_lim& cl);
  inline int operator == (const Cond_lim& x) const;
};

inline Cond_lim& Cond_lim::operator=(const Cond_lim_base& cl_base)
{
  DERIV(Cond_lim_base)::operator=(cl_base);
  return *this;
}

inline Cond_lim& Cond_lim::operator=(const Cond_lim& cl)
{
  DERIV(Cond_lim_base)::operator=(cl.valeur());
  return *this;
}

inline int Cond_lim::operator == (const Cond_lim& x) const
{
  return (DERIV(Cond_lim_base)&) x == (DERIV(Cond_lim_base)&) *this ;
}

#endif
