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

#include <Flux_parietal_diphasique_simple_lineaire.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Flux_parietal_diphasique_simple_lineaire, "Flux_parietal_diphasique_simple_lineaire", Flux_parietal_base);

Sortie& Flux_parietal_diphasique_simple_lineaire::printOn(Sortie& os) const { return os; }

Entree& Flux_parietal_diphasique_simple_lineaire::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("coeff_echange_monophasique|single_phase_exchange_coeff", &h_mono_, Param::REQUIRED);
  param.ajouter("coeff_osv|osv_coeff", &coeff_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  /* n_l / n_g : phases liquide / gaz continu */
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : nullptr;

  if (!pbm || pbm->nb_phases() == 1) // pas un Pb_Multiphase -> monophasique liquide
    Process::exit("Flux_parietal_diphasique_simple_lineaire can only be used for a multiphase problem with 2 phases!");
  else // recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    {
      for (int n = 0; n < pbm->nb_phases(); n++)
        if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))
          n_l = n;
        else if (pbm->nom_phase(n).debute_par("gaz") && (n_g < 0 || pbm->nom_phase(n).finit_par("continu")))
          n_g = n;
    }

  if (n_l < 0)
    Process::exit(que_suis_je() + " : main phase not found!");

  assert (n_l >= 0 && n_g >= 0);

  const Milieu_composite *mc = sub_type(Milieu_composite, pb_->milieu()) ? &ref_cast(Milieu_composite, pb_->milieu()) : nullptr;

  if (mc->has_saturation(n_l, n_g))
    sat = &mc->get_saturation(n_l, n_g);

  if (!sat)
    Process::exit("Flux_parietal_diphasique_simple_lineaire needs saturation!");

  return is;
}

double Flux_parietal_diphasique_simple_lineaire::fraction_flux_vap(const input_t& in, double phi, double dTf_phi, double dp_phi, double dTp_phi, double& dTf_eps, double& dp_eps, double& dTp_eps) const
{
  const double Ts = sat->Tsat(in.p),
               Tld = Ts - phi * coeff_,
               dp_Tld = sat->dP_Tsat(in.p) - dp_phi * coeff_,
               dTp_Tld = -dTp_phi * coeff_,
               dTf_Tld = -dTf_phi * coeff_;

  const double eps = std::min(1.0, std::max(0.0, (in.T[n_l] - Tld) / (Ts - Tld)));

  if (in.T[n_l] > Tld && in.T[n_l] < Ts) // derivee non nulle
    {
      dTf_eps = ((Ts - Tld) * (-dTf_Tld) + (in.T[n_l] - Tld) * dTf_Tld) / (Ts - Tld) / (Ts - Tld);
      dp_eps = ((Ts - Tld) * (-dp_Tld) - (in.T[n_l] - Tld) * (sat->dP_Tsat(in.p) - dp_Tld)) / (Ts - Tld) / (Ts - Tld);
      dTp_eps = ((Ts - Tld) * (-dTp_Tld) + (in.T[n_l] - Tld) * dTp_Tld) / (Ts - Tld) / (Ts - Tld);
    }

  return eps;
}

void Flux_parietal_diphasique_simple_lineaire::qp(const input_t& in, output_t& out) const
{
  double Ts = sat->Tsat(in.p) ;

  const Milieu_composite& milc = ref_cast(Milieu_composite, pb_->milieu());

  if (out.nonlinear)
    (*out.nonlinear) = 1; // we turn on nonlinear in case it goes to two-phase during the newton

  for (int k = 0; k < in.N; k++)
    if (n_l != k)
      if (milc.has_saturation(n_l, k))
        {
          const double P_mono = h_mono_ * (in.Tp - in.T[n_l]);

          if (in.Tp < Ts)
            {
              if (out.qpk)
                (*out.qpk) = P_mono;
              if (out.dTp_qpk)
                (*out.dTp_qpk)(n_l) = h_mono_;
              if (out.dTf_qpk)
                (*out.dTf_qpk)(n_l, n_l) = -h_mono_;
            }
          else
            {
              if (Ts <= in.T[n_l]) // Everything in the evaporative term
                {
                  if (out.qpi)
                    (*out.qpi)(n_l, k) = P_mono;
                  if (out.dTp_qpi)
                    (*out.dTp_qpi)(n_l, k) = h_mono_;
                  if (out.dTf_qpi)
                    (*out.dTf_qpi)(n_l, k, n_l) = -h_mono_;
                }
              else
                {
                  double dTf_eps = 0.0, dp_eps = 0.0, dTp_eps = 0.0;
                  double eps = fraction_flux_vap(in, P_mono, -h_mono_, 0.0, h_mono_, dTf_eps, dp_eps, dTp_eps);

                  // Fill in the outputs : heat flux towards liquid
                  if (out.qpk)
                    (*out.qpk)(n_l) = (1.0 - eps) * P_mono;
                  if (out.dTp_qpk)
                    (*out.dTp_qpk)(n_l) = (1.0 - eps) * h_mono_ - P_mono * dTp_eps;
                  if (out.dTf_qpk)
                    (*out.dTf_qpk)(n_l, n_l) = -(1.0 - eps) * h_mono_ + P_mono * dTf_eps;

                  // Evaporation
                  if (out.qpi)
                    (*out.qpi)(n_l, k) = eps * P_mono;
                  if (out.dTp_qpi)
                    (*out.dTp_qpi)(n_l, k) = dTp_eps * P_mono + eps * h_mono_;
                  if (out.dTf_qpi)
                    (*out.dTf_qpi)(n_l, k, n_l) = dTf_eps * P_mono - eps * h_mono_;
                }
            }
        }
}
