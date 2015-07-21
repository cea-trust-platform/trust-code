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
// File:        Paroi_std_hyd_VEF_3couches.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_std_hyd_VEF_3couches.h>

Implemente_instanciable(Paroi_std_hyd_VEF_3couches,"loi_standard_hydr_3couches_VEF",Paroi_std_hyd_VEF);


Sortie& Paroi_std_hyd_VEF_3couches::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_std_hyd_VEF_3couches::readOn(Entree& s)
{
  Paroi_std_hyd_VEF::readOn(s);
  return s ;
}

////////////////////////////////////////////////////
//
// Fonctions utiles au calcul des lois de paroi:
//
////////////////////////////////////////////////////

int Paroi_std_hyd_VEF_3couches::calculer_k_eps(double& k, double& eps , double yp, double u_star,
                                               double d_visco, double dist)
{
  // PQ : 05/04/07 : repris de l'ancienne methode de calcul
  //  a la definition des zones pres, i.e selon :
  //         valmax  = table_hyd.val(30) => y+~30
  //   valmin  = table_hyd.val(5)  => y+~5

  if(yp<5.)
    {
      k   = 0.;
      eps = 0.;
    }
  else if(yp>30.)
    {
      double u_star_carre = u_star*u_star;

      k   = u_star_carre/sqrt(Cmu);
      eps = u_star_carre*u_star/(Kappa*dist);
    }
  else
    {
      double lm_plus = calcul_lm_plus(yp);
      double deriv = Fdypar_direct(lm_plus);
      double x = lm_plus*u_star*deriv;

      k   = x*x/sqrt(Cmu);
      eps = (k*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;
    }

  return 1;
}

