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
// File:        Modele_turbulence_scal_Fluctuation_Temperature.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_scal_Fluctuation_Temperature_included
#define Modele_turbulence_scal_Fluctuation_Temperature_included

#include <Modele_turbulence_scal_base.h>
#include <Transport_Fluctuation_Temperature.h>
#include <Transport_Flux_Chaleur_Turbulente.h>
#include <Ref_Champ_Fonc.h>

class Modele_turbulence_scal_Fluctuation_Temperature :  public Modele_turbulence_scal_base
{
  Declare_instanciable(Modele_turbulence_scal_Fluctuation_Temperature);

public:

  void completer();
  int preparer_calcul();
  virtual bool initTimeStep(double dt);
  void mettre_a_jour(double );
  void associer_eqn(const Equation_base&);
  void associer_viscosite_turbulente(const Champ_Fonc& );
  inline Champ_Inc& Fluctu_Temperature();
  inline const Champ_Inc& Fluctu_Temperature() const;
  inline Champ_Inc& Flux_Chaleur_Turb();
  inline const Champ_Inc& Flux_Chaleur_Turb() const;
  inline Transport_Fluctuation_Temperature& equation_Fluctu();
  inline const Transport_Fluctuation_Temperature& equation_Fluctu() const;
  inline Transport_Flux_Chaleur_Turbulente& equation_Chaleur();
  inline const Transport_Flux_Chaleur_Turbulente& equation_Chaleur() const;

  int sauvegarder(Sortie& os) const;
  int reprendre(Entree& is);
  void imprimer(Sortie&) const;
  void set_param(Param&);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);

  //////////////////////////////////////////////////////
  //Methode creer_champ pas codee a surcharger si necessaire
  //virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

private :

  Transport_Fluctuation_Temperature eqn_transport_Fluctu_Temp;
  Transport_Flux_Chaleur_Turbulente eqn_transport_Flux_Chaleur_Turb;
  //Entree& lire(const Motcle&, Entree&);


protected :
  REF(Champ_Fonc) la_viscosite_turbulente;
  Champ_Fonc& calculer_diffusivite_turbulente();
  // nous n'avons plus alpha_turb = visco_turb/Prdt_turb
};


//
//  Fonctions inline de la classe Modele_turbulence_hyd_K_Eps
//

inline Transport_Fluctuation_Temperature& Modele_turbulence_scal_Fluctuation_Temperature::equation_Fluctu()
{
  return eqn_transport_Fluctu_Temp;
}

inline const Transport_Fluctuation_Temperature& Modele_turbulence_scal_Fluctuation_Temperature::equation_Fluctu() const
{
  return eqn_transport_Fluctu_Temp;
}

inline Transport_Flux_Chaleur_Turbulente& Modele_turbulence_scal_Fluctuation_Temperature::equation_Chaleur()
{
  return eqn_transport_Flux_Chaleur_Turb;
}

inline const Transport_Flux_Chaleur_Turbulente& Modele_turbulence_scal_Fluctuation_Temperature::equation_Chaleur() const
{
  return eqn_transport_Flux_Chaleur_Turb;
}

inline const Champ_Inc& Modele_turbulence_scal_Fluctuation_Temperature::Fluctu_Temperature() const
{
  return eqn_transport_Fluctu_Temp.inconnue();
}

inline Champ_Inc& Modele_turbulence_scal_Fluctuation_Temperature::Fluctu_Temperature()
{
  return eqn_transport_Fluctu_Temp.inconnue();
}

inline const Champ_Inc& Modele_turbulence_scal_Fluctuation_Temperature::Flux_Chaleur_Turb() const
{
  return eqn_transport_Flux_Chaleur_Turb.inconnue();
}

inline Champ_Inc& Modele_turbulence_scal_Fluctuation_Temperature::Flux_Chaleur_Turb()
{
  return eqn_transport_Flux_Chaleur_Turb.inconnue();
}

#endif
