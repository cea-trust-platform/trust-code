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

#include <Op_Diff_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Champ_Uniforme.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Neumann_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>
#include <Milieu_base.h>
#include <TRUSTTrav.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Porosites_champ.h>
#include <Device.h>
#include <Echange_couplage_thermique.h>
#include <Champ_front_calc_interne.h>

Implemente_instanciable(Op_Diff_VEF_Face,"Op_Diff_VEF_const_P1NC",Op_Diff_VEF_base);

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

void Op_Diff_VEF_Face::ajouter_cas_scalaire(const DoubleTab& inconnue,
                                            DoubleTab& resu, DoubleTab& tab_flux_bords,
                                            DoubleTab& nu,
                                            const Domaine_Cl_VEF& domaine_Cl_VEF,
                                            const Domaine_VEF& domaine_VEF ) const
{
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int nb_faces = domaine_VEF.nb_faces();
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  int nb_bords=domaine_VEF.nb_front_Cl();
  // On dimensionne et initialise le tableau des bilans de flux:
  tab_flux_bords.resize(domaine_VEF.nb_faces_bord(),1);
  tab_flux_bords=0.;

  // Construction du tableau grad_ si necessaire
  /*
  if(!grad_.get_md_vector().non_nul())
    {
      grad_.resize(0, Objet_U::dimension);
      domaine_VEF.domaine().creer_tableau_elements(grad_);
    }
  Champ_P1NC::calcul_gradient(inconnue,grad_,domaine_Cl_VEF);
   */

  const int premiere_face_int=domaine_VEF.premiere_face_int();

  // ToDo OpenMP Fusionner avec boucle faces interne
  // On traite les faces bord
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1=0;
      int num2=le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          for (int ind_face=num1; ind_face<nb_faces_bord_reel; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              int fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso = le_bord.num_face(fac_asso);
              for (int kk=0; kk<2; kk++)
                {
                  int elem = face_voisins(num_face,kk);
                  for (int i=0; i<nb_faces_elem; i++)
                    {
                      int j = elem_faces(elem,i);
                      if ( j > num_face  && j != fac_asso )
                        {
                          double valA = viscA(num_face,j,elem,nu(elem));
                          double flux = valA*(inconnue(j)-inconnue(num_face));
                          resu(num_face)+=flux;
                          if(j<nb_faces) // face reelle
                            resu(j)-=+0.5*flux;
                        }
                    }
                }
            }
        }
      else   // Il n'y a qu'une seule composante, donc on traite
        // une equation scalaire (pas la vitesse) on a pas a utiliser
        // le tau tangentiel (les lois de paroi thermiques ne calculent pas
        // d'echange turbulent a la paroi pour l'instant
        {
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              int elem = face_voisins(num_face,0);
              for (int i=0; i<nb_faces_elem; i++)
                {
                  int j = elem_faces(elem,i);
                  if (j > num_face || num_face>=nb_faces)
                    {
                      double valA = viscA(num_face,j,elem,nu(elem));
                      double flux=valA*(inconnue(j)-inconnue(num_face));
                      if (num_face<nb_faces) // face reelle
                        {
                          resu(num_face)+=flux;
                          tab_flux_bords(num_face,0)-=flux;
                        }
                      if(j<nb_faces) // face reelle
                        {
                          resu(j)-=flux;
                          if (j<premiere_face_int)
                            tab_flux_bords(j,0)+=flux;
                        }
                    }
                }
            }
        }
    }

  // Faces internes :
  const int * face_voisins_addr = mapToDevice(domaine_VEF.face_voisins());
  const int* elem_faces_addr = mapToDevice(domaine_VEF.elem_faces());
  const double* face_normales_addr = mapToDevice(domaine_VEF.face_normales());
  const double* inverse_volumes_addr = mapToDevice(domaine_VEF.inverse_volumes());
  const double* inconnue_addr = mapToDevice(inconnue, "inconnue");
  const double* nu_addr = mapToDevice(nu, "nu");
  double* resu_addr = computeOnTheDevice(resu, "resu");
  start_timer();
  #pragma omp target teams distribute parallel for if (computeOnDevice)
  for (int num_face=premiere_face_int; num_face<nb_faces; num_face++)
    {
      for (int k=0; k<2; k++)
        {
          int elem = face_voisins_addr[2*num_face+k];
          {
            for (int i=0; i<nb_faces_elem; i++)
              {
                int j=elem_faces_addr[elem*nb_faces_elem+i];
                if ( j  > num_face )
                  {
                    int contrib=1;
                    if(j>=nb_faces) // C'est une face virtuelle
                      {
                        int el1 = face_voisins_addr[2*j+0];
                        int el2 = face_voisins_addr[2*j+1];
                        if((el1==-1)||(el2==-1))
                          contrib=0;
                      }
                    if(contrib)
                      {
                        double pscal = 0;
                        for (int dim=0; dim<dimension; dim++)
                          pscal += face_normales_addr[num_face*dimension+dim]*face_normales_addr[j*dimension+dim];
                        int signe = (face_voisins_addr[num_face*2] == face_voisins_addr[j*2]) || (face_voisins_addr[num_face*2+1] == face_voisins_addr[j*2+1]) ? -1 : 1;
                        double valA = signe*(pscal*nu_addr[elem])*inverse_volumes_addr[elem];
                        double flux = valA*(inconnue_addr[j]-inconnue_addr[num_face]);
                        #pragma omp atomic
                        resu_addr[num_face]+=flux;
                        if(j<nb_faces) // On traite les faces reelles
                          #pragma omp atomic
                          resu_addr[j]-=flux;
                      }
                  }
              }
          }
        }
    }
  end_timer(Objet_U::computeOnDevice, "Face loop in Op_Diff_VEF_Face::ajouter");
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
  copyPartialFromDevice(resu, 0, premiere_face_int, "resu on boundary");
  copyPartialFromDevice(inconnue, 0, premiere_face_int, "inconnue on boundary");
  start_timer();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {
              double flux=la_cl_paroi.flux_impose(face-ndeb)*domaine_VEF.surface(face);
              resu[face] += flux;
              tab_flux_bords(face,0) = flux;
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {
              double flux=la_cl_paroi.h_imp(face-ndeb)*(la_cl_paroi.T_ext(face-ndeb)-inconnue(face))*domaine_VEF.surface(face);
              resu[face] += flux;
              tab_flux_bords(face,0) = flux;
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
              double flux=(phiext+h*(Text-inconnue(face)))*domaine_VEF.surface(face);
              resu[face] += flux;
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
  end_timer(0, "Boundary condition on resu in Op_Diff_VEF_Face::ajouter_cas_scalaire\n");
  copyPartialToDevice(resu, 0, premiere_face_int, "resu on boundary");
  copyPartialToDevice(inconnue, 0, premiere_face_int, "inconnue on boundary");
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
  if (le_modele_turbulence.valeur().utiliser_loi_paroi())
   {
      Champ_P1NC::calcul_duidxj_paroi(grad_,nu,nu_turb,tau_tan_,domaine_Cl_VEF);
      grad_.echange_espace_virtuel(); // gradient_elem a jour sur les elements virtuels
  }
  DoubleTab Re;
  Re.resize(0, Objet_U::dimension, Objet_U::dimension);
  domaine_VEF.domaine().creer_tableau_elements(Re);
  Re = 0.;
  if (le_modele_turbulence.valeur().calcul_tenseur_Re(nu_turb, grad_, Re))
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
  const int * face_voisins_addr = mapToDevice(domaine_VEF.face_voisins());
  const double * face_normales_addr = mapToDevice(domaine_VEF.face_normales());
  const double * nu_addr = mapToDevice(nu, "nu");
  const double * grad_addr = mapToDevice(grad_, "grad");
  double * resu_addr = computeOnTheDevice(resu, "resu");
  double * tab_flux_bords_addr = computeOnTheDevice(tab_flux_bords, "tab_flux_bords");
  start_timer();
  #pragma omp target teams distribute parallel for if (computeOnDevice)
  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      for (int k=0; k<2; k++)
        {
          int elem = face_voisins_addr[2*num_face+k];
          if (elem>=0)
            {
              int ori = 1 - 2 * k;
              for (int i = 0; i < nb_comp; i++)
                for (int j = 0; j < nb_comp; j++)
                  {
                    double flux = ori * face_normales_addr[num_face * nb_comp + j]
                                  * (nu_addr[elem] * grad_addr[elem * nb_comp * nb_comp + i * nb_comp + j] /* + Re(elem, i, j) */ );
                    #pragma omp atomic
                    resu_addr[num_face * nb_comp + i] -= flux;
                    if (num_face < nb_faces_bord)
                      #pragma omp atomic
                      tab_flux_bords_addr[num_face * nb_comp + i] -= flux;
                  }
            }
        } // Fin de la boucle sur les 2 elements comnuns a la face
    } // Fin de la boucle sur les faces
  end_timer(Objet_U::computeOnDevice, "Face loop in Op_Diff_VEF_Face::ajouter");

  // Update flux_bords on symmetry:
  const int nb_bords=domaine_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Symetrie,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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
  DoubleVect n(Objet_U::dimension);
  DoubleTrav Tgrad(Objet_U::dimension,Objet_U::dimension);

  // On dimensionne et initialise le tableau des bilans de flux:
  tab_flux_bords.resize(domaine_VEF.nb_faces_bord(),nb_comp);
  tab_flux_bords=0.;
  assert(nb_comp>1);
  int nb_bords=domaine_VEF.nb_front_Cl();
  int ind_face;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  flux0=la_cl_paroi.h_imp(face-ndeb,nc)*(la_cl_paroi.T_ext(face-ndeb,nc)-inconnue(face,nc))*domaine_VEF.surface(face);
                  resu(face,nc) += flux0;
                  tab_flux_bords(face,nc) = flux0;
                }
            }
        }
      else if (sub_type(Echange_couplage_thermique,la_cl.valeur()))
        {
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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

  const Champ_base& inco = equation().inconnue().valeur();
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



void Op_Diff_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice) const
{

  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();

  int n1 = domaine_VEF.nb_faces();
  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  DoubleTab nu;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  DoubleVect porosite_eventuelle(equation().milieu().porosite_face());
  if (!marq)
    porosite_eventuelle=1;


  if(nb_dim==2)
    nb_comp=transporte.dimension(1);

  int i,j,num_face;
  int elem1,elem2;
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  double val;

  int nb_bords=domaine_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face associe..
          int num2b=num1+le_bord.nb_faces()/2;
          for (num_face=num1; num_face<num2b; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              elem2 = face_voisins(num_face,1);
              fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j=elem_faces(elem1,i)) > num_face )
                    {
                      val = viscA(num_face,j,elem1,nu(elem1));
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          int n0=num_face*nb_comp+nc;
                          int j0=j*nb_comp+nc;

                          matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                          matrice(n0,j0)-=val*porosite_eventuelle(j);
                          matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                          matrice(j0,j0)+=val*porosite_eventuelle(j);

                        }
                    }
                  if (elem2!=-1)
                    if ( (j=elem_faces(elem2,i)) > num_face )
                      {
                        val= viscA(num_face,j,elem2,nu(elem2));
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            int n0=num_face*nb_comp+nc;
                            int j0=j*nb_comp+nc;
                            int  n0perio=fac_asso*nb_comp+nc;
                            matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                            matrice(n0,j0)-=val*porosite_eventuelle(j);
                            matrice(j0,n0perio)-=val*porosite_eventuelle(num_face);
                            matrice(j0,j0)+=val*porosite_eventuelle(j);

                          }
                      }
                }
            }

        }
      else
        {
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem1,i)) > num_face )
                    {
                      val = viscA(num_face,j,elem1,nu(elem1));
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          int n0=num_face*nb_comp+nc;
                          int j0=j*nb_comp+nc;

                          matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                          matrice(n0,j0)-=val*porosite_eventuelle(j);
                          matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                          matrice(j0,j0)+=val*porosite_eventuelle(j);

                        }
                    }
                }
            }
        }
    }
  int n0 = domaine_VEF.premiere_face_int();
  for (num_face=n0; num_face<n1; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              val = viscA(num_face,j,elem1,nu(elem1));
              for (int nc=0; nc<nb_comp; nc++)
                {
                  int nn0=num_face*nb_comp+nc;
                  int j0=j*nb_comp+nc;

                  matrice(nn0,nn0)+=val*porosite_eventuelle(num_face);
                  matrice(nn0,j0)-=val*porosite_eventuelle(j);
                  matrice(j0,nn0)-=val*porosite_eventuelle(num_face);
                  matrice(j0,j0)+=val*porosite_eventuelle(j);

                }
            }
          if (elem2!=-1)
            if ( (j=elem_faces(elem2,i)) > num_face )
              {
                val= viscA(num_face,j,elem2,nu(elem2));
                for (int nc=0; nc<nb_comp; nc++)
                  {
                    int nn0=num_face*nb_comp+nc;
                    int j0=j*nb_comp+nc;

                    matrice(nn0,nn0)+=val*porosite_eventuelle(num_face);
                    matrice(nn0,j0)-=val*porosite_eventuelle(j);
                    matrice(j0,nn0)-=val*porosite_eventuelle(num_face);
                    matrice(j0,j0)+=val*porosite_eventuelle(j);

                  }
              }
        }
    }
  // Neumann :
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              matrice(face,face) += la_cl_paroi.h_imp(face-ndeb)*domaine_VEF.face_surfaces(face);
            }
        }
      // [ABN]: a finir il faut encore corriger le dimensionnement de la matrice ...
