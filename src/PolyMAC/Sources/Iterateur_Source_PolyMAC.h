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

#ifndef Iterateur_Source_PolyMAC_included
#define Iterateur_Source_PolyMAC_included


//
// .DESCRIPTION class Iterateur_Source_PolyMAC
//
// .SECTION voir aussi
//
#include <Iterateur_Source_PolyMAC_base.h>
#include <ItSouPolyMACFa.h>

Declare_deriv(Iterateur_Source_PolyMAC_base);

class Iterateur_Source_PolyMAC : public DERIV(Iterateur_Source_PolyMAC_base)
{

  Declare_instanciable(Iterateur_Source_PolyMAC);

public:

  inline Iterateur_Source_PolyMAC(const Iterateur_Source_PolyMAC_base&);
  inline void completer_();
  inline DoubleTab& ajouter(DoubleTab& ) const;
  inline DoubleTab& calculer(DoubleTab& ) const;
  inline Evaluateur_Source_PolyMAC& evaluateur();
  inline void associer(const Source_base& source);
};


//
//  Fonctions inline de la classe Iterateur_Source_PolyMAC
//

inline Iterateur_Source_PolyMAC::Iterateur_Source_PolyMAC(const Iterateur_Source_PolyMAC_base& Opb)

  : DERIV(Iterateur_Source_PolyMAC_base)()
{
  DERIV(Iterateur_Source_PolyMAC_base)::operator=(Opb);
}

inline void Iterateur_Source_PolyMAC::completer_()
{
  valeur().completer_();
}

inline Evaluateur_Source_PolyMAC& Iterateur_Source_PolyMAC::evaluateur()
{
  return valeur().evaluateur();
}

inline DoubleTab& Iterateur_Source_PolyMAC::ajouter(DoubleTab& resu) const
{
  return valeur().ajouter(resu);
}

inline DoubleTab& Iterateur_Source_PolyMAC::calculer(DoubleTab& resu) const
{
  return valeur().calculer(resu);
}

inline void Iterateur_Source_PolyMAC::associer(const Source_base& source)
{
  return valeur().associer(source);
}
#endif
