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

#include <Solide.h>
#include <Champ_Uniforme.h>
#include <Param.h>

Implemente_instanciable(Solide,"Solide",Milieu_base);
// XD Solide milieu_base Solide -1 Solid with cp and/or rho non-uniform.
// XD attr rho field_base rho 1 Density (kg.m-3).
// XD attr cp field_base cp 1 Specific heat (J.kg-1.K-1).
// XD attr lambda field_base lambda_u 1 Conductivity (W.m-1.K-1).
// XD ref gravite field_base

// Description:
//    Ecrit les caracteristiques du milieu su run flot de sortie.
//    Simple appel a: Milieu_base::printOn(Sortie&)
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
Sortie& Solide::printOn(Sortie& os) const
{
  return Milieu_base::printOn(os);
}

// Description:
//    Lit les caracteristiques du milieu a partir d'un
//    flot d'entree.
// cf Milieu_base::readOn
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Solide::readOn(Entree& is)
{
  Milieu_base::readOn(is);
  return is;
}

void Solide::set_param(Param& param)
{
  Milieu_base::set_param(param);
  param.ajouter_condition("is_read_rho","Density (rho) has not been read for a Solide type medium.");
  param.ajouter_condition("is_read_Cp","Heat capacity (Cp) has not been read for a Solide type medium.");
  param.ajouter_condition("is_read_lambda","Conductivity (lambda) has not been read for a Solide type medium.");
}

// Description:
//    Verifie que les champs caracterisant le milieu solide
//    qui on ete lu par readOn(Entree&) sont coherents.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: la conductivite (lambda) n'est pas strictement positive
// Exception: l'une des proprietes physique du solide: masse volumique (rho),
//            capacite calorifique (Cp) ou conductivite (lambda) n'a pas
//            ete definie.
// Effets de bord:
// Postcondition:
void Solide::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (lambda(0,0) <= 0)
        {
          msg += "The conductivity lambda is not striclty positive. \n";
          err = 1;
        }
    }

  Milieu_base::verifier_coherence_champs(err,msg);
}
