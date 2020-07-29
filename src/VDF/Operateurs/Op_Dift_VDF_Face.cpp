/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Op_Dift_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Face.h>
#include <Mod_turb_hyd_base.h>
#include <Debog.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur(Op_Dift_VDF_Face,"Op_Dift_VDF_Face",Op_Dift_VDF_Face_base);

implemente_It_VDF_Face(Eval_Dift_VDF_const_Face)

//// printOn
//

Sortie& Op_Dift_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_Dift_VDF_Face::readOn(Entree& s )
{
  return s ;
}

//////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Op_Dift_VDF_Face
//
//////////////////////////////////////////////////////////////////


void Op_Dift_VDF_Face::associer_diffusivite_turbulente(const Champ_Fonc& visc_turb)
{
  Op_Diff_Turbulent_base::associer_diffusivite_turbulente(visc_turb);
  Evaluateur_VDF& eval = iter.evaluateur();
  Eval_Dift_VDF_const_Face& eval_diff_turb = (Eval_Dift_VDF_const_Face&) eval;
  eval_diff_turb.associer_diff_turb(visc_turb);
}

void Op_Dift_VDF_Face::completer()
{
  // Cerr << "Op_Dift_VDF_Face::completer() " << finl;
  Op_Dift_VDF_base::completer();
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& visc_turb = mod_turb.viscosite_turbulente();
  associer_diffusivite_turbulente(visc_turb);
  Evaluateur_VDF& eval = iter.evaluateur();
  Eval_Dift_VDF_const_Face& eval_diff_turb = (Eval_Dift_VDF_const_Face&) eval;
  eval_diff_turb.associer_modele_turbulence(mod_turb);
}

//
// Fonctions inline de la classe Op_Dift_VDF_Face
//
//// Op_Dift_VDF_Face
//
Op_Dift_VDF_Face::Op_Dift_VDF_Face() :
  Op_Dift_VDF_Face_base(It_VDF_Face(Eval_Dift_VDF_const_Face)())
{
}
