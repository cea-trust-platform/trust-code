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

#include <Op_Conv_VEF_base.h>

#include <Milieu_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <TRUSTTrav.h>
#include <Discretisation_base.h>
#include <Champ.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Dirichlet_homogene.h>
#include <Periodique.h>

Implemente_base(Op_Conv_VEF_base,"Op_Conv_VEF_base",Operateur_Conv_base);

Sortie& Op_Conv_VEF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Conv_VEF_base::readOn(Entree& s )
{
  return s ;
}


/*! @brief definit si l'on convecte psi avec phi*u ou avec u
 *
 */
int  Op_Conv_VEF_base::phi_u_transportant(const Equation_base& eq) const
{
  if (eq.inconnue().le_nom()=="vitesse")
    return 0;
  return 1;
}


void Op_Conv_VEF_base::associer_vitesse(const Champ_base& vit)
{
  vitesse_ = ref_cast(Champ_Inc_base,vit);
}


void Op_Conv_VEF_base::abortTimeStep()
{
  Operateur_Conv_base::abortTimeStep();
}

double Op_Conv_VEF_base::calculer_dt_stab() const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  remplir_fluent();
  if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(fluent_,equation().milieu());

  // Remplissage de faces_entrelaces_Cl_ qui contient les faces de bord non Dirichlet et les faces internes non std pour lequelles on utilise le volumes_entrelaces_Cl
  // Ce tableau temporaire a ete cree pour fusionner plusieurs kernels Kokkos en un seul
  if (faces_entrelaces_Cl_.size_array()==0)
    {
      faces_entrelaces_Cl_.resize(domaine_VEF.premiere_face_std());
      int ind_face=-1;
      // On traite les conditions aux limites
      // Si une face porte une condition de Dirichlet on n'en tient pas compte
      // dans le calcul de dt_stab
      for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
          if (!sub_type(Dirichlet, la_cl.valeur()) && !sub_type(Dirichlet_homogene, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int num_face = ndeb; num_face < nfin; num_face++)
                faces_entrelaces_Cl_(++ind_face) = num_face;
            }
        }
      // Faces internes non std:
      int ndeb = domaine_VEF.premiere_face_int();
      int nfin = domaine_VEF.premiere_face_std();
      for (int num_face = ndeb; num_face < nfin; num_face++)
        faces_entrelaces_Cl_(++ind_face) = num_face;
      faces_entrelaces_Cl_.resize(ind_face+1);
    }

  double dt_stab =1.e30;
  CIntArrView faces_entrelaces_Cl = faces_entrelaces_Cl_.view_ro();
  CDoubleArrView fluent = fluent_.view_ro();
  CDoubleArrView volumes_entrelaces_Cl = domaine_Cl_VEF.volumes_entrelaces_Cl().view_ro();
  start_gpu_timer(__KERNEL_NAME__);
  Kokkos::parallel_reduce(__KERNEL_NAME__,
                          Kokkos::RangePolicy<>(0, faces_entrelaces_Cl_.size_array()), KOKKOS_LAMBDA(
                            const int ind_face, double& dtstab)
  {
    int num_face = faces_entrelaces_Cl(ind_face);
    double dt_face = volumes_entrelaces_Cl(num_face)/(fluent[num_face]+DMINFLOAT);
    if (dt_face < dtstab) dtstab = dt_face;
  }, Kokkos::Min<double>(dt_stab));
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  // On traite les faces internes standard
  // ToDo Kokkos
  const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();
  int ndeb = domaine_VEF.premiere_face_std();
  int nfin = domaine_VEF.nb_faces();

  bool kernelOnDevice = fluent_.checkDataOnDevice();
  const double* fluent_addr = mapToDevice(fluent_, "", kernelOnDevice);
  const double* volumes_entrelaces_addr = mapToDevice(volumes_entrelaces, "", kernelOnDevice);
  // ToDo bug nvc++ compiler recent bouh
  start_gpu_timer();
  if (kernelOnDevice)
    {
      #pragma omp target teams distribute parallel for reduction(min:dt_stab)
      for (int num_face = ndeb; num_face < nfin; num_face++)
        {
          double dt_face = volumes_entrelaces_addr[num_face] / (fluent_addr[num_face] + DMINFLOAT);
          dt_stab = (dt_face < dt_stab) ? dt_face : dt_stab;
        }
    }
  else
    {
      for (int num_face = ndeb; num_face < nfin; num_face++)
        {
          double dt_face = volumes_entrelaces_addr[num_face] / (fluent_addr[num_face] + DMINFLOAT);
          dt_stab = (dt_face < dt_stab) ? dt_face : dt_stab;
        }
    }
  end_gpu_timer(kernelOnDevice, "Face loop in Op_Conv_VEF_base::calculer_dt_stab()");
  dt_stab = Process::mp_min(dt_stab);
  // astuce pour contourner le type const de la methode
  Op_Conv_VEF_base& op = ref_cast_non_const(Op_Conv_VEF_base,*this);
  op.fixer_dt_stab_conv(dt_stab);
  if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
    multiplier_par_rho_si_dilatable(fluent_,equation().milieu());

  return dt_stab;
}

