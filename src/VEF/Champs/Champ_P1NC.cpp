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

#include <Dirichlet_paroi_defilante.h>
#include <Echange_externe_impose.h>
#include <Scalaire_impose_paroi.h>
#include <Dirichlet_paroi_fixe.h>
#include <Modele_turbulence_hyd_base.h>
#include <Schema_Temps_base.h>
#include <Neumann_homogene.h>
#include <Operateur_base.h>
#include <Champ_Uniforme.h>
#include <Neumann_paroi.h>
#include <distances_VEF.h>
#include <Probleme_base.h>
#include <Fluide_base.h>
#include <Periodique.h>
#include <Champ_P1NC.h>
#include <Operateur.h>
#include <TRUSTTab.h>
#include <TRUSTTrav.h>
#include <SFichier.h>
#include <Device.h>
#include <kokkos++.h>
#include <TRUSTArray_kokkos.tpp>

Implemente_instanciable(Champ_P1NC,"Champ_P1NC",Champ_Inc_base);

Sortie& Champ_P1NC::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_P1NC::readOn(Entree& s)
{
  lire_donnees(s);
  return s;
}

// PQ : 01/10/04 : mettre_a_jour permet de calculer qu'une seule fois le champ conforme u_bar (defini aux sommets)
//                   pour pouvoir etre utilise dans les differents operateurs ou lors de l'appel
//                   des sondes definies a partir des valeurs "smooth" (dans le cas present : u_bar)
void Champ_P1NC::mettre_a_jour(double un_temps)
{
  Champ_Inc_base::mettre_a_jour(un_temps);
  filtrer_L2_deja_appele_ = 0;
}
/*! @brief
 *
 */
void Champ_P1NC::abortTimeStep()
{
  ch_som_ = 0;
}

int Champ_P1NC::compo_normale_sortante(int num_face) const
{
  double vit_norm = 0;
  for (int ncomp = 0; ncomp < nb_comp(); ncomp++)
    vit_norm += valeurs()(num_face, ncomp) * domaine_vef().face_normales(num_face, ncomp);
  return (vit_norm > 0);
}

DoubleTab& Champ_P1NC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps, int distant) const
{
  return Champ_P1NC_implementation::trace(fr, valeurs(tps), x, distant);
}

void Champ_P1NC::cal_rot_ordre1(DoubleTab& tab_vorticite) const
{
  const int nb_elem = domaine_vef().nb_elem(), nb_elem_tot = domaine_vef().nb_elem_tot();
  DoubleTrav tab_gradient_elem(nb_elem_tot, dimension, dimension);
  gradient(tab_gradient_elem);
  CDoubleTabView3 gradient_elem = tab_gradient_elem.view_ro<3>();
  switch(dimension)
    {
    case 2:
      {
        DoubleArrView vorticite = static_cast<DoubleVect&>(tab_vorticite).view_wo();
        Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(const int num_elem)
        {
          vorticite(num_elem) = gradient_elem(num_elem, 1, 0) - gradient_elem(num_elem, 0, 1);
        });
      }
      break;
    case 3:
      {
        DoubleTabView vorticite = tab_vorticite.view_wo();
        Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(const int num_elem)
        {
          vorticite(num_elem, 0) = gradient_elem(num_elem, 2, 1) - gradient_elem(num_elem, 1, 2);
          vorticite(num_elem, 1) = gradient_elem(num_elem, 0, 2) - gradient_elem(num_elem, 2, 0);
          vorticite(num_elem, 2) = gradient_elem(num_elem, 1, 0) - gradient_elem(num_elem, 0, 1);
        });
      }
    }
  end_gpu_timer(__KERNEL_NAME__);
  tab_vorticite.echange_espace_virtuel();
  return;
}

