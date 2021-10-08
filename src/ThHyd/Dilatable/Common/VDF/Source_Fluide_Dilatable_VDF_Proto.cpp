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
// File:        Source_Fluide_Dilatable_VDF_Proto.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/VDF
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Fluide_Dilatable_VDF_Proto.h>
#include <Dirichlet_homogene.h>
#include <Zone_Cl_VDF.h>
#include <Zone_Cl_dis.h>
#include <Dirichlet.h>
#include <Zone_VDF.h>

void Source_Fluide_Dilatable_VDF_Proto::associer_zones_impl(const Zone_dis& zone,const Zone_Cl_dis& zone_cl)
{
  la_zone = ref_cast(Zone_VDF,zone.valeur());
  la_zone_Cl = ref_cast(Zone_Cl_VDF,zone_cl.valeur());
}

void Source_Fluide_Dilatable_VDF_Proto::associer_volume_porosite_impl(const Zone_dis& zone, DoubleVect& volumes, DoubleVect& porosites)
{
  volumes.ref(ref_cast(Zone_VF,zone.valeur()).volumes());
  porosites.ref(ref_cast(Zone_VF,zone.valeur()).porosite_elem());
}

void Source_Fluide_Dilatable_VDF_Proto::ajouter_impl(const DoubleVect& g,const double rho_m,
                                                     const DoubleTab& tab_rho, DoubleTab& resu) const
{

  const int nb_faces = la_zone->nb_faces(), premiere_face_interne = la_zone->premiere_face_int();
  const IntVect& orientation = la_zone->orientation();
  const DoubleVect& volumes_entrelaces = la_zone->volumes_entrelaces(), porosite_surf=la_zone->porosite_face();

  for (int num_cl=0 ; num_cl<la_zone->nb_front_Cl() ; num_cl++)
    {
      const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Dirichlet,la_cl.valeur()) || sub_type(Dirichlet_homogene,la_cl.valeur())) { /* Do nothing */ }
      else
        {
          for (int face=ndeb ; face<nfin ; face++)
            resu(face) += (tab_rho(face)-rho_m)*g(orientation(face)) * volumes_entrelaces(face)*porosite_surf(face);
        }
    }

  for (int face=premiere_face_interne ; face<nb_faces; face++)
    resu(face) += (tab_rho(face)-rho_m)*g(orientation(face)) * volumes_entrelaces(face)*porosite_surf(face);

}
