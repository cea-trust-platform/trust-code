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

#ifndef Op_Diff_PolyVEF_P0_Elem_included
#define Op_Diff_PolyVEF_P0_Elem_included

#include <Op_Diff_PolyVEF_P0_base.h>
#include <Matrice_Morse.h>

class Op_Diff_PolyVEF_P0_Elem : public Op_Diff_PolyVEF_P0_base
{

  Declare_instanciable_sans_constructeur( Op_Diff_PolyVEF_P0_Elem ) ;

public :
  Op_Diff_PolyVEF_P0_Elem();
  void completer() override;
  void init_op_ext() const override; //fait plus de travail qu'en PolyVEF_P0P1NC: op_ext + som_ext, pe_ext...

  /* interface {dimensionner,ajouter}_blocs */
  int has_interface_blocs() const override { return 1; }
  double calculer_dt_stab() const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const override { }
  void mettre_a_jour(double t) override;

  /* flux paroi_interface : d_nucleation(e, k) : diametre de nucleation de la phase k dans l'element e */
  const DoubleTab& d_nucleation() const;

private:
  /* sommets connectes a un autre probleme par un Echange_contact */
  void init_s_dist() const;
  mutable std::map<int, std::map<const Operateur_Diff_base *, int>> s_dist; //s_dist[som] = { { pb1, som1 }, { pb2, som2 }, ... }
  mutable int s_dist_init_ = 0;

  /* tableaux op_ext et pe_ext */
  mutable IntTab som_mix, som_ext_d, som_ext_pe, som_ext_pf; //sommet s = som_ext(i) : melange-t-il les composantes, couple (probleme, elem) dans som_ext_e([som_ext_d(i, 0), som_ext_d(i + 1, 0)[, 0/1)
  //faces Echange_contact (pb1, face1, pb2, face2) dans som_ext_f([som_ext_d(i, 1), som_ext_d(i + 1, 1)[, 0/1/2/3)
  mutable int som_ext_init_ = 0;

  /* tableau renvoye par d_nucleation(), rempli lors de ajouter_blocs() */
  mutable DoubleTab d_nuc_;
  mutable int d_nuc_a_jour_ = 0; //d_nucleation() est utilisable ("a jour") entre le moment ou on a appelle ajouter_blocs() et le mettre_a_jour() suivant
};

/* comme des synonymes, mais avec l'info de ce qu'on est dans que_suis_je() */
class Op_Dift_PolyVEF_P0_Elem : public Op_Diff_PolyVEF_P0_Elem
{
  Declare_instanciable( Op_Dift_PolyVEF_P0_Elem ) ;
};

#endif /* Op_Diff_PolyVEF_P0_Elem_included */
