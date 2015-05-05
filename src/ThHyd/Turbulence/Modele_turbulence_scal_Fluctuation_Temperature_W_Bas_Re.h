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
// File:        Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re_included
#define Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re_included

#include <Modele_turbulence_scal_Fluctuation_Temperature_W.h>
#include <Transport_Fluctuation_Temperature_W_Bas_Re.h>
#include <Modele_Fonc_Bas_Reynolds_Thermique.h>

class Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re :  public Modele_turbulence_scal_Fluctuation_Temperature_W
{
  Declare_instanciable(Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re);

public:
  virtual void completer();
  int preparer_calcul();
  virtual void mettre_a_jour(double );
  //void associer_eqn(const Equation_base&);

  inline Champ_Inc& Fluctu_Temperature();
  inline const Champ_Inc& Fluctu_Temperature() const;

  inline Transport_Fluctuation_Temperature_W& equation_Fluctu();
  inline const Transport_Fluctuation_Temperature_W& equation_Fluctu() const;

  inline Modele_Fonc_Bas_Reynolds_Thermique& associe_modele_fonction();
  inline const Modele_Fonc_Bas_Reynolds_Thermique& associe_modele_fonction() const;
  virtual Champ_Fonc& calculer_diffusivite_turbulente();
  void set_param(Param&);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
  //////////////////////////////////////////////////////
  //Methode creer_champ pas codee a surcharger si necessaire
  //virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

private :

//Entree& lire(const Motcle&, Entree&);
  REF(Transport_Fluctuation_Temperature_W_Bas_Re) eqn_transport_Fluctu_Temp;
  Modele_Fonc_Bas_Reynolds_Thermique mon_modele_fonc;


protected :
  // nous n'avons plus alpha_turb = visco_turb/Prdt_turb
};


//
//  Fonctions inline de la classe Modele_turbulence_hyd_K_Eps
//

inline Transport_Fluctuation_Temperature_W& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::equation_Fluctu()
{
  //        Cerr << "on est dans WBasRE1 " << finl;
  //        Cerr << " eqn= " << eqn_transport_Fluctu_Temp.valeur() << finl;
  return eqn_transport_Fluctu_Temp.valeur();
}

inline const Transport_Fluctuation_Temperature_W& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::equation_Fluctu() const
{
  //Cerr << "on est dans WBasRE11 " << finl;
  //Cerr << " eqn =  " << eqn_transport_Fluctu_Temp.valeur() << finl;
  return eqn_transport_Fluctu_Temp.valeur();
}


inline const Champ_Inc& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::Fluctu_Temperature() const
{
  //Cerr << "on est dans WBasRE1 " << finl;
  //Cerr << " inc =  " << eqn_transport_Fluctu_Temp->inconnue() << finl;
  return eqn_transport_Fluctu_Temp->inconnue();
}

inline Champ_Inc& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::Fluctu_Temperature()
{
  //Cerr << "on est dans WBasRE11 " << finl;
  //Cerr << " inc =  " << eqn_transport_Fluctu_Temp->inconnue() << finl;
  return eqn_transport_Fluctu_Temp->inconnue();
}

inline Modele_Fonc_Bas_Reynolds_Thermique& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::associe_modele_fonction()
{
  return mon_modele_fonc;
}

inline const Modele_Fonc_Bas_Reynolds_Thermique& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::associe_modele_fonction() const
{
  return mon_modele_fonc;
}

#endif
