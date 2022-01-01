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
// File:        Iterateur_Source_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Iterateurs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Iterateur_Source_VDF_included
#define Iterateur_Source_VDF_included

#include <Iterateur_Source_VDF_base.h>
#include <ItSouVDFFa.h>

Declare_deriv(Iterateur_Source_VDF_base);
class Iterateur_Source_VDF : public DERIV(Iterateur_Source_VDF_base)
{
  Declare_instanciable(Iterateur_Source_VDF);
public:
  Iterateur_Source_VDF(const Iterateur_Source_VDF_base& Opb) : DERIV(Iterateur_Source_VDF_base)()
  { DERIV(Iterateur_Source_VDF_base)::operator=(Opb); }

  inline void completer_() { valeur().completer_(); }
  inline void associer(const Source_base& source) { return valeur().associer(source); }
  inline DoubleTab& ajouter(DoubleTab& resu) const { return valeur().ajouter(resu); }
  inline DoubleTab& calculer(DoubleTab& resu) const { return valeur().calculer(resu); }
  inline Evaluateur_Source_VDF& evaluateur() { return valeur().evaluateur(); }
};

#endif /* Iterateur_Source_VDF_included */
