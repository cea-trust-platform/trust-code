/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Paroi_hyd_base_VDF_included
#define Paroi_hyd_base_VDF_included

#include <Turbulence_paroi_base.h>

#include <distances_VDF.h>
#include <TRUST_Ref.h>

class Champ_Fonc_base;
class Domaine_Cl_VDF;
class Domaine_VDF;

class Paroi_hyd_base_VDF : public Turbulence_paroi_base
{
  Declare_base(Paroi_hyd_base_VDF);
public:
  void associer(const Domaine_dis_base& ,const Domaine_Cl_dis_base& ) override;
  void init_lois_paroi_();

  /**
   * OC 09/2004 :
   * Finalement, certaines classes du code, en VDF, demande le calcul de tab_u_star a la loi de paroi ;
   *  Or seul qques classes  de loi de paroi calcule reellement le tab_u_star d'ou des pbs de compatibilites
   * entre certaines fonctionnalites du code. Donc soit on met tab_u_star() en methode virtuelle pure
   * (mais ca fait doublon avec cisaillement_paroi(), soit on recalcule tab_u_star a partir de cisaillement_paroi()
   * dans les classes qui en ont besoin : c est el choix fait actuellement (cf.Paroi_ODVM_scal_VDF).
   */
  //virtual const DoubleVect& tab_u_star() const =0 ;

  /**
   * O.C. 09/2004:
   * Calcule le tableau des vitesses de frottements a partir du tableau de cisaillement.
   * Cette methode peut etre appelee si besoin par d'autres classes  afin de calculer u_star
   * ,une fois le cisaillement rempli toutefois!
   */
  /* Cette methode n'est pas correcte quand le calcul est QC. Voir Paroi_hyd_base_VDF.cpp
     void calculer_u_star_avec_cisaillement(DoubleVect& tab_u_star) const ;
  */
protected:
};

#endif /* Paroi_hyd_base_VDF_included */
