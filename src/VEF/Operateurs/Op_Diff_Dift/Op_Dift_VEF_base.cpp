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

#include <Modele_turbulence_scal_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Navier_Stokes_std.h>
#include <Op_Dift_VEF_base.h>
#include <Domaine_Cl_VEF.h>


Implemente_base(Op_Dift_VEF_base, "Op_Dift_VEF_base", Op_Diff_VEF_base);

Sortie& Op_Dift_VEF_base::printOn(Sortie& is) const { return Op_Diff_VEF_base::printOn(is); }

Entree& Op_Dift_VEF_base::readOn(Entree& is) { return Op_Diff_VEF_base::readOn(is); }

void Op_Dift_VEF_base::associer_modele_turbulence(const Modele_turbulence_hyd_base& mod)
{
  le_modele_turbulence = mod;
}

void Op_Dift_VEF_base::mettre_a_jour(double)
{
  if (sub_type(Navier_Stokes_std, equation())) // on traite l'hydraulique
    {
      if (le_modele_turbulence->utiliser_loi_paroi())
        {
          // Modif BM: on ne prend la ref que si le tableau a ete initialise, sinon ca bloque l'initialisation
          const DoubleTab& tab = le_modele_turbulence->loi_paroi().Cisaillement_paroi();
          if (tab.size_array() > 0) tau_tan_.ref(tab);
        }
    }
}

void Op_Dift_VEF_base::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis, const Champ_Inc_base& ch_transporte)
{
  le_dom_vef = ref_cast(Domaine_VEF, domaine_dis);
  la_zcl_vef = ref_cast(Domaine_Cl_VEF, domaine_cl_dis);
  inconnue_ = ch_transporte;
}

void Op_Dift_VEF_base::completer()
{
  Operateur_base::completer();

  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Modele_turbulence_hyd_base, modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_base& mod_turb = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
      const Champ_Fonc_base& viscosite_turbulente = mod_turb.viscosite_turbulente();
      associer_diffusivite_turbulente(viscosite_turbulente);
      associer_modele_turbulence(mod_turb);
    }
  else if (sub_type(Modele_turbulence_scal_base, modele_turbulence.valeur()))
    {
      const Modele_turbulence_scal_base& modele_turbulence_scalaire = ref_cast(Modele_turbulence_scal_base, modele_turbulence.valeur());
      const Champ_Fonc_base& conductivite_turbulente = modele_turbulence_scalaire.conductivite_turbulente();
      associer_diffusivite_turbulente(conductivite_turbulente);
    }
  else
    {
      Cerr << "Error in Op_Dift_VEF_base::completer() " << finl;
      Cerr << que_suis_je() << " operator is presently associated to " << equation().que_suis_je() << finl;
      Cerr << "instead of being associated to an equation dedicated to a turbulent flow." << finl;
      Process::exit();
    }
}

void Op_Dift_VEF_base::calculer_borne_locale(DoubleVect& tab_borne_visco_turb, double dt_conv, double dt_diff_sur_dt_conv) const
{
  const Domaine_VEF& le_dom_VEF = domaine_vef();
  int nb_elem = le_dom_VEF.nb_elem();
  int flag = diffusivite().valeurs().dimension(0) > 1 ? 1 : 0;
  const double dim = Objet_U::dimension;
  CDoubleTabView diffu = diffusivite().valeurs().view_ro();
  CDoubleArrView carre_pas_maille = le_dom_VEF.carre_pas_maille().view_ro();
  DoubleArrView borne_visco_turb = tab_borne_visco_turb.view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(const int elem)
  {
    double h_inv = 1. / carre_pas_maille(elem);
    // C'est pas tres propre pour recuperer diffu mais ca evite de coder cette methode dans plusieurs classes:
    double diffu_elem = (flag ? diffu(elem, 0) : diffu(0, 0));
    double coef = 1. / (2 * (dt_conv + DMINFLOAT) * dim * h_inv * dt_diff_sur_dt_conv) - diffu_elem;
    if (coef > 0 && coef < borne_visco_turb(elem))
      borne_visco_turb(elem) = coef;
  });
  end_gpu_timer(__KERNEL_NAME__);
}

