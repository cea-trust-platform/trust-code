/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Terme_Boussinesq_VEF_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Domaine_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <Navier_Stokes_std.h>
#include <Synonyme_info.h>
#include <Device.h>

Implemente_instanciable(Terme_Boussinesq_VEF_Face,"Boussinesq_VEF_P1NC",Terme_Boussinesq_base);
Add_synonym(Terme_Boussinesq_VEF_Face,"Boussinesq_temperature_VEF_Face");
Add_synonym(Terme_Boussinesq_VEF_Face,"Boussinesq_concentration_VEF_Face");

//// printOn
Sortie& Terme_Boussinesq_VEF_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_VEF_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

void Terme_Boussinesq_VEF_Face::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis);
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis);
}

DoubleTab& Terme_Boussinesq_VEF_Face::ajouter(DoubleTab& tab_resu) const
{
  ArrOfDouble T0 = getScalaire0();
  if(equation_scalaire().que_suis_je()=="Convection_Diffusion_Temperature_sensibility")
    T0=0.;
  // Verifie la validite de T0:
  check();

  const DoubleTab& tab_param = equation_scalaire().inconnue().valeurs();
  int nbcomp_param = tab_param.line_size(), nbcomp = tab_resu.line_size();
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();

  CDoubleArrView scalaire0 = Scalaire0_.view_ro();
  CDoubleTabView beta_v = beta().valeurs().view_ro();
  CDoubleArrView g = static_cast<const DoubleVect&>(gravite().valeurs()).view_ro();
  CDoubleTabView xv = domaine_VEF.xv().view_ro();
  CDoubleTabView xp = domaine_VEF.xp().view_ro();
  CDoubleArrView porosite_surf = equation().milieu().porosite_face().view_ro();
  CDoubleTabView face_normales = domaine_VEF.face_normales().view_ro();
  CDoubleTabView param = tab_param.view_ro();
  CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
  DoubleTabView resu = tab_resu.view_rw();

  // Boucle sur toutes les faces
  int nb_faces = domaine_VEF.nb_faces();
  int dim = Objet_U::dimension;
  const int beta_dimension0 = beta().valeurs().dimension(0);
  const int beta_nb_dim = beta().valeurs().nb_dim();

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       nb_faces,
                       KOKKOS_LAMBDA (int face)
  {
    int elem1 = face_voisins(face,0);
    int elem2 = face_voisins(face,1);

    double delta_param = 0.0;
    for (int compo = 0; compo < nbcomp_param; compo++)
      delta_param += valeur(beta_v,beta_dimension0,beta_nb_dim,elem1,elem2,compo,nbcomp_param)*(scalaire0(compo)-param(face,compo));

    for (int comp = 0; comp < dim; comp++)
      {
        double delta_coord;
        if (elem2 == -1) // Face de bord
          delta_coord = xv(face,comp) - xp(elem1,comp);
        else
          delta_coord = xp(elem2,comp) - xp(elem1,comp);

        for (int compo = 0; compo < nbcomp; compo++)
          resu(face,compo) += delta_param*delta_coord*face_normales(face,compo)*g(comp)*porosite_surf(face);
      }
  });
  end_gpu_timer(__KERNEL_NAME__);

  return tab_resu;
}

