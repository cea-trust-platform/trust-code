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

#include <Evaluateur_Source_VEF_Face.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>

Evaluateur_Source_VEF_Face::Evaluateur_Source_VEF_Face(const Evaluateur_Source_VEF_Face& eval) : Evaluateur_Source_Face(eval)
{
  volumes_entrelaces.ref(eval.volumes_entrelaces);
  volumes_entrelaces_Cl.ref(eval.volumes_entrelaces_Cl);
  porosite_surf.ref(eval.porosite_surf);
  face_voisins.ref(eval.face_voisins);
}

void Evaluateur_Source_VEF_Face::changer_volumes_entrelaces_Cl(DoubleVect& vol)
{
  volumes_entrelaces_Cl.ref(vol);
}

void Evaluateur_Source_VEF_Face::completer()
{
  Cerr << "Evaluateur_Source_VEF_Face::completer()" << finl;
  volumes_entrelaces.ref(ref_cast(Domaine_VEF,le_dom.valeur()).volumes_entrelaces());
  volumes_entrelaces_Cl.ref(ref_cast(Domaine_Cl_VEF,la_zcl.valeur()).volumes_entrelaces_Cl());
  porosite_surf.ref(ref_cast(Domaine_Cl_VEF,la_zcl.valeur()).equation().milieu().porosite_face());
  face_voisins.ref(ref_cast(Domaine_VEF,le_dom.valeur()).face_voisins());
}
