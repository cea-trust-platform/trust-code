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


#include <Champ_Elem_PolyMAC_P0.h>
#include <Zone_Cl_dis.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC_P0.h>
#include <Domaine.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_contact_PolyMAC_P0.h>
#include <Connectivite_som_elem.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <array>
#include <cmath>

Implemente_instanciable(Champ_Elem_PolyMAC_P0,"Champ_Elem_PolyMAC_P0",Champ_Elem_PolyMAC);

// printOn

Sortie& Champ_Elem_PolyMAC_P0::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// readOn

Entree& Champ_Elem_PolyMAC_P0::readOn(Entree& s)
{
  lire_donnees(s) ;
  return s ;
}

// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: la_zone_PolyMAC_P0_P0.valeur()
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Zone_PolyMAC_P0& Champ_Elem_PolyMAC_P0::zone_PolyMAC_P0() const
{
  return ref_cast(Zone_PolyMAC_P0, la_zone_VF.valeur());
}

inline void Champ_Elem_PolyMAC_P0::mettre_a_jour(double tps)
{
  if (temps()!=tps) grad_a_jour = 0 ;
  Champ_Inc_P0_base::mettre_a_jour(tps);
}

void Champ_Elem_PolyMAC_P0::init_grad(int full_stencil) const
{
  if (fgrad_d.size()) return;
  const IntTab&             f_cl = fcl();
  const Zone_PolyMAC_P0&       zone = ref_cast(Zone_PolyMAC_P0, la_zone_VF.valeur());
  const Conds_lim&           cls = zone_Cl_dis().les_conditions_limites(); // CAL du champ à dériver
  zone.fgrad(1, 0, cls, f_cl, NULL, NULL, 1, full_stencil, fgrad_d, fgrad_e, fgrad_w);
}

void Champ_Elem_PolyMAC_P0::calc_grad(int full_stencil) const
{
  if (grad_a_jour) return;
  const IntTab&             f_cl = fcl();
  const Zone_PolyMAC_P0&       zone = ref_cast(Zone_PolyMAC_P0, la_zone_VF.valeur());
  const Conds_lim&           cls = zone_Cl_dis().les_conditions_limites(); // CAL du champ à dériver
  zone.fgrad(1, 0, cls, f_cl, NULL, NULL, 1, full_stencil, fgrad_d, fgrad_e, fgrad_w);
  grad_a_jour = 1;
}
