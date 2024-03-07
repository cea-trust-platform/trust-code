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

#ifndef Modele_turbulence_Longueur_Melange_VDF_included
#define Modele_turbulence_Longueur_Melange_VDF_included

#include <Mod_turb_hyd_RANS_0_eq.h>
#include <TRUST_Ref.h>

class Domaine_Cl_dis;
class Domaine_Cl_VDF;
class Domaine_VDF;

/*! @brief Classe Modele_turbulence_Longueur_Melange_VDF Cette classe represente le modele de turbulence de longueur de melange de Prandtl.
 *
 *     ATTENTION : modele code que pour un canal 2D horizontal !!!!
 *
 * @sa Modele_turbulence_hyd_base
 */
class Modele_turbulence_Longueur_Melange_VDF: public Mod_turb_hyd_RANS_0_eq
{

  Declare_instanciable_sans_constructeur(Modele_turbulence_Longueur_Melange_VDF);

public:
  void set_param(Param& param) override;
  Modele_turbulence_Longueur_Melange_VDF();
  void associer(const Domaine_dis&, const Domaine_Cl_dis&) override;

protected:

  Champ_Fonc& calculer_viscosite_turbulente() override;
  void calculer_energie_cinetique_turb() override;
  void calculer_Sij2();
  int preparer_calcul() override;

  DoubleVect Sij2;

  REF(Domaine_VDF) le_dom_VDF;
  REF(Domaine_Cl_VDF) le_dom_Cl_VDF;

private:

  int direction;
  double alt_min, alt_max;

};

#endif
