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
// File:        Iterateur_Source_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Sources/Iterateurs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Iterateur_Source_VEF_included
#define Iterateur_Source_VEF_included


//
// .DESCRIPTION class Iterateur_Source_VEF
//
// .SECTION voir aussi
//
#define It_Sou_VEF_Face(_TYPE_) name2(Iterateur_Source_VEF_Face, _TYPE_)
#include <MItSouVEFFa.h>
#include <Iterateur_Source_VEF_base.h>

Declare_deriv(Iterateur_Source_VEF_base);

class Iterateur_Source_VEF : public DERIV(Iterateur_Source_VEF_base)
{

  Declare_instanciable(Iterateur_Source_VEF);

public:

  inline Iterateur_Source_VEF(const Iterateur_Source_VEF_base&);
  inline void completer_();
  inline DoubleTab& ajouter(DoubleTab& ) const;
  inline DoubleTab& calculer(DoubleTab& ) const;
  inline Evaluateur_Source_VEF& evaluateur();
  inline void associer(const Source_base& source);
};


//
//  Fonctions inline de la classe Iterateur_Source_VEF
//

inline Iterateur_Source_VEF::Iterateur_Source_VEF(const Iterateur_Source_VEF_base& Opb)

  : DERIV(Iterateur_Source_VEF_base)(Opb) {}

inline void Iterateur_Source_VEF::completer_()
{
  valeur().completer_();
}

inline Evaluateur_Source_VEF& Iterateur_Source_VEF::evaluateur()
{
  return valeur().evaluateur();
}

inline DoubleTab& Iterateur_Source_VEF::ajouter(DoubleTab& resu) const
{
  return valeur().ajouter(resu);
}

inline DoubleTab& Iterateur_Source_VEF::calculer(DoubleTab& resu) const
{
  return valeur().calculer(resu);
}

inline void Iterateur_Source_VEF::associer(const Source_base& source)
{
  return valeur().associer(source);
}

#endif
