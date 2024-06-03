/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Interpolation_IBM_power_law_tbl_u_star.h>
#include <TRUSTTrav.h>

#include <Domaine.h>
#include <Param.h>

Implemente_instanciable( Interpolation_IBM_power_law_tbl_u_star, "Interpolation_IBM_power_law_tbl_u_star|IBM_power_law_tbl_u_star", Interpolation_IBM_mean_gradient ) ;
// XD Interpolation_IBM_power_law_tbl_u_star interpolation_ibm_mean_gradient Interpolation_IBM_power_law_tbl_u_star 1 Immersed Boundary Method (IBM): law u star.

Sortie& Interpolation_IBM_power_law_tbl_u_star::printOn( Sortie& os ) const
{
  Objet_U::printOn( os );
  return os;
}

Entree& Interpolation_IBM_power_law_tbl_u_star::readOn( Entree& is )
{
  Param param(que_suis_je());
  Interpolation_IBM_base::set_param(param);
  param.ajouter("points_solides",&solid_points_lu_,Param::REQUIRED);  // XD_ADD_P field_base Node field giving the projection of the node on the immersed boundary
  param.ajouter("est_dirichlet",&is_dirichlet_lu_,Param::REQUIRED);   // XD_ADD_P field_base Node field of booleans indicating whether the node belong to an element where the interface is
  param.ajouter("correspondance_elements",&corresp_elems_lu_,Param::REQUIRED); // XD_ADD_P field_base Cell field giving the SALOME cell number
  param.ajouter("elements_solides",&solid_elems_lu_,Param::REQUIRED); // XD_ADD_P field_base Node field giving the element number containing the solid point
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Interpolation_IBM_power_law_tbl_u_star::discretise(const Discretisation_base& dis, Domaine_dis_base& le_dom_EF)
{
  int nb_comp = Objet_U::dimension;
  Noms units(nb_comp);
  Noms c_nam(nb_comp);

  if ((&corresp_elems_lu_)->non_nul())
    {
      dis.discretiser_champ("champ_elem",le_dom_EF,"corresp_elems","none",1,0., corresp_elems_);
      corresp_elems_.valeur().affecter(corresp_elems_lu_);
    }
  dis.discretiser_champ("champ_sommets",le_dom_EF,"solid_elems","none",1,0., solid_elems_);
  solid_elems_.valeur().affecter(solid_elems_lu_);
  dis.discretiser_champ("champ_sommets",le_dom_EF,"is_dirichlet","none",1,0., is_dirichlet_);
  is_dirichlet_.valeur().affecter(is_dirichlet_lu_);
  dis.discretiser_champ("vitesse",le_dom_EF,vectoriel,c_nam,units,nb_comp,0., solid_points_);
  solid_points_.valeur().affecter(solid_points_lu_);
  computeSommetsVoisins(le_dom_EF, solid_points_, corresp_elems_);
}