void calculer_gradientP1NC_2D(const DoubleTab& tab_variable, const Domaine_VEF& domaine_VEF, const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& tab_gradient_elem)
{
  const DoubleTab& tab_face_normales = domaine_VEF.face_normales();
  const IntTab& tab_face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& tab_inverse_volumes = domaine_VEF.inverse_volumes();
  const ArrOfInt& tab_est_face_bord = domaine_VEF.est_face_bord();

  CDoubleTabView face_normales = tab_face_normales.view_ro();
  CIntTabView face_voisins = tab_face_voisins.view_ro();
  CDoubleArrView inverse_volumes = tab_inverse_volumes.view_ro();
  CDoubleArrView variable = static_cast<const ArrOfDouble&>(tab_variable).view_ro();
  CIntArrView est_face_bord = tab_est_face_bord.view_ro();
  DoubleTabView gradient_elem = tab_gradient_elem.view_rw();

  int dimension = Objet_U::dimension;
  const int nb_faces_tot = domaine_VEF.nb_faces_tot();
  const int nb_elem = domaine_VEF.nb_elem_tot();

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       nb_faces_tot,
                       KOKKOS_LAMBDA (int fac)
  {
    int type_face = est_face_bord(fac);

    // Faces de bord periodiques
    if (type_face == 2)
      {
        int elem1 = face_voisins(fac, 0);
        int elem2 = face_voisins(fac, 1);

        for (int i = 0; i < dimension; i++)
          {
            double grad = 0.5 * face_normales(fac,i) * variable(fac);
            Kokkos::atomic_add(&gradient_elem(elem1, i), +grad);
            Kokkos::atomic_add(&gradient_elem(elem2, i), -grad);
          }
      }
    // Faces de bord non periodiques
    else if (type_face == 1)
      {
        int elem1 = face_voisins(fac,0);

        for (int i = 0; i < dimension; i++)
          {
            double grad = face_normales(fac,i) * variable(fac);
            Kokkos::atomic_add(&gradient_elem(elem1, i), grad);
          }
      }
    // Faces internes
    else if (type_face == 0)
      {
        int elem1 = face_voisins(fac, 0);
        int elem2 = face_voisins(fac, 1);

        for (int i = 0; i < dimension; i++)
          {
            double grad = face_normales(fac,i) * variable(fac);
            if (elem1 >= 0) Kokkos::atomic_add(&gradient_elem(elem1, i), +grad);
            if (elem2 >= 0) Kokkos::atomic_add(&gradient_elem(elem2, i), -grad);
          }
      }
  });
  end_gpu_timer(__KERNEL_NAME__);

  // Division par le volume de l'element
  auto vol_div = KOKKOS_LAMBDA (int elem, int i)
  {
    gradient_elem(elem, i) *= inverse_volumes(elem);
  };
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_2D({0,0}, {nb_elem,dimension}), vol_div);
  end_gpu_timer(__KERNEL_NAME__);
}

void calculer_gradientP1NC_3D(const DoubleTab& tab_variable, const Domaine_VEF& domaine_VEF, const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& tab_gradient_elem)
{
  const DoubleTab& tab_face_normales = domaine_VEF.face_normales();
  const IntTab& tab_face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& tab_inverse_volumes = domaine_VEF.inverse_volumes();
  const ArrOfInt& tab_est_face_bord = domaine_VEF.est_face_bord();

  CDoubleTabView face_normales = tab_face_normales.view_ro();
  CIntTabView face_voisins = tab_face_voisins.view_ro();
  CDoubleArrView inverse_volumes = tab_inverse_volumes.view_ro();
  CDoubleTabView variable = tab_variable.view_ro();
  CIntArrView est_face_bord = tab_est_face_bord.view_ro();
  DoubleTabView3 gradient_elem = tab_gradient_elem.view_rw<3>();

  const int nb_faces_tot = domaine_VEF.nb_faces_tot();
  const int nb_elem = domaine_VEF.nb_elem_tot();

  int dimension = Objet_U::dimension;
  const int nb_comp = tab_variable.line_size();

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       nb_faces_tot,
                       KOKKOS_LAMBDA (int fac)
  {
    int type_face = est_face_bord(fac);
    // type_face 2 periodique
    double coef = (type_face == 2 ? 0.5 : 1);
    for (int j=0; j<2; j++)
      {
        int elem = face_voisins(fac, j);
        if (elem >= 0)
          {
            for (int icomp = 0; icomp < nb_comp; icomp++)
              for (int i = 0; i < dimension; i++)
                {
                  double grad = coef * face_normales(fac, i) * variable(fac, icomp);
                  Kokkos::atomic_add(&gradient_elem(elem, icomp, i), grad);
                }
            coef*=-1;
          }
      }
  });
  end_gpu_timer(__KERNEL_NAME__);

  // Division par le volume de l'element
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       range_3D({0,0,0}, {nb_elem,nb_comp,dimension}),
                       KOKKOS_LAMBDA (int elem, int icomp, int i)
  {
    gradient_elem(elem, icomp, i) *= inverse_volumes(elem);
  });
  end_gpu_timer(__KERNEL_NAME__);
}

void calculer_gradientP1NC(const DoubleTab& tab_variable, const Domaine_VEF& domaine_VEF, const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& tab_gradient_elem)
{
  const int nb_comp = tab_variable.line_size();
  tab_gradient_elem = 0.;

  // Cas du calcul du gradient d'un tableau de vecteurs ou
  // tableau de scalaires tab_gradient_elem(,,)
  if (nb_comp != 1 || tab_gradient_elem.nb_dim() == 3)
    calculer_gradientP1NC_3D(tab_variable, domaine_VEF, domaine_Cl_VEF, tab_gradient_elem);
  // Cas du calcul du gradient d'un tableau de scalaire dans un
  // tableau gradient_elem(,)
  else
    calculer_gradientP1NC_2D(tab_variable, domaine_VEF, domaine_Cl_VEF, tab_gradient_elem);
}

