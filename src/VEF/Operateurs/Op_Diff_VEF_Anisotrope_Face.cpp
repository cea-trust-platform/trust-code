/****************************************************************************
* Copyright (c) 2021, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Diff_VEF_Anisotrope_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VEF_Anisotrope_Face.h>
#include <Champ_P1NC.h>
#include <Champ_Q1NC.h>
#include <Champ_Uniforme.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Neumann_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>
#include <Ref_Champ_P1NC.h>
#include <Ref_Champ_Q1NC.h>
#include <Milieu_base.h>
#include <Debog.h>
#include <DoubleTrav.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Porosites_champ.h>
#include <Check_espace_virtuel.h>

Implemente_instanciable( Op_Diff_VEF_Anisotrope_Face, "Op_Diff_VEFANISOTROPE_var_P1NC", Op_Diff_VEF_base) ;


//// printOn
Sortie& Op_Diff_VEF_Anisotrope_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
Entree& Op_Diff_VEF_Anisotrope_Face::readOn(Entree& s )
{
  return s ;
}

// Description:
// associe le champ de diffusivite
void Op_Diff_VEF_Anisotrope_Face::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}

void Op_Diff_VEF_Anisotrope_Face::completer()
{
  Operateur_base::completer();
}

const Champ_base& Op_Diff_VEF_Anisotrope_Face::diffusivite() const
{
  return diffusivite_.valeur();
}

void Op_Diff_VEF_Anisotrope_Face::ajouter_cas_scalaire(const DoubleTab& inconnue,
                                                       DoubleTab& resu, DoubleTab& tab_flux_bords,
                                                       DoubleTab& nu,
                                                       const Zone_Cl_VEF& zone_Cl_VEF,
                                                       const Zone_VEF& zone_VEF ) const
{
  // assurer on a un tableau bidimensionnel
  assert(nu.nb_dim()==2);
  // assurer vrai nombre de composants pour l'anisotropie (9 pour 3d et 4 pour 2d)
  assert(nu.dimension(1)==dimension*dimension);

  const IntTab& elemfaces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int i,j,num_face;
  int nb_faces = zone_VEF.nb_faces();
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA,flux;
  int n_bord, ind_face;
  int nb_bords=zone_VEF.nb_front_Cl();
  // On dimensionne et initialise le tableau des bilans de flux:
  tab_flux_bords.resize(zone_VEF.nb_faces_bord(),1);
  tab_flux_bords=0.;
  const int& premiere_face_int=zone_VEF.premiere_face_int();

  // On traite les faces bord
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      //const IntTab& elemfaces = zone_VEF.elem_faces();
      int num1=0;
      int num2=le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (ind_face=num1; ind_face<nb_faces_bord_reel; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso = le_bord.num_face(fac_asso);
              for (int kk=0; kk<2; kk++)
                {
                  int elem = face_voisins(num_face,kk);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( ( (j= elemfaces(elem,i)) > num_face ) && (j != fac_asso) )
                        {
                          valA = viscA(num_face,j,elem,nu);
                          resu(num_face)+=valA*inconnue(j);
                          resu(num_face)-=valA*inconnue(num_face);
                          if(j<nb_faces) // face reelle
                            {
                              resu(j)+=0.5*valA*inconnue(num_face);
                              resu(j)-=0.5*valA*inconnue(j);
                            }
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
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              int elem = face_voisins(num_face,0);

              for (i=0; i<nb_faces_elem; i++)
                {
                  if (( (j= elemfaces(elem,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                    {
                      valA = viscA(num_face,j,elem,nu);

                      if (ind_face<nb_faces_bord_reel)
                        {
                          flux=valA*(inconnue(j)-inconnue(num_face));
                          // PL : c'est bien un - ici pour flux_bords. Cette valeur est ensuite
                          // ecrasee pour les bords avec Neumann et ne sert donc que pour les bords
                          // avec Dirichlet ou le volume de controle est nul
                          tab_flux_bords(num_face,0)-=flux;
                          resu(num_face)+=flux;
                        }

                      if(j<nb_faces) // face reelle
                        {
                          flux=valA*(inconnue(num_face)-inconnue(j));
                          if (j<premiere_face_int)
                            tab_flux_bords(j,0)-=flux;
                          resu(j)+=flux;
                        }
                    }
                }
            }
        }
    }

  // Faces internes :
  for (num_face=premiere_face_int; num_face<nb_faces; num_face++)
    {
      for (int k=0; k<2; k++)
        {
          int elem = face_voisins(num_face,k);
          {
            for (i=0; i<nb_faces_elem; i++)
              {
                j=elemfaces(elem,i);
                if ( j  > num_face )
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
                        valA = viscA(num_face,j,elem,nu);
                        resu(num_face)+=valA*inconnue(j);
                        resu(num_face)-=valA*inconnue(num_face);
                        if(j<nb_faces) // On traite les faces reelles
                          {
                            resu(j)+=valA*inconnue(num_face);
                            resu(j)-=valA*inconnue(j);
                          }
                      }
                  }
              }
          }
        }
    }

  // Neumann :
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              flux=la_cl_paroi.flux_impose(face-ndeb)*zone_VEF.surface(face);
              resu[face] += flux;
              tab_flux_bords(face,0) = flux;
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
              flux=la_cl_paroi.h_imp(face-ndeb)*(la_cl_paroi.T_ext(face-ndeb)-inconnue(face))*zone_VEF.surface(face);
              resu[face] += flux;
              tab_flux_bords(face,0) = flux;
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
            tab_flux_bords(face,0) = 0.;
        }
    }
}

void Op_Diff_VEF_Anisotrope_Face::ajouter_cas_vectoriel(const DoubleTab& inconnue,
                                                        DoubleTab& resu, DoubleTab& tab_flux_bords,
                                                        DoubleTab& nu,
                                                        const Zone_Cl_VEF& zone_Cl_VEF,
                                                        const Zone_VEF& zone_VEF,
                                                        int nb_comp) const
{
  Cerr << "Error in Op_Diff_VEF_Anisotrope_Face::ajouter_cas_vectoriel()" << finl;
  Cerr << "This case is not coded yet" << finl;
  Process::exit(-1);
}

void Op_Diff_VEF_Anisotrope_Face::ajouter_cas_multi_scalaire(const DoubleTab& inconnue,
                                                             DoubleTab& resu, DoubleTab& tab_flux_bords,
                                                             DoubleTab& nu,
                                                             const Zone_Cl_VEF& zone_Cl_VEF,
                                                             const Zone_VEF& zone_VEF,
                                                             int nb_comp) const
{
  const int nb_cols_nu = nu.dimension(1);
  assert(nb_cols_nu == dimension*dimension*nb_comp*nb_comp);

  const IntTab& elemfaces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int i0,j,num_face;
  int nb_faces = zone_VEF.nb_faces();
  int n_bord;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA,flux0;
  DoubleVect n(Objet_U::dimension);
  DoubleTrav Tgrad(Objet_U::dimension,Objet_U::dimension);

  // dimensionning and initializing flow balance tabulars
  tab_flux_bords.resize(zone_VEF.nb_faces_bord(),nb_comp);
  tab_flux_bords=0.;
  assert( nb_comp>1 );
  int nb_bords=zone_VEF.nb_front_Cl();
  int ind_face;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      //periodic case
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
                          for(int c1=0; c1<nb_comp; c1++)
                            {
                              for(int c2=0; c2<nb_comp; c2++)
                                {
                                  int diffusivity_index = c1*nb_comp + c2;
                                  int start_id = elem*nb_cols_nu+diffusivity_index*dimension*dimension;
                                  ArrOfDouble diffu_c1_c2_elem;
                                  diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                                  valA = viscA(num_face,j,elem,diffu_c1_c2_elem);

                                  resu(num_face,c1)+=valA*inconnue(j,c2);
                                  resu(num_face,c1)-=valA*inconnue(num_face,c2);
                                  if(j<nb_faces) // real face
                                    {
                                      resu(j,c1)+=0.5*valA*inconnue(num_face,c2);
                                      resu(j,c1)-=0.5*valA*inconnue(j,c2);
                                    }
                                }
                            }
                        }
                    }//loop over i0 (nb_faces) for elem1
                }//loop over kk
            } // ind face
        }// end periodic
      else
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              int elem=face_voisins(num_face,0);

              // Loop over faces :
              for (int i=0; i<nb_faces_elem; i++)
                if (( (j= elemfaces(elem,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                  {
                    for(int c1=0; c1<nb_comp; c1++)
                      {
                        for(int c2=0; c2<nb_comp; c2++)
                          {
                            int diffusivity_index = c1*nb_comp + c2;
                            int start_id = elem*nb_cols_nu+diffusivity_index*dimension*dimension;
                            ArrOfDouble diffu_c1_c2_elem;
                            diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                            valA = viscA(num_face,j,elem,diffu_c1_c2_elem);

                            if (ind_face<nb_faces_bord_reel)
                              {
                                double flux=valA*(inconnue(j,c2)-inconnue(num_face,c2));
                                resu(num_face,c1)+=flux;
                                tab_flux_bords(num_face,c1)+=flux;
                              }
                            if(j<nb_faces) // real faces
                              {
                                resu(j,c1)+=valA*inconnue(num_face,c2);
                                resu(j,c1)-=valA*inconnue(j,c2);
                              }
                          }// loop c2
                      }// loop c1
                  } // j
            }//end of ind_face loop
        }//else
    }//end for the boundaries loop

  // dealing now with internal faces


  for (num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
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
                  if(j>=nb_faces) // it is a virtual face
                    {
                      el1 = face_voisins(j,0);
                      el2 = face_voisins(j,1);
                      if((el1==-1)||(el2==-1))
                        contrib=0;
                    }
                  if(contrib)
                    {
                      for(int c1=0; c1<nb_comp; c1++)

                        for(int c2=0; c2<nb_comp; c2++)
                          {
                            int diffusivity_index = c1*nb_comp + c2;
                            int start_id = elem*nb_cols_nu+diffusivity_index*dimension*dimension;
                            ArrOfDouble diffu_c1_c2_elem;
                            diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                            valA = viscA(num_face,j,elem,diffu_c1_c2_elem);

                            resu(num_face,c1)+=valA*inconnue(j,c2);
                            resu(num_face,c1)-=valA*inconnue(num_face,c2);
                            if(j<nb_faces) // dealing with real faces
                              {
                                resu(j,c1)+=valA*inconnue(num_face,c2);
                                resu(j,c1)-=valA*inconnue(j,c2);
                              }
                            else
                              {
                                //the face is virtual
                              }
                          }
                    }
                }
            }//loop i0 over elem faces
        }//loop over k
    }//loop over faces


  // Neumann
  //On se base sur ce qui est fait pour le cas scalaire
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

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
                  flux0=la_cl_paroi.flux_impose(face-ndeb,nc)*zone_VEF.surface(face);
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
                  flux0=la_cl_paroi.h_imp(face-ndeb,nc)*(la_cl_paroi.T_ext(face-ndeb,nc)-inconnue(face,nc))*zone_VEF.surface(face);
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


DoubleTab& Op_Diff_VEF_Anisotrope_Face::ajouter(const DoubleTab& inconnue_org, DoubleTab& resu) const
{
  remplir_nu(nu_);
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  int nb_comp = 1;
  int nb_dim = resu.nb_dim();
  if(nb_dim==2)
    nb_comp=resu.dimension(1);
  DoubleTab nu;
  DoubleTab tab_inconnue;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  const DoubleTab& inconnue=modif_par_porosite_si_flag(inconnue_org,tab_inconnue,marq,porosite_face);

  const Champ_base& inco = equation().inconnue().valeur();
  const Nature_du_champ nature_champ = inco.nature_du_champ();
  if(nature_champ==scalaire)
    ajouter_cas_scalaire(inconnue, resu, flux_bords_, nu, zone_Cl_VEF, zone_VEF);
  else if (nature_champ==vectoriel)
    ajouter_cas_vectoriel(inconnue, resu, flux_bords_, nu, zone_Cl_VEF, zone_VEF,nb_comp);
  else if (nature_champ==multi_scalaire)
    ajouter_cas_multi_scalaire(inconnue, resu, flux_bords_, nu, zone_Cl_VEF, zone_VEF,nb_comp);
  modifier_flux(*this);

  return resu;
}

DoubleTab& Op_Diff_VEF_Anisotrope_Face::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}

void Op_Diff_VEF_Anisotrope_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice) const
{

  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int n1 = zone_VEF.nb_faces();
  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  DoubleTab nu;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  DoubleVect porosite_eventuelle(zone_VEF.porosite_face());
  if (!marq)
    porosite_eventuelle=1;


  if(nb_dim==2)
    nb_comp=transporte.dimension(1);

  int i,j,num_face;
  int elem1,elem2;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double val;

  int nb_bords=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
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
                      val = viscA(num_face,j,elem1,nu);
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
                        val= viscA(num_face,j,elem2,nu);
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
                      val = viscA(num_face,j,elem1,nu);
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
  int n0 = zone_VEF.premiere_face_int();
  for (num_face=n0; num_face<n1; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              val = viscA(num_face,j,elem1,nu);
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
                val= viscA(num_face,j,elem2,nu);
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
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

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
              matrice(face,face) += la_cl_paroi.h_imp(face-ndeb)*zone_VEF.face_surfaces(face);
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

void Op_Diff_VEF_Anisotrope_Face::ajouter_contribution_multi_scalaire(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int n1 = zone_VEF.nb_faces();
  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  DoubleTab nu;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  DoubleVect porosite_eventuelle(zone_VEF.porosite_face());
  if (!marq)
    porosite_eventuelle=1;


  if(nb_dim==2)
    nb_comp=transporte.dimension(1);

  const int nb_cols_nu = nu.dimension(1);
  assert(nb_cols_nu == dimension*dimension*nb_comp*nb_comp);

  int i,j,num_face;
  int elem1,elem2;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double val;

  int nb_bords=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
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
                      for(int c1=0; c1<nb_comp; c1++)
                        for(int c2=0; c2<nb_comp; c2++)
                          {

                            int diffusivity_index = c1*nb_comp + c2;
                            int start_id = elem1*nb_cols_nu+diffusivity_index*dimension*dimension;
                            ArrOfDouble diffu_c1_c2_elem;
                            diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                            val = viscA(num_face,j,elem1,diffu_c1_c2_elem);

                            int n0=num_face*nb_comp+c1;
                            int j0=j*nb_comp+c1;
                            int n0b=num_face*nb_comp+c2;
                            int j0b=j*nb_comp+c2;

                            matrice(n0,n0b)+=val*porosite_eventuelle(num_face);
                            matrice(n0,j0b)-=val*porosite_eventuelle(j);
                            matrice(j0,n0b)-=val*porosite_eventuelle(num_face);
                            matrice(j0,j0b)+=val*porosite_eventuelle(j);


                          }
                    }
                  if (elem2!=-1)
                    if ( (j=elem_faces(elem2,i)) > num_face )
                      {
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            for(int c1=0; c1<nb_comp; c1++)
                              for(int c2=0; c2<nb_comp; c2++)
                                {

                                  int diffusivity_index = c1*nb_comp + c2;
                                  int start_id = elem1*nb_cols_nu+diffusivity_index*dimension*dimension;
                                  ArrOfDouble diffu_c1_c2_elem;
                                  diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                                  val = viscA(num_face,j,elem1,diffu_c1_c2_elem);

                                  int n0=num_face*nb_comp+c1;
                                  int j0=j*nb_comp+c1;
                                  int n0b=num_face*nb_comp+c2;
                                  int j0b=j*nb_comp+c2;

                                  matrice(n0,n0b)+=val*porosite_eventuelle(num_face);
                                  matrice(n0,j0b)-=val*porosite_eventuelle(j);
                                  matrice(j0,n0b)-=val*porosite_eventuelle(num_face);
                                  matrice(j0,j0b)+=val*porosite_eventuelle(j);

                                  int  n0periob=fac_asso*nb_comp+c2;
                                  matrice(j0,n0periob)-=val*porosite_eventuelle(num_face);
                                }
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
                      for(int c1=0; c1<nb_comp; c1++)

                        for(int c2=0; c2<nb_comp; c2++)
                          {

                            int diffusivity_index = c1*nb_comp + c2;
                            int start_id = elem1*nb_cols_nu+diffusivity_index*dimension*dimension;
                            ArrOfDouble diffu_c1_c2_elem;
                            diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                            val = viscA(num_face,j,elem1,diffu_c1_c2_elem);

                            int n0=num_face*nb_comp+c1;
                            int j0=j*nb_comp+c1;
                            int n0b=num_face*nb_comp+c2;
                            int j0b=j*nb_comp+c2;

                            matrice(n0,n0b)+=val*porosite_eventuelle(num_face);
                            matrice(n0,j0b)-=val*porosite_eventuelle(j);
                            matrice(j0,n0b)-=val*porosite_eventuelle(num_face);
                            matrice(j0,j0b)+=val*porosite_eventuelle(j);

                          }
                    }
                }
            }
        }
    }
  for (num_face=zone_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              for(int c1=0; c1<nb_comp; c1++)
                for(int c2=0; c2<nb_comp; c2++)
                  {
                    int diffusivity_index = c1*nb_comp + c2;
                    int start_id = elem1*nb_cols_nu+diffusivity_index*dimension*dimension;
                    ArrOfDouble diffu_c1_c2_elem;
                    diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                    val = viscA(num_face,j,elem1,diffu_c1_c2_elem);

                    int n0=num_face*nb_comp+c1;
                    int j0=j*nb_comp+c1;
                    int n0b=num_face*nb_comp+c2;
                    int j0b=j*nb_comp+c2;

                    matrice(n0,n0b)+=val*porosite_eventuelle(num_face);
                    matrice(n0,j0b)-=val*porosite_eventuelle(j);
                    matrice(j0,n0b)-=val*porosite_eventuelle(num_face);
                    matrice(j0,j0b)+=val*porosite_eventuelle(j);


                  }
            }
          if (elem2!=-1)
            if ( (j=elem_faces(elem2,i)) > num_face )
              {
                for(int c1=0; c1<nb_comp; c1++)
                  for(int c2=0; c2<nb_comp; c2++)
                    {
                      int diffusivity_index = c1*nb_comp + c2;
                      int start_id = elem2*nb_cols_nu+diffusivity_index*dimension*dimension;
                      ArrOfDouble diffu_c1_c2_elem;
                      diffu_c1_c2_elem.ref_array(nu, start_id ,dimension*dimension);
                      val = viscA(num_face,j,elem2,diffu_c1_c2_elem);

                      int n0=num_face*nb_comp+c1;
                      int j0=j*nb_comp+c1;
                      int n0b=num_face*nb_comp+c2;
                      int j0b=j*nb_comp+c2;

                      matrice(n0,n0b)+=val*porosite_eventuelle(num_face);
                      matrice(n0,j0b)-=val*porosite_eventuelle(j);
                      matrice(j0,n0b)-=val*porosite_eventuelle(num_face);
                      matrice(j0,j0b)+=val*porosite_eventuelle(j);

                    }
              }
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
}

void Op_Diff_VEF_Anisotrope_Face::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  int nb_comp = resu.line_size();
  int nb_bords=zone_VEF.nb_front_Cl();

  // Partie imposee :
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nb_comp; comp++)
              resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*zone_VEF.surface(face);
        }
    }
}

void Op_Diff_VEF_Anisotrope_Face::verifier() const
{
  static int testee=0;
  if(testee)
    return;
  testee=1;
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  //  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  //  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();

  const DoubleTab& xv=zone_VEF.xv();
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
          if(dabs(resu(i,0))>1.e-10)
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
          if(dabs(2-resu(i,0))>1.e-10)
            {
              Cerr << " delta(y(1-y),0) ("<<i<<") = "
                   << resu(i,0);
              Cerr << finl;
            }
        }
    }
}
