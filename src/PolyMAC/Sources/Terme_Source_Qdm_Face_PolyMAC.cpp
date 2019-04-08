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
// File:        Terme_Source_Qdm_Face_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Qdm_Face_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Champ_Don_Fonc_txyz.h>
#include <Zone_Cl_dis.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>

#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Periodique.h>
#include <Neumann.h>
#include <Neumann_homogene.h>
#include <Neumann_sortie_libre.h>

Implemente_instanciable(Terme_Source_Qdm_Face_PolyMAC,"Source_Qdm_face_PolyMAC",Source_base);



//// printOn
//

Sortie& Terme_Source_Qdm_Face_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Qdm_Face_PolyMAC::readOn(Entree& s )
{
  s >> la_source;
  if (la_source->nb_comp() != dimension)
    {
      Cerr << "Erreur a la lecture du terme source de type " << que_suis_je() << finl;
      Cerr << "le champ source doit avoir " << dimension << " composantes" << finl;
      exit();
    }
  return s ;
}

void Terme_Source_Qdm_Face_PolyMAC::associer_pb(const Probleme_base& )
{
  ;
}

void Terme_Source_Qdm_Face_PolyMAC::associer_zones(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC, zone_dis.valeur());
  la_zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, zone_Cl_dis.valeur());
}


DoubleTab& Terme_Source_Qdm_Face_PolyMAC::ajouter(DoubleTab& resu) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const Zone_Cl_PolyMAC& zone_Cl_PolyMAC = la_zone_Cl_PolyMAC.valeur();

  /* 1. faces de bord -> on ne contribue qu'aux faces de Neumann */
  for (int n_bord=0; n_bord<zone_PolyMAC.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_PolyMAC.les_conditions_limites(n_bord);
      if (!sub_type(Neumann,la_cl.valeur()) && !sub_type(Neumann_homogene,la_cl.valeur())) continue;
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      for (int f = le_bord.num_premiere_face(); f < le_bord.num_premiere_face() + le_bord.nb_faces(); f++)
        {
          int e = zone_PolyMAC.face_voisins(f, 0);
          double fac = zone_PolyMAC.porosite_face(f) * zone_PolyMAC.face_surfaces(f);
          for (int r = 0; r < dimension; r++)
            resu(f) += fac * la_source->valeurs()(sub_type(Champ_Uniforme,la_source.valeur()) ? 0 : e, r)
                       * (zone_PolyMAC.xv(f, r) - zone_PolyMAC.xp(e, r));
        }
    }
  /* 2. faces internes -> contributions amont/aval */
  for (int f = zone_PolyMAC.premiere_face_int(); f < zone_PolyMAC.nb_faces(); f++)
    {
      double fac = zone_PolyMAC.porosite_face(f) * zone_PolyMAC.face_surfaces(f);
      for (int i = 0; i < 2; i++) for (int r = 0, e = zone_PolyMAC.face_voisins(f, i); r < dimension; r++)
          resu(f) += fac * la_source->valeurs()(sub_type(Champ_Uniforme,la_source.valeur()) ? 0 : e, r)
                     * (zone_PolyMAC.xv(f, r) - zone_PolyMAC.xp(e, r)) * (i ? -1 : 1);
    }
  return resu;
}

DoubleTab& Terme_Source_Qdm_Face_PolyMAC::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Terme_Source_Qdm_Face_PolyMAC::mettre_a_jour(double temps)
{
  la_source->mettre_a_jour(temps);
}
