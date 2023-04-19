/****************************************************************************
* Copyright (c) 2023, CEA
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
// File:        Op_Conv_Amont_PolyMAC_old_Elem.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Operateurs/Conv_iterateur
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Amont_PolyMAC_old_Elem.h>
#include <Champ_P0_PolyMAC_old.h>
#include <Champ_Face_PolyMAC_old.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Amont_PolyMAC_old_Elem,"Op_Conv_Amont_PolyMAC_old_Elem_PolyMAC_old",Op_Conv_PolyMAC_old_iterateur_base);

implemente_It_PolyMAC_old_Elem(Eval_Amont_PolyMAC_old_Elem)

//// printOn
//

Sortie& Op_Conv_Amont_PolyMAC_old_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_Amont_PolyMAC_old_Elem::readOn(Entree& s )
{
  return s ;
}


// Description:
// complete l'iterateur et l'evaluateur
void Op_Conv_Amont_PolyMAC_old_Elem::associer(const Domaine_dis& domaine_dis,
                                              const Domaine_Cl_dis& domaine_cl_dis,
                                              const Champ_Inc& ch_transporte)
{
  const Domaine_PolyMAC_old& zvdf = ref_cast(Domaine_PolyMAC_old,domaine_dis.valeur());
  const Domaine_Cl_PolyMAC_old& zclvdf = ref_cast(Domaine_Cl_PolyMAC_old,domaine_cl_dis.valeur());
  const Champ_P0_PolyMAC_old& inco = ref_cast(Champ_P0_PolyMAC_old,ch_transporte.valeur());

  iter->associer(zvdf, zclvdf, *this);

  Eval_Amont_PolyMAC_old_Elem& eval_conv = (Eval_Amont_PolyMAC_old_Elem&) iter.evaluateur();
  eval_conv.associer_domaines(zvdf, zclvdf );          // Evaluateur_PolyMAC_old::associer
  eval_conv.associer_inconnue(inco );        // Eval_PolyMAC_old_Elem::associer_inconnue
}

// Description:
// associe le champ de vitesse a l'evaluateur
void Op_Conv_Amont_PolyMAC_old_Elem::associer_vitesse(const Champ_base& ch_vit)
{
  const Champ_Face_PolyMAC_old& vit = ref_cast(Champ_Face_PolyMAC_old, ch_vit);

  Eval_Amont_PolyMAC_old_Elem& eval_conv = (Eval_Amont_PolyMAC_old_Elem&) iter.evaluateur();
  eval_conv.associer(vit);                // Eval_Conv_PolyMAC_old::associer
}

const Champ_base& Op_Conv_Amont_PolyMAC_old_Elem::vitesse() const
{
  Eval_Amont_PolyMAC_old_Elem& eval_conv = (Eval_Amont_PolyMAC_old_Elem&) iter.evaluateur();
  return eval_conv.vitesse();
}

Champ_base& Op_Conv_Amont_PolyMAC_old_Elem::vitesse()
{
  Eval_Amont_PolyMAC_old_Elem& eval_conv = (Eval_Amont_PolyMAC_old_Elem&) iter.evaluateur();
  return eval_conv.vitesse();
}

//
// Fonctions inline de la classe Op_Conv_Amont_PolyMAC_old_Elem
//
// Description:
// constructeur
Op_Conv_Amont_PolyMAC_old_Elem::Op_Conv_Amont_PolyMAC_old_Elem() :
  Op_Conv_PolyMAC_old_iterateur_base(It_PolyMAC_old_Elem(Eval_Amont_PolyMAC_old_Elem)())
{
}
