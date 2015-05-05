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
// File:        Convection_Diffusion_Turbulent.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Convection_Diffusion_Turbulent_included
#define Convection_Diffusion_Turbulent_included

#include <Modele_turbulence_scal.h>
#include <Equation_base.h>
class Champ_Fonc;
class Operateur_Diff;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Turbulent
//     Cette classe represente la convection-diffusion d'une ou plusieurs
//     grandeurs scalaires (la temperature, une concentration ou plusieurs
//     concentrations) par un fluide verifiant la condition
//     d'incompressibilite div U = 0, avec modelisation de la turbulence.
//     Ce n'est pas (a elle seule) une classe de la hierarchie des equations
//     de TrioU, mais elle porte un modele de turbulence.
// .SECTION voir aussi
//     Convection_Diffusion_std  Mod_turb_scal
//     Convection_Diffusion_Temperature_Turbulent
//     Convection_Diffusion_Concentration_Turbulent
//     Cette est definie hors hierarchie Objet_U et Equation_base,
//     ceci permet la definition des equations de convection-diffusion
//     turbulentes par heritage multiple, voir par exemple
//     Convection_Diffusion_Temperature_Turbulent.
//////////////////////////////////////////////////////////////////////////////
class Convection_Diffusion_Turbulent
{

public :

  Entree& lire_modele(Entree&, const Equation_base& );

  //On passe desormais par equation().get_modele(TURBULENCE) pour recuperer
  //le modele de turbulence puis le tableau des valeurs de la diffusion turbulente

  void completer();
  virtual bool initTimeStep(double dt);
  int preparer_calcul();
  virtual int sauvegarder(Sortie&) const;
  virtual int reprendre(Entree&);
  virtual void mettre_a_jour(double);
  inline virtual ~Convection_Diffusion_Turbulent();
protected:
  Entree& lire_op_diff_turbulent(Entree&, const Equation_base&, Operateur_Diff&);
  Modele_turbulence_scal le_modele_turbulence;
};

Convection_Diffusion_Turbulent::~Convection_Diffusion_Turbulent()
{}


#endif
