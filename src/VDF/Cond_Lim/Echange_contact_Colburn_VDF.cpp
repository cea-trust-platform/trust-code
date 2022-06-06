/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Echange_contact_Colburn_VDF.h>
#include <Probleme_base.h>
#include <Zone_VDF.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Conduction.h>

Implemente_instanciable(Echange_contact_Colburn_VDF,"Paroi_Echange_contact_Colburn_VDF",Echange_contact_Correlation_VDF);



Sortie& Echange_contact_Colburn_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_Colburn_VDF::readOn(Entree& is )
{
  return Echange_contact_Correlation_VDF::readOn(is);
}


/**
 * Calcule le coeff d echange suivant la correlation de Colburn
 */
double Echange_contact_Colburn_VDF::calculer_coefficient_echange(int i)
{
  double Re,Pr;
  Re = std::fabs(getQh()*getDh()/getMu(i));
  Pr = getMu(i)*getCp()/getLambda(i);
  return 0.023*pow(Re,0.8)*pow(Pr,1./3.)*getLambda(i)/getDh();
}


double Echange_contact_Colburn_VDF::volume(double s, double d)
{
  return s*0.1963495*d;
}


