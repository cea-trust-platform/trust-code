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

void Terme_Source_Canal_perio_VEF_P1NC::associer_domaines(const Domaine_dis_base& domaine_dis,
                                                          const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis);
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis);
}

DoubleTab& Terme_Source_Canal_perio_VEF_P1NC::ajouter(DoubleTab& tab_resu) const
{
  const Domaine_VF& domaine_VF = le_dom_VEF.valeur();
  int premiere_face_std = le_dom_VEF->premiere_face_std() ;
  ArrOfDouble tab_s;
  tab_s = source();
  int nb_faces = domaine_VF.nb_faces();
  int dim = Objet_U::dimension;
  CDoubleArrView volumes_entrelaces_Cl = le_dom_Cl_VEF->volumes_entrelaces_Cl().view_ro();
  CDoubleArrView volumes_entrelaces = domaine_VF.volumes_entrelaces().view_ro();
  CDoubleArrView porosite_face = equation().milieu().porosite_face().view_ro();
  CDoubleArrView s = tab_s.view_ro();
  if (tab_s.size_array()==dim)
    {
      // Case Navier Stokes, s is uniform
      DoubleTabView resu = tab_resu.view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces, KOKKOS_LAMBDA(
                             const int num_face)
      {
        double vol = (num_face<premiere_face_std ? volumes_entrelaces_Cl(num_face) : volumes_entrelaces(num_face))*porosite_face(num_face);
        for (int i=0; i<dim; i++)
          resu(num_face,i) += s(i)*vol;
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  else
    {
      // Case Energy, s is non uniform
      bilan_=0;
      CIntArrView fd = domaine_VF.faces_doubles().view_ro();
      DoubleArrView bilan = bilan_.view_rw();
      DoubleArrView resu = static_cast<DoubleVect&>(tab_resu).view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces, KOKKOS_LAMBDA(
                             const int num_face)
      {
        double vol = (num_face < premiere_face_std ? volumes_entrelaces_Cl(num_face) : volumes_entrelaces(num_face))*porosite_face(num_face);
        double contrib = s[num_face]*vol;
        resu(num_face)+= contrib;
        Kokkos::atomic_add(&bilan(0), contrib*(1-0.5*fd[num_face]));
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  return tab_resu;
}

void Terme_Source_Canal_perio_VEF_P1NC::calculer_debit(double& debit_e) const
{
  debit_e = 0.;
  const Domaine_VF& domaine_VF = le_dom_VEF.valeur();
  const Domaine_Cl_dis_base& domaine_Cl_dis = le_dom_Cl_VEF.valeur();
  int nb_bords = domaine_VF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_dis.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          if (bord_periodique_ == le_bord.le_nom())
            {
              const Champ_Inc_base& velocity = (sub_type(Convection_Diffusion_std,equation()) ? ref_cast(Convection_Diffusion_std,equation()).vitesse_transportante() : equation().inconnue());
              // Check we have really velocity:
              assert(velocity.le_nom()=="vitesse");
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces()/2;
              int axe = direction_ecoulement_;
              int dim = Objet_U::dimension;
              bool is_dilatable = equation().probleme().is_dilatable();
              // Si l'on est en Quasi Compressible, il faut conserver le debit massique et non pas le debit volumique.
              // C'est pour cela que dans le cas QC, on multiplie les vecteurs vitesse
              // par la masse volumique discretisee aux faces pour que lorsqu'on integre sur la surface,
              // on obtienne bien un debit massique et non pas un debit volumique.
              const DoubleTab& tab_rho_face = is_dilatable ? ref_cast(Fluide_Dilatable_base,equation().milieu()).rho_discvit() : velocity.valeurs() /* tableau de meme format mais non utilise */;
              if (getenv("TRUST_KOKKOS_REDUCTION_WEIRD"))
                {
                  ToDo_Kokkos("critical reduction mais ecarts en optimise pas en debug, pourquoi?");
                  CDoubleArrView rho_face = static_cast<const DoubleVect&>(tab_rho_face).view_ro();
                  CDoubleTabView face_normales = domaine_VF.face_normales().view_ro();
                  CDoubleArrView porosite_face = equation().milieu().porosite_face().view_ro();
                  CDoubleTabView vitesse = velocity.valeurs().view_ro();
                  Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                                          Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA(
                                            const int num_face, double& debit)
                  {
                    double debit_face = 0;
                    if (axe >= 0)
                      debit_face += porosite_face(num_face) * vitesse(num_face, axe) *
                                    std::fabs(face_normales(num_face, axe));
                    else
                      {
                        for (int i = 0; i < dim; i++)
                          debit_face += porosite_face(num_face) * vitesse(num_face, i) *
                                        face_normales(num_face, i);
                      }
                    debit += (is_dilatable ? rho_face(num_face) : 1) * debit_face;
                    //printf("Provisoire %d %f \n",num_face,debit_face);
                  }, Kokkos::Sum<double>(debit_e));
                  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
                  //Cerr << "Provisoire GPU debit_e=" << debit_e << finl;
                }
              else
                {
                  // PL: On utilise des copies partielles en attendant...
                  const DoubleTab& face_normales = domaine_VF.face_normales();
                  const DoubleVect& porosite_face = equation().milieu().porosite_face();
                  const DoubleTab& vitesse = velocity.valeurs();
                  copyPartialFromDevice(face_normales, ndeb*dimension, nfin*dimension, "face_normales on boundary");
                  copyPartialFromDevice(porosite_face, ndeb, nfin, "porosite_face on boundary");
                  copyPartialFromDevice(tab_rho_face, ndeb, nfin, "tab_rho_face on boundary");
                  copyPartialFromDevice(vitesse,       ndeb*dimension, nfin*dimension, "vitesse on boundary");
                  for (int num_face = ndeb; num_face < nfin; num_face++)
                    {
                      double debit_face = 0;
                      if (axe >= 0)
                        debit_face += porosite_face(num_face) * vitesse(num_face, axe) *
                                      std::fabs(face_normales(num_face, axe));
                      else
                        {
                          for (int i = 0; i < dim; i++)
                            debit_face += porosite_face(num_face) * vitesse(num_face, i) * face_normales(num_face, i);
                        }
                      debit_e += (is_dilatable ? tab_rho_face(num_face) : 1) * debit_face;
                    }
                  copyPartialToDevice(face_normales, ndeb*dimension, nfin*dimension, "face_normales on boundary");
                  copyPartialToDevice(porosite_face, ndeb, nfin, "porosite_face on boundary");
                  copyPartialToDevice(tab_rho_face, ndeb, nfin, "tab_rho_face on boundary");
                  copyPartialToDevice(vitesse,       ndeb*dimension, nfin*dimension, "vitesse on boundary");
                  //Cerr << "Provisoire CPU debit_e=" << debit_e << finl;
                }
            }
        }
    }
  debit_e = mp_sum(debit_e);
}



