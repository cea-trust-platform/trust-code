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
// File:        Diffu_scal_lm.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Diffu_scal_lm.h>
#include <Fluide_Incompressible.h>
#include <MuLambda_TBLE_base.h>

Implemente_instanciable(Diffu_scal_lm,"Diffu_scal_lm",Diffu_totale_scal_base);

Sortie& Diffu_scal_lm::printOn(Sortie& os) const
{
  return os ;
}

Entree& Diffu_scal_lm::readOn( Entree& is)
{
  return is ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Cette methode determine la diffusivite totale a en un point donne du maillage fin de Eq_couch_lim ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Si la composante du cisaillement a la paroi est dans le sens de l'ecoulement

double Diffu_scal_lm::calculer_a_local(int ind)
{
  double a ;// diffusivite totale
  Eq_couch_lim& leq = eq_couch_lim.valeur();

  double visco_cin = mu_lambda->getNu(leq, ind);
  double diffusivite_thermique = mu_lambda->getAlpha(leq, ind);

  /*
    double T = leq.get_Unp1(0,ind);
    Cout << "T " << T<< finl;
    Cout << "visco_cin " << visco_cin<< finl;
    Cout << "diffusivite_thermique " << diffusivite_thermique<< finl;
  */

  if(ind==0)
    {
      a = diffusivite_thermique;
    }
  else
    {
      a = diffusivite_thermique + (visco_tot.valeur().calculer_a_local(ind)-visco_cin)/getPrandtl();
    }
  return a;
}

double Diffu_scal_lm::calculer_D_local(int ind)
{

  return 0;
}
