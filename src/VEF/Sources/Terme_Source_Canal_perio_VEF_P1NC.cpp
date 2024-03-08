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

#include <Terme_Source_Canal_perio_VEF_P1NC.h>
#include <Domaine_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <Periodique.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Fluide_Dilatable_base.h>
#include <communications.h>
#include <SFichier.h>
#include <Convection_Diffusion_std.h>

// Mai 04 : modif pour terme source en QC
//
Implemente_instanciable(Terme_Source_Canal_perio_VEF_P1NC,"Canal_perio_VEF_P1NC",Terme_Source_Canal_perio);
Implemente_instanciable(Terme_Source_Canal_perio_QC_VEF_P1NC,"Canal_perio_QC_VEF_P1NC",Terme_Source_Canal_perio_VEF_P1NC);


//// printOn
//
Sortie& Terme_Source_Canal_perio_QC_VEF_P1NC::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//
//   //// readOn
//   //

Entree& Terme_Source_Canal_perio_QC_VEF_P1NC::readOn(Entree& s )
{
  return Terme_Source_Canal_perio::readOn(s);
}

//// printOn
//

Sortie& Terme_Source_Canal_perio_VEF_P1NC::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//
//// readOn
//

Entree& Terme_Source_Canal_perio_VEF_P1NC::readOn(Entree& s )
{
  return Terme_Source_Canal_perio::readOn(s);
}

void Terme_Source_Canal_perio_VEF_P1NC::associer_domaines(const Domaine_dis& domaine_dis,
                                                          const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis.valeur());
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
}

DoubleTab& Terme_Source_Canal_perio_VEF_P1NC::ajouter(DoubleTab& resu) const
{
  const Domaine_VF& domaine_VF = le_dom_VEF.valeur();
  const DoubleVect& volumes_entrelaces = domaine_VF.volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = le_dom_Cl_VEF.valeur().volumes_entrelaces_Cl();
  int premiere_face_std=le_dom_VEF.valeur().premiere_face_std() ;
  const DoubleVect& porosite_face=equation().milieu().porosite_face();
  ArrOfDouble s;
  s = source();
  int nb_faces = domaine_VF.nb_faces();
  if (s.size_array()==dimension)
    {
      // Case Navier Stokes, s is uniform
      const double* s_addr = mapToDevice(s);
      const double* volumes_entrelaces_Cl_addr = mapToDevice(volumes_entrelaces_Cl);
      const double* volumes_entrelaces_addr = mapToDevice(volumes_entrelaces);
      const double* porosite_face_addr = mapToDevice(porosite_face);
      double* resu_addr = computeOnTheDevice(resu);
      start_gpu_timer();
      #pragma omp target teams distribute parallel for if (computeOnDevice)
      for (int num_face = 0; num_face<nb_faces; num_face++)
        {
          double vol = (num_face<premiere_face_std ? volumes_entrelaces_Cl_addr[num_face] : volumes_entrelaces_addr[num_face]) * porosite_face_addr[num_face];
          for (int i=0; i<dimension; i++)
            resu_addr[num_face*dimension+i]+= s_addr[i]*vol;
        }
      end_gpu_timer(Objet_U::computeOnDevice, "Face loop in Terme_Source_Canal_perio_VEF_P1NC::ajouter");
    }
  else
    {
      // Case Energy, s is non uniform (ToDo OpenMP mais rarement utilise)
      bilan_=0;
      const ArrOfInt& fd=domaine_VF.faces_doubles();
      for (int num_face = 0; num_face<nb_faces; num_face++)
        {
          double vol = (num_face < premiere_face_std ? volumes_entrelaces_Cl(num_face) : volumes_entrelaces(num_face))*porosite_face(num_face);
          double contrib = s[num_face]*vol;
          resu(num_face)+= contrib;
          bilan_(0)+= contrib*(1-0.5*fd[num_face]);
        }
    }
  return resu;
}

void Terme_Source_Canal_perio_VEF_P1NC::calculer_debit(double& debit_e) const
{
  const Domaine_VF& domaine_VF = le_dom_VEF.valeur();
  const Domaine_Cl_dis_base& domaine_Cl_dis = le_dom_Cl_VEF.valeur();
  const Champ_Inc_base& velocity = (sub_type(Convection_Diffusion_std,equation()) ? ref_cast(Convection_Diffusion_std,equation()).vitesse_transportante() : equation().inconnue());
  const DoubleVect& porosite_face=equation().milieu().porosite_face();
  // Check we have really velocity:
  assert(velocity.le_nom()=="vitesse");
  const DoubleTab& vitesse = velocity.valeurs();
  int nb_bords = domaine_VF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_dis.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          if (bord_periodique_ == le_bord.le_nom())
            {
              debit_e=0.;
              //const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces()/2;
              int axe = direction_ecoulement_;
              if (equation().probleme().is_dilatable()==1)
                {
                  // Si l'on est en Quasi Compressible, il faut conserver
                  // le debit massique et non pas le debit volumique.
                  // C'est pour cela que dans le cas QC, on multiplie les vecteurs vitesse
                  // par la masse volumique discretisee aux faces pour que lorsqu'on integre sur la surface,
                  // on obtienne bien un debit massique et non pas un debit volumique.
                  const DoubleTab& tab_rho_face = ref_cast(Fluide_Dilatable_base,equation().milieu()).rho_discvit();

                  if (axe>=0)
                    for (int num_face=ndeb; num_face<nfin; num_face++)
                      {
                        double debit_face = porosite_face(num_face)*vitesse(num_face,axe) * std::fabs(domaine_VF.face_normales(num_face,axe));
                        debit_e += tab_rho_face[num_face] * debit_face;
                      }
                  else
                    for (int num_face=ndeb; num_face<nfin; num_face++)
                      {
                        double debit_face=0;
                        for (int i=0; i<dimension; i++)
                          debit_face += porosite_face(num_face)*vitesse(num_face,i) * domaine_VF.face_normales(num_face,i);
                        debit_e += tab_rho_face[num_face] * debit_face;
                      }
                }
              else
                {
                  if (axe>=0)
                    for (int num_face=ndeb; num_face<nfin; num_face++)
                      {
                        double debit_face = porosite_face(num_face)*vitesse(num_face,axe) * std::fabs(domaine_VF.face_normales(num_face,axe));
                        debit_e += debit_face;
                      }
                  else
                    for (int num_face=ndeb; num_face<nfin; num_face++)
                      {
                        double debit_face=0;
                        for (int i=0; i<dimension; i++)
                          debit_face += porosite_face(num_face)*vitesse(num_face,i) * domaine_VF.face_normales(num_face,i);
                        debit_e += debit_face;
                      }
                }
            }
        }
    }
  debit_e = mp_sum(debit_e);
}



