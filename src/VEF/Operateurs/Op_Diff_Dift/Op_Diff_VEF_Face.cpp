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

#include <Op_Diff_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Champ_Uniforme.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Neumann_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Echange_externe_radiatif.h>
#include <Neumann_sortie_libre.h>
#include <Milieu_base.h>
#include <TRUSTTrav.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Porosites_champ.h>
#include <Device.h>
#include <Echange_couplage_thermique.h>
#include <Champ_front_calc_interne.h>

Implemente_instanciable_sans_constructeur(Op_Diff_VEF_Face,"Op_Diff_VEF_P1NC",Op_Diff_VEF_base);

Op_Diff_VEF_Face::Op_Diff_VEF_Face()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Diff_VEF_Face::readOn(Entree& s )
{
  return s ;
}

/*! @brief associe le champ de diffusivite
 *
 */
void Op_Diff_VEF_Face::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}

void Op_Diff_VEF_Face::completer()
{
  Operateur_base::completer();
}

const Champ_base& Op_Diff_VEF_Face::diffusivite() const
{
  return diffusivite_.valeur();
}

void Op_Diff_VEF_Face::ajouter_cas_scalaire(const DoubleTab& tab_inconnue,
                                            DoubleTab& tab_resu, DoubleTab& tab_flux_bords,
                                            DoubleTab& tab_nu,
                                            const Domaine_Cl_VEF& domaine_Cl_VEF,
                                            const Domaine_VEF& domaine_VEF ) const
{
  int nb_faces = domaine_VEF.nb_faces();
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  int nb_bords=domaine_VEF.nb_front_Cl();
  const int premiere_face_int=domaine_VEF.premiere_face_int();

  // On dimensionne et initialise le tableau des bilans de flux:
  if (tab_flux_bords.size_array()==0) tab_flux_bords.resize(domaine_VEF.nb_faces_bord(),1);
  tab_flux_bords=0.;

  CIntTabView elem_faces = domaine_VEF.elem_faces().view_ro();
  CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
  CDoubleTabView face_normale = domaine_VEF.face_normales().view_ro();
  CDoubleArrView inverse_volumes = domaine_VEF.inverse_volumes().view_ro();
  CDoubleTabView nu = tab_nu.view_ro();
  CDoubleTabView inconnue = tab_inconnue.view_ro();
  DoubleTabView flux_bords = tab_flux_bords.view_rw();
  DoubleTabView resu = tab_resu.view_rw();
  // On traite les faces bord
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int num1=0;
      int num2=le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();
      CIntArrView le_bord_num_face = le_bord.num_face().view_ro();
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          CIntArrView face_associee = la_cl_perio.face_associee().view_ro();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                               Kokkos::RangePolicy<>(num1, nb_faces_bord_reel), KOKKOS_LAMBDA(
                                 const int ind_face)
          {
            int num_face = le_bord_num_face(ind_face);
            int fac_asso = face_associee(ind_face);
            fac_asso = le_bord_num_face(fac_asso);
            for (int kk=0; kk<2; kk++)
              {
                int elem = face_voisins(num_face,kk);
                for (int i=0; i<nb_faces_elem; i++)
                  {
                    int j = elem_faces(elem,i);
                    if ( j > num_face  && j != fac_asso )
                      {
                        double valA = viscA(num_face,j,elem, nu(elem,0), face_voisins, face_normale, inverse_volumes);
                        double flux = valA*(inconnue(j,0)-inconnue(num_face,0));
                        Kokkos::atomic_add(&resu(num_face,0), +flux);
                        if(j<nb_faces) // face reelle
                          Kokkos::atomic_add(&resu(j,0), -0.5*flux);
                      }
                  }
              }
          });
          end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
        }
      else   // Il n'y a qu'une seule composante, donc on traite
        // une equation scalaire (pas la vitesse) on a pas a utiliser
        // le tau tangentiel (les lois de paroi thermiques ne calculent pas
        // d'echange turbulent a la paroi pour l'instant
        {
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                               Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(
                                 const int ind_face)
          {
            int num_face = le_bord_num_face(ind_face);
            int elem = face_voisins(num_face,0);
            for (int i=0; i<nb_faces_elem; i++)
              {
                int j = elem_faces(elem,i);
                if (j > num_face || num_face>=nb_faces)
                  {
                    double valA = viscA(num_face,j,elem,nu(elem,0), face_voisins, face_normale, inverse_volumes);
                    double flux=valA*(inconnue(j,0)-inconnue(num_face,0));
                    if (num_face<nb_faces) // face reelle
                      {
                        Kokkos::atomic_add(&resu(num_face,0), +flux);
                        Kokkos::atomic_add(&flux_bords(num_face,0), -flux);
                      }
                    if(j<nb_faces) // face reelle
                      {
                        Kokkos::atomic_add(&resu(j,0), -flux);
                        if (j<premiere_face_int)
                          Kokkos::atomic_add(&flux_bords(j,0), +flux);
                      }
                  }
              }
          });
          end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
        }
    }

  // Faces internes :
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(premiere_face_int, nb_faces), KOKKOS_LAMBDA (const int num_face)
  {
    for (int k = 0; k < 2; k++)
      {
        int elem = face_voisins(num_face,k);
        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem,i);
            if (j > num_face)
              {
                int contrib = 1;

                if(j >= nb_faces) // C'est une face virtuelle
                  {
                    int el1 = face_voisins(j,0);
                    int el2 = face_voisins(j,1);
                    if((el1 == -1) || (el2 == -1))
                      contrib = 0;
                  }

                if(contrib)
                  {
                    double valA = viscA(num_face,j,elem,nu(elem,0),face_voisins,face_normale,inverse_volumes);
                    double flux = valA * (inconnue(j,0) - inconnue(num_face,0));
                    Kokkos::atomic_add(&resu(num_face,0), flux);
                    if(j < nb_faces) // On traite les faces reelles
                      Kokkos::atomic_add(&resu(j,0), -flux);
                  }
              }
          }
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
  /* ToDo OpenMP use grad as calculer_cas_vectoriel
  for (int num_face=premiere_face_int; num_face<nb_faces; num_face++)
  {
    for (int k=0; k<2; k++)
      {
        int elem = face_voisins_addr[2*num_face+k];
        if (elem>=0)
          {
            int ori = 1 - 2 * k;
            for (int j = 0; j < dimension; j++)
              {
                double flux = ori * face_normales_addr[num_face * dimension + j]
                              * nu_addr[elem] * grad_addr[elem * dimension + j];
                #pragma omp atomic
                resu_addr[num_face] -= flux;
              }
          }
      } // Fin de la boucle sur les 2 elements comnuns a la face
  } // Fin de la boucle sur les faces */

  // Neumann :
  copyPartialFromDevice(tab_resu, 0, premiere_face_int, "resu on boundary");
  copyPartialFromDevice(tab_inconnue, 0, premiere_face_int, "inconnue on boundary");
  start_gpu_timer();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {
              double flux=la_cl_paroi.flux_impose(face-ndeb)*domaine_VEF.surface(face);
              tab_resu[face] += flux;
              tab_flux_bords(face,0) = flux;
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {
              double flux=la_cl_paroi.h_imp(face-ndeb)*(la_cl_paroi.T_ext(face-ndeb)-tab_inconnue(face))*domaine_VEF.surface(face);
              tab_resu[face] += flux;
              tab_flux_bords(face,0) = flux;

              if (la_cl_paroi.has_emissivite())
                {
                  const double text = la_cl_paroi.T_ext(face - ndeb), T = tab_inconnue(face);
                  flux = 5.67e-8 * la_cl_paroi.emissivite(face - ndeb) * (text * text * text * text - T * T * T * T) * domaine_VEF.surface(face);
                  tab_resu[face] += flux;
                  tab_flux_bords(face, 0) += flux;
                }
            }
        }
      else if (sub_type(Echange_couplage_thermique, la_cl.valeur()))
        {
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {
              double h=la_cl_paroi.h_imp(face-ndeb);
              double Text=la_cl_paroi.T_ext(face-ndeb);
              double phiext=la_cl_paroi.flux_exterieur_impose(face-ndeb);
              double flux=(phiext+h*(Text-tab_inconnue(face)))*domaine_VEF.surface(face);
              tab_resu[face] += flux;
              tab_flux_bords(face,0) = flux;
            }
        }
      else if (sub_type(Neumann_homogene,la_cl.valeur())
               || sub_type(Symetrie,la_cl.valeur())
               || sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          for (int face=ndeb; face<nfin; face++)
            tab_flux_bords(face,0) = 0.;
        }
    }
  end_gpu_timer(0, __KERNEL_NAME__);
  copyPartialToDevice(tab_resu, 0, premiere_face_int, "resu on boundary");
  copyPartialToDevice(tab_inconnue, 0, premiere_face_int, "inconnue on boundary");
}

