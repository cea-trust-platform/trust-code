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

#ifndef Dispersion_bulles_base_included
#define Dispersion_bulles_base_included

#include <Correlation_base.h>
#include <TRUSTTab.h>

/*! @brief classe Dispersion_bulles_base utilitaire pour les operateurs de dispersion turbulente ou la force
 *
 *       exercee sur le gaz par le liquide prend la forme
 *       F_{kl} = - F_{lk} = - C_{kl} grad(alpha{k}) + C_{lk} grad(alpha{l}) ou la phase
 *       l est la phase liquide porteuse et k != 0 une phase quelconque
 *       cette classe definit une fonction C_{kl}!=C_{lk} dependant de :
 *         alpha, p, T -> inconnues (une valeur par phase chacune)
 *         rho, mu, sigma, nut, k_turb -> proprietes physiques (idem)
 *         ndv(k, l) -> ||v_k - v_l||, a remplir pour k < l
 *     sortie :
 *         coeff(k, l, 0/1) -> coefficient C_{kl} et sa derivee en ndv(k, l), rempli pour k < l
 *
 *
 */

class Dispersion_bulles_base : public Correlation_base
{
  Declare_base(Dispersion_bulles_base);
public:
  struct input_t
  {
    double dh = 0.;            // diametre hyd
    DoubleTab alpha;  // alpha[n] : taux de vide de la phase n
    DoubleTab T;      // T[n]     : temperature de la phase n
    DoubleTab p;      // pression
    DoubleTab rho;    // rho[n]        : masse volumique de la phase n
    DoubleTab mu;     // mu[n]         : viscosite dynamique de la phase n
    DoubleTab sigma;  // sigma[ind_trav]:tension superficielle sigma(ind_trav), ind_trav = (n*(N-1)-(n-1)*(n)/2) + (m-n-1)
    DoubleTab k_turb; // k_turb[n]     : energie cinetique turbulente de la phase n
    DoubleTab nut;    // nut[n]        : viscosite turbulente de la phase n
    DoubleTab d_bulles;//d_bulles[n]   : diametre de bulles de la phase n
    DoubleTab nv;     // nv(k, l) : norme de ||v_k - v_l||
    int e;                // indice d'element
  };
  /* valeurs de sortie */
  struct output_t
  {
    DoubleTab Ctd;    //Cl(k, l)       : coeff de portance entre les phases k et l
  };

  virtual void coefficient(const input_t& input, output_t& output) const  = 0;
};

#endif
