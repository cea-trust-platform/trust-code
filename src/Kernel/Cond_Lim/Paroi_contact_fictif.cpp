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

#include <Paroi_contact_fictif.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>

Implemente_instanciable(Paroi_contact_fictif, "paroi_contact_fictif", Cond_lim_utilisateur_base);
// XD paroi_contact_fictif condlim_base paroi_contact_fictif -1 This keyword is derivated from paroi_contact and is especially dedicated to compute coupled fluid/solid/fluid problem in case of thin material. Thanks to this option, solid is considered as a fictitious media (no mesh, no domain associated), and coupling is performed by considering instantaneous thermal equilibrium in it (for the moment).
// XD attr autrepb ref_Pb_base autrepb 0 Name of other problem.
// XD attr nameb chaine nameb 0 Name of bord.
// XD attr conduct_fictif floattant conduct_fictif 0 thermal conductivity
// XD attr ep_fictive floattant ep_fictive 0 thickness of the fictitious media

Sortie& Paroi_contact_fictif::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord << " " << conduct_fictif << " " << ep_fictif;
}

Entree& Paroi_contact_fictif::readOn(Entree& s)
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  s >> conduct_fictif;
  s >> ep_fictif;
  return s;
}

void Paroi_contact_fictif::complement(Nom& ajout)
{
  const Nom& nom_mon_pb = mon_equation->probleme().le_nom();
  if (mon_equation->discretisation().is_vdf())
    {
      ajout = "paroi_echange_contact_VDF ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else if (mon_equation->discretisation().is_polymac())
    {
      ajout = "paroi_echange_contact_PolyMAC ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else if (mon_equation->discretisation().is_polymac_p0p1nc())
    {
      ajout = "paroi_echange_contact_PolyMAC_P0P1NC ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else if (mon_equation->discretisation().is_polymac_p0())
    {
      ajout = "paroi_echange_contact_PolyMAC_P0 ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else
    {
      ajout = "temperature_imposee_paroi ";
      ajout += "Champ_Front_contact_fictif_VEF ";
      ajout += nom_mon_pb;
      ajout += " ";
      ajout += nom_bord_;
      ajout += " ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " ";
      ajout += Nom(conduct_fictif, "%e");
      ajout += " ";
      ajout += Nom(ep_fictif, "%e");
    }
}
