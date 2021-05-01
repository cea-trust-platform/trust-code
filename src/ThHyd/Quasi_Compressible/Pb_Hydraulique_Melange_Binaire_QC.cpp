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
// File:        Pb_Hydraulique_Melange_Binaire_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_Hydraulique_Melange_Binaire_QC.h>
#include <Verif_Cl.h>

Implemente_instanciable(Pb_Hydraulique_Melange_Binaire_QC,"Pb_Hydraulique_Melange_Binaire_QC",Pb_QC_base);


// Description:
//    Simple appel a: Probleme_base::printOn(Sortie&)
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
Sortie& Pb_Hydraulique_Melange_Binaire_QC::printOn(Sortie& os) const
{
  return Probleme_base::printOn(os);
}

// Description:
//    Simple appel a: Probleme_base::readOn(Entree&)
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
Entree& Pb_Hydraulique_Melange_Binaire_QC::readOn(Entree& is)
{
  return Probleme_base::readOn(is);
}

// Description:
//    Renvoie le nombre d'equation,
//    Renvoie 2 car il y a 2 equations a un probleme de
//    Hydraulique Melange Binaire :
//        l'equation de Navier Stokes
//        l' equation de conv/diff fraction massique
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
int Pb_Hydraulique_Melange_Binaire_QC::nombre_d_equations() const
{
  return 2;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_QC si i=0
//    Renvoie l'equation de conv/diff fraction massique de type
//    Convection_Diffusion_fraction_massique_QC si i=1
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
const Equation_base& Pb_Hydraulique_Melange_Binaire_QC::equation(int i) const
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "\nError in Pb_Hydraulique_Melange_Binaire_QC::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_frac_mass;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_QC si i=0
//    Renvoie l'equation de conv/diff fraction massique de type
//    Convection_Diffusion_fraction_massique_QC si i=1
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
Equation_base& Pb_Hydraulique_Melange_Binaire_QC::equation(int i)
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "\nError in Pb_Hydraulique_Melange_Binaire_QC::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_frac_mass;
}

// Description:
//    Teste la compatibilite des equations de la thermique
//    et de l'hydraulique. Le test se fait sur les conditions
//    aux limites discretisees de chaque equation.
//    Appel la fonction de librairie hors classe:
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
int Pb_Hydraulique_Melange_Binaire_QC::verifier()
{
  // TODO : FIXME
//  const Zone_Cl_dis& zone_Cl_hydr = eq_hydraulique.zone_Cl_dis();
//  const Zone_Cl_dis& zone_Cl_conc = eq_frac_mass.zone_Cl_dis();
//
//  return tester_compatibilite_hydr_concentration(zone_Cl_hydr,zone_Cl_conc);
  return 1;
}




