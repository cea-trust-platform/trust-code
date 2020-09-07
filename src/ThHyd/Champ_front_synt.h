/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Champ_front_synt.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_synt_included
#define Champ_front_synt_included

#include <Ch_front_var_instationnaire_dep.h>
#include <Ref_Champ_Inc_base.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_synt
//     Classe derivee de Champ_front_base
// .SECTION voir aussi
//     Champ_front_base
//////////////////////////////////////////////////////////////////////////////


class Champ_front_synt : public Ch_front_var_instationnaire_dep
{
  Declare_instanciable(Champ_front_synt);

  const double amp = 1.452762113;
  const double pi=2.*acos(0.);

public:

  virtual int initialiser(double temps, const Champ_Inc_base& inco);
  Champ_front_base& affecter_(const Champ_front_base& ch);
  virtual void mettre_a_jour(double temps);

protected :
  REF(Champ_Inc_base) ref_inco_;

  DoubleVect moyenne;
  DoubleVect dir_fluct;

  double lenghtScale; //echelle integrale espace
  int nbModes;
  double turbKinEn; //energie cinetique turbulente
  double p;
  double timeScale;//echelle integralte temps
};

#endif

