/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Terme_Boussinesq_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Sources
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Boussinesq_CoviMAC_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Convection_Diffusion_Concentration.h>
#include <Convection_Diffusion_Temperature.h>
#include <Navier_Stokes_std.h>
#include <Synonyme_info.h>

Implemente_instanciable(Terme_Boussinesq_CoviMAC_Face,"Boussinesq_CoviMAC_Face",Terme_Boussinesq_base);
Add_synonym(Terme_Boussinesq_CoviMAC_Face,"Boussinesq_temperature_Face_CoviMAC");
Add_synonym(Terme_Boussinesq_CoviMAC_Face,"Boussinesq_concentration_CoviMAC_Face");

//// printOn
Sortie& Terme_Boussinesq_CoviMAC_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_CoviMAC_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

void Terme_Boussinesq_CoviMAC_Face::associer_zones(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC, zone_dis.valeur());
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, zone_Cl_dis.valeur());
}

DoubleTab& Terme_Boussinesq_CoviMAC_Face::ajouter(DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const DoubleTab& param = equation_scalaire().inconnue().valeurs();
  const DoubleTab& beta_valeurs = beta().valeur().valeurs();
  const DoubleVect& grav = gravite().valeurs();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& xv = zone.xv(), &xp = zone.xp();
  const DoubleVect& pf = zone.porosite_face();
  const DoubleVect& fs = zone.face_surfaces();

  DoubleVect g(dimension);
  g = grav;

  int nb_dim = param.nb_dim();

  // Verifie la validite de T0:
  check();
  int e, i, f, n;
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; ch.fcl(f, 0) < 2 && i < 2 && (e = f_e(f, i)) >= 0; i++) //contributions amont/aval
      {
        double coeff = 0;
        for (n = 0; n < nb_dim; n++) coeff += valeur(beta_valeurs, e, e ,n) * (Scalaire0(n) - valeur(param, e, n));
        resu(f) += coeff * (i ? -1 : 1) * zone.dot(&xv(f, 0), g.addr(), &xp(e, 0)) * fs(f) * pf(f);
      }
  return resu;
}