void Op_Diff_VEF_Face::ajouter_cas_vectoriel(const DoubleTab& inconnue,
                                             DoubleTab& resu, DoubleTab& tab_flux_bords,
                                             DoubleTab& nu,
                                             const Domaine_Cl_VEF& domaine_Cl_VEF,
                                             const Domaine_VEF& domaine_VEF,
                                             int nb_comp) const
{
  assert(nb_comp==dimension);

  // On dimensionne et initialise le tableau des bilans de flux:
  tab_flux_bords.resize(domaine_VEF.nb_faces_bord(),nb_comp);
  tab_flux_bords=0.;

  // Construction du tableau grad_ si necessaire
  if(!grad_.get_md_vector().non_nul())
    {
      grad_.resize(0, Objet_U::dimension, Objet_U::dimension);
      domaine_VEF.domaine().creer_tableau_elements(grad_);
    }
  Champ_P1NC::calcul_gradient(inconnue,grad_,domaine_Cl_VEF);

  /* ToDo OpenMP : factoriser avec Op_Dift_VEF_Face.cpp dans une classe template
  if (le_modele_turbulence->utiliser_loi_paroi())
   {
      Champ_P1NC::calcul_duidxj_paroi(grad_,nu,nu_turb,tau_tan_,domaine_Cl_VEF);
      grad_.echange_espace_virtuel(); // gradient_elem a jour sur les elements virtuels
  }
  DoubleTab Re;
  Re.resize(0, Objet_U::dimension, Objet_U::dimension);
  domaine_VEF.domaine().creer_tableau_elements(Re);
  Re = 0.;
  if (le_modele_turbulence->calcul_tenseur_Re(nu_turb, grad_, Re))
  {
      Cerr << "On utilise une diffusion turbulente non linaire dans NS" << finl;
      for (int elem=0; elem<nb_elem; elem++)
          for (int i=0; i<nbr_comp; i++)
              for (int j=0; j<nbr_comp; j++)
                  Re(elem,i,j) *= nu_turb[elem];
  }
  else
  {
      for (int elem=0; elem<nb_elem; elem++)
          for (int i=0; i<nbr_comp; i++)
              for (int j=0; j<nbr_comp; j++)
                  Re(elem,i,j) = nu_turb[elem]*(grad_(elem,i,j) + grad_(elem,j,i));
  }
  Re.echange_espace_virtuel();
  */

  int nb_faces = domaine_VEF.nb_faces();
  int nb_faces_bord = domaine_VEF.premiere_face_int();
  CIntTabView face_voisins_v = domaine_VEF.face_voisins().view_ro();
  CDoubleTabView face_normales_v = domaine_VEF.face_normales().view_ro();
  CDoubleTabView nu_v = nu.view_ro();
  CDoubleTabView3 grad_v = grad_.view3_ro();
  DoubleTabView resu_v = resu.view_rw();
  DoubleTabView tab_flux_bords_v = tab_flux_bords.view_rw();

  auto kern_ajouter = KOKKOS_LAMBDA(int
                                    num_face)
  {
    for (int k = 0; k < 2; k++)
      {
        int elem = face_voisins_v(num_face, k);
        if (elem >= 0)
          {
            int ori = 1 - 2 * k;
            double nu_elem = nu_v(elem, 0);
            for (int i = 0; i < nb_comp; i++)
              for (int j = 0; j < nb_comp; j++)
                {
                  double flux = ori * face_normales_v(num_face, j) * (nu_elem * grad_v(elem, i, j)  /* + Re(elem, i, j) */ );
                  Kokkos::atomic_sub(&resu_v(num_face, i), flux);

                  double flux_bord = ori * face_normales_v(num_face, j) * (nu_elem * (grad_v(elem, i, j) + grad_v(elem, j, i)) );
                  if (num_face < nb_faces_bord)
                    Kokkos::atomic_sub(&tab_flux_bords_v(num_face, i), flux_bord);
                }
          }
      }
  };

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces, kern_ajouter);
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  // Update flux_bords on symmetry:
  const int nb_bords=domaine_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Symetrie,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            tab_flux_bords(face,0) = 0.;
        }
    }
}

