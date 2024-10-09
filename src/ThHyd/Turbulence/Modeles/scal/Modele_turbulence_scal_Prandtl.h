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

#ifndef Modele_turbulence_scal_Prandtl_included
#define Modele_turbulence_scal_Prandtl_included

#include <Modele_turbulence_scal_diffturb_base.h>
#include <Parser_U.h>

/*! @brief Classe Modele_turbulence_scal_Prandtl Cette classe represente le modele de calcul suivant
 *
 *     pour la diffusivite turbulente:
 *                   alpha_turb = visco_turb / Prdt_turb;
 *
 * @sa Mod_Turb_scal_diffuturb_base
 */
class Modele_turbulence_scal_Prandtl: public Modele_turbulence_scal_diffturb_base
{
  Declare_instanciable(Modele_turbulence_scal_Prandtl);
public:
  void mettre_a_jour(double) override;
  void set_param(Param&) override;

protected:
  double LePrdt_ = 0.9;
  Nom LePrdt_fct_; // stockage de la chaine de caractere pour le prandtl du jdd
  Nom definition_fonction_; // stockage de la chaine du jdd
  Parser_U fonction_; // fonction de calcul de alpha_t
  Parser_U fonction1_; // fonction de calcul de Prandtl variant en espace
  virtual Champ_Fonc_base& calculer_diffusivite_turbulente();
};

#endif /* Modele_turbulence_scal_Prandtl_included */
