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
#include <Echange_externe_impose.h>
#include <Op_Diff_DG_base.h>
#include <Check_espace_virtuel.h>
#include <Domaine_Cl_DG.h>
#include <Champ_Elem_DG.h>
#include <Champ_Fonc_P0_base.h>
#include <Schema_Temps_base.h>
#include <Domaine_DG.h>
#include <Champ_Uniforme.h>
#include <communications.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Milieu_base.h>
#include <TRUSTTrav.h>
#include <SFichier.h>

Implemente_base(Op_Diff_DG_base, "Op_Diff_DG_base", Operateur_Diff_base);

Sortie& Op_Diff_DG_base::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Diff_DG_base::readOn(Entree& s) { return s; }

double Op_Diff_DG_base::calculer_dt_stab() const
{
  update_nu();
  const Domaine& mon_dom = le_dom_dg_->domaine();
  double dt_stab = DMAXFLOAT;

  const int nb_elem = mon_dom.nb_elem();

  if (!has_champ_masse_volumique())
    {
      // Methode "standard" de calcul du pas de temps
      // Ce calcul est tres conservatif: si le max de la diffusivite
      // n'est pas atteint a l'endroit ou le min de delta_h_carre est atteint,
      // le pas de temps est sous-estime.
      const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
      const DoubleVect& valeurs_diffusivite = champ_diffusivite.valeurs();
      double alpha_max = local_max_vect(valeurs_diffusivite);

      // Detect if a heat flux is imposed on a boundary through Paroi_echange_externe_impose keyword
      double h_imp_max = -1, h_imp_temp = -2;

      const Domaine_Cl_DG& le_dom_cl_dg = la_zcl_dg_.valeur();
      for (int i = 0; i < le_dom_cl_dg.nb_cond_lim(); i++)
        {
          // loop on boundaries
          const Cond_lim_base& la_cl = le_dom_cl_dg.les_conditions_limites(i).valeur();

          if (sub_type(Echange_externe_impose, la_cl))
            {
              const Echange_externe_impose& la_cl_int = ref_cast(Echange_externe_impose, la_cl);
              const Champ_front_base& le_ch_front = ref_cast(Champ_front_base, la_cl_int.h_imp());
              const DoubleVect& tab = le_ch_front.valeurs();
              if (tab.size() != 0)
                {
                  h_imp_temp = local_max_vect(tab); // get h_imp from datafile
                  h_imp_temp = std::fabs(h_imp_temp); // we should take the absolute value since it can be negative!
                  h_imp_max = (h_imp_temp > h_imp_max) ? h_imp_temp : h_imp_max; // Should we take the max if more than one bc has h_imp ?
                }
            }
        } // End loop on boundaries
      h_imp_max = Process::mp_max(h_imp_max);

      if (alpha_max != 0.0 && nb_elem != 0)
        {
          double min_delta_h_carre = le_dom_dg_->carre_pas_du_maillage();
          if (h_imp_max > 0.0)  //a heat flux is imposed on a boundary condition
            {
              // get the thermal conductivity
              const Equation_base& mon_eqn = le_dom_cl_dg.equation();
              const Milieu_base& mon_milieu = mon_eqn.milieu();
              const DoubleVect& tab_lambda = mon_milieu.conductivite().valeurs();
              double max_conductivity = local_max_vect(tab_lambda);

              // compute Biot number given by Bi = L*h/lambda.
              double Bi = h_imp_max * sqrt(min_delta_h_carre) / max_conductivity;
              // if Bi>1, replace conductivity by h_imp*h in diffusivity. We are very conservative since h_imp_max is not necessarily located where max_conductivity is.
              if (Bi > 1.0)
                alpha_max *= h_imp_max * sqrt(min_delta_h_carre) / max_conductivity;
            }
          dt_stab = min_delta_h_carre / (2. * dimension * alpha_max);
        }

    }
  else
    {
      const int deux_dim = 2 * Objet_U::dimension;
      const Champ_base& champ_diffu = diffusivite();
      const DoubleTab& valeurs_diffu = champ_diffu.valeurs();
      const Champ_base& champ_rho = get_champ_masse_volumique();
      const DoubleTab& valeurs_rho = champ_rho.valeurs();

      assert(sub_type(Champ_Elem_DG, champ_rho));
      assert(sub_type(Champ_Fonc_P0_base, champ_diffu));
      // assert(valeurs_rho.size_array()== mon_dom.les_elems().dimension_tot(0));
      // Champ_Elem_DG : champ aux elems et aux faces
      // Champ de masse volumique variable.
      const IntTab& e_f = le_dom_dg_->elem_faces();
      //Cerr << e_f << finl;
      for (int elem = 0; elem < nb_elem; elem++)
        {
          const double diffu = valeurs_diffu(elem);
          const double rho = valeurs_rho(elem);
          double dt;
          if (e_f.dimension(1) == deux_dim || e_f(elem, deux_dim) == -1)
            {
              // Maille type VDF (deux_dim faces sur l'element)
              // ToDo: coder dans le cas has_champ_masse_volumique()==false
              double h = 0;
              for (int f = 0; f < deux_dim; f++)
                {
                  int face = e_f(elem, f);
                  const double d = le_dom_dg_->volumes(elem) / le_dom_dg_->face_surfaces(face);
                  h += 0.5 / (d * d); // On multiplie par 0.5 car face comptee 2 fois
                  //Cerr << elem << " " << face << " " << le_dom_poly_->surface(face) << finl;
                }
              // Voir Op_Diff_VDF_Elem_base::calculer_dt_stab():
              dt = 0.5 * rho / ((diffu + DMINFLOAT) * h);
              //Cerr << "VDF " << dt << finl;
            }
          else
            {
              dt = le_dom_dg_->carre_pas_maille(elem) * rho / (deux_dim * (diffu + DMINFLOAT));
              //Cerr << "NC  " << dt << finl;
            }
          if (dt < dt_stab)
            dt_stab = dt;
        }
    }

  dt_stab = Process::mp_min(dt_stab);
  return dt_stab;
}

