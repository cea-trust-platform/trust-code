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
// File:        Op_Conv_Quick_VDF_Face_Axi.cpp
// Directory:   $TRUST_ROOT/src/VDF/Axi/Operateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Quick_VDF_Face_Axi.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Quick_VDF_Face_Axi,"Op_Conv_Quick_VDF_Face_Axi",Op_Conv_VDF_base);

implemente_It_VDF_Face(Eval_Quick_VDF_Face_Axi)


// printOn et readOn

Sortie& Op_Conv_Quick_VDF_Face_Axi::printOn(Sortie& s ) const
{
  return s;
}

Entree& Op_Conv_Quick_VDF_Face_Axi::readOn(Entree& is )
{
  return is;
}


// Description:
// complete l'iterateur et l'evaluateur

void Op_Conv_Quick_VDF_Face_Axi::associer(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_cl_dis,
                                          const Champ_Inc& ch_vit)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  const Champ_Face& vit = ref_cast(Champ_Face,ch_vit.valeur());

  iter.associer(zvdf, zclvdf, *this);

  Eval_Quick_VDF_Face_Axi& eval_conv = (Eval_Quick_VDF_Face_Axi&) iter.evaluateur();
  eval_conv.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  eval_conv.associer_inconnue(vit);             // Eval_VDF_Face::associer_inconnue
}

const Champ_Inc_base& Op_Conv_Quick_VDF_Face_Axi::vitesse() const
{
  Eval_Quick_VDF_Face_Axi& eval_conv = (Eval_Quick_VDF_Face_Axi&) iter.evaluateur();
  return eval_conv.vitesse();
}

Champ_Inc_base& Op_Conv_Quick_VDF_Face_Axi::vitesse()
{
  Eval_Quick_VDF_Face_Axi& eval_conv = (Eval_Quick_VDF_Face_Axi&) iter.evaluateur();
  return eval_conv.vitesse();
}


// Description:
// constructeur

Op_Conv_Quick_VDF_Face_Axi::Op_Conv_Quick_VDF_Face_Axi() :Op_Conv_VDF_base(It_VDF_Face(Eval_Quick_VDF_Face_Axi)())
{
}