void Champ_P1NC::gradient(DoubleTab& gradient_elem) const
{
  // Calcul du gradient: par exemple gradient de la vitesse pour le calcul de la vorticite
  const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF, equation().domaine_Cl_dis());
  calculer_gradientP1NC(valeurs(), domaine_vef(), domaine_Cl_VEF, gradient_elem);

}

int Champ_P1NC::imprime(Sortie& os, int ncomp) const
{
  imprime_P1NC(os, ncomp);
  return 1;
}

Champ_base& Champ_P1NC::affecter_(const Champ_base& ch)
{
#ifdef CLEMENT
  DoubleTab& tab = valeurs();
  DoubleTab noeuds;
  remplir_coord_noeuds(noeuds);
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const int nb_faces = domaine_VEF.nb_faces();
  const IntTab& face_sommets = domaine_VEF.face_sommets();
  const DoubleTab& sommets=domaine().coord_sommets();
  int nb_som=sommets.dimension(0);
  Cerr << "Affecter Clement : ... " << finl;
  DoubleTab valf(valeurs());
  ch.valeur_aux(noeuds, valf);
  DoubleTab vals(nb_som, nb_compo_);
  if(nb_compo_==1)
    {
      vals.resize(nb_som);
    }
  ch.valeur_aux(sommets, vals);
  for(int face=0; face<nb_faces; face++)
    {
      for(int comp=0; comp<nb_compo_; comp++)
        {
          double somme=0;
          for (int isom=0; isom<dimension; isom++)
            if(nb_compo_==1)
              somme+=vals(face_sommets(face,isom));
            else
              somme+=vals(face_sommets(face,isom), comp);
          if(nb_compo_==1)
            {
              if(dimension==2)
                tab(face)=1./6.*somme+2./3.*valf(face);
              else
                tab(face)=17./60.*somme+3./20.*valf(face);
            }
          else if(dimension==2)
            tab(face,comp)=1./6.*somme+2./3.*valf(face,comp);
          else
            tab(face,comp)=17./60.*somme+3./20.*valf(face,comp);
        }
    }
  return *this;
#else
  return Champ_Inc_base::affecter_(ch);
#endif
}

//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
void Champ_P1NC::verifie_valeurs_cl()
{
  const Domaine_Cl_dis_base& zcl = equation().domaine_Cl_dis();
  int nb_cl = zcl.nb_cond_lim();
  DoubleTab& ch_tab = valeurs();
  int nb_compo = nb_comp();
  int ndeb, nfin, num_face;

  for (int i = 0; i < nb_cl; i++)
    {
      const Cond_lim_base& la_cl = zcl.les_conditions_limites(i).valeur();
      if (sub_type(Periodique, la_cl))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl);
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          int voisine;
          double moy;

          for (num_face = ndeb; num_face < nfin; num_face++)
            {
              voisine = la_cl_perio.face_associee(num_face - ndeb) + ndeb;
              for (int comp = 0; comp < nb_compo; comp++)
                {
                  if (ch_tab(num_face, comp) != ch_tab(voisine, comp))
                    {
                      moy = 0.5 * (ch_tab(num_face, comp) + ch_tab(voisine, comp));
                      ch_tab(num_face, comp) = moy;
                      ch_tab(voisine, comp) = moy;
                    }
                }
            }
        }
    }
  ch_tab.echange_espace_virtuel();
}

void Champ_P1NC::calcul_critere_Q(DoubleVect& tab_Critere_Q) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF, equation().domaine_Cl_dis());
  const int nb_elem = domaine_vef().nb_elem(), nb_elem_tot = domaine_vef().nb_elem_tot();
  const int dim = Objet_U::dimension;

  DoubleTrav tab_gradient_elem(nb_elem_tot, dim, dim);
  const DoubleTab& vitesse = valeurs();
  Champ_P1NC::calcul_gradient(vitesse, tab_gradient_elem, domaine_Cl_VEF);

  CDoubleTabView3 gradient_elem = tab_gradient_elem.view_ro<3>();
  DoubleArrView Critere_Q = tab_Critere_Q.view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       range_1D(0, nb_elem), KOKKOS_LAMBDA(
                         const int num_elem)
  {
    double crit = 0.;
    for (int i = 0; i < dim; i++)
      for (int j = 0; j < dim; j++)
        {
          double deriv1 = gradient_elem(num_elem, i, j);
          double deriv2 = gradient_elem(num_elem, j, i);
          crit += -0.25 * deriv1 * deriv2;
        }
    Critere_Q[num_elem] = crit;
  });
  end_gpu_timer(__KERNEL_NAME__);
}

