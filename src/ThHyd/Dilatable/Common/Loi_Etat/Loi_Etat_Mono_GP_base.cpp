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
// File:        Loi_Etat_Mono_GP_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Loi_Etat
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Mono_GP_base.h>
#include <Param.h>


Implemente_base(Loi_Etat_Mono_GP_base,"Loi_Etat_Mono_Gaz_Parfait_base",Loi_Etat_GP_base);

// Description:
//    Imprime la loi sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Loi_Etat_Mono_GP_base::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture d'une loi sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition: l'objet est construit avec les parametres lus
Entree& Loi_Etat_Mono_GP_base::readOn(Entree& is)
{
  double Cv_ = -1, gamma_ = -1;
  Param param(que_suis_je());
  param.ajouter("Cp",&Cp_,Param::REQUIRED);
  param.ajouter("Prandtl",&Pr_,Param::REQUIRED);
  param.ajouter("Cv",&Cv_);
  param.ajouter("gamma",&gamma_);
  param.ajouter("rho_constant_pour_debug",&rho_constant_pour_debug_);
  param.lire_avec_accolades_depuis(is);

  debug = gamma_<0 ? 1 : 0;

  if (gamma_<0) gamma_ = -gamma_;

  if (Cv_!=-1) R_ = Cp_ - Cv_;
  else if (gamma_!=-1) R_ = Cp_ *(1.-1./gamma_);
  else
    {
      Cerr<<"ERREUR : on attendait la definition du Cv (constante en gaz parfaits)"<<finl;
      Cerr<<"ou de la constante gamma (constante en gaz parfaits)"<<finl;
      abort();
    }
  return is;
}
