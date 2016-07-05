/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Dirichlet_entree_fluide.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Dirichlet_entree_fluide_included
#define Dirichlet_entree_fluide_included

#include <Dirichlet.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Dirichlet_entree_fluide
//    Cette classe represente une condition aux limite imposant une grandeur
//    sur l'entree du fluide. Des classes derivees specialiseront la grandeur
//    imposee: vitesse, temperature, concentration, fraction_massique ...
// .SECTION voir aussi
//    Dirichlet Entree_fluide_vitesse_imposee
//    Entree_fluide_temperature_imposee Entree_fluide_concentration_imposee
//    Entree_fluide_K_Eps_impose
//    Entree_fluide_fraction_massique_imposee
//////////////////////////////////////////////////////////////////////////////
class Dirichlet_entree_fluide  : public Dirichlet
{

  Declare_base(Dirichlet_entree_fluide);

};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Entree_fluide_vitesse_imposee
//    Cas particulier de la classe Dirichlet_entree_fluide
//    pour la vitesse imposee: impose la vitesse d'entree du fluide dans
//    une equation de type Navier_Stokes
// .SECTION voir aussi
//    Dirichlet_entree_fluide Navier_Stokes_std
//////////////////////////////////////////////////////////////////////////////
class Entree_fluide_vitesse_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_vitesse_imposee);

public :
  int compatible_avec_eqn(const Equation_base&) const;

};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Entree_fluide_temperature_imposee
//    Cas particulier de la classe Dirichlet_entree_fluide
//    pour la temperature imposee: impose la temperature d'entree du fluide
//    dans une equation de type Convection_Diffusion_Temperature
// .SECTION voir aussi
//    Dirichlet_entree_fluide Convection_Diffusion_Temperature
//////////////////////////////////////////////////////////////////////////////
class Entree_fluide_temperature_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_temperature_imposee);

public :
  int compatible_avec_eqn(const Equation_base&) const;

};



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Entree_fluide_temperature_imposee
//    Cas particulier de la classe Dirichlet_entree_fluide
//    pour la temperature imposee: impose la temperature d'entree du fluide
//    dans une equation de type Convection_Diffusion_Temperature
// .SECTION voir aussi
//    Dirichlet_entree_fluide Convection_Diffusion_Temperature
//////////////////////////////////////////////////////////////////////////////
class Entree_fluide_T_h_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_T_h_imposee);

public :
  double val_imp(int i) const;
  double val_imp(int i,int j) const;
  int compatible_avec_eqn(const Equation_base&) const;
  inline void bascule_cond_lim_en_enthalpie();
  inline void bascule_cond_lim_en_temperature();

protected:

  Champ_front le_champ_Text;
  Champ_front le_champ_hext;
  int type_cond_lim;

};



inline void Entree_fluide_T_h_imposee::bascule_cond_lim_en_enthalpie()
{
  type_cond_lim = 1;
}

inline void Entree_fluide_T_h_imposee::bascule_cond_lim_en_temperature()
{
  type_cond_lim = 0;
}





//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Entree_fluide_concentration_imposee
//    Cas particulier de la classe Dirichlet_entree_fluide
//    pour la concentration imposee: impose la concentration d'entree du
//    fluide dans une equation de type Convection_Diffusion_Concentration
// .SECTION voir aussi
//    Dirichlet_entree_fluide Convection_Diffusion_Concentration
//////////////////////////////////////////////////////////////////////////////
// CLASS: Entree_fluide_Fluctu_temperature_imposee :
//
// Cas particulier de la classe Dirichlet_entree_fluide
// pour le taux de disspation et la variance de temperature imposee
//
//////////////////////////////////////////////////////////////////////////////

class Entree_fluide_Fluctu_Temperature_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_Fluctu_Temperature_imposee);

public :
  int compatible_avec_eqn(const Equation_base&) const;

};

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Entree_fluide_Flux_Chaleur_Turbulente_imposee :
//
// Cas particulier de la classe Dirichlet_entree_fluide
// pour le flux de chaleur turbulente imposee
//
//////////////////////////////////////////////////////////////////////////////

class Entree_fluide_Flux_Chaleur_Turbulente_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_Flux_Chaleur_Turbulente_imposee);

public :
  int compatible_avec_eqn(const Equation_base&) const;

};

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Entree_fluide_concentration_imposee :
//
// Cas particulier de la classe Dirichlet_entree_fluide
// pour la concentration imposee
//
//////////////////////////////////////////////////////////////////////////////

class Entree_fluide_concentration_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_concentration_imposee);
public :
  virtual int compatible_avec_eqn(const Equation_base&) const;

};


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Entree_fluide_fraction_massique_imposee :
//
// Cas particulier de la classe Dirichlet_entree_fluide
// pour la fraction massique imposee
//
//////////////////////////////////////////////////////////////////////////////

class Entree_fluide_fraction_massique_imposee  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_fraction_massique_imposee);
public :
  virtual int compatible_avec_eqn(const Equation_base&) const;

};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Entree_fluide_K_Eps_impose
//    Cas particulier de la classe Dirichlet_entree_fluide
//    pour une concentration TURBULENTE imposee.
//    C'est le meme type de classe que Entree_fluide_concentration_imposee
//    en imposant des grandeurs turbulentes.
//    Impose les valeurs de K et epsilon d'entree du fluide dans une
//    equation de type Transport_K_eps
// .SECTION voir aussi
//    Dirichlet_entree_fluide Entree_fluide_concentration_imposee
//    Transport_K_eps
//////////////////////////////////////////////////////////////////////////////
class Entree_fluide_K_Eps_impose  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_K_Eps_impose);
public :
  int compatible_avec_eqn(const Equation_base&) const;

};




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Entree_fluide_V2_impose
//    Cas particulier de la classe Dirichlet_entree_fluide
//    pour les fluctuation de vitesse du modele K_Eps_V2 imposee.
//    C'est le meme type de classe que Entree_fluide_concentration_imposee
//    en imposant des grandeurs turbulentes.
//    Impose les valeurs de V2  d'entree du fluide dans une
//    equation de type Transport_V2
// .SECTION voir aussi
//    Dirichlet_entree_fluide Entree_fluide_concentration_imposee
//    Transport_V2
//////////////////////////////////////////////////////////////////////////////
class Entree_fluide_V2_impose  : public Dirichlet_entree_fluide
{

  Declare_instanciable(Entree_fluide_V2_impose);
public :
  int compatible_avec_eqn(const Equation_base&) const;

};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Frontiere_ouverte_vitesse_imposee_sortie
//    Cas particulier de la classe Entree_fluide_vitesse_imposee
//    pour la vitesse imposee: impose la vitesse de sortie du fluide dans
//    une equation de type Navier_Stokes
// .SECTION voir aussi
//    Dirichlet_entree_fluide Navier_Stokes_std
//////////////////////////////////////////////////////////////////////////////
class Frontiere_ouverte_vitesse_imposee_sortie  : public Entree_fluide_vitesse_imposee
{

  Declare_instanciable(Frontiere_ouverte_vitesse_imposee_sortie);

};

#endif

