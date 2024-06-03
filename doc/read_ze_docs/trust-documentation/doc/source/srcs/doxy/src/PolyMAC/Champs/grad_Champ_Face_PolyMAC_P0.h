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

#ifndef grad_Champ_Face_PolyMAC_P0_included
#define grad_Champ_Face_PolyMAC_P0_included

#include <Champ_Fonc_Face_PolyMAC.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <TRUST_Ref.h>

/*! @brief class grad_Champ_Face_PolyMAC_P0 for the calculation of the gradient This field is a Champ_Fonc_Face_PolyMAC_P0 that calculates the gradient of a velocity field
 *
 *       grad_u(f, n*D + dU) returns the gradient of the dU velocity component in the phase n at face f
 *       grad_u(nf_tot + e*D + dX, n*D + dU) returns the gradient of the dU velocity component in the phase n in element e along the dX component
 *         (i.e. = dU/dX)
 *       In 2D, the gradient at the element e in phase n is equal to :
 *                   |   grad_u(nf_tot + e*D + 0, n*D + 0)   grad_u(nf_tot + e*D + 1, n*D + 0)   |
 *         grad_u =  |                                                                           |
 *                   |   grad_u(nf_tot + e*D + 0, n*D + 1)   grad_u(nf_tot + e*D + 1, n*D + 1)   |
 *
 *       The gradient is calculated using past values of the velocity
 *         We compute the gradient at the faces for all velocity components, then interpolate it to obtain the gradient at the elements
 *
 */
class grad_Champ_Face_PolyMAC_P0: public Champ_Fonc_Face_PolyMAC
{
  Declare_instanciable(grad_Champ_Face_PolyMAC_P0);
public:
  int fixer_nb_valeurs_nodales(int n) override;
  void mettre_a_jour(double) override;
  void me_calculer(double);

  inline virtual Champ_Face_PolyMAC_P0& champ_a_deriver() { return champ_.valeur(); }
  inline virtual const Champ_Face_PolyMAC_P0& champ_a_deriver() const { return champ_.valeur(); }
  inline void associer_champ(const Champ_Face_PolyMAC_P0& ch) { champ_ = ch; }
  inline void associer_domaine_Cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
  {
    le_Dom_Cl_PolyMAC_P0P1NC = static_cast<const Domaine_Cl_PolyMAC&>(le_dom_Cl_dis_base);
  }

  // Interpolation du gradient de la vitesse
  mutable IntTab gradve_d, gradve_e;           // Tables utilisees dans fgrad pour obtenir le grad aux faces de la vitesse aux elems
  mutable DoubleTab gradve_w;
  void update_tab_grad(int full_stencil);      // Mise a jour des tables utilisees dans fgrad
  void calc_gradfve();                  // Mise a jour du gradient aux faces de la vitesse aux elements
  void update_ge();                           // Calcul du gradient aux elements a partir du gradient aux faces ; base sur la methode similaire de Champ_Face_PolyMAC_P0
  void init_grad();

protected:
  REF(Domaine_Cl_PolyMAC) le_Dom_Cl_PolyMAC_P0P1NC;
  REF(Champ_Face_PolyMAC_P0) champ_;

  int is_init = 0;
  void init_ge2() const; //ordre 2 -> avec une matrice
  mutable IntTab ve2d, ve2j, ve2bj;
  mutable DoubleTab ve2c, ve2bc;
  void update_ge2(DoubleTab& val, int incr = 0) const;
};

#endif /* grad_Champ_Face_PolyMAC_P0_included */
