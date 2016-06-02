/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Modele_Fonc_Bas_Reynolds.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_Fonc_Bas_Reynolds.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Discretisation_base.h>

Implemente_deriv(Modele_Fonc_Bas_Reynolds_Base);
Implemente_instanciable(Modele_Fonc_Bas_Reynolds,"Modele_Fonc_Bas_Reynolds",DERIV(Modele_Fonc_Bas_Reynolds_Base));

//
// printOn et readOn

Sortie& Modele_Fonc_Bas_Reynolds::printOn(Sortie& s ) const
{
  return  s << valeur().que_suis_je() << finl;
}

Entree& Modele_Fonc_Bas_Reynolds::readOn(Entree& is )
{
  Motcle typ;
  is >> typ;
  Motcle nom1("Modele_");

  nom1 += typ;
  //  if ( (typ == "Jones_Launder") || (typ == "Nagano") || (typ == "Lam_Bremhorst")  )
  //if ( (typ == "Jones_Launder") || (typ == "Nagano") || (typ == "launder_Sharma") )
  {
    nom1 += "_";
    Cerr << nom1 << finl;
    Nom discr = equation().discretisation().que_suis_je();
    if (discr=="VEFPreP1B") discr = "VEF";
    nom1 += discr;
  }
  DERIV(Modele_Fonc_Bas_Reynolds_Base)::typer(nom1);
  valeur().associer_eqn(equation());
  valeur().associer(equation().zone_dis(), equation().zone_Cl_dis());
  valeur().associer_pb(equation().probleme());
  is >> valeur();
  return is;
}
