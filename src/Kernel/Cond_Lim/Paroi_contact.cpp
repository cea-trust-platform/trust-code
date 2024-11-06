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

#include <Discretisation_base.h>
#include <Paroi_contact.h>
#include <Equation_base.h>
#include <Probleme_base.h>

Implemente_instanciable(Paroi_contact, "paroi_contact", Cond_lim_utilisateur_base);
// XD paroi_contact condlim_base paroi_contact -1 Thermal condition between two domains. Important: the name of the boundaries in the two domains should be the same. (Warning: there is also an old limitation not yet fixed on the sequential algorithm in VDF to detect the matching faces on the two boundaries: faces should be ordered in the same way). The kind of condition depends on the discretization. In VDF, it is a heat exchange condition, and in VEF, a temperature condition. NL2 Such a coupling requires coincident meshes for the moment. In case of non-coincident meshes, run is stopped and two external files are automatically generated in VEF (connectivity_failed_boundary_name and connectivity_failed_pb_name.med). In 2D, the keyword Decouper_bord_coincident associated to the connectivity_failed_boundary_name file allows to generate a new coincident mesh. NL2 In 3D, for a first preliminary cut domain with HOMARD (fluid for instance), the second problem associated to pb_name (solide in a fluid/solid coupling problem) has to be submitted to HOMARD cutting procedure with connectivity_failed_pb_name.med. NL2 Such a procedure works as while the primary refined mesh (fluid in our example) impacts the fluid/solid interface with a compact shape as described below (values 2 or 4 indicates the number of division from primary faces obtained in fluid domain at the interface after HOMARD cutting): NL2 2-2-2-2-2-2 NL2 2-4-4-4-4-4-2 \\; 2-2-2 NL2 2-4-4-4-4-2 \\; 2-4-2 NL2 2-2-2-2-2 \\; 2-2 NL2 OK NL2 NL2 2-2 \\; \\; 2-2-2 NL2 2-4-2 \\; 2-2 NL2 2-2 \\; 2-2 NL2 NOT OK
// XD attr autrepb ref_Pb_base autrepb 0 Name of other problem.
// XD attr nameb chaine nameb 0 boundary name of the remote problem which should be the same than the local name

Sortie& Paroi_contact::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord;
}

Entree& Paroi_contact::readOn(Entree& s)
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  return s;
}

void Paroi_contact::complement(Nom& ajout)
{
  const Nom& nom_mon_pb = mon_equation->probleme().le_nom();
  if (nom_mon_pb == nom_autre_pb)
    {
      Cerr << "Error in " << que_suis_je() << " the name of the other problem is the same as my problem :" << nom_mon_pb << " on the boundary :" << nom_bord_ << finl;
      Process::exit();
    }

  if (mon_equation->discretisation().is_vdf() || mon_equation->discretisation().is_polymac_family())
    {
      if (mon_equation->discretisation().is_vdf())
        ajout = "paroi_echange_contact_VDF ";
      else
        ajout = mon_equation->discretisation().is_polymac_p0p1nc() ? "paroi_echange_contact_PolyMAC_P0P1NC " :
                mon_equation->discretisation().is_polymac_p0() ? "paroi_echange_contact_PolyMAC_P0 " : "paroi_echange_contact_PolyMAC ";

      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord + " ";
      ajout += mon_equation->inconnue().le_nom() + " 1.e10";
    }
  else
    {
      ajout = "scalaire_impose_paroi ";
      ajout += "Champ_Front_contact_VEF ";
      ajout += nom_mon_pb;
      ajout += " ";
      ajout += nom_bord_;
      ajout += " ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
    }
}