//      else if (sub_type(Echange_interne_global_impose,la_cl.valeur()))
//        {
//          const Echange_interne_global_impose& la_cl_paroi = ref_cast(Echange_interne_global_impose, la_cl.valeur());
//          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
//          const Champ_front_calc_interne& Text = ref_cast(Champ_front_calc_interne, la_cl_paroi.T_ext().valeur());
//          const IntTab& fmap = Text.face_map();
//          int ndeb = le_bord.num_premiere_face();
//          int nfin = ndeb + le_bord.nb_faces();
//          for (int face=ndeb; face<nfin; face++)
//            {
//              int opp_face = fmap(face-ndeb)+ndeb;
//              matrice(face,face) += la_cl_paroi.h_imp(face-ndeb)*domaine_VEF.face_surfaces(face);
//              matrice(opp_face,face) -= la_cl_paroi.h_imp(face-ndeb)*domaine_VEF.face_surfaces(face);
//            }
//        }
      else if (sub_type(Echange_couplage_thermique, la_cl.valeur()))
        {
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              double h=la_cl_paroi.h_imp(face-ndeb);
              double dphi_dT=la_cl_paroi.derivee_flux_exterieur_imposee(face-ndeb);
              matrice(face,face) += (h+dphi_dT)*domaine_VEF.face_surfaces(face);
            }

        }
      else if (sub_type(Neumann_homogene,la_cl.valeur())
               || sub_type(Symetrie,la_cl.valeur())
               || sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
}

