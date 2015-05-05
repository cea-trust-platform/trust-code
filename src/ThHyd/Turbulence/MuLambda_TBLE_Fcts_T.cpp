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
// File:        MuLambda_TBLE_Fcts_T.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <MuLambda_TBLE_Fcts_T.h>
#include <Eq_couch_lim.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(MuLambda_TBLE_Fcts_T,"MuLambda_TBLE_Fcts_T",MuLambda_TBLE_base);


Sortie& MuLambda_TBLE_Fcts_T::printOn(Sortie& os) const
{
  return os;
}

Entree& MuLambda_TBLE_Fcts_T::readOn(Entree& is)
{
  return is;
}


void MuLambda_TBLE_Fcts_T::setFcts(Nom& ch_mu, Nom& ch_lambda)
{
  chaine_mu = ch_mu;
  chaine_lambda = ch_lambda;
}

void MuLambda_TBLE_Fcts_T::initialiser(const Milieu_base& milieu)
{
  p_mu.setNbVar(1);
  p_mu.setString(chaine_mu);
  p_mu.addVar("val");
  p_mu.parseString();

  p_lambda.setNbVar(1);
  p_lambda.setString(chaine_lambda);
  p_lambda.addVar("val");
  p_lambda.parseString();

  if (!sub_type(Champ_Uniforme,milieu.masse_volumique().valeur()))
    {
      Cerr << "Variable volume mass is not possible yet with TBLE law." << finl;
      exit();
    }
  rho = milieu.masse_volumique().valeurs()(0,0);
  if (!sub_type(Champ_Uniforme,milieu.capacite_calorifique().valeur()))
    {
      Cerr << "Variable capacity heat exchange is not possible yet with TBLE law." << finl;
      exit();
    }
  rhoCp = rho*milieu.capacite_calorifique().valeurs()(0,0);
}

double MuLambda_TBLE_Fcts_T::getNu(REF(Eq_couch_lim) eq_T, int ind)
{
  double T = eq_T->get_Unp1(0,ind);
  p_mu.setVar(0, T);
  return p_mu.eval()/rho;
}

double MuLambda_TBLE_Fcts_T::getAlpha(REF(Eq_couch_lim) eq_T, int ind)
{
  double T = eq_T->get_Unp1(0,ind);
  p_lambda.setVar(0, T);
  return p_lambda.eval()/rhoCp;
}
