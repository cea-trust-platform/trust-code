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

#include <Source_Generique_Face_PolyVEF_P0.h>
#include <Domaine_PolyVEF_P0.h>
#include <Equation_base.h>
#include <Milieu_base.h>

Implemente_instanciable(Source_Generique_Face_PolyVEF_P0, "Source_Generique_Face_PolyVEF_P0", Source_Generique_Face_PolyMAC_P0P1NC);

Sortie& Source_Generique_Face_PolyVEF_P0::printOn(Sortie& os) const { return os << que_suis_je(); }

Entree& Source_Generique_Face_PolyVEF_P0::readOn(Entree& is) { return Source_Generique_base::readOn(is); }

DoubleTab& Source_Generique_Face_PolyVEF_P0::ajouter(DoubleTab& resu) const
{
  OWN_PTR(Champ_base) espace_stockage;
  const Champ_base& la_source = ch_source_->get_champ(espace_stockage); // Aux faces
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  for (int f = 0; f < domaine.nb_faces(); f++)
    resu(f) += pf(f) * vf(f) * la_source.valeurs()(f);
  return resu;
}
