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

#ifndef Convection_Diffusion_std_included
#define Convection_Diffusion_std_included

#include <Schema_Temps_base.h>
#include <Operateur_Conv.h>
#include <Operateur_Diff.h>
#include <Equation_base.h>
#include <TRUST_Ref.h>


class Champ_Inc_base;

/*! @brief classe Convection_Diffusion_std Cette classe est la base des equations modelisant le transport
 *
 *      d'un scalaire.
 *      Cette classe porte les termes communs a l'equation
 *      de transport d'un scalaire en regime laminaire.
 *      On se place sous l'hypothese de fluide incompressible.
 *          DT/dt = div(terme visqueux) + termes sources/rho_0
 *      avec DT/dt : derivee particulaire du scalaire
 *      Rq: l'implementation de la classe permet bien sur de negliger
 *          certains termes de l'equation (le terme diffusif,
 *          le terme convectif,tel ou tel terme source).
 *
 * @sa Equation_base, Classe abstraite, Methodes abstraites:, Entree& lire(const Motcle&, Entree&), const Champ_Inc_base& inconnue() const, Champ_Inc_base& inconnue()
 */
class Convection_Diffusion_std : public Equation_base
{
  Declare_base(Convection_Diffusion_std);

public :

  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int nombre_d_operateurs() const override;
  const Operateur& operateur(int) const override;
  Operateur& operateur(int) override;
  inline void associer_vitesse(const Champ_base& );
  inline const Champ_Inc_base& vitesse_transportante() const;
  const Champ_Inc_base& inconnue() const override =0;
  Champ_Inc_base& inconnue() override =0;
  virtual const Champ_Don_base& diffusivite_pour_transport() const;
  virtual const Champ_base& diffusivite_pour_pas_de_temps() const;
  virtual const Champ_base& vitesse_pour_transport() const;
  // E. Saikali : Methodes utiles pour un heritage V
  int sauvegarder_base(Sortie&) const;
  int reprendre_base(Entree&);

protected :

  OBS_PTR(Champ_Inc_base) la_vitesse_transportante;
  Operateur_Conv terme_convectif;
  Operateur_Diff terme_diffusif;
};


/*! @brief Renvoie une reference sur le champ representant la vitesse transportante.
 *
 * @return (Champ_Inc_base&) le champ representant la vitesse transportante
 */
inline const Champ_Inc_base& Convection_Diffusion_std::vitesse_transportante() const
{
  return la_vitesse_transportante.valeur();
}


/*! @brief Associe la vitesse transportante a l'equation.
 *
 * @param (Champ_Inc_base& vit) le champ a affecter a la vitesse transportante
 */
inline void Convection_Diffusion_std::associer_vitesse(const Champ_base& vit)
{
  la_vitesse_transportante = ref_cast(Champ_Inc_base,vit);
}
#endif
