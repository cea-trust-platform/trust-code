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
// File:        Cond_lim.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Cond_lim.h>

Implemente_deriv(Cond_lim_base);
Implemente_instanciable(Cond_lim,"Cond_lim",DERIV(Cond_lim_base));


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime seulement le nom de la condition aux limites.
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
Sortie& Cond_lim::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    Lit la condition aux limites sur un flot d'entree.
// Precondition:
// Parametre: Entree& s
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
Entree& Cond_lim::readOn(Entree& s )
{
  return DERIV(Cond_lim_base)::readOn(s) ;
}

// Description:
//    Adopte la Cond_lim passee en parametre.
//    Il n'y a pas de copie.
//    La Cond_lim_base eventuellement detenue avant l'appel est detruite.
//    ATTENTION cl est nul ensuite
// Precondition:
// Parametre: Cond_lim& cl
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Cond_lim&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Cond_lim::adopt(Cond_lim& cl)
{
  detach();
  set_O_U_Ptr(&(cl.valeur()));
  cl.set_O_U_Ptr(NULL);
}
