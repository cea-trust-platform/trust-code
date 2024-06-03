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

#include <Schema_Euler_Implicite.h>
#include <Op_Diff_negligeable.h>
#include <Masse_PolyMAC_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Operateur.h>

Implemente_base(Masse_PolyMAC_base, "Masse_PolyMAC_base", Solveur_Masse_base);
Sortie& Masse_PolyMAC_base::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }
Entree& Masse_PolyMAC_base::readOn(Entree& s) { return s; }

Implemente_base(Masse_PolyMAC_P0P1NC_base, "Masse_PolyMAC_P0P1NC_base", Masse_PolyMAC_base);
Sortie& Masse_PolyMAC_P0P1NC_base::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }
Entree& Masse_PolyMAC_P0P1NC_base::readOn(Entree& s) { return s ; }

void Masse_PolyMAC_base::associer_domaine_dis_base(const Domaine_dis_base& le_dom_dis_base)
{
  le_dom_PolyMAC = ref_cast(Domaine_PolyMAC, le_dom_dis_base);
}

void Masse_PolyMAC_base::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
{
  le_dom_Cl_PolyMAC = ref_cast(Domaine_Cl_PolyMAC, le_dom_Cl_dis_base);
}

void Masse_PolyMAC_base::completer()
{
  if (!sub_type(Schema_Implicite_base, equation().schema_temps()))
    {
      Cerr << "===================================================================================" << finl;
      Cerr << "Error when using " << equation().schema_temps().que_suis_je() << " scheme:" << finl;
      Cerr << "You can only use implicit schemes with the PolyMAC discretization (for mass solver)." << finl;
      Process::exit();
    }
  no_diff_ = true;

  for (int i = 0; i < equation().nombre_d_operateurs(); i++)
    if (sub_type(Operateur_Diff_base, equation().operateur(i).l_op_base()))
      if (!sub_type(Op_Diff_negligeable, equation().operateur(i).l_op_base()))
        no_diff_ = false;
}

void Masse_PolyMAC_base::appliquer_coef(DoubleVect& coef) const
{
  if (has_coefficient_temporel_)
    {
      REF(Champ_base) ref_coeff;
      ref_coeff = equation().get_champ(name_of_coefficient_temporel_);

      DoubleTab values;
      if (sub_type(Champ_Inc_base,ref_coeff.valeur()))
        {
          const Champ_Inc_base& coeff = ref_cast(Champ_Inc_base,ref_coeff.valeur());
          ConstDoubleTab_parts val_parts(coeff.valeurs());
          values.ref(val_parts[0]);

        }
      else if (sub_type(Champ_Fonc_base,ref_coeff.valeur()))
        {
          const Champ_Fonc_base& coeff = ref_cast(Champ_Fonc_base,ref_coeff.valeur());
          values.ref(coeff.valeurs());

        }
      else if (sub_type(Champ_Don_base,ref_coeff.valeur()))
        {
          DoubleTab nodes;
          equation().inconnue().valeur().remplir_coord_noeuds(nodes);
          ref_coeff.valeur().valeur_aux(nodes,values);
        }
      tab_multiply_any_shape(coef, values, VECT_REAL_ITEMS);
    }
}
