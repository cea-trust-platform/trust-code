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

#ifndef Modele_turbulence_hyd_LES_selectif_mod_VDF_included
#define Modele_turbulence_hyd_LES_selectif_mod_VDF_included

#include <Modele_turbulence_hyd_LES_VDF.h>

/*! @brief classe Modele_turbulence_hyd_LES_selectif_VDF Cette classe correspond a la mise en oeuvre du modele sous
 *
 *  maille fonction de structure selectif modifie en VDF
 *  La modification concerne l angle de coupure : il depend du pas du maillage
 *
 */
class Modele_turbulence_hyd_LES_selectif_mod_VDF: public Modele_turbulence_hyd_LES_VDF
{
  Declare_instanciable_sans_constructeur(Modele_turbulence_hyd_LES_selectif_mod_VDF);
public:
  Modele_turbulence_hyd_LES_selectif_mod_VDF()
  {
    Csm1_ = 0.086;  // nelle cste pour 64^3
    Csm2_ = 0.676;  // si CSM1 = 0.086 pour 64^3
  }

  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void calculer_fonction_structure() override;
  int a_pour_Champ_Fonc(const Motcle&, REF(Champ_base)&) const;
  void discretiser() override;

protected:
  int kc_ = -123, ki_ = -123, thi_ = 0, canal_ = 0, dir_par_ = -123;
  double demi_h_ = -123.;
  Champ_Fonc la_vorticite_;

  void cutoff();
};

#endif /* Modele_turbulence_hyd_LES_selectif_mod_VDF_included */
