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
// File:        Echange_contact_VDF_Plaque_Flux_Cte.cpp
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_VDF_Plaque_Flux_Cte.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Zone_VDF.h>
#include <Milieu_base.h>
#include <Champ_front_fonc.h>
#include <Champ_Uniforme.h>
#include <Conduction.h>
#include <Solv_TDMA.h>

Implemente_instanciable(Echange_contact_VDF_Plaque_Flux_Cte,"Paroi_Echange_contact_Plaque_Parallele_Flux_Cte_VDF",Echange_contact_Correlation_VDF);




Sortie& Echange_contact_VDF_Plaque_Flux_Cte::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_VDF_Plaque_Flux_Cte::readOn(Entree& is )
{
  return Echange_contact_Correlation_VDF::readOn(is);
}


/**
 * Calcule le coeff d echange suivant la correlation de Shah
 */
double Echange_contact_VDF_Plaque_Flux_Cte::calculer_coefficient_echange(int i)
{
  double Nu;
  //double x_star;

  //  Re = std::fabs(getQh()*getDh()/getMu(i));
  //Pr = getMu(i)*getCp()/getLambda(i);
  /*x_star = coord(i)-coord(0);

  if (x_star < 2e-4)
  {
  Nu = 1.49*pow(x_star,-0.3333);
  }
  else if (x_star > 1e-3)
  {
  Nu = 8.235+8.68*pow(1000*x_star,-0.506)*exp(-164*x_star);
  }
  else
  Nu = 1.49*pow(x_star,-0.3333)-0.4;
  */
  Nu = 8.24;
  return Nu*getLambda(i)/getDh();
}



double Echange_contact_VDF_Plaque_Flux_Cte::volume(double s, double d)
{
  return s*0.25*d;
}