void Op_Diff_VEF_Face::ajouter_cas_multi_scalaire(const DoubleTab& inconnue,
                                                  DoubleTab& resu, DoubleTab& tab_flux_bords,
                                                  DoubleTab& nu,
                                                  const Domaine_Cl_VEF& domaine_Cl_VEF,
                                                  const Domaine_VEF& domaine_VEF,
                                                  int nb_comp) const
{
  const IntTab& elemfaces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int i0,j,num_face;
  int nb_faces = domaine_VEF.nb_faces();
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  int n_bord;
  double flux0;
  //DoubleVect n(Objet_U::dimension);
  //DoubleTrav Tgrad(Objet_U::dimension,Objet_U::dimension);

  // On dimensionne et initialise le tableau des bilans de flux:
  tab_flux_bords.resize(domaine_VEF.nb_faces_bord(),nb_comp);
  tab_flux_bords=0.;
  assert(nb_comp>1);
  int nb_bords=domaine_VEF.nb_front_Cl();
  int ind_face;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      // const IntTab& elemfaces = domaine_VEF.elem_faces();
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (ind_face=num1; ind_face<nb_faces_bord_reel; ind_face++)
            {
              fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso = le_bord.num_face(fac_asso);
              num_face = le_bord.num_face(ind_face);
              for (int kk=0; kk<2; kk++)
                {
                  int elem = face_voisins(num_face, kk);
                  for (i0=0; i0<nb_faces_elem; i0++)
                    {
                      if ( ( (j= elemfaces(elem,i0)) > num_face ) && (j != fac_asso ) )
                        {
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              double valA = viscA(num_face,j,elem,nu(elem,nc));
                              resu(num_face,nc)+=valA*inconnue(j,nc);
                              resu(num_face,nc)-=valA*inconnue(num_face,nc);
                              if(j<nb_faces) // face reelle
                                {
                                  ////ATENTION DIFF NUM_face avec ma version
                                  resu(j,nc)+=0.5*valA*inconnue(num_face,nc);
                                  resu(j,nc)-=0.5*valA*inconnue(j,nc);
                                }
                            }
                        }
                    }
                }
            }
        }// fin if periodique
      else
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              int elem=face_voisins(num_face,0);

              // Boucle sur les faces :
              for (int i=0; i<nb_faces_elem; i++)
                if (( (j= elemfaces(elem,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                  {
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        double valA = viscA(num_face,j,elem,nu(elem,nc));
                        if (ind_face<nb_faces_bord_reel)
                          {
                            double flux=valA*(inconnue(j,nc)-inconnue(num_face,nc));
                            resu(num_face,nc)+=flux;
                            tab_flux_bords(num_face,nc)-=flux;
                          }

                        if(j<nb_faces) // face reelle
                          {
                            resu(j,nc)+=valA*inconnue(num_face,nc);
                            resu(j,nc)-=valA*inconnue(j,nc);
                          }
                      }
                  }
            }
        }
    }//Fin for n_bord

  // On traite les faces internes

  for (num_face=domaine_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
    {
      for (int k=0; k<2; k++)
        {
          int elem = face_voisins(num_face,k);
          for (i0=0; i0<nb_faces_elem; i0++)
            {
              if ( (j= elemfaces(elem,i0)) > num_face )
                {
                  int el1,el2;
                  int contrib=1;
                  if(j>=nb_faces) // C'est une face virtuelle
                    {
                      el1 = face_voisins(j,0);
                      el2 = face_voisins(j,1);
                      if((el1==-1)||(el2==-1))
                        contrib=0;
                    }
                  if(contrib)
                    {
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          double valA = viscA(num_face,j,elem,nu(elem,nc));
                          resu(num_face,nc)+=valA*inconnue(j,nc);
                          resu(num_face,nc)-=valA*inconnue(num_face,nc);
                          if(j<nb_faces) // On traite les faces reelles
                            {
                              resu(j,nc)+=valA*inconnue(num_face,nc);
                              resu(j,nc)-=valA*inconnue(j,nc);
                            }
                          else
                            {
                              // La face j est virtuelle
                            }
                        }
                    }
                }
            }
        }
    }// Fin faces internes


  //On se base sur ce qui est fait pour le cas scalaire
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  flux0=la_cl_paroi.flux_impose(face-ndeb,nc)*domaine_VEF.surface(face);
                  resu(face,nc) += flux0;
                  tab_flux_bords(face,nc) = flux0;
                }
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          throw;
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  flux0=la_cl_paroi.h_imp(face-ndeb,nc)*(la_cl_paroi.T_ext(face-ndeb,nc)-inconnue(face,nc))*domaine_VEF.surface(face);
                  resu(face,nc) += flux0;
                  tab_flux_bords(face,nc) = flux0;

                  if (la_cl_paroi.has_emissivite())
                    {
                      const double text = la_cl_paroi.T_ext(face - ndeb, nc), T = inconnue(face, nc);
                      flux0 = 5.67e-8 * la_cl_paroi.emissivite(face - ndeb, nc) * (text * text * text * text - T * T * T * T) * domaine_VEF.surface(face);
                      resu(face, nc) += flux0;
                      tab_flux_bords(face, nc) += flux0;
                    }
                }
            }
        }
      else if (sub_type(Echange_couplage_thermique,la_cl.valeur()))
        {
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  double phiext = la_cl_paroi.flux_exterieur_impose(face-ndeb,nc);
                  flux0 = (phiext + la_cl_paroi.h_imp(face-ndeb,nc)*(la_cl_paroi.T_ext(face-ndeb,nc)-inconnue(face,nc)))*domaine_VEF.surface(face);
                  resu(face,nc) += flux0;
                  tab_flux_bords(face,nc) = flux0;
                }
            }
        }
      else if (sub_type(Neumann_homogene,la_cl.valeur())
               || sub_type(Symetrie,la_cl.valeur())
               || sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int nc=0; nc<nb_comp; nc++)
              tab_flux_bords(face,nc) = 0.;
        }
    }
}


