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

#include <Correction_Antal_PolyMAC_P0.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Pb_Multiphase.h>
#include <math.h>

Implemente_instanciable(Correction_Antal_PolyMAC_P0, "Correction_Antal_Face_PolyMAC_P0", Source_base);
// XD Correction_Antal source_base Correction_Antal 1 Antal correction source term for multiphase problem

Sortie& Correction_Antal_PolyMAC_P0::printOn(Sortie& os) const
{
  return os;
}

Entree& Correction_Antal_PolyMAC_P0::readOn(Entree& is)
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

void Correction_Antal_PolyMAC_P0::completer() // We must wait for all readOn's to be sure that the bubble dispersion and lift correlations are created
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());

  if (!pbm.has_champ("diametre_bulles")) Process::exit("Correction_Lubchenko_PolyMAC_P0::completer() : a bubble diameter must be defined !");
}


void Correction_Antal_PolyMAC_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
}

void Correction_Antal_PolyMAC_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  const DoubleTab& pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue()->passe(),
                    &rho   = equation().milieu().masse_volumique()->passe(),
                     &d_bulles = equation().probleme().get_champ("diametre_bulles").valeurs();
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins(),
                &fcl = ch.fcl();
  const DoubleVect& pe = equation().milieu().porosite_elem(),
                    &pf = equation().milieu().porosite_face(),
                     &ve = domaine.volumes(),
                      &vf = domaine.volumes_entrelaces(),
                       &fs = domaine.face_surfaces();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir(),
                   &n_f = domaine.face_normales(),
                    &y_elem = domaine.y_elem(),
                     &y_faces = domaine.y_faces(),
                      &n_y_elem = domaine.normale_paroi_elem(),
                       &n_y_faces = domaine.normale_paroi_faces();
  int N = pvit.line_size() ,
      D = dimension,
      nf_tot = domaine.nb_faces_tot(),
      nf = domaine.nb_faces(),
      ne_tot = domaine.nb_elem_tot();

  DoubleTrav dv(N, N), pvit_l(N,D), scal_u(N) ;
  int e, f, c, k, d, i;

  double fac, a_l, rho_l, db_l, secmem_l;

  for (f = 0; f < nf; f++)
    if (fcl(f, 0) < 2)
      {
        // Calculation of correct velocity at the face
        pvit_l = 0 ;
        for (d = 0 ; d<D ; d++)
          for (k = 0 ; k<N ; k++)
            for (c=0 ; c<2 && (e = f_e(f, c)) >= 0; c++)
              pvit_l(k, d) += vf_dir(f, c)/vf(f)*pvit(nf_tot+D*e+d, k) ;
        scal_u = 0;
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            scal_u(k) += pvit_l(k, d)*n_f(f, d)/fs(f);
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            pvit_l(k, d) += (pvit(f, k) - scal_u(k)) * n_f(f, d)/fs(f) ;

        // Retract component normal to the wall
        scal_u = 0;
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            scal_u(k) += pvit_l(k, d)*n_y_faces(f, d);
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            pvit_l(k, d) -= scal_u(k)*n_y_faces(f, d) ;

        // Calculation of liuqid-gas velocity difference
        dv = 0.;
        for ( k = 0; k < N; k++)
          if (k != n_l)
            {
              for (d = 0 ; d<D ; d++)  dv(k, n_l) += (pvit_l(k, d)-pvit_l(n_l, d)) * (pvit_l(k, d)-pvit_l(n_l, d));
              dv(k, n_l) = std::sqrt(dv(k, n_l));
            }

        for (k = 0; k < N; k++)
          if (k != n_l)
            {
              fac = 0 ;
              for (d = 0 ; d<D ; d++) fac += n_y_faces(f, d) * n_f(f, d)/fs(f);

              fac *= pf(f) * vf(f) ;
              a_l = (    alpha(f_e(f, 0), k)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? alpha(e, k)*vf_dir(f,1) : 0)  ) / vf(f);
              rho_l=(    rho(f_e(f, 0), n_l)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? rho(e, n_l)*vf_dir(f,1) : 0)  ) / vf(f);
              db_l= ( d_bulles(f_e(f, 0), k)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? d_bulles(e, k)*vf_dir(f,1):0) ) / vf(f);

              secmem_l = fac * 2. * a_l * rho_l * dv(k,n_l) * dv(k,n_l) / db_l * std::max(0., Cw1_ + Cw2_*db_l/(2.*y_faces(f))) ;

              secmem(f, k)   += secmem_l;
              secmem(f, n_l) -= secmem_l;
            }

      }

  for ( e = 0; e < ne_tot; e++)
    {
      pvit_l = 0;
      // Fill velocity at the element
      for (d = 0 ; d<D ; d++)
        for (k = 0 ; k<N ; k++)
          pvit_l(k, d) += pvit(nf_tot+D*e+d, k) ;

      // Retract component normal to the wall
      scal_u = 0;
      for (k = 0 ; k<N ; k++)
        for (d = 0 ; d<D ; d++)
          scal_u(k) += pvit_l(k, d)*n_y_elem(e, d);
      for (k = 0 ; k<N ; k++)
        for (d = 0 ; d<D ; d++)
          pvit_l(k, d) -= scal_u(k)*n_y_elem(e, d) ;

      // Calculation of norm of velocity
      dv = 0.;
      for ( k = 0; k < N; k++)
        if (k != n_l)
          {
            for (d = 0 ; d<D ; d++)  dv(k, n_l) +=  (pvit_l(k, d)-pvit_l(n_l, d)) * (pvit_l(k, d)-pvit_l(n_l, d));
            dv(k, n_l) = std::sqrt(dv(k, n_l));
          }

      for (k = 0; k < N; k++)
        if (k != n_l)
          {
            fac = pe(e) * ve(e) ;
            secmem_l = fac * 2. * alpha(e,k) * rho(e,n_l) * dv(k,n_l) * dv(k,n_l) / d_bulles(e,k) * std::max(0., Cw1_ + Cw2_*d_bulles(e,k)/(2.*y_elem(e))) ;

            for ( d = 0, i = nf_tot + D * e; d < D; d++, i++)
              {
                secmem(i , k)   += secmem_l * n_y_elem(e, d);
                secmem(i , n_l) -= secmem_l * n_y_elem(e, d);
              }
          }
    }
}
