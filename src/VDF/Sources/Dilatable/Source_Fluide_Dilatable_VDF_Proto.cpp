/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Source_Fluide_Dilatable_VDF_Proto.h>
#include <Dirichlet_homogene.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Domaine_Cl_VDF.h>
#include <Domaine_Cl_dis.h>
#include <Dirichlet.h>
#include <TRUSTTab.h>
#include <Domaine_VDF.h>

void Source_Fluide_Dilatable_VDF_Proto::associer_domaines_impl(const Domaine_dis& domaine,const Domaine_Cl_dis& domaine_cl)
{
  le_dom = ref_cast(Domaine_VDF,domaine.valeur());
  le_dom_Cl = ref_cast(Domaine_Cl_VDF,domaine_cl.valeur());
}

void Source_Fluide_Dilatable_VDF_Proto::associer_volume_porosite_impl(const Domaine_dis& domaine, DoubleVect& volumes, DoubleVect& porosites)
{
  volumes.ref(ref_cast(Domaine_VF,domaine.valeur()).volumes());
  porosites.ref(le_dom_Cl->equation().milieu().porosite_elem());
}

void Source_Fluide_Dilatable_VDF_Proto::ajouter_impl(const DoubleVect& g,const double rho_m,
                                                     const DoubleTab& tab_rho, DoubleTab& resu) const
{

  const int nb_faces = le_dom->nb_faces(), premiere_face_interne = le_dom->premiere_face_int();
  const IntVect& orientation = le_dom->orientation();
  const DoubleVect& volumes_entrelaces = le_dom->volumes_entrelaces(), porosite_surf=le_dom_Cl->equation().milieu().porosite_face();

  for (int num_cl=0 ; num_cl<le_dom->nb_front_Cl() ; num_cl++)
    {
      const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
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
