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

#include <Echange_contact_VDF.h>
#include <Op_Diff_VDF_base.h>
#include <Champ_front_calc.h>
#include <Eval_Diff_VDF.h>
#include <Pb_Multiphase.h>
#include <TRUSTTrav.h>
#include <Champ_Don.h>
#include <Operateur.h>
#include <Motcle.h>

Implemente_base(Op_Diff_VDF_base, "Op_Diff_VDF_base", Operateur_Diff_base);

Sortie& Op_Diff_VDF_base::printOn(Sortie& s) const { return s << que_suis_je(); }
Entree& Op_Diff_VDF_base::readOn(Entree& s) { return s; }

void Op_Diff_VDF_base::completer()
{
  Operateur_base::completer();
  // Certains operateurs (Axi) n'ont pas d'iterateurs en VDF... Encore une anomalie dans la conception a corriger un jour !
  if (iter.non_nul())
    {
      iter->completer_();
      iter->associer_champ_convecte_ou_inc(equation().inconnue(), nullptr);
      iter->set_convective_op_pb_type(false /* diff op */, sub_type(Pb_Multiphase, equation().probleme()));
    }
}

int Op_Diff_VDF_base::impr(Sortie& os) const
{
  // Certains operateurs (Axi) n'ont pas d'iterateurs en VDF... Encore une anomalie dans la conception a corriger un jour !
  return (iter.non_nul()) ? iter->impr(os) : 0;
}

/*! @brief calcule la contribution de la diffusion, la range dans resu
 *
 */
DoubleTab& Op_Diff_VDF_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(inco, resu);
}

void Op_Diff_VDF_base::init_op_ext() const
{
  const Zone_VDF& zvdf = iter->zone();
  const Zone_Cl_VDF& zclvdf = iter->zone_Cl();
  op_ext = { this };      //le premier op_ext est l'operateur local

  for (int n_bord = 0; n_bord < zvdf.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      if (sub_type(Echange_contact_VDF, la_cl.valeur()))
        {
          const Echange_contact_VDF& cl = ref_cast(Echange_contact_VDF, la_cl.valeur());
          const Champ_front_calc& ch = ref_cast(Champ_front_calc, cl.T_autre_pb().valeur());
          const Equation_base& o_eqn = ch.equation();
          const Op_Diff_VDF_base *o_diff = &ref_cast(Op_Diff_VDF_base, o_eqn.operateur(0).l_op_base());

          if (std::find(op_ext.begin(), op_ext.end(), o_diff) == op_ext.end())
            op_ext.push_back(o_diff);
        }
    }
  op_ext_init_ = 1;
}
