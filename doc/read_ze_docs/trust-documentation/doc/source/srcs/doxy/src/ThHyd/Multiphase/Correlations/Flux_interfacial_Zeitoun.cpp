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

#include <Flux_interfacial_Zeitoun.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Flux_interfacial_Zeitoun, "Flux_interfacial_Zeitoun", Flux_interfacial_base);

Sortie& Flux_interfacial_Zeitoun::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_interfacial_Zeitoun::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("dv_min", &dv_min_);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Flux_interfacial_Zeitoun::completer()
{
  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (!pbm->has_champ("diametre_bulles")) Process::exit("Flux_interfacial_Ranz_Mashall : bubble diameter needed !");

  if (a_res_ < 1.e-12)
    {
      a_res_ = ref_cast(QDM_Multiphase, pb_->equation(0)).alpha_res();
      a_res_ = std::max(1.e-4, a_res_*10.);
    }

  a_res_ = -1.;
}


void Flux_interfacial_Zeitoun::coeffs(const input_t& in, output_t& out) const
{
  out.hi = 0.;
  out.da_hi= 0.;
  out.dp_hi = 0.;
  out.dT_hi= 0.;

  int k, N = out.hi.dimension(0);
  for (k = 0; k < N; k++)
    if (k != n_l)
      {
        int    ind_trav = (k>n_l) ? (n_l*(N-1)-(n_l-1)*(n_l)/2) + (k-n_l-1) : (k*(N-1)-(k-1)*(k)/2) + (n_l-k-1);
        if ( in.T[k] >  in.T[n_l]) // Cas condensation
          {
            double Re_b   = in.rho[n_l] * in.nv[N * n_l + k] * in.d_bulles[k] / in.mu[n_l];

            double Ja     = std::max(in.T[k] -  in.T[n_l], 2.) * in.rho[n_l] * in.Cp[n_l] / (in.rho[k] *             in.Lvap[ind_trav] );
            double dP_Ja  = std::max(in.T[k] -  in.T[n_l], 2.) * in.rho[n_l] * in.Cp[n_l] /  in.rho[k] * -in.dP_Lvap[ind_trav]/(in.Lvap[ind_trav]*in.Lvap[ind_trav]) ;
            double dTk_Ja = (in.T[k] -  in.T[n_l] > 2.) ?        in.rho[n_l] * in.Cp[n_l] / (in.rho[k] *             in.Lvap[ind_trav] ) : 0. ;
            double dTl_Ja = (in.T[k] -  in.T[n_l] > 2.) ?      - in.rho[n_l] * in.Cp[n_l] / (in.rho[k] *             in.Lvap[ind_trav] ) : 0. ;

            double Nu     = 2.04* std::pow(Re_b, .61)*            std::pow(std::max(in.alpha[k], a_min_coeff), 0.328)*              std::pow(Ja, -0.308);
            double da_Nu  = 2.04* std::pow(Re_b, .61)*( (in.alpha[k] > a_min_coeff) ? 0.328*std::pow(in.alpha[k], 0.328-1.) : 0. )* std::pow(Ja, -1.308);
            double dP_Nu  = 2.04* std::pow(Re_b, .61)*            std::pow(std::max(in.alpha[k], a_min_coeff), 0.328)*              -.308*dP_Ja  *std::pow(Ja, -1.308);
            double dTk_Nu = 2.04* std::pow(Re_b, .61)*            std::pow(std::max(in.alpha[k], a_min_coeff), 0.328)*              -.308*dTk_Ja *std::pow(Ja, -1.308);
            double dTl_Nu = 2.04* std::pow(Re_b, .61)*            std::pow(std::max(in.alpha[k], a_min_coeff), 0.328)*              -.308*dTl_Ja *std::pow(Ja, -1.308);

            out.hi(n_l, k)       =  Nu    * in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) * std::max(in.alpha[k], a_min); // std::max() pour que le flux interfacial sont non nul
            out.da_hi(n_l, k, k) = (in.alpha[k] > a_min ?
                                    Nu    * in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) :
                                    0.)
                                   + da_Nu* in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) * std::max(in.alpha[k], a_min);
            out.dp_hi(n_l, k)    = dP_Nu  * in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) * std::max(in.alpha[k], a_min);
            out.dT_hi(n_l, k,n_l)= dTl_Nu * in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) * std::max(in.alpha[k], a_min);
            out.dT_hi(n_l, k, k) = dTk_Nu * in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) * std::max(in.alpha[k], a_min);
            out.hi(k, n_l) = 1e8;
            if (in.alpha[k] < a_res_)
              {
                double hi_loc = out.hi(n_l, k);
                out.hi(n_l, k)       = 1.e8*(1-in.alpha[k]/a_res_) +       out.hi(n_l, k) * in.alpha[k]/a_res_;
                out.da_hi(n_l, k, k) =        -1.e8/a_res_         + out.da_hi(n_l, k, k) * in.alpha[k]/a_res_ + hi_loc/a_res_;
                out.dp_hi(n_l, k)    = out.dp_hi(n_l, k) * in.alpha[k]/a_res_ ;
              }
          }
        else  // Cas flashing
          {
            out.hi(n_l, k) = 1.e8;
            out.da_hi(n_l, k, k) = 0.;
            out.dp_hi(n_l, k)    = 0.;
            out.dT_hi(n_l, k,n_l)= 0.;
            out.dT_hi(n_l, k, k) = 0.;
            out.hi(k, n_l) = 1e8;
          }
      }
}
