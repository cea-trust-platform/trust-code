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
// File:        Paroi_Knudsen_non_negligeable.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Paroi_Knudsen_non_negligeable_included
#define Paroi_Knudsen_non_negligeable_included

#include <Dirichlet_paroi_defilante.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Paroi_Knudsen_non_negligeable
//    Impose un saut de vitesse de paroi dans une equation de type Navier_Stokes
//    proportionnel au gradient de vitesse transverse.
// .SECTION voir aussi
//    Dirichlet
//    Objet compatible avec des equations du domaine de l'Hydraulique
//    ou indetermine. (voir compatible_avec_eqn(const Equation_base&) const)
//////////////////////////////////////////////////////////////////////////////
class Paroi_Knudsen_non_negligeable : public Dirichlet_paroi_defilante
{

  Declare_instanciable(Paroi_Knudsen_non_negligeable);

public :
  inline const DoubleTab& vitesse_paroi()
  {
    return vitesse_paroi_.valeurs();
  };
  inline const DoubleTab& k()
  {
    return k_.valeurs();
  } ;
  int compatible_avec_eqn(const Equation_base&) const;
  void completer();
  Champ_front vitesse_paroi_;
  Champ_front k_;
};

#endif
