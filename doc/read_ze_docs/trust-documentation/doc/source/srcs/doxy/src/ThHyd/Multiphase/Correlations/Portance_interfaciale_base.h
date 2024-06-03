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

#ifndef Portance_interfaciale_base_included
#define Portance_interfaciale_base_included
#include <Correlation_base.h>
#include <TRUSTTab.h>

/*! @brief classe Portance_interfaciale_base utilitaire pour les operateurs de frottement interfacial prenant la forme
 *
 *       F_{0l} = - F_{l0} = C_{0l} (u_l - u_0) x rot(u_0) ou la phase
 *       0 est la phase porteuse et l != 0 une phase quelconque
 *       cette classe definit une fonction C_{kl} dependant de :
 *         alpha, p, T -> inconnues (une valeur par phase chacune)
 *         rho, mu, sigma -> proprietes physiques (idem)
 *         ndv(k, l) -> ||v_k - v_l||, a remplir pour k < l
 *     sortie :
 *         coeff(k, l, 0/1) -> coefficient C_{kl} et sa derivee en ndv(k, l), rempli pour k < l
 *
 *
 */

class Portance_interfaciale_base : public Correlation_base
{
  Declare_base(Portance_interfaciale_base);
public:
  struct input_t
  {
    double dh;            // diametre hyd
    DoubleTab alpha;  // alpha[n] : taux de vide de la phase n
    DoubleTab T;      // T[n]     : temperature de la phase n
    DoubleTab p;             // pression
    DoubleTab nv;     // nv[k, l] : norme de ||v_k - v_l||
    DoubleTab mu;     // mu[n]         : viscosite dynamique de la phase n
    DoubleTab rho;    // rho[n]        : masse volumique de la phase n
    DoubleTab sigma;  // sigma[ind_trav]:tension superficielle sigma(ind_trav), ind_trav = (n*(N-1)-(n-1)*(n)/2) + (m-n-1)
    DoubleTab k_turb; // k_turb[n]     : energie cinetique turbulente de la phase n
    DoubleTab d_bulles;//d_bulles[n]   : diametre de bulles de la phase n
    int e;                // indice d'element
  };
  /* valeurs de sortie */
  struct output_t
  {
    DoubleTab Cl;    //Cl(k, l)       : coeff de portance entre les phases k et l
  };

  virtual void coefficient(const input_t& input, output_t& output) const  = 0;
};

#endif
