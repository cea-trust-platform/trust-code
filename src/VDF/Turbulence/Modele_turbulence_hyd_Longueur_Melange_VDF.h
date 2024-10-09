/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Modele_turbulence_hyd_Longueur_Melange_VDF_included
#define Modele_turbulence_hyd_Longueur_Melange_VDF_included

#include <Modele_turbulence_hyd_Longueur_Melange_base.h>

class Domaine_Cl_VDF;
class Domaine_VDF;

/*! @brief Classe Modele_turbulence_hyd_Longueur_Melange_VDF Cette classe represente le modele de turbulence de longueur de melange de Prandtl.
 *     ATTENTION : modele code que pour un canal 2D horizontal !!!!
 *
 * @sa Modele_turbulence_hyd_Longueur_Melange_base
 */
class Modele_turbulence_hyd_Longueur_Melange_VDF: public Modele_turbulence_hyd_Longueur_Melange_base
{
  Declare_instanciable(Modele_turbulence_hyd_Longueur_Melange_VDF);
public:
  void set_param(Param& param) override;
  Champ_Fonc_base& calculer_viscosite_turbulente() override;
  void calculer_Sij2();
  int preparer_calcul() override;

protected:
  int direction_ = 1;
  double alt_min_ = 0., alt_max_ = 2.;
};

#endif /* Modele_turbulence_hyd_Longueur_Melange_VDF_included */
