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
// File:        Transport_Fluctuation_Temperature.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

#ifndef Transport_Fluctuation_Temperature_included
#define Transport_Fluctuation_Temperature_included

#include <Convection_Diffusion_std.h>
#include <Op_Diff_Fluctu_Temp_Base.h>
#include <Ref_Fluide_Incompressible.h>
#include <Ref_Modele_turbulence_scal_Fluctuation_Temperature.h>

class Motcle;

class Transport_Fluctuation_Temperature : public Convection_Diffusion_std
{

  Declare_instanciable_sans_constructeur(Transport_Fluctuation_Temperature);


public :

  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  Transport_Fluctuation_Temperature();
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

  Champ_Inc le_champ_Fluctu_Temperature;
  Op_Diff_Fluctu_Temp terme_diffusif;

  REF(Fluide_Incompressible)le_fluide;
  REF(Modele_turbulence_scal_Fluctuation_Temperature) mon_modele_Fluctu_Temp;


};


// Fonctions inline:



// Description:
// renvoie le champ inconnue.
inline Champ_Inc& Transport_Fluctuation_Temperature::inconnue()
{
  return le_champ_Fluctu_Temperature;
}

// Description:
// renvoie le champ inconnue.
inline const Champ_Inc& Transport_Fluctuation_Temperature::inconnue() const
{
  return le_champ_Fluctu_Temperature;
}

inline const Modele_turbulence_scal_Fluctuation_Temperature& Transport_Fluctuation_Temperature::modele_turbulence() const
{
  assert(mon_modele_Fluctu_Temp.non_nul());
  return mon_modele_Fluctu_Temp.valeur();
}

inline Modele_turbulence_scal_Fluctuation_Temperature& Transport_Fluctuation_Temperature::modele_turbulence()
{
  assert(mon_modele_Fluctu_Temp.non_nul());
  return mon_modele_Fluctu_Temp.valeur();
}

#endif


