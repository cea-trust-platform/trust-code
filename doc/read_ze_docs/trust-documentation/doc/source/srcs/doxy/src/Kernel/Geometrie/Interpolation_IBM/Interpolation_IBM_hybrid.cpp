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

#include <Interpolation_IBM_hybrid.h>
#include <TRUSTTab.h>
#include <Param.h>


Implemente_instanciable( Interpolation_IBM_hybrid, "Interpolation_IBM_hybride|IBM_hybride", Interpolation_IBM_elem_fluid ) ;
// XD interpolation_ibm_hybride interpolation_ibm_elem_fluid ibm_hybride 1 Immersed Boundary Method (IBM): hybrid (fluid/mean gradient) interpolation.

Sortie& Interpolation_IBM_hybrid::printOn( Sortie& os ) const
{
  Objet_U::printOn( os );
  return os;
}

Entree& Interpolation_IBM_hybrid::readOn( Entree& is )
{
  //Interpolation_IBM_elem_fluid::readOn(is);
  Param param(que_suis_je());
  Interpolation_IBM_elem_fluid::set_param(param);
  param.ajouter("est_dirichlet",&is_dirichlet_lu_,Param::REQUIRED);   // XD_ADD_P field_base Node field of booleans indicating whether the node belong to an element where the interface is
  param.ajouter("elements_solides",&solid_elems_lu_,Param::REQUIRED); // XD_ADD_P field_base Node field giving the element number containing the solid point
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Interpolation_IBM_hybrid::discretise(const Discretisation_base& dis, Domaine_dis_base& le_dom_EF)
{
  Cerr << "(IBM) Warning! Interpolation IBM_hybrid has no validation test case." << finl;

  Interpolation_IBM_elem_fluid::discretise(dis,le_dom_EF);

  dis.discretiser_champ("champ_sommets",le_dom_EF,"solid_elems","none",1,0., solid_elems_);
  solid_elems_.valeur().affecter(solid_elems_lu_);
  dis.discretiser_champ("champ_sommets",le_dom_EF,"is_dirichlet","none",1,0., is_dirichlet_);
  is_dirichlet_.valeur().affecter(is_dirichlet_lu_);

  computeSommetsVoisins(le_dom_EF, solid_points_, corresp_elems_);
}

