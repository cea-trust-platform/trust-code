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

#include <Paroi_negligeable_VEF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Modele_turbulence_hyd_base.h>
#include <Operateur_base.h>
#include <Champ_Uniforme.h>
#include <distances_VEF.h>
#include <Milieu_base.h>
#include <Fluide_base.h>
#include <Champ_P1NC.h>
#include <TRUSTTrav.h>
#include <Operateur.h>

Implemente_instanciable(Paroi_negligeable_VEF, "negligeable_VEF", Paroi_hyd_base_VEF);

Sortie& Paroi_negligeable_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Paroi_negligeable_VEF::readOn(Entree& s)
{
  return s;
}

int Paroi_negligeable_VEF::init_lois_paroi()
{
  init_lois_paroi_();

  // Dimensionnement du tableau elem_paroi
  int num_cl, fac, i;
  const Conds_lim& les_cl = le_dom_Cl_VEF->les_conditions_limites();
  //  const IntTab& elem_faces = le_dom_VEF->elem_faces();
  const IntTab& face_voisins = le_dom_VEF->face_voisins();

  DoubleTrav Verif_elem_double(le_dom_VEF->nb_elem());
  compteur_elem_paroi = 0;
  Verif_elem_double = 0;
  elem_paroi.resize(le_dom_VEF->nb_faces_bord());
  elem_paroi_double.resize(le_dom_VEF->nb_faces_bord());

  for (num_cl = 0; num_cl < les_cl.size(); num_cl++)
    {
      const Cond_lim& la_cl = les_cl[num_cl];
      const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();
      int num1, verif;

      if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
        {
          for (fac = ndeb; fac < nfin; fac++)
            {
              num1 = face_voisins(fac, 0);
              if (num1 == -1)
                num1 = face_voisins(fac, 1);
              if (Verif_elem_double[num1] == 0)
                {
                  elem_paroi[compteur_elem_paroi] = num1;
                  elem_paroi_double[fac] = compteur_elem_paroi;
                  Verif_elem_double[num1]++;
                  compteur_elem_paroi++;
                }
              else
                {
                  verif = 0;
                  for (i = 0; i < compteur_elem_paroi; i++)
                    {
                      if (elem_paroi[i] == num1)
                        {
                          elem_paroi_double[fac] = i;
                          verif++;
                        }
                    }
                  if (verif == 0)
                    {
                      Cerr << "Il y a un gros pbl dans la detremination de elem_paroi_double" << finl;
                    }
                }
            }
        }
    }
  elem_paroi.resize(compteur_elem_paroi);  // On a tous les elts qui touchent la paroi par une face
  // PBL : est ce qu on a plusieurs fois le meme???? -> Non car Verif_elem_double!!
  return 1;
}

int Paroi_negligeable_VEF::calculer_hyd_BiK(DoubleTab& tab_k, DoubleTab& tab_eps)
{
  return calculer_hyd(tab_k); // the value in argument is not used anyway
}

