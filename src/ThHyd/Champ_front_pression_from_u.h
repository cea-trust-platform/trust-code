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
// File:        Champ_front_pression_from_u.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

// .NOM champ_front_normal : Champ a la frontiere uniforme normal
// .ENTETE Trio_U VEF/Champs
// .LIBRAIRIE libchvef
// .FILE Champ_front_pression_from_u.h
// .FILE Champ_front_pression_from_u.cpp

#ifndef Champ_front_pression_from_u_included
#define Champ_front_pression_from_u_included

#include <Ch_front_var_instationnaire_dep.h>
#include <Ref_Champ_Inc_base.h>
#include <Parser_U.h>

//.DESCRIPTION  class Champ_front_pression_from_u
//
// Classe derivee de Champ_front_base qui represente les
// champs aux frontieres normaux :

//.SECTION voir aussi
// Champ_front_base

class Champ_front_pression_from_u : public Ch_front_var_instationnaire_dep
{
  Declare_instanciable(Champ_front_pression_from_u);

public:

  virtual int initialiser(double temps, const Champ_Inc_base& inco);
  void mettre_a_jour(double temps) ;
protected:
  REF(Champ_Inc_base) ref_inco_;
  Parser_U fonction_;
};

#endif
