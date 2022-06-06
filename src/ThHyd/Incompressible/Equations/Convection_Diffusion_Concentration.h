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

#ifndef Convection_Diffusion_Concentration_included
#define Convection_Diffusion_Concentration_included

#include <Convection_Diffusion_std.h>
#include <Ref_Constituant.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Concentration
//     Cas particulier de Convection_Diffusion_std
//     pour un ou plusieurs constituants.
//     Dans le cas de plusieurs constituants les champs
//     concentration et diffusivite sont vectoriels.
// .SECTION voir aussi
//      Convection_Diffusion_std
//////////////////////////////////////////////////////////////////////////////
class Convection_Diffusion_Concentration : public Convection_Diffusion_std
{
  Declare_instanciable_sans_constructeur(Convection_Diffusion_Concentration);

public :

  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  Convection_Diffusion_Concentration();
  const Milieu_base& milieu() const override;
  Milieu_base& milieu() override;
  const Constituant& constituant() const;
  Constituant& constituant();
  void associer_milieu_base(const Milieu_base& ) override;
  inline void associer_constituant(const Constituant& );
  void discretiser() override;
  inline const Champ_Inc& inconnue() const override;
  inline Champ_Inc& inconnue() override;
  inline int nb_constituants() const;
  int impr(Sortie& os) const override;
  int preparer_calcul() override;
  void mettre_a_jour(double) override;
  const Champ_Don& diffusivite_pour_transport() const override;
  const Motcle& domaine_application() const override;
  const double& masse_molaire() const;

protected :

  int nb_constituants_;
  Champ_Inc la_concentration;
  REF(Constituant) le_constituant;
  double masse_molaire_;
};


// Description:
//    Associe un constituant a l'equation.
// Precondition:
// Parametre: Constituant& un_constituant
//    Signification: un constituant a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un constituant associe
inline void Convection_Diffusion_Concentration::associer_constituant(const Constituant& un_constituant)
{
  le_constituant = un_constituant;
}


// Description:
//    Renvoie le champ inconnue de l'equation: la concentration.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue de l'equation, la concentration.
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Inc& Convection_Diffusion_Concentration::inconnue() const
{
  return la_concentration;
}


// Description:
//    Renvoie le champ inconnue de l'equation: la concentration.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue de l'equation, la concentration.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& Convection_Diffusion_Concentration::inconnue()
{
  return la_concentration;
}


// Description:
//    Renvoie le nombre de constituants du "Constituant" de l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de constituants de l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Convection_Diffusion_Concentration::nb_constituants() const
{
  return nb_constituants_;
}


#endif