void Op_Diff_VEF_Face::ajouter_contribution_multi_scalaire(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();

  int n1 = domaine_VEF.nb_faces();
  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  DoubleTab nu;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  DoubleVect porosite_eventuelle(equation().milieu().porosite_face());
  if (!marq)
    porosite_eventuelle=1;


  if(nb_dim==2)
    nb_comp=transporte.dimension(1);

  int i,j,num_face;
  int elem1,elem2;
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  double val;

  int nb_bords=domaine_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face associe..
          int num2b=num1+le_bord.nb_faces()/2;
          for (num_face=num1; num_face<num2b; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              elem2 = face_voisins(num_face,1);
              fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j=elem_faces(elem1,i)) > num_face )
                    {
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          val = viscA(num_face,j,elem1,nu(elem1,nc));

                          int n0=num_face*nb_comp+nc;
                          int j0=j*nb_comp+nc;

                          matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                          matrice(n0,j0)-=val*porosite_eventuelle(j);
                          matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                          matrice(j0,j0)+=val*porosite_eventuelle(j);

                        }
                    }
                  if (elem2!=-1)
                    if ( (j=elem_faces(elem2,i)) > num_face )
                      {
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            val = viscA(num_face,j,elem2,nu(elem1,nc));
                            int n0=num_face*nb_comp+nc;
                            int j0=j*nb_comp+nc;
                            int  n0perio=fac_asso*nb_comp+nc;
                            matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                            matrice(n0,j0)-=val*porosite_eventuelle(j);
                            matrice(j0,n0perio)-=val*porosite_eventuelle(num_face);
                            matrice(j0,j0)+=val*porosite_eventuelle(j);

                          }
                      }
                }
            }

        }
      else
        {
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem1,i)) > num_face )
                    {
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          val = viscA(num_face,j,elem1,nu(elem1,nc));
                          int n0=num_face*nb_comp+nc;
                          int j0=j*nb_comp+nc;

                          matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                          matrice(n0,j0)-=val*porosite_eventuelle(j);
                          matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                          matrice(j0,j0)+=val*porosite_eventuelle(j);

                        }
                    }
                }
            }
        }
    }
  for (num_face=domaine_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  val = viscA(num_face,j,elem1,nu(elem1,nc));
                  int n0=num_face*nb_comp+nc;
                  int j0=j*nb_comp+nc;

                  matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                  matrice(n0,j0)-=val*porosite_eventuelle(j);
                  matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                  matrice(j0,j0)+=val*porosite_eventuelle(j);

                }
            }
          if (elem2!=-1)
            if ( (j=elem_faces(elem2,i)) > num_face )
              {
                for (int nc=0; nc<nb_comp; nc++)
                  {
                    val= viscA(num_face,j,elem2,nu(elem2,nc));
                    int n0=num_face*nb_comp+nc;
                    int j0=j*nb_comp+nc;

                    matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                    matrice(n0,j0)-=val*porosite_eventuelle(j);
                    matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                    matrice(j0,j0)+=val*porosite_eventuelle(j);

                  }
              }
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
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
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
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