int Op_Diff_DG_base::impr(Sortie& os) const
{
  return 1;
}

void Op_Diff_DG_base::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& zcl, const Champ_Inc_base&)
{
  le_dom_dg_ = ref_cast(Domaine_DG, domaine_dis);
  la_zcl_dg_ = ref_cast(Domaine_Cl_DG, zcl);
}

DoubleTab& Op_Diff_DG_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(inco, resu);
}

void Op_Diff_DG_base::completer()
{
  Operateur_base::completer();
  nu_.resize(0, equation().que_suis_je() == "Transport_K_Eps" ? 2 : diffusivite().valeurs().line_size());
  le_dom_dg_->domaine().creer_tableau_elements(nu_);
  nu_a_jour_ = 0;
}

void Op_Diff_DG_base::update_nu() const
{
  if (nu_a_jour_) return; // on a deja fait le travail

  int i, j;

  const DoubleTab& diffu = diffusivite().valeurs();
  if (equation().que_suis_je() != "Transport_K_Eps")
    {
      if (!diffu.get_md_vector().non_nul())
        {
          // diffusivite uniforme
          int n = nu_.dimension_tot(0), nb_comp = nu_.line_size();
          // Tableaux vus comme uni-dimenionnels:
          const DoubleVect& arr_diffu = diffu;
          DoubleVect& arr_nu = nu_;
          for (i = 0; i < n; i++)
            for (j = 0; j < nb_comp; j++)
              arr_nu[i * nb_comp + j] = arr_diffu[j];
        }
      else
        {
          assert(nu_.get_md_vector() == diffu.get_md_vector());
          assert_espace_virtuel_vect(diffu);
          nu_.inject_array(diffu);
        }
    }

  /* ajout de la diffusivite turbulente si elle existe */
  if (has_diffusivite_turbulente())
    {
      throw;

      const DoubleTab& diffu_turb = diffusivite_turbulente().valeurs();
      if (equation().que_suis_je() == "Transport_K_Eps")
        {
          bool nu_uniforme = sub_type(Champ_Uniforme, diffusivite());
          double val_nu = diffu(0, 0);
          for (i = 0; i < diffu_turb.dimension(0); i++)
            for (j = 0; j < 2; j++)
              {
                if (!nu_uniforme)
                  val_nu = diffu(i, 0);
                nu_(i, j) = val_nu + diffu_turb(i, j);
              }
        }
      else
        {
          if (!diffu_turb.get_md_vector().non_nul())
            {
              // diffusvite uniforme
              int n = nu_.dimension_tot(0), nb_comp = nu_.line_size();
              // Tableaux vus comme uni-dimenionnels:
              const DoubleVect& arr_diffu_turb = diffu_turb;
              DoubleVect& arr_nu = nu_;
              for (i = 0; i < n; i++)
                for (j = 0; j < nb_comp; j++)
                  arr_nu[i * nb_comp + j] += arr_diffu_turb[j];
            }
          else
            {
              assert(nu_.get_md_vector() == diffu_turb.get_md_vector());
              assert_espace_virtuel_vect(diffu_turb);
              nu_ += diffu_turb;
            }
        }
    }

  nu_a_jour_ = 1;
}
