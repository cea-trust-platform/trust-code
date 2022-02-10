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
// File:        Echange_interne_parfait.h
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_interne_parfait_included
#define Echange_interne_parfait_included

#include <Echange_interne_impose.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_interne_parfait:
//    Cette classe represente le cas particulier de la classe
//    Echange_interne_impose où h_gap=+infinity
//    En VDF la seule partie contribuant au h_tot reste les épaisseurs de demi-maille.
// .SECTION voir aussi
//    Echange_interne_parfait Echange_interne_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_interne_parfait  : public Echange_interne_impose
{

  Declare_instanciable(Echange_interne_parfait);

public:
  // Override to have h_gap=+infinity:
  double calcul_h_imp(const double h_gap, const double invLambda) const override;
};


#endif