DoubleTab& Op_Diff_VEF_Face::ajouter(const DoubleTab& inconnue_org, DoubleTab& resu) const
{
  remplir_nu(nu_);
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();

  int nb_comp = 1;
  int nb_dim = resu.nb_dim();
  if(nb_dim==2)
    nb_comp=resu.dimension(1);
  DoubleTab nu;
  DoubleTab tab_inconnue;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();
  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  const DoubleTab& inconnue=modif_par_porosite_si_flag(inconnue_org,tab_inconnue,marq,porosite_face);

  const Champ_base& inco = equation().inconnue();
  const Nature_du_champ nature_champ = inco.nature_du_champ();
  if(nature_champ==scalaire)
    ajouter_cas_scalaire(inconnue, resu, flux_bords_, nu, domaine_Cl_VEF, domaine_VEF);
  else if (nature_champ==vectoriel)
    ajouter_cas_vectoriel(inconnue, resu, flux_bords_, nu, domaine_Cl_VEF, domaine_VEF,nb_comp);
  else if (nature_champ==multi_scalaire)
    ajouter_cas_multi_scalaire(inconnue, resu, flux_bords_, nu, domaine_Cl_VEF, domaine_VEF,nb_comp);
  modifier_flux(*this);

  return resu;
}

DoubleTab& Op_Diff_VEF_Face::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}

