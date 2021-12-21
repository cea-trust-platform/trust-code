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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Echange_interne_global_parfait.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_interne_global_parfait.h>
#include <Zone_Cl_dis_base.h>
#include <Discretisation_base.h>
#include <EChaine.h>

Implemente_instanciable(Echange_interne_global_parfait,"Paroi_echange_interne_global_parfait",Echange_interne_global_impose);
// XD Echange_interne_global_parfait condlim_base Paroi_echange_interne_global_parfait -1 Internal heat exchange boundary condition with perfect (infinite) exchange coefficient.

// Postcondition:
// Description:
//    Ecrit le type de l'objet sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
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
Sortie& Echange_interne_global_parfait::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}


Entree& Echange_interne_global_parfait::readOn(Entree& s )
{
  EChaine e("Champ_front_uniforme 1 1.0");
  e >> h_imp_;        // won't be used anyway
  init();
  return s;
}

//
// Compatible with all discretisations
int Echange_interne_global_parfait::compatible_avec_discr(const Discretisation_base& discr) const
{
  if (discr.que_suis_je() == "EF")
    return 1;
  else if (discr.que_suis_je() == "EF_axi")
    return 1;
  else
    err_pas_compatible(discr);
  return 0;
}
