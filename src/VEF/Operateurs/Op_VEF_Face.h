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
// File:        Op_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_VEF_Face_included
#define Op_VEF_Face_included

#include <TRUSTTrav.h>
#include <SFichier.h>
class Matrice_Morse;
class Zone_VEF;
class Zone_Cl_VEF;
class Equation_base;
class Sortie;
class Operateur_base;

class Op_VEF_Face
{
public :
  inline Op_VEF_Face():controle_modifier_flux_(0) { };
  void dimensionner(const Zone_VEF&, const Zone_Cl_VEF&, Matrice_Morse&) const;
  void modifier_pour_Cl(const Zone_VEF&, const Zone_Cl_VEF&, Matrice_Morse&, DoubleTab&)const ;
  int impr(Sortie&, const Operateur_base&) const;
  void modifier_flux(const Operateur_base&) const;
  void modifier_matrice_pour_periodique_avant_contribuer(Matrice_Morse& matrice, const Equation_base& ) const;
  void modifier_matrice_pour_periodique_apres_contribuer(Matrice_Morse& matrice, const Equation_base&) const ;
private:
  mutable int controle_modifier_flux_;
  mutable SFichier Flux, Flux_moment, Flux_sum;
};

#endif



