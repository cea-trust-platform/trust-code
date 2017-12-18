/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Pb_Thermohydraulique_Concentration.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_Thermohydraulique_Concentration.h>
#include <Fluide_Incompressible.h>
#include <Constituant.h>
#include <Verif_Cl.h>

Implemente_instanciable(Pb_Thermohydraulique_Concentration,"Pb_Thermohydraulique_Concentration",Pb_qdm_fluide);


// Description:
//    Simple appel a: Pb_qdm_fluide::printOn(Sortie&)
//    Ecrit le probleme sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Pb_Thermohydraulique_Concentration::printOn(Sortie& os) const
{
  return Pb_qdm_fluide::printOn(os);
}


// Description:
//    Simple appel a: Pb_qdm_fluide::readOn(Entree&)
//    Lit le probleme a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Pb_Thermohydraulique_Concentration::readOn(Entree& is)
{
  return Pb_qdm_fluide::readOn(is);
}

// Description:
//    Renvoie le nombre d'equation,
//    Renvoie 3 car il y a 2 equations a un probleme de
//    thermo-hydraulique avec concentration:
//        - l'equation de Navier Stokes
//        - equation d'energie en regime laminaire
//        - une equation de convection-diffusion
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'equations
//    Contraintes: renvoie toujours 3, car il y 3 equations au probleme
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Pb_Thermohydraulique_Concentration::nombre_d_equations() const
{
  return 3;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0
//    Renvoie l'equation de la thermique de type
//    Convection_Diffusion_Temperature si i=1
//    Renvoie l'equation de la thermique de type
//    Convection_Diffusion_Concentration si i=2
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: 0 <= i <= 2
//    Acces:
// Retour:  Equation_base&
//    Signification: l'equation correspondante a l'index
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Equation_base& Pb_Thermohydraulique_Concentration::equation(int i) const
{
  if ( !( i==0 || i==1 || i==2 ) )
    {
      Cerr << "\nPb_Thermohydraulique_Concentration::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else if (i == 1)
    return eq_thermique;
  else
    return eq_concentration;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0
//    Renvoie l'equation de la thermique de type
//    Convection_Diffusion_Temperature si i=1
//    Renvoie l'equation de la thermique de type
//    Convection_Diffusion_Concentration si i=2
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: 0 <= i <= 2
//    Acces:
// Retour:  Equation_base&
//    Signification: l'equation correspondante a l'index
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Equation_base& Pb_Thermohydraulique_Concentration::equation(int i)
{
  if ( !( i==0 || i==1 || i==2 ) )
    {
      Cerr << "\nPb_Thermohydraulique_Concentration::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else if (i == 1)
    return eq_thermique;
  else
    return eq_concentration;
}



// Description:
//    Associe un milieu au probleme,
//    Si le milieu est de type
//      - Fluide_Incompressible, il sera associe a l'equation de l'hydraulique
//        et a l'equation d'energie.
//      - Constituant, il sera associe a l'equation de convection-diffusion
//    Un autre type de milieu provoque une erreur
// Precondition:
// Parametre: Milieu_base& mil
//    Signification: le milieu physique a associer au probleme
//    Valeurs par defaut:
//    Contraintes: doit etre de type fluide incompressible
//                 ou de type constituant
//                 reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: mauvais type de milieu physique
// Effets de bord:
// Postcondition:
void Pb_Thermohydraulique_Concentration::associer_milieu_base(const Milieu_base& mil)
{
  if ( sub_type(Fluide_Incompressible,mil) )
    {
      eq_hydraulique.associer_milieu_base(mil);
      eq_thermique.associer_milieu_base(mil);
    }
  else if ( sub_type(Constituant,mil) )
    eq_concentration.associer_milieu_base(mil);
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a " << finl;
      Cerr << "un probleme de type Pb_Thermohydraulique_Concentration " << finl;
      exit();
    }
}


// Description:
//    Teste la compatibilite des equations de convection-diffusion
//    et de l'hydraulique. Le test se fait sur les conditions
//    aux limites discretisees de chaque equation.
//    Appels aux fonctions de librairie hors classe:
//      tester_compatibilite_hydr_thermique(const Zone_Cl_dis&,const Zone_Cl_dis&)
//      tester_compatibilite_hydr_concentration(const Zone_Cl_dis&,const Zone_Cl_dis&)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Pb_Thermohydraulique_Concentration::verifier()
{
  const Zone_Cl_dis& zone_Cl_hydr = eq_hydraulique.zone_Cl_dis();
  const Zone_Cl_dis& zone_Cl_th = eq_thermique.zone_Cl_dis();
  const Zone_Cl_dis& zone_Cl_co = eq_concentration.zone_Cl_dis();
  tester_compatibilite_hydr_thermique(zone_Cl_hydr,zone_Cl_th);
  return tester_compatibilite_hydr_concentration(zone_Cl_hydr,zone_Cl_co);
}
