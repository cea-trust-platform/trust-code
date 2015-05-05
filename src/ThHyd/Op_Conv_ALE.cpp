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
// File:        Op_Conv_ALE.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_ALE.h>

Implemente_base(Op_Conv_ALE,"Operateur_Conv_ALE",Operateur_Conv_base);

Sortie& Op_Conv_ALE::printOn(Sortie& os) const
{
  return os;
}

Entree& Op_Conv_ALE::readOn(Entree& is)
{
  return is;
}

void Op_Conv_ALE::associer_vitesse(const Champ_base& vit)
{
  la_vitesse = ref_cast(Champ_Inc_base,vit);
}

DoubleTab& Op_Conv_ALE::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  op_conv.ajouter(inco, resu);
  Cerr << "equation : " << equation().le_nom() << finl;
  Motcle le_nom_eqn=equation().le_nom();
  if (le_nom_eqn!="pbNavier_Stokes_standard")
    {
      ajouterALE(inco,resu);
    }
  //si ajout ALE ici, pas besoin de redimensionner resu dans ajouterALE
  //ajouterALE(inco,resu);
  //ajouterALE(inco,resu);
  return resu;
}
DoubleTab& Op_Conv_ALE::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  //Cerr << "Op_Conv_ALE::calculer" << finl;
  op_conv.calculer(inco, resu);
  return resu;
}
void Op_Conv_ALE::associer(const Zone_dis& zdis,
                           const Zone_Cl_dis& zcl_dis,
                           const Champ_Inc& inco)
{
  //Cerr << "Op_Conv_ALE::associer" << finl;
  dom=inco->domaine();
  op_conv.l_op_base().associer(zdis, zcl_dis, inco);//rendu public en checkout
}

//DoubleTab& OpConvALE::ajouterALE(const DoubleTab& inco, DoubleTab& resu) const
//{
//  return resu;
//}
/*
  const Domaine_ALE& Op_Conv_ALE::domaine_ALE() const
  {
  return ref_cast(Domaine_ALE, dom.valeur());
  }

  Domaine_ALE& Op_Conv_ALE::domaine_ALE()
  {
  return ref_cast(Domaine_ALE, dom.valeur());
  }
*/