void Champ_P1NC::calcul_y_plus(const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleVect& y_plus) const
{
  // On initialise le champ y_plus avec une valeur negative,
  // comme ca lorsqu'on veut visualiser le champ pres de la paroi,
  // on n'a qu'a supprimer les valeurs negatives et n'apparaissent
  // que les valeurs aux parois.

  int ndeb, nfin, l_unif;
  double visco0 = -1.;
  y_plus = -1.;

  const Equation_base& eqn_hydr = equation();
  const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
  const Champ_Don_base& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin.valeurs();

  if (sub_type(Champ_Uniforme, ch_visco_cin))
    {
      if (tab_visco(0, 0) > DMINFLOAT)
        visco0 = tab_visco(0, 0);
      else
        visco0 = DMINFLOAT;
      l_unif = 1;
    }
  else
    l_unif = 0;

  if ((!l_unif) && (tab_visco.local_min_vect() < DMINFLOAT))
    // GF on ne doit pas changer tab_visco ici !
    {
      Cerr << " visco <=0 ?" << finl;
      exit();
    }
  // tab_visco+=DMINFLOAT;

  DoubleTab yplus_faces(1, 1); // will contain yplus values if available
  int yplus_already_computed = 0; // flag

  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Modele_turbulence_hyd_base, modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_base& mod_turb = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
      const Turbulence_paroi_base& loipar = mod_turb.loi_paroi();
      // if( ! sub_type( Paroi_negligeable_VEF , loipar ) )
      if (loipar.use_shear())
        {
          yplus_faces.resize(domaine_vef().nb_faces_tot());
          yplus_faces.ref(loipar.tab_d_plus());
          yplus_already_computed = 1;
        }
    }

  for (int n_bord = 0; n_bord < domaine_vef().nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if( sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()) || la_cl->que_suis_je() == "Frontiere_ouverte_vitesse_imposee_ALE")
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          int dim = Objet_U::dimension;
          int nfac = domaine_vef().domaine().nb_faces_elem();
          DoubleTrav counter(y_plus.size());
          CDoubleTabView xp = domaine_vef().xp().view_ro(); // DOUBT: Why defined inside the for loop?
          CDoubleTabView xv = domaine_vef().xv().view_ro();
          CDoubleTabView face_normale = domaine_vef().face_normales().view_ro();
          CIntTabView elem_faces = domaine_vef().elem_faces().view_ro();
          CIntTabView face_voisins = domaine_vef().face_voisins().view_ro();
          CDoubleTabView vit = eqn_hydr.inconnue().valeurs().view_ro();  // DOUBT: Is this correct? Original assignment was `const Champ_P1NC& vit = *this;`
          CDoubleArrView visco = static_cast<const DoubleVect&>(tab_visco).view_ro();
          CDoubleArrView yp_faces = static_cast<const DoubleVect&>(yplus_faces).view_ro();
          DoubleArrView y_plus_view = y_plus.view_rw();
          DoubleArrView counter_view = static_cast<DoubleVect&>(counter).view_wo();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA (const int num_face)
          {
            int elem = face_voisins(num_face, 0);
            y_plus_view(elem) = 0;
          });
          end_gpu_timer(__KERNEL_NAME__);
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA (const int num_face)
          {
            int num[3] {};
            double val[3] {};
            int elem = face_voisins(num_face, 0);
            if (yplus_already_computed)
              {
                // y+ is only defined on faces so we take the face value to put in the element
                Kokkos::atomic_add(&y_plus_view(elem), yp_faces(num_face));
              }
            else
              {
                bool ok = false;
                for (int i = 0; i < dim; i++)
                  {
                    num[i] = elem_faces(elem, i);
                    if (num[i] == num_face && !ok)
                      {
                        num[i] = elem_faces(elem, dim);
                        ok = true;
                      }
                  }

                double dist = distance(dim, num_face, elem, xp, xv, face_normale);
                dist *= (dim + 1.) / dim; // pour se ramener a distance paroi / milieu de num[0]-num[1]
                double norm_v = norm_vit1_lp(dim, vit, num_face, nfac, num, face_normale, val);
                double d_visco = l_unif ? visco0:visco[elem];

                // PQ : 01/10/03 : corrections par rapport a la version premiere
                double norm_tau = d_visco * norm_v / dist;
                double u_etoile = sqrt(norm_tau);
                Kokkos::atomic_add(&y_plus_view(elem), dist * u_etoile / d_visco);
              } // else yplus already computed
            Kokkos::atomic_add(&counter_view(elem), +1);
          }); // loop on faces
          end_gpu_timer(__KERNEL_NAME__);
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(0, y_plus.size()), KOKKOS_LAMBDA (const int elem)
          {
            if (counter_view(elem)>0) y_plus_view(elem)/=counter_view(elem);
          });
          end_gpu_timer(__KERNEL_NAME__);

        } // Fin de paroi fixe

    } // Fin boucle sur les bords
}

