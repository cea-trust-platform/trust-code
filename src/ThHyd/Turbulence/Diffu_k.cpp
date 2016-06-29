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
// File:        Diffu_k.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Diffu_k.h>
#include <Fluide_Incompressible.h>
#include <Champ_Don.h>
#include <Mod_echelle_LRM_base.h>
#include <MuLambda_TBLE_base.h>

Implemente_instanciable(Diffu_k,"Diffu_k",Diffu_totale_hyd_base);

Sortie& Diffu_k::printOn(Sortie& os) const
{
  return os ;
}

Entree& Diffu_k::readOn( Entree& is)
{
  return is ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Cette methode determine la diffusivite totale a en un point donne du maillage fin de Eq_couch_lim ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Si la composante du cisaillement a la paroi est dans le sens de l'ecoulement

double Diffu_k::calculer_a_local(int ind)
{

  double yw, Uw, vv_bar, l_mu, a;
  int N;

  Eq_couch_lim& leq = eq_couch_lim.valeur();
  Mod_echelle_LRM_base& ech= mod_echelle_LRM_base.valeur();
  //Mod_echelle_LRM_base& echv= vv_bar.valeur();

  double visco_cin = mu_lambda->getNu(eq_couch_lim_T, ind);
  double visco_turb = 0.;
  double u=0.;

  N = leq.get_N();

  if(ind==0)
    {
      a = visco_cin;
    }

  else
    {
      // Attention si ind=N, depassement de tableau
      if (ind==N)
        {
          yw=leq.get_y(ind);
          Uw=leq.get_Unp1(dimension-1,ind); //k

        }

      else
        {
          yw=0.5*(leq.get_y(ind+1)+leq.get_y(ind));
          Uw=0.5*(leq.get_Unp1(dimension-1,ind+1)+leq.get_Unp1(dimension-1,ind)); //k
        }

      if (Uw<0.)
        {
          a = visco_cin + 1.e-10;
        }

      else
        {
          vv_bar=ech.calculer_vv_bar(yw, Uw, u, visco_cin);
          l_mu=ech.calculer_l_mu(yw, Uw, u, visco_cin);
          visco_turb =l_mu*sqrt(vv_bar);
          a = visco_cin + visco_turb;
        }
    }

  return a;

}



/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//  La viscosite turbulente est calculee grace a un modele k_epsilon :                   //
//  nu_t = l_mu*sqrt(vv_bar)                                                      //
//                                                                                      //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////
