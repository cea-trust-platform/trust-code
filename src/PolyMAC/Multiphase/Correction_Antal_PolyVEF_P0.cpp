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

#include <Correction_Antal_PolyVEF_P0.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Pb_Multiphase.h>
#include <math.h>

Implemente_instanciable(Correction_Antal_PolyVEF_P0, "Correction_Antal_Face_PolyVEF_P0", Source_base);
// XD Correction_Antal source_base Correction_Antal 1 Antal correction source term for multiphase problem

Sortie& Correction_Antal_PolyVEF_P0::printOn(Sortie& os) const
{
  return os;
}

Entree& Correction_Antal_PolyVEF_P0::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("Cw1", &Cw1_);
  param.ajouter("Cw2", &Cw2_);
  param.lire_avec_accolades_depuis(is);

  //identification des phases
  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  pbm->creer_champ("distance_paroi_globale"); // Besoin de distance a la paroi

  return is;
}

void Correction_Antal_PolyVEF_P0::completer() // We must wait for all readOn's to be sure that the bubble dispersion and lift correlations are created
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());

  if (!pbm.has_champ("diametre_bulles")) Process::exit("Correction_Lubchenko_PolyVEF_P0::completer() : a bubble diameter must be defined !");
}


void Correction_Antal_PolyVEF_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
}

void Correction_Antal_PolyVEF_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue());
  const DoubleTab& pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe(),
                    &rho   = equation().milieu().masse_volumique().passe(),
                     &d_bulles = equation().probleme().get_champ("diametre_bulles").valeurs();
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = equation().milieu().porosite_face(),
                    &vf = domaine.volumes_entrelaces();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir(),
                   &y_faces = domaine.y_faces(),
                    &n_y_faces = domaine.normale_paroi_faces();
  int D = dimension,
      N = pvit.line_size()/D ,
      nf = domaine.nb_faces();

  DoubleTrav dv(N, N), parallel_velocity(N,D), scal_u(N) ;
  int e, f, k, d;

  double fac, a_l, rho_l, db_l, secmem_l;

  for (f = 0; f < nf; f++)
    if (y_faces(f) > 1.e-10)
      {
        // Fill local velocity
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            parallel_velocity(k, d) = pvit(f, N*d+k);

        // Retract component normal to the wall
        scal_u = 0;
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            scal_u(k) += parallel_velocity(k, d)*n_y_faces(f, d);
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            parallel_velocity(k, d) -= scal_u(k)*n_y_faces(f, d) ;

        // Calculation of liquid-gas velocity difference
        dv = 0.;
        for ( k = 0; k < N; k++)
          if (k != n_l)
            {
              for (d = 0 ; d<D ; d++)  dv(k, n_l) += (parallel_velocity(k, d)-parallel_velocity(n_l, d)) * (parallel_velocity(k, d)-parallel_velocity(n_l, d));
              dv(k, n_l) = std::sqrt(dv(k, n_l));
            }

        for (k = 0; k < N; k++)
          if (k != n_l)
            {
              fac = pf(f) * vf(f) ;
              a_l = (    alpha(f_e(f, 0), k)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? alpha(e, k)*vf_dir(f,1) : 0)  ) / vf(f);
              rho_l=(    rho(f_e(f, 0), n_l)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? rho(e, n_l)*vf_dir(f,1) : 0)  ) / vf(f);
              db_l= ( d_bulles(f_e(f, 0), k)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? d_bulles(e, k)*vf_dir(f,1):0) ) / vf(f);

              for (d=0 ; d<D ; d++)
                {
                  secmem_l = fac * n_y_faces(f, d) * 2. * a_l * rho_l * dv(k,n_l) * dv(k,n_l) / db_l * std::max(0., Cw1_ + Cw2_*db_l/(2.*y_faces(f))) ;
                  secmem(f, N*d+k)   += secmem_l;
                  secmem(f, N*d+n_l) -= secmem_l;
                }
            }
      }
}