void Champ_P1NC::calcul_grad_U(const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& tab_grad_u) const
{
  const DoubleTab& u = valeurs();
  const int nb_elem = domaine_vef().nb_elem(), nb_elem_tot = domaine_vef().nb_elem_tot();

  DoubleTrav tab_gradient_elem(nb_elem_tot, dimension, dimension);
  calculer_gradientP1NC(u, domaine_vef(), domaine_Cl_VEF, tab_gradient_elem);
  int dim = dimension;
  CDoubleTabView3 gradient_elem = tab_gradient_elem.view_ro<3>();
  DoubleTabView grad_u = tab_grad_u.view_wo();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(0, nb_elem), KOKKOS_LAMBDA(const int elem)
  {
    int comp = 0;
    for (int i = 0; i < dim; i++)
      {
        for (int j = 0; j < dim; j++)
          {
            grad_u(elem, comp) = gradient_elem(elem, i, j);
            comp++;
          }
      }
  });
  end_gpu_timer(__KERNEL_NAME__);
}

void Champ_P1NC::calcul_grad_T(const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& grad_T) const
{
  const DoubleTab& temp = valeurs();
  calculer_gradientP1NC(temp, domaine_vef(), domaine_Cl_VEF, grad_T);

  // *******************************************************************
  //  PQ : 12/12/05 : pour pouvoir visualiser les oscillations a partir
  //                    des changements de signes du gradient de T
  // *******************************************************************
  /*
   const IntTab& face_voisins = domaine_VEF.face_voisins();
   const IntTab& elem_faces=domaine_VEF.elem_faces();
   int nb_faces = domaine_VEF.nb_faces();
   const int nb_elem = domaine_VEF.nb_elem_tot();

   int num_elem,num_face,elem0,elem1,i;
   ArrOfInt sign_opp_x(nb_faces);
   ArrOfInt sign_opp_y(nb_faces);
   sign_opp_x=0;
   sign_opp_y=0;

   for (num_face=0;num_face<nb_faces;num_face++)
   {
   elem0 = face_voisins(num_face,0);
   elem1 = face_voisins(num_face,1);
   if (elem1!=-1)
   {
   if  (grad_T(elem0,0)*grad_T(elem1,0)<0.) sign_opp_x(num_face)++;
   if  (grad_T(elem0,1)*grad_T(elem1,1)<0.) sign_opp_y(num_face)++;
   }
   }

   grad_T=0.;

   for (num_elem=0;num_elem<nb_elem;num_elem++)
   {
   for (i=0;i<dimension+1;i++)
   {
   grad_T(num_elem,0) +=sign_opp_x(elem_faces(num_elem,i));
   grad_T(num_elem,1) +=sign_opp_y(elem_faces(num_elem,i));
   }
   }
   */

}

// Calcul du coefficient d'echange
void Champ_P1NC::calcul_h_conv(const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& h_conv, int temp_ref) const
{
  const DoubleTab& temp = valeurs();
  const IntTab& face_voisins = domaine_vef().face_voisins();
  const DoubleTab& face_normale = domaine_vef().face_normales();

  // On recupere le flux aux frontieres
  DoubleTab Flux;
  Flux = equation().operateur(0).l_op_base().flux_bords();

  // On initialise a -1
  h_conv = -1.;

  double T_ref = double(temp_ref);

  for (int n_bord = 0; n_bord < domaine_vef().nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Frontiere_dis_base& la_fr = la_cl->frontiere_dis();
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      double h_moy = 0.;
      // Selon les conditions limites
      if (sub_type(Echange_externe_impose, la_cl.valeur()))
        {
          const Champ_base& rho = equation().milieu().masse_volumique();
          const Champ_Don_base& Cp = equation().milieu().capacite_calorifique();
          int rho_uniforme = (sub_type(Champ_Uniforme,rho) ? 1 : 0);
          int cp_uniforme = (sub_type(Champ_Uniforme,Cp) ? 1 : 0);
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              int elem = face_voisins(num_face, 0);
              double rho_cp = (rho_uniforme ? rho.valeurs()(0, 0) : (rho.nb_comp() == 1 ? rho.valeurs()(num_face) : rho.valeurs()(num_face, 0)));
              rho_cp *= (cp_uniforme ? Cp.valeurs()(0, 0) : (Cp.nb_comp() == 1 ? Cp.valeurs()(num_face) : Cp.valeurs()(num_face, 0)));
              h_conv(elem) = ref_cast(Echange_externe_impose,la_cl.valeur()).h_imp(num_face) * rho_cp;
              h_moy += h_conv(elem);
            }
        }
      else if (sub_type(Scalaire_impose_paroi,la_cl.valeur()) || sub_type(Neumann_homogene, la_cl.valeur()) || sub_type(Neumann_paroi, la_cl.valeur()))
        {
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              int elem = face_voisins(num_face, 0);

              double norme_flux = 0.;
              double surf = 0.;

              for (int k = 0; k < Flux.dimension(1); k++)
                norme_flux += Flux(num_face, k) * Flux(num_face, k);

              for (int k = 0; k < Objet_U::dimension; k++)
                surf += face_normale(num_face, k) * face_normale(num_face, k);

              double DT = temp(num_face) - T_ref;

              if (DT == 0.)
                h_conv(elem) = 0.;
              else
                {
                  //La mise a jour de ce champ est faite quand on demande son postraitement
                  //et par consequent les flux ont ete mis a jour et sont deja multiplies par rho*Cp
                  h_conv(elem) = sqrt(norme_flux / surf) / DT;
                }
              h_moy += h_conv(elem);
            }
        }
      h_moy = Process::mp_sum(h_moy);
      int nb_faces = static_cast<int>(Process::mp_sum(le_bord.nb_faces()));  // a single 'bord' should not have so many faces
      h_moy /= nb_faces;
      if (je_suis_maitre())
        {
          Nom fich_hconv;
          fich_hconv = "Heat_transfert_Coeff_";
          fich_hconv += la_fr.le_nom();
          fich_hconv += ".dat";

          SFichier fic(fich_hconv, ios::app);
          fic.setf(ios::scientific);

          double le_temps = equation().schema_temps().temps_courant();
          fic << le_temps << " " << h_moy << finl;
        }
    }
}

