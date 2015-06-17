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
// File:        Op_EF_base.h
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_EF_base_included
#define Op_EF_base_included

#include <DoubleTab.h>
#include <Matrice_Morse.h>

//class Matrice_Morse;
class Zone_EF;
class Zone_Cl_EF;
class DoubleTab;
class Equation_base;
class Sortie;
class Operateur_base;

class Op_EF_base
{
public :
  void dimensionner(const Zone_EF&, const Zone_Cl_EF&, Matrice_Morse&) const;
  void modifier_pour_Cl(const Zone_EF&, const Zone_Cl_EF&, Matrice_Morse&, DoubleTab&)const ;
  void modifier_flux(const Operateur_base&) const;
  int impr(Sortie&, const Operateur_base&) const;
  int elem_contribue(const int& elem) const;
  void marque_elem( const Equation_base& eqn);
protected:
  Matrice_Morse matrice_sto_;
private:
  mutable int controle_modifier_flux_;
  ArrOfInt marqueur_elem_;
};

#endif



