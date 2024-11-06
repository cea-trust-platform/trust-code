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

#include <Paroi_contact_fictif_rayo.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>

Implemente_instanciable(Paroi_contact_fictif_rayo, "paroi_contact_fictif_rayo", Cond_lim_utilisateur_base);

Sortie& Paroi_contact_fictif_rayo::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord << " " << conduct_fictif << " " << ep_fictif << " " << type_rayo;
}

Entree& Paroi_contact_fictif_rayo::readOn(Entree& s)
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  s >> conduct_fictif;
  s >> ep_fictif;
  s >> type_rayo; /* pour l'autre probleme */

  if (type_rayo != "TRANSP" && type_rayo != "SEMI_TRANSP")
    {
      Cerr << "type_rayo should be TRANSP or SEMI_TRANSP and not " << type_rayo << finl;
      Process::exit();
    }

  return s;
}

void Paroi_contact_fictif_rayo::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  if (!rayo)
    {
      if (type_rayo == "TRANSP")
        rayo = 2;
      else if (type_rayo == "SEMI_TRANSP")
        rayo = 1;
      else
        throw;
    }

  if (mon_equation->discretisation().is_vdf())
    {
      if (rayo == 2)
        ajout = "Echange_contact_Rayo_transp_VDF ";
      if (rayo == 1)
        ajout = "Paroi_Echange_contact_rayo_semi_transp_VDF ";

      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else
    {
      Cerr << que_suis_je() << "coded only in non-radiating in VEF/PolyMAC_P0P1NC" << finl;
      Process::exit();
    }
}

