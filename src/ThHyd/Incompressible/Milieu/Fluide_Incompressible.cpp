/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Fluide_Incompressible.h>
#include <Motcle.h>

#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Champ_Fonc_MED.h>

Implemente_instanciable_sans_constructeur(Fluide_Incompressible,"Fluide_Incompressible",Fluide_base);
// XD fluide_incompressible fluide_base fluide_incompressible -1 Class for non-compressible fluids.
// XD attr beta_th field_base beta_th 1 Thermal expansion (K-1).
// XD attr mu field_base mu 1 Dynamic viscosity (kg.m-1.s-1).
// XD attr beta_co field_base beta_co 1 Volume expansion coefficient values in concentration.
// XD attr rho field_base rho 1 Density (kg.m-3).
// XD attr cp field_base cp 1 Specific heat (J.kg-1.K-1).
// XD attr lambda field_base lambda_u 1 Conductivity (W.m-1.K-1).


Fluide_Incompressible::Fluide_Incompressible() { }

Sortie& Fluide_Incompressible::printOn(Sortie& os) const
{
  return Fluide_base::printOn(os);
}

Entree& Fluide_Incompressible::readOn(Entree& is)
{
  Fluide_base::readOn(is);
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
// Exception: la masse volumique (rho) n'est pas de type Champ_Uniforme
// Exception: la capacite calorifique (Cp) n'est pas de type Champ_Uniforme
// Effets de bord:
// Postcondition:
void Fluide_Incompressible::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (rho.non_nul())
    {
      if (!sub_type(Champ_Uniforme,rho.valeur()))
        {
          msg += "The density rho is not of type Champ_Uniforme. \n";
          err = 1;
        }
    }
  if (Cp.non_nul())
    {
      if (!sub_type(Champ_Uniforme,Cp.valeur()))
        {
          msg += "Cp is not of type Champ_Uniforme. \n";
          err = 1;
        }
    }
  Fluide_base::verifier_coherence_champs(err, msg);
}
