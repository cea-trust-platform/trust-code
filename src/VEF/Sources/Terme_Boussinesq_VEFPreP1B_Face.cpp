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

#include <Terme_Boussinesq_VEFPreP1B_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Periodique.h>
#include <Domaine_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <Synonyme_info.h>


extern double calculer_coef_som(True_int elem, True_int dimension, True_int& nb_face_diri, True_int* indice_diri);

Implemente_instanciable(Terme_Boussinesq_VEFPreP1B_Face,"Boussinesq_VEFPreP1B_P1NC",Terme_Boussinesq_VEF_Face);
Add_synonym(Terme_Boussinesq_VEFPreP1B_Face,"Boussinesq_temperature_VEFPreP1B_P1NC");
Add_synonym(Terme_Boussinesq_VEFPreP1B_Face,"Boussinesq_concentration_VEFPreP1B_P1NC");

//// printOn
Sortie& Terme_Boussinesq_VEFPreP1B_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_VEFPreP1B_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

DoubleTab& Terme_Boussinesq_VEFPreP1B_Face::ajouter(DoubleTab& tab_resu) const
{
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_VEF.valeur());
  // Si seulement support P0 on appelle en VEF
  if (domaine_VEF.get_alphaE() && !domaine_VEF.get_alphaS() && !domaine_VEF.get_alphaA())
    return Terme_Boussinesq_VEF_Face::ajouter(tab_resu);

  // Verifie la validite de T0:
  check();

  int nbpts=-1; // nombre de points d'integration
  // static variable can't be used on kernel devices:
  int dim = Objet_U::dimension;
  assert(dim==2 || dim==3);
  if(dim==2)
    {
      nbpts=3; // ordre 2
    }
  else if(dim==3)
    {
      nbpts=4; // ordre 2
    }

  // On remplit les Poids et les coord_bary :
  double Poids = 1./(dim+1);
  DoubleTrav tab_coord_bary(nbpts,dim+1); // lambda_i des points
  if(dim==2)
    {
      tab_coord_bary(0,0)=0.5;
      tab_coord_bary(0,1)=0.;
      tab_coord_bary(0,2)=0.5;

      tab_coord_bary(1,0)=0.;
      tab_coord_bary(1,1)=0.5;
      tab_coord_bary(1,2)=0.5;

      tab_coord_bary(2,0)=0.5;
      tab_coord_bary(2,1)=0.5;
      tab_coord_bary(2,2)=0.;
    }
  else if(dim==3)
    {
      double a = 0.5854101966249685;
      double b = 0.1381966011250105;

      tab_coord_bary(0,0)=a;
      tab_coord_bary(0,1)=b;
      tab_coord_bary(0,2)=b;
      tab_coord_bary(0,3)=b;

      tab_coord_bary(1,0)=b;
      tab_coord_bary(1,1)=a;
      tab_coord_bary(1,2)=b;
      tab_coord_bary(1,3)=b;

      tab_coord_bary(2,1)=b;
      tab_coord_bary(2,0)=b;
      tab_coord_bary(2,2)=a;
      tab_coord_bary(2,3)=b;

      tab_coord_bary(3,2)=b;
      tab_coord_bary(3,0)=b;
      tab_coord_bary(3,1)=b;
      tab_coord_bary(3,3)=a;
    }

  const Champ_Inc& le_scalaire = equation_scalaire().inconnue();
  int nb_comp = le_scalaire->nb_comp(); // Vaut 1 si temperature, nb_constituents si concentration
  int nb_elem_tot = domaine_VEF.nb_elem_tot();
  // XXXTrav to not reallocate on the host/device each time:
  IntTrav tab_les_polygones(nb_elem_tot * nbpts);
  DoubleTrav tab_les_positions(nb_elem_tot * nbpts, dim);

  CDoubleTabView coord_bary = tab_coord_bary.view_ro();
  CIntTabView elem_sommets = domaine_VEF.domaine().les_elems().view_ro();
  CDoubleTabView coord_sommets = domaine_VEF.domaine().les_sommets().view_ro();
  IntArrView les_polygones = static_cast<ArrOfInt&>(tab_les_polygones).view_wo();
  DoubleTabView les_positions = tab_les_positions.view_wo();
  // Remplissage des tableaux de travail:
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       Kokkos::RangePolicy<>(0, nb_elem_tot), KOKKOS_LAMBDA(
                         const int elem)
  {
    for (int pt = 0; pt < nbpts; pt++)
      les_polygones(elem * nbpts  + pt) = elem;

    //On remplit la matrice de changement d'element  :
    double a0[3], a0a1[3], a0a2[3], a0a3[3];
    const int som_glob = elem_sommets(elem,0);
    for (int d=0; d<dim; d++)
      a0[d]=coord_sommets(som_glob,d);

    const int som_glob1 = elem_sommets(elem,1);
    for (int d=0; d<dim; d++)
      a0a1[d]=coord_sommets(som_glob1,d)-a0[d];

    const int som_glob2 = elem_sommets(elem,2);
    for (int d=0; d<dim; d++)
      a0a2[d]=coord_sommets(som_glob2,d)-a0[d];

    if(dim == 3)
      {
        const int som_glob3 = elem_sommets(elem,3);
        for (int d=0; d<dim; d++)
          a0a3[d]=coord_sommets(som_glob3,d)-a0[d];
      }

    //On remplit les_positions :
    for (int pt = 0; pt < nbpts; pt++)
      {
        int point = elem * nbpts  + pt;
        for (int d=0; d<dim; d++)
          if (dim == 2)
            les_positions(point, d) = a0[d]
                                      + coord_bary(pt, 1) * a0a1[d]
                                      + coord_bary(pt, 2) * a0a2[d];
          else if (dim == 3)
            les_positions(point, d) = a0[d]
                                      + coord_bary(pt, 1) * a0a1[d]
                                      + coord_bary(pt, 2) * a0a2[d]
                                      + coord_bary(pt, 3) * a0a3[d];
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  // XXXTrav to not reallocate on the host/device each time:
  DoubleTrav tab_valeurs_scalaire(nb_elem_tot * nbpts, nb_comp);
  DoubleTrav tab_valeurs_beta(nb_elem_tot * nbpts, nb_comp);

  // Calcul du terme source aux points d'integration :
  le_scalaire->valeur_aux_elems(tab_les_positions, tab_les_polygones, tab_valeurs_scalaire);
  beta()->valeur_aux_elems(tab_les_positions, tab_les_polygones, tab_valeurs_beta);

  // Extension possible des volumes de controle:
  //int modif_traitement_diri=( sub_type(Domaine_VEF,domaine_VEF) ? ref_cast(Domaine_VEF,domaine_VEF).get_modif_div_face_dirichlet() : 0);
  //modif_traitement_diri = 0; // Forcee a 0 car ne marche pas d'apres essais Ulrich&Thomas

  const Domaine_Cl_VEF& domaine_Cl_VEF = le_dom_Cl_VEF.valeur();

  CIntArrView rang_elem_non_std = domaine_VEF.rang_elem_non_std().view_ro();
  CIntArrView type_elem_Cl = domaine_Cl_VEF.type_elem_Cl().view_ro();
  CDoubleArrView g = static_cast<const DoubleVect&>(gravite().valeurs()).view_ro();
  CDoubleArrView porosite_surf = equation().milieu().porosite_face().view_ro();
  CIntTabView elem_faces = domaine_VEF.elem_faces().view_ro();
  CDoubleArrView volumes = domaine_VEF.volumes().view_ro();
  CDoubleTabView valeurs_scalaire = tab_valeurs_scalaire.view_ro();
  CDoubleTabView valeurs_beta = tab_valeurs_beta.view_ro();
  CDoubleArrView Scalaire0 = getScalaire0().view_ro();
  // indice_diri
  DoubleTabView resu = tab_resu.view_rw();
  // Boucle sur les elements:
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       Kokkos::RangePolicy<>(0, nb_elem_tot), KOKKOS_LAMBDA(
                         const int elem)
  {
    /*
        True_int nb_face_diri = 0;
        True_int indice_diri[4];
        if (modif_traitement_diri)
          {
            True_int rang_elem = (True_int) rang_elem_non_std(elem);
            True_int type_elem = rang_elem < 0 ? 0 : (True_int) type_elem_Cl(rang_elem);
            calculer_coef_som(type_elem, dim, nb_face_diri, indice_diri);
          } */

    double volume=volumes(elem);

    // Boucle sur les faces de l'element:
    for(int face=0; face<=dim; face++)
      {
        int num_face=elem_faces(elem, face);
        double valeurs_Psi[4];
        // Integration sur les nbpts points:
        for (int pt=0; pt<nbpts; pt++)
          {
            int point = elem * nbpts + pt;

            valeurs_Psi[pt]=(1-dim*coord_bary(pt,face))*porosite_surf(num_face);

            for (int d=0; d<dim; d++)
              {
                double contrib=0;
                for (int comp=0; comp<nb_comp; comp++)
                  contrib+=Poids*volume*(Scalaire0(comp)-valeurs_scalaire(point,comp))*valeurs_beta(point,comp)*g(d)*valeurs_Psi[pt];
                Kokkos::atomic_add(&resu(num_face,d), contrib);
                /*
                                if (modif_traitement_diri)
                                  {
                                    for (int fdiri=0; fdiri<nb_face_diri; fdiri++)
                                      {
                                        int indice=indice_diri[fdiri];
                                        int facel = elem_faces(elem,indice);
                                        if (num_face==facel)
                                          {
                                            Kokkos::atomic_sub(&resu(facel,d), contrib);
                                            double contrib2=contrib/(dim+1-nb_face_diri);
                                            for (int f2=0; f2<dim+1; f2++)
                                              {
                                                int face2=elem_faces(elem,f2);
                                                Kokkos::atomic_add(&resu(face2,d), contrib2);
                                              }
                                          }
                                      }
                                  }
                */
              }
          }
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  // modif pour periodique
  start_gpu_timer();
  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nb_faces_bord=le_bord.nb_faces();
          CIntArrView le_bord_num_face = le_bord.num_face().view_ro();
          CIntArrView la_cl_perio_face_associee = la_cl_perio.face_associee().view_ro();
          Kokkos::parallel_for(__KERNEL_NAME__,
                               Kokkos::RangePolicy<>(0, nb_faces_bord/2), KOKKOS_LAMBDA(
                                 const int ind_face)
          {
            int face = le_bord_num_face(ind_face);
            int face_associee = le_bord_num_face(la_cl_perio_face_associee(ind_face));
            for (int d=0; d<dim; d++)
              {
                resu(face_associee, d) += resu(face, d);
                resu(face, d) = resu(face_associee, d);
              }
          });// for face
        }// sub_type Perio
    }
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
  return tab_resu;
}
