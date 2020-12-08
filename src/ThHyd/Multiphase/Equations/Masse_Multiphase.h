/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Masse_Multiphase.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Masse_Multiphase_included
#define Masse_Multiphase_included

#include <Convection_Diffusion_std.h>
#include <Ref_Fluide_base.h>
#include <Operateur_Evanescence.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Masse_Multiphase
//     Cas particulier de Convection_Diffusion_std pour un fluide quasi conpressible
//     quand le scalaire subissant le transport est la fraction massique
// .SECTION voir aussi
//     Conv_Diffusion_std
//////////////////////////////////////////////////////////////////////////////
class Masse_Multiphase : public Convection_Diffusion_std
{
  Declare_instanciable_sans_constructeur(Masse_Multiphase);

public :

  Masse_Multiphase();
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void associer_fluide(const Fluide_base& );
  inline const Champ_Inc& inconnue() const;
  inline Champ_Inc& inconnue();

  void discretiser();
  const Milieu_base& milieu() const;
  const Fluide_base& fluide() const;
  Fluide_base& fluide();
  Milieu_base& milieu();
  void associer_milieu_base(const Milieu_base& );
  virtual void completer();

  int nombre_d_operateurs() const //pas de diffusion
  {
    return 1;
  }
  const Operateur& operateur(int) const;
  Operateur& operateur(int);

  /*
    interface {dimensionner,assembler}_blocs
    specificites : prend en compte l'evanescence (en dernier)
  */
  virtual void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const;
  virtual void assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const;

  /* champ convecte : alpha * rho */
  static void calculer_champ_conserve(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only);
  virtual std::pair<std::string, fonc_calc_t> get_fonc_champ_conserve() const
  {
    return { "alpha_rho", calculer_champ_conserve };
  }

  /* cette equation n'est jamais resolue directement, mais traitee lors de la resolution de QDM_Multiphase */
  int equation_non_resolue() const
  {
    return 1;
  }

  /////////////////////////////////////////////////////
  virtual const Motcle& domaine_application() const;

protected :

  Champ_Inc l_inco_ch;
  REF(Fluide_base) le_fluide;

  Operateur_Evanescence evanescence;
};


// Description:
//    Renvoie le champ inconnue representant l'inconnue (T ou H)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la temperature (GP)
//      ou l'enthalpie (GR)
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Inc& Masse_Multiphase::inconnue() const
{
  return l_inco_ch;
}


// Description:
//    Renvoie le champ inconnue representant l'inconnue (T ou H)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la temperature (GP)
//      ou l'enthalpie (GR)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& Masse_Multiphase::inconnue()
{
  return l_inco_ch;
}

#endif
