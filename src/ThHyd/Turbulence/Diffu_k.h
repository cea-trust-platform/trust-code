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
// File:        Diffu_k.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Diffu_k_included
#define Diffu_k_included
#include <Diffu_totale_hyd_base.h>
#include <Eq_couch_lim.h>
#include <Ref_Mod_echelle_LRM_base.h>

class Mod_echelle_LRM_base ;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Diffu_k
//    Calsse derivant de la classe Diffu_totale_hyd__base et specifiant la valeur
//    de la diffusivite_totale
//
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class Diffu_k : public Diffu_totale_hyd_base
{
  Declare_instanciable(Diffu_k);

public :

  double calculer_a_local(int ind);
  void associer_modele_echelles(Mod_echelle_LRM_base& mod)
  {
    mod_echelle_LRM_base = mod;
  };
  double calculer_D_local(int ind)
  {
    return 1.;
  };

protected:
  REF(Mod_echelle_LRM_base) mod_echelle_LRM_base;
};

#endif
