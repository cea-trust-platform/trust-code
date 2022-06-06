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

#include <Source_Generique_VDF_Face.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Zone_Cl_dis.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>

Implemente_instanciable(Source_Generique_VDF_Face,"Source_Generique_VDF_Face",Source_Generique_base);


Sortie& Source_Generique_VDF_Face::printOn(Sortie& os) const
{
  return os << que_suis_je() ;
}

Entree& Source_Generique_VDF_Face::readOn(Entree& is)
{
  Source_Generique_base::readOn(is);
  return is;
}

void Source_Generique_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Champ espace_stockage;
  const Champ_base& champ_calc = ch_source_->get_champ(espace_stockage);
  const DoubleTab& valeurs_calc = champ_calc.valeurs();

  int nb_faces = la_zone_VDF->nb_faces();
  const DoubleVect& vol_entrelaces = la_zone_VDF->volumes_entrelaces();
  const DoubleVect& poro_face = la_zone_VDF->porosite_face();
  int nb_front_cl = la_zone_VDF->nb_front_Cl();
  int premiere_face_interne = la_zone_VDF->premiere_face_int();
  int num_face;

  for (int num_cl =0; num_cl<nb_front_cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl_VDF->les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if ((sub_type(Dirichlet,la_cl.valeur()))
          ||
          (sub_type(Dirichlet_homogene,la_cl.valeur()))
         )
        ;
      else
        for (num_face=ndeb; num_face<nfin; num_face++)
          {
            secmem(num_face) += valeurs_calc(num_face)*vol_entrelaces(num_face)*poro_face(num_face);
          }
    }

  for (num_face=premiere_face_interne; num_face<nb_faces; num_face++)
    secmem(num_face) += valeurs_calc(num_face)*vol_entrelaces(num_face)*poro_face(num_face);

  secmem.echange_espace_virtuel();
}

void Source_Generique_VDF_Face::associer_zones(const Zone_dis& zone_dis,
                                               const Zone_Cl_dis& zcl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF,zone_dis.valeur());
  la_zcl_VDF = ref_cast(Zone_Cl_VDF,zcl_dis.valeur());
}

Nom Source_Generique_VDF_Face::localisation_source()
{
  return "faces";
}
