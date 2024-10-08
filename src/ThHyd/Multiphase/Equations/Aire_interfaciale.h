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

#ifndef Aire_interfaciale_included
#define Aire_interfaciale_included

#include <Convection_Diffusion_std.h>
#include <Fluide_base.h>
#include <TRUST_Ref.h>

/*! @brief classe Aire_interfaciale Equation de transport de l'aire interfaciale
 *
 * @sa Conv_Diffusion_std Convection_Diffusion_Temperature
 */
class Aire_interfaciale : public Convection_Diffusion_std
{
  Declare_instanciable_sans_constructeur(Aire_interfaciale);

public :

  Aire_interfaciale();

  void associer_fluide(const Fluide_base& );
  inline const Champ_Inc_base& inconnue() const override { return l_inco_ch_; }
  inline Champ_Inc_base& inconnue() override { return l_inco_ch_; }
  void discretiser() override;
  const Milieu_base& milieu() const override;
  Milieu_base& milieu() override;
  void associer_milieu_base(const Milieu_base& ) override;
  int impr(Sortie& os) const override;
  void mettre_a_jour(double temps) override;

  int positive_unkown() override {return 1;};

  int nombre_d_operateurs() const override //pas de diffusion
  {
    return 1;
  }
  const Operateur& operateur(int) const override;
  Operateur& operateur(int) override;

  const Motcle& domaine_application() const override;

protected :
  OWN_PTR(Champ_Inc_base) l_inco_ch_;
  OWN_PTR(Champ_Fonc_base)  diametre_bulles_;
  OBS_PTR(Fluide_base) le_fluide_;

  int n_l_=-1 ; // Number of the liquid phase (the one where no IA is stored)
};

#endif /* Aire_interfaciale_included */