// La diffusivite est constante par elements donc il faut calculer dt_diff pour chaque element et dt_stab=Min(dt_diff (K) = h(K)*h(K)/(2*dimension*diffu2_(K)))
// ou diffu2_ est la somme des 2 diffusivite laminaire et turbulente

//GF : alpha_dt_stab=(alpha+alpha_t)*alpha_dt_stab/alpha ET alpha_dt_stab=(nu+diff_nu_turb)*valeurs_diffusivite_dt/nu
double Op_Dift_VEF_base::calculer_dt_stab() const
{
  remplir_nu(nu_); // On remplit le tableau nu contenant la diffusivite en chaque elem

  //DoubleVect tab_diffu_turb(diffusivite_turbulente().valeurs());
  DoubleTrav tab_diffu_turb;
  tab_diffu_turb = diffusivite_turbulente().valeurs();
  DoubleTrav tab_diffu;
  tab_diffu = nu_; // XXX : Elie Saikali : Attention pas pareil que DoubleTrav diffu(nu_) !!!!!!!!!

  if (equation().que_suis_je().debute_par("Convection_Diffusion_Temp"))
    {
      double rhocp = mon_equation->domaine_dis().nb_elem() > 0 ?  mon_equation->milieu().capacite_calorifique().valeurs()(0, 0) * mon_equation->milieu().masse_volumique().valeurs()(0, 0) : 1.0;
      tab_diffu_turb /= rhocp;
      tab_diffu /= rhocp;
    }

  const double deux_dim = 2. * Objet_U::dimension;
  const Domaine_VEF& le_dom_VEF = domaine_vef();
  const int le_dom_nb_elem = le_dom_VEF.domaine().nb_elem();
  double dt_stab = 1.e30;
  CDoubleTabView diffu = tab_diffu.view_ro();
  CDoubleArrView diffu_turb = static_cast<const DoubleVect&>(tab_diffu_turb).view_ro();
  CDoubleArrView carre_pas_maille = le_dom_VEF.carre_pas_maille().view_ro();
  if (has_champ_masse_volumique())
    {
      const DoubleTab& tab_rho_elem = get_champ_masse_volumique().valeurs();
      assert(tab_rho_elem.size_array() == le_dom_VEF.nb_elem_tot());
      CDoubleArrView rho_elem = static_cast<const DoubleVect&>(tab_rho_elem).view_ro();
      Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                              Kokkos::RangePolicy<>(0, le_dom_nb_elem), KOKKOS_LAMBDA(
                                const int num_elem, double& dtstab)
      {
        double alpha = diffu(num_elem, 0) + diffu_turb(num_elem); // PQ : 06/03
        alpha /= rho_elem(num_elem);
        double coef = carre_pas_maille(num_elem) / (deux_dim * (alpha + DMINFLOAT));
        if (coef < dtstab) dtstab = coef;
      }, Kokkos::Min<double>(dt_stab));
    }
  else
    {
      const DoubleTab& tab_valeurs_diffusivite = diffusivite_pour_pas_de_temps().valeurs();
      const int nb_comp = tab_valeurs_diffusivite.line_size(), cD = (tab_valeurs_diffusivite.dimension(0) == 1); // uniforme ou pas ?
      CDoubleTabView valeurs_diffusivite = tab_valeurs_diffusivite.view_ro();
      Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                              Kokkos::RangePolicy<>(0, le_dom_nb_elem), KOKKOS_LAMBDA(
                                const int num_elem, double& dtstab)
      {
        for (int nc = 0; nc < nb_comp; nc++)
          {
            double alpha = diffu(num_elem, nc) + diffu_turb(num_elem);
            const double valeurs_diffusivite_dt = valeurs_diffusivite(!cD * num_elem, nc);
            alpha *= valeurs_diffusivite_dt / (diffu(num_elem, nc) + DMINFLOAT);
            double coef = carre_pas_maille(num_elem) / (deux_dim * (alpha + DMINFLOAT));
            if (coef < dtstab) dtstab = coef;
          }
      }, Kokkos::Min<double>(dt_stab));
    }
  end_gpu_timer(__KERNEL_NAME__);
  dt_stab = Process::mp_min(dt_stab);
  return dt_stab;
}