void Op_Diff_VEF_Face::ajouter_contribution(const DoubleTab& tab_transporte, Matrice_Morse& tab_matrice) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();

  modifier_matrice_pour_periodique_avant_contribuer(tab_matrice,equation());

  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  DoubleTab tab_nu;

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  int marq = phi_psi_diffuse(equation());
  modif_par_porosite_si_flag(nu_,tab_nu,!marq,equation().milieu().porosite_elem());

  int nb_dim = tab_transporte.nb_dim();
  int nb_comp = (nb_dim==2 ? tab_transporte.dimension(1) : 1);
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  int nb_bords = domaine_VEF.nb_front_Cl();

  CIntTabView elem_faces = domaine_VEF.elem_faces().view_ro();
  CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
  CDoubleArrView porosite_face = equation().milieu().porosite_face().view_ro();
  CDoubleArrView inverse_volumes = domaine_VEF.inverse_volumes().view_ro();
  CDoubleTabView face_normale = domaine_VEF.face_normales().view_ro();
  CDoubleTabView nu = tab_nu.view_ro();

  IntVect tab_face_associee(domaine_VEF.premiere_face_int());
  IntVect tab_fac2b_idx(domaine_VEF.nb_faces());

  Matrice_Morse_View matrice;
  matrice.set(tab_matrice);

  // Récupérer les indices des faces de bord périodiques et les
  // faces associées dans des tableaux au préalable pour
  // permettre un accès structuré dans le kernel ensuite
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int num1 = le_bord.num_premiere_face();

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          int nb_faces = le_bord.nb_faces();
          int num2b = num1 + nb_faces / 2;

          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face
          // associee...
          for (int fac = num1; fac < num2b; fac++)
            {
              int fac_asso = la_cl_perio.face_associee(fac - num1) + num1;

              tab_face_associee(fac) = fac_asso;
              tab_fac2b_idx(fac) = fac;
              tab_fac2b_idx(fac+nb_faces/2) = fac;
            }
        }
    }

  CIntArrView est_face_bord = domaine_VEF.est_face_bord().view_ro();
  CIntArrView face_associee = tab_face_associee.view_ro();
  CIntArrView fac2b_idx = tab_fac2b_idx.view_ro();

  auto ajouter_contrib = KOKKOS_LAMBDA(const int fac)
  {
    int type_face = est_face_bord(fac);
    int fac2b = fac2b_idx(fac);

    if (type_face == 2 && fac == fac2b)
      {
        int elem1 = face_voisins(fac,0);
        int elem2 = face_voisins(fac,1);

        int fac_asso = face_associee(fac);
        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem1,i);
            if (j > fac)
              {
                double val = viscA(fac,j,elem1,nu(elem1,0), face_voisins, face_normale, inverse_volumes);
                double coeff_face1 = val * (marq ? porosite_face(fac) : 1);
                double coeff_face2 = val * (marq ? porosite_face(j) : 1);

                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    int n0 = fac*nb_comp + nc;
                    int j0 = j*nb_comp + nc;

                    Kokkos::atomic_add(&matrice(n0,n0), +coeff_face1);
                    Kokkos::atomic_add(&matrice(n0,j0), -coeff_face2);
                    Kokkos::atomic_add(&matrice(j0,n0), -coeff_face1);
                    Kokkos::atomic_add(&matrice(j0,j0), +coeff_face2);
                  }
              }
            if (elem2 != -1)
              {
                j = elem_faces(elem2,i);
                if (j > fac)
                  {
                    double val = viscA(fac,j,elem2,nu(elem2,0), face_voisins, face_normale, inverse_volumes);
                    double coeff_face1 = val * (marq ? porosite_face(fac) : 1);
                    double coeff_face2 = val * (marq ? porosite_face(j) : 1);

                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        int n0 = fac*nb_comp + nc;
                        int j0 = j*nb_comp + nc;
                        int n1 = fac_asso*nb_comp+nc;

                        Kokkos::atomic_add(&matrice(n0,n0), +coeff_face1);
                        Kokkos::atomic_add(&matrice(n0,j0), -coeff_face2);
                        Kokkos::atomic_add(&matrice(j0,n1), -coeff_face1);
                        Kokkos::atomic_add(&matrice(j0,j0), +coeff_face2);
                      }
                  }
              }
          }
      }
    else if (type_face == 1)
      {
        int elem1 = face_voisins(fac,0);

        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem1,i);
            if (j > fac)
              {
                double val = viscA(fac,j,elem1,nu(elem1,0), face_voisins, face_normale, inverse_volumes);
                double coeff_face1 = val * (marq ? porosite_face(fac) : 1);
                double coeff_face2 = val * (marq ? porosite_face(j) : 1);

                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    int n0 = fac*nb_comp + nc;
                    int j0 = j*nb_comp + nc;

                    Kokkos::atomic_add(&matrice(n0,n0), +coeff_face1);
                    Kokkos::atomic_add(&matrice(n0,j0), -coeff_face2);
                    Kokkos::atomic_add(&matrice(j0,n0), -coeff_face1);
                    Kokkos::atomic_add(&matrice(j0,j0), +coeff_face2);
                  }
              }
          }
      }
    else if (type_face == 0)
      {
        int elem1 = face_voisins(fac,0);
        int elem2 = face_voisins(fac,1);

        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem1,i);
            if (j > fac)
              {
                double val = viscA(fac,j,elem1,nu(elem1,0), face_voisins, face_normale, inverse_volumes);
                double coeff_face1 = val * (marq ? porosite_face(fac) : 1);
                double coeff_face2 = val * (marq ? porosite_face(j) : 1);

                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    int n0 = fac*nb_comp+nc;
                    int j0 = j*nb_comp+nc;

                    Kokkos::atomic_add(&matrice(n0,n0), +coeff_face1);
                    Kokkos::atomic_add(&matrice(n0,j0), -coeff_face2);
                    Kokkos::atomic_add(&matrice(j0,n0), -coeff_face1);
                    Kokkos::atomic_add(&matrice(j0,j0), +coeff_face2);
                  }
              }
            if (elem2 != -1)
              {
                j = elem_faces(elem2,i);
                if (j > fac)
                  {
                    double val = viscA(fac,j,elem2,nu(elem2,0), face_voisins, face_normale, inverse_volumes);
                    double coeff_face1 = val * (marq ? porosite_face(fac) : 1);
                    double coeff_face2 = val * (marq ? porosite_face(j) : 1);

                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        int n0 = fac*nb_comp+nc;
                        int j0 = j*nb_comp+nc;

                        Kokkos::atomic_add(&matrice(n0,n0), +coeff_face1);
                        Kokkos::atomic_add(&matrice(n0,j0), -coeff_face2);
                        Kokkos::atomic_add(&matrice(j0,n0), -coeff_face1);
                        Kokkos::atomic_add(&matrice(j0,j0), +coeff_face2);
                      }
                  }
              }
          }
      }
  };


  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),domaine_VEF.nb_faces(), ajouter_contrib);
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  int premiere_face_int = domaine_VEF.premiere_face_int();
  DoubleVect tab_h_impose(premiere_face_int);
  DoubleVect tab_derivee_flux_exterieur_imposee(premiere_face_int);

  // Neumann: remplir les tableaux avec les conditions aux
  // bords pour le kernel Kokkos
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            {
              tab_h_impose(face) = la_cl_paroi.h_imp(face - ndeb);

              if (la_cl_paroi.has_emissivite())
                {
                  const DoubleTab& inconnue = equation().inconnue().valeurs();

                  const double T = inconnue(face);
                  tab_h_impose(face) = 4 * 5.67e-8 * la_cl_paroi.emissivite(face - ndeb) * T * T * T;
                }
            }
        }
      else if (sub_type(Echange_couplage_thermique, la_cl.valeur()))
        {
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());

          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            {
              tab_h_impose(face) = la_cl_paroi.h_imp(face-ndeb);
              tab_derivee_flux_exterieur_imposee(face) = la_cl_paroi.derivee_flux_exterieur_imposee(face-ndeb);
            }
        }
    }

  CDoubleArrView h_impose = tab_h_impose.view_ro();
  CDoubleArrView derivee_flux_exterieur_imposee = tab_derivee_flux_exterieur_imposee.view_ro();
  CDoubleArrView face_surfaces = domaine_VEF.face_surfaces().view_ro();

  // Neumann: calcul des contributions aux bords
  auto neumann = KOKKOS_LAMBDA (const int face)
  {
    double h = h_impose(face);
    double dphi_dT = derivee_flux_exterieur_imposee(face);
    matrice(face,face) += (h + dphi_dT) * face_surfaces(face);
  };

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), premiere_face_int, neumann);
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  modifier_matrice_pour_periodique_apres_contribuer(tab_matrice,equation());
}

