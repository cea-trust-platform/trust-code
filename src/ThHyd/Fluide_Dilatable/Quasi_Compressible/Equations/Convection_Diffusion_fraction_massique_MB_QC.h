/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Convection_Diffusion_fraction_massique_MB_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Equations
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Convection_Diffusion_fraction_massique_MB_QC_included
#define Convection_Diffusion_fraction_massique_MB_QC_included

#include <Convection_Diffusion_std.h>
class Fluide_Dilatable_base;
#include <Fluide_Quasi_Compressible.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_fraction_massique_MB_QC
//     Cas particulier de Convection_Diffusion_std pour un fluide quasi conpressible
//     iso-therme et iso-bar
// .SECTION voir aussi
//     Convection_Diffusion_std
//////////////////////////////////////////////////////////////////////////////
class Convection_Diffusion_fraction_massique_MB_QC : public Convection_Diffusion_std
{
  Declare_instanciable_sans_constructeur(Convection_Diffusion_fraction_massique_MB_QC);

public :

  Convection_Diffusion_fraction_massique_MB_QC();
  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void associer_fluide(const Fluide_Dilatable_base& );
  inline const Champ_Inc& inconnue() const;
  inline Champ_Inc& inconnue();

  void discretiser();
  const Milieu_base& milieu() const;
  const Fluide_Dilatable_base& fluide() const;
  Fluide_Dilatable_base& fluide();
  Milieu_base& milieu();
  void associer_milieu_base(const Milieu_base& );
  virtual int impr(Sortie& os) const;
  const Champ_Don& diffusivite_pour_transport();
  const Champ_base& diffusivite_pour_pas_de_temps();
  const Champ_base& vitesse_pour_transport();
  virtual const Motcle& domaine_application() const;

  virtual DoubleTab& derivee_en_temps_inco(DoubleTab& );
  void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;

  int sauvegarder(Sortie&) const;
  int reprendre(Entree&);
  int preparer_calcul();
  virtual void completer();

  void calculer_div_rho_u(DoubleTab& res) const;
  DoubleTab& derivee_en_temps_inco_sans_solveur_masse(DoubleTab& );
protected :

  Champ_Inc l_inco_ch;
  REF(Fluide_Quasi_Compressible) le_fluide;
  Zone_Cl_dis zcl_modif_;
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
inline const Champ_Inc& Convection_Diffusion_fraction_massique_MB_QC::inconnue() const
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
inline Champ_Inc& Convection_Diffusion_fraction_massique_MB_QC::inconnue()
{
  return l_inco_ch;
}

#endif /* Convection_Diffusion_fraction_massique_MB_QC_included */