static double norme_L2(const DoubleTab& u, const Domaine_VEF& domaine_VEF)
{
  const int nb_faces = domaine_VEF.nb_faces();
  double norme = 0;
  int nb_compo_ = u.line_size();

  CDoubleArrView volumes = domaine_VEF.volumes_entrelaces().view_ro(); // DOUBT: reference?
  CDoubleTabView uview = u.view_ro(); // DOUBT: How to name this variable?

  Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__), nb_faces,
                          KOKKOS_LAMBDA (const int i, double& s)
  {
    for (int j = 0; j < nb_compo_; j++)
      s += uview(i, j) * uview(i, j);
    s *= volumes(i);
  }, norme);
  end_gpu_timer(__KERNEL_NAME__);

  return sqrt(norme);
}
double Champ_P1NC::norme_L2(const Domaine& dom) const
{
  Cerr << "Champ_P1NC::norme_L2" << finl;
  {
    DoubleTab x(valeurs());
    filtrer_L2(x);
    Cerr << "Norme filtree : " << ::norme_L2(x, domaine_vef()) << finl;
  }
  const DoubleTab& u = valeurs();
  return ::norme_L2(u, domaine_vef());
}

double Champ_P1NC::norme_H1(const Domaine& dom) const
{
  const Domaine& mon_dom = domaine_vef().domaine();

  double dnorme_H1;
  // DOUBT: dimension?
  int dim = dimension;
  const int nb_elem = mon_dom.nb_elem();
  const int nb_faces_elem = mon_dom.nb_faces_elem();
  CIntTabView elem_faces = domaine_vef().elem_faces().view_ro();
  CDoubleTabView face_normales = domaine_vef().face_normales().view_ro();
  CIntTabView face_voisins = domaine_vef().face_voisins().view_ro();
  CDoubleTabView tab = valeurs().view_ro();
  CDoubleArrView volumes = domaine_vef().volumes().view_ro();

  //On va calculer la norme H1 d'une inconnue P1NC.
  //L'algorithme tient compte des contraintes suivantes:
  //- l'inconnue peut avoir plusieurs composantes
  //  (i.e etre un scalaire ou etre un vecteur)
  //- la dimension du probleme est arbitraire (1, 2 ou 3).
  //ATTENTION: les prismes ne sont pas supportes.
  dnorme_H1 = 0.;
  Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__), nb_comp(), //cas scalaire ou vectoriel
                          KOKKOS_LAMBDA (const int composante, double& norme_H1_comp)
  {
    for (int K = 0; K < nb_elem; K++) //boucle sur les elements
      {
        double norme_grad_elem = 0.; //pour eviter les accumulations
        for (int i = 0; i < dim; i++) //boucle sur la dimension du pb
          {
            double int_grad_elem = 0.; //pour eviter les accumulations
            for (int face = 0; face < nb_faces_elem; face++) //boucle sur les faces d'un "K"
              {
                int face_globale = elem_faces(K, face);

                int_grad_elem += tab(face_globale, composante) * face_normales(face_globale, i) * oriente_normale(face_globale, K, face_voisins);
              } //fin du for sur "face"

            norme_grad_elem += int_grad_elem * int_grad_elem;
          } //fin du for sur "i"

        norme_H1_comp += norme_grad_elem / volumes(K);
      } //fin du for sur "K"

  }, dnorme_H1); // fin du for sur "composante"
  end_gpu_timer(__KERNEL_NAME__);

  return sqrt(dnorme_H1);
}

