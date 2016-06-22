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
// File:        Diffu_laminaire.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Diffu_laminaire.h>
#include <Fluide_Incompressible.h>
#include <Champ_Don.h>
#include <MuLambda_TBLE_base.h>

Implemente_instanciable(Diffu_laminaire,"laminaire",Diffu_totale_hyd_base);

Sortie& Diffu_laminaire::printOn(Sortie& os) const
{
  return os ;
}

Entree& Diffu_laminaire::readOn( Entree& is)
{
  return is ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Cette methode determine la diffusivite totale a en un point donne du maillage fin de Eq_couch_lim ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Si la composante du cisaillement a la paroi est dans le sens de l'ecoulement

double Diffu_laminaire::calculer_a_local(int ind)
{
  double visco_cin = mu_lambda->getNu(eq_couch_lim_T, ind);

  return visco_cin;
}
