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

#include <Op_Grad_VDF_Face_base.h>
#include <Zone_Cl_VDF.h>

Implemente_base(Op_Grad_VDF_Face_base,"Op_Grad_VDF_Face_base",Operateur_Grad_base);

Sortie& Op_Grad_VDF_Face_base::printOn(Sortie& s ) const { return s << que_suis_je(); }
Entree& Op_Grad_VDF_Face_base::readOn(Entree& s ) { return s; }

void Op_Grad_VDF_Face_base::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const Champ_Inc& )
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF, zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
  la_zone_vdf = zvdf;
  la_zcl_vdf = zclvdf;
  porosite_surf.ref(zvdf.porosite_face());
  volume_entrelaces.ref(zvdf.volumes_entrelaces());
  face_voisins.ref(zvdf.face_voisins());
  orientation.ref(zvdf.orientation());
  xp.ref(zvdf.xp());
}

DoubleTab& Op_Grad_VDF_Face_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inco,resu);
}

int Op_Grad_VDF_Face_base::impr(Sortie& os) const
{
  Cerr << "Op_Grad_VDF_Face_base::impr is not coded ! Check its derived classes !" << finl;
  throw;
}
