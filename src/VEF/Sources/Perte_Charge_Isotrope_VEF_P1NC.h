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
// File:        Perte_Charge_Isotrope_VEF_P1NC.h
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Perte_Charge_Isotrope_VEF_P1NC_included
#define Perte_Charge_Isotrope_VEF_P1NC_included

#include <Perte_Charge_VEF.h>
#include <Parser_U.h>


//!  Perte de charge isotrope (proportionnelle a -u )
/**
   du/dt = - lambda(Re,x,y,z,t) * u * ||u|| / 2 Dh

   Lecture des arguments :

   Perte_Charge_Isotrope_VEF_P1NC diametre_hydraulique {
   lambda expression(Re,x,y,z,t)
   diam_hydr champ_don
   [sous_zone nom]
   }
*/

class Perte_Charge_Isotrope_VEF_P1NC : public Perte_Charge_VEF
{
  Declare_instanciable(Perte_Charge_Isotrope_VEF_P1NC);

public:

  void mettre_a_jour(double temps)
  {
    diam_hydr->mettre_a_jour(temps);
  }

protected:

  //! Implemente le calcul effectif de la perte de charge pour un lieu donne
  virtual void coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos,
                                   double t, double norme_u, double dh, double nu, double reynolds,
                                   double& coeff_ortho, double& coeff_long, double& u_l, DoubleVect& v_valeur) const;

private:

  mutable Parser_U lambda;

};

#endif
