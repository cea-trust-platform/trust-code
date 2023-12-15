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
#include <Linear_algebra_tools_impl.h>
#include <Champ_implementation_P1.h>
#include <Frottement_impose_base.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Dirichlet_homogene.h>
#include <Domaine_PolyVEF_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Option_PolyVEF_P0.h>
#include <MD_Vector_tools.h>
#include <TRUSTTab_parts.h>
#include <Comm_Group_MPI.h>
#include <Quadrangle_VEF.h>
#include <communications.h>
#include <Statistiques.h>
#include <Hexaedre_VEF.h>
#include <Matrix_tools.h>
#include <unordered_map>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Tetraedre.h>
#include <Rectangle.h>
#include <PE_Groups.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <LireMED.h>
#include <unistd.h>
#include <Lapack.h>
#include <numeric>
#include <vector>
#include <cfloat>
#include <tuple>
#include <cmath>
#include <cfenv>
#include <set>
#include <map>

Implemente_instanciable(Domaine_PolyVEF_P0, "Domaine_PolyVEF_P0", Domaine_PolyMAC_P0);

Sortie& Domaine_PolyVEF_P0::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_PolyVEF_P0::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_PolyVEF_P0::discretiser()
{
  /* on saut le discretiser() de Domaine_PolyMAC_P0P1NC pour eviter d'initialiser les variables de PolyMAC_P0P1NC_V1 */
  Domaine_Poly_base::discretiser();
  /* comme on a le vecteur complet aux faces, les volumes entrelaces sont divises par D */
  volumes_entrelaces_ /= dimension, volumes_entrelaces_dir_ /= dimension;

  //MD_vector pour la pression (elements + faces de bord)
  MD_Vector_composite mdc_elems_fbord;
  mdc_elems_fbord.add_part(domaine().md_vector_elements());
  mdc_elems_fbord.add_part(md_vector_faces_bord());
  mdv_elems_fbord.copy(mdc_elems_fbord);
}
