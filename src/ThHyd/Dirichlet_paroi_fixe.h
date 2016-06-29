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
// File:        Dirichlet_paroi_fixe.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Dirichlet_paroi_fixe_included
#define Dirichlet_paroi_fixe_included

#include <Dirichlet_homogene.h>





//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Dirichlet_paroi_fixe
//     Represente une paroi immobile dans une equation de type Navier_Stokes.
// .SECTION voir aussi
//     Dirichlet_homogene
//    Objet compatible avec des equations du domaine de l'Hydraulique
//    ou indetermine. (voir compatible_avec_eqn(const Equation_base&) const)
//////////////////////////////////////////////////////////////////////////////
class Dirichlet_paroi_fixe  : public Dirichlet_homogene
{

  Declare_instanciable(Dirichlet_paroi_fixe);
public :
  int compatible_avec_eqn(const Equation_base&) const;

};
#endif
