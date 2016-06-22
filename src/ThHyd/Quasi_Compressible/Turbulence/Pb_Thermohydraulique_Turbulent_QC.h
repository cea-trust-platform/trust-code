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
// File:        Pb_Thermohydraulique_Turbulent_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Pb_Thermohydraulique_Turbulent_QC_included
#define Pb_Thermohydraulique_Turbulent_QC_included

#include <Pb_QC_base.h>
#include <Navier_Stokes_Turbulent_QC.h>
#include <Convection_Diffusion_Chaleur_Turbulent_QC.h>
class Champ_Fonc;



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Pb_Thermohydraulique_Turbulent
//    Cette classe represente un probleme de thermohydraulique
//    avec modelisation de la turbulence:
//     - Equations de Navier_Stokes en regime turbulent
//       pour un fluide incompressible
//     - Equation d'energie en regime turbulent, sous forme generique
//       (equation de la chaleur)
//    Le fluide est quasi compressible
// .SECTION voir aussi
//    Probleme_base Pb_Thermohydraulique_QC Fluide_Quasi_Compressible
//////////////////////////////////////////////////////////////////////////////
class Pb_Thermohydraulique_Turbulent_QC : public Pb_QC_base
{

  Declare_instanciable(Pb_Thermohydraulique_Turbulent_QC);

public:

  int nombre_d_equations() const;
  const Equation_base& equation(int) const ;
  Equation_base& equation(int);
  inline const Champ_Fonc& viscosite_turbulente() const;
  int verifier();
  int postraiter(int force=1);
  virtual int expression_predefini(const Motcle& motlu, Nom& expression);

protected:

  Navier_Stokes_Turbulent_QC eq_hydraulique;
  Convection_Diffusion_Chaleur_Turbulent_QC eq_thermique;

};


// Description:
//    Renvoie le champ representant la viscosite turbulente.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: le champ representant la viscosite turbulente
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Fonc& Pb_Thermohydraulique_Turbulent_QC::viscosite_turbulente() const
{
  return eq_hydraulique.viscosite_turbulente();
}

#endif
