/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Symetrie.h
// Directory:   $TRUST_ROOT/src/Kernel/Cond_Lim
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Symetrie_included
#define Symetrie_included

#include <Cond_lim_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Symetrie
//    Sur les faces de symetrie on a les proprietes suivantes:
//      - composante normale de la vitesse nulle
//      - composante tangentielle de la contrainte nulle
//        (condition de glissement)
//      - les gradients des grandeurs scalaires sont nuls
//    Dans une equation de transport d'un scalaire (temperature, concentration,
//    K, epsilon), la condition Symetrie met les gradients des grandeurs
//    scalaires a 0.
//    Dans une equation de Navier_Stokes, elle impose la composante normale
//    de la vitesse nulle (condition de glissement)
// .SECTION voir aussi
//    Cond_lim_base
//////////////////////////////////////////////////////////////////////////////
class Symetrie  : public Cond_lim_base
{

  Declare_instanciable(Symetrie);

public :

  int compatible_avec_eqn(const Equation_base&) const;

};

#endif
