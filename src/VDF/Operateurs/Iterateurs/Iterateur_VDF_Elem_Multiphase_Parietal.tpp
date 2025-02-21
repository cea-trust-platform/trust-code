/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Iterateur_VDF_Elem_Multiphase_Parietal_TPP_included
#define Iterateur_VDF_Elem_Multiphase_Parietal_TPP_included

#include <Source_Flux_interfacial_base.h>
#include <Flux_parietal_base.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>

template<class _TYPE_> template<typename Type_Double, typename BC>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_bords_flux_parietal_(const BC& cl, const int ndeb, const int nfin, const int N,
                                                                    const DoubleTab& donnee, DoubleTab& resu, Matrice_Morse *mat, VectorDeriv& d_cc, const tabs_t& semi_impl) const
{
  assert (is_pb_multi);
  Type_Double flux(N);

  constexpr bool is_Temp_impose_flux_parietal = std::is_same<BC, Scalaire_impose_paroi>::value,
                 is_Neumann_flux_parietal = std::is_same<BC, Neumann_paroi>::value,
                 is_paroi_contact_flux_parietal = std::is_same<BC, Echange_global_impose>::value;

  const Flux_parietal_base& corr = ref_cast(Flux_parietal_base, corr_flux_parietal_.valeur());
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, op_base->equation().probleme());

  const DoubleTab& alpha = pbm.equation_masse().inconnue().passe(),
                   &dh = pbm.milieu().diametre_hydraulique_elem(),
                    &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                     &lambda = pbm.milieu().conductivite().passe(),
                      &mu = ref_cast(Fluide_base, pbm.milieu()).viscosite_dynamique().passe(),
                       &rho = pbm.milieu().masse_volumique().passe(),
                        &Cp = pbm.milieu().capacite_calorifique().passe();

  const DoubleVect& fs = le_dom->face_surfaces(), &pf = pbm.milieu().porosite_face();

  Flux_parietal_base::input_t in;
  Flux_parietal_base::output_t out;

  DoubleTrav qpk(N), dTp_qpk(N), dTf_qpk(N, N), qpi(N, N), dTp_qpi(N, N), dTf_qpi(N, N, N), d_nuc(N);

  out.qpk = &qpk;
  out.dTf_qpk = &dTf_qpk;
  out.qpi = &qpi;
  out.dTf_qpi = &dTf_qpi;
  out.d_nuc = &d_nuc;

  if (is_Neumann_flux_parietal)
    {
      out.dTp_qpk = &dTp_qpk;
      out.dTp_qpi = &dTp_qpi;
    }

  // Vitesse passee aux elems
  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, pbm.equation_qdm().inconnue());
  DoubleTrav pvit_elem(0, N * dimension);
  le_dom->domaine().creer_tableau_elements(pvit_elem);
  ch.get_elem_vector_field(pvit_elem, true /* passe */ );

  const Source_Flux_interfacial_base *sss =
    op_base->equation().sources().size() && sub_type(Source_Flux_interfacial_base, op_base->equation().sources().dernier().valeur()) ?
    &ref_cast(Source_Flux_interfacial_base, op_base->equation().sources().dernier().valeur()) : nullptr;

  DoubleTab *pqpi = sss ? &(sss->qpi()) : nullptr;
  DoubleTab *pdTf_qpi = sss ? &(sss->dT_qpi()) : nullptr;

  if (pqpi)
    *pqpi = 0.;

  if (pdTf_qpi)
    *pdTf_qpi = 0.;

  DoubleTrav Ts_tab, Sigma_tab, Lvap_tab;
  if (corr.needs_saturation())
    {
      const Milieu_composite& milc = ref_cast(Milieu_composite, op_base->equation().milieu());
      const int nbelem_tot = le_dom->nb_elem_tot(), nb_max_sat = N * (N - 1) / 2; // oui !! suite arithmetique !!
      Ts_tab.resize(nbelem_tot, nb_max_sat), Sigma_tab.resize(nbelem_tot, nb_max_sat), Lvap_tab.resize(nbelem_tot, nb_max_sat);

      for (int k = 0; k < N; k++)
        for (int l = k + 1; l < N; l++)
          if (milc.has_saturation(k, l))
            {
              Saturation_base& z_sat = milc.get_saturation(k, l);
              const int ind_trav = (k * (N - 1) - (k - 1) * (k) / 2) + (l - k - 1); // Et oui ! matrice triang sup !
              assert(press.line_size() == 1);

              // recuperer Tsat et sigma ...
              const DoubleTab& sig = z_sat.get_sigma_tab(), &tsat = z_sat.get_Tsat_tab();

              // fill in the good case
              for (int ii = 0; ii < nbelem_tot; ii++)
                {
                  Ts_tab(ii, ind_trav) = tsat(ii);
                  Sigma_tab(ii, ind_trav) = sig(ii);
                }

              z_sat.Lvap(press.get_span_tot() /* elem reel */, Lvap_tab.get_span_tot(), nb_max_sat, ind_trav);
              z_sat.Tsat(press.get_span_tot() /* elem reel */, Ts_tab.get_span_tot(), nb_max_sat, ind_trav); // Tentative avec pression passee
            }
    }

  DoubleTrav Tf(N), nv(N);
  for (int face = ndeb; face < nfin; face++)
    {
      Tf = 0.;
      qpk = 0.;
      dTf_qpk = 0.;
      qpi = 0.;
      dTf_qpi = 0.;
      nv = 0.;
      d_nuc = 0.;

      const int e = elem(face, 0) > -1 ? elem(face, 0) : elem(face, 1);

      const double y = elem(face, 0) > -1 ? le_dom->dist_face_elem0(face, e) : le_dom->dist_face_elem1(face, e);

      // fill in struct
      in.N = N;
      in.f = face;
      in.y = y;
      in.D_h = dh(e);
      in.D_ch = dh(e);
      in.alpha = &alpha(e, 0);
      in.p = press(e);
      in.lambda = &lambda(e, 0);
      in.mu = &mu(e, 0);
      in.rho = &rho(e, 0);
      in.Cp = &Cp(e, 0);
      in.T = &donnee(e, 0);
      in.v = nv.addr();

      if (corr.needs_saturation())
        {
          in.Lvap = &Lvap_tab(e, 0);
          in.Sigma = &Sigma_tab(e, 0);
          in.Tsat = &Ts_tab(e, 0);
        }
      else
        {
          in.Lvap = nullptr;
          in.Sigma = nullptr;
          in.Tsat = nullptr;
        }

      // velocity norm
      for (int d = 0; d < Objet_U::dimension; d++)
        for (int n = 0; n < N; n++)
          nv(n) += std::pow(pvit_elem(e, N * d + n), 2);

      for (int n = 0; n < N; n++)
        nv(n) = std::sqrt(nv(n));

      // 3 Cas CLS
      if (is_Temp_impose_flux_parietal) // Dirichlet
        {
          in.Tp = ref_cast(Scalaire_impose_paroi, cl).val_imp(face - ndeb);

          corr.qp(in, out); // call correlation

          if (pqpi)
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                (*pqpi)(e, k, l) += qpi(k, l) * fs(face);

          if ((pdTf_qpi) && (mat))
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                for (int m = 0; m < N; m++)
                  (*pdTf_qpi)(e, k, l, m) += dTf_qpi(k, l, m) * fs(face);

          for (int k = 0; k < N; k++)
            flux[k] = (elem(face, 0) != -1) ? qpk(k) * fs(face) * pf(face) : -qpk(k) * fs(face) * pf(face);

          fill_flux_tables_(face, N, 1.0, flux, resu);

          if (mat)
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                (*mat)(e * N + k, e * N + l) += dTf_qpk(k, l) * fs(face) * pf(face);

        }
      if (is_paroi_contact_flux_parietal) // Echange contact
        {
          if (!cl.que_suis_je().debute_par("Paroi_Echange_contact")) Process::exit(que_suis_je() + " : in pb_multiphase there is no echange contact with a wall heat flux correlation : choisis ton camp camarade !");

        }
      else if (is_Neumann_flux_parietal) // Neumann
        {
          // Ici on fait un Newton simple
          in.Tp = donnee(e, 0);

          double flux_imp = ref_cast(Neumann, cl).flux_impose(face - ndeb);
          double flux_cor = 0., dTp_flux_cor = 0.;
          int it = 0, cv = 0;

          for (it = 0; !cv && it < 100; it++)
            {
              flux_cor = 0;
              dTp_flux_cor = 0;

              Tf = 0.;
              qpk = 0.;
              dTp_qpk = 0.;
              dTf_qpk = 0.;
              qpi = 0.;
              dTp_qpi = 0.;
              dTf_qpi = 0.;

              corr.qp(in, out);

              for (int k = 0; k < N; k++)
                {
                  flux_cor += qpk(k);
                  dTp_flux_cor += dTp_qpk(k);
                }

              for (int k = 0; k < N; k++)
                for (int l = 0; l < N; l++)
                  {
                    flux_cor += qpi(k, l);
                    dTp_flux_cor += dTp_qpi(k, l);
                  }

              in.Tp = in.Tp - (flux_cor - flux_imp) / dTp_flux_cor;
              cv = (std::abs(flux_cor - flux_imp) < 1.e-5);
            }

          if (!cv)
            {
              std::string message_erreur = "Non-convergence des Tefs !!!";
              message_erreur.append("\n Tp init = ");
              message_erreur.append(std::to_string( donnee(e, 0)));
              message_erreur.append("\n T fluide = ");
              for (int k = 0; k < N; k++)
                {
                  message_erreur.append(std::to_string(donnee(e, k)));
                  message_erreur.append(" ");
                }
              message_erreur.append("\n Phi cible = ");
              message_erreur.append(std::to_string(flux_imp));
              message_erreur.append("\n Phi cor = ");
              message_erreur.append(std::to_string(flux_cor));
              Process::exit(message_erreur);
            }

          if (pqpi)
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                (*pqpi)(e, k, l) += qpi(k, l) * fs(face);

          if ((pdTf_qpi) && (mat))
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                for (int m = 0; m < N; m++)
                  (*pdTf_qpi)(e, k, l, m) += dTf_qpi(k, l, m) * fs(face);

          for (int k = 0; k < N; k++)
            flux[k] = (elem(face, 0) != -1) ? qpk(k) * fs(face) : -qpk(k) * fs(face);

          fill_flux_tables_(face, N, 1.0, flux, resu);

          if (mat)
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                (*mat)(e * N + k, e * N + l) += dTf_qpk(k, l) * fs(face) * pf(face);
        }
    }
}

#endif /* Iterateur_VDF_Elem_Multiphase_Parietal_TPP_included */