void Op_Diff_VEF_Face::ajouter_contribution_multi_scalaire(const DoubleTab& tab_transporte, Matrice_Morse& tab_matrice) const
{
  modifier_matrice_pour_periodique_avant_contribuer(tab_matrice, equation());

  // On remplit le tableau nu car l'assemblage d'une matrice
  // avec ajouter_contribution peut se faire avant le premier
  // pas de temps
  remplir_nu(nu_);

  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const IntTab& tab_elem_faces = domaine_VEF.elem_faces();
  const IntTab& tab_face_voisins = domaine_VEF.face_voisins();
  const ArrOfInt& tab_est_face_bord = domaine_VEF.est_face_bord();

  int nb_dim = tab_transporte.nb_dim();
  int nb_comp = (nb_dim == 2 ? tab_transporte.dimension(1) : 1);

  DoubleTab tab_nu;
  int marq = phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_, tab_nu, !marq, porosite_elem);
  DoubleVect tab_porosite_eventuelle(equation().milieu().porosite_face());
  if (!marq)
    tab_porosite_eventuelle = 1;

  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  int nb_bords = domaine_VEF.nb_front_Cl();
  int nb_faces_tot = domaine_VEF.nb_faces_tot();

  IntVect tab_face_associee(domaine_VEF.premiere_face_int());
  IntVect tab_fac2b_idx(domaine_VEF.nb_faces_tot());

  // Récupérer les indices des faces de bord périodiques et les
  // faces associées dans des tableaux au préalable pour accès
  // structuré dans le kernel ensuite
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int num1 = le_bord.num_premiere_face();

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          int nb_faces = le_bord.nb_faces();
          int num2b = num1 + nb_faces / 2;

          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face
          // associee...
          for (int fac = num1; fac < num2b; fac++)
            {
              int fac_asso = la_cl_perio.face_associee(fac - num1) + num1;

              tab_face_associee(fac) = fac_asso;
              tab_fac2b_idx(fac) = fac;
              tab_fac2b_idx(fac+nb_faces/2) = fac;
            }
        }
    }

  CIntArrView est_face_bord = tab_est_face_bord.view_ro();
  CIntTabView face_voisins = tab_face_voisins.view_ro();
  CDoubleTabView face_normales = domaine_VEF.face_normales().view_ro();
  CDoubleArrView inverse_volumes = domaine_VEF.inverse_volumes().view_ro();
  CIntTabView elem_faces = tab_elem_faces.view_ro();

  CDoubleTabView nu = tab_nu.view_ro();
  CDoubleArrView porosite_eventuelle = tab_porosite_eventuelle.view_ro();

  CIntArrView fac2b_idx = tab_fac2b_idx.view_ro();
  CIntArrView face_associee = tab_face_associee.view_ro();

  Matrice_Morse_View matrice;
  matrice.set(tab_matrice);

  auto kern_elem_faces = KOKKOS_LAMBDA (const int fac)
  {
    int type_face = est_face_bord(fac);
    int fac2b = fac2b_idx(fac);

    // Periodic boundary faces
    if (type_face == 2 && fac == fac2b)
      {
        int fac_asso = face_associee(fac);

        int elem1 = face_voisins(fac, 0);
        int elem2 = face_voisins(fac, 1);

        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem1, i);
            if (j > fac)
              {
                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    double val = 0.5 * viscA(fac, j, elem1, nu(elem1, nc), face_voisins, face_normales, inverse_volumes);

                    int n0 = fac * nb_comp + nc;
                    int j0 = j * nb_comp + nc;

                    matrice(n0, n0) += val * porosite_eventuelle(fac);
                    matrice(n0, j0) -= val * porosite_eventuelle(j);
                    matrice(j0, n0) -= val * porosite_eventuelle(fac);
                    matrice(j0, j0) += val * porosite_eventuelle(j);
                  }
              }
            if (elem2 != -1)
              {
                j = elem_faces(elem2, i);
                if (j > fac)
                  {
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        double val = 0.5 * viscA(fac, j, elem2, nu(elem1, nc), face_voisins, face_normales, inverse_volumes);

                        int n0 = fac * nb_comp + nc;
                        int j0 = j * nb_comp + nc;
                        int n0perio = fac_asso * nb_comp + nc;

                        matrice(n0, n0) += val * porosite_eventuelle(fac);
                        matrice(n0, j0) -= val * porosite_eventuelle(j);
                        matrice(j0, n0perio) -= val * porosite_eventuelle(fac);
                        matrice(j0, j0) += val * porosite_eventuelle(j);
                      }
                  }
              }
          }
      }
    // Faces de bord non périodiques
    else if (type_face == 1)
      {
        int elem1 = face_voisins(fac, 0);

        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem1, i);
            if (j > fac)
              {
                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    double val = viscA(fac, j, elem1, nu(elem1, nc), face_voisins, face_normales, inverse_volumes);

                    int n0 = fac * nb_comp + nc;
                    int j0 = j * nb_comp + nc;

                    Kokkos::atomic_add(&matrice(n0, n0), +(val * porosite_eventuelle(fac)));
                    Kokkos::atomic_add(&matrice(n0, j0), -(val * porosite_eventuelle(j)));
                    Kokkos::atomic_add(&matrice(j0, n0), -(val * porosite_eventuelle(fac)));
                    Kokkos::atomic_add(&matrice(j0, j0), +(val * porosite_eventuelle(j)));
                  }
              }
          }
      }
    // Faces internes
    else if (type_face == 0)
      {
        int elem1 = face_voisins(fac, 0);
        int elem2 = face_voisins(fac, 1);

        for (int i = 0; i < nb_faces_elem; i++)
          {
            int j = elem_faces(elem1, i);
            if (j > fac)
              {
                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    double val = viscA(fac, j, elem1, nu(elem1, nc), face_voisins, face_normales, inverse_volumes);

                    int n0 = fac * nb_comp + nc;
                    int j0 = j * nb_comp + nc;

                    Kokkos::atomic_add(&matrice(n0, n0), +(val * porosite_eventuelle(fac)));
                    Kokkos::atomic_add(&matrice(n0, j0), -(val * porosite_eventuelle(j)));
                    Kokkos::atomic_add(&matrice(j0, n0), -(val * porosite_eventuelle(fac)));
                    Kokkos::atomic_add(&matrice(j0, j0), +(val * porosite_eventuelle(j)));
                  }
              }

            if (elem2 != -1)
              {
                j = elem_faces(elem2, i);
                if (j > fac)
                  {
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        double val = viscA(fac, j, elem2, nu(elem2, nc), face_voisins, face_normales, inverse_volumes);
                        int n0 = fac * nb_comp + nc;
                        int j0 = j * nb_comp + nc;

                        Kokkos::atomic_add(&matrice(n0, n0), +(val * porosite_eventuelle(fac)));
                        Kokkos::atomic_add(&matrice(n0, j0), -(val * porosite_eventuelle(j)));
                        Kokkos::atomic_add(&matrice(j0, n0), -(val * porosite_eventuelle(fac)));
                        Kokkos::atomic_add(&matrice(j0, j0), +(val * porosite_eventuelle(j)));
                      }
                  }
              }
          }
      }
  };

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces_tot, kern_elem_faces);
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());

      if (sub_type(Echange_externe_impose, la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            for (int nc = 0; nc < nb_comp; nc++)
              {
                const int i = face * nb_comp + nc;
                tab_matrice(i, i) += la_cl_paroi.h_imp(face - ndeb, nc) * domaine_VEF.surface(face);
              }
        }
      if (sub_type(Echange_externe_radiatif, la_cl.valeur()))
        {
          throw;
        }
    }

  modifier_matrice_pour_periodique_apres_contribuer(tab_matrice, equation());
}

