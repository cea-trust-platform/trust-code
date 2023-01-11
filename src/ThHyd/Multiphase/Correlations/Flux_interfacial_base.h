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

#ifndef Flux_interfacial_base_included
#define Flux_interfacial_base_included

#include <Correlation_base.h>
#include <TRUSTTabs.h>

/*! @brief classe Flux_interfacial_base correlations de flux de chaleur interfacial de la forme
 *
 *       Phi_{kl} = h_{kl}(T_k - T_l)
 *       cette classe definit une fonction flux avec :
 *     entrees :
 *         D_h       -> diametre hyd
 *         alpha[n]  -> taux de presence de la phase n
 *         T[n]      -> temperature de la phase n
 *         p         -> pression
 *         nv[N * n + k]     -> norme de la vitesse de la phase n en si k == n, norme de v_k-v_n si k != n
 *         lambda[n], mu[n], rho[n], Cp[n] -> diverses proprietes physiques de la phase n
 *
 *     sorties :
 *        hi(k, l)    -> coeff d'echange entre la phase k et l'interface avec la phase l (hi(l, k) != hi(k, l) !)
 *     dT_hi(k, l, n) -> derivee de hi(k, l) en T[n]
 *     da_hi(k, l, n) -> derivee de hi(k, l) en a[n]
 *     dp_hi(k, l)    -> derivee de hi(k, l) en p
 *
 *
 */
class Flux_interfacial_base : public Correlation_base
{
  Declare_base(Flux_interfacial_base);
public:
  /* parametres d'entree */
  struct input_t
  {
    double dh;            // diametre hyd
    const double *alpha;  // alpha[n] : taux de vide de la phase n
    const double *T;      // T[n]     : temperature de la phase n
    const double *T_passe;// T_passe[n]: temperature de la phase n a l'iteration precedente
    double p;             // pression
    const double *nv;     // nv[N * k + l] : norme de ||v_k - v_l||
    const double *lambda; // lambda[n]     : conductivite de la phase n
    const double *mu;     // mu[n]         : viscosite dynamique de la phase n
    const double *rho;    // rho[n]        : masse volumique de la phase n
    const double *Cp;     // CP[n]         : capacite calorifique de la phase n
    const double *Lvap;   // Lvap[ind_trav]  : chaleur latente changement de phase n=>k ou ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1)
    const double *dP_Lvap;//dP_Lvap[ind_trav]: chaleur latente changement de phase n=>k ou ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1)
    const double *h;      // h[n]          : enthalpie de la phase n
    const double *dP_h;   // dP_h[n]       : derivee en pression de l'enthalpie de la phase n
    const double *dT_h;   // dT_h[n]       : deritee en temperature de la phase n de l'enthalpie de la phase n
    const double *d_bulles;//d_bulles[n]   : diametre de bulles de la phase n
    const double *k_turb; // k_turb[n]     : energie cinetique turbulente de la phase n
    const double *nut;    // nut[n]        : viscosite turbulente de bulles de la phase n
    DoubleTab v;          // v(n, d)       : vitesse de la phase n dans la direction d
    int e;                // indice d'element
  };
  /* valeurs de sortie */
  struct output_t
  {
    DoubleTab hi;    //hi(k, l)       : coeff d'echange entre la phase k et l'interface avec la phase l (hi(l, k) != hi(k, l) !)
    DoubleTab dT_hi; //dT_hi(k, l, n) : derivee de hi(k, l) en T[n]
    DoubleTab da_hi; //da_hi(k, l, n) : derivee de hi(k, l) en a[n]
    DoubleTab dp_hi; //dp_hi(k, l)    : derivee de hi(k, l) en p
  };
  virtual void coeffs(const input_t& input, output_t& output) const = 0;
  double dv_min() const {return dv_min_;};
  double dv_min_ = 0.01;
};

#endif
