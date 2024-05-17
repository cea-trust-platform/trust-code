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

#include <Echange_externe_impose.h>
#include <Equation_base.h>

Implemente_instanciable(Echange_externe_impose, "Paroi_echange_externe_impose", Echange_impose_base);
// XD paroi_echange_externe_impose condlim_base paroi_echange_externe_impose -1 External type exchange condition with a heat exchange coefficient and an imposed external temperature.
// XD attr h_imp chaine h_imp 0 Heat exchange coefficient value (expressed in W.m-2.K-1).
// XD attr himpc front_field_base himpc 0 Boundary field type.
// XD attr text chaine text 0 External temperature value (expressed in oC or K).
// XD attr ch front_field_base ch 0 Boundary field type.


Sortie& Echange_externe_impose::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Echange_externe_impose::readOn(Entree& s) { return Echange_impose_base::readOn(s); }

void Echange_externe_impose::verifie_ch_init_nb_comp() const
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = domaine_Cl_dis().equation();
      const int nb_comp = le_champ_front.valeur().nb_comp();
      eq.verifie_ch_init_nb_comp_cl(eq.inconnue(), nb_comp, *this);
    }
}
