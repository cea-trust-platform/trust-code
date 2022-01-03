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
// File:        Source_Permeabilite_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Permeabilite_VDF.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(ModPerm_Cte,"K_constant",Modele_Permeabilite_base);
Sortie& ModPerm_Cte::printOn(Sortie& s ) const { return s << que_suis_je() << "\n"; }
Entree& ModPerm_Cte::readOn(Entree& is )
{
  Param param(que_suis_je());
  param.ajouter("valeur",&cte);
  param.lire_avec_accolades_depuis(is);
  return is;
}

Implemente_instanciable_sans_constructeur(ModPerm_Carman_Kozeny,"Carman_Kozeny",Modele_Permeabilite_base);
Sortie& ModPerm_Carman_Kozeny::printOn(Sortie& s ) const { return s << que_suis_je() << "\n"; }
Entree& ModPerm_Carman_Kozeny::readOn(Entree& is )
{
  Param param(que_suis_je());
  param.ajouter("diametre",&diam);
  param.lire_avec_accolades_depuis(is);
  return is;
}

Implemente_instanciable_sans_constructeur(ModPerm_ErgunPourDarcy,"ErgunDarcy",ModPerm_Carman_Kozeny);
Sortie& ModPerm_ErgunPourDarcy::printOn(Sortie& s) const { return s << que_suis_je() << "\n"; }
Entree& ModPerm_ErgunPourDarcy::readOn(Entree& is) { return ModPerm_Carman_Kozeny::readOn(is); }

Implemente_instanciable_sans_constructeur(ModPerm_ErgunPourForch,"ErgunForchheimer",ModPerm_Carman_Kozeny);
Sortie& ModPerm_ErgunPourForch::printOn(Sortie& s ) const { return s << que_suis_je() << "\n"; }
Entree& ModPerm_ErgunPourForch::readOn(Entree& is) { return ModPerm_Carman_Kozeny::readOn(is); }
