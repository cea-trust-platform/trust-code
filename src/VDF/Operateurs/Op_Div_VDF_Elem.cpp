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
// File:        Op_Div_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Div_VDF_Elem.h>

Implemente_instanciable_sans_constructeur(Op_Div_VDF_Elem,"Op_Div_VDF_Face",Op_Div_VDF_base);


implemente_It_VDF_Elem(Eval_Div_VDF_Elem)
//// printOn
//

Sortie& Op_Div_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Div_VDF_Elem::readOn(Entree& s )
{
  return s ;
}


void Op_Div_VDF_Elem::associer(const Zone_dis& zone_dis,
                               const Zone_Cl_dis& zone_Cl_dis,
                               const Champ_Inc& )
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());


  // On n'associe pas div a la vitesse car non P0
  // On n'associe pas div a la pression car div(P) n'a pas de sens.

  iter.associer(zvdf, zclvdf, *this);
  Eval_Div_VDF_Elem& eval_div = (Eval_Div_VDF_Elem&) iter.evaluateur();
  eval_div.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones

  la_zone_vdf = zvdf;
  la_zcl_vdf = zclvdf;
}

void Op_Div_VDF_Elem::volumique(DoubleTab& div) const
{
  // PQ : 04/03
  const Zone_VDF& zone_VDF = la_zone_vdf.valeur();
  const DoubleVect& vol = zone_VDF.volumes();
  int nb_elem=zone_VDF.zone().nb_elem_tot();
  int num_elem;

  for(num_elem=0; num_elem<nb_elem; num_elem++)
    div(num_elem)/=vol(num_elem);
}

//
// Fonctions inline de la classe Op_Div_VDF_Elem
//
// Description:
// constructeur
Op_Div_VDF_Elem::Op_Div_VDF_Elem() :
  Op_Div_VDF_base(It_VDF_Elem(Eval_Div_VDF_Elem)())
{
}
