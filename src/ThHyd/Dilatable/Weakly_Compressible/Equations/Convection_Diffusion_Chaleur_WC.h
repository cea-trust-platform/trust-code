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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Convection_Diffusion_Chaleur_WC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Equations
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Convection_Diffusion_Chaleur_WC_included
#define Convection_Diffusion_Chaleur_WC_included

#include <Convection_Diffusion_Chaleur_Fluide_Dilatable_base.h>
#include <Operateur_Grad.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Chaleur_WC
//     Cas particulier de Convection_Diffusion_Chaleur_Fluide_Dilatable_base pour un fluide quasi conpressible
//     quand le scalaire subissant le transport est la temperature en gaz parfaits,
//     ou l'enthalpie en gaz reels.
//     (generalisation de Convection_Diffusion_Temperature pour les gaz reels)
// .SECTION voir aussi
//     Conv_Diffusion_std Convection_Diffusion_Temperature
//////////////////////////////////////////////////////////////////////////////

class Convection_Diffusion_Chaleur_WC : public Convection_Diffusion_Chaleur_Fluide_Dilatable_base
{
  Declare_instanciable(Convection_Diffusion_Chaleur_WC);

public :
  void set_param(Param& titi) override;
  void completer() override;
  const Champ_base& vitesse_pour_transport() override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int preparer_calcul() override;

  int sauvegarder(Sortie&) const override;
  int reprendre(Entree&) override;

  // l'equation Convection_Diffusion_Chaleur_WC a un terme source supplementaire
  //  d P_tot / d t = del P / del t + u.grad(P_tot)
  // Il faut donc un operateur grad
  inline const Operateur_Grad& operateur_gradient_WC() const { return Op_Grad_WC_;}
  inline bool is_generic() override { return true; }

protected:
  void calculer_div_u_ou_div_rhou(DoubleTab& res) const override;
  Operateur_Grad Op_Grad_WC_;
};

#endif /* Convection_Diffusion_Chaleur_WC_included */
