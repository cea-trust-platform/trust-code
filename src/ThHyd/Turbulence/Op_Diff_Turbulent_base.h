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
// File:        Op_Diff_Turbulent_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_Turbulent_base_included
#define Op_Diff_Turbulent_base_included

#include <Ref_Champ_Fonc.h>
#include <Ref_Turbulence_paroi.h>

class DoubleVect;
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Op_Diff_Turbulent_base
//     Classe de base pour les operateurs de diffusion pour un ecoulement
//     turbulent.
// .SECTION voir aussi
//     Operateur_Diff_base
//     Classe abstraite
//     Methodes abstraites
//       void associer_diffusivite_turbulente(const Champ_Fonc& )
//       void associer_loipar(const Turbulence_paroi& )
//////////////////////////////////////////////////////////////////////////////
class Op_Diff_Turbulent_base
{
public :
  virtual ~Op_Diff_Turbulent_base() {};
  void associer_diffusivite_turbulente(const Champ_Fonc& );

protected:

  REF(Champ_Fonc) la_diffusivite_turbulente;
};


#endif
