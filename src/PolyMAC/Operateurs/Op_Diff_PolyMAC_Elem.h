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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Diff_PolyMAC_Elem.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_PolyMAC_Elem_included
#define Op_Diff_PolyMAC_Elem_included

#include <Op_Diff_PolyMAC_base.h>
#include <Matrice_Morse.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Op_Diff_PolyMAC_Elem
//
// <Description of class Op_Diff_PolyMAC_Elem>
//
/////////////////////////////////////////////////////////////////////////////

class Op_Diff_PolyMAC_Elem : public Op_Diff_PolyMAC_base
{

  Declare_instanciable_sans_constructeur( Op_Diff_PolyMAC_Elem ) ;

public :
  Op_Diff_PolyMAC_Elem();
  void completer() override;
  void init_op_ext() const; //remplissage de op_ext (ne peut pas etre fait dans completer(), trop tot)
  // virtual void calculer_flux_bord(const DoubleTab& inco) const { abort(); };

  /* interface {dimensionner,ajouter}_blocs */
  double calculer_dt_stab() const override;
  void dimensionner_blocs_ext(int aux_only, matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs_ext(int aux_only, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  void mettre_a_jour(double t) override;

  /* flux paroi_interface : q_pi(e, k, l) : flux de chaleur contribuant au changement de phase k->l dans l'element e */
  const DoubleTab& q_pi() const;

  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const override { };

private:
  /* tableau renvoye par q_pi(), rempli lors de ajouter_blocs() */
  mutable DoubleTab q_pi_;
  mutable int q_pi_a_jour_ = 0; //q_pi() est utilisable ("a jour") entre le moment ou on a appelle ajouter_blocs() et le mettre_a_jour() suivant
};

Declare_ref(Op_Diff_PolyMAC_Elem);
#endif /* Op_Diff_PolyMAC_Elem_included */
