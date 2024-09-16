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

#include <Iterateur_PolyMAC_base.h>

Implemente_base(Iterateur_PolyMAC_base, "Iterateur_PolyMAC_base", Objet_U);

Entree& Iterateur_PolyMAC_base::readOn(Entree& s)
{
  return s;
}

Sortie& Iterateur_PolyMAC_base::printOn(Sortie& s) const
{
  return s << que_suis_je();
}

void Iterateur_PolyMAC_base::calculer_flux_bord(const DoubleTab& inco) const
{
  Cerr << que_suis_je() << " must implement calculer_flux_bord" << finl;
  Process::exit();
}

void Iterateur_PolyMAC_base::associer(const Domaine_dis_base& z, const Domaine_Cl_dis_base& zcl, const Operateur_base& op)
{
  const Domaine_PolyMAC& domaine_vdf = ref_cast(Domaine_PolyMAC, z);
  const Domaine_Cl_PolyMAC& domaine_cl_vdf = ref_cast(Domaine_Cl_PolyMAC, zcl);
  associer(domaine_vdf, domaine_cl_vdf, op);
}
void Iterateur_PolyMAC_base::associer_domaine_cl_dis(const Domaine_Cl_dis_base& zcl)
{
  const Domaine_Cl_PolyMAC& domaine_cl_vdf = ref_cast(Domaine_Cl_PolyMAC, zcl);
  la_zcl = domaine_cl_vdf;

}
void Iterateur_PolyMAC_base::associer(const Domaine_PolyMAC& domaine_vdf, const Domaine_Cl_PolyMAC& domaine_cl_vdf, const Operateur_base& op)
{
  le_domaine = domaine_vdf;
  la_zcl = domaine_cl_vdf;
  op_base = op;
}