void Op_Diff_VEF_Face::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  int nb_comp = 1;
  int nb_dim = resu.nb_dim();

  int nb_bords=domaine_VEF.nb_front_Cl();

  if(nb_dim==2)
    nb_comp=resu.dimension(1);

  // Partie imposee :

  if (nb_dim == 1)
    {
      for (int n_bord=0; n_bord<nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

          if (sub_type(Neumann_paroi,la_cl.valeur()))
            {
              const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();

              for (int face=ndeb; face<nfin; face++)
                resu[face] += la_cl_paroi.flux_impose(face-ndeb)*domaine_VEF.surface(face);
            }
          else if (sub_type(Echange_externe_impose,la_cl.valeur()))
            {
              Cerr << "Non code pour Echange_externe_impose" << finl;
              assert(0);
            }
          else if (sub_type(Echange_externe_radiatif,la_cl.valeur()))
            {
              Cerr << "Non code pour Echange_externe_radiatif" << finl;
              assert(0);
            }

        }
    }
  else
    {
      for (int n_bord=0; n_bord<nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

          if (sub_type(Neumann_paroi,la_cl.valeur()))
            {
              const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int face=ndeb; face<nfin; face++)
                for (int comp=0; comp<nb_comp; comp++)
                  resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*domaine_VEF.surface(face);
            }
        }
    }
}

