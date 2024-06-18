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

#include <Echange_contact_PolyMAC_P0.h>
#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Schema_Euler_Implicite.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Domaine_PolyMAC_P0.h>
#include <Champ_front_calc.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Operateur.h>
#include <Front_VF.h>
#include <Synonyme_info.h>

Implemente_instanciable(Echange_contact_PolyMAC_P0, "Paroi_Echange_contact_PolyMAC_P0|Paroi_Echange_contact_PolyVEF_P0", Echange_contact_PolyMAC_P0P1NC);
Add_synonym(Echange_contact_PolyMAC_P0, "Paroi_Echange_contact_PolyVEF_P0P1");
using namespace MEDCoupling;

Sortie& Echange_contact_PolyMAC_P0::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Echange_contact_PolyMAC_P0::readOn(Entree& s) { return Echange_contact_PolyMAC_P0P1NC::readOn(s); }

void Echange_contact_PolyMAC_P0::init_op() const
{
  if (o_diff.non_nul())
    return;
  Champ_front_calc ch;
  ch.creer(nom_autre_pb_, nom_bord_, nom_champ_);
  fvf = ref_cast(Front_VF, frontiere_dis()), o_fvf = ref_cast(Front_VF, ch.front_dis()); //frontieres
  const Equation_base& eqn = domaine_Cl_dis().equation(), &o_eqn = ch.equation(); //equations
  i_fvf = eqn.domaine_dis().rang_frontiere(fvf->le_nom()), i_o_fvf = o_eqn.domaine_dis().rang_frontiere(nom_bord_);

  int i_op = -1, o_i_op = -1, i; //indice de l'operateur de diffusion dans l'autre equation
  for (i = 0; i < eqn.nombre_d_operateurs(); i++)
    if (sub_type(Op_Diff_PolyMAC_P0_Elem, eqn.operateur(i).l_op_base()))
      i_op = i;
  if (i_op < 0)
    Process::exit(le_nom() + " : no diffusion operator found in " + eqn.probleme().le_nom() + " !");
  for (i = 0; i < o_eqn.nombre_d_operateurs(); i++)
    if (sub_type(Op_Diff_PolyMAC_P0_Elem, o_eqn.operateur(i).l_op_base()))
      o_i_op = i;
  if (o_i_op < 0)
    Process::exit(le_nom() + " : no diffusion operator found in " + o_eqn.probleme().le_nom() + " !");
  diff = ref_cast(Op_Diff_PolyMAC_P0_Elem, eqn.operateur(i_op).l_op_base());
  o_diff = ref_cast(Op_Diff_PolyMAC_P0_Elem, o_eqn.operateur(o_i_op).l_op_base());

}
