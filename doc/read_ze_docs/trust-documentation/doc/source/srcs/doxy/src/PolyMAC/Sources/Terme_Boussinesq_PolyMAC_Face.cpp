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

#include <Convection_Diffusion_Temperature.h>
#include <Terme_Boussinesq_PolyMAC_Face.h>
#include <Fluide_Incompressible.h>
#include <Neumann_sortie_libre.h>
#include <Champ_Face_PolyMAC.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Synonyme_info.h>
#include <Dirichlet.h>

Implemente_instanciable(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_PolyMAC_Face|Boussinesq_PolyMAC_P0P1NC_Face", Terme_Boussinesq_base);
Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_PolyMAC_P0_Face");

Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_temperature_Face_PolyMAC_P0P1NC");
Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_temperature_Face_PolyMAC_P0");
Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_temperature_Face_PolyMAC");

Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_concentration_PolyMAC_P0P1NC_Face");
Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_concentration_PolyMAC_P0_Face");
Add_synonym(Terme_Boussinesq_PolyMAC_Face, "Boussinesq_concentration_PolyMAC_Face");

Sortie& Terme_Boussinesq_PolyMAC_Face::printOn(Sortie& s) const { return Terme_Boussinesq_base::printOn(s); }

Entree& Terme_Boussinesq_PolyMAC_Face::readOn(Entree& s) { return Terme_Boussinesq_base::readOn(s); }

void Terme_Boussinesq_PolyMAC_Face::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_PolyMAC = ref_cast(Domaine_PolyMAC, domaine_dis.valeur());
  le_dom_Cl_PolyMAC = ref_cast(Domaine_Cl_PolyMAC, domaine_Cl_dis.valeur());
}

void Terme_Boussinesq_PolyMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const DoubleTab& param = equation_scalaire().inconnue().valeurs();
  const DoubleTab& beta_valeurs = beta().valeur().valeurs();
  const IntTab& f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  const DoubleTab& rho = equation().milieu().masse_volumique().passe(), &vfd = domaine.volumes_entrelaces_dir(), &nf = domaine.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const DoubleVect& pf = equation().milieu().porosite_face(), &pe = equation().milieu().porosite_elem(), &ve = domaine.volumes(), &fs = domaine.face_surfaces(), &grav = gravite().valeurs();

  DoubleVect g(dimension);
  g = grav;

  // Verifie la validite de T0:
  check();
  int e, i, f, n, calc_cl = !sub_type(Domaine_PolyMAC_P0, domaine), nb_dim = param.line_size(), cR = (rho.dimension_tot(0) == 1), d, D = dimension, nf_tot = domaine.nb_faces_tot();
  for (f = 0; f < domaine.nb_faces(); f++)
    for (i = 0; (calc_cl || fcl(f, 0) < 2) && i < 2 && (e = f_e(f, i)) >= 0; i++) //contributions amont/aval
      {
        double coeff = 0;
        for (n = 0; n < nb_dim; n++)
          coeff += (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * valeur(beta_valeurs, e, e, n) * (Scalaire0(n) - valeur(param, e, n));

        secmem(f) += coeff * domaine.dot(&nf(f, 0), g.addr()) / fs(f) * vfd(f, i) * pf(f);
      }

  if (sub_type(Domaine_PolyMAC_P0, domaine))
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      {
        double coeff = 0;
        for (n = 0; n < nb_dim; n++)
          coeff += (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * valeur(beta_valeurs, e, e, n) * (Scalaire0(n) - valeur(param, e, n));
        for (d = 0; d < dimension; d++)
          secmem(nf_tot + D * e + d) += coeff * g(d) * pe(e) * ve(e);
      }
}
