/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_Darcy_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Darcy_VDF_Face.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>

Implemente_instanciable_sans_constructeur(Source_Darcy_VDF_Face,"Darcy_VDF_Face",Terme_Source_VDF_base);

implemente_It_Sou_VDF_Face(Eval_Darcy_VDF_Face)

//// printOn
//

Sortie& Source_Darcy_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Source_Darcy_VDF_Face::readOn(Entree& is )
{
  Motcles les_mots(2);
  {
    les_mots[0] = "modele_K";
    les_mots[1] = "porosite";
  }

  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "On attendait une { a la lecture d'une " << que_suis_je() << finl;
      Cerr << "et non : " << motlu << finl;
      exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            Motcle mot;
            is >> mot;
            eval().modK.typer(mot);
            is >> eval().modK.valeur();
            break;
          }
        case 1:
          {
            is >> eval().getPorosite();
            break;
          }
        default :
          {
            Cerr << "Unknown keyword in Source_Darcy_VDF_Face" << finl;
            exit();
          }
        }
      is >> motlu;
    }
  set_fichier("Source_Darcy");
  set_description("Darcy term = Integral(-nu/K*vitesse*dv) [m/s2]");

  return is;
}

void Source_Darcy_VDF_Face::associer_zones(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter->associer_zones(zvdf, zclvdf);
  eval().associer_zones(zvdf, zclvdf);
}

void Source_Darcy_VDF_Face::associer_pb(const Probleme_base& pb)
{
  const Champ_Don& diffu = ref_cast(Fluide_base,pb.milieu()).viscosite_cinematique();
  const Champ_Inc& vit = pb.equation(0).inconnue();
  eval().associer(diffu);
  eval().associer(vit);
}
