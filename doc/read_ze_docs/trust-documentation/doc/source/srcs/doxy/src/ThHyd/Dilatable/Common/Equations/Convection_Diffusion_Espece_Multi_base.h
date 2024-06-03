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

#ifndef Convection_Diffusion_Espece_Multi_base_included
#define Convection_Diffusion_Espece_Multi_base_included

#include <Convection_Diffusion_Espece_Fluide_Dilatable_base.h>

/*! @brief classe Convection_Diffusion_Espece_Multi_base Cas particulier de Convection_Diffusion_Espece_Fluide_Dilatable_base
 *
 *      pour un fluide quasi conpressible quand le scalaire subissant le transport est
 *      la fraction massique
 *
 * @sa Convection_Diffusion_Espece_Fluide_Dilatable_base
 */

class Convection_Diffusion_Espece_Multi_base : public Convection_Diffusion_Espece_Fluide_Dilatable_base
{
  Declare_base(Convection_Diffusion_Espece_Multi_base);

public:
  const Champ_base& get_champ(const Motcle& nom) const override;
  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  // Methodes inlines
  inline const Motcle& get_alias() const { return alias_; }

protected:
  Motcle alias_;
};

#endif /* Convection_Diffusion_Espece_Multi_base_included */
