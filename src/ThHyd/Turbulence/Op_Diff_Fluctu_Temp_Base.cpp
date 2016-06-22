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
// File:        Op_Diff_Fluctu_Temp_Base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_Fluctu_Temp_Base.h>
#include <Discretisation_base.h>

Implemente_base(Op_Diff_Fluctu_Temp_Base,"Op_Diff_Fluctu_Temp_Base",Operateur_base);
Implemente_instanciable(Op_Diff_Fluctu_Temp_negligeable,"Op_Diff_Fluctu_Temp_negligeable",Op_Diff_Fluctu_Temp_Base);
Implemente_deriv(Op_Diff_Fluctu_Temp_Base);
Implemente_instanciable(Op_Diff_Fluctu_Temp,"Op_Diff_Fluctu_Temp",DERIV(Op_Diff_Fluctu_Temp_Base));


////  printOn
//

Sortie& Op_Diff_Fluctu_Temp_Base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Sortie& Op_Diff_Fluctu_Temp_negligeable::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Sortie& Op_Diff_Fluctu_Temp::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_Fluctu_Temp_Base::readOn(Entree& s )
{
  return s ;
}

Entree& Op_Diff_Fluctu_Temp_negligeable::readOn(Entree& s )
{
  return s ;
}

Entree& Op_Diff_Fluctu_Temp::readOn(Entree& s )
{
  Operateur::lire(s);
  return s;
}

void Op_Diff_Fluctu_Temp::typer()
{
  if (typ=="negligeable")
    {
      DERIV(Op_Diff_Fluctu_Temp_Base)::typer("Op_Diff_Fluctu_Temp_negligeable");
      valeur().associer_diffusivite_turbulente();
    }
  else
    {
      //        Cerr << "Eq de Fluctu = " << equation().que_suis_je() << finl;
      Nom nom_type="Op_Diff_Fluctu_Temp_";
      nom_type +=equation().discretisation().que_suis_je();
      nom_type += "_";
      Nom type_inco=equation().inconnue()->que_suis_je();
      nom_type+=(type_inco.suffix("Champ_"));
      if (axi)
        nom_type += "_Axi";
      DERIV(Op_Diff_Fluctu_Temp_Base)::typer(nom_type);
      valeur().associer_eqn(equation());
      valeur().associer_diffusivite_turbulente();
      Cerr << valeur().que_suis_je() << finl;
    }
}




