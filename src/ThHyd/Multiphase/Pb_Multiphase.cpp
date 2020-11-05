/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Pb_Multiphase.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Pb_Multiphase,"Pb_Multiphase",Pb_qdm_fluide);

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
Sortie& Pb_Multiphase::printOn(Sortie& os) const
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
Entree& Pb_Multiphase::readOn(Entree& is)
{
  return Pb_qdm_fluide::readOn(is);
}

void Pb_Multiphase::discretiser(const Discretisation_base& disc)
{
  if (!noms_phases_.size())
    Cerr << "Pb_Multiphase : the phases must be set using Phase " << le_nom() << " { ... } before Discretiser" << finl, Process::exit();
  return Pb_qdm_fluide::discretiser(disc);
}

// Description:
//    Renvoie le nombre d'equation,
//    Renvoie 2 car il y a 2 equations a un probleme de
//    thermo-hydraulique standard:
//        l'equation de Navier Stokes
//        l' equation de la thermique de type Convection_Diffusion_Temperature
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'equation
//    Contraintes: toujours 2 car il y a 2 equations au probleme
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Pb_Multiphase::nombre_d_equations() const
{
  return 3;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0
//    Renvoie l'equation de la thermique de type
//    Convection_Diffusion_Temperature si i=1
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: 0 <= i <= 1
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation correspondante a l'index
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Equation_base& Pb_Multiphase::equation(int i) const
{
  if      (i == 0) return eq_qdm;
  else if (i == 1) return eq_masse;
  else if (i == 2) return eq_energie;
  else Cerr << "Pb_Multiphase::equation() : Wrong equation number" << i << "!" << finl, Process::exit();
  return eq_qdm; //pour renvoyer quelque chose
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0
//    Renvoie l'equation de la thermique de type
//    Convection_Diffusion_Temperature si i=1
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: 0 <= i <= 1
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation correspondante a l'index
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Equation_base& Pb_Multiphase::equation(int i)
{
  if      (i == 0) return eq_qdm;
  else if (i == 1) return eq_masse;
  else if (i == 2) return eq_energie;
  else Cerr << "Pb_Multiphase::equation() : Wrong equation number" << i << "!" << finl, Process::exit();
  return eq_qdm; //pour renvoyer quelque chose
}


// Description:
//    Associe le milieu au probleme
//    Le milieu doit etre de type fluide incompressible
// Parametre: Milieu_base& mil
//    Signification: le milieu physique a associer au probleme
//    Valeurs par defaut:
//    Contraintes: doit etre de type fluide incompressible
//                 reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: mauvais type de milieu physique
// Effets de bord:
// Postcondition:
void Pb_Multiphase::associer_milieu_base(const Milieu_base& mil)
{
  /* controler le type de milieu ici */
  eq_qdm.associer_milieu_base(mil);
  eq_energie.associer_milieu_base(mil);
  eq_masse.associer_milieu_base(mil);
}



// Description:
//    Teste la compatibilite des equations de la thermique
//    et de l'hydraulique. Le test se fait sur les conditions
//    aux limites discretisees de chaque equation.
//    Appel la fonction de librairie hors classe:
//      tester_compatibilite_hydr_thermique(const Zone_Cl_dis&,const Zone_Cl_dis&)
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
int Pb_Multiphase::verifier()
{
  const Zone_Cl_dis& zone_Cl_hydr = eq_qdm.zone_Cl_dis();
  const Zone_Cl_dis& zone_Cl_th = eq_energie.zone_Cl_dis();
  return tester_compatibilite_hydr_thermique(zone_Cl_hydr,zone_Cl_th);
}



