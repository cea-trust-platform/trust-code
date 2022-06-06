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

#ifndef Triangulation_base_included
#define Triangulation_base_included

#include <Interprete_geometrique_base.h>

//////////////////////////////////////////////////////////////////////////////
// .NOM  Triangulation_base
// .ENTETE  TRUST Geometrie
// .LIBRAIRIE
// .FILE  Triangulation_base.h
// .FILE  Triangulation_base.cpp
//
// .DESCRIPTION Triangulation_base
//     Classe destinee a factoriser l'action de triangulation des interpretes
//     de type Trianguler et Tetraedriser
//     .
// .SECTION voir aussi
//
// .CONTRAINTES
//
// .INVARIANTS
// .HTML
// .EPS
//////////////////////////////////////////////////////////////////////////////
class Triangulation_base : public Interprete_geometrique_base
{
  Declare_base(Triangulation_base);

public :

  virtual void trianguler(Zone&) const = 0;
  virtual int dimension_application() const = 0;
  Entree& interpreter_(Entree&) override;

protected :

};

#endif

