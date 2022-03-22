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
// File:        Precond.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Precond_included
#define Precond_included

//
// .DESCRIPTION class Precond
//  Un Precond represente n'importe qu'elle classe
//  derivee de la classe Precond_base
//
// .SECTION voir aussi
//  SSOR Precond_base

#include <Precond_base.h>
#include <Deriv.h>

Declare_deriv(Precond_base);

class Precond : public DERIV(Precond_base)
{
  Declare_instanciable(Precond);
public:
  inline int preconditionner(const Matrice_Base&, const DoubleVect&, DoubleVect&);
  inline int supporte_matrice_morse_sym() { return valeur().supporte_matrice_morse_sym(); }
};

inline int Precond::preconditionner(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution)
{
  // B.M. pour ne pas modifier le comportement d'origine, debrancher le champ si on passe par cette methode:
  Precond_base& p = valeur();
  return p.preconditionner(matrice,secmem,solution);
}

#endif /* Precond_included */