// cf Op_Dift_VEF_Face::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Dift_VEF_base::calculer_pour_post(Champ_base& espace_stockage, const Nom& option, int comp) const
{
  if (Motcle(option) == "stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage.valeurs();

      if (le_dom_vef.non_nul())
        {
          remplir_nu(nu_); // On remplit le tableau nu contenant la diffusivite en chaque elem

          const Domaine_VEF& le_dom_VEF = domaine_vef();
          const Domaine& le_dom = le_dom_VEF.domaine();
          const DoubleVect& diffu_turb = diffusivite_turbulente().valeurs();
          double alpha = -123., coef = -123.;
          ToDo_Kokkos("critical");

          int le_dom_nb_elem = le_dom.nb_elem();
          if (has_champ_masse_volumique())
            {
              const DoubleTab& rho_elem = get_champ_masse_volumique().valeurs();
              assert(rho_elem.size_array() == le_dom_VEF.nb_elem_tot());
              for (int num_elem = 0; num_elem < le_dom_nb_elem; num_elem++)
                {
                  alpha = nu_[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
                  alpha /= rho_elem[num_elem];
                  coef = le_dom_VEF.carre_pas_maille()(num_elem) / (2. * dimension * alpha);
                  es_valeurs(num_elem) = coef;
                }
            }
          else
            {
              const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
              const DoubleTab& valeurs_diffusivite = champ_diffusivite.valeurs();
              const int cD = (valeurs_diffusivite.dimension(0) == 1); // uniforme ou pas ?
              for (int num_elem = 0; num_elem < le_dom_nb_elem; num_elem++)
                {
                  const double valeurs_diffusivite_dt = valeurs_diffusivite(!cD * num_elem);
                  alpha = nu_[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
                  alpha *= valeurs_diffusivite_dt / nu_[num_elem];
                  coef = le_dom_VEF.carre_pas_maille()(num_elem) / (2. * dimension * alpha);
                  es_valeurs(num_elem) = coef;
                }
            }

          assert(est_egal(mp_min_vect(es_valeurs), calculer_dt_stab()));
        }
    }
  else
    Op_Diff_VEF_base::calculer_pour_post(espace_stockage, option, comp);
}

// La diffusivite est constante par elements donc il faut calculer dt_diff pour chaque element et dt_stab=Min(dt_diff (K) = h(K)*h(K)/(2*dimension*diffu2_(K)))
// ou diffu2_ est la somme des 2 diffusivite laminaire et turbulente
double Op_Dift_VEF_base::calculer_dt_stab_P1NCP1B() const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const DoubleVect& volumes = domaine_VEF.volumes(), &diffu_turb = diffusivite_turbulente().valeurs();
  const int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), le_dom_nb_elem = domaine_VEF.domaine().nb_elem();

  const double diffu = diffusivite(0);
  double dt_stab = 1.e30, coef, diffu2_;
  ToDo_Kokkos("critical");

  for (int num_elem = 0; num_elem < le_dom_nb_elem; num_elem++)
    {
      double surf_max = 1.e-30;
      for (int i = 0; i < nb_faces_elem; i++)
        {
          const int num_face = elem_faces(num_elem, i);
          double surf = face_normales(num_face, 0) * face_normales(num_face, 0);
          for (int j = 1; j < dimension; j++)
            surf += face_normales(num_face, j) * face_normales(num_face, j);
          surf_max = (surf > surf_max) ? surf : surf_max;
        }
      double vol = volumes(num_elem);
      vol *= vol / surf_max;
      diffu2_ = diffu + diffu_turb[num_elem];
      coef = vol / (2. * dimension * (diffu2_ + DMINFLOAT));
      dt_stab = (coef < dt_stab) ? coef : dt_stab;
    }

  return Process::mp_min(dt_stab);
}