// cf Op_Conv_VEF_base::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Conv_VEF_base::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = 1.e30;

      if ((le_dom_vef.non_nul()) && (la_zcl_vef.non_nul()))
        {
          const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
          const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
          const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();
          const DoubleVect& volumes_entrelaces_Cl = domaine_Cl_VEF.volumes_entrelaces_Cl();
          double dt_face;
          remplir_fluent();
          if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
            diviser_par_rho_si_dilatable(fluent_,equation().milieu());

          // On traite les conditions aux limites
          // Si une face porte une condition de Dirichlet on n'en tient pas compte
          // dans le calcul de dt_stab
          for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
              if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur())))
                { /* Do nothing */}
              else
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
                  int ndeb = le_bord.num_premiere_face();
                  int nfin = ndeb + le_bord.nb_faces();
                  for (int num_face=ndeb; num_face<nfin; num_face++)
                    {
                      dt_face = volumes_entrelaces_Cl(num_face)/(fluent_[num_face]+1.e-30);
                      es_valeurs(num_face) = dt_face;
                    }
                }
            }

          // On traite les faces internes non standard
          int ndeb = domaine_VEF.premiere_face_int();
          int nfin = domaine_VEF.premiere_face_std();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              dt_face = volumes_entrelaces_Cl(num_face)/(fluent_[num_face]+1.e-30);
              es_valeurs(num_face) = dt_face;
            }

          // On traite les faces internes standard
          ndeb = nfin;
          nfin = domaine_VEF.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              dt_face = volumes_entrelaces(num_face)/(fluent_[num_face]+1.e-30);
              es_valeurs(num_face) = dt_face;
            }
          if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
            multiplier_par_rho_si_dilatable(fluent_,equation().milieu());
        }
    }
  else
    Operateur_Conv_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Conv_VEF_base::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite")
    loc = "face";
  else
    return Operateur_Conv_base::get_localisation_pour_post(option);
  return loc;
}
void Op_Conv_VEF_base::associer_domaine_cl_dis(const Domaine_Cl_dis_base& domaine_cl_dis)
{
  const Domaine_Cl_VEF& zclvef = ref_cast(Domaine_Cl_VEF,domaine_cl_dis);
  la_zcl_vef = zclvef;
}

void Op_Conv_VEF_base::associer(const Domaine_dis_base& domaine_dis,
                                const Domaine_Cl_dis_base& domaine_cl_dis,
                                const Champ_Inc_base& )
{
  const Domaine_VEF& zvef = ref_cast(Domaine_VEF,domaine_dis);
  const Domaine_Cl_VEF& zclvef = ref_cast(Domaine_Cl_VEF,domaine_cl_dis);

  le_dom_vef = zvef;
  la_zcl_vef = zclvef;
  //******************************************************************************
  // Initialisation des jetons pour l'alternance (kamoulox !)

  //******************************************************************************
  Cerr << "Initialisation de la roue pour la permutation des schemas de convection" << finl;
  roue= -1;
  //  roue2=-1;

  le_dom_vef->creer_tableau_faces(fluent_);
}