double Champ_P1NC::norme_L2_H1(const Domaine& dom) const
{
  double pas_de_temps = equation().schema_temps().pas_de_temps();

  return ((1. / pas_de_temps) * norme_L2(dom)) + norme_H1(dom);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie gij aux elements a partir du champ aux faces
//(gij represente la derivee partielle dGi/dxj)
//A partir de gij, on peut calculer Sij = 0.5(gij(i,j)+gij(j,i))
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleTab& Champ_P1NC::calcul_gradient(const DoubleTab& champ, DoubleTab& gij, const Domaine_Cl_VEF& domaine_Cl_VEF)
{
  const Domaine_VEF& domaine_VEF = domaine_Cl_VEF.domaine_vef();
  calculer_gradientP1NC(champ, domaine_VEF, domaine_Cl_VEF, gij);
  return gij;

}

DoubleTab& Champ_P1NC::calcul_duidxj_paroi(DoubleTab& tab_gij, const DoubleTab& tab_nu, const DoubleTab& tab_nu_turb, const DoubleTab& tab_tau_tan, const Domaine_Cl_VEF& domaine_Cl_VEF)
{
  const Domaine_VEF& domaine_VEF = domaine_Cl_VEF.domaine_vef();

  // On va modifier Sij pour les faces de Dirichlet (Paroi_fixe) si nous n utilisons pas Paroi_negligeable!!!!
  // On aura : (grad u)_f = (grad u) - (grad u . n) x n + (grad u . n)_lp x n
  //

  // PQ : 25/01/08 : Mise au propre de la methode pour ne plus etre embete par les histoires de signes notamment
  //                     et savoir reellement ce qu'on fait !!
  //
  // PRINCIPE :
  // (expose en 2D, l'extension au 3D ne pose pas de probleme particulier)
  //
  // On considere 2 reperes : le repere global (x,y) et le repere local (t,n) ou n designe la normale a la paroi
  // Le frottement retourne par la loi de paroi correspond a : || tau_tan || = u*^2 = nu.d(u_t)/dn
  // Soit P la matrice de passage permettant de passer d'un repere a l'autre
  //
  //   P = ( tx  nx )
  //           ( ty  ny )
  //
  // Les matrices des gradients de vitesse dans chacun des reperes :
  //
  //  G_(x,y) =  ( du/dx  du/dy )    et   F_(t,n) = ( du_t/dt   du_t/dn )
  //                 ( dv/dx  dv/dy )                    ( du_n/dt   du_n/dn )
  //
  //                                                                     -1
  // sont reliees l'une a l'autre par :   G_(x,y)  =  P . F_(t,n) . P
  //
  //
  //  Ainsi la correction apportee dans F = ( du_t/dt   du_t/dn + C  )  ou  C = -du_t/dn + tau_tan/nu
  //                                            ( du_n/dt   du_n/dn             )
  //
  //  se reporte dans G de la maniere suivante :
  //
  //                                              -1
  //  G*_(x,y) = G_(x,y) +  P . ( 0  C ). P   =  G_(x,y) + ( C.nx.tx  C.ny.tx )
  //                                ( 0  0 )                   ( C.nx.ty  C.ny.ty )
  //
  //
  // c'est ce dernier terme qu'il s'agit donc de determiner ici

  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  int nb_cl = les_cl.size();

  // tab_tau_tan could be uninitialized at this point. In such a case,
  // we can skip the function because the value of gij won't change
  if (tab_tau_tan.dimension_tot(0) == 0) return tab_gij;

  for (int num_cl = 0; num_cl < nb_cl; num_cl++)
    {
      //Boucle sur les bords
      const Cond_lim& la_cl = les_cl[num_cl];
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()) || la_cl->que_suis_je() == "Frontiere_ouverte_vitesse_imposee_ALE")
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl->frontiere_dis());
          int ndeb = la_front_dis.num_premiere_face();
          int nfin = ndeb + la_front_dis.nb_faces();
          int dim = Objet_U::dimension;
          // Boucle sur les faces
          CDoubleTabView face_normale = domaine_VEF.face_normales().view_ro();
          CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
          CDoubleArrView porosite_face = domaine_Cl_VEF.equation().milieu().porosite_face().view_ro();
          CDoubleTabView tau_tan = tab_tau_tan.view_ro();
          CDoubleArrView nu = static_cast<const ArrOfDouble&>(tab_nu).view_ro();
          CDoubleArrView nu_turb = static_cast<const ArrOfDouble&>(tab_nu_turb).view_ro();
          DoubleTabView3 gij = tab_gij.view_rw<3>();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA (const int fac)
          {
            double P[3][3] {};
            int num1 = face_voisins(fac, 0);
            // definition des vecteurs unitaires constituant le repere local
            // stockes dans la matrice de passage P
            // vecteur tangentiel (porte par la vitesse tangentielle)
            double sum = 0.;
            for (int i = 0; i < dim; i++)
              sum += tau_tan(fac, i) * tau_tan(fac, i);
            double norme_tau_tan = sqrt(sum);
            for (int i = 0; i < dim; i++)
              P[i][0] = tau_tan(fac, i) / (norme_tau_tan + DMINFLOAT);

            // vecteur normal a la paroi
            sum = 0.;
            for (int i = 0; i < dim; i++)
              sum += face_normale(fac, i) * face_normale(fac, i);
            double norme = sqrt(sum);

            int signe = -oriente_normale(fac, num1, face_voisins); // orientation vers l'interieur
            for (int i = 0; i < dim; i++)
              P[i][1] = signe * face_normale(fac, i) / norme;

            // (3D) on complete la base par le deuxieme vecteur tangentiel
            if (dim == 3)
              {
                P[0][2] = P[1][0] * P[2][1] - P[2][0] * P[1][1];
                P[1][2] = P[2][0] * P[0][1] - P[0][0] * P[2][1];
                P[2][2] = P[0][0] * P[1][1] - P[1][0] * P[0][1];
              }
            //         determination du terme d(u_t)/dn a enlever
            //                                                       -1
            //         terme identifie a l'aide du produit : F =  P . G . P
            //
            double dutdn_old = 0.;
            for (int i = 0; i < dim; i++)
              for (int j = 0; j < dim; j++)
                {
                  double gij_value = Kokkos::atomic_fetch_add(&gij(num1, i, j), 0.0);
                  dutdn_old += gij_value * P[j][1] * P[i][0];
                }

            //         Correction finale apportee a la matrice G
            double C = -dutdn_old + norme_tau_tan / (nu[num1] + nu_turb[num1]) * porosite_face(fac);

            // la division par (nu[num1]+nu_turb[num1]) s'impose du fait que l'operateur de diffusion
            // fait intervenir le produit : (nu[num1]+nu_turb[num1])*g(i,j)
            for (int i = 0; i < dim; i++)
              for (int j = 0; j < dim; j++)
                Kokkos::atomic_add(&gij(num1, i, j), C * P[j][1] * P[i][0]);
          });
          end_gpu_timer(__KERNEL_NAME__);
        }
    }

  return tab_gij;
}

