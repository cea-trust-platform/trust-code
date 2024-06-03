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

#ifndef Masse_Multiphase_included
#define Masse_Multiphase_included

#include <Convection_Diffusion_std.h>
#include <Operateur_Evanescence.h>
#include <Operateur_Grad.h>
#include <TRUST_Ref.h>

class Fluide_base;

/*! @brief classe Masse_Multiphase Cas particulier de Convection_Diffusion_std pour un fluide quasi conpressible
 *
 *      quand le scalaire subissant le transport est la fraction massique
 *
 * @sa Conv_Diffusion_std
 */
class Masse_Multiphase : public Convection_Diffusion_std
{
  Declare_instanciable(Masse_Multiphase);
public :
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_fluide(const Fluide_base& );
  inline const Champ_Inc& inconnue() const override;
  inline Champ_Inc& inconnue() override;

  void discretiser() override;
  const Milieu_base& milieu() const override;
  const Fluide_base& fluide() const;
  Fluide_base& fluide();
  Milieu_base& milieu() override;
  void associer_milieu_base(const Milieu_base& ) override;
  void completer() override;

  int nombre_d_operateurs() const override //pas de diffusion
  {
    return 1;
  }
  const Operateur& operateur(int) const override;
  Operateur& operateur(int) override;

  void dimensionner_matrice_sans_mem(Matrice_Morse& matrice) override;

  /*
    interface {dimensionner,assembler}_blocs
    specificites : prend en compte l'evanescence (en dernier)
  */
  int has_interface_blocs() const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) override;

  /* champ convecte : alpha * rho */
  static void calculer_alpha_rho(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  std::pair<std::string, fonc_calc_t> get_fonc_champ_conserve() const override
  {
    return { "alpha_rho", calculer_alpha_rho };
  }

  /////////////////////////////////////////////////////
  const Motcle& domaine_application() const override;

  inline const Operateur_Grad& operateur_gradient_inconnue() const { return Op_Grad_;}

protected :

  Champ_Inc l_inco_ch;
  REF(Fluide_base) le_fluide;
  Operateur_Grad Op_Grad_; // Pour calculer le gradient en VDF

  Operateur_Evanescence evanescence;
};


/*! @brief Renvoie le champ inconnue representant l'inconnue (T ou H) (version const)
 *
 * @return (Champ_Inc&) le champ inconnue representant la temperature (GP) ou l'enthalpie (GR)
 */
inline const Champ_Inc& Masse_Multiphase::inconnue() const
{
  return l_inco_ch;
}


/*! @brief Renvoie le champ inconnue representant l'inconnue (T ou H)
 *
 * @return (Champ_Inc&) le champ inconnue representant la temperature (GP) ou l'enthalpie (GR)
 */
inline Champ_Inc& Masse_Multiphase::inconnue()
{
  return l_inco_ch;
}

#endif
