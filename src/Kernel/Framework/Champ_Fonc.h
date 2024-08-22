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

#ifndef Champ_Fonc_included
#define Champ_Fonc_included

#include <Champ_Fonc_base.h>
#include <TRUST_Deriv.h>

/*! @brief classe Champ_Fonc Classe generique de la hierarchie des champs fontions du temps,
 *
 *      un objet Champ_Fonc peut referencer n'importe quel objet derivant de
 *      Champ_Fonc_base.
 *      La plupart des methodes appellent les methodes de l'objet Champ_Fonc
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Champ_Fonc_base
 */
class Champ_Fonc : public DERIV(Champ_Fonc_base), public Champ_Proto
{
  Declare_instanciable(Champ_Fonc);
public :
  Champ_Fonc(const Champ_Fonc&) = default;
  inline Champ_Fonc& operator=(const Champ_Fonc_base& ) ;
  inline Champ_Fonc& operator=(const Champ_Fonc& ) ;

  using Champ_Proto::valeurs;
  inline DoubleTab& valeurs() override ;
  inline const DoubleTab& valeurs() const override ;
};

inline DoubleTab& Champ_Fonc::valeurs()
{
  return valeur().valeurs();
}

inline const DoubleTab& Champ_Fonc::valeurs() const
{
  return valeur().valeurs();
}

inline Champ_Fonc& Champ_Fonc::operator=(const Champ_Fonc_base& ch_fonc_base)
{
  DERIV(Champ_Fonc_base)::operator=(ch_fonc_base);
  return *this;
}

inline Champ_Fonc& Champ_Fonc::operator=(const Champ_Fonc& ch_fonc)
{
  if (ch_fonc.non_nul()) DERIV(Champ_Fonc_base)::operator=(ch_fonc.valeur());
  return *this;
}

#endif
