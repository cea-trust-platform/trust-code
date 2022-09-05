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

#ifndef Flux_parietal_base_included
#define Flux_parietal_base_included

#include <TRUSTTabs_forward.h>
#include <Correlation_base.h>

/*! @brief classe Flux_parietal_base correlations de flux parietal de la forme
 *
 *         flux de chaleur sensible : q_{p}(k)     = F(alpha_f, p, T_f, T_p, v_f, D_h, D_ch)
 *         flux de chaleur latente  : q_{pi}(k, l) = F(alpha_f, p, T_f, T_p, v_f, D_h, D_ch)
 *           (par ex ebullition nucleee : Gamma_{kl} = q_{pi}(k, l) / Lvap)
 *       cette classe definit deux fonctions q_pk, q_pi
 */

class Flux_parietal_base : public Correlation_base
{
  Declare_base(Flux_parietal_base);
public:

  /* parametres d'entree */
  struct input_t
  {
    int N;                // nombre de phases
    int f;                //
    double D_h;           // diametre hyd
    double D_ch;          // diametre hyd chauffant
    double p;             // pression
    double Tp;            // temperature de la paroi (une seule!)
    const double *alpha;  // alpha[n]  -> taux de presence de la phase n
    const double *T;      // T[n]      -> temperature de la phase n
    const double *v;      // v[n]      -> norme de la vitesse de la phase n
    const double *lambda; // lambda[n] -> conductivite de la phase n
    const double *mu;     // mu[n]     -> viscosite de la phase n
    const double *rho;    // rho[n]    -> masse volumique de la phase n
    const double *Cp;     // Cp[n]     -> capacite calorifique de la phase n
  };
  /* valeurs de sortie */
  struct output_t
  {
    DoubleTab *qpk = NULL;     // (*qpk)(n)           -> flux de chaleur vers la phase n
    DoubleTab *da_qpk = NULL;  // (*da_qpk)(n, m)     -> derivee par rapport a alpha_m
    DoubleTab *dp_qpk = NULL;  // (*dp_qpk)(n)        -> derivee par rapport a p
    DoubleTab *dv_qpk = NULL;  // (*dv_qpk)(n, m)     -> derivee par rapport a v[m]
    DoubleTab *dTf_qpk = NULL; // (*dTf_qpk)(n, m)    -> derivee par rapport a T[m]
    DoubleTab *dTp_qpk = NULL; // (*dTp_qpk)(n)       -> derivee par rapport a Tp
    DoubleTab *qpi = NULL;     // (*qpi)(k, l)        -> flux de chaleur fourni au changement de la phase k vers la phase l (a remplir pour k < l)
    DoubleTab *da_qpi = NULL;  // (*da_qpi)(k, l, m)  -> derivee par rapport a alpha_m
    DoubleTab *dp_qpi = NULL;  // (*dp_qpi)(k, l)     -> derivee par rapport a p
    DoubleTab *dv_qpi = NULL;  // (*dv_qpi)(k, l,m)   -> derivee par rapport a v[m]
    DoubleTab *dTf_qpi = NULL; // (*dTf_qpi)(k, l, m) -> derivee par rapport a T[m]
    DoubleTab *dTp_qpi = NULL; // (*dTp_qpi)(k, l)    -> derivee par rapport a Tp
    DoubleTab *d_nuc = NULL;   // (*d_nucleation)(k)  -> diametre de nucleation de la phase k
    int *nonlinear = NULL;     // nonlinear           -> regler a 1 si q_pk / q_pi est non-lineaire en Tp / Tf; ne pas toucher sinon
  };
  virtual void qp(const input_t& input, output_t& output) const = 0;

  virtual int calculates_bubble_nucleation_diameter() const {return 0;};
};

#endif
