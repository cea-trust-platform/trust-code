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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Terme_Boussinesq_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Boussinesq_PolyMAC_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Zone_PolyMAC_P0.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Convection_Diffusion_Temperature.h>
#include <Navier_Stokes_std.h>
#include <Pb_Multiphase.h>
#include <Synonyme_info.h>

Implemente_instanciable(Terme_Boussinesq_PolyMAC_Face,"Boussinesq_PolyMAC_Face|Boussinesq_PolyMAC_P0_Face",Terme_Boussinesq_base);
Add_synonym(Terme_Boussinesq_PolyMAC_Face,"Boussinesq_temperature_Face_PolyMAC");
Add_synonym(Terme_Boussinesq_PolyMAC_Face,"Boussinesq_temperature_Face_PolyMAC_P0");
Add_synonym(Terme_Boussinesq_PolyMAC_Face,"Boussinesq_concentration_PolyMAC_Face");
Add_synonym(Terme_Boussinesq_PolyMAC_Face,"Boussinesq_concentration_PolyMAC_P0_Face");

//// printOn
Sortie& Terme_Boussinesq_PolyMAC_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_PolyMAC_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

void Terme_Boussinesq_PolyMAC_Face::associer_zones(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC, zone_dis.valeur());
  la_zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, zone_Cl_dis.valeur());
}

void Terme_Boussinesq_PolyMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const DoubleTab& param = equation_scalaire().inconnue().valeurs();
  const DoubleTab& beta_valeurs = beta().valeur().valeurs();
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  const DoubleTab& rho = equation().milieu().masse_volumique().passe(), &vfd = zone.volumes_entrelaces_dir(), &nf = zone.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &ve = zone.volumes(), &fs = zone.face_surfaces(), &grav = gravite().valeurs();

  DoubleVect g(dimension);
  g = grav;

  // Verifie la validite de T0:
  check();
  int e, i, f, n, calc_cl = !sub_type(Zone_PolyMAC_P0, zone), nb_dim = param.line_size(), cR = (rho.dimension_tot(0) == 1), d, D = dimension, nf_tot = zone.nb_faces_tot();
  for (f = 0; f < zone.nb_faces(); f++)
    for (i = 0; (calc_cl || fcl(f, 0) < 2) && i < 2 && (e = f_e(f, i)) >= 0; i++) //contributions amont/aval
      {
        double coeff = 0;
        for (n = 0; n < nb_dim; n++) coeff += (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * valeur(beta_valeurs, e, e ,n) * (Scalaire0(n) - valeur(param, e, n));
        secmem(f) += coeff * zone.dot(&nf(f, 0), g.addr()) / fs(f) * vfd(f, i) * pf(f);
      }

  if (sub_type(Zone_PolyMAC_P0, zone))
    for (e = 0; e < zone.nb_elem_tot(); e++)
      {
        double coeff = 0;
        for (n = 0; n < nb_dim; n++) coeff += (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * valeur(beta_valeurs, e, e ,n) * (Scalaire0(n) - valeur(param, e, n));
        for (d = 0; d < dimension; d++) secmem(nf_tot + D * e + d) += coeff * g(d) * pe(e) * ve(e);
      }
}
