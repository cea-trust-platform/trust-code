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

#include <MD_Vector_composite.h>
#include <Domaine_PolyVEF.h>

Implemente_base(Domaine_PolyVEF, "Domaine_PolyVEF", Domaine_PolyMAC_P0);
Implemente_instanciable(Domaine_PolyVEF_P0, "Domaine_PolyVEF_P0", Domaine_PolyVEF);
Implemente_instanciable(Domaine_PolyVEF_P0P1, "Domaine_PolyVEF_P0P1", Domaine_PolyVEF);
Implemente_instanciable(Domaine_PolyVEF_P0P1NC, "Domaine_PolyVEF_P0P1NC", Domaine_PolyVEF);

Entree& Domaine_PolyVEF::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }
Entree& Domaine_PolyVEF_P0::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }
Entree& Domaine_PolyVEF_P0P1::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }
Entree& Domaine_PolyVEF_P0P1NC::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

Sortie& Domaine_PolyVEF::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }
Sortie& Domaine_PolyVEF_P0::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }
Sortie& Domaine_PolyVEF_P0P1::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }
Sortie& Domaine_PolyVEF_P0P1NC::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }


void Domaine_PolyVEF::discretiser()
{
  /* on saut le discretiser() de Domaine_PolyMAC_P0P1NC pour eviter d'initialiser les variables de PolyMAC_P0P1NC_V1 */
  Domaine_Poly_base::discretiser();
  /* comme on a le vecteur complet aux faces, les volumes entrelaces sont divises par D */
  volumes_entrelaces_ /= dimension, volumes_entrelaces_dir_ /= dimension;
}

void Domaine_PolyVEF_P0::discretiser()
{
  Domaine_PolyVEF::discretiser();
  //MD_vector pour la pression (elements + faces de bord)
  MD_Vector_composite mdc_elems_fbord;
  mdc_elems_fbord.add_part(domaine().md_vector_elements());
  mdc_elems_fbord.add_part(md_vector_faces_bord());
  mdv_elems_fbord.copy(mdc_elems_fbord);
}

void Domaine_PolyVEF_P0P1::discretiser()
{
  Domaine_PolyVEF::discretiser();
  //MD_vector pour la pression (elements + sommets)
  MD_Vector_composite mdc_elems_soms;
  mdc_elems_soms.add_part(domaine().md_vector_elements());
  mdc_elems_soms.add_part(domaine().md_vector_sommets());
  mdv_elems_soms.copy(mdc_elems_soms);
}

void Domaine_PolyVEF_P0P1NC::discretiser()
{
  Domaine_PolyVEF::discretiser();
  //MD_vector pour la pression (elements + faces)
  MD_Vector_composite mdc_elems_faces;
  mdc_elems_faces.add_part(domaine().md_vector_elements());
  mdc_elems_faces.add_part(md_vector_faces());
  mdv_elems_faces.copy(mdc_elems_faces);
}
