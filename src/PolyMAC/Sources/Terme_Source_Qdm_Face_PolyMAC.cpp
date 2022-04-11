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
// File:        Terme_Source_Qdm_Face_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Qdm_Face_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_dis.h>
#include <Zone_PolyMAC.h>
#include <Zone_PolyMAC_V2.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Op_Grad_PolyMAC_Face.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Terme_Source_Qdm_Face_PolyMAC,"Source_Qdm_face_PolyMAC|Source_Qdm_face_PolyMAC_V2",Source_base);

Sortie& Terme_Source_Qdm_Face_PolyMAC::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Terme_Source_Qdm_Face_PolyMAC::readOn(Entree& s )
{
  s >> la_source;
  if (la_source->nb_comp() != equation().inconnue().valeur().nb_comp())
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

void Terme_Source_Qdm_Face_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_Poly_base& zone = ref_cast(Zone_Poly_base, equation().zone_dis().valeur());
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const DoubleTab& vals = la_source->valeurs(), &vfd = zone.volumes_entrelaces_dir(),
                   &rho = equation().milieu().masse_volumique().passe(), &nf = zone.face_normales(),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const DoubleVect& pe = zone.porosite_elem(), &ve = zone.volumes(), &pf = zone.porosite_face(), &vf = zone.volumes_entrelaces(), &fs = zone.face_surfaces();
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl();
  int e, f, i, cS = (vals.dimension_tot(0) == 1), cR = (rho.dimension_tot(0) == 1), nf_tot = zone.nb_faces_tot(),
               n, N = equation().inconnue().valeurs().line_size(), d, D = dimension, calc_cl = !sub_type(Zone_PolyMAC_V2, zone); //en PolyMAC V1, on calcule aux CL

  /* contributions aux faces (par chaque voisin), aux elems */
  DoubleTrav a_f(N), rho_f(N), val_f(N), rho_m(2);
  for (a_f = 1, f = 0; f < zone.nb_faces(); f++) if (!fcl(f, 0)) //face interne
      {
        if (1)
          {
            for (i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++) for (d = 0; d < D; d++)
                  secmem(f, n) += vfd(f, i) * pf(f) * (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * nf(f, d) / fs(f) * vals(!cS * e, N * d + n);
          }

        if (0)
          {
            if (alp) for (a_f = 0, i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++) a_f(n) += vfd(f, i) / vf(f) * (*alp)(e, n);
            for (rho_m = 0, i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++) rho_m(i) += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n);
            for (i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++)
                {
                  double vnf = 0;
                  for (d = 0; d < D; d++) vnf += nf(f, d) / fs(f) * vals(!cS * e, N * d + n);
                  int strat = (i ? 1 : -1) * (rho_m(i) - rho(!cR * e, n)) * vnf > 0;
                  double R = alp && strat ? ((*alp)(e, n) < 1e-4 ? 1 : 0) /* min(max(1 - (*alp)(e, n) / 1e-4, 0.), 1.) */ : 0;
                  secmem(f, n) += vfd(f, i) * pf(f) * a_f(n) * (R * rho_m(i) + (1 - R) * rho(!cR * e, n)) * vnf;
                  // Cerr << "f " << f << " i " << i << " n " << n << " a " << (*alp)(e, n) << " r " << rho(!cR * e, n) << " R " << R << finl;
                }
          }
      }
    else if (calc_cl || fcl(f, 0) < 2) for (e = f_e(f, 0), n = 0; n < N; n++) for (d = 0; d < D; d++) //face de bord non imposee -> avec le (alpha rho) de la maille
          secmem(f, n) += pf(f) * vf(f) * (alp ? (*alp)(e, n) * rho(!cR * e, n) : 1) * nf(f, d) / fs(f) * vals(!cS * e, N * d + n);

  /* en PolyMAC V2 : partie aux elements */
  if (sub_type(Zone_PolyMAC_V2, zone)) for (e = 0; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
          secmem(nf_tot + D * e + d, n) += pe(e) * ve(e) * (alp ? rho(!cR * e, n) * (*alp)(e, n) : 1) * vals(!cS * e, N * d + n);
}

void Terme_Source_Qdm_Face_PolyMAC::mettre_a_jour(double temps)
{
  la_source->mettre_a_jour(temps);
}
