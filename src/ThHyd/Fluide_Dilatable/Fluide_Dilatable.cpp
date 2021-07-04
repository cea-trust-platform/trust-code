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
// File:        Fluide_Dilatable.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Dilatable.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_dis.h>

Implemente_instanciable_sans_constructeur(Fluide_Dilatable,"Fluide_Dilatable",Fluide_Incompressible);

Fluide_Dilatable::Fluide_Dilatable():traitement_PTh(0),Pth_(-1.),Pth_n(-1.),Pth1(-1.) {}

// Description:
//    Ecrit les proprietes du fluide sur un flot de sortie.
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
Sortie& Fluide_Dilatable::printOn(Sortie& os) const
{
  Fluide_Incompressible::ecrire(os);
  return os;
}


// Description:
//   Lit les caracteristiques du fluide a partir d'un flot
//   d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Fluide_Dilatable::readOn(Entree& is)
{
  Fluide_Incompressible::readOn(is);
  return is;
}

// Description:
//    Verifie que les champs lus l'ont ete correctement.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: l'une des proprietes (rho mu Cp ou lambda) du fluide n'a pas ete definie
// Effets de bord:
// Postcondition:
void Fluide_Dilatable::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (rho.non_nul())
    {
    }
  else
    {
      msg += "The density rho has not been specified. \n";
      err = 1;
    }
  if (mu.non_nul())
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          if (mu(0,0) <= 0)
            {
              msg += "The dynamical viscosity mu is not striclty positive. \n";
              err = 1;
            }
        }
    }
  else
    {
      msg += "The dynamical viscosity mu has not been specified. \n";
      err = 1;
    }
  if (lambda.non_nul())
    {
    }
  else
    {
      msg += "The conductivity lambda has not been specified. \n";
      err = 1;
    }
  if (Cp.non_nul())
    {
    }
  else
    {
      msg += "The heat capacity Cp has not been specified. \n";
      err = 1;
    }
  Milieu_base::verifier_coherence_champs(err,msg);
}

// Description:
//    Complete le fluide avec un Cp constant
// Precondition:
// Parametre: double Cp
//    Signification: le cp du fluide
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Dilatable::set_Cp(double Cp_)
{
  Cp.typer("Champ_Uniforme");
  Champ_Uniforme& ch_Cp = ref_cast(Champ_Uniforme,Cp.valeur());
  ch_Cp.dimensionner(1,1);
  DoubleTab& tab_Cp = Cp.valeurs();
  tab_Cp(0,0) = Cp_;
}

void Fluide_Dilatable::update_rho_cp(double temps)
{
  rho_cp_comme_T_.changer_temps(temps);
  rho_cp_comme_T_.valeur().changer_temps(temps);
  DoubleTab& rho_cp = rho_cp_comme_T_.valeurs();
  if (sub_type(Champ_Uniforme,rho))
    rho_cp = rho.valeurs()(0, 0);
  else
    {
      // AB: rho_cp = rho.valeurs() turns rho_cp into a 2 dimensional array with 1 compo. We want to stay mono-dim:
      rho_cp = 1.;
      tab_multiply_any_shape(rho_cp, rho.valeurs());
    }
  if (sub_type(Champ_Uniforme, Cp.valeur()))
    rho_cp *= Cp.valeurs()(0, 0);
  else
    tab_multiply_any_shape(rho_cp,Cp.valeurs());
}

void Fluide_Dilatable::checkTraitementPth(const Zone_Cl_dis& zone_cl)
{
  Cerr << "The method checkTraitementPth must be overloaded in the child classes of Fluide_Dilatable" << finl;
  Process::exit();
}
