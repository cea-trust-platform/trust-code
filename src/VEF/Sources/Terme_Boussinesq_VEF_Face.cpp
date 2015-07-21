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
// File:        Terme_Boussinesq_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Boussinesq_VEF_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Debog.h>
#include <Navier_Stokes_std.h>
#include <Synonyme_info.h>

Implemente_instanciable(Terme_Boussinesq_VEF_Face,"Boussinesq_VEF_P1NC",Terme_Boussinesq_base);
Add_synonym(Terme_Boussinesq_VEF_Face,"Boussinesq_temperature_VEF_Face");
Add_synonym(Terme_Boussinesq_VEF_Face,"Boussinesq_concentration_VEF_Face");

//// printOn
Sortie& Terme_Boussinesq_VEF_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_VEF_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

void Terme_Boussinesq_VEF_Face::associer_zones(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF, zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
}

DoubleTab& Terme_Boussinesq_VEF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& xp = zone_VEF.xp();
  const DoubleTab& xv = zone_VEF.xv();
  const DoubleVect& porosite_surf = zone_VEF.porosite_face();
  const DoubleTab& param = equation_scalaire().inconnue().valeurs();
  const DoubleTab& beta_valeurs = beta().valeur().valeurs();
  const DoubleVect& g = gravite().valeurs();

  int nb_dim = param.nb_dim();
  int nbcomp = resu.dimension(1);
  // Verifie la validite de T0:
  check();

  // Boucle sur toutes les faces
  int nb_faces = zone_VEF.nb_faces();
  for (int face=0; face<nb_faces; face++)
    {
      int elem1 = face_voisins(face,0);
      int elem2 = face_voisins(face,1);
      double delta_param = 0;
      for (int dim=0; dim<nb_dim; dim++)
        delta_param += valeur(beta_valeurs,elem1,elem2,dim)*(Scalaire0(dim)-(nb_dim==1 ? param(face) : param(face,dim)));

      for (int comp=0; comp<dimension; comp++)
        {
          double delta_coord;
          if (elem2==-1) // Face de bord
            delta_coord = xv(face,comp) - xp(elem1,comp);
          else
            delta_coord = xp(elem2,comp) - xp(elem1,comp);
          for (int compo=0; compo<nbcomp; compo++)
            resu(face,compo) += delta_param*delta_coord*face_normales(face,compo)*g(comp)*porosite_surf(face);
        }
    }
  return resu;
}