int Paroi_negligeable_VEF::calculer_hyd(DoubleTab& tab_k_eps)
{

  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  if (sub_type(Fluide_base, eqn_hydr.milieu()))
    {

      int ndeb, nfin, elem, l_unif;
      double norm_tau, u_etoile, norm_v = 0, dist = 0, val1, val2, val3, d_visco, visco = 1.;
      IntVect num(dimension);

      const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
      const IntTab& face_voisins = domaine_VEF.face_voisins();
      const IntTab& elem_faces = domaine_VEF.elem_faces();
      const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
      const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
      const DoubleTab& tab_visco = ch_visco_cin->valeurs();
      const DoubleTab& vit = eqn_hydr.inconnue().valeurs();

      if (sub_type(Champ_Uniforme, ch_visco_cin.valeur()))
        {
          if (tab_visco(0, 0) > DMINFLOAT)
            visco = tab_visco(0, 0);
          else
            visco = DMINFLOAT;
          l_unif = 1;
        }
      else
        l_unif = 0;
      if ((!l_unif) && (tab_visco.local_min_vect() < DMINFLOAT))
        {
          Cerr << " visco <=0 ?" << finl;
          exit();
        }

      for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl_VEF->les_conditions_limites(n_bord);

          if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              ToDo_Kokkos("To avoid an expensive copy D2H of array velocity.");
              for (int num_face = ndeb; num_face < nfin; num_face++)
                {
                  elem = face_voisins(num_face, 0);
                  if (dimension == 2)
                    {

                      num[0] = elem_faces(elem, 0);
                      num[1] = elem_faces(elem, 1);

                      if (num[0] == num_face)
                        num[0] = elem_faces(elem, 2);
                      else if (num[1] == num_face)
                        num[1] = elem_faces(elem, 2);

                      dist = distance_2D(num_face, elem, domaine_VEF);
                      dist *= 3. / 2.; // pour se ramener a distance paroi / milieu de num[0]-num[1]
                      //norm_v=norm_2D_vit1_lp(vit,num_face,num[0],num[1],domaine_VEF,val1,val2);
                      norm_v = norm_2D_vit1(vit, num[0], num[1], num_face, domaine_VEF, val1, val2);

                    } // dim 2
                  else if (dimension == 3)
                    {

                      num[0] = elem_faces(elem, 0);
                      num[1] = elem_faces(elem, 1);
                      num[2] = elem_faces(elem, 2);

                      if (num[0] == num_face)
                        num[0] = elem_faces(elem, 3);
                      else if (num[1] == num_face)
                        num[1] = elem_faces(elem, 3);
                      else if (num[2] == num_face)
                        num[2] = elem_faces(elem, 3);

                      dist = distance_3D(num_face, elem, domaine_VEF);
                      dist *= 4. / 3.; // pour se ramener a distance paroi / milieu de num[0]-num[1]-num[2]
                      //norm_v=norm_3D_vit1_lp(vit, num_face, num[0], num[1], num[2], domaine_VEF, val1, val2, val3);
                      norm_v = norm_3D_vit1(vit, num_face, num[0], num[1], num[2], domaine_VEF, val1, val2, val3);

                    }                      // dim 3

                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  norm_tau = d_visco * norm_v / dist;
                  u_etoile = sqrt(norm_tau);
                  tab_u_star_(num_face) = u_etoile;

                }                      // loop on faces

            }                      // Fin de paroi fixe

        }                      // Fin boucle sur les bords

    }

  return 1;
}

// C est celle la qui nous interesse!!!
int Paroi_negligeable_VEF::calculer_hyd(DoubleTab& tab_nu_t, DoubleTab& tab_k)
{
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  if (sub_type(Fluide_base, eqn_hydr.milieu()))
    {
      const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
      const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
      const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
      const DoubleTab& tab_visco = ch_visco_cin->valeurs();

      double visco0;
      bool l_unif;
      if (sub_type(Champ_Uniforme, ch_visco_cin.valeur()))
        {
          if (tab_visco(0, 0) > DMINFLOAT)
            visco0 = tab_visco(0, 0);
          else
            visco0 = DMINFLOAT;
          l_unif = true;
        }
      else
        {
          visco0 = -1;
          l_unif = false;
        }
      if ((!l_unif) && (tab_visco.local_min_vect() < DMINFLOAT))
        {
          Cerr << " visco <=0 ?" << finl;
          exit();
        }

      for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl_VEF->les_conditions_limites(n_bord);
          if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              int dim = Objet_U::dimension;
              int nfac = domaine_VEF.domaine().nb_faces_elem();
              CDoubleTabView xp = domaine_VEF.xp().view_ro();
              CDoubleTabView xv = domaine_VEF.xv().view_ro();
              CDoubleTabView face_normale = domaine_VEF.face_normales().view_ro();
              CIntTabView elem_faces = domaine_VEF.elem_faces().view_ro();
              CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
              CDoubleTabView vit = eqn_hydr.inconnue().valeurs().view_ro();
              CDoubleArrView visco = static_cast<const DoubleVect&>(tab_visco).view_ro();
              DoubleArrView tab_u_star = tab_u_star_.view_rw();
              Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA (const int num_face)
              {
                int num[3] {};
                double val[3] {};
                int elem = face_voisins(num_face, 0);
                int fac = 0;
                for (int i = 0; i < dim; i++)
                  {
                    if (elem_faces(elem, fac) == num_face) fac++;
                    num[i] = elem_faces(elem, fac);
                    fac++;
                  }
                double dist = distance(dim, num_face, elem, xp, xv, face_normale);
                dist *= (dim + 1.) / dim;                      // pour se ramener a distance paroi / milieu de num[0]-num[1]
                double norm_v = norm_vit1(dim, vit, num_face, nfac, num, face_normale, val);
                double d_visco = l_unif ? visco0 : visco[elem];
                double norm_tau = d_visco * norm_v / dist;
                double u_etoile = sqrt(norm_tau);
                tab_u_star(num_face) = u_etoile;
              });          // loop on faces
              end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
            }                      // Fin de paroi fixe
        }                      // Fin boucle sur les bords
    }
  return 1;
}

int Paroi_negligeable_VEF::calculer_scal(Champ_Fonc_base&)
{
  return 1;
}

bool Paroi_negligeable_VEF::use_shear() const
{
  return false;
}
