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

#include <Terme_Source_Qdm_Face_PolyMAC_P0P1NC.h>
#include <Op_Grad_PolyMAC_P0P1NC_Face.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <Discretisation_base.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Domaine_Cl_dis.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Milieu_base.h>

Implemente_instanciable(Terme_Source_Qdm_Face_PolyMAC_P0P1NC, "Source_Qdm_face_PolyMAC_P0P1NC|Source_Qdm_face_PolyMAC_P0", Terme_Source_Qdm_Face_PolyMAC);

Sortie& Terme_Source_Qdm_Face_PolyMAC_P0P1NC::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Qdm_Face_PolyMAC_P0P1NC::readOn(Entree& s) { return Terme_Source_Qdm_Face_PolyMAC::readOn(s); }

void Terme_Source_Qdm_Face_PolyMAC_P0P1NC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = ref_cast(Domaine_Poly_base, equation().domaine_dis().valeur());
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue().valeur());
  const DoubleTab& vals = la_source->valeurs(), &vfd = domaine.volumes_entrelaces_dir(), &rho = equation().milieu().masse_volumique()->passe(), &nf = domaine.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue()->passe() : nullptr;
  const DoubleVect& pe = equation().milieu().porosite_elem(), &ve = domaine.volumes(), &pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces(),
                    &fs = domaine.face_surfaces();
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  int e, f, i, cS = (vals.dimension_tot(0) == 1), cR = (rho.dimension_tot(0) == 1), nf_tot = domaine.nb_faces_tot(), n, N = equation().inconnue()->valeurs().line_size(), d,
               D = dimension, calc_cl = !sub_type(Domaine_PolyMAC_P0, domaine); //en PolyMAC_P0P1NC V1, on calcule aux CL

  /* contributions aux faces (par chaque voisin), aux elems */
  DoubleTrav a_f(N), rho_m(2);
  for (a_f = 1, f = 0; f < domaine.nb_faces(); f++)
    if (!fcl(f, 0)) //face interne
      {
        if (1)
          {
            for (i = 0; i < 2; i++)
              for (e = f_e(f, i), n = 0; n < N; n++)
                for (d = 0; d < D; d++)
                  secmem(f, n) += vfd(f, i) * pf(f) * (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * nf(f, d) / fs(f) * vals(!cS * e, N * d + n);
          }

        if (0)
          {
            if (alp)
              for (a_f = 0, i = 0; i < 2; i++)
                for (e = f_e(f, i), n = 0; n < N; n++)
                  a_f(n) += vfd(f, i) / vf(f) * (*alp)(e, n);
            for (rho_m = 0, i = 0; i < 2; i++)
              for (e = f_e(f, i), n = 0; n < N; n++)
                rho_m(i) += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n);
            for (i = 0; i < 2; i++)
              for (e = f_e(f, i), n = 0; n < N; n++)
                {
                  double vnf = 0;
                  for (d = 0; d < D; d++)
                    vnf += nf(f, d) / fs(f) * vals(!cS * e, N * d + n);
                  int strat = (i ? 1 : -1) * (rho_m(i) - rho(!cR * e, n)) * vnf > 0;
                  double R = alp && strat ? ((*alp)(e, n) < 1e-4 ? 1 : 0) /* min(max(1 - (*alp)(e, n) / 1e-4, 0.), 1.) */: 0;
                  secmem(f, n) += vfd(f, i) * pf(f) * a_f(n) * (R * rho_m(i) + (1 - R) * rho(!cR * e, n)) * vnf;
                }
          }
      }
    else if (calc_cl || fcl(f, 0) < 2)
      for (e = f_e(f, 0), n = 0; n < N; n++)
        for (d = 0; d < D; d++) //face de bord non imposee -> avec le (alpha rho) de la maille
          secmem(f, n) += pf(f) * vf(f) * (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * nf(f, d) / fs(f) * vals(!cS * e, N * d + n);

  /* en PolyMAC V2 : partie aux elements */
  if (sub_type(Domaine_PolyMAC_P0, domaine))
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          secmem(nf_tot + D * e + d, n) += pe(e) * ve(e) * (alp ? rho(!cR * e, n) * (*alp)(e, n) : 1) * vals(!cS * e, N * d + n);
}

int Terme_Source_Qdm_Face_PolyMAC_P0P1NC::initialiser(double temps)
{
  equation().discretisation().nommer_completer_champ_physique(equation().domaine_dis(), "source_qdm", "", la_source.valeur(), equation().probleme());
  la_source->initialiser(temps);
  return Source_base::initialiser(temps);
}
