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

#ifndef Champ_Face_PolyVEF_P0_included
#define Champ_Face_PolyVEF_P0_included

#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Domaine_PolyVEF_P0.h>
#include <SolveurSys.h>

/*! @brief : class Champ_Face_PolyVEF_P0
 *
 *  Champ correspondant a une inconnue decrite par ses tangentes aux faces duales (ligne amont-aval, type vitesse)
 *  Les flux aux faces sont accessibles par les methodes valeurs_normales(), avec synchro automatique
 *  Degres de libertes : composantes tangentielles aux faces duales
 *
 *
 */

class Champ_Face_PolyVEF_P0 : public Champ_Face_PolyMAC_P0P1NC
{
  Declare_instanciable(Champ_Face_PolyVEF_P0) ;

public :
  inline const Domaine_PolyVEF_P0& domaine_PolyVEF_P0() const { return ref_cast(Domaine_PolyVEF_P0, le_dom_VF.valeur()); }

  int fixer_nb_valeurs_nodales(int n) override;
  void init_auxiliary_variables() override { } /* rien */

  /* reconstruction a l'ordre 2 des composantes tangentes aux faces de la vitesse -> pour eviter les modes parasites */
  void init_vf2() const;
  mutable IntTab vf2d, vf2j, vf2bj;
  mutable DoubleTab vf2c, vf2bc;
  void update_vf2(DoubleTab& val, int incr = 0) const;

  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  // DoubleTab& valeur_aux_elems_passe(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const override;
  DoubleTab& valeur_aux_faces(DoubleTab& result) const override
  {
    result = valeurs();
    return result;
  }
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double, int distant ) const override;

  Champ_base& affecter_(const Champ_base& ) override;

  /* ||v_k|| (si l < 0) ou ||v_k - v_l|| (si l > 0) a une face f */
  inline double v_norm(const DoubleTab& val, int f, int k, int l, double *dnv) const
  {
    double acc = 0;
    int d, D = dimension, N = val.dimension(1) / D;
    for (d = 0; d < D; d++) acc += std::pow(val(f, N * d + k) - (l >= 0 ? val(f, N * d + l) : 0), 2);
    acc = sqrt(acc);
    if (dnv)
      for (d = 0; d < D; d++) dnv[d] = (val(f, N * d + k) - (l >= 0 ? val(f, N * d + l) : 0)) / acc;
    return acc;
  }
};

#endif /* Champ_Face_PolyVEF_P0_included */
