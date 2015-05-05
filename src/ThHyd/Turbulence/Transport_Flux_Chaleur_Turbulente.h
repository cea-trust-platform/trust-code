/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Transport_Flux_Chaleur_Turbulente.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Transport_Flux_Chaleur_Turbulente_included
#define Transport_Flux_Chaleur_Turbulente_included

#include <Convection_Diffusion_std.h>
#include <Op_Diff_Flux_Chaleur_Turb_Base.h>
#include <Ref_Fluide_Incompressible.h>
#include <Ref_Modele_turbulence_scal_Fluctuation_Temperature.h>
class Motcle;

class Transport_Flux_Chaleur_Turbulente : public Convection_Diffusion_std
{

  Declare_instanciable_sans_constructeur(Transport_Flux_Chaleur_Turbulente);

public :

  Transport_Flux_Chaleur_Turbulente();
  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  inline void associer_vitesse(const Champ_Inc& );
  void associer_milieu_base(const Milieu_base& );
  void associer_modele_turbulence(const Modele_turbulence_scal_Fluctuation_Temperature& );
  void discretiser();
  void completer();
  inline const Milieu_base& milieu() const;
  inline Milieu_base& milieu();
  inline const Modele_turbulence_scal_Fluctuation_Temperature& modele_turbulence() const;
  inline Modele_turbulence_scal_Fluctuation_Temperature& modele_turbulence();
  int nombre_d_operateurs() const;
  const Operateur& operateur(int) const;
  Operateur& operateur(int);
  inline const Champ_Inc& vitesse_transportante();
  virtual const Champ_Inc& inconnue() const;
  virtual Champ_Inc& inconnue();

  /////////////////////////////////////////////////////
  const Motcle& domaine_application() const;
  int controler_grandeur();

protected :

  Champ_Inc le_Flux_Chaleur_Turbulente;
  Op_Diff_Flux_Chaleur_Turb terme_diffusif;
  Operateur_Conv terme_convectif;

  REF(Fluide_Incompressible)le_fluide;
  REF(Champ_Inc) la_vitesse_transportante;
  REF(Modele_turbulence_scal_Fluctuation_Temperature) mon_modele_fluctu;


};

// Description:
// renvoie le champ inconnue.
inline Champ_Inc& Transport_Flux_Chaleur_Turbulente::inconnue()
{
  return le_Flux_Chaleur_Turbulente;
}

// Description:
// renvoie le champ inconnue.
inline const Champ_Inc& Transport_Flux_Chaleur_Turbulente::inconnue() const
{
  return le_Flux_Chaleur_Turbulente;
}

inline const Modele_turbulence_scal_Fluctuation_Temperature& Transport_Flux_Chaleur_Turbulente::modele_turbulence() const
{
  assert(mon_modele_fluctu.non_nul());
  return mon_modele_fluctu.valeur();
}

inline Modele_turbulence_scal_Fluctuation_Temperature& Transport_Flux_Chaleur_Turbulente::modele_turbulence()
{
  assert(mon_modele_fluctu.non_nul());
  return mon_modele_fluctu.valeur();
}

inline void Transport_Flux_Chaleur_Turbulente::associer_vitesse(const Champ_Inc& vit)
{
  la_vitesse_transportante = vit;
}

#endif


