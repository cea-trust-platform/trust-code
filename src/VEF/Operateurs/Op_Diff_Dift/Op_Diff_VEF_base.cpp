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

#include <Op_Diff_VEF_base.h>
#include <Champ_P1NC.h>
#include <Champ_Q1NC.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Fonc_P0_base.h>
#include <Discretisation_base.h>
#include <Check_espace_virtuel.h>
#include <Echange_externe_impose.h>

Implemente_base(Op_Diff_VEF_base,"Op_Diff_VEF_base",Operateur_Diff_base);

Sortie& Op_Diff_VEF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Diff_VEF_base::readOn(Entree& s )
{
  return s ;
}


/*! @brief definit si on calcule div(phi nu grad Psi) ou div(nu grap Phi psi)
 *
 */
int Op_Diff_VEF_base::phi_psi_diffuse(const Equation_base& eq) const
{
  if (eq.inconnue().le_nom()=="vitesse")
    return 1;
  return 0;
}

int Op_Diff_VEF_base::impr(Sortie& os) const
{
  return Op_VEF_Face::impr(os, *this);
}

void Op_Diff_VEF_base::associer(const Domaine_dis_base& domaine_dis,
                                const Domaine_Cl_dis_base& domaine_cl_dis,
                                const Champ_Inc_base& ch_transporte)
{

  const Domaine_VEF& zvef = ref_cast(Domaine_VEF,domaine_dis);
  const Domaine_Cl_VEF& zclvef = ref_cast(Domaine_Cl_VEF,domaine_cl_dis);

  if (sub_type(Champ_P1NC,ch_transporte))
    {
      const Champ_P1NC& inco = ref_cast(Champ_P1NC,ch_transporte);
      OBS_PTR(Champ_P1NC) inconnue;
      inconnue = inco;
    }
  if (sub_type(Champ_Q1NC,ch_transporte))
    {
      const Champ_Q1NC& inco = ref_cast(Champ_Q1NC,ch_transporte);
      OBS_PTR(Champ_Q1NC) inconnue;
      inconnue = inco;
    }

  le_dom_vef = zvef;
  la_zcl_vef = zclvef;


  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  int nb_dim = ch_transporte.valeurs().nb_dim();
  int nb_comp = 1;

  if(nb_dim==2)
    nb_comp = ch_transporte.valeurs().dimension(1);

  flux_bords_.resize(domaine_VEF.nb_faces_bord(), nb_comp);
  flux_bords_ = 0.;
}
double Op_Diff_VEF_base::calculer_dt_stab() const
{
  remplir_nu(nu_);
  // La diffusivite est constante dans le domaine donc
  //
  //          dt_diff = h*h/diffusivite

  double dt_stab = DMAXFLOAT;
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  if (!has_champ_masse_volumique())
    {
      // Methode "standard" de calcul du pas de temps
      // Ce calcul est tres conservatif: si le max de la diffusivite
      // n'est pas atteint a l'endroit ou le min de delta_h_carre est atteint,
      // le pas de temps est sous-estime.
      const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
      const DoubleVect&      valeurs_diffusivite = champ_diffusivite.valeurs();
      double alpha_max = local_max_vect(valeurs_diffusivite);

      // Detect if a heat flux is imposed on a boundary through Paroi_echange_externe_impose keyword
      double h_imp_max = -1, h_imp_temp=-2, max_conductivity = 0;
      const Domaine_Cl_VEF& le_dom_cl_vef = la_zcl_vef.valeur();
      const Equation_base& mon_eqn = le_dom_cl_vef.equation();

      for(int i=0; i<le_dom_cl_vef.nb_cond_lim(); i++)
        {
          // loop on boundaries
          const Cond_lim_base& la_cl = le_dom_cl_vef.les_conditions_limites(i).valeur();
          if (sub_type(Echange_externe_impose,la_cl))
            {
              const Echange_externe_impose& la_cl_int = ref_cast(Echange_externe_impose,la_cl);
              const Champ_front_base& le_ch_front = ref_cast( Champ_front_base, la_cl_int.h_imp());
              const DoubleVect& tab = le_ch_front.valeurs();
              if(tab.size() > 0)
                {
                  h_imp_temp = local_max_vect(tab); // get h_imp from datafile
                  h_imp_temp = std::fabs(h_imp_temp); // we should take the absolute value since it can be negative!
                  h_imp_max = (h_imp_temp>h_imp_max) ? h_imp_temp : h_imp_max ; // Should we take the max if more than one bc has h_imp ?
                }
            }
        } // End loop on boundaries

      h_imp_max = Process::mp_max(h_imp_max);

      if ((alpha_max == 0.)||(domaine_VEF.nb_elem()==0))
        dt_stab = DMAXFLOAT;
      else
        {
          const double min_delta_h_carre = domaine_VEF.carre_pas_du_maillage();
          if (h_imp_max>0.0) //we have a BC with Paroi_echange_externe_impose
            {
              // get the thermal conductivity
              const Milieu_base& mon_milieu = mon_eqn.milieu();
              const DoubleVect& tab_lambda = mon_milieu.conductivite().valeurs();
              max_conductivity = local_max_vect(tab_lambda);
              // compute Biot number given by Bi = L*h/lambda.
              double Bi = h_imp_max*sqrt(min_delta_h_carre)/max_conductivity;
              // if Bi>1, replace conductivity by h_imp*h in diffusivity. We are very conservative since h_imp_max is not necessarily located where max_conductivity is.
              if (Bi>1.0)
                alpha_max *= h_imp_max*sqrt(min_delta_h_carre)/max_conductivity;
            }
          dt_stab = min_delta_h_carre / (2. * dimension * alpha_max);
        }
    }
  else
    {
      // Champ de masse volumique variable.
      const double deux_dim = 2. * Objet_U::dimension;
      const Champ_base& champ_diffu = diffusivite();
      const Champ_base& champ_rho = get_champ_masse_volumique();
      assert(sub_type(Champ_Fonc_P0_base, champ_rho));
      assert(sub_type(Champ_Fonc_P0_base, champ_diffu));
      const int nb_elem = domaine_VEF.nb_elem();
      CDoubleArrView carre_pas_maille = domaine_VEF.carre_pas_maille().view_ro();
      CDoubleArrView valeurs_diffu = static_cast<const DoubleVect&>(champ_diffu.valeurs()).view_ro();
      CDoubleArrView valeurs_rho   = static_cast<const DoubleVect&>(champ_rho.valeurs()).view_ro();
      Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                              range_1D(0, nb_elem), KOKKOS_LAMBDA(
                                const int elem, double& dtstab)
      {
        const double h_carre = carre_pas_maille(elem);
        const double diffu   = valeurs_diffu(elem);
        const double rho     = valeurs_rho(elem);
        const double dt      = h_carre * rho / (deux_dim * (diffu+DMINFLOAT));
        if (dt < dtstab) dtstab = dt;
      }, Kokkos::Min<double>(dt_stab));
      end_gpu_timer(__KERNEL_NAME__);
    }
  dt_stab = Process::mp_min(dt_stab);
  return dt_stab;
}

