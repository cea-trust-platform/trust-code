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

#include <Source_Generique_Face_PolyMAC.h>
#include <Zone_PolyMAC_P0.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Champ_Face_PolyMAC.h>
#include <Equation_base.h>

Implemente_instanciable(Source_Generique_Face_PolyMAC, "Source_Generique_Face_PolyMAC|Source_Generique_Face_PolyMAC_P0", Source_Generique_base);


Sortie& Source_Generique_Face_PolyMAC::printOn(Sortie& os) const
{
  return os << que_suis_je() ;
}

Entree& Source_Generique_Face_PolyMAC::readOn(Entree& is)
{
  Source_Generique_base::readOn(is);
  return is;
}

// Methode de calcul de la valeur sur une face encadree par elem1 et elem2 d'un champ uniforme ou non a plusieurs composantes
inline double valeur(const DoubleTab& valeurs_champ, int elem1, int elem2, const int& compo)
{
  if (valeurs_champ.dimension(0)==1)
    return valeurs_champ(0,compo); // Champ uniforme
  else
    {
      if (elem2<0) elem2 = elem1; // face frontiere
      if (valeurs_champ.nb_dim()==1)
        return 0.5*(valeurs_champ(elem1)+valeurs_champ(elem2));
      else
        return 0.5*(valeurs_champ(elem1,compo)+valeurs_champ(elem2,compo));
    }
}

DoubleTab& Source_Generique_Face_PolyMAC::ajouter(DoubleTab& resu) const
{
  Champ espace_stockage;
  const Champ_base& la_source = ch_source_->get_champ(espace_stockage); // Aux faces
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const DoubleVect& pf = zone.porosite_face(), &vf = zone.volumes_entrelaces();
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  for (int f = 0, calc_cl = !sub_type(Zone_PolyMAC_P0, zone); f < zone.nb_faces(); f++)
    if (calc_cl || fcl(f, 0) < 2)
      resu(f) += pf(f) * vf(f) * la_source.valeurs()(f);
  return resu;
}

void Source_Generique_Face_PolyMAC::associer_zones(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zcl_dis)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC,zone_dis.valeur());
  la_zcl_PolyMAC = ref_cast(Zone_Cl_PolyMAC,zcl_dis.valeur());
}

Nom Source_Generique_Face_PolyMAC::localisation_source()
{
  return "faces";
}
