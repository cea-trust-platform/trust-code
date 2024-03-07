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

#ifndef Mod_turb_hyd_RANS_0_eq_included
#define Mod_turb_hyd_RANS_0_eq_included

#include <Modele_turbulence_hyd_base.h>
class Equation_base;

/*! @brief Classe Mod_turb_hyd_RANS_0_eq Classe de base des modeles de type RANS a 0 equation
 *
 * @sa Modele_turbulence_hyd_base
 */
class Mod_turb_hyd_RANS_0_eq: public Modele_turbulence_hyd_base
{

  Declare_base_sans_constructeur(Mod_turb_hyd_RANS_0_eq);

public:

  Mod_turb_hyd_RANS_0_eq();
  void set_param(Param& param) override;
  int reprendre(Entree& is) override;
  int comprend_mot(const Motcle& mot) const;
  int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base) &ch_ref) const;
  int comprend_champ(const Motcle& mot) const;

  /////////////////////////////////////////////////////

  void completer() override;
  void mettre_a_jour(double) override;
  void discretiser() override;
  virtual Champ_Fonc& calculer_viscosite_turbulente()=0;
  virtual void calculer_energie_cinetique_turb()=0;
  void imprimer(Sortie&) const override;
  inline virtual Champ_Fonc& energie_cinetique_turbulente();
  inline virtual const Champ_Fonc& energie_cinetique_turbulente() const;

protected:

  Champ_Fonc energie_cinetique_turb_;
  mutable Champ_Inc K_eps_sortie_;
  Nom fichier_K_eps_sortie_;
};

inline Champ_Fonc& Mod_turb_hyd_RANS_0_eq::energie_cinetique_turbulente()
{
  return energie_cinetique_turb_;
}
inline const Champ_Fonc& Mod_turb_hyd_RANS_0_eq::energie_cinetique_turbulente() const
{
  return energie_cinetique_turb_;
}
#endif