// cf Op_Diff_VEF_base::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Diff_VEF_base::calculer_pour_post(Champ_base& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage.valeurs();

      if (le_dom_vef.non_nul())
        {
          remplir_nu(nu_);
          const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
          if (! has_champ_masse_volumique())
            {

              const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
              const DoubleVect&      valeurs_diffusivite = champ_diffusivite.valeurs();
              double alpha_max = local_max_vect(valeurs_diffusivite);

              if ((alpha_max == 0.)||(domaine_VEF.nb_elem()==0))
                es_valeurs = DMAXFLOAT;
              else
                {
                  const int nb_elem = domaine_VEF.nb_elem();
                  for (int elem = 0; elem < nb_elem; elem++)
                    {
                      es_valeurs(elem) = domaine_VEF.carre_pas_maille()(elem) / (2. * dimension * alpha_max);
                    }
                }
            }
          else
            {
              const double           deux_dim      = 2. * Objet_U::dimension;
              const Champ_base& champ_diffu   = diffusivite();
              const DoubleTab&       valeurs_diffu = champ_diffu.valeurs();
              const Champ_base&      champ_rho     = get_champ_masse_volumique();
              const DoubleTab&       valeurs_rho   = champ_rho.valeurs();
              assert(sub_type(Champ_Fonc_P0_base, champ_rho));
              assert(sub_type(Champ_Fonc_P0_base, champ_diffu));
              const int nb_elem = domaine_VEF.nb_elem();
              assert(valeurs_rho.size_array()==domaine_VEF.nb_elem_tot());
              // Champ de masse volumique variable.
              for (int elem = 0; elem < nb_elem; elem++)
                {
                  const double h_carre = domaine_VEF.carre_pas_maille()(elem);
                  const double diffu   = valeurs_diffu(elem);
                  const double rho     = valeurs_rho(elem);
                  const double dt      = h_carre * rho / (deux_dim * diffu);
                  es_valeurs(elem) = dt;
                }
            }

          assert(mp_min_vect(es_valeurs)==calculer_dt_stab());
        }
    }
  else
    Operateur_Diff_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Diff_VEF_base::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite")
    loc = "elem";
  else
    return Operateur_Diff_base::get_localisation_pour_post(option);
  return loc;
}

void Op_Diff_VEF_base::remplir_nu(DoubleTab& tab_nu) const
{
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  // On dimensionne nu
  const DoubleTab& tab_diffu = diffusivite().valeurs();
  if (!tab_nu.get_md_vector().non_nul())
    {
      tab_nu.resize(0, tab_diffu.line_size());
      domaine_VEF.domaine().creer_tableau_elements(tab_nu, RESIZE_OPTIONS::NOCOPY_NOINIT);
    }
  if (!tab_diffu.get_md_vector().non_nul())
    {
      // diffusivite uniforme
      const int n = tab_nu.dimension_tot(0);
      const int nb_comp = tab_nu.line_size();
      CDoubleArrView diffu = static_cast<const DoubleVect&>(tab_diffu).view_ro();
      DoubleTabView nu = tab_nu.view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                           range_2D({0, 0}, {n, nb_comp}),
                           KOKKOS_LAMBDA(const int i, const int j)
      {
        nu(i, j) = diffu(j);
      });
      end_gpu_timer(__KERNEL_NAME__);
    }
  else
    {
      assert(tab_nu.get_md_vector() == tab_diffu.get_md_vector());
      assert_espace_virtuel_vect(tab_diffu);
      // Sync arrays on the device before copy:
      mapToDevice(tab_diffu);
      computeOnTheDevice(tab_nu);
      tab_nu.inject_array(tab_diffu);
    }
}
