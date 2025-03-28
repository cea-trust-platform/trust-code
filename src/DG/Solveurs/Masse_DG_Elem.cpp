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

#include <Masse_DG_Elem.h>
#include <Champ_Elem_DG.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

Implemente_instanciable(Masse_DG_Elem, "Masse_DG_Elem", Masse_DG_base);

Sortie& Masse_DG_Elem::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_DG_Elem::readOn(Entree& s) { return s; }

DoubleTab& Masse_DG_Elem::appliquer_impl(DoubleTab& sm) const
{
  if (le_dom_dg_->gram_schmidt())
    {
      const DoubleVect& volume = le_dom_dg_->volumes();

      tab_divide_any_shape(sm, volume);
      sm.echange_espace_virtuel();
    }
  else
    {
      const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
      const Quadrature_base& quad = le_dom_dg_->get_quadrature();
      DoubleTab res;
      DoubleTab invSm;
      invSm.copy(sm, RESIZE_OPTIONS::COPY_INIT);

      for (int num_elem = 0; num_elem < le_dom_dg_->nb_elem(); num_elem++)
        {
          Matrice_Dense invM = ch.eval_invMassMatrix(quad, num_elem);
          res.ref_tab(sm, num_elem, 1);

          invM.multvect(invSm, res);
        }
    }

  return sm;
}
