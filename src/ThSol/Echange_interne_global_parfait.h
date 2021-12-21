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
// File:        Echange_interne_global_parfait.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_interne_global_parfait_included
#define Echange_interne_global_parfait_included

#include <Echange_interne_global_impose.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_interne_global_parfait
//    Cette classe represente le cas particulier d'un échange interne parfait (h=+inf)
//    Marche seulement en EF pour le moment.
//
// .SECTION voir aussi
//    Echange_impose_base Echange_interne_global_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_interne_global_parfait : public Echange_interne_global_impose
{

  Declare_instanciable(Echange_interne_global_parfait);
public:
  int compatible_avec_discr(const Discretisation_base& discr) const;

};

#endif