void Op_Diff_VEF_Face::verifier() const
{
  static int testee=0;
  if(testee)
    return;
  testee=1;
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  //  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  //  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();

  const DoubleTab& xv=domaine_VEF.xv();
  DoubleTab vit(equation().inconnue().valeurs());
  DoubleTab resu(vit);
  int i, comp;
  if(dimension==2)
    {
      const int nbf = vit.dimension(0);
      Cerr << " Verification de delta(x,0) " << finl;
      for(i=0; i<nbf; i++)
        {
          vit(i,0)=xv(i,0);
          vit(i,1)=0;
        }
      calculer(vit, resu);
      for(i=0; i<nbf; i++)
        for(comp=0; comp<dimension; comp++)
          resu(i,comp)/=(volumes_entrelaces(i));
      for(i=0; i<nbf; i++)
        {
          if(std::fabs(resu(i,0))>1.e-10)
            {
              Cerr << " delta(x,0) ("<<i<<") = "
                   << resu(i,0);
              Cerr << finl;
            }
        }
      Cerr << " Verification de delta(y(1-y),0) " << finl;
      for(i=0; i<nbf; i++)
        {
          vit(i,0)=xv(i,1)*(1-xv(i,1));
          vit(i,1)=0;
        }
      calculer(vit, resu);
      for(i=0; i<nbf; i++)
        for(comp=0; comp<dimension; comp++)
          resu(i,comp)/=(volumes_entrelaces(i));
      for(i=0; i<nbf; i++)
        {
          if(std::fabs(2-resu(i,0))>1.e-10)
            {
              Cerr << " delta(y(1-y),0) ("<<i<<") = "
                   << resu(i,0);
              Cerr << finl;
            }
        }
    }
}



