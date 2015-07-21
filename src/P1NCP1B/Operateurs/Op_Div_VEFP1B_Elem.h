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
// File:        Op_Div_VEFP1B_Elem.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Div_VEFP1B_Elem_included
#define Op_Div_VEFP1B_Elem_included

#include <Op_Div_VEF_Elem.h>

class Zone_VEF_PreP1b;
class FMatrice_Morse;


class Op_Div_VEFP1B_Elem : public Op_Div_VEF_Elem
{

  Declare_instanciable(Op_Div_VEFP1B_Elem);


public:
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_elem(const DoubleTab&,DoubleTab& ) const;
  DoubleTab& ajouter_som(const DoubleTab&, DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_aretes(const DoubleTab&, DoubleTab& ) const;
  void degres_liberte() const;
  virtual void volumique(DoubleTab& ) const;

  // Implicite
  inline void dimensionner(FMatrice_Morse& ) const {};
  inline void dimensionner(Matrice_Morse& ) const {};
  inline void modifier_pour_Cl(FMatrice_Morse&, DoubleTab&) const {};
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const {};
  inline void contribuer_a_avec(const DoubleTab&, FMatrice_Morse&) const {};
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const {};
  inline void contribuer_au_second_membre(DoubleTab& ) const {};
private:
  mutable IntVect nb_degres_liberte;
};

#endif
