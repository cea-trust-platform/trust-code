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
// File:        Schema_RK_Classique.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_RK_Classique.h>

// XD runge_kutta_ordre_2_classique schema_temps_base runge_kutta_ordre_2_classique -1 This is a classical Runge-Kutta scheme of second order that uses 2 integration points.
Implemente_instanciable(RK2_Classique,"Runge_Kutta_ordre_2_classique",TRUSTSchema_RK<Ordre_RK::DEUX_CLASSIQUE>);
Sortie& RK2_Classique::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::DEUX_CLASSIQUE>::printOn(s); }
Entree& RK2_Classique::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::DEUX_CLASSIQUE>::readOn(s) ; }

// XD runge_kutta_ordre_3_classique schema_temps_base runge_kutta_ordre_3_classique -1 This is a classical Runge-Kutta scheme of third order that uses 3 integration points.
Implemente_instanciable(RK3_Classique,"Runge_Kutta_ordre_3_classique",TRUSTSchema_RK<Ordre_RK::TROIS_CLASSIQUE>);
Sortie& RK3_Classique::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::TROIS_CLASSIQUE>::printOn(s); }
Entree& RK3_Classique::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::TROIS_CLASSIQUE>::readOn(s) ; }

// XD runge_kutta_ordre_4_classique schema_temps_base runge_kutta_ordre_4_classique -1 This is a classical Runge-Kutta scheme of fourth order that uses 4 integration points.
Implemente_instanciable(RK4_Classique, "Runge_Kutta_ordre_4_classique", TRUSTSchema_RK<Ordre_RK::QUATRE_CLASSIQUE>);
Sortie& RK4_Classique::printOn(Sortie& s) const { return TRUSTSchema_RK<Ordre_RK::QUATRE_CLASSIQUE>::printOn(s); }
Entree& RK4_Classique::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::QUATRE_CLASSIQUE>::readOn(s); }

// XD runge_kutta_ordre_4_classique_3_8 schema_temps_base runge_kutta_ordre_4_classique_3_8 -1 This is a classical Runge-Kutta scheme of fourth order that uses 4 integration points and the 3/8 rule.
Implemente_instanciable(RK4_Classique_3_8, "Runge_Kutta_ordre_4_classique_3_8", TRUSTSchema_RK<Ordre_RK::QUATRE_CLASSIQUE_3_8>);
Sortie& RK4_Classique_3_8::printOn(Sortie& s) const { return TRUSTSchema_RK<Ordre_RK::QUATRE_CLASSIQUE_3_8>::printOn(s); }
Entree& RK4_Classique_3_8::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::QUATRE_CLASSIQUE_3_8>::readOn(s); }