////////////////////
// Calcul de 2SijSij
////////////////////
DoubleVect& Champ_P1NC::calcul_S_barre(const DoubleTab& la_vitesse, DoubleVect& SMA_barre, const Domaine_Cl_VEF& domaine_Cl_VEF)
{
  const Domaine_VEF& domaine_VEF = domaine_Cl_VEF.domaine_vef();
  const int nb_elem = domaine_VEF.nb_elem();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();

  DoubleTrav duidxj(nb_elem_tot, dimension, dimension);

  Champ_P1NC::calcul_gradient(la_vitesse, duidxj, domaine_Cl_VEF);

  int dim = Objet_U::dimension;
  CDoubleTabView3 duidxj_v = duidxj.view_ro<3>();
  DoubleArrView SMA_barre_v = SMA_barre.view_wo();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(
                         const int elem)
  {
    double temp = 0.;
    for (int i = 0; i < dim; i++)
      for (int j = 0; j < dim; j++)
        {
          double Sij = 0.5 * (duidxj_v(elem, i, j) + duidxj_v(elem, j, i));
          temp += Sij * Sij;
        }
    SMA_barre_v(elem) = 2. * temp;
  });
  end_gpu_timer(__KERNEL_NAME__);

  return SMA_barre;
}

double Champ_P1NC::calculer_integrale_volumique(const Domaine_VEF& domaine, const DoubleVect& v, Ok_Perio ok)
{
  if (ok != FAUX_EN_PERIO)
    {
      // BM: desole
      Cerr << "Champ_P1NC::calculer_integrale_volumique pas encore code juste en perio !" << finl;
      exit();
    }

  assert(v.line_size() == 1);
  const DoubleVect& volume = domaine.volumes_entrelaces();
  const double resu = mp_prodscal(v, volume);
  return resu;
}

int Champ_P1NC::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  assert(nb_noeuds == domaine_vef().nb_faces());
  const MD_Vector& md = domaine_vef().md_vector_faces();
  creer_tableau_distribue(md);

  Champ_P1NC_implementation::fixer_nb_valeurs_nodales(nb_noeuds);

  return nb_noeuds;
}
