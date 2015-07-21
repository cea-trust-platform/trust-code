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
// File:        Evaluateur_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Evaluateur_VDF.h>
#include <Zone_VDF.h>

Evaluateur_VDF::Evaluateur_VDF(const Evaluateur_VDF& eval)
  :
  la_zone(eval.la_zone),la_zcl(eval.la_zcl),dimension(eval.dimension),premiere_face_bord(eval.premiere_face_bord)
{
  surface.ref(eval.surface);
  orientation.ref(eval.orientation);
  elem_.ref(eval.elem_);
  porosite.ref(eval.porosite);
  volume_entrelaces.ref(eval.volume_entrelaces);
  xv.ref(eval.xv);

}

void Evaluateur_VDF::associer_zones(const Zone_VDF& zone_vdf,
                                    const Zone_Cl_VDF& zone_cl_vdf)
{
  la_zone = zone_vdf;
  la_zcl = zone_cl_vdf;
  dimension = Objet_U::dimension;
  premiere_face_bord = zone_vdf.premiere_face_bord();
  surface.ref(zone_vdf.face_surfaces());
  orientation.ref(zone_vdf.orientation());
  elem_.ref(zone_vdf.face_voisins());
  porosite.ref(zone_vdf.porosite_face());
  volume_entrelaces.ref(zone_vdf.volumes_entrelaces());
  xv.ref(zone_vdf.xv());
}

//// dist_norm_bord
//

double Evaluateur_VDF::dist_norm_bord(int face) const
{
  return la_zone->dist_norm_bord(face);
}