int Op_Conv_VEF_base::impr(Sortie& os) const
{
  return Op_VEF_Face::impr(os, *this );
}

DoubleTab& Op_Conv_VEF_base::calculer(const DoubleTab& transporte,
                                      DoubleTab& resu) const
{
  resu = 0;
  return ajouter(transporte,resu);
}
void Op_Conv_VEF_base::remplir_fluent() const
{
  // Remplissage du tableau fluent par appel a ajouter
  // C'est cher mais au moins cela corrige (en attendant
  // d'optimiser) le probleme d'un pas de temps de convection
  // calcule avec des vitesses du passe
  DoubleTrav tmp(equation().inconnue().valeurs());
  DoubleTab flux_bords_sauve(flux_bords_);  // On sauve les flux_bords car sinon mis a 0
  ajouter(tmp,tmp);
  flux_bords_=flux_bords_sauve;
  // PL: C'est vraiment lourd, mais comment faire? fluent est dependant
  // du schema et donc on peut coder quelque chose comme fluent=vitesse*surface*porosite
  // dans cette presente methode
}


// Calculation of local time: Vect of size number of faces of the domain
// This is the equivalent of "Op_Conv_VEF_base :: calculer_dt_stab ()"
void Op_Conv_VEF_base::calculer_dt_local(DoubleTab& dt_face) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const DoubleVect& volumes_entrelaces =  domaine_VEF.volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = domaine_Cl_VEF.volumes_entrelaces_Cl();

  int nb_faces= domaine_VEF.nb_faces();
  dt_face=(volumes_entrelaces);
  remplir_fluent();

  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (int num_face=ndeb; num_face<nfin; num_face++)
        {
          if( sup_strict(fluent_[num_face], 1.e-30) )
            dt_face(num_face)= volumes_entrelaces_Cl(num_face)/fluent_[num_face];
          else
            dt_face(num_face) = -1.;
        }
    }

  //Non-standard internal faces
  int ndeb = domaine_VEF.premiere_face_int();
  int nfin = domaine_VEF.premiere_face_std();

  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      if( sup_strict(fluent_[num_face], 1.e-30) )
        dt_face(num_face)= volumes_entrelaces(num_face)/fluent_[num_face];
      else
        dt_face(num_face) = -1.;
    }

  //The standard internal faces
  ndeb = nfin;
  nfin = domaine_VEF.nb_faces();
  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      if( sup_strict(fluent_[num_face], 1.e-30) )
        dt_face(num_face)= volumes_entrelaces(num_face)/fluent_[num_face];
      else
        dt_face(num_face) = -1.;
    }

  double max_dt_local= dt_face.mp_max_abs_vect();
  for(int i=0; i<nb_faces; i++)
    {
      if(! sup_strict(dt_face(i), 1.e-16))
        dt_face(i) = max_dt_local;
    }
  dt_face.echange_espace_virtuel();

  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int nb_faces_bord=le_bord.nb_faces();
          for (int ind_face=0; ind_face<nb_faces_bord; ind_face++)
            {
              int ind_face_associee = la_cl_perio.face_associee(ind_face);
              int face = le_bord.num_face(ind_face);
              int face_associee = le_bord.num_face(ind_face_associee);
              if (!est_egal(dt_face(face),dt_face(face_associee),1.e-8))
                {
                  dt_face(face) = std::min(dt_face(face),dt_face(face_associee));
                }
            }
        }
    }
  dt_face.echange_espace_virtuel();

// dt_conv_locaux=dt_face;
}
