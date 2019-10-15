/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Fonc_MED_Tabule.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_MED_Tabule_included
#define Champ_Fonc_MED_Tabule_included

#include <Champ_Fonc_MED.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Champ_Fonc_MED_Tabule
//
// <Description of class Champ_Fonc_MED_Tabule>
//
/////////////////////////////////////////////////////////////////////////////

class Champ_Fonc_MED_Tabule : public Champ_Fonc_MED
{

  Declare_instanciable( Champ_Fonc_MED_Tabule ) ;

public :
  void mettre_a_jour(double temps);
protected :
  DoubleTab tab1_,tab2_;
  double temps1_,temps2_,temps_calc_;

};

#endif /* Champ_Fonc_MED_Tabule_included */
