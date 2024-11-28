/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Op_Diff_Turbulent_PolyVEF_P0_Face_included
#define Op_Diff_Turbulent_PolyVEF_P0_Face_included

#include <Op_Diff_PolyVEF_P0_Face.h>
#include <Correlation.h>
#include <Champ_Fonc.h>
#include <vector>

/*! @brief : class Op_Diff_Turbulent_PolyVEF_P0_Face
 *
 *  Operateur de diffusion de vitesse prenant en compte l'effet de la turbulence par le biais d'une correlation de type Viscosite_turbulente_base.
 *
 *
 */

class Op_Diff_Turbulent_PolyVEF_P0_Face: public Op_Diff_PolyVEF_P0_Face
{
  Declare_instanciable( Op_Diff_Turbulent_PolyVEF_P0_Face );

public:
  void creer_champ(const Motcle& motlu) override;
  void mettre_a_jour(double temps) override;
  void completer() override;
  void modifier_mu(DoubleTab&) const override; //prend en compte la diffusivite turbulente
  inline const Correlation& correlation() const { return corr; }
  bool is_turb() const override { return true; }
  const Correlation* correlation_viscosite_turbulente() const override { return &corr; }

protected:
  Correlation corr; //correlation de viscosite turbulente
  std::vector<Champ_Fonc> nu_t_post_; //flux massiques (kg/m2/s)
  Motcles noms_nu_t_post_; //leurs noms
};

#endif /* Op_Diff_PolyVEF_P0_Face_included */
