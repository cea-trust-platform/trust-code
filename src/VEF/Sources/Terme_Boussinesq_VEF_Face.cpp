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

void Terme_Boussinesq_VEF_Face::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis.valeur());
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
}

DoubleTab& Terme_Boussinesq_VEF_Face::ajouter(DoubleTab& resu) const
{
  ArrOfDouble T0 = getScalaire0();
  if(equation_scalaire().que_suis_je()=="Convection_Diffusion_Temperature_sensibility")
    T0=0.;
  // Verifie la validite de T0:
  check();

  const DoubleTab& param = equation_scalaire().inconnue().valeurs();
  int nbcomp_param = param.line_size(), nbcomp = resu.line_size();
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const double* Scalaire0_addr = mapToDevice(Scalaire0_);
  const double* beta_addr = mapToDevice(beta().valeur().valeurs());
  const int beta_dimension0 = beta().valeur().valeurs().dimension(0);
  const int beta_nb_dim = beta().valeur().valeurs().nb_dim();
  const double* g_addr = mapToDevice(gravite().valeurs());
  const double* xv_addr = mapToDevice(domaine_VEF.xv());
  const double* xp_addr = mapToDevice(domaine_VEF.xp());
  const double* porosite_surf_addr = mapToDevice(equation().milieu().porosite_face());
  const double* face_normales_addr = mapToDevice(domaine_VEF.face_normales());
  const double* param_addr = mapToDevice(param);
  const int* face_voisins_addr = mapToDevice(domaine_VEF.face_voisins());
  double* resu_addr = computeOnTheDevice(resu);
  // Boucle sur toutes les faces
  int nb_faces = domaine_VEF.nb_faces();
  start_gpu_timer();
  #pragma omp target teams distribute parallel for if (computeOnDevice)
  for (int face=0; face<nb_faces; face++)
    {
      int elem1 = face_voisins_addr[2*face], elem2 = face_voisins_addr[2*face+1];
      double delta_param = 0.0;
      for (int compo=0; compo<nbcomp_param; compo++)
        delta_param += valeur_addr(beta_addr,beta_dimension0,beta_nb_dim, elem1, elem2, compo, nbcomp_param)*(Scalaire0_addr[compo]-param_addr[face*nbcomp_param+compo]);

      for (int comp=0; comp<dimension; comp++)
        {
          double delta_coord;
          if (elem2==-1) // Face de bord
            delta_coord = xv_addr[face*dimension+comp] - xp_addr[elem1*dimension+comp];
          else
            delta_coord = xp_addr[elem2*dimension+comp] - xp_addr[elem1*dimension+comp];
          for (int compo=0; compo<nbcomp; compo++)
            resu_addr[face*nbcomp+compo] += delta_param*delta_coord*face_normales_addr[face*nbcomp+compo]*g_addr[comp]*porosite_surf_addr[face];
        }
    }
  end_gpu_timer(Objet_U::computeOnDevice, "Face loop in Terme_Boussinesq_VEF_Face::ajouter\n");
  return resu;
}

